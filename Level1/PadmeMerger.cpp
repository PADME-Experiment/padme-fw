#include <stdlib.h>
#include <stdio.h>
#include <time.h>
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
#define INTIME_TOLERANCE_NS 10

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
  std::ofstream output_stream_handle[LVL1_PROC_MAX];
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
      output_stream_handle[NOutputStreams].open(line.c_str(),std::ios::out | std::ios::binary);
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
  while(1){

    if (cfg->Verbose()>=1)
      printf("=== Processing event %8u ===\n",NumberOfEvents);

    // Get next trigger event
    trigEOR = 0;
    if ( ! trigger->NextEvent() ) {
      trigEOR = 1;
      printf("*** Trigger - End of Run.\n");
    }

    // Load next event for all boards
    nEOR = 0;
    for(unsigned int b=0; b<boards.size(); b++) {
      if ( ! boards[b]->NextEvent() ) {
	nEOR++;
	printf("*** Board %2d - End of Run.\n",boards[b]->GetBoardId());
      }
    }

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
      // (data format version and board serial numbers)

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
      // Boards serial numbers
      for(unsigned int i=0; i<boards.size(); i++) {
	printf("Board %u has SN %u\n",i,boards[i]->GetSerialNumber());
	fbuff = (boards[i]->GetSerialNumber() & 0xffffffff);
	memcpy(file_header+header_size,&fbuff,4);
	header_size += 4;
      }
      // Last line: time when file is opened
      time_t t_open;
      time(&t_open);
      fbuff = (t_open & 0xffffffff);
      memcpy(file_header+header_size,&fbuff,4); header_size += 4;

      // Send header to all output files
      for(unsigned int i = 0; i<NOutputStreams; i++) {
	output_stream_handle[i].write(file_header,header_size);
	output_stream_size[i] += header_size;
      }

    }

    /*
    // Compute trigger time for this event
    if (NumberOfEvents == 0) {
      trigCount = trigger->GetTriggerTime();
      trigCountOld = trigCount;
      trigTotalCount = 0;
    } else {
      trigCount = trigger->GetTriggerTime(); // Get current clock counter
      trig_dT = trigCount-trigCountOld; // Get difference with previous clock counter
      if (trig_dT<0) trig_dT += (1<<40); // Compensate for clock counter roll over (NEED VERIFICATION!)
      trigTotalCount += trig_dT; // Add time difference to total clock counter (64bits -> 2.3E11s @ 80MHz)
      trigCountOld = trigCount; // Save current clock counter
    }

    unsigned int all_in_time = 0;
    nEOR = 0; trigEOR = 0;
    while ( (! all_in_time) && (! nEOR) && (! trigEOR) ) {

      //unsigned int bmax = 0; // Find board with largest time since start of run 

      // Get timing information for current event of each board
      for(unsigned int b=0; b<boards.size(); b++) {

	unsigned int board_in_time = 0;
	unsigned int board_EOR = 0;
	while ( (! board_in_time) && (! board_EOR) ) {

	  board_in_time = 1;

	  // First check if board's group mask is consistent with that of first event (do we need this?)
	  unsigned char grMsk = boards[b]->GetGroupMask();
	  if (NumberOfEvents == 0 ) groupMaskRef[b] = grMsk;
	  if (grMsk != groupMaskRef[b]) {
	    board_in_time = 0;
	    printf("*** Board %2d - Inconsistent group mask (exp: 0x%1x fnd: 0x%1x) - Skipping event %8u\n",boards[b]->GetBoardId(),groupMaskRef[b],grMsk,boards[b]->GetEventCounter());
	    if ( boards[b]->NextEvent() == 0 ) {
	      printf("*** Board %2d - End of Run.\n",boards[b]->GetBoardId());
	      board_EOR = 1;
	    }
	  }

	  // Check if board internal times are aligned. Stop the run if they are not
	  // This may change when all time counters are correctly aligned by S/IN signal
	  TT_evt = 0;
	  boards[b]->GetTriggerTimeTags(TT_grp);
	  unsigned int first_active_group = 1;
	  for (unsigned int g=0; g<4; g++) {
	    if (grMsk & 1<<g) {
	      if (first_active_group) { // This is the first active group: get its time as reference for event
		TT_evt = TT_grp[g];
		first_active_group = 0;
	      } else {
		// Other groups are present: verify they are all in time (0x0010 clock cycles tolerance i.e. ~136ns)
		dTb = TT_evt-TT_grp[g];
		if ( std::abs(dTb) >= 0x0010 ) {
		  if (NumberOfEvents == 0) { // If this happens on first event stop the run
		    printf("*** FATAL ERROR - Board %2d - Internal time mismatch at Start of Run\n",boards[b]->GetBoardId());
		    //root->Exit();
		    exit(1);
		  } else {
		    board_in_time = 0;
		    printf("*** Board %2d - Internal time mismatch - Skipping event %8u\n",boards[b]->GetBoardId(),boards[b]->GetEventCounter());
		    if ( boards[b]->NextEvent() == 0 ) {
		      printf("*** Board %2d - End of Run.\n",boards[b]->GetBoardId());
		      board_EOR = 1;
		    }
		    break; // Break loop on trigger groups
		  }
		}
	      }
	    }
	  }

	  // If board reached EOR, stop processing this board
	  if (board_EOR) {

	    printf("WARNING: board %d reached end of run because of internal mismatches.\n",b);

	  } else {

	    if (NumberOfEvents == 0) {

	      // Board is in time: save current counter and start total time from it
	      dT = 0;
	      CC[b] = 0;
	      TT_old[b] = TT_evt;

	    } else {

	      // Board is in time: update its total clock counter
	      dT = TT_evt-TT_old[b]; // Get time interval since last readout
	      if (dT<0) dT += (1<<30); // Check if clock counter rolled over
	      CC[b] += dT; // Add latest time interval to global clock counter
	      TT_old[b] = TT_evt;

	    }

	    //// Check if this is the highest time in the current set of board times
	    //if (CC[b]>CC[bmax]) bmax = b;

	    if (cfg->Verbose()>=2) {
	      printf("- Board %2d NEv %8u Dt %f (0x%08x) T %f (0x%016llx)\n",
		     b,boards[b]->GetEventCounter(),dT*8.5E-9,dT,CC[b]*8.5E-9,CC[b]);
	    }

	  }

	} // End on loop to get single board in-time event

	if (board_EOR) nEOR++;

      
      } // End of loop over boards

      if (nEOR != 0) {

	// If one or more boards reached EOR, there is no need to continue
	printf("WARNING: %d board(s) reached end of run because of time mismatches (see previous messages).\n",nEOR);
	break; // Exit while loop checking for in-time events

      }
    */

      //// Verify if all boards are in time (0x0010 clock cycles tolerance i.e. ~136ns)
      //all_in_time = 1;
      //for(unsigned int b=0; b<boards.size(); b++) {
      //	if ( CC[bmax]-CC[b] >= 0x0010 ) {
      //	  // This board is still on an earlier event: skip to next event
      //	  all_in_time = 0;
      //	  printf("*** Board %2d - External mismatch - Skipping event %8u\n",boards[b]->GetBoardId(),boards[b]->Event()->GetEventCounter());
      //	  if ( boards[b]->NextEvent() == 0 ) {
      //	    printf("*** Board %2d - End of Run.\n",boards[b]->GetBoardId());
      //	    nEOR++;
      //	  }
      //	}
      //}

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

    // If one or more files reached EOR, stop processing events
    if (nEOR || trigEOR) {
      printf("WARNING: %d board(s) reached end of run because of time mismatches (see previous messages).\n",nEOR);
      break; // Exit from main loop
    }

    // The event is complete and in time: send all data to next Level1 process

    // Merging time is counted from the first in-time event
    if (NumberOfEvents == 0) time(&time_first);

    // Compute total size of event in 4-bytes words
    UInt_t total_event_size = 8+3; // Header and trigger info
    // Add size of all boards
    for (unsigned int b=0; b<boards.size(); b++) {
      total_event_size += boards[b]->GetEventSize();
    }

    // Get precise system time
    struct timespec now;
    if( clock_gettime(CLOCK_REALTIME,&now) == -1 ) {
      perror("clock gettime");
      exit(EXIT_FAILURE);
    }

    // Create event header structure
    char evt_h[4*8];
    unsigned int evt_h_size = 0;
    unsigned int evt_h_buff;
    evt_h_buff = (EVENT_HEADER_TAG << 28) + (total_event_size & 0x0fffffff);
    memcpy(evt_h+evt_h_size,&evt_h_buff,4); evt_h_size += 4;
    memcpy(evt_h+evt_h_size,&NumberOfEvents,4); evt_h_size += 4;
    //evt_h_buff = (unsigned int)now.tv_sec;
    //memcpy(evt_h+evt_h_size,&evt_h_buff,4); evt_h_size += 4;
    //evt_h_buff = (unsigned int)now.tv_nsec;
    //memcpy(evt_h+evt_h_size,&evt_h_buff,4); evt_h_size += 4;
    unsigned int now_sec = (now.tv_sec & 0xffffffff);
    memcpy(evt_h+evt_h_size,&now_sec,4); evt_h_size += 4;
    unsigned int now_nsec = (now.tv_nsec & 0xffffffff);
    memcpy(evt_h+evt_h_size,&now_nsec,4); evt_h_size += 4;
    // Run Time is the number of 80MHz clock cycles since the beginning of the run
    unsigned long int trig_clock = trigger->GetTotalClockCounter();
    memcpy(evt_h+evt_h_size,&trig_clock,8); evt_h_size += 8;
    // Trigger mask currently contains a copy of the 6bits mask from the trigger
    evt_h_buff = trigger->GetTriggerMask();
    memcpy(evt_h+evt_h_size,&evt_h_buff,4); evt_h_size += 4;
    // Event status not defined yet: set it to 0
    evt_h_buff = 0;
    memcpy(evt_h+evt_h_size,&evt_h_buff,4); evt_h_size += 4;

    // Send event header to current output stream
    output_stream_handle[CurrentOutputStream].write(evt_h,evt_h_size);
    output_stream_size[CurrentOutputStream] += evt_h_size;

    // Copy trigger event structure to output (3 words, no need to change anything)
    output_stream_handle[CurrentOutputStream].write((const char*)trigger->Buffer(),4*3);
    output_stream_size[CurrentOutputStream] += 4*3;

    // Loop over all boards and send its event data to output stream
    for(unsigned int b=0; b<boards.size(); b++) {
      unsigned int size = 4*boards[b]->GetEventSize();
      output_stream_handle[CurrentOutputStream].write((const char*)boards[b]->Buffer(),size);
      output_stream_size[CurrentOutputStream] += size;
    }

    // Update counters for this stream
    output_stream_nevents[CurrentOutputStream]++;

    // Move to next output stream (round robin)
    CurrentOutputStream++;
    if (CurrentOutputStream == NOutputStreams) CurrentOutputStream = 0;

    // Count event
    NumberOfEvents++;
    if (NumberOfEvents%100 == 0) printf("- Written %u events\n",NumberOfEvents);

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
    output_stream_handle[i].write(file_tail,tail_size);

    // Close stream
    output_stream_handle[i].close();

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
