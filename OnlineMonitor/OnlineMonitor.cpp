#include "Riostream.h"
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObjArray.h"

#include "TRawEvent.hh"

#include "Configuration.hh"
#include "InputHandler.hh"
#include "ECalMonitor.hh"
#include "TargetMonitor.hh"

int main(int argc, char* argv[])
{
  
  int c;

  TString dataDirectory = "";
  TString runName = "";
  TString outputFileName = "OnlineMonitor.txt";
  TString configFileName = "OnlineMonitor.cfg";
  TString stopFileName = "OnlineMonitor.stop";
  UInt_t nStreams = 0;
  UInt_t verbose = 0;
  UInt_t nEventsToProcess = 0;
  Int_t debugScale = -1;

  struct timespec now;
  
  // Connect to configuration handler
  Configuration* cfg = Configuration::GetInstance();

  // Parse options
  while ((c = getopt(argc, argv, "R:D:S:o:n:v:s:d:c:frh")) != -1) {
    switch (c)
      {
      case 'R':
	runName = optarg;
	break;
      case 'D':
	dataDirectory = optarg;
	break;
      case 'o':
        outputFileName = optarg;
	break;
      case 'S':
        if ( sscanf(optarg,"%u",&nStreams) != 1 ) {
          fprintf (stderr, "Error while processing option '-S'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        if ( nStreams < 1 || nStreams > cfg->NumberOfStreamsMax() ) {
          fprintf (stderr, "Error while processing option '-S'. Required %d streams (must be 1<=S<=%u).\n",nStreams,cfg->NumberOfStreamsMax());
          exit(1);
        }
        break;
      case 'n':
        if ( sscanf(optarg,"%d",&nEventsToProcess) != 1 ) {
          fprintf (stderr, "Error while processing option '-n'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        if (nEventsToProcess<0) {
          fprintf (stderr, "Error while processing option '-n'. Required %d events (must be >=0).\n", nEventsToProcess);
          exit(1);
        }
        break;
      case 'd':
        if ( sscanf(optarg,"%d",&debugScale) != 1 ) {
          fprintf (stderr, "Error while processing option '-d'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        if (debugScale<0) {
          fprintf (stderr, "Error while processing option '-d': required %d, must be >=0.\n", debugScale);
          exit(1);
        }
        break;
      case 'f':
	cfg->EnableFollowMode();
	break;
      case 'r':
	cfg->EnableResumeMode();
	break;
      case 'c':
        configFileName = optarg;
	break;
      case 's':
        stopFileName = optarg;
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
        break;
      case 'h':
        fprintf(stdout,"\nOnlineMonitor -R run_name [-D top rawdata path] [-S streams] [-o output file] [-f] [-s stop file] [-n events] [-v verbose] [-h]\n\n");
        fprintf(stdout,"  -R: define name of run to process\n");
        fprintf(stdout,"  -D: define path to top rawdata directory\n");
        fprintf(stdout,"  -S: define number of streams to use (1 to %u) path to top rawdata directory\n",cfg->NumberOfStreamsMax());
        fprintf(stdout,"  -o: define name of PadmeMonitor output file\n");
        fprintf(stdout,"  -n: define number of events to process (0: all events)\n");
        fprintf(stdout,"  -f: enable FOLLOW mode\n");
        fprintf(stdout,"  -r: enable RESUME mode\n");
        fprintf(stdout,"  -s: define name of control file to stop program when in FOLLOW mode\n");
        fprintf(stdout,"  -c: define name of configuration file\n");
        fprintf(stdout,"  -v: define verbose level\n");
        fprintf(stdout,"  -h: show this help message and exit\n\n");
        exit(0);
      case '?':
        if (optopt == 'v') {
          // verbose with no argument: increase verbose level by 1
          verbose++;
          break;
        } else if (optopt == 'R' || optopt == 'D' || optopt == 'S' || optopt == 's' || optopt == 'n' || optopt == 'o')
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

  // Save configuration parameters for input handler
  if (! runName.IsNull()) cfg->SetRunName(runName);
  if (! dataDirectory.IsNull()) cfg->SetDataDirectory(dataDirectory);
  if (nStreams) cfg->SetNumberOfStreams(nStreams);
  if (! stopFileName.IsNull()) cfg->SetStopFile(stopFileName);
  if (debugScale != -1) cfg->SetDebugScale(debugScale);
  fprintf(stdout,"- Run name: '%s'\n",cfg->RunName().Data());
  fprintf(stdout,"- Rawdata top directory: '%s'\n",cfg->DataDirectory().Data());
  fprintf(stdout,"- Number of streams: '%u'\n",cfg->NumberOfStreams());
  if (cfg->FollowMode()) {
    fprintf(stdout,"- Follow mode enabled\n");
    if (cfg->ResumeMode()) fprintf(stdout,"- Resume mode enabled\n");
    fprintf(stdout,"- Stop file: '%s'\n",cfg->StopFile().Data());
  }
  if (cfg->DebugScale() == 0) {
    fprintf(stdout,"- Debug printout is OFF\n");
  } else {
    fprintf(stdout,"- Debug printout every %d events\n",cfg->DebugScale());
  }

  // Set number of events to process
  if (nEventsToProcess) {
    fprintf(stdout,"- Process first %u events in stream(s)\n",nEventsToProcess);
  } else {
    fprintf(stdout,"- Process all events in stream(s)\n");
  }

  // Set and create output file
  if (! outputFileName.IsNull()) cfg->SetOutputFile(outputFileName);
  fprintf(stdout,"- Output PadmeMonitor file: '%s'\n",cfg->OutputFile().Data());
  FILE* outf = fopen(cfg->OutputFile(),"w");
  fprintf(outf,"# OnlineMonitor - %s\n",cfg->FormatTime(time(0)));
  fclose(outf);

  // Set general verbose level
  if (verbose>0) fprintf(stdout,"- Verbose level: %u\n",verbose);
  cfg->SetVerbose(verbose);

  // Create input handler
  InputHandler* IH = new InputHandler();
  if (IH->Initialize()) {
    perror("- ERROR while initializing InputHandler");
    exit(EXIT_FAILURE);
  }

  // Initialize anlyses
  ECalMonitor* ecal_mon = new ECalMonitor();
  TargetMonitor* target_mon = new TargetMonitor();

  if( clock_gettime(CLOCK_REALTIME,&now) == -1 ) {
    perror("- ERROR clock_gettime");
    exit(EXIT_FAILURE);
  }
  TTimeStamp t_start = TTimeStamp(now.tv_sec,now.tv_nsec);
  printf("=== OnlineMonitor starting on %s\n",cfg->FormatTime(now.tv_sec));

  while(true) {

    TRawEvent* rawEv = IH->NextEvent();
    if (rawEv == 0) {
      printf("- Reached end of streams: exiting\n");
      break;
    }

    // Show event header once in a while (if required)
    if ( (cfg->DebugScale() != 0) && (IH->EventNumber()%cfg->DebugScale() == 0) ) {
      TTimeStamp tts = rawEv->GetEventAbsTime();
      printf("%7u Run %7d Event %7d Time %8d-%06d.%09d RunTime %13llu TrigMask 0x%02x EvtStatus 0x%04x Boards %2d MissBoard 0x%04x\n",
	     IH->EventNumber(),rawEv->GetRunNumber(),rawEv->GetEventNumber(),tts.GetDate(),tts.GetTime(),tts.GetNanoSec(),
	     rawEv->GetEventRunTime(),(rawEv->GetEventTrigMask() & 0xff),(rawEv->GetEventStatus() & 0xffff),
	     rawEv->GetNADCBoards(),(rawEv->GetMissingADCBoards() & 0xffff));
    }

    // Save event information
    cfg->SetRunNumber(rawEv->GetRunNumber());
    cfg->SetEventNumber(rawEv->GetEventNumber());
    cfg->SetEventAbsTime(rawEv->GetEventAbsTime());
    cfg->SetEventRunTime(rawEv->GetEventRunTime());
    cfg->SetEventTrigMask(rawEv->GetEventTrigMask());
    cfg->SetEventStatus(rawEv->GetEventStatus());

    // Call "start of event" procedures for all detectors
    ecal_mon->StartOfEvent();
    target_mon->StartOfEvent();

    // Loop over boards
    UChar_t nBoards = rawEv->GetNADCBoards();
    for(UChar_t b=0;b<nBoards;b++){

      // Get board info
      TADCBoard* adcB = rawEv->ADCBoard(b);
      UChar_t boardId = adcB->GetBoardId();
      UInt_t activeMsk = adcB->GetActiveChannelMask();
      UInt_t acceptMsk = adcB->GetAcceptedChannelMask();
      UChar_t nChn = adcB->GetNADCChannels();

      // Loop over channels
      for(UChar_t c=0;c<nChn;c++){
	TADCChannel* chn = adcB->ADCChannel(c);
	UChar_t chNr = chn->GetChannelNumber();
	if (activeMsk & (1 << chNr)) {
	  if (acceptMsk & (1 << chNr)) {
	    if ( (boardId <= 9) || (boardId >= 14 && boardId <= 23) ) {
	      ecal_mon->Analyze(boardId,chNr,chn->GetSamplesArray());
	    } else if (boardId >= 10 && boardId <= 12) {
	      // PVeto
	    } else if (boardId == 13) {
	      // HEPVeto
	    } else if (boardId >= 24 && boardId <= 26) {
	      // EVeto
	    } else if (boardId == 27) {
	      // SAC + Cosmics pads
	    } else if (boardId == 28) {
	      // Target
	      target_mon->Analyze(boardId,chNr,chn->GetSamplesArray());
	    }
	  }
	}
      }

    }

    // Clear event
    rawEv->Clear("C");

    // Call "end of event" procedures for all detectors
    ecal_mon->EndOfEvent();
    target_mon->EndOfEvent();

    // Check if we processed enough events
    if ( nEventsToProcess && (IH->EventsRead() >= nEventsToProcess) ) {
      printf("- Processed %d events: exiting\n",IH->EventsRead());
      break;
    }

  } // End loop over events

  // Finalize all detectors
  ecal_mon->Finalize();
  target_mon->Finalize();

  if( clock_gettime(CLOCK_REALTIME,&now) == -1 ) {
    perror("- ERROR clock_gettime");
    exit(EXIT_FAILURE);
  }
  TTimeStamp t_end = TTimeStamp(now.tv_sec,now.tv_nsec);
  printf("=== OnlineMonitor ending on %s\n",cfg->FormatTime(now.tv_sec));

  Double_t t_start_f = 1.*t_start.GetSec()+1.E-9*t_start.GetNanoSec();
  Double_t t_end_f = 1.*t_end.GetSec()+1.E-9*t_end.GetNanoSec();
  Double_t t_run_f = t_end_f-t_start_f;
  printf("- Total run time %.3fs\n",t_run_f);
  printf("- Total processed events %d\n",IH->EventsRead());
  if (IH->EventsRead()>0) printf("- Event processing time %.3f ms/evt\n",1000.*t_run_f/IH->EventsRead());
  if (t_run_f>0.) printf("- Event processing rate %.2f evt/s\n",IH->EventsRead()/t_run_f);

  delete IH;
  delete ecal_mon;
  delete target_mon;

  exit(0);

}
