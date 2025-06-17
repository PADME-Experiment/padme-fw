#include "Riostream.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <vector>

#include "Configuration.hh"
#include "ChamberInputHandler.hh"
#include "SmallChamberMonitor.hh"

#include "utlConfigParser.hh"

int main(int argc, char* argv[])
{
  
  int c;

  TString runName = "";
  TString dataDirectory = "";
  TString trendDirectory = "";
  TString outputDirectory = "";
  TString configFileName = "";
  TString stopFileName = "";
  TString endrunFileName = "";
  //UInt_t nStreams = 0;
  UInt_t nEventsToProcess = 0;
  Int_t debugScale = -1;

  struct timespec now;
  
  // Connect to configuration handler
  Configuration* cfg = Configuration::GetInstance();

  // Parse options
  //while ((c = getopt(argc, argv, "R:D:S:o:t:n:s:d:c:frvIh")) != -1) {
  while ((c = getopt(argc, argv, "R:D:o:t:n:s:e:d:c:frvh")) != -1) {
    switch (c)
      {
      case 'R':
	runName = optarg;
	break;
      case 'D':
	dataDirectory = optarg;
	break;
      case 'o':
        outputDirectory = optarg;
	break;
      case 't':
        trendDirectory = optarg;
	break;
      //case 'S':
      //  if ( sscanf(optarg,"%u",&nStreams) != 1 ) {
      //    fprintf (stderr, "Error while processing option '-S'. Wrong parameter '%s'.\n", optarg);
      //    exit(EXIT_FAILURE);
      //  }
      //  if ( nStreams < 1 || nStreams > cfg->NumberOfStreamsMax() ) {
      //    fprintf (stderr, "Error while processing option '-S'. Required %d streams (must be 1<=S<=%u).\n",nStreams,cfg->NumberOfStreamsMax());
      //    exit(EXIT_FAILURE);
      //  }
      //  break;
      case 'n':
        if ( sscanf(optarg,"%d",&nEventsToProcess) != 1 ) {
          fprintf (stderr, "Error while processing option '-n'. Wrong parameter '%s'.\n", optarg);
          exit(EXIT_FAILURE);
        }
        if (nEventsToProcess<0) {
          fprintf (stderr, "Error while processing option '-n'. Required %d events (must be >=0).\n", nEventsToProcess);
          exit(EXIT_FAILURE);
        }
        break;
      case 'd':
        if ( sscanf(optarg,"%d",&debugScale) != 1 ) {
          fprintf (stderr, "Error while processing option '-d'. Wrong parameter '%s'.\n", optarg);
          exit(EXIT_FAILURE);
        }
        if (debugScale<0) {
          fprintf (stderr, "Error while processing option '-d': required %d, must be >=0.\n", debugScale);
          exit(EXIT_FAILURE);
        }
        break;
	//case 'I':
	//	cfg->EnableIgnoreDisabledChannels();
	//	break;
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
      case 'e':
        endrunFileName = optarg;
	break;
      case 'v':
	cfg->SetVerbose(cfg->Verbose()+1);
        break;
      case 'h':
        fprintf(stdout,"\nSmallChamberMonitor -R run_name [-D datadir] [-o outdir] [-f] [-r] [-s stopfile] [-c configfile] [-n events] [-d debug] [-v] [-h]\n\n");
        fprintf(stdout,"  -R: define name of run to process\n");
        fprintf(stdout,"  -D: define path to top rawdata directory [default: '%s']\n",cfg->DataDirectory().Data());
        //fprintf(stdout,"  -S: define number of streams to use [default: %u; max: %u] \n",cfg->NumberOfStreams(),cfg->NumberOfStreamsMax());
        fprintf(stdout,"  -o: define directory where PadmeMonitor files will be written [default: '%s']\n",cfg->OutputDirectory().Data());
        fprintf(stdout,"  -t: define directory where trend files will be written [default: '%s']\n",cfg->TrendDirectory().Data());
        fprintf(stdout,"  -n: define number of events to process [default: 0 (all events)]\n");
        fprintf(stdout,"  -f: enable FOLLOW mode [default: disabled]\n");
        fprintf(stdout,"  -r: enable RESUME mode [default: disabled]\n");
        //fprintf(stdout,"  -I: ignore disabled channels in detector boards [default: do not ignore]\n");
        fprintf(stdout,"  -s: define name of control file to stop program [default: '%s']\n",cfg->StopFile().Data());
        fprintf(stdout,"  -e: define name of tag file to signal end of run [default: '%s']\n",cfg->EndRunFile().Data());
        fprintf(stdout,"  -c: define name of configuration file[default: '%s']\n",cfg->ConfigFile().Data());
        fprintf(stdout,"  -d: define frequency of debug printout [default: %u]\n",cfg->DebugScale());
        fprintf(stdout,"  -v: increase verbose level (can be repeated)\n");
        fprintf(stdout,"  -h: show this help message and exit\n\n");
        exit(EXIT_SUCCESS);
      case '?':
	//if (optopt == 'R' || optopt == 'D' || optopt == 'S' || optopt == 's' || optopt == 'n' || optopt == 'o' || optopt == 't' || optopt == 'c' || optopt == 'd')
	if (optopt == 'R' || optopt == 'D' || optopt == 's' || optopt == 'e' || optopt == 'n' || optopt == 'o' || optopt == 't' || optopt == 'c' || optopt == 'd')
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
  if (! trendDirectory.IsNull()) cfg->SetTrendDirectory(trendDirectory);
  //if (nStreams) cfg->SetNumberOfStreams(nStreams);
  if (! configFileName.IsNull()) cfg->SetConfigFile(configFileName);
  if (! stopFileName.IsNull()) cfg->SetStopFile(stopFileName);
  if (! endrunFileName.IsNull()) cfg->SetEndRunFile(endrunFileName);
  if (debugScale != -1) cfg->SetDebugScale(debugScale);

  // Show settings for this run
  fprintf(stdout,"- Run name: '%s'\n",cfg->RunName().Data());
  fprintf(stdout,"- Rawdata top directory: '%s'\n",cfg->DataDirectory().Data());
  fprintf(stdout,"- Output PadmeMonitor directory: '%s'\n",cfg->OutputDirectory().Data());
  fprintf(stdout,"- Trend directory: '%s'\n",cfg->TrendDirectory().Data());
  //fprintf(stdout,"- Number of streams: %u\n",cfg->NumberOfStreams());
  if (cfg->FollowMode()) {
    fprintf(stdout,"- Follow mode enabled\n");
    if (cfg->ResumeMode()) fprintf(stdout,"- Resume mode enabled\n");
    fprintf(stdout,"- Stop file: '%s'\n",cfg->StopFile().Data());
  }
  fprintf(stdout,"- End-of-run file: '%s'\n",cfg->EndRunFile().Data());
  fprintf(stdout,"- Configuration file: '%s'\n",cfg->ConfigFile().Data());
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

  // Create configuration file parser
  utl::ConfigParser* configParser = new utl::ConfigParser((const std::string)cfg->ConfigFile());
  if (cfg->Verbose()>1) configParser->Print();

  // Configure Chamber analyzer
  Bool_t analyzeChamber = true;
  SmallChamberMonitor* chamber_mon = 0;
  if ( configParser->HasConfig("ANALYZE","SmallChamber") && (std::stoi(configParser->GetSingleArg("ANALYZE","SmallChamber")) == 0) ) analyzeChamber = false;
  if (analyzeChamber) {
    TString configFileChamber = "config/SmallChamber.cfg";
    if (configParser->HasConfig("CONFIGFILE","SmallChamber")) configFileChamber = configParser->GetSingleArg("CONFIGFILE","SmallChamber");
    chamber_mon = new SmallChamberMonitor(configFileChamber);
  }

  // N.B. InputHandler must be created AFTER all detectors have been initialized to avoid clashes on histogram booking

  // Create input handler
  ChamberInputHandler* IH = new ChamberInputHandler();
  if (IH->Initialize()) {
    perror("- ERROR while initializing InputHandler");
    // Create end-of-run tag file
    std::ofstream outfile(cfg->EndRunFile().Data());
    if (not outfile.good()) printf("ChamberInputHandler::NextEvent - WARNING - Cannot create end-of-run tag file %s.\n",cfg->EndRunFile().Data());
    exit(EXIT_FAILURE);
  }

  if( clock_gettime(CLOCK_REALTIME,&now) == -1 ) {
    perror("- ERROR clock_gettime");
    exit(EXIT_FAILURE);
  }
  TTimeStamp t_start = TTimeStamp(now.tv_sec,now.tv_nsec);
  printf("=== SmallChamberMonitor starting on %s\n",cfg->FormatTime(now.tv_sec));

  while(true) {

    ChamberEvent* rawEv = IH->NextEvent();
    if (rawEv == 0) {
      printf("- Reached end of streams: exiting\n");
      break;
    }

    // Save event information
    //cfg->SetRunNumber(rawEv->GetRunNumber());
    cfg->SetEventNumber(rawEv->evt);
    TTimeStamp tts = TTimeStamp(rawEv->daqTimeSec,1000*rawEv->daqTimeMicroSec);
    cfg->SetEventAbsTime(tts);
    cfg->SetEventRunTime(rawEv->srsTimeStamp);

    // Show event header once in a while (if required)
    if ( (cfg->DebugScale() != 0) && (IH->EventNumber()%cfg->DebugScale() == 0) ) {
      printf("%7u Event %7llu Time %8d-%06d.%09d RunTime %8d Trigger %8u Error %8u\n",
	     IH->EventNumber(),rawEv->evt,tts.GetDate(),tts.GetTime(),tts.GetNanoSec(),
	     rawEv->srsTimeStamp,rawEv->srsTrigger,rawEv->error);
    }

    // Analyze event
    if (analyzeChamber) {
      chamber_mon->StartOfEvent();
      chamber_mon->AnalyzeEvent(rawEv);
      chamber_mon->EndOfEvent();
    }

    // Check if we processed enough events
    if ( nEventsToProcess && (IH->EventsRead() >= nEventsToProcess) ) {
      printf("- Processed %d events: exiting\n",IH->EventsRead());
      break;
    }

  } // End loop over events

  // Finalize detector
  if (analyzeChamber) chamber_mon->Finalize();

  // Create end-of-run tag file
  std::ofstream outfile(cfg->EndRunFile().Data());
  if (not outfile.good()) printf("ChamberInputHandler::NextEvent - WARNING - Cannot create end-of-run tag file %s.\n",cfg->EndRunFile().Data());

  if( clock_gettime(CLOCK_REALTIME,&now) == -1 ) {
    perror("- ERROR clock_gettime");
    exit(EXIT_FAILURE);
  }
  TTimeStamp t_end = TTimeStamp(now.tv_sec,now.tv_nsec);
  printf("=== SmallChamberMonitor ending on %s\n",cfg->FormatTime(now.tv_sec));

  Double_t t_start_f = 1.*t_start.GetSec()+1.E-9*t_start.GetNanoSec();
  Double_t t_end_f = 1.*t_end.GetSec()+1.E-9*t_end.GetNanoSec();
  Double_t t_run_f = t_end_f-t_start_f;
  printf("- Total run time %.3fs\n",t_run_f);
  printf("- Total processed events %d\n",IH->EventsRead());
  if (IH->EventsRead()>0) printf("- Event processing time %.3f ms/evt\n",1000.*t_run_f/IH->EventsRead());
  if (t_run_f>0.) printf("- Event processing rate %.2f evt/s\n",IH->EventsRead()/t_run_f);

  // Final cleanup
  delete IH;
  if (chamber_mon) delete chamber_mon;

  exit(EXIT_SUCCESS);

}
