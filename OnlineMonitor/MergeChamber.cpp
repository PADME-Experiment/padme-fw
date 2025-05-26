#include "Riostream.h"
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "TRawEvent.hh"

#include "Configuration.hh"
#include "InputHandler.hh"
#include "OutputHandler.hh"
#include "EventCopier.hh"

#include "Chamber.hh"

int main(int argc, char* argv[])
{
  
  int c;

  TString runName = "";
  TString chamberRunName = "";
  TString dataDirectory = "";
  TString chamberDataDirectory = "";
  TString outputDirectory = "";
  TString stopFileName = "";
  UInt_t nStreams = 0;
  UInt_t nEventsToProcess = 0;
  Int_t nEventsPerFile = -1;
  Int_t debugScale = -1;
  Int_t skipEvents = 0;

  struct timespec now;
  
  // Connect to configuration handler
  Configuration* cfg = Configuration::GetInstance();
  cfg->SetOutputDirectory("rawfile"); // Change default output directory
  cfg->SetNumberOfStreams(cfg->NumberOfStreamsMax());

  // Parse options
  while ((c = getopt(argc, argv, "R:C:I:D:s:O:S:N:n:d:vh")) != -1) {
    switch (c)
      {
      case 'R':
	runName = optarg;
	break;
      case 'C':
	chamberRunName = optarg;
	break;
      case 'I':
	dataDirectory = optarg;
	break;
      case 'D':
	chamberDataDirectory = optarg;
	break;
      case 'O':
        outputDirectory = optarg;
	break;
      case 's':
        if ( sscanf(optarg,"%d",&skipEvents) != 1 ) {
          fprintf (stderr, "Error while processing option '-s'. Wrong parameter '%s'.\n", optarg);
          exit(EXIT_FAILURE);
        }
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
        fprintf(stdout,"\nMergeRun -R run_name -C chamber_run_name [-I inputdir] [-D chamber_inputdir] [-O outputdir] [-S streams] [-N events] [-n events] [-d debug] [-v] [-h]\n\n");
        fprintf(stdout,"  -R: define name of PADME run to merge\n");
        fprintf(stdout,"  -C: define name of Chamber run to merge\n");
        fprintf(stdout,"  -I: define path to PADME top input directory [default: '%s']\n",cfg->DataDirectory().Data());
        fprintf(stdout,"  -D: define path to Chamber top input directory [default: '%s']\n",cfg->ChamberDataDirectory().Data());
        fprintf(stdout,"  -s: define number of events to skip at beginning. >0: skip PADME events. <0: skip Chamber events [default: %d]\n",cfg->NumberOfEventsToSkip());
        fprintf(stdout,"  -O: define path to top output directory [default: '%s']\n",cfg->OutputDirectory().Data());
        fprintf(stdout,"  -S: define number of streams to use [default: %u; max: %u] \n",cfg->NumberOfStreams(),cfg->NumberOfStreamsMax());
        fprintf(stdout,"  -N: define maximum number of events to write in each output file (0: no limit) [default: %u]\n",cfg->EventsPerFile());
        fprintf(stdout,"  -n: define total number of events to process (0: no limit) [default: %u]\n",nEventsToProcess);
        fprintf(stdout,"  -d: define frequency of debug printout (0: no debug) [default: %u]\n",cfg->DebugScale());
        fprintf(stdout,"  -v: increase verbose level (can be repeated)\n");
        fprintf(stdout,"  -h: show this help message and exit\n\n");
        exit(EXIT_SUCCESS);
      case '?':
	if (optopt == 'R' || optopt == 'C' || optopt == 'I' || optopt == 'D' || optopt == 'O' || optopt == 'S' || optopt == 'N' || optopt == 'n' || optopt == 'd')
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

  // Check if run names were defined
  if (runName.IsNull()) {
    fprintf (stderr,"ERROR - No PADME run name defined with -R option.\n");
    exit(EXIT_FAILURE);
  }
  cfg->SetRunName(runName);
  if (chamberRunName.IsNull()) {
    fprintf (stderr,"ERROR - No Chamber run name defined with -C option.\n");
    exit(EXIT_FAILURE);
  }
  cfg->SetChamberRunName(chamberRunName);

  // Save configuration parameters for this run
  if (! dataDirectory.IsNull()) cfg->SetDataDirectory(dataDirectory);
  if (! chamberDataDirectory.IsNull()) cfg->SetChamberDataDirectory(chamberDataDirectory);
  if (! outputDirectory.IsNull()) cfg->SetOutputDirectory(outputDirectory);
  if (nStreams) cfg->SetNumberOfStreams(nStreams);
  if (nEventsPerFile != -1) cfg->SetEventsPerFile(nEventsPerFile);
  if (debugScale != -1) cfg->SetDebugScale(debugScale);
  if (skipEvents != cfg->NumberOfEventsToSkip()) cfg->SetNumberOfEventsToSkip(skipEvents);

  // Show settings for this run
  fprintf(stdout,"- PADME run name: '%s'\n",cfg->RunName().Data());
  fprintf(stdout,"- MMChamber run name: '%s'\n",cfg->ChamberRunName().Data());
  fprintf(stdout,"- PADME Input Rawdata top directory: '%s'\n",cfg->DataDirectory().Data());
  fprintf(stdout,"- Chamber Input Rawdata top directory: '%s'\n",cfg->ChamberDataDirectory().Data());
  fprintf(stdout,"- Number of events to skip: %d\n",cfg->NumberOfEventsToSkip());
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
  //EventCopier* EC = new EventCopier();
  //if (EC->Initialize()) {
  //  perror("- ERROR while initializing EventCopier");
  //  exit(EXIT_FAILURE);
  //}

  // Create output handler
  //OutputHandler* OH = new OutputHandler();
  //if (OH->Initialize()) {
  //  perror("- ERROR while initializing OutputHandler");
  //  exit(EXIT_FAILURE);
  //}

  // Create input handler
  InputHandler* IH = new InputHandler();
  if (IH->Initialize()) {
    perror("- ERROR while initializing InputHandler");
    exit(EXIT_FAILURE);
  }

  // Create chamber reader
  Chamber* CH = new Chamber(cfg->ChamberDataDirectory()+"/"+cfg->ChamberRunName()+".root");
  TTree* chTree = CH->fChain;
  Long64_t chEntries = chTree->GetEntriesFast();
  printf("Chamber entries = %lld\n",chEntries);

  if( clock_gettime(CLOCK_REALTIME,&now) == -1 ) {
    perror("- ERROR clock_gettime");
    exit(EXIT_FAILURE);
  }
  TTimeStamp t_start = TTimeStamp(now.tv_sec,now.tv_nsec);
  printf("=== MergeRun starting on %s\n",cfg->FormatTime(now.tv_sec));

  Long64_t chEntry = 0;
  //Long64_t iEntry, nBytes;
  //Int_t oldSec = 0;
  //Int_t oldMSec = 0;
  //Int_t oldSrsTS = 0;
  //Int_t dt,ds,dst,nroll;
  //Float_t chClockFreq = 40.; // Chamber clock frequency in MHz

  //Int_t oldPSec  = 0;
  //Int_t oldPMSec = 0;
  //Int_t oldPTrgT = 0;
  //Int_t pSec,pMSec,pTrgT;
  //Long64_t pdClockRollover = 1099511627776; // PADME trigger clock counter has 40 bits -> 2^40=1099511627776. Rollover every 2^40/(80E6 Hz)=13744 sec

  //Int_t oldPdTime = 0;
  //Int_t oldPdMSec = 0;
  Int_t oldPdClk = 0;
  //Int_t pdTime,pdMSec;
  ULong64_t pdClk;
  Int_t pdDiff;
  Double_t pdDiff_us; // PADME trigger clock increment converted to microseconds
  UInt_t pdTrig,pdPatt;
  //Int_t oldChTime = 0;
  //Int_t oldChMSec = 0;
  Int_t oldChClk = 0;
  //Int_t chTime,chMSec;
  Int_t chClk,chDiff;
  //Double_t chDiff_us; // Chamber trigger clock increment converted to microseconds
  Double_t chDiff_us_corr; // Chamber trigger clock increment converted to microseconds and corrected for clock drift
  UInt_t chTrig;

  //Float_t ChToPdClockRatio = 1.-1.4458E-5; // dT(padme) = dT(chamber)*2*ChToPdClockRatio
  Double_t chClockCorrectionFactor = -0.000197;
  Int_t chClockRollover = 16777216; // Chamber clock counter has 24 bits -> 2^24=16777216. Rollover every 2^24/(40E6 Hz)=0.42 sec
  Double_t chClockRolloverTime = (Double_t)chClockRollover/(40.+chClockCorrectionFactor)/1.E6;
  printf("- Chamber clock rollover at %d i.e. %8.6fs\n",chClockRollover,chClockRolloverTime);

  /*
  // Get first PADME event
  TRawEvent* rawEv = IH->NextEvent();
  TTimeStamp tts = rawEv->GetEventAbsTime();
  oldPdTime = tts.GetSec();
  oldPdMSec = tts.GetNanoSec()/1000;
  oldPdClk = rawEv->TriggerInfo()->GetTriggerTime();
  pdTrig = rawEv->TriggerInfo()->GetTriggerCounter();

  // Get first chamber event
  iEntry = CH->LoadTree(chEntry);
  nBytes = CH->GetEntry(chEntry);
  oldChTime = CH->daqTimeSec;
  oldChMSec = CH->daqTimeMicroSec;
  oldChClk = CH->srsTimeStamp;
  chTrig = CH->srsTrigger;
  chEntry++;
  */

  oldPdClk = 0;
  oldChClk = 0;

  Double_t eps = 0.;
  Double_t totEps = 0.;
  UInt_t nEps = 0;

  UInt_t nMiss = 0;
  UInt_t nMissBTF = 0;
  UInt_t nMissCh = 0;

  TRawEvent* rawEv;

  //TTimeStamp pdTime, chTime;
  Double_t pdTime, oldPdTime, pdTimeDiff;
  Double_t chTime, oldChTime, chTimeDiff;

  /*
  // Skip first PADME event
  rawEv = IH->NextEvent();
  oldPdClk = rawEv->TriggerInfo()->GetTriggerTime();
  pdPatt = rawEv->GetEventTrigMask();
  nMiss++;
  if (pdPatt == 1) nMissBTF++;
  */

  // Initialize Chamber events counter
  chEntry = 0;

  // Use chamber first event to define trigger number difference between PADME and chamber
  CH->LoadTree(chEntry);
  CH->GetEntry(chEntry);
  Int_t chDeltaEvent = CH->srsTrigger; // PADME first trigger is 0 by defintiton

  // Skip PADME/Chamber events (if needed)
  if (cfg->NumberOfEventsToSkip() > 0) { // Skipping PADME events
    printf("- Skipping first %d PADME events\n",cfg->NumberOfEventsToSkip());
    for(Int_t i=0; i<cfg->NumberOfEventsToSkip(); i++) {
      rawEv = IH->NextEvent();
      //oldPdClk = rawEv->TriggerInfo()->GetTriggerTime();
      pdPatt = rawEv->GetEventTrigMask();
      nMiss++;
      if (pdPatt == 1) nMissBTF++;
    }
  } else if (cfg->NumberOfEventsToSkip() < 0) { // Skipping Chamber events
    printf("- Skipping first %d Chamber events\n",-cfg->NumberOfEventsToSkip());
    chEntry = -cfg->NumberOfEventsToSkip();
    nMissCh = -cfg->NumberOfEventsToSkip();
  }

  Bool_t firstEvent = true;
  while(true) {

    // Read next PADME event
    rawEv = IH->NextEvent();
    if (rawEv == 0) {
      printf("- Reached end of PADME streams: exiting\n");
      break;
    }
    pdTrig = rawEv->GetEventNumber();
    pdTime = rawEv->GetEventAbsTime().AsDouble();
    pdPatt = rawEv->GetEventTrigMask();
    //pdClk = rawEv->TriggerInfo()->GetTriggerTime();
    pdClk = rawEv->GetEventRunTime();
    pdDiff = pdClk-oldPdClk;
    pdDiff_us = pdDiff/80.;

    // Read next Chamber event
    CH->LoadTree(chEntry);
    CH->GetEntry(chEntry);
    chTrig = CH->srsTrigger;
    chTime = TTimeStamp(CH->daqTimeSec,1000*CH->daqTimeMicroSec).AsDouble();
    chClk = CH->srsTimeStamp;

    // Skip first event as delta times cannot be checked
    if (firstEvent) {
      firstEvent = false;
      oldPdClk = pdClk;
      oldPdTime = pdTime;
      oldChClk = chClk;
      oldChTime = chTime;
      chEntry++;
      continue;
    }

    //printf("PADME %2.2x %7d %10d %10.3fus Chamber %7lld %7d %10d %10.3fus Diff %6.3f\n",pdPatt,pdTrig,pdDiff,pdDiff_us,chEntry,chTrig,2*chDiff,chDiff_us_corr,chDiff_us_corr-pdDiff_us);

    // Compute the Chamber clock counter increment taking into account rollovers (0.42sec)
    chDiff = chClk-oldChClk;
    // Check if the Chamber time gap wrt previous event is too large and requires multiple clock rollover corrections
    chTimeDiff = chTime-oldChTime;
    if (chTimeDiff > chClockRolloverTime) {
      // Compute the number of rollovers and apply it to the Chamber clock counter difference wrt last good event
      Int_t nRollOver = int(chTimeDiff/chClockRolloverTime);
      chDiff += nRollOver*chClockRollover;
      printf("- Long delay detected: %.3f #RollOver %d New ChDiff %d\n",chTimeDiff,nRollOver,chDiff);
    }
    if (chDiff<0) chDiff += chClockRollover;
    //chDiff_us = chDiff/40.;
    chDiff_us_corr = chDiff/(40.+chClockCorrectionFactor);
 
    // Check if Chamber skipped a trigger
    while ( (chDiff_us_corr-pdDiff_us) > 0.1) { // Tolerance is 100ns
      printf("- Reading next PADME event - pdDiff %10.3f chDiff %10.3f\n",pdDiff_us,chDiff_us_corr);
      nMiss++;
      if (pdPatt == 1) nMissBTF++;
      rawEv = IH->NextEvent();
      if (rawEv == 0) {
	printf("- Reached end of PADME events: exiting\n");
	break;
      }
      pdTrig = rawEv->GetEventNumber();
      pdTime = rawEv->GetEventAbsTime().AsDouble();
      pdPatt = rawEv->GetEventTrigMask();
      //pdClk = rawEv->TriggerInfo()->GetTriggerTime();
      pdClk = rawEv->GetEventRunTime();
      pdDiff = pdClk-oldPdClk;
      pdDiff_us = pdDiff/80.;
      //printf("PADME %2.2x %7d %10d %10.3fus Chamber %7lld %7d %10d %10.3fus Diff %6.3f\n",pdPatt,pdTrig,pdDiff,pdDiff_us,chEntry,chTrig,2*chDiff,chDiff_us_corr,chDiff_us_corr-pdDiff_us);
    }

    // Check if PADME skipped a trigger
    while ( (pdDiff_us-chDiff_us_corr) > 0.1) { // Tolerance is 100ns
      printf("- Reading next Chamber event - pdDiff %10.3f chDiff %10.3f\n",pdDiff_us,chDiff_us_corr);
      nMissCh++;
      chEntry++;
      if (chEntry >= chEntries) {
	printf("- Reached end of Chamber events: exiting\n");
	break;
      }
      CH->LoadTree(chEntry);
      CH->GetEntry(chEntry);
      chTrig = CH->srsTrigger;
      chTime = TTimeStamp(CH->daqTimeSec,1000*CH->daqTimeMicroSec).AsDouble();
      chClk = CH->srsTimeStamp;
      chDiff = chClk-oldChClk;
      pdTimeDiff = pdTime-oldPdTime;
      if (pdTimeDiff>chClockRolloverTime) {
	UInt_t nRoll = int(pdTimeDiff/chClockRolloverTime);
	printf("- Long PADME interval %8.6fs: applying %d Chamber rollovers\n",pdTimeDiff,nRoll);
	chDiff += nRoll*chClockRollover;
      }
      if (chDiff<0) chDiff += chClockRollover;
      //chDiff_us = chDiff/40.;
      chDiff_us_corr = chDiff/(40.+chClockCorrectionFactor);
    }
    if (chEntry >= chEntries) break;

    // Compute clock correction factor for this run
    eps = 40.*(2.*(Double_t)chDiff/(Double_t)pdDiff-1.);
    if (abs(eps)<0.001) {
      totEps += eps;
      nEps++;
    }

    //printf("PADME %2.2x %7d %10d Chamber %7lld %7d %10d Delta %7d %10d\n",pdPatt,pdTrig,pdDiff,chEntry,chTrig,2*chDiff,chTrig-pdTrig,2*chDiff-pdDiff);
    //printf("PADME %.3fus Chamber %.3fus Diff %.3fus CorrDiff %.3fus Eps %.6fMHz\n",pdDiff_us,chDiff_us,chDiff_us-pdDiff_us,chDiff_us_corr-pdDiff_us,eps);
    Double_t timeDiff = chTime-pdTime;
    //if (abs(timeDiff)>0.020)
    printf("PADME %2.2x %7d %10d %10.3fus Chamber %7lld %7d %10d %10.3fus Diff %6.3f TDiff %6.1fms\n",pdPatt,pdTrig,pdDiff,pdDiff_us,chEntry,chTrig-chDeltaEvent,2*chDiff,chDiff_us_corr,chDiff_us_corr-pdDiff_us,1000.*timeDiff);

    oldPdClk = pdClk;
    oldPdTime = pdTime;
    oldChClk = chClk;
    oldChTime = chTime;
    chEntry++;
    if (chEntry >= chEntries) {
      printf("- Reached end of Chamber events: exiting\n");
      break;
    }
}

  /*
  while(true) {

    Int_t delta = (oldPdTime*1000+oldPdMSec/1000)-(oldChTime*1000+oldChMSec/1000);
    printf("PADME time %10d.%6.6d %d Chamber time %10d.%6.6d %d Delta %8d\n",oldPdTime,oldPdMSec,pdTrig,oldChTime,oldChMSec,chTrig,delta);
 
    if ( (oldPdTime>oldChTime) || ( (oldPdTime==oldChTime) && (oldPdMSec>oldChMSec) ) ) {
      // PADME is ahead of chamber: read next chamber event
      iEntry = CH->LoadTree(chEntry);
      nBytes = CH->GetEntry(chEntry);
      chTime = CH->daqTimeSec;
      chMSec = CH->daqTimeMicroSec;
      chClk = CH->srsTimeStamp;
      chTrig = CH->srsTrigger;
      chEntry++;
      if (chEntry >= chEntries) {
	printf("- Reached end of chamber streams: exiting\n");
	break;
      }
      oldChTime = chTime;
      oldChMSec = chMSec;
      oldChClk = chClk;

    } else {
      // Chamber is ahead of PADME: read next PADME event
      TRawEvent* rawEv = IH->NextEvent();
      if (rawEv == 0) {
	printf("- Reached end of PADME streams: exiting\n");
	break;
      }
      TTimeStamp tts = rawEv->GetEventAbsTime();
      pdTime = tts.GetSec();
      pdMSec = tts.GetNanoSec()/1000;
      pdClk = rawEv->TriggerInfo()->GetTriggerTime();
      pdTrig = rawEv->TriggerInfo()->GetTriggerCounter();
      rawEv->Clear("C");
      oldPdTime = pdTime;
      oldPdMSec = pdMSec;
      oldPdClk = pdClk;
    }

  }
  */

  /*
  while(true) {

    Int_t delta = (pdTime*1000+pdMSec/1000)-(chTime*1000+chMSec/1000);
    printf("PADME time %10d.%6.6d Chamber time %10d.%6.6d Delta %8d\n",pdTime,pdMSec,chTime,chMSec,delta);

    if ( (pdTime>chTime) || ( (pdTime==chTime) && (pdMSec>chMSec) ) ) {
      // PADME is ahead of chamber: read next chamber event
      iEntry = CH->LoadTree(chEntry);
      nBytes = CH->GetEntry(chEntry);
      chTime = CH->daqTimeSec;
      chMSec = CH->daqTimeMicroSec;
      chEntry++;
      if (chEntry >= chEntries) {
	printf("- Reached end of chamber streams: exiting\n");
	break;
      }
    } else {
      // Chamber is ahead of PADME: read next PADME event
      TRawEvent* rawEv = IH->NextEvent();
      if (rawEv == 0) {
	printf("- Reached end of PADME streams: exiting\n");
	break;
      }
      TTimeStamp tts = rawEv->GetEventAbsTime();
      pdTime = tts.GetSec();
      pdMSec = tts.GetNanoSec()/1000;
      rawEv->Clear("C");
    }

  }
  */

  /*
  while(true) {

    // Get next PADME event
    TRawEvent* rawEv = IH->NextEvent();
    if (rawEv == 0) {
      printf("- Reached end of PADME streams: exiting\n");
      break;
    }

    // Get next chamber event
    iEntry = CH->LoadTree(chEntry);
    nBytes = CH->GetEntry(chEntry);

    // Check chamber time
    if (oldSec) {
      dt = 1000000*(CH->daqTimeSec-oldSec)+(CH->daqTimeMicroSec-oldMSec);
      nroll = int(chClockFreq*dt/chClockRollover);
      //ds = CH->srsTimeStamp-oldSrsTS;
      //if (ds<0) ds = chClockRollover+ds; // We can use absolute time to check if the clock counter rolled over more than once
      ds = CH->srsTimeStamp-oldSrsTS;
      if (ds<0) ds += chClockRollover; // Correct for clock counter rollover
      if (nroll) ds += nroll*chClockRollover; // Take into account additional rollovers using info from absolute clock
      dst = int(ds/chClockFreq+0.5);
      //printf("Chamber NRoll = %2d\tSRSts = %8u\tdt = %6d us\tds = %7d\tSRSdT = %6d us\tdelay = %d us\n",nroll,CH->srsTimeStamp,dt,ds,dst,dst-dt);
      //printf("Chamber NRoll = %2d\tSRSts = %8u\tds = %8d\tds(50Hz) = %10.6f\n",nroll,CH->srsTimeStamp,ds,(1.*ds)/786408.5);
      printf("Chamber NRoll = %2d\tSRSts = %8u\tds = %8d\tds(50Hz) = %10.6f\n",nroll,CH->srsTimeStamp,ds,(1.*ds)/786407.87);
    } else {
      printf("Chamber NRoll = %2d\tSRSts = %8u\n",0,CH->srsTimeStamp);
    }

    oldSec   = CH->daqTimeSec;
    oldMSec  = CH->daqTimeMicroSec;
    oldSrsTS = CH->srsTimeStamp;

    pSec  = rawEv->GetEventAbsTime().GetSec();
    pMSec = rawEv->GetEventAbsTime().GetNanoSec()/1000;
    pTrgT = rawEv->TriggerInfo()->GetTriggerTime();

    if (oldPSec) {
      dt = 1000000*(pSec-oldPSec)+(pMSec-oldPMSec);
      ds = pTrgT-oldPTrgT;
      nroll = 0;
      if (ds<0) {
	ds += pdClockRollover;
	nroll = 1;
      }
      printf("PADME   NRoll = %2d\tSRSts = %8u\tds = %8d\n",nroll,pTrgT,ds);
    } else {
      printf("PADME   NRoll = %2d\tSRSts = %8u\n",0,pTrgT);
    }

    oldPSec  = pSec;
    oldPMSec = pMSec;
    oldPTrgT = pTrgT;

    // Show event header once in a while (if required)
    if ( cfg->DebugScale() && (IH->EventNumber()%cfg->DebugScale() == 0) ) {
      TTimeStamp tts = rawEv->GetEventAbsTime();
      printf("PADME   Entry %8u Run %8d Event %8d Time %8d-%06d.%09d RunTime %13llu TrigMask 0x%02x EvtStatus 0x%04x Boards %2d MissBoard 0x%04x\n",
	     IH->EventNumber(),rawEv->GetRunNumber(),rawEv->GetEventNumber(),tts.GetDate(),tts.GetTime(),tts.GetNanoSec(),
	     rawEv->GetEventRunTime(),(rawEv->GetEventTrigMask() & 0xff),(rawEv->GetEventStatus() & 0xffff),
	     rawEv->GetNADCBoards(),(rawEv->GetMissingADCBoards() & 0xffff));
      TTimeStamp chts = TTimeStamp(CH->daqTimeSec,CH->daqTimeMicroSec*1000);
      //printf("Chamber Entry %8lld Bytes %6lld Event %8lld Time %10d.%6.6d TimeStamp %10d Trigger %10d\n",iEntry,nBytes,CH->evt,CH->daqTimeSec,CH->daqTimeMicroSec,CH->srsTimeStamp,CH->srsTrigger);
      printf("Chamber Entry %8lld Bytes %6lld Event %8lld Time %8d-%06d.%09d TimeStamp %11d Trigger %10d\n",iEntry,nBytes,CH->evt,chts.GetDate(),chts.GetTime(),chts.GetNanoSec(),CH->srsTimeStamp,CH->srsTrigger);
    }

    // switch off output during testing period
    //// Copy current input event to output event structure
    //if (EC->CopyEvent(OH->GetRawEvent(),rawEv)) {
    //  perror("- ERROR while copying input event to output event");
    //  exit(EXIT_FAILURE);
    //}
    //
    //// Write current event to output file
    ////if (OH->WriteEvent(rawEv)) {
    //if (OH->WriteEvent()) {
    //  perror("- ERROR while writing output file");
    //  exit(EXIT_FAILURE);
    //}

    // Clear event
    rawEv->Clear("C");

    // Check if we processed enough events
    if ( nEventsToProcess && (IH->EventsRead() >= nEventsToProcess) ) {
      printf("- Processed %d events: exiting\n",IH->EventsRead());
      break;
    }

    // Update chamber event counter
    chEntry++;
    if (chEntry >= chEntries) {
      printf("- Reached end of chamber streams: exiting\n");
      break;
    }

  } // End loop over events
  */

  // Finalize event copier
  //EC->Finalize();

  // Finalize output handler
  //OH->Finalize();

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
  printf("- Total discarded PADME events: %d\n",nMiss);
  printf("- Total discarded PADME BTF events: %d\n",nMissBTF);
  printf("- Total discarded Chamber events: %d\n",nMissCh);
  printf("- Average frequency correction on 40MHz: %.6f\n",totEps/nEps);
  //printf("- Output files: %d\n",OH->GetTotalOutFiles());
  //printf("- Total output events: %d\n",OH->GetTotalEvents());
  //printf("- Total output data: %lld\n",OH->GetTotalSize());
  if (IH->EventsRead()>0) printf("- Event processing time: %.3f ms/evt\n",1000.*t_run_f/IH->EventsRead());
  if (t_run_f>0.) printf("- Event processing rate: %.2f evt/s\n",IH->EventsRead()/t_run_f);
  //if (cfg->Verbose()) {
  //  printf("- List of output files\n");
  //  for(UInt_t i = 0; i < OH->GetTotalOutFiles(); i++) {
  //    printf("  %4u %s\n",i,OH->GetOutFile(i).Data());
  //  }
  //}

  delete IH;
  //delete OH;
  //delete EC;
  delete CH;

  exit(EXIT_SUCCESS);

}
