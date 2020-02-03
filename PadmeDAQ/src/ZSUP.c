#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <math.h>

#include "DB.h"
#include "Config.h"
#include "Tools.h"
#include "PEvent.h"
#include "Signal.h"

#include "ZSUP.h"

//#define TIME_TAG_LEN     20
//#define MAX_FILENAME_LEN MAX_DATA_FILE_LEN+TIME_TAG_LEN

//#define MAX_N_OUTPUT_FILES 10240

extern int InBurst;
extern int BreakSignal;

// Handle zero suppression
int ZSUP_readdata ()
{

  // Input/Output event buffers
  int maxPEvtSize;
  char *inEvtBuffer = NULL;
  char *outEvtBuffer = NULL;

  // This is the pointer to the buffer which will be sent to output. Can point to inEvtBuffer or outEvtBuffer
  char *outputEventBuffer = NULL;

  // Dimensions (in 4 bytes words) of input and output event structures
  unsigned int inputEventSize, outputEventSize;

  // Size in bytes of file head and tail
  unsigned int fHeadSize, fTailSize;

  // Input/Output file handles
  int inFileHandle;
  int outFileHandle;
  unsigned int *line; // Used to read input buffer one line at a time
  unsigned int readSize,writeSize;

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

  // File to handle DAQ interaction with GUI
  //FILE* iokf; // InitOK file

  // Process timers
  time_t t_daqstart, t_daqstop, t_daqtotal;
  time_t t_now;

  unsigned int i;

  // Set signal handlers to make sure output file is closed correctly
  InBurst = 1;
  set_signal_handlers();

  // Allocate buffers to hold input and output event structures (same max size)

  maxPEvtSize = (PEVT_HEADER_LEN + 4*(PEVT_GRPHEAD_LEN + 512 + PEVT_GRPTTT_LEN) + 32*512)*4;

  inEvtBuffer = (char *)malloc(maxPEvtSize);
  if (inEvtBuffer == NULL) {
    printf("Unable to allocate input event buffer of size %d\n",maxPEvtSize);
    return 1;
  }
  printf("- Allocated input event buffer with size %d\n",maxPEvtSize);

  outEvtBuffer = (char *)malloc(maxPEvtSize);
  if (outEvtBuffer == NULL) {
    printf("Unable to allocate output event buffer of size %d\n",maxPEvtSize);
    return 1;
  }
  printf("- Allocated output event buffer with size %d\n",maxPEvtSize);

  // Zero counters
  totalReadSize = 0;
  totalReadEvents = 0;
  totalWriteSize = 0;
  totalWriteEvents = 0;

  // Start counting output files
  fileIndex = 0;
  tooManyOutputFiles = 0;

  // We have to open the output file first to avoid network-related lock-ups

  if ( strcmp(Config->output_mode,"FILE")==0 ) {

    // Generate name for initial output file and verify it does not exist
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

  // Open output file
  if ( strcmp(Config->output_mode,"FILE")==0 ) {
    printf("- Opening output file %d with path '%s'\n",fileIndex,pathName[fileIndex]);
    outFileHandle = open(pathName[fileIndex],O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
  } else {
    printf("- Opening output stream '%s'\n",pathName[fileIndex]);
    outFileHandle = open(pathName[fileIndex],O_WRONLY);
  }
  if (outFileHandle == -1) {
    printf("ERROR - Unable to open file '%s' for writing.\n",pathName[fileIndex]);
    return 2;
  }

  // Open virtual file for input data stream (will wait for DAQ to start before proceeding)
  printf("- Opening input stream from file '%s'\n",Config->input_stream);
  inFileHandle = open(Config->input_stream,O_RDONLY, S_IRUSR | S_IWUSR);
  if (inFileHandle == -1) {
    printf("ERROR - Unable to open input stream '%s' for reading.\n",Config->input_stream);
    return 1;
  }

  // Create initok file to tell RunControl that we are ready
  if ( create_initok_file() ) return 1;
  printf("- Setting process status to INITIALIZED (%d)\n",DB_STATUS_INITIALIZED);
  printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_INITIALIZED);

  // Read file header (4 words) from input stream
  readSize = read(inFileHandle,inEvtBuffer,16);
  if (readSize != 16) {
    printf("ERROR - Unable to read header from input stream\n");
    return 2;
  }
  totalReadSize += readSize;

  // First line: tag,version,index
  line = (unsigned int *)(inEvtBuffer+0);
  unsigned int tag = (*line >> 28) & 0xF;
  if (tag != 0X9) {
    printf("ERROR - File does not start with the right tag - Expected 0x9 - Found 0x%1X\n",tag);
    return 2;
  }
  unsigned int version = (*line >> 16) & 0x0FFF;
  if ( version != 3 ) {
    printf("ERROR - Invalid input stream format version: %d. Should be 3\n",version);
    return 2;
  }
  // This will be enabled after file-based tests have finished
  //unsigned int index = *line & 0xFFFF;
  //if ( index != 0 ) {
  //  printf("ERROR - Stream has non-zero index\n");
  //  return 2;
  //}

  // Second line: run number
  int run_number;
  memcpy(&run_number,inEvtBuffer+4,4);
  if ( run_number != Config->run_number ) {
    printf("WARNING - Run number from stream not consistent with that from configuration: stream %d config %d\n",run_number,Config->run_number);
  }
  printf("- Run number %d\n",run_number);

  // Third line: board_id and board serial number (save it to DB)
  line = (unsigned int *)(inEvtBuffer+8);
  int board_id = ( (*line & 0xff000000) >> 24 );
  unsigned int board_sn = (*line & 0x00ffffff);
  printf("- Board id %d S/N %u\n",board_id,board_sn);
  if (board_id != Config->board_id) {
    printf("WARNING - Board id from stream not consistent with that from configuration: stream %d config %d\n",board_id,Config->board_id);
  }

  // Save board serial number to DB for this process
  printf("DBINFO - %s - add_proc_config_para %s %u\n",format_time(time(0)),"board_sn",board_sn);

  // Fourth line: start of file time tag
  unsigned int start_time;
  memcpy(&start_time,inEvtBuffer+12,4);
  printf("- Start time %s\n",format_time(start_time));

  // Now that we have a recognized input stream we can start the zero suppression process

  time(&t_daqstart);
  printf("%s - Zero suppression started\n",format_time(t_daqstart));

  // Tell DB that the process has started
  printf("- Setting process status to RUNNING (%d)\n",DB_STATUS_RUNNING);
  printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_RUNNING);
  printf("DBINFO - %s - process_set_time_start %s\n",format_time(time(0)),format_time(t_daqstart));

  // Register the output file in the DB and send it the header

  fileTOpen[fileIndex] = t_daqstart;
  fileSize[fileIndex] = 0;
  fileEvents[fileIndex] = 0;

  // Register file in the DB
  if ( strcmp(Config->output_mode,"FILE")==0 ) {
    printf("DBINFO - %s - file_create %s %s %d %d\n",format_time(time(0)),fileName[fileIndex],"DAQDATA",PEVT_CURRENT_VERSION,fileIndex);
    printf("DBINFO - %s - file_set_time_open %s %s\n",format_time(time(0)),fileName[fileIndex],format_time(fileTOpen[fileIndex]));
  }

  // Write header to file
  fHeadSize = create_file_head(fileIndex,run_number,board_id,board_sn,fileTOpen[fileIndex],(void *)outEvtBuffer);
  writeSize = write(outFileHandle,outEvtBuffer,fHeadSize);
  if (writeSize != fHeadSize) {
    printf("ERROR - Unable to write file header to file. Header size: %u, Write result: %u\n",
	   fHeadSize,writeSize);
    return 2;
  }
  totalWriteSize += writeSize;
  fileSize[fileIndex] += fHeadSize;

  // Main loop
  int inputStreamEnd = 0;
  while (inputStreamEnd == 0) {

    // Read first line of next event
    readSize = read(inFileHandle,inEvtBuffer,4);
    if (readSize != 4) {
      printf("ERROR - Unable to read first line of event from stream.\n");
      return 2;
    }
    totalReadSize += readSize;
    line = (unsigned int *)(inEvtBuffer+0);
    unsigned int tag = (*line >> 28) & 0xF;

    // Check if this is a file tail tag
    if (tag == 0x5) {

      // File tail reached: read final information and exit

      // First line of file tail contains tag and number of events
      unsigned int nInEvents = *line & 0x0FFFFFFF;

      // Read remaining 3 words of file tail
      readSize = read(inFileHandle,inEvtBuffer+4,12);
      if (readSize != 12) {
	printf("ERROR - Unable to read final part of tail from stream.\n");
	return 2;
      }
      totalReadSize += readSize;

      // Second and third lines of file tail contain the total size of the input file
      unsigned long int eofFileSize;
      memcpy(&eofFileSize,inEvtBuffer+4,8);

      // Fourth line of file tail contains the end of file time tag
      unsigned int eofTimeTag;
      memcpy(&eofTimeTag,inEvtBuffer+12,4);

      // Print report about input stream
      printf("- Reached tail of stream - Events %u Size %lu Time %s\n",nInEvents,eofFileSize,format_time(eofTimeTag));

      inputStreamEnd = 1;
      continue;

    }

    // Check if this is an event tag
    if (tag != 0XE) {
      printf("ERROR - Event does not start with the right tag - Expected 0xE or 0x5 - Found 0x%1X\n",tag);
      return 2;
    }

    // Get size of event and read the full event in the input buffer
    inputEventSize = 4*(*line & 0x0FFFFFFF);
    readSize = read(inFileHandle,inEvtBuffer+4,inputEventSize-4); // First 4 bytes already read
    if (readSize != inputEventSize-4) {
      printf("ERROR - Unable to read final part of event from stream.\n");
      return 2;
    }
    totalReadSize += readSize;
    totalReadEvents++;

    if ( (Config->zero_suppression % 100) == 0 ) {

      // If zero suppression is switched off, we just send input event to output
      outputEventSize = inputEventSize;
      outputEventBuffer = inEvtBuffer;

      //// Even if zero suppression is off, show we are alive
      ////if (totalReadEvents % 100 == 0) {
      //if (totalReadEvents % Config->debug_scale == 0) {
      //	printf("Event %7d\tZero suppression OFF\n",totalReadEvents);
      //}

    } else {

      // Extract 0-suppression configuration
      //printf("Config-zero_suppression is %u\n",Config->zero_suppression);
      unsigned int zsupMode = (Config->zero_suppression / 100) & 0x1; // 0=rejction, 1=flagging
      unsigned int zsupAlgr = (Config->zero_suppression % 100) & 0xF; // 0=off, 1-15=algorithm code
      //printf("Entering zsup with mode %u algorithm %u\n",zsupMode,zsupAlgr);

      // If Autopass flag (bit 4 of status) is on, force zero suppression to flagging mode
      line = (unsigned int *)(inEvtBuffer+8);
      unsigned short int status = (*line >> 22) & 0x03FF;
      if ( status & 0x0010 ) zsupMode = 1;

      // Apply zero suppression algorithm
      //printf("Calling apply_zero_suppression with mode %u algorithm %u\n",zsupMode,zsupAlgr);
      outputEventSize = apply_zero_suppression(zsupMode,zsupAlgr,(void *)inEvtBuffer,(void *)outEvtBuffer);
      outputEventBuffer = outEvtBuffer;

    }
	  
    // Write event header to debug info once in a while
    //if ( (eventInfo.EventCounter % 100) == 0 ) {
    if ( (totalReadEvents % Config->debug_scale) == 0 ) {
      unsigned char i,j;
      printf("- Event %7d - Header",totalReadEvents);
      for (i=0;i<6;i++) {
	printf(" %1d(",i);
	for (j=0;j<4;j++) { printf("%02x",(unsigned char)(outputEventBuffer[i*4+3-j])); }
	printf(")");
      }
      printf("\n");
    }

    // Write data to output file
    writeSize = write(outFileHandle,outputEventBuffer,outputEventSize);
    if (writeSize != outputEventSize) {
      printf("ERROR - Unable to write event data to output file. Event size: %u, Write result: %u\n",
	     outputEventSize,writeSize);
      return 2;
    }
    fileSize[fileIndex] += writeSize;
    totalWriteSize += writeSize;
    fileEvents[fileIndex]++;
    totalWriteEvents++;

    // If we are running in FILE output mode, check if we need a new data file
    // i.e. required time elapsed or file size/events threshold exceeded
    if ( strcmp(Config->output_mode,"FILE")==0 ) {

      time(&t_now);
      if (
	  (t_now-fileTOpen[fileIndex] >= Config->file_max_duration) ||
	  (fileSize[fileIndex]        >= Config->file_max_size    ) ||
	  (fileEvents[fileIndex]      >= Config->file_max_events  )
	  ) {

	// Register file closing time
	fileTClose[fileIndex] = t_now;

	// Write tail to file
	fTailSize = create_file_tail(fileEvents[fileIndex],fileSize[fileIndex],fileTClose[fileIndex],(void *)outEvtBuffer);
	writeSize = write(outFileHandle,outEvtBuffer,fTailSize);
	if (writeSize != fTailSize) {
	  printf("ERROR - Unable to write file header to output file. Tail size: %u, Write result: %u\n",
		 fTailSize,writeSize);
	  return 2;
	}
	fileSize[fileIndex] += writeSize;
	totalWriteSize += writeSize;

	// Close old output file and show some info about counters
	if (close(outFileHandle) == -1) {
	  printf("ERROR - Unable to close output file '%s'.\n",fileName[fileIndex]);
	  return 2;
	};
	printf("%s - Closed output file '%s' after %d secs with %u events and size %lu bytes\n",
	       format_time(fileTClose[fileIndex]),pathName[fileIndex],
	       (int)(fileTClose[fileIndex]-fileTOpen[fileIndex]),
	       fileEvents[fileIndex],fileSize[fileIndex]);

	// Close file in DB
	printf("DBINFO - %s - file_set_time_close %s %s\n",format_time(time(0)),fileName[fileIndex],format_time(fileTClose[fileIndex]));
	printf("DBINFO - %s - file_set_size %s %lu\n",format_time(time(0)),fileName[fileIndex],fileSize[fileIndex]);
	printf("DBINFO - %s - file_set_n_events %s %u\n",format_time(time(0)),fileName[fileIndex],fileEvents[fileIndex]);

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
	  outFileHandle = open(pathName[fileIndex],O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
	  if (outFileHandle == -1) {
	    printf("ERROR - Unable to open file '%s' for writing.\n",pathName[fileIndex]);
	    return 2;
	  }
	  fileTOpen[fileIndex] = t_now;
	  fileSize[fileIndex] = 0;
	  fileEvents[fileIndex] = 0;

	  // Register file in the DB
	  printf("DBINFO - %s - file_create %s %s %d %d\n",format_time(time(0)),fileName[fileIndex],"DAQDATA",PEVT_CURRENT_VERSION,fileIndex);
	  printf("DBINFO - %s - file_set_time_open %s %s\n",format_time(time(0)),fileName[fileIndex],format_time(fileTOpen[fileIndex]));

	  // Write header to file
	  fHeadSize = create_file_head(fileIndex,run_number,board_id,board_sn,fileTOpen[fileIndex],(void *)outEvtBuffer);
	  writeSize = write(outFileHandle,outEvtBuffer,fHeadSize);
	  if (writeSize != fHeadSize) {
	    printf("ERROR - Unable to write file header to file. Header size: %u, Write result: %u\n",
		   fHeadSize,writeSize);
	    return 2;
	  }
	  fileSize[fileIndex] += writeSize;
	  totalWriteSize += writeSize;

	} else {

	  tooManyOutputFiles = 1;

	}

      }

    }

    // Check if it is time to stop DAQ (user interrupt, too many output files)
    if ( BreakSignal || tooManyOutputFiles ) break;

  }

  // Get time when processing ends
  time(&t_now);
 
  // Tell user what stopped DAQ
  if ( inputStreamEnd ) printf("=== Stopping ZSUP on End of Stream ===\n");
  if ( BreakSignal ) printf("=== Stopping ZSUP on interrupt %d ===\n",BreakSignal);
  if ( tooManyOutputFiles ) printf("=== Stopping ZSUP after writing %d data files ===\n",fileIndex);

  // Close input stream file
  printf("- Closing input stream.\n");
  if (close(inFileHandle) == -1) {
    printf("ERROR - Unable to close input stream '%s'.\n",Config->input_stream);
    return 2;
  };

  // If ZSUP was stopped for writing too many output files, we do not have to close the last file
  if ( ! tooManyOutputFiles ) {

    // Register file closing time
    fileTClose[fileIndex] = t_now;

    // Write tail to file
    fTailSize = create_file_tail(fileEvents[fileIndex],fileSize[fileIndex],fileTClose[fileIndex],(void *)outEvtBuffer);
    writeSize = write(outFileHandle,outEvtBuffer,fTailSize);
    if (writeSize != fTailSize) {
      printf("ERROR - Unable to write file tail to file. Tail size: %u, Write result: %u\n",
    	     fTailSize,(unsigned int)writeSize);
      return 2;
    }
    fileSize[fileIndex] += writeSize;
    totalWriteSize += writeSize;

    // Close output file and show some info about counters
    if (close(outFileHandle) == -1) {
      printf("ERROR - Unable to close output file '%s'.\n",fileName[fileIndex]);
      return 2;
    };
    if ( strcmp(Config->output_mode,"FILE")==0 ) {
      printf("%s - Closed output file '%s' after %d secs with %u events and size %lu bytes\n",
	     format_time(t_now),pathName[fileIndex],(int)(fileTClose[fileIndex]-fileTOpen[fileIndex]),
	     fileEvents[fileIndex],fileSize[fileIndex]);
      printf("DBINFO - %s - file_set_time_close %s %s\n",format_time(time(0)),fileName[fileIndex],format_time(fileTClose[fileIndex]));
      printf("DBINFO - %s - file_set_size %s %lu\n",format_time(time(0)),fileName[fileIndex],fileSize[fileIndex]);
      printf("DBINFO - %s - file_set_n_events %s %u\n",format_time(time(0)),fileName[fileIndex],fileEvents[fileIndex]);
    } else {
      printf("%s - Closed output stream '%s' after %d secs with %u events and size %lu bytes\n",
	     format_time(t_now),pathName[fileIndex],(int)(fileTClose[fileIndex]-fileTOpen[fileIndex]),
	     fileEvents[fileIndex],fileSize[fileIndex]);
    }

    // Update file counter
    fileIndex++;

  }

  time(&t_daqstop);
  printf("%s - Zero suppression stopped\n",format_time(t_daqstop));

  // Deallocate input/output event buffer
  free(inEvtBuffer);
  free(outEvtBuffer);
  printf("- Deallocated I/O event buffer\n");

  // Tell DB that the process has ended
  printf("DBINFO - %s - process_set_time_stop %s\n",format_time(time(0)),format_time(t_daqstop));
  printf("DBINFO - %s - process_set_total_events %d\n",format_time(time(0)),totalWriteEvents);
  printf("DBINFO - %s - process_set_total_size %ld\n",format_time(time(0)),totalWriteSize);
  if ( strcmp(Config->output_mode,"FILE")==0 ) {
    printf("DBINFO - %s - process_set_n_files %d\n",format_time(time(0)),fileIndex);
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
  printf("\n=== ZSUP ending on %s ===\n",format_time(t_daqstop));
  printf("Total running time: %d secs\n",(int)t_daqtotal);
  printf("Total number of events read: %u - %6.2f events/s\n",totalReadEvents,evtReadPerSec);
  printf("Total size of data read: %lu B - %6.2f KB/s\n",totalReadSize,sizeReadPerSec);
  printf("Total number of events written: %u - %6.2f events/s\n",totalWriteEvents,evtWritePerSec);
  printf("Total size of data written: %lu B - %6.2f KB/s\n",totalWriteSize,sizeWritePerSec);
  if ( strcmp(Config->output_mode,"FILE")==0 ) {
    printf("=== Files created =======================================\n");
    for(i=0;i<fileIndex;i++) {
      printf("'%s' %u %lu",pathName[i],fileEvents[i],fileSize[i]);
      printf(" %s",format_time(fileTOpen[i])); // Optimizer effect! :)
      printf(" %s\n",format_time(fileTClose[i]));
    }
  }
  printf("=========================================================\n");

  // Free space allocated for file names
  for(i=0;i<fileIndex;i++) free(fileName[i]);

  return 0;

}

unsigned int apply_zero_suppression (unsigned int zsupMode, unsigned int zsupAlgr, void *inBuff,void *outBuff)
{
  unsigned int *line;
  unsigned int outLine;

  void *inStart = inBuff;
  void *outStart = outBuff;

  void *inCursor = inBuff;
  void *outCursor = outBuff;

  unsigned int inSize = 0;
  unsigned int outSize = 0;

  unsigned int i;

  // Extract 0-suppression configuration
  //unsigned int zsupMode = (Config->zero_suppression / 100) & 0x1; // 0=rejction, 1=flagging
  //unsigned int zsupAlgr = (Config->zero_suppression % 100) & 0xF; // 0=off, 1-15=algorithm code

  // Position cursors at beginning of input and output event structures
  inCursor = inStart; outCursor = outStart;

  // First line of event header contains the tag (already checked) and the event size
  // Input event size is stored for final consistency check
  // First line of output will be created after the output event structure is finalized
  line = (unsigned int *)(inCursor);
  unsigned int inSizeCheck = *line & 0x0FFFFFFF;

  inCursor += 4; outCursor += 4; // Move to second line

  // Second line of event header contains board id, LVDS pattern, zsup algorithm, and group mask
  // Input line is copied to output after adding the suppression algorithm used
  // Group mask is saved for later use
  line = (unsigned int *)(inCursor);
  unsigned int groupMask = *line & 0x0000000F;
  outLine = (*line & 0xFFFFFF0F) + (zsupAlgr << 4);
  memcpy(outCursor,&outLine,4);

  inCursor += 4; outCursor += 4; // Move to third line

  // Third line of event header contains event status mask and event counter
  // Input line is copied to output after setting the zero suppression bit in the status mask
  line = (unsigned int *)(inCursor);
  //unsigned int eventNumber = *line & 0x003FFFFF;
  outLine = (*line & 0xFEFFFFFF) + (zsupMode << 24);
  memcpy(outCursor,&outLine,4);

  inCursor += 4; outCursor += 4; // Move to fourth line

  // Fourth line of event header contains the (coarse) Event Time Tag
  // Input line is copied to output with no changes
  memcpy(outCursor,inCursor,4);

  inCursor += 4; outCursor += 4; // Move to fifth line

  // Fifth line of event header contains the Active Channel Mask
  // Input active channel mask is saved and copied to output with no changes
  line = (unsigned int *)(inCursor);
  unsigned int activeChannelMask = *line;
  memcpy(outCursor,inCursor,4);

  inCursor += 4; outCursor += 4; // Move to sixth line

  // Sixth line contains the accepted channel mask
  // Input accepted channel mask is ignored (should be identical to active channel mask)
  // Output accepted channel mask will be defined after applying zero suppression
  // ...hence not much to do here...

  // ...just move to beginning of trigger group section
  inCursor += 4; outCursor += 4;

  // Event header is over: add size of event header to event size counters...
  inSize += 6; outSize += 6;

  // Get number of trigger groups from group mask
  unsigned int nGroups = ( (groupMask >> 0) && 0x1 ) + ( (groupMask >> 1) && 0x1 ) + ( (groupMask >> 2) && 0x1 ) + ( (groupMask >> 3) && 0x1 );
  
  // Copy all triggers to output with no modifications
  unsigned int grSize;
  for (i=0;i<nGroups;i++) {

    line = (unsigned int *)(inCursor);
    grSize = (*line) & 0x00000FFF;
    memcpy(outCursor,inCursor,4*grSize);

    // Add size of trigger group to event size counters
    inSize += grSize; outSize += grSize;

    // Jump to next group section
    inCursor += 4*grSize; outCursor += 4*grSize;

  }

  // Initialize mask of accepted channels
  unsigned int acceptedChannelMask = 0;

  // Loop over all channels and apply zero suppression
  unsigned int iCh,bCh,accept;
  for (iCh=0;iCh<32;iCh++) {

    bCh = (1 << iCh); // Bit pattern for this channel

    // Check if channel was acquired
    if (activeChannelMask & bCh) {

      // Call required zero suppression algorithm for this channel
      // The function MUST copy the 1024 16bit samples from the input buffer to the output buffer
      // while applying its zero suppression algorithm
      // WARNING: if required algorithm is unknown then channel is accepted!
      accept = 1;
      if ( zsupAlgr == 1 ) {
	accept = zsup_algorithm_1(inCursor,outCursor); // Channel number not needed
      } else if ( zsupAlgr == 2 ) {
        accept = zsup_algorithm_2(iCh,inCursor,outCursor);
      }
      
      // Tag accepted channels in the accepted channel mask
      if ( accept ) acceptedChannelMask += bCh;

      // Move to next channel
      inCursor += 1024*2;
      inSize += 1024/2;
      // Note: channel is written to output only if accepted or if zero suppression is in tagging mode
      if ( accept || zsupMode == 1 ) {
	outCursor += 1024*2;
	outSize += 1024/2;
      }

    }

  }

  ////if (eventNumber % 100 == 0) {
  //if (eventNumber % Config->debug_scale == 0) {
  //  printf("Event %7d\tActive 0x%08x\tAccepted 0x%08x\n",eventNumber,activeChannelMask,acceptedChannelMask);
  //}
  //printf("\tActive   0x%08x    Input   %6d\n",activeChannelMask,inSize);
  //printf("\tAccepted 0x%08x    Output  %6d\n",acceptedChannelMask,outSize);

  // Verify that input event size is consistent
  if (inSize != inSizeCheck) {
    // This should never happen
    printf("WARNING - Inconsistent input event size: expected %u found %u\n",inSizeCheck,inSize);
  }

  // Create first line of output event header (tag and event size)
  outLine = ( 0xE << 28 ) + ( outSize & 0x0FFFFFFF );
  memcpy(outStart,&outLine,4);

  // Save accepted channel mask to 6th line of output event header
  memcpy(outStart+20,&acceptedChannelMask,4);

  return 4*outSize; // Return size of output event (in bytes) to caller

}

unsigned int zsup_algorithm_1(void *inC,void *outC)
{

  // Initialize some counters
  float sum = 0.;
  float sum2 = 0.;
  float mean = 0.;
  float rms = 0.;
  //float thrHi = 8192.;
  float thrLo = -8192;
  unsigned int overThr = 0;
  unsigned int nOverThr = 0;
  unsigned int nMaxOverThr = 0;

  short s; // Used to store sample values (can be negative due to DRS4 corrections)
  float fs; // Used to store float version of sample values

  unsigned int cursor = 0; // Point cursor to first sample

  // Loop over 1024 samples
  unsigned int i;
  for (i=0;i<1024;i++) {

    // Get value of current sample and copy it to output
    memcpy(&s,inC+cursor,2);
    memcpy(outC+cursor,&s,2);
    fs = (float)s;
    //if (s<0 || s>4095) printf("\tWARNING %d %f\n",s,fs);
    //if (fs<0. || fs>4095.) printf("\tWARNING %d %f\n",s,fs);

    // Use the first 80 samples to compute pedestal and sigma pedestal
    if (i<Config->zs1_head) {

      // Compute sum of samples and sum of squares of samples (used for RMS)
      sum += fs;
      sum2 += fs*fs;
      if ( i == Config->zs1_head-1 ) {
	mean = sum/Config->zs1_head;
	rms = sqrt((sum2-sum*mean)/(Config->zs1_head-1));
	//thrHi = mean+Config->zs1_nsigma*rms;
	thrLo = mean-Config->zs1_nsigma*rms;
	//printf("Channel %d mean %f rms %f thrHi %f thrLo %f\n",Ch,mean,rms,thrHi,thrLo);
      }

    } else if ( i < 1024-Config->zs1_tail ) { // Do not consider final set of samples

      // Get longest set of consecutive samples above threshold
      if (overThr) {
	if (fs<thrLo) {
	  nOverThr++;
	  if (nOverThr>nMaxOverThr) nMaxOverThr = nOverThr;
	} else {
	  overThr = 0;
	  nOverThr = 0;
	}
      } else {
	if (fs<thrLo) {
	  overThr = 1;
	  nOverThr = 1;
	  if (nOverThr>nMaxOverThr) nMaxOverThr = nOverThr;
	}
      }

    }

    // Move to next sample
    cursor += 2;

  }

  // Channel is accepted if rms is bad or if at least zs1_nabovethr channels are above threshold
  //if ( (rms>Config->zs1_badrmsthr) ||  (nMaxOverThr>=Config->zs1_nabovethr) ) return 1;
  if (rms>Config->zs1_badrmsthr) {
    printf("Accepted for bad RMS\n");
    return 1;
  }
  if (nMaxOverThr>=Config->zs1_nabovethr) return 1;

  printf("Rejected for lack of signal\n");
  return 0; // Otherwise channel is rejected

}

unsigned int zsup_algorithm_2(unsigned int ch,void *inC,void *outC)
{

  // Initialize some counters. NB double is needed as sums can exceed 2*10^9
  double sum  = 0.;
  double sum2 = 0.;
  double rms  = 0.;

  short s; // Used to store sample values (can be negative due to DRS4 corrections)
  double fs; // Used to store float version of sample values

  unsigned int cursor = 0; // Point cursor to first sample

  // Loop over samples skipping last section (noisy)
  unsigned int i,imax;
  imax = 1024-Config->zs2_tail;
  for (i=0;i<1024;i++) {

    // Get value of current sample and copy it to output
    memcpy(&s,inC+cursor,2);
    memcpy(outC+cursor,&s,2);
    //if (s<0 || s>4095) printf("\tsample %d\n",s); // DEBUG

    // Compute sum of samples and sum of squares of samples (used for RMS) skipping last section of event
    if (i<imax) {
      fs = (double)s;
      //if (fs<0. || fs>4095.) printf("\tsample %d float %f\n",s,fs); // DEBUG
      sum += fs;
      sum2 += fs*fs;
    }

    // Move to next sample
    cursor += 2;

  }

  rms = sqrt((sum2-sum*sum/imax)/(imax-1));
  //printf("\tch %.2d\t%8.3f\n",ch,rms);
  if (rms < Config->zs2_minrms_ch[ch]) {
    //printf("Rejected for low rms\n");
    return 0;
  }

  //printf("Accepted for hi rms %8.3f\n",rms);
  return 1;

}
