#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <libgen.h>

#include "Config.h"
#include "Trigger.h"

#define PEVT_CURRENT_VERSION 3

#define PEVT_FHEAD_TAG 0x9
#define PEVT_TRIG_TAG  0x3
#define PEVT_FTAIL_TAG 0x5

// Return pid of locking process or 0 if no other DAQ is running
pid_t create_lock()
{
  FILE* lock_handle;
  pid_t pid;
  struct stat sb;

  if ( strcmp(Config->lock_file,"none")==0 ) {
    printf("PadmeTrig::create_lock - No lock file required\n");
    return 0;
  }

  // Check if lock file exists and return its pid
  if ( access(Config->lock_file,F_OK) != -1 ) {
    lock_handle = fopen(Config->lock_file,"r");
    if ( fscanf(lock_handle,"%d",&pid) == 0 ) {
      printf("PadmeTrig::create_lock - ERROR - Lock file '%s' found but could not read the PID\n",Config->lock_file);
      pid = -1;
    }
    fclose(lock_handle);
    return pid;
  }

  // Check if directory for lock file exists
  char* path = strdup(Config->lock_file);
  char* lock_dir = dirname(path); // N.B. dirname modifies its argument!
  if ( stat(lock_dir,&sb) == 0 && S_ISDIR(sb.st_mode) ) {

    // Create lock file and write own pid in it
    lock_handle = fopen(Config->lock_file,"w");
    fprintf(lock_handle,"%d",getpid());
    fclose(lock_handle);
    printf("PadmeTrig::create_lock - Lock file '%s' created for PID %d\n",Config->lock_file,getpid());
    return 0;

  }

  printf("PadmeTrig::create_lock - ERROR - Directory '%s' does not exist: cannot create lock file '%s'\n",lock_dir,Config->lock_file);
  return -1;

}

// Remove lock file (called just before exiting)
void remove_lock()
{
  struct stat sb;

  if (strcmp(Config->lock_file,"none")==0) return;

  // Check if lock file directory exists
  char* path = strdup(Config->lock_file);
  char* lock_dir = dirname(path); // N.B. dirname modifies its argument!
  if ( stat(lock_dir,&sb) == 0 && S_ISDIR(sb.st_mode) ) {
    if ( unlink(Config->lock_file) ) {
      printf("PadmeTrig::remove_lock - ERROR - Problem while removing lock file '%s'\n",Config->lock_file);
    } else {
      printf("PadmeTrig::remove_lock - Lock file '%s' removed\n",Config->lock_file);
    }
  } else {
    printf("PadmeTrig::remove_lock - ERROR - Directory '%s' does not exist: cannot remove lock file '%s'.\n",lock_dir,Config->lock_file);
  }

  return;

}

unsigned int create_file_head(unsigned int fIndex, int runNr, time_t timeTag, void *fHead)
{
  unsigned int line;
  line = (PEVT_FHEAD_TAG << 28) + (PEVT_CURRENT_VERSION << 16) + (fIndex & 0xFFFF);
  memcpy(fHead,&line,4);         // First line: file head tag (4) + version (12) + file index (16)
  memcpy(fHead+4,&runNr,4);      // Second line: run number (32)
  line = (timeTag & 0xffffffff);
  memcpy(fHead+12,&line,4);      // Third line: start of file time tag (32)
  return 12;                     // Return total size of file header in bytes
}

unsigned int create_file_tail(unsigned int nEvts, unsigned long int fSize, time_t timeTag, void *fTail)
{
  unsigned int line;
  unsigned long int fTotalSize = fSize + 16; // Add size of tail to total file size
  line = (PEVT_FTAIL_TAG << 28) + (nEvts & 0x0FFFFFFF);
  memcpy(fTail,&line,4);         // First line: file tail tag (4) + number of events (28)
  memcpy(fTail+4,&fTotalSize,8); // Second + third line: total file size (64)
  line = (timeTag & 0xffffffff);
  memcpy(fTail+12,&line,4);      // Fourth line: end of file time tag (32)
  return 16;                     // Return total size of file tail in bytes
}

