#include "Riostream.h"
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "TRawEvent.hh"

#include "Configuration.hh"
#include "InputHandler.hh"
#include "ECalMonitor.hh"
#include "EVetoMonitor.hh"
#include "PVetoMonitor.hh"
#include "HEPVetoMonitor.hh"
#include "SACMonitor.hh"
#include "TargetMonitor.hh"
#include "TriggerMonitor.hh"
#include "ADCMonitor.hh"

#include "utlConfigParser.hh"

int main(int argc, char* argv[])
{
  
  int c;

  TString runName = "";
  TString dataDirectory = "";
  TString outputDirectory = "";
  TString configFileName = "";
  TString stopFileName = "";
  UInt_t nStreams = 0;
  UInt_t nEventsToProcess = 0;
  Int_t debugScale = -1;

  struct timespec now;
  
  // Connect to configuration handler
  Configuration* cfg = Configuration::GetInstance();

  // Parse options
  while ((c = getopt(argc, argv, "R:D:S:o:n:s:d:c:frvh")) != -1) {
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
	cfg->SetVerbose(cfg->Verbose()+1);
        break;
      case 'h':
        fprintf(stdout,"\nOnlineMonitor -R run_name [-D datadir] [-S streams] [-o outdir] [-f] [-r] [-s stopfile] [-c configfile] [-n events] [-d debug] [-v] [-h]\n\n");
        fprintf(stdout,"  -R: define name of run to process\n");
        fprintf(stdout,"  -D: define path to top rawdata directory [default: '%s']\n",cfg->DataDirectory().Data());
        fprintf(stdout,"  -S: define number of streams to use [default: %u; max: %u] \n",cfg->NumberOfStreams(),cfg->NumberOfStreamsMax());
        fprintf(stdout,"  -o: define directory where PadmeMonitor files will be written [default: '%s']\n",cfg->OutputDirectory().Data());
        fprintf(stdout,"  -n: define number of events to process [default: 0 (all events)]\n");
        fprintf(stdout,"  -f: enable FOLLOW mode [default: disabled]\n");
        fprintf(stdout,"  -r: enable RESUME mode [default: disabled]\n");
        fprintf(stdout,"  -s: define name of control file to stop program [default: '%s']\n",cfg->StopFile().Data());
        fprintf(stdout,"  -c: define name of configuration file[default: '%s']\n",cfg->ConfigFile().Data());
        fprintf(stdout,"  -d: define frequency of debug printout [default: %u]\n",cfg->DebugScale());
        fprintf(stdout,"  -v: increase verbose level (can be repeated)\n");
        fprintf(stdout,"  -h: show this help message and exit\n\n");
        exit(EXIT_SUCCESS);
      case '?':
	if (optopt == 'R' || optopt == 'D' || optopt == 'S' || optopt == 's' || optopt == 'n' || optopt == 'o' || optopt == 'c' || optopt == 'd')
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
  if (! configFileName.IsNull()) cfg->SetConfigFile(configFileName);
  if (! stopFileName.IsNull()) cfg->SetStopFile(stopFileName);
  if (debugScale != -1) cfg->SetDebugScale(debugScale);

  // Show settings for this run
  fprintf(stdout,"- Run name: '%s'\n",cfg->RunName().Data());
  fprintf(stdout,"- Rawdata top directory: '%s'\n",cfg->DataDirectory().Data());
  fprintf(stdout,"- Output PadmeMonitor directory: '%s'\n",cfg->OutputDirectory().Data());
  fprintf(stdout,"- Number of streams: %u\n",cfg->NumberOfStreams());
  if (cfg->FollowMode()) {
    fprintf(stdout,"- Follow mode enabled\n");
    if (cfg->ResumeMode()) fprintf(stdout,"- Resume mode enabled\n");
    fprintf(stdout,"- Stop file: '%s'\n",cfg->StopFile().Data());
  }
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

  // Configure ADC analyzer
  Bool_t analyzeADC = true;
  ADCMonitor* adc_mon = 0;
  if ( configParser->HasConfig("ANALYZE","ADC") && (std::stoi(configParser->GetSingleArg("ANALYZE","ADC")) == 0) ) analyzeADC = false;
  if (analyzeADC) {
    TString configFileADC = "config/ADC.cfg";
    if (configParser->HasConfig("CONFIGFILE","ADC")) configFileADC = configParser->GetSingleArg("CONFIGFILE","ADC");
    adc_mon = new ADCMonitor(configFileADC);
  }

  // Configure Trigger analyzer
  Bool_t analyzeTrigger = true;
  TriggerMonitor* trigger_mon = 0;
  if ( configParser->HasConfig("ANALYZE","Trigger") && (std::stoi(configParser->GetSingleArg("ANALYZE","Trigger")) == 0) ) analyzeTrigger = false;
  if (analyzeTrigger) {
    TString configFileTrigger = "config/Target.cfg";
    if (configParser->HasConfig("CONFIGFILE","Trigger")) configFileTrigger = configParser->GetSingleArg("CONFIGFILE","Trigger");
    trigger_mon = new TriggerMonitor(configFileTrigger);
  }

  // Configure Target analyzer
  Bool_t analyzeTarget = true;
  TargetMonitor* target_mon = 0;
  if ( configParser->HasConfig("ANALYZE","Target") && (std::stoi(configParser->GetSingleArg("ANALYZE","Target")) == 0) ) analyzeTarget = false;
  if (analyzeTarget) {
    TString configFileTarget = "config/Target.cfg";
    if (configParser->HasConfig("CONFIGFILE","Target")) configFileTarget = configParser->GetSingleArg("CONFIGFILE","Target");
    target_mon = new TargetMonitor(configFileTarget);
  }

  // Configure ECal analyzer
  Bool_t analyzeECal = true;
  ECalMonitor* ecal_mon = 0;
  if ( configParser->HasConfig("ANALYZE","ECal") && (std::stoi(configParser->GetSingleArg("ANALYZE","ECal")) == 0) ) analyzeECal = false;
  if (analyzeECal) {
    TString configFileECal = "config/ECal.cfg";
    if (configParser->HasConfig("CONFIGFILE","ECal")) configFileECal = configParser->GetSingleArg("CONFIGFILE","ECal");
    ecal_mon = new ECalMonitor(configFileECal);
  }

  // Configure SAC analyzer
  Bool_t analyzeSAC = true;
  SACMonitor* sac_mon = 0;
  if ( configParser->HasConfig("ANALYZE","SAC") && (std::stoi(configParser->GetSingleArg("ANALYZE","SAC")) == 0) ) analyzeSAC = false;
  if (analyzeSAC) {
    TString configFileSAC = "config/SAC.cfg";
    if (configParser->HasConfig("CONFIGFILE","SAC")) configFileSAC = configParser->GetSingleArg("CONFIGFILE","SAC");
    sac_mon = new SACMonitor(configFileSAC);
  }

  // Configure PVeto analyzer
  Bool_t analyzePVeto = true;
  PVetoMonitor* pveto_mon = 0;
  if ( configParser->HasConfig("ANALYZE","PVeto") && (std::stoi(configParser->GetSingleArg("ANALYZE","PVeto")) == 0) ) analyzePVeto = false;
  if (analyzePVeto) {
    TString configFilePVeto = "config/PVeto.cfg";
    if (configParser->HasConfig("CONFIGFILE","PVeto")) configFilePVeto = configParser->GetSingleArg("CONFIGFILE","PVeto");
    pveto_mon = new PVetoMonitor(configFilePVeto);
  }

  // Configure EVeto analyzer
  Bool_t analyzeEVeto = true;
  EVetoMonitor* eveto_mon = 0;
  if ( configParser->HasConfig("ANALYZE","EVeto") && (std::stoi(configParser->GetSingleArg("ANALYZE","EVeto")) == 0) ) analyzeEVeto = false;
  if (analyzeEVeto) {
    TString configFileEVeto = "config/EVeto.cfg";
    if (configParser->HasConfig("CONFIGFILE","EVeto")) configFileEVeto = configParser->GetSingleArg("CONFIGFILE","EVeto");
    eveto_mon = new EVetoMonitor(configFileEVeto);
  }

  // Configure HEPVeto analyzer
  Bool_t analyzeHEPVeto = true;
  HEPVetoMonitor* hepveto_mon = 0;
  if ( configParser->HasConfig("ANALYZE","HEPVeto") && (std::stoi(configParser->GetSingleArg("ANALYZE","HEPVeto")) == 0) ) analyzeHEPVeto = false;
  if (analyzeHEPVeto) {
    TString configFileHEPVeto = "config/HEPVeto.cfg";
    if (configParser->HasConfig("CONFIGFILE","HEPVeto")) configFileHEPVeto = configParser->GetSingleArg("CONFIGFILE","HEPVeto");
    hepveto_mon = new HEPVetoMonitor(configFileHEPVeto);
  }

  // N.B. InputHandler must be created AFTER all detectors have been initialized to avoid clashes on histogram booking

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
    if (analyzeTrigger) trigger_mon->StartOfEvent();
    if (analyzeADC)     adc_mon->StartOfEvent();
    if (analyzeTarget)  target_mon->StartOfEvent();
    if (analyzeECal)    ecal_mon->StartOfEvent();
    if (analyzeSAC)     sac_mon->StartOfEvent();
    if (analyzePVeto)   pveto_mon->StartOfEvent();
    if (analyzeEVeto)   eveto_mon->StartOfEvent();
    if (analyzeHEPVeto) hepveto_mon->StartOfEvent();

    // Loop over boards
    UChar_t nBoards = rawEv->GetNADCBoards();
    for(UChar_t b=0;b<nBoards;b++){

      // Get board info
      TADCBoard* adcB = rawEv->ADCBoard(b);
      UChar_t boardId = adcB->GetBoardId();
      UInt_t activeMsk = adcB->GetActiveChannelMask();
      UInt_t acceptMsk = adcB->GetAcceptedChannelMask();
      UChar_t nChn = adcB->GetNADCChannels();
      UChar_t nTrg = adcB->GetNADCTriggers();

      // Save board information
      cfg->SetBoardActiveMask(activeMsk);
      cfg->SetBoardAcceptMask(acceptMsk);

      // Loop over trigger groups and compute trigger times
      for(UChar_t t = 0; t<4; t++) {
	cfg->SetBoardTriggerTime(t,0.);
	cfg->SetBoardGroupSIC(t,0);
      }
      if (analyzeTrigger) {
	for(UChar_t t=0;t<nTrg;t++){
	  TADCTrigger* trg = adcB->ADCTrigger(t);
	  UChar_t trNr = trg->GetGroupNumber();
	  cfg->SetBoardGroupSIC(trNr,trg->GetStartIndexCell());
	  trigger_mon->AnalyzeChannel(boardId,trNr,trg->GetSamplesArray());
	  cfg->SetBoardTriggerTime(trNr,trigger_mon->GetTriggerTime());
	}
      }
      if (cfg->Verbose() > 2) {
	printf("- Run %d Event %d Board %2d Trigger times",cfg->GetRunNumber(),cfg->GetEventNumber(),boardId);
	for(UChar_t t=0;t<4;t++) printf(" %6.1f",cfg->GetBoardTriggerTime(t));
	printf(" Group SICs");
	for(UChar_t t=0;t<4;t++) printf(" %4d",cfg->GetBoardGroupSIC(t));
	printf("\n");
      }

      // Loop over channels
      for(UChar_t c=0;c<nChn;c++){
	TADCChannel* chn = adcB->ADCChannel(c);
	UChar_t chNr = chn->GetChannelNumber();
	if (activeMsk & (1 << chNr)) {
	  if (analyzeADC) adc_mon->AnalyzeChannel(boardId,chNr,chn->GetSamplesArray()); // ADC
	  if ( (boardId <= 9) || (boardId >= 14 && boardId <= 23) ) {
	    if (analyzeECal) ecal_mon->AnalyzeChannel(boardId,chNr,chn->GetSamplesArray()); // ECal
	  } else if (boardId >= 10 && boardId <= 12) {
	    if (analyzePVeto) pveto_mon->AnalyzeChannel(boardId,chNr,chn->GetSamplesArray()); // PVeto
	  } else if (boardId == 13) {
	    if (analyzeHEPVeto) hepveto_mon->AnalyzeChannel(boardId,chNr,chn->GetSamplesArray()); // HEPVeto
	  } else if (boardId >= 24 && boardId <= 26) {
	    if (analyzeEVeto) eveto_mon->AnalyzeChannel(boardId,chNr,chn->GetSamplesArray()); // EVeto
	  } else if (boardId == 27) {
	    if (analyzeSAC) sac_mon->AnalyzeChannel(boardId,chNr,chn->GetSamplesArray()); // SAC + Cosmics pads
	  } else if (boardId == 28) {
	    if (analyzeTarget) target_mon->AnalyzeChannel(boardId,chNr,chn->GetSamplesArray()); // Target
	  }
	}
      }

      // Analyze global board quantities
      if (analyzeADC) adc_mon->Analyze(boardId); // ADC
      if ( (boardId <= 9) || (boardId >= 14 && boardId <= 23) ) {
	if (analyzeECal) ecal_mon->Analyze(boardId); // ECal
      } else if (boardId >= 10 && boardId <= 12) {
	if (analyzePVeto) pveto_mon->Analyze(boardId); // PVeto
      } else if (boardId == 13) {
	if (analyzeHEPVeto) hepveto_mon->Analyze(boardId); // HEPVeto
      } else if (boardId >= 24 && boardId <= 26) {
	if (analyzeEVeto) eveto_mon->Analyze(boardId); // EVeto
      } else if (boardId == 27) {
	if (analyzeSAC) sac_mon->Analyze(boardId); // SAC + Cosmics pads
      } else if (boardId == 28) {
	if (analyzeTarget) target_mon->Analyze(boardId); // Target
      }

    }

    // Clear event
    rawEv->Clear("C");

    // Call "end of event" procedures for all detectors
    if (analyzeTrigger) trigger_mon->EndOfEvent();
    if (analyzeADC)     adc_mon->EndOfEvent();
    if (analyzeTarget)  target_mon->EndOfEvent();
    if (analyzeECal)    ecal_mon->EndOfEvent();
    if (analyzeSAC)     sac_mon->EndOfEvent();
    if (analyzePVeto)   pveto_mon->EndOfEvent();
    if (analyzeEVeto)   eveto_mon->EndOfEvent();
    if (analyzeHEPVeto) hepveto_mon->EndOfEvent();

    // Check if we processed enough events
    if ( nEventsToProcess && (IH->EventsRead() >= nEventsToProcess) ) {
      printf("- Processed %d events: exiting\n",IH->EventsRead());
      break;
    }

  } // End loop over events

  // Finalize all detectors
  if (analyzeTrigger) trigger_mon->Finalize();
  if (analyzeADC)     adc_mon->Finalize();
  if (analyzeTarget)  target_mon->Finalize();
  if (analyzeECal)    ecal_mon->Finalize();
  if (analyzeSAC)     sac_mon->Finalize();
  if (analyzePVeto)   pveto_mon->Finalize();
  if (analyzeEVeto)   eveto_mon->Finalize();
  if (analyzeHEPVeto) hepveto_mon->Finalize();

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

  // Final cleanup
  delete IH;
  if (trigger_mon) delete trigger_mon;
  if (adc_mon)     delete adc_mon;
  if (target_mon)  delete target_mon;
  if (ecal_mon)    delete ecal_mon;
  if (sac_mon)     delete sac_mon;
  if (pveto_mon)   delete pveto_mon;
  if (eveto_mon)   delete eveto_mon;
  if (hepveto_mon) delete hepveto_mon;

  exit(EXIT_SUCCESS);

}
