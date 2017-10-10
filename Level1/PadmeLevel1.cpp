#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "ADCBoard.hh"
#include "DBService.hh"
#include "RootIO.hh"

int main(int argc, char* argv[])
{
  
  int c;
  std::string listfile = "";
  std::string outfile = "rawdata.root";
  std::string datadir = "data";
  int neventsperfile = 10000;
  int runnr = 0;
  int verbose = 0;
  int updatedb = 0;

  // Parse options
  while ((c = getopt (argc, argv, "n:d:r:l:o:v:uh")) != -1) {
    switch (c)
      {
      case 'r':
	if (runnr!=0) {
          fprintf (stderr, "Error while processing option '-r'. Multiple runs specified.\n");
          exit(1);
	}
        if ( sscanf(optarg,"%d",&runnr) != 1 ) {
          fprintf (stderr, "Error while processing option '-r'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        if (runnr<0) {
          fprintf (stderr, "Error while processing option '-r'. Run number set to %d (must be >=0).\n", runnr);
          exit(1);
        }
        fprintf(stdout,"Merging files from run %d\n",runnr);
        break;
      case 'n':
        if ( sscanf(optarg,"%d",&neventsperfile) != 1 ) {
          fprintf (stderr, "Error while processing option '-n'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        if (neventsperfile<0) {
          fprintf (stderr, "Error while processing option '-n'. Number of events must be >=0, found %d.\n", neventsperfile);
          exit(1);
        }
        fprintf(stdout,"Writing up to %d events per output file\n",neventsperfile);
        break;
      case 'd':
        datadir = optarg;
        fprintf(stdout,"Set input data directory to '%s'\n",datadir.c_str());
        break;
      case 'o':
        outfile = optarg;
        fprintf(stdout,"Set output data file to '%s'\n",outfile.c_str());
        break;
      case 'l':
        listfile = optarg;
        fprintf(stdout,"Data will be read from files listed in '%s'\n",listfile.c_str());
        break;
      case 'v':
        if ( sscanf(optarg,"%d",&verbose) != 1 ) {
          fprintf (stderr, "Error while processing option '-v'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        if (verbose<0) {
          fprintf (stderr, "Error while processing option '-v'. Verbose level set to %d (must be >=0).\n", verbose);
          exit(1);
        }
        fprintf(stdout,"Set verbose level to %d\n",verbose);
        break;
      case 'u':
        fprintf(stdout,"Enabling DB update\n");
	updatedb = 1;
	break;
      case 'h':
        fprintf(stdout,"\nPadmeDigi ([-r run_number]|[-l list_file]) [-d input files directory] [-o output root file] [-n events per file] [-v verbosity] [-h]\n\n");
        fprintf(stdout,"  -r: define run to process\n");
        fprintf(stdout,"  -l: define file with list of data files to process\n");
        fprintf(stdout,"      n.b. either -r or -l must be specified\n");
        fprintf(stdout,"  -u: update DB with total number of events merged (default: no DB update)\n");
        fprintf(stdout,"  -d: define directory where input files are located (default: \"%s\")\n",datadir.c_str());
        fprintf(stdout,"  -o: define an output file in root format (default: \"%s\")\n",outfile.c_str());
        fprintf(stdout,"  -n: define max number of events per output file (0=no limit, default: %d)\n",neventsperfile);
        fprintf(stdout,"  -v: define verbose level (default: %d)\n",verbose);
        fprintf(stdout,"  -h: show this help message and exit\n\n");
        exit(0);
      case '?':
        if (optopt == 'v') {
          // verbose with no argument: just enable at minimal level
          verbose = 1;
          break;
        } else if (optopt == 'r' || optopt == 'l' || optopt == 'd' || optopt == 'o' || optopt == 'n')
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

  // Verify that some input was specified
  if ( listfile.compare("")==0 && runnr==0 ) {
    fprintf (stderr,"No run number and no list file were specified. Exiting.");
    exit(1);
  }
  if ( listfile.compare("")!=0 && runnr!=0 ) {
    fprintf (stderr,"Both run number and list file were specified. Exiting.");
    exit(1);
  }

  ADCBoard* board;
  std::vector<ADCBoard*> boards;

  // If no list is specified, get board and file lists from DB
  if (listfile.compare("")==0) {

    // Get handle to DB
    DBService* db = DBService::GetInstance();

    // Get from DB list of board ids used in current run
    std::vector<int> boardList;
    int rc = db->GetBoardList(boardList,runnr);
    if (rc != DBSERVICE_OK) {
      printf("ERROR retrieving from DB list of files for run %d. Aborting\n",runnr);
      exit(1);
    }
    if (boardList.size() == 0) {
      printf("ERROR no boards associated to run %d found in DB. Aborting\n",runnr);
      exit(1);
    }

    // Create vector of boards
    for(unsigned int b=0; b<boardList.size(); b++) {

      printf("Board %d\tReading data from board id %d\n",b,boardList[b]);
      board = new ADCBoard(boardList[b]);
      board->SetVerbose(verbose);

      // Get list of files created for each board during run
      std::vector<std::string> fileList;
      rc = db->GetFileList(fileList,runnr,boardList[b]);
      if (rc != DBSERVICE_OK) {
	printf("ERROR retrieving from DB list of files for run %d board id %d. Aborting\n",runnr,boardList[b]);
	exit(1);
      }
      // Add files to board
      for(unsigned int f=0; f<fileList.size(); f++) {
	std::string filePath = datadir+"/"+fileList[f];
	printf("\tFile %d - Reading from file %s\n",f,filePath.c_str());
	board->AddFile(filePath);
      }

      // Add board to boards vector
      boards.push_back(board);
      
    }

  } else {

    // Get list of boards and files from file
    std::ifstream list;
    std::string line;
    int bid;
    char bfile[1024];
    list.open(listfile.c_str());
    while(!list.eof()){

      getline(list,line);
      if (line.compare("")!=0) {

	if ( sscanf(line.c_str(),"%d %s",&bid,bfile) != 2 ) {
	  printf("ERROR while parsing list '%s'. Aborting\n",listfile.c_str());
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
	  board->SetVerbose(verbose);
	  boards.push_back(board);
	}

	// Add file to board
	std::string filePath = datadir+"/"+bfile;
	printf("\tBoard %d - File %s\n",bid,filePath.c_str());
	board->AddFile(filePath);

      }

    }
    list.close();

  }

  // Show list of known boards/files
  printf("Reading %d board(s)\n",(int)boards.size());
  std::vector<ADCBoard*>::iterator it;
  for (it = boards.begin(); it != boards.end(); ++it) {
    board = *it;
    printf("Board %d Files %d\n",board->GetBoardId(),board->GetNFiles());
    for(int f=0; f<board->GetNFiles(); f++) printf("File %d %s\n",f,board->GetFileName(f).c_str());
  }
  
  // Connect to root services
  RootIO* root = new RootIO();
  root->SetVerbose(verbose);

  // Initialize root output file
  if ( root->Init(outfile,neventsperfile) != ROOTIO_OK ) {
    printf("ERROR while initializing root output. Aborting\n");
    exit(1);
  }

  // Loop over all events in files

  //UInt_t TT0[boards.size()];  // Time tag of first event in run
  UInt_t TTg[boards.size()];  // Time tag of last good event
  UInt_t TT[boards.size()];   // Time tag of current event
  Int_t  dT[boards.size()];   // dT wrt last good event
  Int_t  dTB0[boards.size()]; // dT wrt board 0 in current event

  int eventnr = 0;
  while(1){

    if (verbose>=1) printf("=== Processing event %8d ===\n",eventnr);

    // Load next event for all boards
    UInt_t nEOR = 0;
    for(unsigned int b=0; b<boards.size(); b++) {
      if ( boards[b]->NextEvent() == 0 ) {
	nEOR++;
	printf("*** Board %2d - End of Run.\n",boards[b]->GetBoardId());
      }
    }
    if (nEOR != 0) {
      if (nEOR == boards.size()) {
	printf("All boards reached end of run.\n");
      } else {
	printf("WARNING: only %d board(s) reached end of run.\n",nEOR);
      }
      break; // Exit from main loop
    }

    nEOR = 0;
    UInt_t in_time = 0;
    while ( (! in_time) && (nEOR==0) ) {

      // Get timing information
      UInt_t bmax = 0;
      for(unsigned int b=0; b<boards.size(); b++) {
	TT[b]  = boards[b]->Event()->GetTriggerTimeTag(0);
	if(eventnr==0) {
	  //TT0[b] = TT[b];
	  TTg[b] = TT[b]; // Event 0 is good by definition
	}
	dT[b] = TT[b]-TTg[b];
	if (dT[b]<0) dT[b] += (1<<30);
	if (dT[b]>dT[bmax]) bmax = b;
	dTB0[b] = TT[b]-TT[0];

	if (verbose>=2) {
	  if (b==0) {
	    printf("- Board %2d NEv %8u Tabs %f (0x%08x) Dt %f (0x%08x)\n",b,boards[b]->Event()->GetEventCounter(),TT[b]*8.5E-9,TT[b],dT[b]*8.5E-9,dT[b]);
	  } else {
	    printf("- Board %2d NEv %8u Tabs %f (0x%08x) Dt %f (0x%08x) DtB0 %f (0x%08x)\n",b,boards[b]->Event()->GetEventCounter(),TT[b]*8.5E-9,TT[b],dT[b]*8.5E-9,dT[b],dTB0[b]*8.5E-9,dTB0[b]);
	  }
	}

      }

      // Verify if all events are in time (0x1000 clock cycles tolerance)
      in_time = 1;
      for(unsigned int b=0; b<boards.size(); b++) {
	if ( (dT[bmax]-dT[b]) >= 0x1000 ) {
	  in_time = 0;
	  printf("*** Board %2d - Skipping event %8u\n",boards[b]->GetBoardId(),boards[b]->Event()->GetEventCounter());
	  if ( boards[b]->NextEvent() == 0 ) {
	    printf("*** Board %2d - End of Run.\n",boards[b]->GetBoardId());
	    nEOR++;
	  }
	}
      }

    }

    // If one or more files reached EOR, stop processing events
    if (nEOR != 0) {
      if (nEOR == boards.size()) {
	printf("All boards reached end of run.\n");
      } else {
	printf("WARNING: only %d board(s) reached end of run.\n",nEOR);
      }
      break; // Exit from main loop
    }

    // Tag current set of times ad "good"
    for (unsigned int b=0; b<boards.size(); b++) {
      TTg[b] = TT[b];
    }

    // The event is complete: copy structure to TRawEvent and send to output file
    if (root->FillRawEvent(runnr,eventnr,boards) != ROOTIO_OK) {
      printf("ERROR while writing event %d to root file. Aborting\n",eventnr);
      exit(1);
    }

    // Count event
    eventnr++;
  }

  // Finalize root file
  if (root->Exit() != ROOTIO_OK) {
      printf("ERROR while finalizing root file. Aborting\n");
      exit(1);
  }

  // If input was from a real run, finalize output and update DB if required
  if (listfile.compare("")==0) {

    printf("Run %d closed after writing %d events\n",runnr,eventnr);

    if (updatedb == 0) {

      printf("Option -u was not specified: DB not updated.\n");

    } else {

      // Get handle to DB
      DBService* db = DBService::GetInstance();

      // Update DB with number of events for this run
      int n_events;
      int rc = db->GetRunEvents(n_events,runnr);
      if (rc != DBSERVICE_OK) {
	printf("ERROR retrieving from DB old number of events for run %d. Aborting\n",runnr);
	exit(1);
      }
      if (n_events == eventnr) {
	printf("Run %d Events %d. DB is up-to-date: no action.\n",runnr,eventnr);
      } else {
	if (n_events == 0) {
	  printf("Run %d Events %d. Writing number of events to DB.\n",runnr,eventnr);
	} else {
	  printf("Run %d Events %d. WARNING - DB returns %d events: updating DB.\n",runnr,eventnr,n_events);
	}
	int rc = db->UpdateRunEvents(eventnr,runnr);
	if (rc != DBSERVICE_OK) {
	  printf("ERROR updating DB for run %d. Aborting\n",runnr);
	  exit(1);
	}
      }

    }

  } else {

    printf("A total of %d merged events were written to the output file\n",eventnr);

  }

  exit(0);
}
