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

#include "Config.h"
#include "Tools.h"
#include "PEvent.h"
#include "Signal.h"

#include "FAKE.h"

//#define TIME_TAG_LEN     20
//#define MAX_FILENAME_LEN MAX_DATA_FILE_LEN+TIME_TAG_LEN

//#define MAX_N_OUTPUT_FILES 10240

extern int InBurst;
extern int BreakSignal;

// Handle zero suppression
int FAKE_readdata ()
{

  // Input/Output event buffers
  int maxPEvtSize;
  char *outEvtBuffer = NULL;

  // Dimensions (in 4 bytes words) of input and output event structures
  unsigned int outputEventSize;

  // Size in bytes of file head and tail
  unsigned int fHeadSize, fTailSize;

  // Output file handle
  int outFileHandle;
  unsigned int writeSize;

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
  FILE* iokf; // InitOK file

  // Process timers
  time_t t_daqstart, t_daqstop, t_daqtotal;
  time_t t_now;

  unsigned int i;

  unsigned int boardSN = 0; // Use a fake board serial number

  // If quit file is already there, assume this is a test run and do nothing
  if ( access(Config->quit_file,F_OK) != -1 ) {
    printf("DAQ_readdata - Quit file '%s' found: will not generate FAKE events\n",Config->quit_file);
    return 0;
  }

  // Allocate buffer to hold output event structure
  maxPEvtSize = (PEVT_HEADER_LEN + 4*(PEVT_GRPHEAD_LEN + 512 + PEVT_GRPTTT_LEN) + 32*512)*4;
  outEvtBuffer = (char *)malloc(maxPEvtSize);
  if (outEvtBuffer == NULL) {
    printf("Unable to allocate output event buffer of size %d\n",maxPEvtSize);
    return 1;
  }
  printf("- Allocated output event buffer with size %d\n",maxPEvtSize);

  // FAKE is now ready to start. Create InitOK file
  printf("- Creating InitOK file '%s'\n",Config->initok_file);
  if ( access(Config->initok_file,F_OK) == -1 ) {
    iokf = fopen(Config->initok_file,"w");
    fclose(iokf);
    printf("- InitOK file '%s' created\n",Config->initok_file);
  } else {
    printf("- InitOK file '%s' already exists (?)\n",Config->initok_file);
    return 1;
  }

  if (Config->startdaq_mode == 0) {

    // If SW controlled DAQ is selected, we need to wait for the appearance of
    // the start file before starting producing FAKE events

    // Wait for Start/Quit file
    while(1){
      if ( access(Config->start_file,F_OK) != -1 ) {
	printf("- Start file '%s' found: starting FAKE events production\n",Config->start_file);
	break;
      }
      if ( access(Config->quit_file,F_OK) != -1 ) {
	printf("- Quit file '%s' found: exiting\n",Config->quit_file);
	return 3;
      }
      // Sleep for ~1ms before checking again
      usleep(1000);
    }

  } else {

    // Just start producing events
    printf("- Starting FAKE events production\n");

  }

  // Set signal handlers to make sure output file is closed correctly
  InBurst = 1;
  set_signal_handlers();

  time(&t_daqstart);
  printf("%s - Fake data generation started\n",format_time(t_daqstart));

  // Zero counters
  totalWriteSize = 0;
  totalWriteEvents = 0;

  // Start counting output files
  fileIndex = 0;
  tooManyOutputFiles = 0;

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
  fileTOpen[fileIndex] = t_daqstart;
  fileSize[fileIndex] = 0;
  fileEvents[fileIndex] = 0;

  // Write header to file
  fHeadSize = create_file_head(fileIndex,Config->run_number,Config->board_id,boardSN,fileTOpen[fileIndex],(void *)outEvtBuffer);
  writeSize = write(outFileHandle,outEvtBuffer,fHeadSize);
  if (writeSize != fHeadSize) {
    printf("ERROR - Unable to write file header to file. Header size: %u, Write result: %u\n",
	   fHeadSize,writeSize);
    return 2;
  }
  totalWriteSize += writeSize;
  fileSize[fileIndex] += fHeadSize;

  unsigned int triggerTimeTag = 0;
  unsigned int triggerTimeDelay = 2352941; // 20ms in 8.5E-9ns ticks

  unsigned int eventNumber = 0;

  // Main loop
  while (1) {

    outputEventSize = create_fake_event(eventNumber,triggerTimeTag,(void *)outEvtBuffer);

    // Write data to output file
    writeSize = write(outFileHandle,outEvtBuffer,outputEventSize);
    if (writeSize != outputEventSize) {
      printf("ERROR - Unable to write event data to output file. Event size: %u, Write result: %u\n",
	     outputEventSize,writeSize);
      return 2;
    }
    fileSize[fileIndex] += writeSize;
    totalWriteSize += writeSize;
    fileEvents[fileIndex]++;
    totalWriteEvents++;

    // Update event counter and trigger time
    eventNumber ++;
    triggerTimeTag = (triggerTimeTag + triggerTimeDelay) & 0x3fffffff;

    // Get current time: used to check if it is time to stop run or to change file
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

	  // Write header to file
	  fHeadSize = create_file_head(fileIndex,Config->run_number,Config->board_id,boardSN,fileTOpen[fileIndex],(void *)outEvtBuffer);
	  writeSize = write(outFileHandle,outEvtBuffer,fHeadSize);
	  if (writeSize != fHeadSize) {
	    printf("ERROR - Unable to write file header to file. Header size: %u, Write result: %u\n",
		   fHeadSize,writeSize);
	    return 2;
	  }
	  fileSize[fileIndex] += writeSize;

	} else {

	  tooManyOutputFiles = 1;

	}

      }

    }

    // Check if it is time to stop DAQ (user interrupt, too many output files, quit file, time elapsed)
    if (
	 BreakSignal || tooManyOutputFiles ||
	 (access(Config->quit_file,F_OK) != -1) ||
	 ( Config->total_daq_time && ( t_now-t_daqstart >= Config->total_daq_time ) )
       ) break;

    // Sleep for a while before continuing
    usleep(Config->daq_loop_delay);

  }

  // Get time when processing ends
  time(&t_now);
 
  // Tell user what stopped DAQ
  if ( BreakSignal ) printf("=== Stopping FAKE on interrupt %d ===\n",BreakSignal);
  if ( tooManyOutputFiles ) printf("=== Stopping FAKE after writing %d data files ===\n",fileIndex);
  if ( access(Config->quit_file,F_OK) != -1 )
    printf("=== Stopping FAKE on quit file '%s' ===\n",Config->quit_file);
  if ( Config->total_daq_time && ( t_now-t_daqstart >= Config->total_daq_time ) )
    printf("=== Stopping FAKE after %d secs of run (requested %d) ===\n",(int)(t_now-t_daqstart),Config->total_daq_time);

  // If DAQ was stopped for writing too many output files, we do not have to close the last file
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

    // Close output file and show some info about counters
    if (close(outFileHandle) == -1) {
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

  InBurst = 0; // Signal FAKE has stopped
  time(&t_daqstop);
  printf("%s - Fake data generation stopped\n",format_time(t_daqstop));

  // Deallocate input/output event buffer
  free(outEvtBuffer);

  // Give some final report
  evtWritePerSec = 0.;
  sizeWritePerSec = 0.;
  t_daqtotal = t_daqstop-t_daqstart;
  if (t_daqtotal>0) {
    evtWritePerSec = 1.*totalWriteEvents/t_daqtotal;
    sizeWritePerSec = totalWriteSize/(t_daqtotal*1024.);
  }
  printf("\n=== FAKE ending on %s ===\n",format_time(t_daqstop));
  printf("Total running time: %d secs\n",(int)t_daqtotal);
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

  return 0;

}

