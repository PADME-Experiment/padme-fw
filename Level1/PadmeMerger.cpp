#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <vector>
#include <cmath>

#include "EventTags.hh"
//#include "ADCEventTags.hh"
#include "Trigger.hh"
#include "ADCBoard.hh"
#include "Configuration.hh"
#include "DBService.hh"

#define LVL1_PROC_MAX 16

// Clock frequencies (in GHz) from the board producers (P.Branchini of INFN Roma3 and P.Barba of CAEN)
// N.B. as the values are specified with no suffix, they are double precision
#define TRIGGER_CLOCK_FREQUENCY  80.0000E-3
#define V1742_CLOCK_FREQUENCY   117.1875E-3

// Time tolerance (in ns) for trigger and boards to be considered in-time
//#define INTIME_TOLERANCE_NS 10
#define INTIME_TOLERANCE_NS 15

// Define fcntl commands to test and increase size of fifo pipes (set:1024+7, get:1024+8)
#define F_SETPIPE_SZ 1031
#define F_GETPIPE_SZ 1032
#define PIPESIZE_MB  16

int main(int argc, char* argv[])
{

  //int rc; // DB library return code

  //time_t time_start, time_stop, time_first, time_last;
  time_t time_start, time_first, time_last;
  //char t_fmt[20]; // Formatted time string "YYYY-MM-DD hh:mm:ss"

  // Set standard output/error in unbuffered mode
  setbuf(stdout,NULL);
  setbuf(stderr,NULL);

  // Make sure we are on a sane machine (int: 32bits, long int: 64bits)
  if (sizeof(int) < 4 || sizeof(long int) < 8) {
    printf("*** ERROR *** On this machine int is %lu bytes and long int is %lu bytes. Aborting.\n",sizeof(int),sizeof(long int));
    exit(1);
  }

  // Connect to configuration handler
  Configuration* cfg = Configuration::GetInstance();

  // Get default parameters from configurator
  int run_number = cfg->RunNumber();
  std::string input_stream_list = cfg->InputStreamList();
  std::string output_stream_list = cfg->OutputStreamList();
  unsigned int verbose = cfg->Verbose();

  // Parse options
  int c;
  while ((c = getopt (argc, argv, "r:i:o:v:h")) != -1) {
    switch (c)
      {
      case 'r':
	if (run_number != cfg->RunNumber()) {
	  fprintf (stderr, "Error while processing option '-r'. Multiple runs specified.\n");
          exit(1);
      	}
	if (sscanf(optarg,"%d",&run_number) != 1) {
          fprintf (stderr, "Error while processing option '-r'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        if (run_number<0) {
          fprintf (stderr, "Error while processing option '-r'. Run number set to %d (must be >=0).\n", run_number);
          exit(1);
        }
        fprintf(stdout,"Merging files from run %d\n",run_number);
      	cfg->SetRunNumber(run_number);
	break;
      case 'o':
        output_stream_list = optarg;
        fprintf(stdout,"Merged data will be written to streams listed in '%s'\n",output_stream_list.c_str());
	cfg->SetOutputStreamList(output_stream_list);
        break;
      case 'i':
        input_stream_list = optarg;
        fprintf(stdout,"Data will be read from streams listed in '%s'\n",input_stream_list.c_str());
	cfg->SetInputStreamList(input_stream_list);
        break;
      case 'v':
        if ( sscanf(optarg,"%u",&verbose) != 1 ) {
          fprintf (stderr, "Error while processing option '-v'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        fprintf(stdout,"Set verbose level to %u\n",verbose);
	cfg->SetVerbose(verbose);
        break;
      case 'h':
        fprintf(stdout,"\nPadmeMerger [-r run_number] [-i input_stream_list] [-o output_stream_list] [-v verbose_level] [-h]\n\n");
        fprintf(stdout,"  -r: define run number being processed (default: %d)\n",cfg->RunNumber());
        fprintf(stdout,"  -i: define file with list of input streams (default: '%s')\n",cfg->InputStreamList().c_str());
        fprintf(stdout,"  -o: define file with list of output streams (default: '%s')\n",cfg->OutputStreamList().c_str());
        fprintf(stdout,"  -v: define verbose level (default: %u)\n",cfg->Verbose());
        fprintf(stdout,"  -h: show this help message and exit\n\n");
        exit(0);
      case '?':
        if (optopt == 'v') {
          // verbose with no argument: increas verbose level by 1
          cfg->SetVerbose(cfg->Verbose()+1);
          break;
	} else if (optopt == 'r' || optopt == 'i' || optopt == 'o')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint(optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
        exit(1);
      default:
        abort();
      }
  }

  // Check if input and output file lists were defined and are accessible
  if ( cfg->InputStreamList().compare("") == 0 ) {
    printf("ERROR no input file list defined. Aborting\n");
    exit(1);
  }
  if ( access( cfg->InputStreamList().c_str(), F_OK ) != 0 ) {
    printf("ERROR input file list %s is not accessible. Aborting\n",cfg->InputStreamList().c_str());
    exit(1);
  }
  if ( cfg->OutputStreamList().compare("") == 0 ) {
    printf("ERROR no output file list defined. Aborting\n");
    exit(1);
  }
  if ( access( cfg->OutputStreamList().c_str(), F_OK ) != 0 ) {
    printf("ERROR output file list %s is not accessible. Aborting\n",cfg->OutputStreamList().c_str());
    exit(1);
  }

  time(&time_start);
  printf("=== PadmeMerger starting on %s UTC ===\n",cfg->FormatTime(time_start));

  ADCBoard* board;
  std::vector<ADCBoard*> boards;

  std::ifstream list;
  std::string line;

  // Create Trigger handler
  Trigger* trigger = new Trigger();

  // Get list of boards and files from input file list
  int bid;
  char bfile[1024];
  list.open(cfg->InputStreamList().c_str());
  while(!list.eof()){

    getline(list,line);
    if (line.compare("")!=0) {

      if ( sscanf(line.c_str(),"%d %s",&bid,bfile) != 2 ) {
	printf("ERROR while parsing list '%s'. Aborting\n",cfg->InputStreamList().c_str());
	exit(1);
      }

      // If board id is 99, this is the Trigger stream
      if (bid == 99) {

	std::string filePath = bfile;
	//printf("\tBoard %d - File %s\n",bid,filePath.c_str());
	trigger->AddFile(filePath);

      } else {

	// Up to 32 ADC boards can be accepted in current setup
	if (bid>=32) {
	  printf("*** ERROR *** Out-of-bound board id specified in input list: %d. Aborting",bid);
	  exit(1);
	}

	// Find board with correct board id or add it to list if new
	std::vector<ADCBoard*>::iterator it;
	for (it = boards.begin(); it != boards.end(); ++it) {
	  board = *it;
	  if (board->GetBoardId() == bid) break;
	}
	if (it == boards.end()) {
	  printf("Board id %d\n",bid);
	  board = new ADCBoard(bid);
	  boards.push_back(board);
	}

	// Add file to board
	std::string filePath = bfile;
	//printf("\tBoard %d - File %s\n",bid,filePath.c_str());
	board->AddFile(filePath);

      }

    }

  }
  list.close();

  // Show list of files with trigger info and initialize the trigger
  printf("Trigger files %d\n",trigger->GetNFiles());
  for(int f=0; f<trigger->GetNFiles(); f++) printf("File %d %s\n",f,trigger->GetFileName(f).c_str());
  trigger->Init();

  // Show list of known boards (with list of files) and initialize them
  printf("Reading %d board(s)\n",(int)boards.size());
  std::vector<ADCBoard*>::iterator it;
  for (it = boards.begin(); it != boards.end(); ++it) {
    board = *it;
    printf("Board %d Files %d\n",board->GetBoardId(),board->GetNFiles());
    for(int f=0; f<board->GetNFiles(); f++) printf("File %d %s\n",f,board->GetFileName(f).c_str());
    board->Init();
  }
  
  // Get list of fifo files for output streams, open them and reset relative counters (#events and size)
  std::string output_stream[LVL1_PROC_MAX];
  //std::ofstream output_stream_handle[LVL1_PROC_MAX];
  int output_stream_handle[LVL1_PROC_MAX];
  unsigned int writeSize;
  unsigned int output_stream_nevents[LVL1_PROC_MAX];
  unsigned long long int output_stream_size[LVL1_PROC_MAX];
  unsigned int NOutputStreams = 0;
  list.open(cfg->OutputStreamList().c_str());
  while(!list.eof()){
    getline(list,line);
    if (line.compare("")!=0) {
      if (NOutputStreams >= LVL1_PROC_MAX) {
	printf("ERROR - file with list of output streams contains more than %d files. Aborting",LVL1_PROC_MAX);
	exit(1);
      }
      printf("- Opening output stream %2u '%s'\n",NOutputStreams,line.c_str());
      output_stream[NOutputStreams] = line;

      // Open output stream and increase the stream buffer size
      //output_stream_handle[NOutputStreams].open(line.c_str(),std::ios::out | std::ios::binary);
      output_stream_handle[NOutputStreams] = open(line.c_str(),O_WRONLY);
      //long pipe_size = (long)fcntl(output_stream_handle[NOutputStreams],1024+8);
      long pipe_size = (long)fcntl(output_stream_handle[NOutputStreams],F_GETPIPE_SZ);
      if (pipe_size == -1) { perror("get pipe size failed."); }
      printf("Default pipe size: %ld\n", pipe_size);

      //int ret = fcntl(output_stream_handle[NOutputStreams],1024+7,128*1024*1024);
      int ret = fcntl(output_stream_handle[NOutputStreams],F_SETPIPE_SZ,PIPESIZE_MB*1024*1024);
      if (ret < 0) { perror("set pipe size failed."); }

      //pipe_size = (long)fcntl(output_stream_handle[NOutputStreams],1024+8);
      pipe_size = (long)fcntl(output_stream_handle[NOutputStreams],F_GETPIPE_SZ);
      if (pipe_size == -1) { perror("get pipe size 2 failed."); }
      printf("Pipe size: %ld\n", pipe_size);

      output_stream_nevents[NOutputStreams] = 0;
      output_stream_size[NOutputStreams] = 0;
      NOutputStreams++;
    }
  }
  list.close();
  printf("- Using a total of %u output Level1 streams\n",NOutputStreams);

  printf("DBINFO - %s - process_set_status %d\n",cfg->FormatTime(time(0)),DB_STATUS_RUNNING);
  printf("DBINFO - %s - process_set_time_start %s\n",cfg->FormatTime(time(0)),cfg->FormatTime(time_start));

  unsigned int CurrentOutputStream = 0; // First event will be sent to first output stream

  // Loop over all events in files

  //int dT; // Clock counter difference wrt previous event: can be negative if clock counter rolled over
  //int dTb; // Clock counter difference between two groups of the same board
  //unsigned int TT_evt,TT_grp[ADCEVENT_NTRIGGERS]; // Clock counters for board and for group
  //unsigned int TT_old[boards.size()]; // Clock counter at previous event
  //unsigned long int CC[boards.size()]; // Total ADC Clock Counts since start of run (ns)

  //unsigned long int trigTotalCount, trigCount, trigCountOld; // Total, current and old values of trigger clock counter
  unsigned long long int boardTime, trigTime; // Total Board/Trigger time since start of run in ns
  long long int dT_board_trig; // Time difference between board and trigger

  //unsigned char groupMaskRef[boards.size()]; // Reference group masks from event 0

  unsigned int nEOR, trigEOR;
  unsigned int NumberOfEvents = 0;
  unsigned short int EventStatus = 0;
  unsigned int MissingADCBoards; // Bit mask of ADC boards missing in current event
  //struct timespec sys_time;
  //long int old_sys_time_ns = 0;
  //long int dt_ns;
  while(1){

    //if (cfg->Verbose()>=1) printf("=== Processing event %8u ===\n",NumberOfEvents);

    // Initialize EventStatus and MissingADCBoards
    EventStatus = (1 << EVENT_V03_STATUS_ISCOMPLETE_BIT); // Assume event is complete
    MissingADCBoards = 0x00000000; // No board is missing yet

    // Get next trigger event
    trigEOR = 0;
    //clock_gettime(CLOCK_REALTIME,&sys_time);
    //dt_ns = sys_time.tv_nsec-old_sys_time_ns; if (dt_ns<0) dt_ns += 1000000000;
    //old_sys_time_ns = sys_time.tv_nsec;
    //printf("Before trigger %9ld.%09ld dt %3ld\n",sys_time.tv_sec,sys_time.tv_nsec,dt_ns/1000000);
    if ( ! trigger->NextEvent() ) {
      trigEOR = 1;
      printf("*** Trigger - End of Run.\n");
    }

    // Load next event for all boards
    //clock_gettime(CLOCK_REALTIME,&sys_time);
    //printf("Before boards  %ld.%09ld\n",sys_time.tv_sec,sys_time.tv_nsec);
    nEOR = 0;
    for(unsigned int b=0; b<boards.size(); b++) {
      //clock_gettime(CLOCK_REALTIME,&sys_time);
      //printf("Before brd. %2d %ld.%09ld\n",boards[b]->GetBoardId(),sys_time.tv_sec,sys_time.tv_nsec);
      if ( ! boards[b]->NextEvent() ) {
	nEOR++;
	printf("*** Board %2d - End of Run.\n",boards[b]->GetBoardId());
      }
    }
    //clock_gettime(CLOCK_REALTIME,&sys_time);
    //printf("After boards   %ld.%09ld\n",sys_time.tv_sec,sys_time.tv_nsec);

    // Check if anybody reached End Of Run
    if ( trigEOR || nEOR ) {
      if (trigEOR) printf("Trigger reached end of run.\n");
      if (nEOR == boards.size()) {
	printf("All boards reached end of run.\n");
      } else {
	printf("%d board(s) reached end of run.\n",nEOR);
      }
      break; // Exit from main loop
    }

    if (NumberOfEvents == 0) {

      // All the info we need to create the file header is now available
      // (data format version, board ids, and board serial numbers)

      // Create file header structure
      unsigned int fbuff;
      char file_header[16+4*boards.size()];
      unsigned int header_size = 0;

      // First line: start of file tag, version (index is always 0)
      printf("Version is %d\n",boards[0]->File(0)->GetVersion());
      fbuff = (EVENT_FILE_HEAD_TAG << 28) + ( (boards[0]->File(0)->GetVersion() & 0x0fff) << 16 );
      memcpy(file_header+header_size,&fbuff,4); header_size += 4;

      // Second line: run number
      fbuff = cfg->RunNumber();
      memcpy(file_header+header_size,&fbuff,4); header_size += 4;

      // Third line: number of ADC boards
      fbuff = (boards.size() & 0xffffffff);
      memcpy(file_header+header_size,&fbuff,4); header_size += 4;

      // Several lines with boards ids (8bits) and serial numbers (24bits)
      for(unsigned int i=0; i<boards.size(); i++) {
	printf("Board %u has id %d and SN %u\n",i,boards[i]->GetBoardId(),boards[i]->GetSerialNumber());
	fbuff = ( (boards[i]->GetBoardId() & 0xff) << 24 ) + (boards[i]->GetSerialNumber() & 0x00ffffff);
	memcpy(file_header+header_size,&fbuff,4); header_size += 4;
      }
      // Last line: time when file is opened
      time_t t_open;
      time(&t_open);
      fbuff = (t_open & 0xffffffff);
      memcpy(file_header+header_size,&fbuff,4); header_size += 4;

      // Send header to all output files
      for(unsigned int i = 0; i<NOutputStreams; i++) {
	//output_stream_handle[i].write(file_header,header_size);
	writeSize = write(output_stream_handle[i],file_header,header_size);
	if (writeSize != header_size) {
	  printf("ERROR - Unable to write file header to file. Header size: %d, Write result: %d\n",
		 header_size,writeSize);
	  exit(1);
	}
	output_stream_size[i] += header_size;
      }

    }

    /*
    unsigned int skipped_trigger;
    unsigned int all_in_time = 0;
    nEOR = 0; trigEOR = 0;
    while ( (! all_in_time) && (! nEOR) && (! trigEOR) ) {

      // Verify if all boards are in time with the trigger (10ns tolerance)
      trigTime = lrint(trigger->GetTotalClockCounter()/TRIGGER_CLOCK_FREQUENCY);
      //printf("Trigger 0x%02x %u %llu %llu %llu\n",trigger->GetTriggerMask(),trigger->GetTriggerCounter(),trigger->GetClockCounter(),trigger->GetTotalClockCounter(),trigTime);
      all_in_time = 1;
      skipped_trigger = 0;
      for(unsigned int b=0; b<boards.size(); b++) {

	boardTime = lrint(boards[b]->GetTotalClockCounter()/V1742_CLOCK_FREQUENCY);
	//unsigned int ttt[4];
	//boards[b]->GetTriggerTimeTags(ttt);
	//printf("Board 0x%x %u %u %u %u %u %llu %llu\n",boards[b]->GetGroupMask(),ttt[0],ttt[1],ttt[2],ttt[3],boards[b]->GetClockCounter(),boards[b]->GetTotalClockCounter(),boardTime);

	dT_board_trig = boardTime-trigTime;
	if (cfg->Verbose()>=2)
	  printf("Board %7u %16llu Trigger %4u %16llu dT %10lld\n",
		 boards[b]->GetEventCounter(),boardTime,trigger->GetTriggerCounter(),trigTime,dT_board_trig);
	if ( dT_board_trig < -INTIME_TOLERANCE_NS ) {

	  // This board is still on an earlier event: skip to next event
	  // This happens after one or more boards miss a trigger and the trigger is advanced to its next event
      	  all_in_time = 0;
      	  printf("*** Board %2d - Board on earlier event: skipping it %8u\n",boards[b]->GetBoardId(),boards[b]->Event()->GetEventCounter());
      	  if ( ! boards[b]->NextEvent() ) {
      	    printf("*** Board %2d - End of Run.\n",boards[b]->GetBoardId());
      	    nEOR++;
      	  }

	} else if ( dT_board_trig > INTIME_TOLERANCE_NS ) {

	  // This board skipped a trigger
      	  printf("*** Board %2d - Board skipped a trigger on event %8u\n",boards[b]->GetBoardId(),boards[b]->Event()->GetEventCounter());
	  skipped_trigger = 1;

	}

	// If one or more boards skipped an event we move to the next trigger event
	if (skipped_trigger) {
	  all_in_time = 0;
      	  printf("*** One or more boards skipped a trigger (see previous messages): getting next trigger event\n");
	  if ( ! trigger->NextEvent() ) {
	    printf("*** Trigger - End of Run.\n");
	    trigEOR = 1;
	  }
	}

      }

    } // End of while loop checking for in-time events
    */

    // Verify if all boards are in time with the trigger (10ns tolerance)
    trigTime = lrint(trigger->GetTotalClockCounter()/TRIGGER_CLOCK_FREQUENCY);
    nEOR = 0;
    for(unsigned int b=0; b<boards.size(); b++) {

      boardTime = lrint(boards[b]->GetTotalClockCounter()/V1742_CLOCK_FREQUENCY);
      dT_board_trig = boardTime-trigTime;

      if (cfg->Verbose()>=2)
	printf("Board %7u %16llu Trigger %4u %16llu dT %10lld\n",
	       boards[b]->GetEventCounter(),boardTime,trigger->GetTriggerCounter(),trigTime,dT_board_trig);

      while (dT_board_trig < -INTIME_TOLERANCE_NS) {
	// Board time is earlier than trigger time: this should be impossible but we check for it anyway
	printf("*** Board %2d - Board time %llu less than Trigger time %llu: skip event and try to recover\n",boards[b]->GetBoardId(),boardTime,trigTime);
	if ( ! boards[b]->NextEvent() ) {
	  printf("*** Board %2d - End of Run.\n",boards[b]->GetBoardId());
	  nEOR++;
	  break;
	}
	boardTime = lrint(boards[b]->GetTotalClockCounter()/V1742_CLOCK_FREQUENCY);
	dT_board_trig = boardTime-trigTime;
      }

      if (dT_board_trig > INTIME_TOLERANCE_NS) {
	// Board time is after trigger time, i.e. board probably missed a trigger: tag it as missing
	printf("*** Board %2d - Board time %llu greater than Trigger time %llu: setting board as MISSING in this event\n",boards[b]->GetBoardId(),boardTime,trigTime);
	boards[b]->SetMissingEvent();
      }

    }

    // If one or more files reached EOR, stop processing events
    if (nEOR) {
      printf("WARNING: %d board(s) reached end of run because of time mismatches (see previous messages).\n",nEOR);
      break; // Exit from main loop
    }

    // Create missing boards mask and handle "Event complete" status bit
    for(unsigned int b=0; b<boards.size(); b++) {
      if (boards[b]->EventIsMissing()) {
	MissingADCBoards |= (1 << boards[b]->GetBoardId()); // Set ADC board bit in mask
	EventStatus &= ( ~(1 << EVENT_V03_STATUS_ISCOMPLETE_BIT) ); // Reset "Event complete" status bit
      }
    }

    // Add autopass bit to event status (0: autopass OFF, 1: autopass ON)
    if (! trigger->GetTriggerAuto()) { // autopass bit from trigger board is inverted: 0:ON, 1:OFF
      EventStatus |= (1 << EVENT_V03_STATUS_AUTOPASS_BIT);
    } else {
      EventStatus &= ( ~(1 << EVENT_V03_STATUS_AUTOPASS_BIT) );
    }


    // Send all data to next Level1 process

    // Merging time is counted from the first in-time event
    if (NumberOfEvents == 0) time(&time_first);

    // Compute total size of event in 4-bytes words
    UInt_t total_event_size = EVENT_V03_EVENTHEAD_LEN+TRIGEVENT_V03_EVENTHEAD_LEN; // Header and trigger info
    // Add size of all boards
    for (unsigned int b=0; b<boards.size(); b++) {
      if (! boards[b]->EventIsMissing()) total_event_size += boards[b]->GetEventSize();
    }

    // Get precise system time
    struct timespec now;
    if( clock_gettime(CLOCK_REALTIME,&now) == -1 ) {
      perror("clock gettime");
      exit(EXIT_FAILURE);
    }

    //printf("Before output  %ld.%09ld\n",now.tv_sec,now.tv_nsec);

    // Create event header structure
    char evt_h[4*EVENT_V03_EVENTHEAD_LEN];
    unsigned int evt_h_size = 0;
    unsigned int evt_h_buff;

    // Line 0: tag + event size (in 4bytes words)
    evt_h_buff = (EVENT_HEADER_TAG << 28) + ( (total_event_size & 0x0fffffff) << EVENT_EVENTSIZE_POS );
    memcpy(evt_h+evt_h_size,&evt_h_buff,4); evt_h_size += 4;

    // Line 1: event number
    evt_h_buff = ( (NumberOfEvents & 0xffffffff) << EVENT_V03_EVENTNUMBER_POS );
    memcpy(evt_h+evt_h_size,&evt_h_buff,4); evt_h_size += 4;

    // Line 2+3: event time sec+nsec
    evt_h_buff = ( (now.tv_sec & 0xffffffff) << EVENT_V03_EVENTTIMESEC_POS );
    memcpy(evt_h+evt_h_size,&evt_h_buff,4); evt_h_size += 4;
    evt_h_buff = ( (now.tv_nsec & 0xffffffff) << EVENT_V03_EVENTTIMENSEC_POS );
    memcpy(evt_h+evt_h_size,&evt_h_buff,4); evt_h_size += 4;

    // Line 4+5: run time, i.e. the number of 80MHz clock cycles counted by the trigger controller since the beginning of the run
    unsigned long long int trig_clock = trigger->GetTotalClockCounter();
    memcpy(evt_h+evt_h_size,&trig_clock,8); evt_h_size += 8;

    // Line 6: event status + trigger mask
    // Trigger mask currently contains a copy of the 6bits mask from the trigger
    evt_h_buff = ( (EventStatus & 0xffff) << EVENT_V03_EVENTSTATUS_POS ) + ( (trigger->GetTriggerMask() & 0xffff) << EVENT_V03_TRIGMASK_POS );
    memcpy(evt_h+evt_h_size,&evt_h_buff,4); evt_h_size += 4;

    // Line 7: bit mask of boards missing from current event
    evt_h_buff = ( (MissingADCBoards & 0xffffffff) << EVENT_V03_MISSADCBOARDS_POS );
    memcpy(evt_h+evt_h_size,&evt_h_buff,4); evt_h_size += 4;

    // Send event header to current output stream
    //output_stream_handle[CurrentOutputStream].write(evt_h,evt_h_size);
    writeSize = write(output_stream_handle[CurrentOutputStream],evt_h,evt_h_size);
    if (writeSize != evt_h_size) {
      printf("ERROR - Unable to write event header to file. Header size: %d, Write result: %d\n",
	     evt_h_size,writeSize);
      exit(1);
    }
    output_stream_size[CurrentOutputStream] += evt_h_size;

    // Copy trigger event structure to output (3 words, no need to change anything)
    //output_stream_handle[CurrentOutputStream].write((const char*)trigger->Buffer(),4*TRIGEVENT_V03_EVENTHEAD_LEN);
    writeSize = write(output_stream_handle[CurrentOutputStream],(const char*)trigger->Buffer(),4*TRIGEVENT_V03_EVENTHEAD_LEN);
    if (writeSize != 4*TRIGEVENT_V03_EVENTHEAD_LEN) {
      printf("ERROR - Unable to write trigger event structure to file. Structure size: %d, Write result: %d\n",
	     4*TRIGEVENT_V03_EVENTHEAD_LEN,writeSize);
      exit(1);
    }
    output_stream_size[CurrentOutputStream] += 4*TRIGEVENT_V03_EVENTHEAD_LEN;

    if (cfg->Verbose()>=1)
      printf("Event %9u Status 0x%04x Trigger Mask 0x%04x Missing boards 0x%08x\n",NumberOfEvents,EventStatus,trigger->GetTriggerMask(),MissingADCBoards);

    // Loop over all boards and send their event data to output stream
    for(unsigned int b=0; b<boards.size(); b++) {
      if (! boards[b]->EventIsMissing()) {
	unsigned int size = 4*boards[b]->GetEventSize();
	//output_stream_handle[CurrentOutputStream].write((const char*)boards[b]->Buffer(),size);
	writeSize = write(output_stream_handle[CurrentOutputStream],(const char*)boards[b]->Buffer(),size);
	if (writeSize != size) {
	  printf("ERROR - Unable to write board data to file. Data size: %d, Write result: %d\n",
		 size,writeSize);
	  exit(1);
	}
	output_stream_size[CurrentOutputStream] += size;
      }
    }

    // Update counters for this stream
    output_stream_nevents[CurrentOutputStream]++;

    // Move to next output stream (round robin)
    CurrentOutputStream++;
    if (CurrentOutputStream == NOutputStreams) CurrentOutputStream = 0;

    // Count event
    NumberOfEvents++;
    if (NumberOfEvents%100 == 0) printf("- Written %u events\n",NumberOfEvents);
    if (NumberOfEvents%cfg->DebugScale() == 0) {
      printf("  Event %7u size %7u time %10lu.%09lus clock %10llu status %04x trigger mask %02x fifo %02x auto %02x missing boards %08x\n",NumberOfEvents,total_event_size,now.tv_sec,now.tv_nsec,trig_clock,EventStatus,trigger->GetTriggerMask(),trigger->GetTriggerFifo(),trigger->GetTriggerAuto(),MissingADCBoards);
    }

  } // End of main while loop

  // We are done: send file tail to all output streams and close them
  for (unsigned int i=0; i<NOutputStreams; i++) {

    // Create file tail structure
    unsigned int fbuff;
    unsigned long long int lbuff;
    char file_tail[16]; // 4 words of 4 bytes
    unsigned int tail_size = 0;
    // Add size of tail to total size of output stream
    output_stream_size[i] += 16;
    // First line: end of file tag, #events written to stream
    fbuff = (EVENT_FILE_TAIL_TAG << 28) + ((output_stream_nevents[i] & 0x0fffffff) << 0);
    memcpy(file_tail+tail_size,&fbuff,4); tail_size += 4;
    // Second and third line: bytes written to stream (add size of tail)
    lbuff = output_stream_size[i];
    memcpy(file_tail+tail_size,&lbuff,8); tail_size += 8;
    // Fourth line: time when file is closed
    time_t t_close;
    time(&t_close);
    fbuff = (t_close & 0xffffffff);
    memcpy(file_tail+tail_size,&fbuff,4); tail_size += 4;

    printf("Sending tail structure to Level1 stream %d\n",i);

    // Write tail structure to stream
    //output_stream_handle[i].write(file_tail,tail_size);
    writeSize = write(output_stream_handle[i],file_tail,tail_size);
    if (writeSize != tail_size) {
      printf("ERROR - Unable to write stream tail to file. Tail size: %d, Write result: %d\n",
	     tail_size,writeSize);
      return 2;
    }

    // Close stream
    //output_stream_handle[i].close();
    if (close(output_stream_handle[i]) == -1) {
      printf("ERROR - Unable to close output stream '%s'.\n",output_stream[i].c_str());
      exit(1);
    };

  }

  // Record end-of-merging time
  time(&time_last);

  // At least one board or trigger reached EOR. Now wait for everybody else to do the same.
  int millisec = 100; // length of time to sleep between tests, in milliseconds
  unsigned int n_test_max = 100; // Number of tests to do before giving up
  struct timespec req = {0}; req.tv_sec = 0; req.tv_nsec = millisec * 1000000L;
  trigEOR = 0; nEOR = 0;
  unsigned int n_test = 0;
  while ( ( (! trigEOR) || nEOR<boards.size() ) && n_test<n_test_max ) {
    nanosleep(&req, (struct timespec *)NULL);
    trigEOR = 0;
    if ( ! trigger->NextEvent() ) trigEOR = 1;
    nEOR = 0;
    for(unsigned int b=0; b<boards.size(); b++) {
      if ( ! boards[b]->NextEvent() ) nEOR++;
    }
    n_test++;
  }
  if (nEOR<boards.size()) {
    printf("WARNING - Only %u boards over %lu reached End of Run status. Stopping anyway but please check the board logs\n",nEOR,boards.size());
  }
  if (! trigEOR) {
    printf("WARNING - Trigger did not reach End of Run status. Stopping anyway but please check the Trigger logs\n");
  }

  printf("Run %d closed after writing %u events in %lu s (%lu %lu)\n",cfg->RunNumber(),NumberOfEvents,time_last-time_first,time_last,time_first);

  unsigned long long int total_output_size = 0;
  for (unsigned int i=0; i<NOutputStreams; i++) {
    total_output_size += output_stream_size[i];
    double size_mib = output_stream_size[i]/(1024.*1024.);
    double event_rate = 0.;
    double data_rate = 0.;
    if (time_last-time_first) {
      event_rate = (1.*output_stream_nevents[i])/(time_last-time_first);
      data_rate = size_mib/(time_last-time_first);
    }
    printf("Out stream %2u Events %7u Data %11.1f MiB Rates %6.1f evt/s %7.3f MiB/s\n",i,output_stream_nevents[i],size_mib,event_rate,data_rate);
  }
  double size_mib = total_output_size/(1024.*1024.);
  double event_rate = 0.;
  double data_rate = 0.;
  if (time_last-time_first) {
    event_rate = (1.*NumberOfEvents)/(time_last-time_first);
    data_rate = size_mib/(time_last-time_first);
  }
  printf("Total         Events %7u Data %11.1f MiB Rates %6.1f evt/s %7.3f MiB/s\n",NumberOfEvents,size_mib,event_rate,data_rate);

  printf("DBINFO - %s - process_set_status %d\n",cfg->FormatTime(time(0)),DB_STATUS_FINISHED);
  printf("DBINFO - %s - process_set_time_stop %s\n",cfg->FormatTime(time(0)),cfg->FormatTime(time_last));
  printf("DBINFO - %s - process_set_total_events %d\n",cfg->FormatTime(time(0)),NumberOfEvents);
  printf("DBINFO - %s - process_set_total_size %llu\n",cfg->FormatTime(time(0)),total_output_size);

  // Show exit time
  printf("=== PadmeMerger exiting on %s UTC ===\n",cfg->FormatTime(time(0)));

  exit(0);
}