// Handle failure in initialization phase
// If rmv_lock<>0 the lock file will also be removed
void init_fail(int rmv_lock)
{
  FILE* iff;
  struct stat sb;

  if (strcmp(Config->initfail_file,"none")!=0) {

    // Check if directory for initfail file exists
    char* path = strdup(Config->initfail_file);
    char* iff_dir = dirname(path); // N.B. dirname modifies its argument!
    if ( stat(iff_dir,&sb) == 0 && S_ISDIR(sb.st_mode) ) {
      if ( access(Config->initfail_file,F_OK) == -1 ) {
	iff = fopen(Config->initfail_file,"w");
	fclose(iff);
	printf("PadmeTrig::init_fail - InitFail file '%s' created\n",Config->initfail_file);
      } else {
	printf("PadmeTrig::init_fail - InitFail file '%s' already exists.\n",Config->initfail_file);
      }
    } else {
      printf("PadmeTirg::init_fail - Directory '%s' does not exist: cannot create InitFail file '%s'.\n",iff_dir,Config->initfail_file);
    }

  }

  if (rmv_lock) remove_lock();
  exit(1);

}

int main(int argc, char *argv[]) {

  pid_t pid;
  int c;
  char mask[4];

  // File to handle DAQ interaction with GUI
  FILE* iokf; // InitOK file

  // Input data information
  unsigned char data[100000]; // Need more info from Albicocco to fine tune this size. Using a VERY conservative value
  unsigned char buff[8];
  unsigned int data_len,writeSize;
  unsigned int numEvents;

  // Output event information
  char *outEvtBuffer = NULL;
  int maxPEvtSize, pEvtSize;
  unsigned int fHeadSize, fTailSize;

  // Global counters for input data
  unsigned long int totalReadSize;
  unsigned int totalReadEvents;
  float evtReadPerSec, sizeReadPerSec;

  // Global counters for output data
  unsigned long int totalWriteSize;
  unsigned int totalWriteEvents;
  float evtWritePerSec, sizeWritePerSec;

  // Information about output files
  unsigned int fileIndex;
  int tooManyOutputFiles;
  char tmpName[MAX_FILENAME_LEN];
  char* fileName[MAX_N_OUTPUT_FILES];
  char* pathName[MAX_N_OUTPUT_FILES];
  unsigned long int fileSize[MAX_N_OUTPUT_FILES];
  unsigned int fileEvents[MAX_N_OUTPUT_FILES];
  time_t fileTOpen[MAX_N_OUTPUT_FILES];
  time_t fileTClose[MAX_N_OUTPUT_FILES];
  int fileHandle;

  // Various timer variables
  time_t t_daqstart, t_daqstop, t_daqtotal;
  time_t t_now;

  unsigned int i,p;

  int trigger = -1; // Used to blindly set a trigger mask when -t option is specified

  // Use line buffering for stdout
  setlinebuf(stdout);

  // Show welcome message
  printf("===========================================\n");
  printf("=== Welcome to the PADME Trigger system ===\n");
  printf("===========================================\n");

  // Initialize run configuration
  if ( init_config() ) {
    printf("*** ERROR *** Problem initializing run configuration.\n");
    exit(1);
  }

  // Parse options
  while ((c = getopt (argc, argv, "t:c:h")) != -1)
    switch (c)
      {
      case 'c':
	// Check if another configuration file was specified
	if ( strcmp(Config->config_file,"")!=0 ) {
	  printf("*** ERROR *** Multiple configuration files specified: '%s' and '%s'.\n",Config->config_file,optarg);
	  exit(1);
	}
	// Read configuration parameters from configuration file
	if ( read_config(optarg) ) {
	  printf("*** ERROR *** Problem while reading configuration file '%s'.\n",optarg);
	  exit(1);
	}
        break;
      case 't':
	trigger = atoi(optarg);
	break;
      case 'h':
	fprintf(stdout,"\nPadmeTrig [-c cfg_file] [-t mask] [-h]\n\n");
	fprintf(stdout,"  -c: use file 'cfg_file' to set configuration parameters for this process\n");
	fprintf(stdout,"     If no file is specified, use default settings\n");
	fprintf(stdout,"  -t: set trigger mask and exit (default: mask = 0x00\n");
	fprintf(stdout,"  -h: show this help message and exit\n\n");
	exit(0);
      case '?':
        if (optopt == 'c')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	else if (optopt == 't')
	  trigger = 0;
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option '-%c'.\n", optopt);
        else
          fprintf (stderr,"Unknown option character '\\x%x'.\n",optopt);
        exit(1);
      default:
        abort ();
      }

  // If a trigger mask was explicitily specified, just set it and exit
  if (trigger != -1) {

    if ( trig_get_trigbusymask(mask) != TRIG_OK ) {
      printf("PadmeTrig *** ERROR *** Problem while readying trigger and busy masks. Exiting.\n");
      exit(1);
    }
    printf("Current masks: trig 0x%02x busy 0x%02x dummy 0x%02x 0x%02x\n",mask[3],mask[2],mask[1],mask[0]);

    // Disable busy
    if ( trig_set_busymask(0) != TRIG_OK ) {
      printf("PadmeTrig *** ERROR *** Problem while resetting busy mask. Exiting.\n");
      exit(1);
    }
    printf("Busy disabled\n");

    // Get requested trigger mask and set it
    char trig = trigger & 0x3f;
    if ( trig_set_trigmask(trig) != TRIG_OK ) {
      printf("PadmeTrig *** ERROR *** Problem while setting trigger mask to 0x%02x. Exiting.\n",trig);
      exit(1);
    }
    printf("Trigger mask set to 0x%02x\n",trig);

    if ( trig_get_trigbusymask(mask) != TRIG_OK ) {
      printf("PadmeTrig *** ERROR *** Problem while readying trigger and busy masks. Exiting.\n");
      exit(1);
    }
    printf("Current masks: trig 0x%02x busy 0x%02x dummy 0x%02x 0x%02x\n",mask[3],mask[2],mask[1],mask[0]);

    // All is done: exit
    printf("\n=== May the force be with you. Bye! ===\n");
    exit(0);

  }

  // Show configuration
  print_config();

  // Check if another PadmeTrig program is running
  printf("\n=== Verifying that no other PadmeTrig instances are running ===\n");
  if ( (pid = create_lock()) ) {
    if (pid > 0) {
      printf("*** ERROR *** Another PadmeTrig is running with PID %d. Exiting.\n",pid);
    } else {
      printf("*** ERROR *** Problems while creating lock file '%s'. Exiting.\n",Config->lock_file);
    }
    init_fail(0);
  }

  // Allocate output event buffer (max among file header, trigger event, file tail)
  maxPEvtSize = 16; // Header 12, Event 12, Tail 16
  outEvtBuffer = (char *)malloc(maxPEvtSize);
  if (outEvtBuffer == NULL) {
    printf("Unable to allocate output event buffer of size %d\n",maxPEvtSize);
    return 1;
  }
  printf("- Allocated output event buffer with size %d\n",maxPEvtSize);

  // Connect to Trigger board
  printf("\n=== Connect to Trigger board ===\n");
  if ( trig_init() != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while connecting to Trigger board. Exiting.\n");
    init_fail(1);
  }

  // Show current masks
  if ( trig_get_trigbusymask(mask) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while readying trigger and busy masks. Exiting.\n");
    init_fail(1);
  }
  printf("Current masks: trig 0x%02x busy 0x%02x dummy 0x%02x 0x%02x\n",mask[3],mask[2],mask[1],mask[0]);

  // Disable all triggers
  if ( trig_set_trigmask(0) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while resetting trigger mask. Exiting.\n");
    init_fail(1);
  }

  // Disable busy
  if ( trig_set_busymask(0) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while resetting busy mask. Exiting.\n");
    init_fail(1);
  }

  // Drain trigger buffer
  while(data_len) {
    if ( trig_get_data((void*)data,&data_len) != TRIG_OK ) {
      printf("PadmeTrig *** ERROR *** Problem while cleaning trigger buffer. Exiting.\n");
      init_fail(1);
    }
  }

  // Show current masks
  if ( trig_get_trigbusymask(mask) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while reading trigger and busy masks. Exiting.\n");
    init_fail(1);
  }
  printf("Current masks: trig 0x%02x busy 0x%02x dummy 0x%02x 0x%02x\n",mask[3],mask[2],mask[1],mask[0]);

  // If quit file is already there, assume this is a test run and do nothing
  if ( access(Config->quit_file,F_OK) != -1 ) {
    printf("PadmeTrig - Quit file '%s' found: will not run acquisition\n",Config->quit_file);
    exit(0);
  }

  // Initialization is now finished: create InitOK file to tell RunControl we are ready.
  if (strcmp(Config->initok_file,"none")!=0) {
    printf("- Creating InitOK file '%s'\n",Config->initok_file);
    if ( access(Config->initok_file,F_OK) == -1 ) {
      iokf = fopen(Config->initok_file,"w");
      fclose(iokf);
      printf("- InitOK file '%s' created\n",Config->initok_file);
    } else {
      printf("- InitOK file '%s' already exists (?)\n",Config->initok_file);
      exit(1);
    }
  }

  // Now wait for Start file before enabling triggers
  while(1){
    if ( access(Config->start_file,F_OK) != -1 ) {
      printf("- Start file '%s' found: starting Triggers\n",Config->start_file);
      break;
    }
    if ( access(Config->quit_file,F_OK) != -1 ) {
      printf("- Quit file '%s' found: exiting\n",Config->quit_file);
      exit(0);
    }
    // Sleep for ~1ms before checking again
    usleep(1000);
  }

  time(&t_daqstart);
  printf("%s - Starting trigger generation\n",format_time(t_daqstart));

  // Zero counters
  totalReadSize = 0;
  totalReadEvents = 0;
  totalWriteSize = 0;
  totalWriteEvents = 0;

  // Start counting output files
  fileIndex = 0;
  tooManyOutputFiles = 0;

  if ( strcmp(Config->output_mode,"FILE")==0 ) {

    // Generate name for initial output file and verify it does not exist
    //generate_filename(fileName[fileIndex],t_daqstart);
    generate_filename(tmpName,t_daqstart);
    fileName[fileIndex] = (char*)malloc(strlen(tmpName)+1);
    strcpy(fileName[fileIndex],tmpName);
    pathName[fileIndex] = (char*)malloc(strlen(Config->data_dir)+strlen(fileName[fileIndex])+1);
    strcpy(pathName[fileIndex],Config->data_dir);
    strcat(pathName[fileIndex],fileName[fileIndex]);

  } else {

    // Use only one virtual file for streaming out all data
    pathName[fileIndex] = (char*)malloc(strlen(Config->output_stream)+1);
    strcpy(pathName[fileIndex],Config->output_stream);

  }

  // Open file
  if ( strcmp(Config->output_mode,"FILE")==0 ) {
    printf("- Opening output file %d with path '%s'\n",fileIndex,pathName[fileIndex]);
    fileHandle = open(pathName[fileIndex],O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
  } else {
    printf("- Opening output stream '%s'\n",pathName[fileIndex]);
    fileHandle = open(pathName[fileIndex],O_WRONLY);
  }
  if (fileHandle == -1) {
    printf("ERROR - Unable to open file '%s' for writing.\n",pathName[fileIndex]);
    exit(1);
  }
  fileTOpen[fileIndex] = t_daqstart;
  fileSize[fileIndex] = 0;
  fileEvents[fileIndex] = 0;

  // Write header to file
  fHeadSize = create_file_head(fileIndex,Config->run_number,fileTOpen[fileIndex],(void *)outEvtBuffer);
  writeSize = write(fileHandle,outEvtBuffer,fHeadSize);
  if (writeSize != fHeadSize) {
    printf("ERROR - Unable to write file header to file. Header size: %d, Write result: %d\n",
	   fHeadSize,writeSize);
    exit(1);
  }
  fileSize[fileIndex] += fHeadSize;
  totalWriteSize += fHeadSize;

  // Enable all triggers
  if ( trig_set_trigmask(Config->trigger_mask) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while resetting trigger mask. Exiting.\n");
    init_fail(1);
  }

  // Show current masks
  if ( trig_get_trigbusymask(mask) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while reading trigger and busy masks. Exiting.\n");
    init_fail(1);
  }
  printf("Current masks: trig 0x%02x busy 0x%02x dummy 0x%02x 0x%02x\n",mask[3],mask[2],mask[1],mask[0]);

  // Create trigger header (will be the same for all events)
  unsigned int header = ((PEVT_TRIG_TAG & 0xf) << 28) + ((3 & 0x0fffffff) << 0); // 4bits Trigger tag + 28bits Trigger size (4bytes words)

  // Defienn fixed event length
  pEvtSize = 12;

  // Run data acquisition
  while(1){

    if ( trig_get_data((void*)data,&data_len) != TRIG_OK ) {
      printf("PadmeTrig *** ERROR *** Problem while reading data. Exiting.\n");
      exit(1);
    }
    //printf("New packet of length %u bytes received\n",data_len);
    numEvents = data_len/8;

    // Update global counters
    totalReadSize += data_len;
    totalReadEvents += numEvents;

    //unsigned long int word;
    //unsigned long int trig_time;
    //unsigned int trig_map,trig_count;
    //unsigned long int old_time = 0;
    for(p=0;p<data_len;p+=8){
      //printf("0x%02x%02x%02x%02x%02x%02x%02x%02x\n",data[p+0],data[p+1],data[p+2],data[p+3],data[p+4],data[p+5],data[p+6],data[p+7]);
      for(i=0;i<8;i++) buff[i] = data[p+7-i]; // Reverse order of the 8 bytes: LSB arrives first, MSB arrives last
      //printf("0x%02x%02x%02x%02x%02x%02x%02x%02x\n",buff[0],buff[1],buff[2],buff[3],buff[4],buff[5],buff[6],buff[7]);

      // Create event structure and write it to file
      memcpy(outEvtBuffer,&header,4);
      memcpy(outEvtBuffer+4,buff,8);
      writeSize = write(fileHandle,outEvtBuffer,pEvtSize);
      if (writeSize != pEvtSize) {
	printf("ERROR - Unable to write data to output file. Data size: %d, Write result: %d\n",pEvtSize,writeSize);
	exit(1);
      }
	  
      // Update file counters
      fileSize[fileIndex] += pEvtSize;
      fileEvents[fileIndex]++;
	  
      // Update global counters
      totalWriteSize += pEvtSize;
      totalWriteEvents++;

      if (totalWriteEvents%100 == 0) {
	printf("- Trigger %u received\n",totalWriteEvents);
      }

      // Trigger debugging code
      //trig_time  =                (word & 0x000000FFFFFFFFFF);
      //trig_map   = (unsigned int)((word & 0x00003F0000000000) >> 40);
      //trig_count = (unsigned int)((word & 0x00FFC00000000000) >> 46);
      //float dt = (trig_time-old_time)*12.5E-6;
      //printf("0x%016lx %13lu 0x%02x %4u %f\n",word,trig_time,trig_map,trig_count,dt);
      //old_time = trig_time;

    }

    // Save current time
    time(&t_now);

    // If we are running in FILE output mode, check if we need a new data file
    // i.e. required time elapsed or file size/events threshold exceeded
    if ( strcmp(Config->output_mode,"FILE")==0 ) {

      if (
	  (t_now-fileTOpen[fileIndex] >= Config->file_max_duration) ||
	  (fileSize[fileIndex]        >= Config->file_max_size    ) ||
	  (fileEvents[fileIndex]      >= Config->file_max_events  )
	  ) {

	// Register file closing time
	fileTClose[fileIndex] = t_now;

	// Write tail to file
	fTailSize = create_file_tail(fileEvents[fileIndex],fileSize[fileIndex],fileTClose[fileIndex],(void *)outEvtBuffer);
	writeSize = write(fileHandle,outEvtBuffer,fTailSize);
	if (writeSize != fTailSize) {
	  printf("ERROR - Unable to write file tail to file. Tail size: %d, Write result: %d\n",
		 fTailSize,writeSize);
	  return 2;
	}
	fileSize[fileIndex] += fTailSize;
	totalWriteSize += fTailSize;

	// Close old output file and show some info about counters
	if (close(fileHandle) == -1) {
	  printf("ERROR - Unable to close output file '%s'.\n",fileName[fileIndex]);
	  return 2;
	};
	printf("%s - Closed output file '%s' after %d secs with %u events and size %lu bytes\n",
	       format_time(fileTClose[fileIndex]),pathName[fileIndex],
	       (int)(fileTClose[fileIndex]-fileTOpen[fileIndex]),
	       fileEvents[fileIndex],fileSize[fileIndex]);

	// Update file counter
	fileIndex++;

	if ( fileIndex<MAX_N_OUTPUT_FILES ) {

	  // Open new output file and reset all counters
	  generate_filename(tmpName,t_now);
	  fileName[fileIndex] = (char*)malloc(strlen(tmpName)+1);
	  strcpy(fileName[fileIndex],tmpName);
	  pathName[fileIndex] = (char*)malloc(strlen(Config->data_dir)+strlen(fileName[fileIndex])+1);
	  strcpy(pathName[fileIndex],Config->data_dir);
	  strcat(pathName[fileIndex],fileName[fileIndex]);
	  printf("- Opening output file %d with path '%s'\n",fileIndex,pathName[fileIndex]);
	  fileHandle = open(pathName[fileIndex],O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
	  if (fileHandle == -1) {
	    printf("ERROR - Unable to open file '%s' for writing.\n",pathName[fileIndex]);
	    exit(1);
	  }
	  fileTOpen[fileIndex] = t_now;
	  fileSize[fileIndex] = 0;
	  fileEvents[fileIndex] = 0;

	  // Write header to file
	  fHeadSize = create_file_head(fileIndex,Config->run_number,fileTOpen[fileIndex],(void *)outEvtBuffer);
	  writeSize = write(fileHandle,outEvtBuffer,fHeadSize);
	  if (writeSize != fHeadSize) {
	    printf("ERROR - Unable to write file header to file. Header size: %d, Write result: %d\n",
		   fHeadSize,writeSize);
	    exit(1);
	  }
	  fileSize[fileIndex] += fHeadSize;
	  totalWriteSize += fHeadSize;

	} else {

	  tooManyOutputFiles = 1;

	}

      }

    }

    // Check if it is time to stop Triggers (quit file, time elapsed)
    if (
	 tooManyOutputFiles ||
	 (access(Config->quit_file,F_OK) != -1) ||
	 ( Config->total_daq_time && ( t_now-t_daqstart >= Config->total_daq_time ) )
       ) break;

    // Sleep for a while before continuing
    usleep(Config->daq_loop_delay);

  }

  // Tell user what stopped DAQ
  if ( tooManyOutputFiles ) printf("=== Stopping Trigger after writing %d data files ===\n",fileIndex);
  if ( access(Config->quit_file,F_OK) != -1 )
    printf("=== Stopping Trigger on quit file '%s' ===\n",Config->quit_file);
  if ( Config->total_daq_time && ( t_now-t_daqstart >= Config->total_daq_time ) )
    printf("=== Stopping Trigger after %d secs of run (requested %d) ===\n",(int)(t_now-t_daqstart),Config->total_daq_time);

  // If DAQ was stopped for writing too many output files, we do not have to close the last file
  if ( ! tooManyOutputFiles ) {

    // Register file closing time
    fileTClose[fileIndex] = t_now;

    // Write tail to file
    fTailSize = create_file_tail(fileEvents[fileIndex],fileSize[fileIndex],fileTClose[fileIndex],(void *)outEvtBuffer);
    writeSize = write(fileHandle,outEvtBuffer,fTailSize);
    if (writeSize != fTailSize) {
      printf("ERROR - Unable to write file tail to file. Tail size: %d, Write result: %d\n",
	     fTailSize,writeSize);
      return 2;
    }
    fileSize[fileIndex] += fTailSize;
    totalWriteSize += fTailSize;

    // Close output file and show some info about counters
    if (close(fileHandle) == -1) {
      printf("ERROR - Unable to close output file '%s'.\n",fileName[fileIndex]);
      return 2;
    };
    if ( strcmp(Config->output_mode,"FILE")==0 ) {
      printf("%s - Closed output file '%s' after %d secs with %u events and size %lu bytes\n",
	     format_time(t_now),pathName[fileIndex],(int)(fileTClose[fileIndex]-fileTOpen[fileIndex]),
	     fileEvents[fileIndex],fileSize[fileIndex]);
    } else {
      printf("%s - Closed output stream '%s' after %d secs with %u events and size %lu bytes\n",
	     format_time(t_now),pathName[fileIndex],(int)(fileTClose[fileIndex]-fileTOpen[fileIndex]),
	     fileEvents[fileIndex],fileSize[fileIndex]);
    }

    // Update file counter
    fileIndex++;

  }

  // Disable all triggers
  if ( trig_set_trigmask(0) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while resetting trigger mask. Exiting.\n");
    exit(1);
  }

  // Drain trigger buffer
  while(data_len) {
    if ( trig_get_data((void*)data,&data_len) != TRIG_OK ) {
      printf("PadmeTrig *** ERROR *** Problem while finalizing data readout. Exiting.\n");
      exit(1);
    }
  }

  time(&t_daqstop);
  printf("%s - Acquisition stopped\n",format_time(t_daqstop));

  if ( trig_end() != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while disconnecting from trigger board. Exiting.\n");
    exit(1);
  }

  // Give some final report
  evtReadPerSec = 0.;
  sizeReadPerSec = 0.;
  evtWritePerSec = 0.;
  sizeWritePerSec = 0.;
  t_daqtotal = t_daqstop-t_daqstart;
  if (t_daqtotal>0) {
    evtReadPerSec = 1.*totalReadEvents/t_daqtotal;
    sizeReadPerSec = totalReadSize/(t_daqtotal*1024.);
    evtWritePerSec = 1.*totalWriteEvents/t_daqtotal;
    sizeWritePerSec = totalWriteSize/(t_daqtotal*1024.);
  }
  printf("\n=== DAQ ending on %s ===\n",format_time(t_daqstop));
  printf("Total running time: %d secs\n",(int)t_daqtotal);
  printf("Total number of events acquired: %u - %6.2f events/s\n",totalReadEvents,evtReadPerSec);
  printf("Total size of data acquired: %lu B - %6.2f KB/s\n",totalReadSize,sizeReadPerSec);
  printf("Total number of events written: %u - %6.2f events/s\n",totalWriteEvents,evtWritePerSec);
  printf("Total size of data written: %lu B - %6.2f KB/s\n",totalWriteSize,sizeWritePerSec);
  if ( strcmp(Config->output_mode,"FILE")==0 ) {
    printf("=== Files created =======================================\n");
    for(i=0;i<fileIndex;i++) {
      printf("'%s' %u %lu",fileName[i],fileEvents[i],fileSize[i]);
      printf(" %s",format_time(fileTOpen[i])); // Optimizer effect! :)
      printf(" %s\n",format_time(fileTClose[i]));
    }
  }
  printf("=========================================================\n");

  // Deallocate output event buffer
  free(outEvtBuffer);

  // Free space allocated for file names
  for(i=0;i<fileIndex;i++) free(fileName[i]);

  // Remove lock file
  remove_lock();

  // Clean up configuration
  end_config();

  // All is done: exit
  printf("\n=== May the force be with you. Bye! ===\n");
  exit(0);

}
