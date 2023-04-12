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
#include "TF1.h"

#include "HistoSvc.hh"
#include "utlConfigParser.hh"
#include "PadmeVRecoConfig.hh"

#include "TRawEvent.hh"

#include "TemplateFitting.hh"

int main(int argc, char* argv[])
{
  
  int c;
  int nevents = 0;

  TString inputFileName;
  TObjArray inputFileNameList;
  TString outputFileName("WaveformAnalysis.root");
  TString configFileName("config/WaveformAnalysis.cfg");
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
  
  // Get verbose level from config file
  Int_t verbose = config->GetParOrDefault("Output", "Verbose", 0);

  // Initialize output file using HistoSvc
  HistoSvc* HS = HistoSvc::GetInstance();
  HS->SetVerbose(verbose);
  HS->Initialize(outputFileName);
  HS->CreateDir("WF");

  // Define parameters to use for hit building process
  Int_t fGoodSamples = config->GetParOrDefault("RECO","GoodSamples",994);
  Int_t fPedestalSamples = config->GetParOrDefault("RECO","PedestalSamples",100);
  Double_t fPedestalRMSThreshold = config->GetParOrDefault("RECO","PedestalRMSThreshold",5.);
  Double_t fZeroSupRMSThreshold = config->GetParOrDefault("RECO","ZeroSupRMSThreshold",5.);

  // Create and configure template histogram fitters
  FitTemp8Par* fitter8 = new FitTemp8Par();
  fitter8->SetVerbose(verbose);
  fitter8->SetGoodSamples(fGoodSamples);
  FitTemp7Par* fitter7 = new FitTemp7Par();
  fitter7->SetVerbose(verbose);
  fitter7->SetGoodSamples(fGoodSamples);

  // Create global histograms
  TH1D* hMix  = HS->BookHisto("WF","TauMix","Tau mixing parameter",100,0.,1.);
  TH1D* hFast = HS->BookHisto("WF","TauFast","Tau of fast BGO component",100,0.,200.);
  TH1D* hSlow = HS->BookHisto("WF","TauSlow","Tau of slow BGO component",100,0.,500.);

  // Create histogram to hold waveform
  //TH1D* hWF = HS->BookHisto("WF",Form("WF%04d",detChannel),Form("Channel %04d",detChannel),1024,0.,1024.);
  TH1D* hWF = new TH1D("WF","Waveform",1024,0.,1024.);

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

    TADCBoard* board;
    TADCChannel* channel;
    //TADCTrigger* group;

    // Loop over boards and check if it must be analyzed
    for(UChar_t iBoard = 0; iBoard < rawEv->GetNADCBoards(); iBoard++) {

      board = rawEv->ADCBoard(iBoard);
      UChar_t boardId = board->GetBoardId();
      if (config->BoardIsMine(boardId)) {

	//printf("Board %d is mine\n",boardId);

	// Loop over channels and check if they are active
	for(UChar_t ich = 0; ich < board->GetNADCChannels();ich++) {

	  channel = board->ADCChannel(ich);
	  UChar_t channelId = channel->GetChannelNumber();

	  // A few channels in the boards are not used (set to -1): skip them
	  Int_t detChannel = config->GetDetectorChannel(boardId,channelId);
	  if (detChannel == -1) continue;

	  // Convert samples to double
	  Double_t samplesD[1024];
	  for(UInt_t i=0; i<1024; i++) samplesD[i] = (Double_t)channel->GetSample(i);

	  // Get RMS of "good" samples and use it to reject empty waveforms (poor man's zero suppression)
	  Double_t rms = TMath::RMS(fGoodSamples,samplesD);
	  if (rms<fZeroSupRMSThreshold) continue; // Do not analyze "empty" channels

	  // Compute pedestal
	  Double_t pedestal = TMath::Mean(fPedestalSamples,samplesD);

	  // Check if pedestal samples are flat
	  Double_t pedrms = TMath::RMS(fPedestalSamples,samplesD);
	  if (pedrms > fPedestalRMSThreshold) {
	    printf("WARNING - Channel %d has anomalous pedestal RMS of %f\n",channelId,pedrms);
	  }

	  // Subtract pedestal and convert samples to positive signal in mV
	  Double_t ybinD[1024];
	  for(Int_t i=0; i<1024; i++) ybinD[i] = -(samplesD[i]-pedestal)/4096.*1000.;

	  // Copy waveform to histogram for fitting. Also find max value and its position
	  Int_t fWFMaxBin = 0;
	  Double_t fWFMaxVal = 0.;
	  hWF->Reset();
	  for(Int_t i=0; i<1024; i++) {
	    hWF->SetBinContent(i+1,ybinD[i]);
	    if ( (i<fGoodSamples) && (ybinD[i] > fWFMaxVal) ) {
	      fWFMaxVal = ybinD[i];
	      fWFMaxBin = i+1;
	    }
	  }
	  if (verbose) printf("\nBoard %2d Channel %2d DetChan %4d Max bin %2d Max val %f\n",
			      boardId,channelId,detChannel,fWFMaxBin,fWFMaxVal);

	  /*
	  // Fit the histogram with 8 parameters (initial parabola)
	  fitter8->SetWaveformHisto(hWF);
	  fitter8->SetScale(fWFMaxVal);
	  fitter8->SetShift((Double_t)(fWFMaxBin-10));
	  fitter8->SetLStart(5.);
	  fitter8->SetLLength(10.);
	  fitter8->SetDeltaMax(5.);
	  fitter8->SetTauMix(1.);
	  fitter8->SetTauFast(60.);
	  fitter8->SetTauSlow(300.);
	  fitter8->DoFit("");
	  */

	  // Fit the histogram with 7 parameters
	  fitter7->SetWaveformHisto(hWF);
	  fitter7->SetScale(fWFMaxVal);
	  fitter7->SetShift((Double_t)(fWFMaxBin-10));
	  fitter7->SetLLength(10.);
	  fitter7->SetDeltaMax(5.);
	  fitter7->SetTauMix(1.);
	  fitter7->SetTauFast(60.);
	  fitter7->SetTauSlow(300.);
	  fitter7->DoFit("+"); // Keep previous function

	  // Fill global histograms if fit converged and scale is large
	  if ( (fitter7->GetFitStatus() == 0) && (fitter7->GetScale() > 100.) ) {
	    hMix->Fill(fitter7->GetTauMix());
	    hFast->Fill(fitter7->GetTauFast());
	    hSlow->Fill(fitter7->GetTauSlow());
	  }

	}

      }

    }

    // Clear event
    rawEv->Clear("C");
    
  } // End loop over events

  delete rawEv;

  HS->Finalize();

  exit(0);

}
