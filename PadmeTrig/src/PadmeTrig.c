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

#include "DB.h"
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

  // Check if directory for lock file exists
  char* path = strdup(Config->lock_file);
  char* lock_dir = dirname(path); // N.B. dirname modifies its argument!
  if ( stat(lock_dir,&sb) != 0 || (! S_ISDIR(sb.st_mode)) ) {
    printf("PadmeTrig::create_lock - ERROR - Directory '%s' does not exist: cannot create lock file '%s'\n",lock_dir,Config->lock_file);
    return -1;
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

  // Create lock file and write own pid in it
  lock_handle = fopen(Config->lock_file,"w");
  fprintf(lock_handle,"%d",getpid());
  fclose(lock_handle);
  printf("PadmeTrig::create_lock - Lock file '%s' created for PID %d\n",Config->lock_file,getpid());
  return 0;

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

int create_initok_file()
{

  FILE* iff;
  struct stat sb;

  if (strcmp(Config->initok_file,"none")==0) return 0;

  // Check if directory for initok file exists
  char* path = strdup(Config->initok_file);
  char* iff_dir = dirname(path); // N.B. dirname modifies its argument!
  if ( stat(iff_dir,&sb) != 0 || (! S_ISDIR(sb.st_mode)) ) {
    printf("PadmeTrig::create_initok_file - Directory '%s' does not exist: cannot create InitOK file '%s'.\n",iff_dir,Config->initok_file);
    return 1;
  }

  // Check if file is already there (not cleaned in previous run?)
  if ( access(Config->initok_file,F_OK) != -1 ) {
    printf("PadmeTrig::create_initok_file - InitOK file '%s' already exists.\n",Config->initok_file);
    return 1;
  }

  // Create InitOK file
  iff = fopen(Config->initok_file,"w");
  fclose(iff);
  printf("- InitOK file '%s' created\n",Config->initok_file);
  return 0;

}

int create_initfail_file()
{

  FILE* iff;
  struct stat sb;

  if (strcmp(Config->initfail_file,"none")==0) return 0;

  // Check if directory for initfail file exists
  char* path = strdup(Config->initfail_file);
  char* iff_dir = dirname(path); // N.B. dirname modifies its argument!
  if ( stat(iff_dir,&sb) != 0 || (! S_ISDIR(sb.st_mode)) ) {
    printf("PadmeTrig::create_initfail_file - Directory '%s' does not exist: cannot create InitFail file '%s'.\n",iff_dir,Config->initfail_file);
    return 1;
  }

  // Check if file is already there (not cleaned in previous run?)
  if ( access(Config->initfail_file,F_OK) != -1 ) {
    printf("PadmeTrig::create_initfail_file - InitFail file '%s' already exists.\n",Config->initfail_file);
    return 1;
  }

  // Create InitFail file
  iff = fopen(Config->initfail_file,"w");
  fclose(iff);
  printf("- InitFail file '%s' created\n",Config->initfail_file);
  return 0;

}

void proc_finalize(int error,int rmv_lock,int create_file,int update_db,int status)
{
  if (create_file) create_initfail_file();
  if (status == DB_STATUS_IDLE) {
    printf("- Setting process status to IDLE (%d)\n",DB_STATUS_IDLE);
  } else if (status == DB_STATUS_INITIALIZING) {
    printf("- Setting process status to INITIALIZING (%d)\n",DB_STATUS_INITIALIZING);
  } else if (status == DB_STATUS_INIT_FAIL) {
    printf("- Setting process status to INIT_FAIL (%d)\n",DB_STATUS_INIT_FAIL);
  } else if (status == DB_STATUS_INITIALIZED) {
    printf("- Setting process status to INITIALIZED (%d)\n",DB_STATUS_INITIALIZED);
  } else if (status == DB_STATUS_ABORTED) {
    printf("- Setting process status to ABORTED (%d)\n",DB_STATUS_ABORTED);
  } else if (status == DB_STATUS_RUNNING) {
    printf("- Setting process status to RUNNING (%d)\n",DB_STATUS_RUNNING);
  } else if (status == DB_STATUS_RUN_FAIL) {
    printf("- Setting process status to RUN_FAIL (%d)\n",DB_STATUS_RUN_FAIL);
  } else if (status == DB_STATUS_FINISHED) {
    printf("- Setting process status to FINISHED (%d)\n",DB_STATUS_FINISHED);
  } else if (status == DB_STATUS_CLOSE_FAIL) {
    printf("- Setting process status to CLOSE_FAIL (%d)\n",DB_STATUS_CLOSE_FAIL);
  } else {
    printf("- Setting process status to UNKNOWN (%d)\n",DB_STATUS_UNKNOWN);
    status = DB_STATUS_UNKNOWN;
  }
  printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),status);
  if (rmv_lock) remove_lock();
  if (error) exit(1);
  exit(0);
}

