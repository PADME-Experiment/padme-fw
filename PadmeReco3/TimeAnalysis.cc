#include "Riostream.h"
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <sys/stat.h>

#include "TMath.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObjArray.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"

#include "HistoSvc.hh"
#include "TrigTimeSvc.hh"
#include "utlConfigParser.hh"
#include "PadmeVRecoConfig.hh"

#include "TRawEvent.hh"

#define N_MAX_CHANNELS 1000

struct TimeNtuple {
  
  Int_t RunNumber;
  Int_t EventNumber;

  Int_t NChannels;

  Int_t BoardId[N_MAX_CHANNELS];
  Int_t ChannelId[N_MAX_CHANNELS];

  Double_t WaveformRMS[N_MAX_CHANNELS];
  Double_t PedestalRMS[N_MAX_CHANNELS];

  Double_t SamplesPoint[N_MAX_CHANNELS];
  Double_t TriggerPoint[N_MAX_CHANNELS];

  Double_t ChannelTime[N_MAX_CHANNELS];

};

int main(int argc, char* argv[])
{
  
  int c;
  int nevents = 0;

  TString inputFileName;
  TObjArray inputFileNameList;
  TString outputFileName("TimeAnalysis.root");
  TString configFileName("config/TimeAnalysis.cfg");
  struct stat filestat;

  // Parse options
  while ((c = getopt (argc, argv, "i:l:o:c:n:h")) != -1) {
    switch (c)
      {
      case 'i':
        inputFileNameList.Add(new TObjString(optarg));
        fprintf(stdout,"Added input data file '%s'\n",optarg);
	break;
      case 'l':
	if ( stat(Form(optarg),&filestat) == 0 ) {
	  fprintf(stdout,"Reading list of input files from '%s'\n",optarg);
	  std::ifstream inputList(optarg);
	  while( inputFileName.ReadLine(inputList) ) inputFileNameList.Add(new TObjString(inputFileName.Data()));
	} else {
	  fprintf(stderr,"ERROR: file list '%s' is not accessible\n",optarg);
	  exit(1);
	}
        break;
      case 'o':
	outputFileName = TString(optarg);
	break;
      case 'c':
	configFileName = TString(optarg);
	break;
      case 'n':
        if ( sscanf(optarg,"%d",&nevents) != 1 ) {
          fprintf (stderr, "Error while processing option '-n'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        if (nevents<0) {
          fprintf (stderr, "Error while processing option '-n'. Required %d events (must be >=0).\n", nevents);
          exit(1);
        }
	if (nevents) {
	  fprintf(stdout,"Will read first %d events in file\n",nevents);
	} else {
	  fprintf(stdout,"Will read all events in file\n");
	}
        break;
      case 'h':
        fprintf(stdout,"\nReadTest [-i input root file] [-l list of input files] [-v verbosity] [-h]\n\n");
        fprintf(stdout,"  -i: define an input file in root format\n");
        fprintf(stdout,"  -l: define a list of input files\n");
        fprintf(stdout,"  -o: define name of output file\n");
        fprintf(stdout,"  -c: define name of configuration file\n");
        fprintf(stdout,"  -n: define number of events to read from input file (0: all events)\n");
        fprintf(stdout,"  -h: show this help message and exit\n\n");
        exit(0);
      case '?':
        if (optopt == 'i' || optopt == 'l' || optopt == 'o' || optopt == 'c')
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

  // Check if configuration file exists
  if ( stat(configFileName,&filestat) != 0 ) {
    fprintf (stderr,"ERROR: configuration file '%s' is not accessible\n",configFileName.Data());
    exit(1);
  }

  // Check if input files were specified
  if ( inputFileNameList.GetEntries() == 0 ) {
    fprintf (stderr,"ERROR No Input File specified\n");
    exit(1);
  }

  // Create chain of input files
  fprintf(stdout,"=== === === Chain of input files === === ===\n");
  TChain* inputChain = new TChain("RawEvents");
  for (Int_t iFile = 0; iFile < inputFileNameList.GetEntries(); iFile++) {
    fprintf(stdout,"%4d %s\n",iFile,((TObjString*)inputFileNameList.At(iFile))->GetString().Data());
    inputChain->AddFile(((TObjString*)inputFileNameList.At(iFile))->GetString());
  }
  if (inputChain->GetEntries() == 0) {
    perror(Form("ERROR No events found for tree 'RawEvents' in input chain"));
    exit(1);
  }

  // Get some info about the input chain
  Int_t runNEntries = inputChain->GetEntries();
  std::cout << "Found Tree 'RawEvents' with " << runNEntries << " entries" << std::endl;
  //for(Int_t i=0; i < inputChain->GetListOfBranches()->GetEntries(); i++) {
  //  std::cout << "Branch " << i << " is " << inputChain->GetListOfBranches()->At(i)->GetName() << std::endl;
  //}
  TRawEvent* rawEv = new TRawEvent();
  inputChain->SetBranchAddress("RawEvent",&rawEv);

  // Set number of events to read
  Int_t ntoread = runNEntries;
  if (nevents && nevents<runNEntries) {
    ntoread = nevents;
    printf("Reading first %d events\n",ntoread);
  }

  // Read configuration
  utl::ConfigParser* configParser = new utl::ConfigParser((const std::string)configFileName);
  PadmeVRecoConfig* config = new PadmeVRecoConfig(configParser,"ECal");

  Int_t fCFShift = 5;
  Double_t fCFRatio = 0.8;
  Double_t fCFThreshold = 0.;

  // Get verbose level from config file
  Int_t verbose = config->GetParOrDefault("Output", "Verbose", 0);

  // Initialize output file using HistoSvc
  HistoSvc* HS = HistoSvc::GetInstance();
  HS->SetVerbose(verbose);
  HS->Initialize(outputFileName);
  HS->CreateDir("TimeAnalysis");

  // Initialize Trigger Time Service
  TrigTimeSvc* TTS = TrigTimeSvc::GetInstance();
  TTS->SetVerbose(verbose);
  TTS->Initialize();

  // Define parameters to use for hit building process
  Int_t fGoodSamples = config->GetParOrDefault("RECO","GoodSamples",994);
  Int_t fPedestalSamples = config->GetParOrDefault("RECO","PedestalSamples",100);
  Double_t fPedestalRMSThreshold = config->GetParOrDefault("RECO","PedestalRMSThreshold",5.);
  Double_t fZeroSupRMSThreshold = config->GetParOrDefault("RECO","ZeroSupRMSThreshold",5.);

  // Create ntuple
  TimeNtuple tNtuple;
  TTree* hNtuple = HS->BookNtuple("TimeAnalysis","TimeNtuple","Ntuple with results of time analysis");

  hNtuple->Branch("RunNumber"  ,&(tNtuple.RunNumber),  "RunNumber/I"  ); 
  hNtuple->Branch("EventNumber",&(tNtuple.EventNumber),"EventNumber/I");

  hNtuple->Branch("NChannels",  &(tNtuple.NChannels),  "NChannels/I"   );

  hNtuple->Branch("BoardId",    &(tNtuple.BoardId),    "BoardId[NChannels]/I");
  hNtuple->Branch("ChannelId",  &(tNtuple.ChannelId),  "ChannelId[NChannels]/I");

  hNtuple->Branch("WaveformRMS",&(tNtuple.WaveformRMS),"WaveformRMS[NChannels]/D");
  hNtuple->Branch("PedestalRMS",&(tNtuple.PedestalRMS),"PedestalRMS[NChannels]/D");

  hNtuple->Branch("SamplesPoint",&(tNtuple.SamplesPoint),"SamplesPoint[NChannels]/D");
  hNtuple->Branch("TriggerPoint",&(tNtuple.TriggerPoint),"TriggerPoint[NChannels]/D");

  hNtuple->Branch("ChannelTime",&(tNtuple.ChannelTime),"ChannelTime[NChannels]/D");

  // Create global histograms
  TH1D* hWF2900 = HS->BookHisto("TimeAnalysis","WF2900","Board 29 Channel 0 waveform",1024,0.,1024.);
  TH1D* hCF2900 = HS->BookHisto("TimeAnalysis","CF2900","Board 29 Channel 0 Constant fraction",1024,0.,1024.);
  TH1D* hDiff2901 = HS->BookHisto("TimeAnalysis","Diff2901","B29C00 vs B29C01 Time difference",1000,-2.,2.);
  TH1D* hDiff2908 = HS->BookHisto("TimeAnalysis","Diff2908","B29C00 vs B29C08 Time difference",1000,-2.,2.);
  TH1D* hDiff2916 = HS->BookHisto("TimeAnalysis","Diff2916","B29C00 vs B29C16 Time difference",1000,-2.,2.);
  TH1D* hDiff2924 = HS->BookHisto("TimeAnalysis","Diff2924","B29C00 vs B29C24 Time difference",1000,-2.,2.);
  TH1D* hDiff3000 = HS->BookHisto("TimeAnalysis","Diff3000","B29C00 vs B30C00 Time difference",1000,-2.,2.);
  TH1D* hDiff3008 = HS->BookHisto("TimeAnalysis","Diff3008","B29C00 vs B30C08 Time difference",1000,-2.,2.);
  TH1D* hDiff3016 = HS->BookHisto("TimeAnalysis","Diff3016","B29C00 vs B30C16 Time difference",1000,-2.,2.);
  TH1D* hDiff3024 = HS->BookHisto("TimeAnalysis","Diff3024","B29C00 vs B30C24 Time difference",1000,-2.,2.);

  for(Int_t iev=0;iev<ntoread;iev++){

    // Read event
    //printf("Reading event %d\n",iev);
    inputChain->GetEntry(iev);
    //printf("Event %d read\n",iev);

    // Show event header
    if ( (verbose > 0) || (iev%10 == 0) ) {
      TTimeStamp tts = rawEv->GetEventAbsTime();
      printf("N %7d Run %7d Event %7d Time %8d-%06d.%09d RunTime %12llu TrigMask %08x EvtStatus %08x Boards %2d MissBoard %08x\n",
	     iev,rawEv->GetRunNumber(),rawEv->GetEventNumber(),tts.GetDate(),tts.GetTime(),tts.GetNanoSec(),
	     rawEv->GetEventRunTime(),rawEv->GetEventTrigMask(),rawEv->GetEventStatus(),
	     rawEv->GetNADCBoards(),rawEv->GetMissingADCBoards());
    }

    tNtuple.RunNumber = rawEv->GetRunNumber();
    tNtuple.EventNumber = rawEv->GetEventNumber();
    tNtuple.NChannels = 0; // Reset channel counter for this event

    TADCBoard* board;
    TADCChannel* channel;
    //TADCTrigger* group;

    // Save reference time (Board 29 Channel 0) for time difference histograms
    Double_t refTime = 0.;

    // Loop over boards and check if it must be analyzed
    for(UChar_t iBoard = 0; iBoard < rawEv->GetNADCBoards(); iBoard++) {

      board = rawEv->ADCBoard(iBoard);
      UChar_t boardId = board->GetBoardId();

      // Compute trigger times for all boards
      TTS->ProcessEvent(rawEv);

      // Loop over channels and check if they are active
      for(UChar_t ich = 0; ich < board->GetNADCChannels();ich++) {

	channel = board->ADCChannel(ich);
	UChar_t channelId = channel->GetChannelNumber();
	UChar_t groupId = channelId/8;

	// Only 8 channels are used on each board
	if (channelId !=  0 && channelId !=  1 && channelId !=  8 && channelId !=  9 &&
	    channelId != 16 && channelId != 17 && channelId != 24 && channelId != 25) continue;

	// Save board/channel id to ntuple
	tNtuple.BoardId[tNtuple.NChannels] = boardId;
	tNtuple.ChannelId[tNtuple.NChannels] = channelId;

	// Convert samples to double
	Double_t samplesD[1024];
	for(UInt_t i=0; i<1024; i++) samplesD[i] = (Double_t)channel->GetSample(i);

	// Get RMS of "good" samples and use it to reject empty waveforms (poor man's zero suppression)
	Double_t rms = TMath::RMS(fGoodSamples,samplesD);
	if (rms<fZeroSupRMSThreshold) continue; // Do not analyze "empty" channels

	printf("Board %2d Channel %2d Group %d RefPoint %f\n",boardId,channelId,groupId,TTS->GetReferencePoint(boardId,groupId));
	tNtuple.TriggerPoint[tNtuple.NChannels] = TTS->GetReferencePoint(boardId,groupId);

	// Compute pedestal
	Double_t pedestal = TMath::Mean(fPedestalSamples,samplesD);

	// Check if pedestal samples are flat
	Double_t pedrms = TMath::RMS(fPedestalSamples,samplesD);
	if (pedrms > fPedestalRMSThreshold && boardId != 28) {
	  printf("WARNING - Run %5d Event %6d Board %2d Channel %2d - Anomalous pedestal RMS: %.2f\n",
		 rawEv->GetRunNumber(),rawEv->GetEventNumber(),boardId,channelId,pedrms);
	}

	// Subtract pedestal to get ready for constant fraction
	for(UInt_t i=0; i<1024; i++) samplesD[i] = pedestal-samplesD[i];

	// Save waveform parameters to ntuple
	tNtuple.WaveformRMS[tNtuple.NChannels] = rms;
	tNtuple.PedestalRMS[tNtuple.NChannels] = pedrms;

	// Show waveform and constant fraction for single channel
	if (boardId == 29 && channelId == 0) {
	  for(Int_t i=0;i<1024;i++) hWF2900->SetBinContent(i+1,samplesD[i]);
	  for(Int_t i=0;i<fCFShift;i++) hCF2900->SetBinContent(i+1,0.);
	  for(Int_t i=fCFShift;i<1024;i++) hCF2900->SetBinContent(i+1,samplesD[i-fCFShift]-fCFRatio*samplesD[i]);
	}

	// Compute channel time point using constant fraction

	// Apply constant fraction to waveform to look for precise hit time
	Double_t tPoint = 0.;
	Double_t val;
	Double_t oldVal = 0.;
	Bool_t signalZone = false;
	for(Int_t i=fCFShift;i<1024;i++) {
	  val = samplesD[i-fCFShift]-fCFRatio*samplesD[i];
	  if (signalZone) {
	    // Check if we crossed the constant fraction threshold
	    if ( (oldVal < fCFThreshold) && (val >= fCFThreshold) ) {
	      // Linearly interpolate the two bins to find precise time
	      tPoint = (Double_t)i-(val-fCFThreshold)/(val-oldVal);
	      break;
	    }
	  } else {
	    // Check if we entered the signal rise zone
	    //if (val < -3.*pedrms) signalZone = true;
	    if (val < -100.) signalZone = true;
	  }
	  oldVal = val;
	}
	tNtuple.SamplesPoint[tNtuple.NChannels] = tPoint;

	// Save final time value for this channel
	Double_t chTime = TTS->GetTimeDifference(boardId,groupId,tPoint);
	tNtuple.ChannelTime[tNtuple.NChannels] = chTime;
	if (boardId == 29 && channelId == 0) refTime = chTime; 

	// Save time difference wrt reference channel
	Double_t deltaT = chTime-refTime;
	if (boardId == 29 && channelId ==  1) hDiff2901->Fill(deltaT);
	if (boardId == 29 && channelId ==  8) hDiff2908->Fill(deltaT);
	if (boardId == 29 && channelId == 16) hDiff2916->Fill(deltaT);
	if (boardId == 29 && channelId == 24) hDiff2924->Fill(deltaT);
	if (boardId == 30 && channelId ==  0) hDiff3000->Fill(deltaT);
	if (boardId == 30 && channelId ==  8) hDiff3008->Fill(deltaT);
	if (boardId == 30 && channelId == 16) hDiff3016->Fill(deltaT);
	if (boardId == 30 && channelId == 24) hDiff3024->Fill(deltaT);

	tNtuple.NChannels++;

      }

    }

    hNtuple->Fill();

    // Clear event
    rawEv->Clear("C");
    
  } // End loop over events

  delete rawEv;

  HS->Finalize();
  TTS->Finalize();

  exit(0);

}
