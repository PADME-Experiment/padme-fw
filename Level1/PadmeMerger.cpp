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

void fmt_time(char buf[20],time_t* t)
{
  struct tm* tgm = gmtime(t);
  sprintf(buf,"%04d-%02d-%02d %02d:%02d:%02d",1900+tgm->tm_year,tgm->tm_mon+1,tgm->tm_mday,tgm->tm_hour,tgm->tm_min,tgm->tm_sec);
}

int main(int argc, char* argv[])
{

  int rc; // DB library retrun code

  time_t time_start, time_stop, time_first, time_last;
  char t_fmt[20]; // Formatted time string "YYYY-MM-DD hh:mm:ss"

  // Set standard output/error in unbuffered mode
  setbuf(stdout,NULL);
  setbuf(stderr,NULL);

  time(&time_start); fmt_time(t_fmt,&time_start);
  printf("=== PadmeMerger starting on %s UTC ===\n",t_fmt);
  
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
  //std::string raw_file_header = cfg->RawFileHeader();
  //unsigned int n_events_per_file = cfg->NEventsPerFile();
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

  // Check if input file list was defined
  if (cfg->InputStreamList().compare("")==0) {
    printf("ERROR no input file list defined. Aborting\n");
    exit(1);
  }

  // If this is an official run, connect to DB and get id of merger
  if (cfg->RunNumber()) {

    // Get handle to DB
    DBService* db = DBService::GetInstance();

    // Get id of merger for future DB accesses
    int merger_id = 0;
    rc = db->GetMergerId(merger_id,cfg->RunNumber());
    if (rc != DBSERVICE_OK) {
      printf("ERROR retrieving from DB id of merger process for run %d. Aborting\n",cfg->RunNumber());
      exit(1);
    }
    cfg->SetMergerId(merger_id);

  }

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
  unsigned long int output_stream_size[LVL1_PROC_MAX];
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

  // Everything is set: tell DB merger has started
  if (cfg->RunNumber()) {

    // Get handle to DB
    DBService* db = DBService::GetInstance();

    // Update merger status
    rc = db->SetMergerStatus(2,cfg->MergerId());
    if (rc != DBSERVICE_OK) {
      printf("ERROR setting merger status in DB. Aborting\n");
      exit(1);
    }

    // Update merger start time
    rc = db->SetMergerTime("START",cfg->MergerId());
    if (rc != DBSERVICE_OK) {
      printf("ERROR setting merger start time in DB. Aborting\n");
      exit(1);
    }

  }

  unsigned int CurrentOutputStream = 0; // First event will be sent to first output stream

  // Loop over all events in files

  unsigned long int boardTime, trigTime; // Total Board/Trigger time since start of run in ns
  long int dT_board_trig; // Time difference between board and trigger
  //unsigned long int trigIncrement; // Time increment since previous trigger
  unsigned int nADCClockReset; // Number of ADC clock resets since last trigger
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

    // Verify if all boards are in time with the trigger (INTIME_TOLERANCE_NS is the time tolerance in ns)
    trigTime = lrint(trigger->GetTotalClockCounter()/TRIGGER_CLOCK_FREQUENCY); // Trigger time in ns

    // Check if there was a long pause in DAQ and compute how many times the ADC 30bit clock counter did a reset
    nADCClockReset = lrint(trigger->GetClockIncrement()*(V1742_CLOCK_FREQUENCY/TRIGGER_CLOCK_FREQUENCY)/(1LL<<30));

    // If there was a long delay in the triggers, ADC clock counters could reset multiple times
    // Check if we can recover the timing with all the boards
    if (nADCClockReset > 0) {

      printf("Long trigger pause of %lu ns: trying to recover sinchronization",lrint(trigger->GetClockIncrement()/TRIGGER_CLOCK_FREQUENCY));
      printf("ADC clock resets since last trigger: %d\n",nADCClockReset);

      unsigned int n_not_recovered = 0;
      for(unsigned int b=0; b<boards.size(); b++) {

	unsigned int n_resets_added = 0;
	unsigned int recovered = 0;
	unsigned long int board_count = boards[b]->GetTotalClockCounter();
	while(recovered == 0) {

	  boardTime = lrint(board_count/V1742_CLOCK_FREQUENCY);
	  dT_board_trig = boardTime-trigTime;
	  printf("Board %2d - Trigger %lu ns - Board %lu ns - dT %ld ns\n",boards[b]->GetBoardId(),trigTime,boardTime,dT_board_trig);
	  if (dT_board_trig < -INTIME_TOLERANCE_NS) {
	    board_count += (1LL<<30); // Add a clock reset period (~9s)
	    if (++n_resets_added > nADCClockReset) {
	      printf("ERROR - After %d clock counter resets board %2d time still behind trigger time.\n",n_resets_added,boards[b]->GetBoardId());
	      break;
	    }
	  } else if (dT_board_trig > INTIME_TOLERANCE_NS) {
	    printf("ERROR - Board %2d lost synchronization with trigger.\n",boards[b]->GetBoardId());
	    break;
	  } else {
	    printf("After %d clock counter resets, board %2d recovered synchronization with trigger.\n",n_resets_added,boards[b]->GetBoardId());
	    boards[b]->SetTotalClockCounter(board_count);
	    recovered = 1;
	  }

	}
	if (recovered == 0) n_not_recovered++;

      }
      if (n_not_recovered > 0) {
	printf("ERROR - %d boards lost synchronization with trigger: RUN MUST BE STOPPED.\n",n_not_recovered);
	break; // Exit main loop and wait for run to finish
      }
    }

    nEOR = 0;
    for(unsigned int b=0; b<boards.size(); b++) {

      boardTime = lrint(boards[b]->GetTotalClockCounter()/V1742_CLOCK_FREQUENCY);
      dT_board_trig = boardTime-trigTime;

      //if (cfg->Verbose()>=2)
	printf("Board %2d Evt %7u Time %16lu --- Trigger Evt %4u Time %16lu --- dT %10ld\n",
	       boards[b]->GetBoardId(),boards[b]->GetEventCounter(),boardTime,trigger->GetTriggerCounter(),trigTime,dT_board_trig);

      while (dT_board_trig < -INTIME_TOLERANCE_NS) {
	// Board time is earlier than trigger time: this happens when trigger controller does not send a packet
	printf("*** Board %2d - Board time %lu less than Trigger time %lu: skip event and try to recover\n",boards[b]->GetBoardId(),boardTime,trigTime);
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
	printf("*** Board %2d - Board time %lu greater than Trigger time %lu: setting board as MISSING in this event\n",boards[b]->GetBoardId(),boardTime,trigTime);
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
    unsigned long int trig_clock = trigger->GetTotalClockCounter();
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

    //clock_gettime(CLOCK_REALTIME,&sys_time);
    //printf("After output   %ld.%09ld\n",sys_time.tv_sec,sys_time.tv_nsec);

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
    unsigned long int lbuff;
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
  //unsigned int n_test_max = 100; // Number of tests to do before giving up
  unsigned int n_test_max = 10000; // Number of tests to do before giving up
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

  unsigned long int total_output_size = 0;
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

  // If input was from a real run, update DB
  if (cfg->RunNumber()) {

    // Get handle to DB
    DBService* db = DBService::GetInstance();

    // Update merger status
    rc = db->SetMergerStatus(3,cfg->MergerId());
    if (rc != DBSERVICE_OK) {
      printf("ERROR setting merger status in DB. Aborting\n");
      exit(1);
    }

    // Update merger stop time
    rc = db->SetMergerTime("STOP",cfg->MergerId());
    if (rc != DBSERVICE_OK) {
      printf("ERROR setting merger stop time in DB. Aborting\n");
      exit(1);
    }

    // Update DB with final counters (files created, events written, data written)
    //rc = db->UpdateMergerInfo(root->GetTotalFiles(),root->GetTotalEvents(),root->GetTotalSize(),cfg->MergerId());
    //if (rc != DBSERVICE_OK) {
    //  printf("ERROR updating DB with number of files (n=%u) number of events (n=%u) and output size (size=%lu) for merger id %d. Aborting\n",root->GetTotalFiles(),root->GetTotalEvents(),root->GetTotalSize(),cfg->MergerId());
    //  exit(1);
    //}

  }

  // Show exit time
  time(&time_stop); fmt_time(t_fmt,&time_stop);
  printf("=== PadmeMerger exiting on %s UTC ===\n",t_fmt);

  exit(0);
}