void timespec_diff(const struct timespec *start, const struct timespec *stop,
                   struct timespec *result)
{
    if ((stop->tv_nsec - start->tv_nsec) < 0) {
        result->tv_sec = stop->tv_sec - start->tv_sec - 1;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
    } else {
        result->tv_sec = stop->tv_sec - start->tv_sec;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }
    return;
}

void show_register(unsigned char reg)
{
  unsigned char mask[4];
  if ( trig_get_register(reg,mask) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while readying register 0x%02x. Exiting.\n",reg);
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  printf("Current register 0x%02x: 0x%02x%02x%02x%02x\n",reg,mask[0],mask[1],mask[2],mask[3]);
}

int main(int argc, char *argv[]) {

  pid_t pid;
  int c;
  //int rc;
  //unsigned char mask[4];
  unsigned char val1;
  unsigned short int val2;
  //unsigned int val4;
  //unsigned long int val8;
  unsigned char trig,reg;

  // File to handle DAQ interaction with GUI
  //FILE* iokf; // InitOK file

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
  unsigned int fileIndex = 0;
  int tooManyOutputFiles;
  char tmpName[MAX_FILENAME_LEN];
  char* fileName[MAX_N_OUTPUT_FILES];
  char* pathName[MAX_N_OUTPUT_FILES];
  unsigned long int fileSize[MAX_N_OUTPUT_FILES];
  unsigned int fileEvents[MAX_N_OUTPUT_FILES];
  time_t fileTOpen[MAX_N_OUTPUT_FILES];
  time_t fileTClose[MAX_N_OUTPUT_FILES];
  int fileHandle = 0;

  // Trigger debugging/logging variables
  unsigned long int word;
  unsigned long int trig_time;
  unsigned char trig_map,trig_count,trig_fifo,trig_auto;
  unsigned long int diff_time;
  unsigned long int old_time = 0;
  //time_t sys_time, old_sys_time;
  struct timespec clock_time, old_clock_time,clock_diff;
  unsigned int trig_stat[256];
  unsigned int trig_stat_old[256];
  unsigned int trig_cnts[8];
  unsigned int trig_diff[8];

  // Various timer variables
  time_t t_daqstart, t_daqstop, t_daqtotal;
  time_t t_now;

  unsigned int i,j,p;

  // Reset trigger statistics
  for (i=0;i<256;i++) { trig_stat[i] = 0; trig_stat_old[i] = 0; }

  // Use line buffering for stdout
  setlinebuf(stdout);

  // Initialize run configuration
  if ( init_config() ) {
    printf("*** ERROR *** Problem initializing run configuration.\n");
    exit(1);
  }

  // Parse options
  while ((c = getopt (argc, argv, "c:h")) != -1)
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
      //case 't':
      //  trigger = atoi(optarg);
      //  break;
      case 'h':
	fprintf(stdout,"\nPadmeTrig [-c cfg_file] [-h]\n\n");
	fprintf(stdout,"  -c: use file 'cfg_file' to set configuration parameters for this process\n");
	fprintf(stdout,"     If no file is specified, use default settings\n");
	fprintf(stdout,"  -h: show this help message and exit\n\n");
	exit(0);
      case '?':
        if (optopt == 'c')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option '-%c'.\n", optopt);
        else
          fprintf (stderr,"Unknown option character '\\x%x'.\n",optopt);
        exit(1);
      default:
        abort ();
      }

  // Show welcome message
  printf("===========================================\n");
  printf("=== Welcome to the PADME Trigger system ===\n");
  printf("===========================================\n");

  // Show configuration
  print_config();

  // Save configuration to DB
  //save_config();

  // Check if another PadmeTrig program is running
  printf("\n=== Verifying that no other PadmeTrig instances are running ===\n");
  pid = create_lock();
  if (pid > 0) {
    printf("*** ERROR *** Another PadmeTrig is running with PID %d. Exiting.\n",pid);
    proc_finalize(1,0,1,0,DB_STATUS_INIT_FAIL);
  } else if (pid < 0) {
    printf("*** ERROR *** Problems while creating lock file '%s'. Exiting.\n",Config->lock_file);
    proc_finalize(1,0,1,0,DB_STATUS_INIT_FAIL);
  }

  // Update process status
  printf("- Setting process status to INITIALIZING (%d)\n",DB_STATUS_INITIALIZING);
  printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_INITIALIZING);

  // Allocate output event buffer (max among file header, trigger event, file tail)
  maxPEvtSize = 16; // Header 12, Event 12, Tail 16
  outEvtBuffer = (char *)malloc(maxPEvtSize);
  if (outEvtBuffer == NULL) {
    printf("Unable to allocate output event buffer of size %d\n",maxPEvtSize);
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  printf("- Allocated output event buffer with size %d\n",maxPEvtSize);

  // Connect to Trigger board
  printf("\n=== Connect to Trigger board at address %s port %u ===\n",Config->trigger_addr,Config->trigger_port);
  if ( trig_init(Config->trigger_addr,Config->trigger_port) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while connecting to Trigger board. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }

  // Show registers before configuring board
  for (reg=0x00;reg<0x10;reg++) {
    //if ( trig_get_register(reg,mask) != TRIG_OK ) {
    //  printf("PadmeTrig *** ERROR *** Problem while readying register 0x%02x. Exiting.\n",reg);
    //  proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
    //}
    //printf("Current register 0x%02x: 0x%02x%02x%02x%02x\n",reg,mask[0],mask[1],mask[2],mask[3]);
    show_register(reg);
  }
  show_register(0x1D);

  // Show firmware version
  unsigned int fw_ver;
  if ( trig_get_fw_version(&fw_ver) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while reading firmware version. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  printf("Firmware version: 0x%04x (%d)\n",fw_ver,fw_ver);

  // Program Trigger module with current configuration

  // Trigger mask
  if ( trig_get_trigmask(&val1) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while reading trigger mask. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  printf("Trigger mask: 0x%02x\n",val1);

  // Busy mask
  if ( trig_get_busymask(&val1) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while reading busy mask. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  printf("Busy mask: 0x%02x\n",val1);

  // Correlated trigger delay
  if ( trig_get_correlated_delay(&val2) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while reading correlated delay. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  printf("Correlated delay: 0x%04x %u\n",val2,val2);
  if ( trig_set_correlated_delay(Config->correlated_trigger_delay) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while setting correlated delay. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  if ( trig_get_correlated_delay(&val2) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while reading correlated delay. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  printf("Correlated delay: 0x%04x %u\n",val2,val2);

  // Timepix shutter delay
  if ( trig_get_timepix_delay(&val1) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while reading timepix delay. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  printf("Timepix shutter delay: 0x%02x %u\n",val1,val1);
  if ( trig_set_timepix_delay(Config->timepix_shutter_delay) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while setting timepix delay. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  if ( trig_get_timepix_delay(&val1) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while reading timepix delay. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  printf("Timepix shutter delay: 0x%02x %u\n",val1,val1);

  // Timepix shutter width
  if ( trig_get_timepix_width(&val1) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while reading timepix width. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  printf("Timepix shutter width: 0x%02x %u\n",val1,val1);
  if ( trig_set_timepix_width(Config->timepix_shutter_width) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while setting timepix width. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  if ( trig_get_timepix_width(&val1) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while reading timepix width. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  printf("Timepix shutter width: 0x%02x %u\n",val1,val1);

  // Trigger 0 distribution delay
  if ( trig_get_trigger0_delay(&val1) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while reading trigger 0 distribution delay. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  printf("Trigger 0 delay: 0x%02x %u\n",val1,val1);
  if ( trig_set_trigger0_delay(Config->trigger0_delay) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while setting trigger 0 distribution delay. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  if ( trig_get_trigger0_delay(&val1) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while reading trigger 0 distribution delay. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  printf("Trigger 0 delay: 0x%02x %u\n",val1,val1);

  for (trig = 0; trig<8; trig++) {

    unsigned short int tsg,tsa;
    switch(trig){
      //case 0: tsg = Config->trig0_scale_global; tsa = Config->trig0_scale_autopass; break;
      case 0:                                   tsa = Config->trig0_scale_autopass; break;
      case 1: tsg = Config->trig1_scale_global; tsa = Config->trig1_scale_autopass; break;
      case 2: tsg = Config->trig2_scale_global; tsa = Config->trig2_scale_autopass; break;
      case 3: tsg = Config->trig3_scale_global; tsa = Config->trig3_scale_autopass; break;
      case 4: tsg = Config->trig4_scale_global; tsa = Config->trig4_scale_autopass; break;
      case 5: tsg = Config->trig5_scale_global; tsa = Config->trig5_scale_autopass; break;
      case 6: tsg = Config->trig6_scale_global; tsa = Config->trig6_scale_autopass; break;
      case 7: tsg = Config->trig7_scale_global; tsa = Config->trig7_scale_autopass; break;
      default: printf("WHAT?\n");
    }

    if (trig != 0) {
      if ( trig_get_trigger_global_factor(trig,&val2) != TRIG_OK ) {
	printf("PadmeTrig *** ERROR *** Problem while reading trigger 0x%02x global factor. Exiting.\n",trig);
	proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
      }
      printf("Trigger 0x%02x global downscale factor old: 0x%04x %u\n",trig,val2,val2);
      if ( trig_set_trigger_global_factor(trig,tsg) != TRIG_OK ) {
	printf("PadmeTrig *** ERROR *** Problem while setting trigger 0x%02x global factor. Exiting.\n",trig);
	proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
      }
      if ( trig_get_trigger_global_factor(trig,&val2) != TRIG_OK ) {
	printf("PadmeTrig *** ERROR *** Problem while reading trigger 0x%02x global factor. Exiting.\n",trig);
	proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
      }
      printf("Trigger 0x%02x global downscale factor new: 0x%04x %u\n",trig,val2,val2);
    }

    if ( trig_get_trigger_autopass_factor(trig,&val2) != TRIG_OK ) {
      printf("PadmeTrig *** ERROR *** Problem while reading trigger 0x%02x autopass factor. Exiting.\n",trig);
      proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
    }
    printf("Trigger 0x%02x autopass downscale factor old: 0x%04x %u\n",trig,val2,val2);
    if ( trig_set_trigger_autopass_factor(trig,tsa) != TRIG_OK ) {
      printf("PadmeTrig *** ERROR *** Problem while setting trigger 0x%02x autopass factor. Exiting.\n",trig);
      proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
    }
    if ( trig_get_trigger_autopass_factor(trig,&val2) != TRIG_OK ) {
      printf("PadmeTrig *** ERROR *** Problem while reading trigger 0x%02x autopass factor. Exiting.\n",trig);
      proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
    }
    printf("Trigger 0x%02x autopass downscale factor new: 0x%04x %u\n",trig,val2,val2);

  }

  // Disable all triggers
  printf("- Disabling all triggers.\n");
  if ( trig_set_trigmask(0x00) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while resetting trigger mask. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  if ( trig_get_trigmask(&val1) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while reading trigger mask. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  printf("Current trigger mask: 0x%02x\n",val1);

  // Disable all (non-CPU) busy
  //printf("- Disabling all busy.\n");
  //if ( trig_set_busymask(0x10) != TRIG_OK ) {
  // Set busy mask
  printf("- Setting busy mask to 0x%02x.\n",Config->busy_mask);
  if ( trig_set_busymask(Config->busy_mask) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while setting busy mask. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  if ( trig_get_busymask(&val1) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while reading busy mask. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  printf("Current busy mask: 0x%02x\n",val1);

  // Drain trigger buffer
  data_len = 1; // Make sure trig_get_data is executed at least once
  printf("- Draining trigger buffer\n");
  while(data_len) {
    if ( trig_get_data((void*)data,&data_len) != TRIG_OK ) {
      printf("PadmeTrig *** ERROR *** Problem while cleaning trigger buffer. Exiting.\n");
      proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
    }
    printf("- Draining trigger buffer - data_len = %d\n",data_len);
  }

  // Show registers after initializing board
  for (reg=0x00;reg<0x10;reg++) {
    //if ( trig_get_register(reg,mask) != TRIG_OK ) {
    //  printf("PadmeTrig *** ERROR *** Problem while readying register 0x%02x. Exiting.\n",reg);
    //  proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
    //}
    //printf("Current register 0x%02x: 0x%02x%02x%02x%02x\n",reg,mask[0],mask[1],mask[2],mask[3]);
    show_register(reg);
  }
  show_register(0x1D);

  // If using STREAM output, open stream now to avoid DAQ locking
  if ( strcmp(Config->output_mode,"STREAM")==0 ) {

    pathName[fileIndex] = (char*)malloc(strlen(Config->output_stream)+1);
    strcpy(pathName[fileIndex],Config->output_stream);

    printf("- Opening output stream '%s'\n",pathName[fileIndex]);
    fileHandle = open(pathName[fileIndex],O_WRONLY);
    if (fileHandle == -1) {
      printf("PadmeTrig *** ERROR *** Unable to open file '%s' for writing.\n",pathName[fileIndex]);
      proc_finalize(1,1,0,1,DB_STATUS_RUN_FAIL);
    }

  }

  // Initialization is now finished: create InitOK file to tell RunControl we are ready.
  if ( create_initok_file() ) {
    printf("PadmeTrig *** ERROR *** Problem while creating InitOK file. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }

  printf("- Setting process status to INITIALIZED (%d)\n",DB_STATUS_INITIALIZED);
  printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_INITIALIZED);

  // Now wait for Start file before enabling triggers
  printf("- Trigger board initialized: waiting for start_file '%s'\n",Config->start_file);
  while(1){
    if ( access(Config->quit_file,F_OK) != -1 ) {
      printf("- Quit file '%s' found: exiting\n",Config->quit_file);
      proc_finalize(0,1,0,1,DB_STATUS_ABORTED);
    }
    if ( access(Config->start_file,F_OK) != -1 ) {
      printf("- Start file '%s' found: starting Triggers\n",Config->start_file);
      break;
    }
    // Sleep for ~1ms before checking again
    usleep(1000);
  }

  // Start run: enable SIN
  printf("- Sending start_run command to trigger board\n");
  if ( trig_start_run() != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while starting the run. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }

  time(&t_daqstart);
  printf("%s - Starting trigger generation\n",format_time(t_daqstart));
  //old_sys_time = t_daqstart;
  
  printf("- Setting process status to RUNNING (%d)\n",DB_STATUS_RUNNING);
  printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_RUNNING);
  printf("DBINFO - %s - process_set_time_start %s\n",format_time(time(0)),format_time(t_daqstart));

  // Zero counters
  totalReadSize = 0;
  totalReadEvents = 0;
  totalWriteSize = 0;
  totalWriteEvents = 0;

  // Initialize file counters
  fileTOpen[fileIndex] = t_daqstart;
  fileSize[fileIndex] = 0;
  fileEvents[fileIndex] = 0;

  // When using FILE output, only open file when DAQ has started
  if ( strcmp(Config->output_mode,"FILE")==0 ) {

    // Generate name for initial output file and verify it does not exist
    //generate_filename(fileName[fileIndex],t_daqstart);
    generate_filename(tmpName,t_daqstart);
    fileName[fileIndex] = (char*)malloc(strlen(tmpName)+1);
    strcpy(fileName[fileIndex],tmpName);
    pathName[fileIndex] = (char*)malloc(strlen(Config->data_dir)+strlen(fileName[fileIndex])+1);
    strcpy(pathName[fileIndex],Config->data_dir);
    strcat(pathName[fileIndex],fileName[fileIndex]);

    printf("- Opening output file %d with path '%s'\n",fileIndex,pathName[fileIndex]);
    fileHandle = open(pathName[fileIndex],O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (fileHandle == -1) {
      printf("PadmeTrig *** ERROR *** Unable to open file '%s' for writing.\n",pathName[fileIndex]);
      proc_finalize(1,1,0,1,DB_STATUS_RUN_FAIL);
    }

    printf("DBINFO - %s - file_create %s %s %d %d\n",format_time(time(0)),fileName[fileIndex],"TRIGDATA",PEVT_CURRENT_VERSION,fileIndex);
    printf("DBINFO - %s - file_set_time_open %s %s\n",format_time(time(0)),fileName[fileIndex],format_time(fileTOpen[fileIndex]));

  }
 
  /*
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
    printf("PadmeTrig *** ERROR *** Unable to open file '%s' for writing.\n",pathName[fileIndex]);
    proc_finalize(1,1,0,1,DB_STATUS_RUN_FAIL);
  }
  fileTOpen[fileIndex] = t_daqstart;
  fileSize[fileIndex] = 0;
  fileEvents[fileIndex] = 0;
  */

  // Write header to file
  fHeadSize = create_file_head(fileIndex,Config->run_number,fileTOpen[fileIndex],(void *)outEvtBuffer);
  writeSize = write(fileHandle,outEvtBuffer,fHeadSize);
  if (writeSize != fHeadSize) {
    printf("*** ERROR *** Unable to write file header to file. Header size: %d, Write result: %d\n",
	   fHeadSize,writeSize);
    proc_finalize(1,1,0,1,DB_STATUS_RUN_FAIL);
  }
  fileSize[fileIndex] += fHeadSize;
  totalWriteSize += fHeadSize;

  // Enable all triggers
  printf("- Enabling requested triggers.\n");
  if ( trig_set_trigmask(Config->trigger_mask) != TRIG_OK ) {
    printf("*** ERROR *** Problem while resetting trigger mask. Exiting.\n");
    proc_finalize(1,1,0,1,DB_STATUS_RUN_FAIL);
  }
  if ( trig_get_trigmask(&val1) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while reading trigger mask. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }
  printf("Current trigger mask: 0x%02x\n",val1);

  // Create trigger header (will be the same for all events)
  unsigned int header = ((PEVT_TRIG_TAG & 0xf) << 28) + ((3 & 0x0fffffff) << 0); // 4bits Trigger tag + 28bits Trigger size (4bytes words)

  // Define fixed event length
  pEvtSize = 12;

  // Run data acquisition
  tooManyOutputFiles = 0;
  while(1){

    if ( trig_get_data((void*)data,&data_len) != TRIG_OK ) {
      printf("*** ERROR *** Problem while reading data. Exiting.\n");
      proc_finalize(1,1,0,1,DB_STATUS_RUN_FAIL);
    }
    //printf("New packet of length %u bytes received\n",data_len);
    numEvents = data_len/8;

    // Update global counters
    totalReadSize += data_len;
    totalReadEvents += numEvents;

    for(p=0;p<data_len;p+=8){
      //printf("0x%02x%02x%02x%02x%02x%02x%02x%02x\n",data[p+0],data[p+1],data[p+2],data[p+3],data[p+4],data[p+5],data[p+6],data[p+7]);
      for(i=0;i<8;i++) buff[i] = data[p+7-i]; // Reverse order of the 8 bytes: LSB arrives first, MSB arrives last
      //printf("0x%02x%02x%02x%02x%02x%02x%02x%02x\n",buff[0],buff[1],buff[2],buff[3],buff[4],buff[5],buff[6],buff[7]);

      // Create event structure and write it to file
      memcpy(outEvtBuffer,&header,4);
      memcpy(outEvtBuffer+4,buff,8);
      writeSize = write(fileHandle,outEvtBuffer,pEvtSize);
      if (writeSize != pEvtSize) {
	printf("*** ERROR *** Unable to write data to output file. Data size: %d, Write result: %d\n",pEvtSize,writeSize);
	proc_finalize(1,1,0,1,DB_STATUS_RUN_FAIL);
      }

      // Get trigger mask for statistics
      memcpy(&word,buff,8);
      trig_stat[(unsigned char)((word & 0x0000FF0000000000) >> 40)]++;

      // Write trigger info to stdout once every Config->debug_scale triggers
      if (totalWriteEvents%Config->debug_scale == 0) {

	//time(&sys_time);
	clock_gettime(CLOCK_REALTIME,&clock_time);
	//memcpy(&word,buff,8);
	/* Trigger data packet changed with latest firmware release
	trig_time  =                (word & 0x000000FFFFFFFFFF);
	trig_map   = (unsigned int)((word & 0x00003F0000000000) >> 40);
	trig_count = (unsigned int)((word & 0x00FFC00000000000) >> 46);
	*/
	trig_time  =                 (word & 0x000000FFFFFFFFFF);
	trig_map   = (unsigned char)((word & 0x0000FF0000000000) >> 40);
	trig_count = (unsigned char)((word & 0x00FF000000000000) >> 48);
	trig_fifo  = (unsigned char)((word & 0x0100000000000000) >> 56);
	trig_auto  = (unsigned char)((word & 0x0200000000000000) >> 57);
	if (totalWriteEvents == 0) {
	  printf("- Trigger %9u %#018lx %13lu %#04x %4u %1x %1x\n",totalWriteEvents,word,trig_time,trig_map,trig_count,trig_fifo,trig_auto);
	} else {

	  if (old_time <= trig_time) {
	    diff_time = trig_time-old_time;
	  } else {
	    // Take into account Trigger Board clock 40-bits rollover
	    diff_time = (trig_time+0x10000000000)-old_time;
	  }
	  float dt = diff_time/80.0E3; // Trigger clock is 80.0MHz: get time interval in ms
	  //int sys_dt = sys_time-old_sys_time;
	  //printf("- Trigger %9u %#018lx %13lu %#04x %4u %1x %1x %fms %ds\n",totalWriteEvents,word,trig_time,trig_map,trig_count,trig_fifo,trig_auto,dt,sys_dt);
	  timespec_diff(&old_clock_time,&clock_time,&clock_diff);
	  int dclock_ms = clock_diff.tv_sec*1000+clock_diff.tv_nsec/1000000;
	  int dclock_us = (clock_diff.tv_nsec%1000000)/1000;

	  printf("- Trigger %9u %#018lx %13lu %#04x %4u %1x %1x %11.3fms %7d.%3.3dms",totalWriteEvents,word,trig_time,trig_map,trig_count,trig_fifo,trig_auto,dt,dclock_ms,dclock_us);
	  if (dt > 0.) { printf(" %.2fHz",1000.*Config->debug_scale/dt); } else { printf(" 0.00Hz"); }
	  printf("\n");

	  for (j=0;j<8;j++) { trig_cnts[j] = 0; trig_diff[j] = 0; }
	  for (i=0;i<256;i++) {
	    if (trig_stat[i]) {
	      for (j=0;j<8;j++) {
		if (i & (1 << j)) {
		  trig_cnts[j] += trig_stat[i];
		  trig_diff[j] += (trig_stat[i]-trig_stat_old[i]);
		}
	      }
	    }
	  }
	  printf("- TrigMsk %ld",clock_time.tv_sec);
	  for (j=0;j<8;j++) {
	    if (trig_cnts[j]) {
	      if (dt > 0.) {
		printf(" %u(%u,%u,%.2fHz)",j,trig_cnts[j],trig_diff[j],1000.*trig_diff[j]/dt);
	      } else {
		printf(" %u(%u,%u,0.00Hz)",j,trig_cnts[j],trig_diff[j]);
	      }
	    }
	  }
	  printf("\n");

	}
	old_time = trig_time;
	//old_sys_time = sys_time;
	old_clock_time = clock_time;
	for (i=0;i<256;i++) { trig_stat_old[i] = trig_stat[i]; }
      }
	  
      // Update file counters
      fileSize[fileIndex] += pEvtSize;
      fileEvents[fileIndex]++;
	  
      // Update global counters
      totalWriteSize += pEvtSize;
      totalWriteEvents++;

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
	  printf("*** ERROR *** Unable to write file tail to file. Tail size: %d, Write result: %d\n",
		 fTailSize,writeSize);
	  proc_finalize(1,1,0,1,DB_STATUS_RUN_FAIL);
	}
	fileSize[fileIndex] += fTailSize;
	totalWriteSize += fTailSize;

	// Close old output file and show some info about counters
	if (close(fileHandle) == -1) {
	  printf("*** ERROR *** Unable to close output file '%s'.\n",fileName[fileIndex]);
	  proc_finalize(1,1,0,1,DB_STATUS_RUN_FAIL);
	};
	printf("%s - Closed output file '%s' after %d secs with %u events and size %lu bytes\n",
	       format_time(fileTClose[fileIndex]),pathName[fileIndex],
	       (int)(fileTClose[fileIndex]-fileTOpen[fileIndex]),
	       fileEvents[fileIndex],fileSize[fileIndex]);
	printf("DBINFO - %s - file_set_time_close %s %s\n",format_time(time(0)),fileName[fileIndex],format_time(fileTClose[fileIndex]));
	printf("DBINFO - %s - file_set_n_events %s %u\n",format_time(time(0)),fileName[fileIndex],fileEvents[fileIndex]);
	printf("DBINFO - %s - file_set_size %s %lu\n",format_time(time(0)),fileName[fileIndex],fileSize[fileIndex]);

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
	    printf("*** ERROR *** Unable to open file '%s' for writing.\n",pathName[fileIndex]);
	    proc_finalize(1,1,0,1,DB_STATUS_RUN_FAIL);
	  }
	  fileTOpen[fileIndex] = t_now;
	  fileSize[fileIndex] = 0;
	  fileEvents[fileIndex] = 0;

	  printf("DBINFO - %s - file_create %s %s %d %d\n",format_time(time(0)),fileName[fileIndex],"TRIGDATA",PEVT_CURRENT_VERSION,fileIndex);
	  printf("DBINFO - %s - file_set_time_open %s %s\n",format_time(time(0)),fileName[fileIndex],format_time(fileTOpen[fileIndex]));

	  // Write header to file
	  fHeadSize = create_file_head(fileIndex,Config->run_number,fileTOpen[fileIndex],(void *)outEvtBuffer);
	  writeSize = write(fileHandle,outEvtBuffer,fHeadSize);
	  if (writeSize != fHeadSize) {
	    printf("*** ERROR *** Unable to write file header to file. Header size: %d, Write result: %d\n",
		   fHeadSize,writeSize);
	    proc_finalize(1,1,0,1,DB_STATUS_RUN_FAIL);
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
      printf("*** ERROR *** Unable to write file tail to file. Tail size: %d, Write result: %d\n",
	     fTailSize,writeSize);
      proc_finalize(1,1,0,1,DB_STATUS_RUN_FAIL);
    }
    fileSize[fileIndex] += fTailSize;
    totalWriteSize += fTailSize;

    // Close output file and show some info about counters
    if (close(fileHandle) == -1) {
      printf("*** ERROR *** Unable to close output file '%s'.\n",fileName[fileIndex]);
      proc_finalize(1,1,0,1,DB_STATUS_RUN_FAIL);
    };
    if ( strcmp(Config->output_mode,"FILE")==0 ) {
      printf("%s - Closed output file '%s' after %d secs with %u events and size %lu bytes\n",
	     format_time(fileTClose[fileIndex]),pathName[fileIndex],
	     (int)(fileTClose[fileIndex]-fileTOpen[fileIndex]),
	     fileEvents[fileIndex],fileSize[fileIndex]);
      printf("DBINFO - %s - file_set_time_close %s %s\n",format_time(time(0)),fileName[fileIndex],format_time(t_now));
      printf("DBINFO - %s - file_set_n_events %s %u\n",format_time(time(0)),fileName[fileIndex],fileEvents[fileIndex]);
      printf("DBINFO - %s - file_set_size %s %lu\n",format_time(time(0)),fileName[fileIndex],fileSize[fileIndex]);

    } else {
      printf("%s - Closed output stream '%s' after %d secs with %u events and size %lu bytes\n",
	     format_time(fileTClose[fileIndex]),pathName[fileIndex],
	     (int)(fileTClose[fileIndex]-fileTOpen[fileIndex]),
	     fileEvents[fileIndex],fileSize[fileIndex]);
    }

    // Update file counter
    fileIndex++;

  }

  // Disable all triggers
  if ( trig_set_trigmask(0x00) != TRIG_OK ) {
    printf("*** ERROR *** Problem while resetting trigger mask. Exiting.\n");
    proc_finalize(1,1,0,1,DB_STATUS_RUN_FAIL);
  }

  // Drain trigger buffer
  while(data_len) {
    if ( trig_get_data((void*)data,&data_len) != TRIG_OK ) {
      printf("*** ERROR *** Problem while finalizing data readout. Exiting.\n");
      proc_finalize(1,1,0,1,DB_STATUS_RUN_FAIL);
    }
  }

  // Stop run: disable SIN
  if ( trig_stop_run() != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while stopping the run. Exiting.\n");
    proc_finalize(1,1,1,1,DB_STATUS_INIT_FAIL);
  }

  time(&t_daqstop);
  printf("%s - Acquisition stopped\n",format_time(t_daqstop));

  if ( trig_end() != TRIG_OK ) {
    printf("*** ERROR *** Problem while disconnecting from trigger board. Exiting.\n");
    proc_finalize(1,1,0,1,DB_STATUS_RUN_FAIL);
  }

  // Tell DB that the process has ended
  printf("- Setting process status to FINISHED (%d)\n",DB_STATUS_FINISHED);
  printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_FINISHED);
  printf("DBINFO - %s - process_set_time_stop %s\n",format_time(time(0)),format_time(t_daqstop));
  printf("DBINFO - %s - process_set_total_events %d\n",format_time(time(0)),totalWriteEvents);
  printf("DBINFO - %s - process_set_total_size %ld\n",format_time(time(0)),totalWriteSize);
  if ( strcmp(Config->output_mode,"FILE")==0 ) {
    printf("DBINFO - %s - process_set_n_files %d\n",format_time(time(0)),fileIndex);
  }

  // Show final report
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

  printf("Trigger mask statistics:");
  for (j=0;j<8;j++) { trig_cnts[j] = 0; }
  for (i=0;i<256;i++) {
    if (trig_stat[i]) {
      for (j=0;j<8;j++) {
	if (i & (1 << j)) trig_cnts[j] += trig_stat[i];
      }
    }
  }
  for (j=0;j<8;j++) {
    if (trig_cnts[j]) {
      printf(" %u(%u,%.2fHz)",j,trig_cnts[j],1.*trig_cnts[j]/t_daqtotal);
    }
  }
  printf("\n");

  if ( strcmp(Config->output_mode,"FILE")==0 ) {
    printf("=== %2d files created =====================================\n",fileIndex);
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