unsigned int create_fake_event(unsigned int eventNumber, unsigned int triggerTimeTag, void *outBuff)
{
  unsigned int outLine;

  void *outStart = outBuff;

  void *outCursor = outBuff;

  unsigned int outSize = 0;

  unsigned int i;


  // Position cursors at beginning of output event structures
  outCursor = outStart;

  // First line of event header contains the tag and the event size
  // First line of output will be created after the output event structure is finalized

  outCursor += 4; // Move to second line

  // Second line of event header contains board id, LVDS pattern, zsup algorithm, and group mask
  // Group mask is saved for later use
  unsigned int groupMask = Config->group_enable_mask;
  outLine = ((Config->board_id & 0xFF) << 24) + (0x0000 << 8) + (0 << 4) + ((groupMask & 0xF) << 0);
  memcpy(outCursor,&outLine,4);

  outCursor += 4; // Move to third line

  // Third line of event header contains event status mask and event counter
  // Input line is copied to output after setting the zero suppression bit in the status mask
  unsigned int eventStatus = 0x03; // Event has data (bit 0), DRS4 corrections are applied (bit 1)
  outLine = ((eventStatus & 0x3FF) << 22)+((eventNumber & 0x3FFFFF) << 0);
  memcpy(outCursor,&outLine,4);

  outCursor += 4; // Move to fourth line

  // Fourth line of event header contains the (coarse) Event Time Tag
  unsigned int eventTimeTag = 0; // Not relevant: set to 0
  memcpy(outCursor,&eventTimeTag,4);

  outCursor += 4; // Move to fifth line

  // Fifth line of event header contains the Active Channel Mask
  unsigned int activeChannelMask = Config->channel_enable_mask;
  memcpy(outCursor,&activeChannelMask,4);

  outCursor += 4; // Move to sixth line

  // Sixth line contains the accepted channel mask
  // As no zero suppression was applied, just save the active channel mask
  unsigned int acceptedChannelMask = Config->channel_enable_mask;
  memcpy(outCursor,&acceptedChannelMask,4);

  // Move to beginning of trigger group section
  outCursor += 4;

  // Event header is over: add size of event header to event size counters...
  outSize += 6;

  // Get number of trigger groups from group mask
  unsigned int nGroups = ( (groupMask >> 0) && 0x1 ) + ( (groupMask >> 1) && 0x1 ) + ( (groupMask >> 2) && 0x1 ) + ( (groupMask >> 3) && 0x1 );
  
  // Copy all triggers to output with no modifications
  unsigned int startIndexCell = rand() % 1024; // Not perfectly uniform but close enough
  unsigned int frequency = 2; // 0=5GHz, 1=2.5GHz, 2=1GHz
  unsigned int grSize = 1+1024/2+1; // head + 1024 x 16bit samples + tail
  for (i=0;i<nGroups;i++) {

    // Trigger head (Start Index Cell, DAQ frequency, Trigger sampled bit, Group size
    outLine = ((startIndexCell & 0x3FF) << 22) + ((frequency & 0x3) << 20) + (1 << 19) + ((grSize & 0xFFF) << 0);
    memcpy(outCursor,&outLine,4);

    outCursor += 4; // Move to sample section

    generate_trigger(outCursor); // Generate trigger samples

    outCursor += 1024*2; // Jump to trigger group tail section

    // Trigger tail (Trigger time tag)
    outLine = ((triggerTimeTag & 0x3FFFFFFF) << 0);
    memcpy(outCursor,&outLine,4);

    outSize += grSize; // Add size of trigger group to event size counter

    outCursor += 4; // Jump to next trigger group

  }

  // Loop over all channels and generate required ones
  unsigned int iCh,bCh;
  for (iCh=0;iCh<32;iCh++) {

    bCh = (1 << iCh); // Bit pattern for this channel

    // If channel is active, generate it
    if (activeChannelMask & bCh) {

      generate_channel(iCh,outCursor);

      outSize += 1024/2; // Add size of channel to event size counter

      outCursor += 1024*2; // Jump to next channel

    }

  }

  if (eventNumber % 100 == 0) {
    printf("Event %7d\tSize %d\tTriggerTimeTag %d\n",eventNumber,outSize,triggerTimeTag);
  }

  // Create first line of output event header (tag and event size)
  outLine = ( 0xE << 28 ) + ( outSize & 0x0FFFFFFF );
  memcpy(outStart,&outLine,4);

  return 4*outSize; // Return size of output event (in bytes) to caller

}

void generate_trigger(void *outC)
{

  short s; // Used to store sample values (can be negative due to DRS4 corrections)

  short base = 3900;
  short flat = 20; // 20/sqrt(12) ~ 6

  unsigned int cursor = 0; // Point cursor to first sample

  unsigned int i;
  for (i=0;i<1024;i++) {
    s = base + (rand()%flat); // Flat distribution around base with RMS~6
    memcpy(outC+cursor,&s,2);
    cursor += 2;
  }

}

void generate_channel(unsigned int ch,void *outC)
{

  short s; // Used to store sample values (can be negative due to DRS4 corrections)

  short base = 3900;
  short flat = 20; // 20/sqrt(12) ~ 6

  unsigned int cursor = 0; // Point cursor to first sample

  unsigned int i;
  for (i=0;i<1024;i++) {
    s = base + (rand()%flat); // Flat distribution around base with RMS~6
    memcpy(outC+cursor,&s,2);
    cursor += 2;
  }

}
