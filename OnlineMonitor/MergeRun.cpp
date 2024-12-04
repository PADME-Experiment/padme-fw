#include "Riostream.h"
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "TRawEvent.hh"

#include "Configuration.hh"
#include "InputHandler.hh"
#include "OutputHandler.hh"
#include "EventCopier.hh"

int main(int argc, char* argv[])
{
  
  int c;

  TString runName = "";
  TString dataDirectory = "";
  TString outputDirectory = "";
  TString stopFileName = "";
  UInt_t nStreams = 0;
  UInt_t nEventsToProcess = 0;
  Int_t nEventsPerFile = -1;
  Int_t debugScale = -1;

  struct timespec now;
  
  // Connect to configuration handler
  Configuration* cfg = Configuration::GetInstance();
  cfg->SetOutputDirectory("rawfile"); // Change default output directory
  cfg->SetNumberOfStreams(cfg->NumberOfStreamsMax());

  // Parse options
  while ((c = getopt(argc, argv, "R:I:O:S:N:n:d:vh")) != -1) {
    switch (c)
      {
      case 'R':
	runName = optarg;
	break;
      case 'I':
	dataDirectory = optarg;
	break;
      case 'O':
        outputDirectory = optarg;
	break;
      case 'S':
        if ( sscanf(optarg,"%u",&nStreams) != 1 ) {
          fprintf (stderr, "Error while processing option '-S'. Wrong parameter '%s'.\n", optarg);
          exit(EXIT_FAILURE);
        }
        if ( nStreams < 1 || nStreams > cfg->NumberOfStreamsMax() ) {
          fprintf (stderr, "Error while processing option '-S'. Required %d streams (must be 1<=S<=%u).\n",nStreams,cfg->NumberOfStreamsMax());
          exit(EXIT_FAILURE);
        }
        break;
      case 'N':
        if ( sscanf(optarg,"%d",&nEventsPerFile) != 1 ) {
          fprintf (stderr, "Error while processing option '-N'. Wrong parameter '%s'.\n", optarg);
          exit(EXIT_FAILURE);
        }
        if (nEventsPerFile<0) {
          fprintf (stderr, "Error while processing option '-N'. Required %d events per file (must be >=0).\n", nEventsPerFile);
          exit(EXIT_FAILURE);
        }
        break;
      case 'n':
        if ( sscanf(optarg,"%u",&nEventsToProcess) != 1 ) {
          fprintf (stderr, "Error while processing option '-n'. Wrong parameter '%s'.\n", optarg);
          exit(EXIT_FAILURE);
        }
        break;
      case 'd':
        if ( sscanf(optarg,"%d",&debugScale) != 1 ) {
          fprintf (stderr, "Error while processing option '-d'. Wrong parameter '%s'.\n", optarg);
          exit(EXIT_FAILURE);
        }
        if (debugScale<0) {
          fprintf (stderr, "Error while processing option '-d'. Requested debug scale %d (must be >=0).\n", debugScale);
          exit(EXIT_FAILURE);
        }
        break;
      case 'v':
	cfg->SetVerbose(cfg->Verbose()+1);
        break;
      case 'h':
        fprintf(stdout,"\nMergeRun -R run_name [-I inputdir] [-O outputdir] [-S streams] [-N events] [-n events] [-d debug] [-v] [-h]\n\n");
        fprintf(stdout,"  -R: define name of run to process\n");
        fprintf(stdout,"  -I: define path to top input directory [default: '%s']\n",cfg->DataDirectory().Data());
        fprintf(stdout,"  -O: define path to top output directory [default: '%s']\n",cfg->OutputDirectory().Data());
        fprintf(stdout,"  -S: define number of streams to use [default: %u; max: %u] \n",cfg->NumberOfStreams(),cfg->NumberOfStreamsMax());
        fprintf(stdout,"  -N: define maximum number of events to write in each output file (0: no limit) [default: %u]\n",cfg->EventsPerFile());
        fprintf(stdout,"  -n: define total number of events to process (0: no limit) [default: %u]\n",nEventsToProcess);
        fprintf(stdout,"  -d: define frequency of debug printout (0: no debug) [default: %u]\n",cfg->DebugScale());
        fprintf(stdout,"  -v: increase verbose level (can be repeated)\n");
        fprintf(stdout,"  -h: show this help message and exit\n\n");
        exit(EXIT_SUCCESS);
      case '?':
	if (optopt == 'R' || optopt == 'I' || optopt == 'O' || optopt == 'S' || optopt == 'N' || optopt == 'n' || optopt == 'd')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint(optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
        exit(EXIT_FAILURE);
      default:
        abort();
      }
  }

  // Check if run name was defined
  if (runName.IsNull()) {
    fprintf (stderr,"ERROR - No run name defined with -R option.\n");
    exit(EXIT_FAILURE);
  }
  cfg->SetRunName(runName);

  // Save configuration parameters for this run
  if (! dataDirectory.IsNull()) cfg->SetDataDirectory(dataDirectory);
  if (! outputDirectory.IsNull()) cfg->SetOutputDirectory(outputDirectory);
  if (nStreams) cfg->SetNumberOfStreams(nStreams);
  if (nEventsPerFile != -1) cfg->SetEventsPerFile(nEventsPerFile);
  if (debugScale != -1) cfg->SetDebugScale(debugScale);

  // Show settings for this run
  fprintf(stdout,"- Run name: '%s'\n",cfg->RunName().Data());
  fprintf(stdout,"- Input Rawdata top directory: '%s'\n",cfg->DataDirectory().Data());
  fprintf(stdout,"- Output Rawfile top directory: '%s'\n",cfg->OutputDirectory().Data());
  fprintf(stdout,"- Number of streams: %u\n",cfg->NumberOfStreams());
  fprintf(stdout,"- Number of events per file: %u\n",cfg->EventsPerFile());
  if (cfg->DebugScale() == 0) {
    fprintf(stdout,"- Debug printout is OFF\n");
  } else {
    fprintf(stdout,"- Debug printout every %d events\n",cfg->DebugScale());
  }
  if (nEventsToProcess) {
    fprintf(stdout,"- Process first %u events in stream(s)\n",nEventsToProcess);
  } else {
    fprintf(stdout,"- Process all events in stream(s)\n");
  }
  if (cfg->Verbose()) fprintf(stdout,"- Verbose level: %u\n",cfg->Verbose());

  // Create event copier
  EventCopier* EC = new EventCopier();
  if (EC->Initialize()) {
    perror("- ERROR while initializing EventCopier");
    exit(EXIT_FAILURE);
  }

  // Create output handler
  OutputHandler* OH = new OutputHandler();
  if (OH->Initialize()) {
    perror("- ERROR while initializing OutputHandler");
    exit(EXIT_FAILURE);
  }

  // Create input handler
  InputHandler* IH = new InputHandler();
  if (IH->Initialize()) {
    perror("- ERROR while initializing InputHandler");
    exit(EXIT_FAILURE);
  }

  if( clock_gettime(CLOCK_REALTIME,&now) == -1 ) {
    perror("- ERROR clock_gettime");
    exit(EXIT_FAILURE);
  }
  TTimeStamp t_start = TTimeStamp(now.tv_sec,now.tv_nsec);
  printf("=== MergeRun starting on %s\n",cfg->FormatTime(now.tv_sec));

  while(true) {

    TRawEvent* rawEv = IH->NextEvent();
    if (rawEv == 0) {
      printf("- Reached end of streams: exiting\n");
      break;
    }

    // Show event header once in a while (if required)
    if ( cfg->DebugScale() && (IH->EventNumber()%cfg->DebugScale() == 0) ) {
      TTimeStamp tts = rawEv->GetEventAbsTime();
      printf("%7u Run %7d Event %7d Time %8d-%06d.%09d RunTime %13llu TrigMask 0x%02x EvtStatus 0x%04x Boards %2d MissBoard 0x%04x\n",
	     IH->EventNumber(),rawEv->GetRunNumber(),rawEv->GetEventNumber(),tts.GetDate(),tts.GetTime(),tts.GetNanoSec(),
	     rawEv->GetEventRunTime(),(rawEv->GetEventTrigMask() & 0xff),(rawEv->GetEventStatus() & 0xffff),
	     rawEv->GetNADCBoards(),(rawEv->GetMissingADCBoards() & 0xffff));
    }

    // Copy current input event to output event structure
    if (EC->CopyEvent(OH->GetRawEvent(),rawEv)) {
      perror("- ERROR while copying input event to output event");
      exit(EXIT_FAILURE);
    }

    // Write current event to output file
    //if (OH->WriteEvent(rawEv)) {
    if (OH->WriteEvent()) {
      perror("- ERROR while writing output file");
      exit(EXIT_FAILURE);
    }

    // Clear event
    rawEv->Clear("C");

    // Check if we processed enough events
    if ( nEventsToProcess && (IH->EventsRead() >= nEventsToProcess) ) {
      printf("- Processed %d events: exiting\n",IH->EventsRead());
      break;
    }

  } // End loop over events

  // Finalize event copier
  EC->Finalize();

  // Finalize output handler
  OH->Finalize();

  // Finalize input handler
  IH->Finalize();

  if( clock_gettime(CLOCK_REALTIME,&now) == -1 ) {
    perror("- ERROR clock_gettime");
    exit(EXIT_FAILURE);
  }
  TTimeStamp t_end = TTimeStamp(now.tv_sec,now.tv_nsec);
  printf("=== MergeRun ending on %s\n",cfg->FormatTime(now.tv_sec));

  Double_t t_start_f = 1.*t_start.GetSec()+1.E-9*t_start.GetNanoSec();
  Double_t t_end_f = 1.*t_end.GetSec()+1.E-9*t_end.GetNanoSec();
  Double_t t_run_f = t_end_f-t_start_f;
  printf("- Total run time: %.3fs\n",t_run_f);
  printf("- Total processed events: %d\n",IH->EventsRead());
  printf("- Output files: %d\n",OH->GetTotalOutFiles());
  printf("- Total output events: %d\n",OH->GetTotalEvents());
  printf("- Total output data: %lld\n",OH->GetTotalSize());
  if (IH->EventsRead()>0) printf("- Event processing time: %.3f ms/evt\n",1000.*t_run_f/IH->EventsRead());
  if (t_run_f>0.) printf("- Event processing rate: %.2f evt/s\n",IH->EventsRead()/t_run_f);
  if (cfg->Verbose()) {
    printf("- List of output files\n");
    for(UInt_t i = 0; i < OH->GetTotalOutFiles(); i++) {
      printf("  %4u %s\n",i,OH->GetOutFile(i).Data());
    }
  }

  delete IH;
  delete OH;
  delete EC;

  exit(EXIT_SUCCESS);

}
