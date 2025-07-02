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
  
  // Connect to configuration handler
  Configuration* cfg = Configuration::GetInstance();
  cfg->SetOutputDirectory("rawfile"); // Change default output directory
  cfg->SetNumberOfStreams(cfg->NumberOfStreamsMax()); // Make sure we use all PADME streams

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
  Int_t skipEventsDefault = cfg->NumberOfEventsToSkip();
  Int_t skipEvents = skipEventsDefault;

  struct timespec now;

  // Parse options
  //while ((c = getopt(argc, argv, "R:C:I:D:s:O:S:N:n:d:vh")) != -1) {
  while ((c = getopt(argc, argv, "R:C:I:D:s:O:N:n:d:vh")) != -1) {
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
        fprintf(stdout,"  -s: define number of events to skip at beginning. >0: skip PADME events. <0: skip Chamber events [default: from data]\n");
        fprintf(stdout,"  -O: define path to top output directory [default: '%s']\n",cfg->OutputDirectory().Data());
        //fprintf(stdout,"  -S: define number of streams to use [default: %u; max: %u] \n",cfg->NumberOfStreams(),cfg->NumberOfStreamsMax());
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
  if (skipEvents != skipEventsDefault) cfg->SetNumberOfEventsToSkip(skipEvents);

  // Show settings for this run
  fprintf(stdout,"- PADME run name: '%s'\n",cfg->RunName().Data());
  fprintf(stdout,"- MMChamber run name: '%s'\n",cfg->ChamberRunName().Data());
  fprintf(stdout,"- PADME Input Rawdata top directory: '%s'\n",cfg->DataDirectory().Data());
  fprintf(stdout,"- Chamber Input Rawdata top directory: '%s'\n",cfg->ChamberDataDirectory().Data());
  if (cfg->NumberOfEventsToSkip() != skipEventsDefault) {
    fprintf(stdout,"- Number of events to skip: %d\n",cfg->NumberOfEventsToSkip());
  }else {
    fprintf(stdout,"- Number of events to skip: computed from data\n");
  }
  fprintf(stdout,"- Output Rawfile top directory: '%s'\n",cfg->OutputDirectory().Data());
  //fprintf(stdout,"- Number of streams: %u\n",cfg->NumberOfStreams());
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

  if( clock_gettime(CLOCK_REALTIME,&now) == -1 ) {
    perror("- ERROR clock_gettime");
    exit(EXIT_FAILURE);
  }
  TTimeStamp t_start = TTimeStamp(now.tv_sec,now.tv_nsec);
  printf("=== MergeChamber starting on %s\n",cfg->FormatTime(now.tv_sec));

  Long64_t chEntry = 0;

  ULong64_t pdClk,oldPdClk = 0;
  Int_t pdDiff;
  Double_t pdDiff_us; // PADME trigger clock increment converted to microseconds
  Double_t pdTime, oldPdTime, pdTimeDiff;
  UInt_t pdTrig,pdPatt;
  Int_t chClk,oldChClk,chDiff;
  //Double_t chDiff_us; // Chamber trigger clock increment converted to microseconds
  Double_t chDiff_us_corr; // Chamber trigger clock increment converted to microseconds and corrected for clock drift
  Double_t chTime, oldChTime, chTimeDiff;
  UInt_t chTrig;

  //Double_t chClockCorrectionFactor = -0.000197;
  //Double_t chClockCorrectionFactor = -0.000201;
  Double_t chClockCorrectionFactor = -0.000200; // in MHz
  Int_t chClockRollover = 16777216; // Chamber clock counter has 24 bits -> 2^24=16777216. Rollover every 2^24/(40E6 Hz)=0.42 sec
  Double_t chClockRolloverTime = (Double_t)chClockRollover/(40.+chClockCorrectionFactor)/1.E6;
  printf("- Chamber clock rollover at %d i.e. %8.6fs\n",chClockRollover,chClockRolloverTime);

  Double_t clockDiffTolerance = 0.3; // (in us) Tolerance above which a clock difference is associated to lost events

  Double_t eps = 0.;
  Double_t totEps = 0.;
  UInt_t nEps = 0;

  UInt_t nMiss = 0;
  UInt_t nMissBTF = 0;
  UInt_t nMissCh = 0;
  UInt_t nTotBTF = 0;

  TRawEvent* rawEv;

  if (cfg->NumberOfEventsToSkip() == skipEventsDefault) {

#define MERGECHAMBER_ALIGNMENT_EVENTS 50

    // Create input handler
    InputHandler* IH = new InputHandler();
    if (IH->Initialize()) {
      perror("- ERROR while initializing InputHandler");
      exit(EXIT_FAILURE);
    }

    // Create chamber reader
    Chamber* CH = new Chamber(cfg->ChamberDataDirectory()+"/"+cfg->ChamberRunName()+".root");
    TTree* chTree = CH->fChain;
    Long64_t chEvts = chTree->GetEntriesFast();
    if (chEvts<50) {
      printf("WARNING - Only %lld events in Chamber file: cannot compute best first event matching. Please set it with the -s option.\n",chEvts);
      exit(0);
    }

    // Initialize Chamber events counter
    chEntry = 0;

    // Read first 50 events from both streams
    Double_t pdTimeList[MERGECHAMBER_ALIGNMENT_EVENTS], chTimeList[MERGECHAMBER_ALIGNMENT_EVENTS];
    ULong64_t pdClkList[MERGECHAMBER_ALIGNMENT_EVENTS];
    Int_t chClkList[MERGECHAMBER_ALIGNMENT_EVENTS];
    while(true) {
      rawEv = IH->NextEvent();
      pdTimeList[chEntry] = rawEv->GetEventAbsTime().AsDouble();
      pdClkList[chEntry] = rawEv->GetEventRunTime();
      CH->LoadTree(chEntry);
      CH->GetEntry(chEntry);
      chTimeList[chEntry] = TTimeStamp(CH->daqTimeSec,1000*CH->daqTimeMicroSec).AsDouble();
      chClkList[chEntry] = CH->srsTimeStamp;
      chEntry++;
      if (chEntry>MERGECHAMBER_ALIGNMENT_EVENTS) break;
    }

    IH->Finalize();
    delete IH;
    delete CH;

    // Find best absolute time match for each PADME event
    UInt_t bestMatchList[MERGECHAMBER_ALIGNMENT_EVENTS];
    Double_t bestDiffList[MERGECHAMBER_ALIGNMENT_EVENTS];
    for(UInt_t i=0; i<MERGECHAMBER_ALIGNMENT_EVENTS; i++) { // Check PADME times after the initial slowdown
      bestMatchList[i] = 0;
      bestDiffList[i] = 10000.;
      for(UInt_t j=0; j<MERGECHAMBER_ALIGNMENT_EVENTS; j++) { // Compare with all Chamber events
	Double_t diff = 1000.*(pdTimeList[i]-chTimeList[j]);
	if (abs(diff) < abs(bestDiffList[i])) {
	  bestDiffList[i] = diff;
	  bestMatchList[i] = j;
	}
      }
      printf("=== PADME event %2d Best match with Chamber event %2d - Skip %2d events - Time diff %7.3fms\n",i,bestMatchList[i],i-bestMatchList[i],bestDiffList[i]);
    }

    // Now assume that the matches in final events are correct and go back to first event using precise clock counter matching
    oldPdClk = pdClkList[MERGECHAMBER_ALIGNMENT_EVENTS-4];
    chEntry = bestMatchList[MERGECHAMBER_ALIGNMENT_EVENTS-4];
    oldChClk = chClkList[chEntry];
    oldChTime = chTimeList[chEntry];
    Int_t toSkip = 0;
    for(Int_t i=MERGECHAMBER_ALIGNMENT_EVENTS-5; i>=0; i--) {

      chEntry--;
      if (chEntry<0) break;

      chDiff = oldChClk-chClkList[chEntry];
      if (chDiff<0) chDiff += chClockRollover; // Add a first rollover if difference is negative
      chTimeDiff = chTimeList[chEntry]-oldChTime;
      if (chTimeDiff > chClockRolloverTime) {
	Int_t nRollOver = int(chTimeDiff/chClockRolloverTime);
	chDiff += nRollOver*chClockRollover;
      }
      chDiff_us_corr = chDiff/(40.+chClockCorrectionFactor);

      pdDiff = oldPdClk-pdClkList[i];
      pdDiff_us = pdDiff/80.;

      // Check if Chamber skipped a trigger
      while ( (chDiff_us_corr-pdDiff_us) > clockDiffTolerance) {
	i--;
	if (i<0) break;
	pdDiff = oldPdClk-pdClkList[i];
	pdDiff_us = pdDiff/80.;
      }
      if (i<0) break;

      // Check if PADME skipped a trigger
      while ( (pdDiff_us-chDiff_us_corr) > clockDiffTolerance) {
	chEntry--;
	if (chEntry<0) break;
	chDiff = oldChClk-chClkList[chEntry];
	if (chDiff<0) chDiff += chClockRollover;
	chTimeDiff = chTimeList[chEntry]-oldChTime;
	if (chTimeDiff > chClockRolloverTime) {
	  Int_t nRollOver = int(chTimeDiff/chClockRolloverTime);
	  chDiff += nRollOver*chClockRollover;
	}
	chDiff_us_corr = chDiff/(40.+chClockCorrectionFactor);
      }
      if (chEntry<0) break;

      toSkip = i-chEntry;
      printf("PADME %2d %10lld Chamber %2lld %10d ClkDiff %6.3fus TimeDiff %6.1fms\n",i,pdClkList[i],chEntry,chClkList[chEntry],chDiff_us_corr-pdDiff_us,1000.*(pdTimeList[i]-chTimeList[chEntry]));

      oldPdClk = pdClkList[i];
      oldPdTime = pdTimeList[i];
      oldChClk = chClkList[chEntry];
      oldChTime = chTimeList[chEntry];
 
    }

    printf("- Computed number of events to skip: %d\n", toSkip);
    cfg->SetNumberOfEventsToSkip(toSkip);

  }

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
    pdClk = rawEv->GetEventRunTime();
    pdDiff = pdClk-oldPdClk;
    pdDiff_us = pdDiff/80.;
    if (pdPatt == 1) nTotBTF++;

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
    if (chDiff<0) chDiff += chClockRollover; // Add a first rollover if difference is negative
    // Check if the Chamber time gap wrt previous event is too large and requires multiple clock rollover corrections
    chTimeDiff = chTime-oldChTime;
    if (chTimeDiff > chClockRolloverTime) {
      // Compute the number of rollovers and apply it to the Chamber clock counter difference wrt last good event
      printf("PADME %2.2x %8d %15.6fs %9d %15.6fus\n",pdPatt,pdTrig,pdTime,pdDiff,pdDiff_us);
      printf("Chamber  %8d %15.6fs %9d %15.6fus\n",chTrig,chTime,chDiff,chDiff/(40.+chClockCorrectionFactor));
      Int_t nRollOver = int(chTimeDiff/chClockRolloverTime);
      printf("Number of rollovers: %.6fs / %.6fs = %d\n",chTimeDiff,chClockRolloverTime,nRollOver);
      chDiff += nRollOver*chClockRollover;
      printf("- Long delay detected: %.6fs #RollOver %d New ChDiff %d %.3fus PdDiff %.3fus\n",chTimeDiff,nRollOver,chDiff,chDiff/(40.+chClockCorrectionFactor),pdDiff_us);
     }
    chDiff_us_corr = chDiff/(40.+chClockCorrectionFactor);
 
    // Check if Chamber skipped a trigger
    while ( (chDiff_us_corr-pdDiff_us) > clockDiffTolerance) {
      printf("- Skipping PADME event %d (%2.2x) - pdDiff %10.3f chDiff %10.3f\n",pdTrig,pdPatt,pdDiff_us,chDiff_us_corr);
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
      pdClk = rawEv->GetEventRunTime();
      pdDiff = pdClk-oldPdClk;
      pdDiff_us = pdDiff/80.;
      if (pdPatt == 1) nTotBTF++;
    }

    // Check if PADME skipped a trigger
    while ( (pdDiff_us-chDiff_us_corr) > clockDiffTolerance) {
      printf("- Skipping Chamber event %lld (trigger %d) - pdDiff %10.3f chDiff %10.3f\n",chEntry,chTrig-chDeltaEvent,pdDiff_us,chDiff_us_corr);
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
      if (chDiff<0) chDiff += chClockRollover;
      chTimeDiff = chTime-oldChTime;
      if (chTimeDiff > chClockRolloverTime) {
	// Compute the number of rollovers and apply it to the Chamber clock counter difference wrt last good event
	printf("PADME %2.2x %8d %15.6fs %9d %15.6fus\n",pdPatt,pdTrig,pdTime,pdDiff,pdDiff_us);
	printf("Chamber  %8d %15.6fs %9d %15.6fus\n",chTrig,chTime,chDiff,chDiff/(40.+chClockCorrectionFactor));
	Int_t nRollOver = int(chTimeDiff/chClockRolloverTime);
	printf("Number of rollovers: %.6fs / %.6fs = %d\n",chTimeDiff,chClockRolloverTime,nRollOver);
	chDiff += nRollOver*chClockRollover;
	printf("- Long delay detected while skipping Chamber event: %.6fs #RollOver %d New ChDiff %d %.3fus PdDiff %.3fus\n",chTimeDiff,nRollOver,chDiff,chDiff/(40.+chClockCorrectionFactor),pdDiff_us);
      }
      chDiff_us_corr = chDiff/(40.+chClockCorrectionFactor);
    }
    if (chEntry >= chEntries) break;

    // Compute clock correction factor for this run
    eps = 40.*(2.*(Double_t)chDiff/(Double_t)pdDiff-1.);
    if (abs(eps)<0.001) {
      totEps += eps;
      nEps++;
    }

    //if ( cfg->DebugScale() && (IH->EventNumber()%cfg->DebugScale() == 0) ) {
    Double_t timeDiff = 1000.*(chTime-pdTime); // Absolute time difference in ms
    printf("PADME %7d (%2.2x) %9d %12.3fus Chamber %7lld %7d %9d %12.3fus Diff %6.3fus TDiff %6.1fms\n",pdTrig,pdPatt,pdDiff,pdDiff_us,chEntry,chTrig-chDeltaEvent,2*chDiff,chDiff_us_corr,chDiff_us_corr-pdDiff_us,timeDiff);
    //}

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
  printf("- Total discarded Chamber events: %d\n",nMissCh);
  printf("- Total processed BTF events: %d\n",nTotBTF);
  printf("- Total discarded BTF events: %d\n",nMissBTF);
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
