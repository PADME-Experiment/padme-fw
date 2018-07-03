#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <cmath>

#include "EventTags.hh"
#include "ADCEventTags.hh"
#include "ADCBoard.hh"
#include "Configuration.hh"
#include "DBService.hh"

#define LVL1_PROC_MAX 16

int main(int argc, char* argv[])
{

  int rc; // DB library retrun code

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
  list.close();

  // Show list of known boards/files and initialize them
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

  int dT; // Clock counter difference wrt previous event: can be negative if clock counter rolled over
  int dTb; // Clock counter difference between two groups of the same board
  unsigned int TT_evt,TT_grp[ADCEVENT_NTRIGGERS]; // Clock counters for board and for group
  unsigned int TT_old[boards.size()]; // Clock counter at previous event
  unsigned long long int CC[boards.size()]; // Total Clock Counts since start of run

  unsigned char groupMaskRef[boards.size()]; // Reference group masks from event 0

  unsigned int nEOR;
  unsigned int NumberOfEvents = 0;
  while(1){

    if (cfg->Verbose()>=1)
      printf("=== Processing event %8u ===\n",NumberOfEvents);

    nEOR = 0; // Reset end_of_run counter

    // Load next event for all boards
    for(unsigned int b=0; b<boards.size(); b++) {
      if ( ! boards[b]->NextEvent() ) {
	nEOR++;
	printf("*** Board %2d - End of Run.\n",boards[b]->GetBoardId());
      }
    }
    if (nEOR != 0) {
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

    unsigned int all_in_time = 0;
    while ( (! all_in_time) && (! nEOR) ) {

      unsigned int bmax = 0; // Find board with largest time since start of run 

      // Get timing information for current event of each board
      for(unsigned int b=0; b<boards.size(); b++) {

	unsigned int board_in_time = 0;
	unsigned int board_EOR = 0;
	while ( (! board_in_time) && (! board_EOR) ) {

	  board_in_time = 1;

	  // First check if board's group mask is consistent with that of first event (do we need this?)
	  unsigned char grMsk = boards[b]->GetGroupMask();
	  if (NumberOfEvents == 0 ) {
	    groupMaskRef[b] = grMsk;
	  } else {
	    if (grMsk != groupMaskRef[b]) {
	      board_in_time = 0;
	      printf("*** Board %2d - Inconsistent group mask (exp: 0x%1x fnd: 0x%1x) - Skipping event %8u\n",boards[b]->GetBoardId(),groupMaskRef[b],grMsk,boards[b]->GetEventCounter());
	      if ( boards[b]->NextEvent() == 0 ) {
		printf("*** Board %2d - End of Run.\n",boards[b]->GetBoardId());
		board_EOR = 1;
	      }
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

	    // Check if this is the highest time in the current set of board times
	    if (CC[b]>CC[bmax]) bmax = b;

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

      } else {

	// Verify if all boards are in time (0x0010 clock cycles tolerance i.e. ~136ns)
	all_in_time = 1;
	for(unsigned int b=0; b<boards.size(); b++) {
	  if ( CC[bmax]-CC[b] >= 0x0010 ) {
	    // This board is still on an earlier event: skip to next event
	    all_in_time = 0;
	    printf("*** Board %2d - External mismatch - Skipping event %8u\n",boards[b]->GetBoardId(),boards[b]->Event()->GetEventCounter());
	    if ( boards[b]->NextEvent() == 0 ) {
	      printf("*** Board %2d - End of Run.\n",boards[b]->GetBoardId());
	      nEOR++;
	    }
	  }
	}

      }

    } // End of while loop checking for in-time events

    // If one or more files reached EOR, stop processing events
    if (nEOR) {
      printf("WARNING: %d board(s) reached end of run because of time mismatches (see previous messages).\n",nEOR);
      break; // Exit from main loop
    }

    // The event is complete and in time: send all data to next Level1 process

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
    // For the moment save clock counts of first board as event run time.
    // When trigger info will be added, replace this with number of ns since start of run
    memcpy(evt_h+evt_h_size,CC,8); evt_h_size += 8;
    // Trigger mask will come from trigger controller
    // For now assume all triggers are from BTF (bit0 in trig mask)
    evt_h_buff = 0x00000001;
    memcpy(evt_h+evt_h_size,&evt_h_buff,4); evt_h_size += 4;
    // Event status not defined yet: set it to 0
    evt_h_buff = 0;
    memcpy(evt_h+evt_h_size,&evt_h_buff,4); evt_h_size += 4;

    // Send event header to current output stream
    output_stream_handle[CurrentOutputStream].write(evt_h,evt_h_size);
    output_stream_size[CurrentOutputStream] += evt_h_size;

    // Create trigger data structure
    // These data will come from the trigger controller. For now set to 0
    char trg_h[4*3];
    unsigned int trg_h_size = 0;
    unsigned int trg_h_buff;
    unsigned int trigger_mask = 0;
    unsigned int trigger_count = 0;
    unsigned long long int trigger_clock = 0;
    trg_h_buff = (EVENT_TRIGGER_INFO_TAG << 28) + ((trigger_mask & 0x00000fff) << 16) + ((trigger_count & 0x0000ffff) << 0);
    memcpy(trg_h+trg_h_size,&trg_h_buff,4); trg_h_size += 4;
    memcpy(trg_h+trg_h_size,&trigger_clock,8); trg_h_size += 8;

    // Send trigger data to current output stream
    output_stream_handle[CurrentOutputStream].write(trg_h,trg_h_size);
    output_stream_size[CurrentOutputStream] += trg_h_size;

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

  // At least one board reached EOR. Now wait for all boards to do the same.
  int millisec = 100; // length of time to sleep between tests, in milliseconds
  struct timespec req = {0}; req.tv_sec = 0; req.tv_nsec = millisec * 1000000L;
  while (nEOR<boards.size()) {
    nanosleep(&req, (struct timespec *)NULL);  
    nEOR = 0;
    for(unsigned int b=0; b<boards.size(); b++) {
      if ( ! boards[b]->NextEvent() ) nEOR++;
    }
  }

  printf("Run %d closed after writing %u events\n",cfg->RunNumber(),NumberOfEvents);

  unsigned long long int total_output_size = 0;
  for (unsigned int i=0; i<NOutputStreams; i++) {
    total_output_size += output_stream_size[i];
    printf("Out stream %2u Events %7u Data %10llu\n",i,output_stream_nevents[i],output_stream_size[i]);
  }
  printf("Total         Events %7u Data %10llu\n",NumberOfEvents,total_output_size);

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

  exit(0);
}
