#include "TRandom.h"
#include "TMarker.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TChain.h"
#include "TTreeIndex.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TMath.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TSpline.h"
#include "TString.h"
#include "TRawEvent.hh"
#include "TF1.h"
#include "TButton.h"
#include "TTimeStamp.h"
#include <TSystem.h>

#include "Riostream.h"
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <sys/stat.h>

#include <iostream>
#include <ctime>

#define NTUPLE_N_BOARDS 2
#define NTUPLE_N_CHANNELS 32
#define VPP 1.

using namespace std;

///////////// TARGET 
//. ADC mapping
// ADC28  15 13 11 9 7 5 3 1 0 2 4 6 8 10 12 14 22 20 18 16 17 19 21 23 25 27 29 31 30 28 26 24
/////////////

///////////// LeadGlass - combined between Run £ config files and OnlineMonitor Run4 config files
// [DETECTOR]
// NAME    LeadGlass
// READOUT ADC
// NCHANNELS 1

// [ADC]
// NADC  1
// ID    14

// #Format of the next lines: ADC$ID - list of channels (-1: channel not used)
// ADC14 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1  1

// [RECO]

// PedestalSamples    100
// SignalSamplesStart 100 
// SignalSamplesEnd   600

// ChargeToNPoTs 0.239

// BunchLengthThreshold 50.
///////////////

// first 0-15 are X channels, last 16-31 are Y channels
int PhysicsChannels[NTUPLE_N_CHANNELS] = {15,13,11,9,7,5,3,1,0,2,4,6,8,10,12,14,22,20,18,16,17,19,21,23,25,27,29,31,30,28,26,24};
int PhysicstoADC[NTUPLE_N_CHANNELS] = {8,7,9,6,10,5,11,4,12,3,13,2,14,1,15,0,19,20,18,21,17,22,16,23,31,24,30,25,29,26,28,27};

const int NPED_target = 100; //number
const int NPED_LG = 50;
static const int NAVG = 1000;
static const int NSAMPLETIME = 1024;

const double DigThre = 1000; //mV digitizer saturation threshold
const double FEEThre = 900; //mV, FEE saturation
const double fImpedance = 50.;
  
const double digiTime = 1.; // Sampled at 5 GS/s  ---> da prendere dalla rootupla
const double fTimeBin = digiTime*1e-9; // seconds

const int NChannels = 32; 

int go_ahead_flag = 0;
int write_to_file_flag = 1;

struct Eve{
  Int_t NTNevent;
  ULong64_t NTEventTime;
  // Double_t NTAbsDateEvent;
  // Double_t NTAbsTimeEvent;
  Double_t NTAbsTimeEventUNIX;
  Double_t NTQChOld[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTQCh1WfSliding[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTQCh1WfSlidingCNCorr[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTQCh1WfFixed[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTQCh1WfFixedCNCorr[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTQChCumSliding[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTQChCumSlidingCNCorr[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTQChCumFixed[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTQChCumFixedCNCorr[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t PedChOld[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t PedCh1Wf[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t PedCh1WfCN[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t PedChCum[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTVMaxOld[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTVMax1Wf[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTVMax1WfCNCorr[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTVMaxCum[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTVMaxCumCNCorr[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTTMaxOld[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTTMax1Wf[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTTMax1WfCNCorr[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTTMaxCum[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTTMaxCumCNCorr[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  // Double_t WavesOld[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS][NSAMPLETIME];
  // Double_t Waves1Wf[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS][NSAMPLETIME];
  // Double_t Waves1WfCNCorr[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS][NSAMPLETIME];
  // Double_t Waves1WfCN[NTUPLE_N_BOARDS][NSAMPLETIME];
  // Double_t WavesCumCN[NTUPLE_N_BOARDS][NSAMPLETIME];
  // Double_t WavesCum[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS][NSAMPLETIME];
  // Double_t WavesCumCNCorr[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS][NSAMPLETIME];
  Double_t SampleTime[NSAMPLETIME];
  // Bool_t Saturation[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS]; //saturation evaluation
  Int_t NTTrigMask;
};

void go_ahead(){
  go_ahead_flag = 1;
}

int fillTGraph(TGraphErrors *graph, int i, double x, double y, double dx, double dy) {
  graph->SetPoint(i, x, y);
  int n = graph->GetN();
  graph->SetPointError(n-1, dx, dy);
  graph->SetName("graph");
  return n;
}

bool IsRemoteFile(const TString& name)
{
  return name.BeginsWith("root://") ||
         name.BeginsWith("http://") ||
         name.BeginsWith("https://") ||
         name.BeginsWith("xroot://") ||
         name.BeginsWith("davs://");
}

int main(int argc, char* argv[]) {
  
  int c;
  int verbose = 0;
  int nevents = 0;

  TString inputFileName;
  TObjArray inputFileNameList;
  struct stat filestat;

  TString outputFileName = "RawHisto.root";
  Int_t event;
  vector<UInt_t> events;
  Int_t board;
  vector<UInt_t> boards;

  // Parse options
  while ((c = getopt (argc, argv, "i:l:o:n:e:b:vh")) != -1) {
    switch (c){
      case 'i':
        inputFileNameList.Add(new TObjString(optarg));
        fprintf(stdout,"Added input data file '%s'\n",optarg);
	      break;
      case 'l':
        if ( stat(optarg, &filestat) == 0 || IsRemoteFile(optarg) ) {

          fprintf(stdout,"Reading list of input files from '%s'\n",optarg);
          ifstream inputList(optarg);

          while( inputFileName.ReadLine(inputList) ){

            TString fname = inputFileName.Data();

            if ( IsRemoteFile(fname) || stat(fname.Data(), &filestat) == 0 ) {

              inputFileNameList.Add(new TObjString(fname));
              fprintf(stdout,"Added input data file '%s'\n",fname.Data());

            } else {
              fprintf(stdout,"WARNING: file '%s' is not accessible\n",fname.Data());
            }
          }
        } else {
          fprintf(stdout,"WARNING: file list '%s' is not accessible\n",optarg);
        }
        break;
      case 'o':
        outputFileName = optarg;
        fprintf(stdout,"Output histogram file set to '%s'\n",optarg);
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
      case 'e':
        if ( sscanf(optarg,"%d",&event) != 1 ) {
          fprintf (stderr, "Error while processing option '-e'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        if (event<0) {
          fprintf (stderr, "Error while processing option '-e'. Required event %d (must be >=0).\n", event);
          exit(1);
        }
        events.push_back(event);
        fprintf(stdout,"Added event %d to list\n",event);
        break;
      case 'b':
        if ( sscanf(optarg,"%d",&board) != 1 ) {
          fprintf (stderr, "Error while processing option '-b'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        if (board<0 || board>31) {
          fprintf (stderr, "Error while processing option '-b'. Required board %d (must be 0<=board<=31).\n", board);
          exit(1);
        }
        boards.push_back(board);
        fprintf(stdout,"Added board %d to list\n",board);
        break;
      case 'v':
	      verbose++;
        break;
      case 'h':
        fprintf(stdout,"\nRawHisto [-i <file>] [-l <list>] [-o <file>] [-n <n>] [-e <event>] [-b <board>] [-v <level>] [-h]\n\n");
        fprintf(stdout,"  -i: add a file to list of input files (can be repeated)\n");
        fprintf(stdout,"  -l: specify text file with list of input files (one file per line)\n");
        fprintf(stdout,"  -o: define the name of the output file (default: %s)\n",outputFileName.Data());
        fprintf(stdout,"  -n: total number of events to save to output file (default: 0 i.e. no limit)\n");
        fprintf(stdout,"  -e: add <event> to list of events to save to output file (can be repeated)\n");
        fprintf(stdout,"  -b: add <board> to list of boards to save to output file (can be repeated)\n");
        fprintf(stdout,"  -v: enable verbose output (repeat for more output)\n");
        fprintf(stdout,"  -h: show this help message and exit\n\n");
        fprintf(stdout,"N.B. if no -e/-b options are specified, then all events/boards will be saved to the output file\n\n");
        exit(0);
      case '?':
        if (optopt == 'i' || optopt == 'l' || optopt == 'o' || optopt == 'n' || optopt == 'e' || optopt == 'b')
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

  // Check if the input file list is empty
  if ( inputFileNameList.GetEntries() == 0 ) {
    fprintf(stderr,"ERROR - Input file list is empty\n");
    exit(1);
  }

  if (verbose) fprintf(stdout,"Set verbose level to %d\n",verbose);

  // Create chain of input files
  fprintf(stdout,"=== === === Chain of input files === === ===\n");
  TChain* inputChain = new TChain("RawEvents");
  for (Int_t iFile = 0; iFile < inputFileNameList.GetEntries(); iFile++) {
    fprintf(stdout,"%4d %s\n",iFile,((TObjString*)inputFileNameList.At(iFile))->GetString().Data());
    inputChain->AddFile(((TObjString*)inputFileNameList.At(iFile))->GetString());
  }
  if (inputChain->GetEntries() == 0) {
    fprintf(stderr,"ERROR - Tree 'RawEvents' in input chain has 0 entries\n");    
    exit(1);
  }

  // Get some info about the input chain
  Long64_t runNEntries = inputChain->GetEntries();
  cout << "Found Tree 'RawEvents' with " << runNEntries << " entries" << endl;

  TRawEvent* rawEv = new TRawEvent();
  inputChain->SetBranchAddress("RawEvent",&rawEv);

  // Create ntuple -- maybe not needed in this case?
  Eve Event; 
    
  TTree* tree = new TTree("NTU","Event3");
  tree->Branch("Nevent",&(Event.NTNevent),"Nevent/I");
  tree->Branch("EventTime",&(Event.NTEventTime),"EventTime/l");
  // tree->Branch("AbsDateEvent",&(Event.NTAbsDateEvent),"AbsDateEvent/D");
  // tree->Branch("AbsTimeEvent",&(Event.NTAbsTimeEvent),"AbsTimeEvent/D");
  tree->Branch("AbsTimeEventUNIX",&(Event.NTAbsTimeEventUNIX),"AbsTimeEventUNIX/D");
  tree->Branch("QChOld",&(Event.NTQChOld),Form("QChOld[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("QCh1WfSliding",&(Event.NTQCh1WfSliding),Form("QCh1WfSliding[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("QCh1WfSlidingCNCorr",&(Event.NTQCh1WfSlidingCNCorr),Form("QCh1WfSlidingCNCorr[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("QCh1WfFixed",&(Event.NTQCh1WfFixed),Form("QCh1WfFixed[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("QCh1WfFixedCNCorr",&(Event.NTQCh1WfFixedCNCorr),Form("QCh1WfFixedCNCorr[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("QChCumSliding",&(Event.NTQChCumSliding),Form("QChCumSliding[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("QChCumSlidingCNCorr",&(Event.NTQChCumSlidingCNCorr),Form("QChCumSlidingCNCorr[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("QChCumFixed",&(Event.NTQChCumFixed),Form("QChCumFixed[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("QChCumFixedCNCorr",&(Event.NTQChCumFixedCNCorr),Form("QChCumFixedCNCorr[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  // tree->Branch("PedChOld",&(Event.PedChOld),Form("PedChOld[%d][%d]/D,",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  // tree->Branch("PedCh1Wf",&(Event.PedCh1Wf),Form("PedCh1Wf[%d][%d]/D,",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  // tree->Branch("PedCh1WfCN",&(Event.PedCh1WfCN),Form("PedCh1WfCN[%d][%d]/D,",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  // tree->Branch("PedChCum",&(Event.PedChCum),Form("PedChCum[%d][%d]/D,",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("VMaxOld",&(Event.NTVMaxOld),Form("VMaxOld[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("VMax1Wf",&(Event.NTVMax1Wf),Form("VMax1Wf[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("VMax1WfCNCorr",&(Event.NTVMax1WfCNCorr),Form("VMax1WfCNCorr[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("VMaxCum",&(Event.NTVMaxCum),Form("VMaxCum[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("VMaxCumCNCorr",&(Event.NTVMaxCumCNCorr),Form("VMaxCumCNCorr[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("TMaxOld",&(Event.NTTMaxOld),Form("TMaxOld[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("TMax1Wf",&(Event.NTTMax1Wf),Form("TMax1Wf[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("TMax1WfCNCorr",&(Event.NTTMax1WfCNCorr),Form("TMax1WfCNCorr[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("TMaxCum",&(Event.NTTMaxCum),Form("TMaxCum[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("TMaxCumCNCorr",&(Event.NTTMaxCumCNCorr),Form("TMaxCumCNCorr[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  // tree->Branch("WavesOld",&(Event.WavesOld), Form("WavesOld[%d][%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS,NSAMPLETIME));
  // tree->Branch("Waves1Wf",&(Event.Waves1Wf), Form("Waves1Wf[%d][%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS,NSAMPLETIME));
  // tree->Branch("Waves1WfCN",&(Event.Waves1WfCN), Form("Waves1WfCN[%d][%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS,NSAMPLETIME));
  // tree->Branch("WavesCumCN",&(Event.WavesCumCN), Form("WavesCumCN[%d][%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS,NSAMPLETIME));
  // tree->Branch("Waves1WfCNCorr",&(Event.Waves1WfCNCorr), Form("Waves1WfCNCorr[%d][%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS,NSAMPLETIME));
  // tree->Branch("WavesCum",&(Event.WavesCum), Form("WavesCum[%d][%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS,NSAMPLETIME));
  // tree->Branch("WavesCumCNCorr",&(Event.WavesCumCNCorr), Form("WavesCumCNCorr[%d][%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS,NSAMPLETIME));
  tree->Branch("SampleTime",&(Event.SampleTime),Form("SampleTime[%d]/D", NSAMPLETIME));
  // tree->Branch("Saturation",&(Event.Saturation),Form("Saturation[%d][%d]/B",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("TrigMask",&(Event.NTTrigMask),Form("TrigMask/I"));
  
  // Create output file for histograms
  TFile* histoFile = new TFile(outputFileName,"RECREATE");
  if(!histoFile) {
    fprintf(stderr,"ERROR - Cannot create output file %s\n",outputFileName.Data());
    exit(1);
  }

  // Define parameters for signal analysis  
  Double_t Sample[NTUPLE_N_CHANNELS][NSAMPLETIME];
  Double_t AbsRecoSampleOld[NSAMPLETIME];
  Double_t AbsRecoSample1Wf[NTUPLE_N_CHANNELS][NSAMPLETIME];
  Double_t AbsRecoSampleCNXStrips[NSAMPLETIME];
  Double_t AbsRecoSampleCNYStrips[NSAMPLETIME];
  Double_t AbsRecoSample1WfCNXStripsCorr[NTUPLE_N_CHANNELS][NSAMPLETIME];
  Double_t AbsRecoSample1WfCNYStripsCorr[NTUPLE_N_CHANNELS][NSAMPLETIME];
  Double_t SampleCum[NTUPLE_N_CHANNELS][NSAMPLETIME];
  Double_t AbsRecoSampleCum[NTUPLE_N_CHANNELS][NSAMPLETIME];
  Double_t AbsRecoSampleCumCNXStripsCorr[NTUPLE_N_CHANNELS][NSAMPLETIME];
  Double_t AbsRecoSampleCumCNYStripsCorr[NTUPLE_N_CHANNELS][NSAMPLETIME];
    
  Int_t readEvent = 0;
  
  //cumulative waveform analysis variables
  Double_t baselineSumCum[NTUPLE_N_CHANNELS] = {0}; 
  Double_t baseNCum[NTUPLE_N_CHANNELS] = {0};
  Double_t ChargeCumSliding[NTUPLE_N_CHANNELS] = {0};
  Double_t ChargeCumFixed[NTUPLE_N_CHANNELS] = {0};
  Double_t PedTempCum[NTUPLE_N_CHANNELS] = {0};

  int NWFcumulative = 100; //number of waveforms to be summed for cumulative waveform analysis
  int nsum = 0; //counter for cumulative waveforms
  int nRND = 0; //counter for notBTF waveforms

  // maximum of X and Y strips
  Double_t VMax1Wf_chan[NTUPLE_N_CHANNELS] = {0.};

  std::vector<std::pair<int, Long64_t>> evmap;
  evmap.reserve(runNEntries);

  for (Long64_t i = 0; i < runNEntries; ++i) {
      inputChain->GetEntry(i);
      if(!rawEv->GetEventNumber()) {
        fprintf(stderr,"ERROR - Event number is 0 for entry %lld\n", i);
        continue;
      }
      evmap.emplace_back(rawEv->GetEventNumber(), i);
  }

  std::sort(evmap.begin(), evmap.end(), [](const std::pair<int, Long64_t> &a, const std::pair<int, Long64_t> &b) {return a.first < b.first;});

  for (Long64_t iev = 0; iev < runNEntries; ++iev) {

    // Reset scalar event fields
    Event.NTNevent = -999;
    Event.NTEventTime = 0;
    Event.NTAbsTimeEventUNIX = -999.;
    Event.NTTrigMask = -999;

    // Reset arrays
    for (int ib = 0; ib < NTUPLE_N_BOARDS; ++ib) {
      for (int ich = 0; ich < NTUPLE_N_CHANNELS; ++ich) {

        Event.NTQChOld[ib][ich] = -999.;
        Event.NTQCh1WfSliding[ib][ich] = -999.;
        Event.NTQCh1WfSlidingCNCorr[ib][ich] = -999.;
        Event.NTQCh1WfFixed[ib][ich] = -999.;
        Event.NTQCh1WfFixedCNCorr[ib][ich] = -999.;
        Event.NTQChCumSliding[ib][ich] = -999.;
        Event.NTQChCumSlidingCNCorr[ib][ich] = -999.;
        Event.NTQChCumFixed[ib][ich] = -999.;
        Event.NTQChCumFixedCNCorr[ib][ich] = -999.;

        Event.PedChOld[ib][ich] = -999.;
        Event.PedCh1Wf[ib][ich] = -999.;
        Event.PedCh1WfCN[ib][ich] = -999.;
        Event.PedChCum[ib][ich] = -999.;

        Event.NTVMaxOld[ib][ich] = -999.;
        Event.NTVMax1Wf[ib][ich] = -999.;
        Event.NTVMax1WfCNCorr[ib][ich] = -999.;
        Event.NTVMaxCum[ib][ich] = -999.;
        Event.NTVMaxCumCNCorr[ib][ich] = -999.;

        Event.NTTMaxOld[ib][ich] = -999.;
        Event.NTTMax1Wf[ib][ich] = -999.;
        Event.NTTMax1WfCNCorr[ib][ich] = -999.;
        Event.NTTMaxCum[ib][ich] = -999.;
        Event.NTTMaxCumCNCorr[ib][ich] = -999.;

        // Event.Saturation[ib][ich] = false;
      }
    }
    for (int s = 0; s < NSAMPLETIME; ++s) {
      Event.SampleTime[s] = -999.;
    }
    // reset temporary per-event buffers
    for (int ch = 0; ch < NTUPLE_N_CHANNELS; ++ch) {
      VMax1Wf_chan[ch] = -999.;

      for (int s = 0; s < NSAMPLETIME; ++s) {
        Sample[ch][s] = 0.;
        AbsRecoSample1Wf[ch][s] = 0.;
        AbsRecoSample1WfCNXStripsCorr[ch][s] = 0.;
        AbsRecoSample1WfCNYStripsCorr[ch][s] = 0.;
      }
    }

    for (int s = 0; s < NSAMPLETIME; ++s) {
      AbsRecoSampleOld[s] = 0.;
      AbsRecoSampleCNXStrips[s] = 0.;
      AbsRecoSampleCNYStrips[s] = 0.;
    }
  
    if (evmap[iev].second < 0) {
      fprintf(stderr,"ERROR - Invalid entry index for event map\n");
      continue;
    }
    // Long64_t entryIndex = evmap[iev].second; // sorted entry
    inputChain->GetEntry(evmap[iev].second); // proper time synchronization
    // cout << "sorted index: " << iev << "  event#: " << rawEv->GetEventNumber() << "  originalEntry: " << entryIndex << endl;
    // inputChain->GetEntry(iev); // proper mapping commented for HV scan 
    // Bool_t Saturated = false;
    UInt_t trigMask = rawEv->GetEventTrigMask();
    if (trigMask != 1) {
      nRND++;
      continue;
    }
    Event.NTTrigMask = trigMask;
    nsum++;

    // Access your event object as usual
    Int_t runNumber = rawEv->GetRunNumber();
    if(!runNumber) {
      fprintf(stderr,"ERROR - Run number is 0\n");
      continue;
    }
    Int_t evtNumber = rawEv->GetEventNumber();
    if(!evtNumber) {
      fprintf(stderr,"ERROR - Event number is 0\n");
      continue;
    }

    ULong64_t EvT = rawEv->GetEventRunTime();
    Double_t AbsTimeEvUNIX = rawEv->GetEventAbsTime().AsDouble(); // UNIX TIME
    // AbsTimeEvUNIX.Print();

    // Check if this event number was selected
    if ( (events.size() > 0) && (count(events.begin(), events.end(), evtNumber) == 0) ) {
      continue;
    }
    readEvent++;
    if ( nevents && (readEvent>nevents) ) {
      printf("- Read %d event(s): stopping here\n",nevents);
      break;
    }

    // Show some activity meter
    if ( readEvent % 1000 == 0 ) {
      printf("Run: %7d ------> Processing event: %8d/%d (%.0f%%)\n",runNumber,readEvent,runNEntries,(float)(100*readEvent/runNEntries));
    }

    // Get event trigger mask and select cosmics events
    // UInt_t trigMask = rawEv->GetEventTrigMask();
    // if ( !(trigMask & 0x01) ) continue; 
    // Event.NTTrigMask= trigMask;
    // cout << "Event number: " << iev << " - Trigger mask: " << trigMask << endl;
    // if(trigMask == 1) nsum++;
    // if(trigMask != 1) nRND++;
    
    //channels where the maximum is
    Int_t Xchan_MAX = 0;
    Int_t Ychan_MAX = 0;

    // Loop over boards

    for(UChar_t brd = 0; brd<rawEv->GetNADCBoards(); brd++){
      // Check if this board was selected
      if (!rawEv->ADCBoard(brd)){
        fprintf(stderr,"ERROR - no Board ID registered\n");
        continue;
      }
      Int_t brdID = rawEv->ADCBoard(brd)->GetBoardId();
      // cout << "brdID= " << (int)brdID << endl;
      if ( (boards.size() > 0) && (count(boards.begin(),boards.end(),brdID) == 0) ){
        continue;
      }
      if (brdID != 14 && brdID != 28){
        continue;
      }

      TADCBoard* adcB = rawEv->ADCBoard(brd); 
      if(!adcB){
        fprintf(stderr,"ERROR - ADCBoard pointer is null\n");
        continue;
      }
      // UChar_t nTrg = adcB->GetNADCTriggers();
      UChar_t nChn = adcB->GetNADCChannels(); // hardcoded selecting only 1 channel, the other one is just pedestal 
      if(!nChn){
        fprintf(stderr,"ERROR - no channels available for board %d\n", brdID);
        continue;
      }
      // cout << "number of available channels = " << (int)adcB->GetNADCChannels() << endl;

      // Loop over the channels
      for(UChar_t ch=0; ch<nChn; ch++){
        Double_t ChargeOld = 0.;
        Double_t PedTempOld = 0.;
        Double_t Charge1WfSliding = 0.;
        Double_t Charge1WfFixed = 0.;        
        Double_t PedTemp1Wf = 0.;

        TADCChannel* ADCChn = adcB->ADCChannel(ch);
        if (!ADCChn){
          fprintf(stderr,"ERROR - ADCChannel pointer is null for board %d channel %d\n",brdID, ch);
          continue;   // check that the pointer exists        
        } 
        UChar_t chan = ADCChn->GetChannelNumber();
        // cout << "brdID: " << brdID << << "chan: " << (int)chan << endl;
        if (brdID == 14 && chan != 31) continue;
        if(trigMask!=1) continue;
        // Loop over the samples
        for(UShort_t s=0; s<NAVG; s++){
          Sample[chan][s] = (Double_t)ADCChn->GetSample(s);
          SampleCum[chan][s] += (Double_t)ADCChn->GetSample(s);
        }

        // Compute the PEDESTAL for the four methods: old Reco one - 1 waveform - TMaxCut -  cumulative
        Double_t baseFrom = 0;
        Double_t baselineSumOld{0},baseNOld{0};
        Double_t baselineSum1Wf{0},baseN1Wf{0};

        for(Int_t s = baseFrom; s < NPED_target + baseFrom; s++) {
          if(brdID == 14){
            if(s<NPED_LG){
              baselineSumOld += Sample[chan][s];
              baseNOld++;
            }
          } if(brdID == 28){
            if(s<NPED_target){
              baselineSumOld += Sample[chan][s];
              baseNOld++;
            }
            baselineSum1Wf += Sample[chan][s];
            baseN1Wf++;
            if(nsum>0 && nsum%NWFcumulative==0){
              baselineSumCum[chan] += SampleCum[chan][s];    
              baseNCum[chan]++;
            }
          }
        }
        if(brdID == 14){
          PedTempOld = (Double_t)baselineSumOld/(Double_t)baseNOld;
          // cout << "baselineSumOld: " << baselineSumOld << " - baseNOld: " << baseNOld << " - Pedestal PedTempOld: " << PedTempOld << endl;
        } if(brdID == 28){
          PedTempOld = (Double_t)baselineSumOld/(Double_t)baseNOld;
          PedTemp1Wf = (Double_t)baselineSum1Wf/(Double_t)baseN1Wf;
          if(nsum>0 && nsum%NWFcumulative==0){
            PedTempCum[chan] = (Double_t)baselineSumCum[chan]/(Double_t)baseNCum[chan];
            baselineSumCum[chan] = 0;
            baseNCum[chan] = 0;
          }
        }
        
        // Get total charge and position of maximum
        Double_t VMaxOld = -999.;
        Double_t TMaxOld = -999.;
        Double_t VMax1Wf = -999.;
        Double_t TMax1Wf = -999.;
        Double_t VMaxCum = -999.;
        Double_t TMaxCum = -999.;

        // Loop over the samples defining the RECO waveforms for each method
        for(UShort_t s=0; s<NAVG; s++){
          if(brdID == 14){ //LG has negative wf
            AbsRecoSampleOld[s] = VPP*(PedTempOld - Sample[chan][s]); // /4096.*1000. //Signal NOT IN mV
            if (AbsRecoSampleOld[s] > VMaxOld) {  
              VMaxOld = AbsRecoSampleOld[s];
              TMaxOld = (Double_t)s;
              // cout << "LOOP FILLING: VMaxOld: " << VMaxOld << " - TMaxOld: " << TMaxOld << endl;
            }
          } if(brdID == 28){
            // Sample[chan][s] = (Double_t)ADCChn->GetSample(s);   
            AbsRecoSampleOld[s] = VPP*(Sample[chan][s]-PedTempOld)/4096.*1000.; //Signal in mV
            if (AbsRecoSampleOld[s] > VMaxOld) {  
              VMaxOld = AbsRecoSampleOld[s];
              TMaxOld = (Double_t)s;
            }
            AbsRecoSample1Wf[chan][s] = VPP*(Sample[chan][s]-PedTemp1Wf)/4096.*1000.; //Signal in mV       
            if (AbsRecoSample1Wf[chan][s] > VMax1Wf) {  
              VMax1Wf = AbsRecoSample1Wf[chan][s];
              TMax1Wf = (Double_t)s;
            }
            if(nsum>0 && nsum%NWFcumulative==0){
              // cout << "TrigMask = " << trigMask << endl;
              AbsRecoSampleCum[chan][s] = (VPP*(SampleCum[chan][s]-PedTempCum[chan])/4096.*1000.)/nsum; //Signal in mV
              SampleCum[chan][s] = 0;
              // fillTGraph(&WaveGraphCum, s, s, AbsRecoSampleCum[chan][s], 0, 0);
              if (AbsRecoSampleCum[chan][s] > VMaxCum) {
                VMaxCum = AbsRecoSampleCum[chan][s];
                TMaxCum = (Double_t)s;
              }
            }
          }
        } // end of loop on samples
        //filling the vector of the maximum of every channel
        VMax1Wf_chan[chan] = VMax1Wf;

        //Check saturation HERE
        // if(VMaxOld>FEEThre) Saturated = true; // WARNING : very preliminary saturation check, need to improve
        // saturation properly evaluated: doing VMax<900mV plays the same role of asking a cut directly on ADCChn->GetSample(s);   

        //Compute CHARGES
        //check time integration start-stop!
        // negli header dei rawdata ci sono nsample e sampling rate: sono dei bit (numeri salvati) facilmente accessibili con una funzioncina
        Int_t qStart1Wf = TMax1Wf-150/1.;  
        Int_t qStop1Wf = TMax1Wf+250/1.;   
        // if(TMax1Wf>380) { qStart1Wf = TMax1Wf-180/1.; qStop1Wf = TMax1Wf+220/1.; } // to avoid integrating fake signals out of the trigger window
        
        //Here evaluation of qStartCum-qStopCum with sigmoid fit
        Double_t qStartCum = TMaxCum - 150./1.;  
        Double_t qStopCum = TMaxCum + 250./1.;
        // if(TMax1Wf>380) { qStart1Wf = TMax1Wf-180/1.; qStop1Wf = TMax1Wf+220/1.; } // to avoid integrating fake signals out of the trigger window

        for(UShort_t t=0; t<NAVG; t++){
          // ChargeOld --> charge computation taken by line 504-507 of TargetReconstruction.cc
          if(brdID == 14){
            if(t>100 && t<600){ // LG integration window taken from PadmeReco config files & 
              ChargeOld += (AbsRecoSampleOld[t])*4.8828E-3; // renorm copied by the LeadglassReconstrction.cc
              // ChargeOld = PedTempOld*(600-100)-(TMath::Mean(NAVG,&Sample[chan][0])*NAVG); //1pC/1000??? what is 1.75???
            }
          } if(brdID == 28){
            if(t>200 && t<700){
              ChargeOld += AbsRecoSampleOld[t]/50*1E-9/1E-12/1000/1.75; //1pC/1000??? what is 1.75???
              if(AbsRecoSample1Wf[chan][t]<0) Charge1WfFixed += 0; // to avoid negative contributions due to noise
              Charge1WfFixed += (AbsRecoSample1Wf[chan][t]*1e-3)/fImpedance*fTimeBin/1E-12; //charge in pC
              if(nsum>0 && nsum%NWFcumulative==0){
                if(AbsRecoSampleCum[chan][t]<0) ChargeCumFixed[chan] += 0; // to avoid negative contributions due to noise
                ChargeCumFixed[chan] += (AbsRecoSampleCum[chan][t]*1e-3)/fImpedance*fTimeBin/1E-12; //charge in pC
              }
            }
            // Charge1Wf --> charge computation with 1 waveform
            // ChargeCum --> cumulative charge computation with NWFcumulative waveform 
            if(t>qStart1Wf && t<qStop1Wf){
              if(AbsRecoSample1Wf[chan][t]<0) Charge1WfSliding += 0; // to avoid negative contributions due to noise
              Charge1WfSliding += (AbsRecoSample1Wf[chan][t]*1e-3)/fImpedance*fTimeBin/1E-12; //charge in pC
            }
            
            if(nsum>0 && nsum%NWFcumulative==0){
              if(t>qStartCum && t<qStopCum){
                if(AbsRecoSampleCum[chan][t]<0) ChargeCumSliding[chan] += 0; // to avoid negative contributions due to noise
                ChargeCumSliding[chan] += (AbsRecoSampleCum[chan][t]*1e-3)/fImpedance*fTimeBin/1E-12; //charge in pC
              }
            }
          }
        }

        Int_t idx;
        if(brdID == 14){
          idx = 0; //hardcoded for LeadGlass board 
          Event.NTQChOld[idx][chan] = ChargeOld;
          // Event.NTQCh1WfSliding[idx][chan] = 0;
          // Event.NTQCh1WfFixed[idx][chan] = 0;
          // Event.PedChOld[idx][chan] = PedTempOld;
          // Event.PedCh1Wf[idx][chan] = 0;
          Event.NTVMaxOld[idx][chan] = VMaxOld;
          Event.NTTMaxOld[idx][chan] = TMaxOld * digiTime;
          // cout << "NTU: VMaxOld: " << VMaxOld << " - TMaxOld: " << TMaxOld << endl;
          // Event.NTVMax1Wf[idx][chan] = 0;
          // Event.NTTMax1Wf[idx][chan] = 0;
          // Event.NTVMaxCum[idx][chan] = 0;
          // Event.NTTMaxCum[idx][chan] = 0;
          // Event.Saturation[idx][chan] = Saturated;

          // if(nsum>0 && nsum%NWFcumulative==0){
          //   Event.NTQChCumSliding[idx][chan] = 0;
          //   Event.NTQChCumFixed[idx][chan] = 0;
          //   Event.PedChCum[idx][chan] = 0;
          // }
          // else {
          //   Event.NTQChCumSliding[idx][chan] = -999.;
          //   Event.PedChCum[idx][chan] = -999.;
          // }

          for(UShort_t s=0;s<NSAMPLETIME;s++){
          // waveforms storage in ntuple
            if(s<NAVG){
              // Event.WavesOld[idx][chan][s] = AbsRecoSampleOld[s];
              // Event.Waves1Wf[idx][chan][s] = 0;
              if(nsum>0 && nsum%NWFcumulative==0){
                // Event.WavesCum[idx][chan][s] = AbsRecoSampleCum[chan][s];
              } else {
                // Event.WavesCum[idx][chan][s] = 0.;
              }
              Event.SampleTime[s]=s*digiTime;
            } else {
              // Event.WavesOld[idx][chan][s] = 0.;
              // Event.Waves1Wf[idx][chan][s] = 0.;
              // Event.WavesCum[idx][chan][s] = 0.;
            }
          } 

        } if(brdID == 28) {
          idx = 1; //hardcoded for Target board
          Event.NTQChOld[idx][chan] = ChargeOld;
          Event.NTQCh1WfSliding[idx][chan] = Charge1WfSliding;
          Event.NTQCh1WfFixed[idx][chan] = Charge1WfFixed;
          // Event.PedChOld[idx][chan] = PedTempOld;
          // Event.PedCh1Wf[idx][chan] = PedTemp1Wf;
          Event.NTVMaxOld[idx][chan] = VMaxOld;
          Event.NTTMaxOld[idx][chan] = TMaxOld * digiTime;
          Event.NTVMax1Wf[idx][chan] = VMax1Wf;
          Event.NTTMax1Wf[idx][chan] = TMax1Wf * digiTime;
          Event.NTVMaxCum[idx][chan] = VMaxCum;
          Event.NTTMaxCum[idx][chan] = TMaxCum * digiTime;
          // Event.Saturation[idx][chan] = Saturated;

          if(nsum>0 && nsum%NWFcumulative==0){
            Event.NTQChCumSliding[idx][chan] = ChargeCumSliding[chan];
            Event.NTQChCumFixed[idx][chan] = ChargeCumFixed[chan];
            // Event.PedChCum[idx][chan] = PedTempCum[chan];
            ChargeCumSliding[chan]=0;
            ChargeCumFixed[chan]=0;
            PedTempCum[chan]=0;
          }
          else {
            Event.NTQChCumSliding[idx][chan] = -999.;
            Event.NTQChCumFixed[idx][chan] = -999.;
            // Event.PedChCum[idx][chan] = -999.;
          }

          for(UShort_t s=0;s<NSAMPLETIME;s++){
          // waveforms storage in ntuple
            if(s<NAVG){
              // Old[idx][chan][s] = AbsRecoSampleOld[s];
              // Event.Waves1Wf[idx][chan][s] = AbsRecoSample1Wf[chan][s];
              if(nsum>0 && nsum%NWFcumulative==0){
                // Event.WavesCum[idx][chan][s] = AbsRecoSampleCum[chan][s];
              } else {
                // Event.WavesCum[idx][chan][s] = 0.;
              }
              // Event.SampleTime[s]=s*digiTime;
            } else{
              // Event.WavesOld[idx][chan][s] = 0.;
              // Event.Waves1Wf[idx][chan][s] = 0.;
              // Event.WavesCum[idx][chan][s] = 0.;
            }
          }
        }
      }   // end loop on channels

      // looping again on channels and samples for common noise subtraction after having identified the channels where the beam is
      // work on this for the 1WF common noise subtraction --> if it works, move to cumulative and check if cumulative WV - Cumulative CN is better or not than 1WF-1WfCN to cumulative
      
      // loop over channels for common noise determination
      // channel where maximum is +- 3 strips - both in X and Y --> (3+3+1)x2 = 14
      
      // CMN subtraction ca work only on physics triggers
      
      vector<int> BeamXStrips, BeamYStrips;
      // vector<int> BeamXAdc, BeamYAdc;
      double Xmax = -999.;
      double Ymax = -999.;

      Int_t idx;
      // for(UChar_t chan=0; chan<nChn; chan++){
      //   idx = 0;
      //   if(brdID == 14){

      //     Event.NTQCh1WfSlidingCNCorr[idx][chan] = 0;
      //     Event.NTQCh1WfFixedCNCorr[idx][chan] = 0;
      //     Event.NTVMax1WfCNCorr[idx][chan] = 0;
      //     Event.NTTMax1WfCNCorr[idx][chan] = 0;
      //     Event.NTVMaxCumCNCorr[idx][chan] = 0;
      //     Event.NTTMaxCumCNCorr[idx][chan] = 0;
          
      //     if(nsum>0 && nsum%NWFcumulative==0){
      //       Event.NTQChCumSlidingCNCorr[idx][chan] = 0;
      //       Event.NTQChCumFixedCNCorr[idx][chan] = 0;
      //     }
      //     else {
      //       Event.NTQChCumSlidingCNCorr[idx][chan] = -999.;
      //       Event.NTQChCumFixedCNCorr[idx][chan] = -999.;
      //     }
      //     for(UShort_t s=0;s<NSAMPLETIME;s++){ 
      //     // waveforms storage in ntuple
      //       if(s<NAVG){
      //         // Event.Waves1WfCNCorr[idx][chan][s] = 0; 
      //         if(nsum>0 && nsum%NWFcumulative==0){
      //           // Event.WavesCumCNCorr[idx][chan][s] = 0;
      //         } else {
      //           // Event.WavesCumCNCorr[idx][chan][s] = 0.;
      //         }
      //       }else{
      //         // Event.Waves1WfCNCorr[idx][chan][s] = 0.;
      //         // Event.WavesCumCNCorr[idx][chan][s] = 0.;
      //       }
      //     }
      //   }
      // }

      if(brdID==28){
        for(UChar_t chan=0; chan<nChn; chan++){
          if (chan < 16) { // X group
            if (VMax1Wf_chan[chan] > Xmax) {
              // cout << "VMax1Wf_chan[chan] = " << VMax1Wf_chan[chan] << " - Xmax = " << Xmax << endl;
              Xmax = VMax1Wf_chan[chan];
              Xchan_MAX = chan;
            }
          } if (chan > 15) { // Y group          
            if (VMax1Wf_chan[chan] > Ymax) {
              // cout << "VMax1Wf_chan[chan] = " << VMax1Wf_chan[chan] << " - Ymax = " << Ymax << endl;
              Ymax = VMax1Wf_chan[chan];
              Ychan_MAX = chan;
            }
          }
        }
        // cout << "Xmax = " << Xmax << " - Ymax = " << Ymax << endl;
        // cout << "Xchan_MAX = " << Xchan_MAX << " - Ychan_MAX = " << Ychan_MAX << endl;

        int Xphys_MAX = PhysicsChannels[Xchan_MAX];
        int Yphys_MAX = PhysicsChannels[Ychan_MAX];

        BeamXStrips.clear();
        BeamYStrips.clear();
        // BeamXAdc.clear();
        // BeamYAdc.clear();

        // ----- X effective region (physics based) -----
        for(int dp = -3; dp <= 3; dp++){
          int phys = Xphys_MAX + dp;
          if(phys < 0 || phys >= NTUPLE_N_CHANNELS) continue;
          int adc = PhysicstoADC[phys];   // convert physics -> ADC
          if(adc >= 0 && adc < 16){       // ensure X group ADC
            BeamXStrips.push_back(phys);
            // BeamXAdc.push_back(PhysicstoADC[phys]);
            // cout << "effective X-physics channel: " << phys << " corresponding to ADC: " << adc << endl;
          }
        }
        // ----- Y effective region (physics based) -----
        for(int dp = -3; dp <= 3; dp++){
          int phys = Yphys_MAX + dp;
          if(phys < 0 || phys >= NTUPLE_N_CHANNELS) continue;
          int adc = PhysicstoADC[phys];   // convert physics -> ADC
          if(adc >= 16 && adc < 32){      // ensure Y group ADC
            BeamYStrips.push_back(phys);
            // BeamYAdc.push_back(PhysicstoADC[phys]);
            // cout << "effective Y-physics channel: " << phys << " corresponding to ADC: " << adc << endl;
          }
        }
        // find the noisiest NO-BEAM channels
        Double_t RMSNoiseXStrip = -1.;
        Double_t RMSNoiseYStrip = -1.;
        Int_t noisyStripX = -1;
        Int_t noisyStripY = -1;

        for(UChar_t chan=0; chan<nChn; chan++){
          bool isBeamX = (find(BeamXStrips.begin(), BeamXStrips.end(), PhysicsChannels[chan]) != BeamXStrips.end());
          bool isBeamY = (find(BeamYStrips.begin(), BeamYStrips.end(), PhysicsChannels[chan]) != BeamYStrips.end());
          bool isBeam = isBeamX || isBeamY;

          if(isBeam) continue; // only NO-BEAM channels

          Double_t rms = TMath::RMS(NAVG, &AbsRecoSample1Wf[chan][0]); // compute RMS of the waveform

          if(chan < 16){
            if(rms > RMSNoiseXStrip){
              RMSNoiseXStrip = rms;
              noisyStripX = chan;
            }
          } else {
            if(rms > RMSNoiseYStrip){
              RMSNoiseYStrip = rms;
              noisyStripY = chan;
            }
          }
        }

        // cout << "Noisiest NO-BEAM X channel: " << noisyStripX << " RMS = " << RMSNoiseXStrip << endl;
        // cout << "Noisiest NO-BEAM Y channel: " << noisyStripY << " RMS = " << RMSNoiseYStrip << endl;


        for(UChar_t chan=0; chan<nChn; chan++){
          // cout << "entering in channel: " << (int)chan << " before skipping channels" << endl;
          bool isBeamX = (find(BeamXStrips.begin(), BeamXStrips.end(),PhysicsChannels[chan]) != BeamXStrips.end());
          bool isBeamY = (find(BeamYStrips.begin(), BeamYStrips.end(),PhysicsChannels[chan]) != BeamYStrips.end());
          bool isBeam = isBeamX || isBeamY;   // beam flag

          // Skip BEAM channels → use only NO-BEAM for CMN
          if(isBeam) continue;
          // cout << "channel: " << (int)chan << " used for CMN wf determination" << endl;
          // cout << "N BeamXStrips: " << BeamXStrips.size() << endl;
          // cout << "N BeamYStrips: " << BeamYStrips.size() << endl;
          
          // Skip the noisiest NO-BEAM channels
          if(chan == noisyStripX) continue;
          if(chan == noisyStripY) continue;
          // loop over samples for common noise waveform reconstruction

          for(int s = 0; s<NSAMPLETIME; s++){
            if(chan<16){
              AbsRecoSampleCNXStrips[s] += AbsRecoSample1Wf[chan][s]; //Signal in mV
              // cout << "chan= " << (int)chan << " - AbsRecoSampleCNXStrips: " << AbsRecoSampleCNXStrips[s] << " - AbsRecoSample1Wf[chan][s]" << AbsRecoSample1Wf[chan][s] << endl;
            }
            if(chan>15){
              AbsRecoSampleCNYStrips[s] += AbsRecoSample1Wf[chan][s]; //Signal in mV
              // cout << "chan= " << (int)chan << " - AbsRecoSampleCNYStrips: " << AbsRecoSampleCNYStrips[s] << " - AbsRecoSample1Wf[chan][s]" << AbsRecoSample1Wf[chan][s] << endl;
            }
          }

        } // end loop on channels      
        // cout << "end of first channel in loop for common noise determination" << endl;
        // cout << "channels nChn = " << (int)nChn << " BeamXStrips.size() = " << BeamXStrips.size() << " BeamYStrips.size() = " << BeamYStrips.size() << endl;
        
        int nValidX = (nChn/2) - BeamXStrips.size() - (noisyStripX != -1 ? 1 : 0);
        int nValidY = (nChn/2) - BeamYStrips.size() - (noisyStripY != -1 ? 1 : 0);

        // cout << "nValidX = " << nValidX << " - nValidY = " << nValidY << endl;

        for(UShort_t s = 0; s < NAVG; s++){
          // cout << "AbsRecoSampleCNXStrips[s] = " << AbsRecoSampleCNXStrips[s] << " - AbsRecoSampleCNYStrips[s] = " << AbsRecoSampleCNYStrips[s] << endl;  
          AbsRecoSampleCNXStrips[s] = AbsRecoSampleCNXStrips[s]/nValidX;
          AbsRecoSampleCNYStrips[s] = AbsRecoSampleCNYStrips[s]/nValidY;
          // cout << "AbsRecoSampleCNXStrips[s] = " << AbsRecoSampleCNXStrips[s] << " - AbsRecoSampleCNYStrips[s] = " << AbsRecoSampleCNYStrips[s] << endl;  

        }

        // loop over channels for common noise subtraction
        Double_t ChargeCumSlidingCNCorr[NTUPLE_N_CHANNELS]= {0.};
        Double_t ChargeCumFixedCNCorr[NTUPLE_N_CHANNELS]= {0.};

        for(UChar_t chan = 0; chan < nChn; chan++){

          bool isBeamX = (find(BeamXStrips.begin(), BeamXStrips.end(),PhysicsChannels[chan]) != BeamXStrips.end());
          bool isBeamY = (find(BeamYStrips.begin(), BeamYStrips.end(),PhysicsChannels[chan]) != BeamYStrips.end());
          bool isBeam = isBeamX || isBeamY;   // beam flag
          
          Double_t Charge1WfSlidingCNCorr = 0.;    
          Double_t Charge1WfFixedCNCorr = 0.;   
          Double_t VMax1WfCNCorr = -999.;
          Double_t TMax1WfCNCorr = -999.;
          Double_t VMaxCumCNCorr = -999.;
          Double_t TMaxCumCNCorr = -999.;

          for(UShort_t s = 0; s < NAVG; s++){
            
            // NO-BEAM CHANNELS 
            if(!isBeam){
              if(chan < 16){
                AbsRecoSample1WfCNXStripsCorr[chan][s] = AbsRecoSampleCNXStrips[s];
                AbsRecoSampleCumCNXStripsCorr[chan][s] = AbsRecoSampleCNXStrips[s];
              } 
              else{
                AbsRecoSample1WfCNYStripsCorr[chan][s] = AbsRecoSampleCNYStrips[s];
                AbsRecoSampleCumCNYStripsCorr[chan][s] = AbsRecoSampleCNYStrips[s];
              }
              // VMax1WfCNCorr = -999.;
              // TMax1WfCNCorr = -999.;
              // VMaxCumCNCorr = -999.;
              // TMaxCumCNCorr = -999.;
              continue;
            }
            // ON-BEAM CHANNELS 
            if(chan < 16){
              Double_t val = AbsRecoSample1Wf[chan][s] - AbsRecoSampleCNXStrips[s];
              AbsRecoSample1WfCNXStripsCorr[chan][s] = val;
              if(val > VMax1WfCNCorr){
                VMax1WfCNCorr = val;
                TMax1WfCNCorr = (Double_t)s;
              }
              AbsRecoSampleCumCNXStripsCorr[chan][s] += val;
              if(nsum > 0 && nsum % NWFcumulative == 0){
                // cout << "nsum = " << nsum << " - nsum= " << nsum << endl;
                AbsRecoSampleCumCNXStripsCorr[chan][s] /= nsum;
                if(AbsRecoSampleCumCNXStripsCorr[chan][s] > VMaxCumCNCorr){
                  VMaxCumCNCorr = AbsRecoSampleCumCNXStripsCorr[chan][s];
                  TMaxCumCNCorr = (Double_t)s;
                }
              }
            }
            else{ // chan > 15
              Double_t val = AbsRecoSample1Wf[chan][s] - AbsRecoSampleCNYStrips[s];
              AbsRecoSample1WfCNYStripsCorr[chan][s] = val;
              if(val > VMax1WfCNCorr){
                VMax1WfCNCorr = val;
                TMax1WfCNCorr = (Double_t)s;
              }
              AbsRecoSampleCumCNYStripsCorr[chan][s] += val;
              if(nsum > 0 && nsum % NWFcumulative == 0){
                AbsRecoSampleCumCNYStripsCorr[chan][s] /= nsum;
                if(AbsRecoSampleCumCNYStripsCorr[chan][s] > VMaxCumCNCorr){
                  VMaxCumCNCorr = AbsRecoSampleCumCNYStripsCorr[chan][s];
                  TMaxCumCNCorr = (Double_t)s;
                }
              }
            }
          }
        
          // preparing Charge integration after common noise subtraction
          Int_t qStart1WfCNCorr = TMax1WfCNCorr-150/1.;  
          Int_t qStop1WfCNCorr = TMax1WfCNCorr+250/1.;  

          Double_t qStartCumCNCorr = TMaxCumCNCorr - 150./1.;  
          Double_t qStopCumCNCorr = TMaxCumCNCorr + 250./1.;
          
          for(UShort_t t=0; t<NAVG; t++){ //riprendi da qui con differenza tra channel X and Y
            // Charge1WfCN --> charge computation with 1 waveform with common noise subtraction
            // ChargeCumCN --> cumulative charge computation with NWFcumulative waveform with common noise subtraction
            if((int)chan<16){
              if(t>200 && t<700){
                if(AbsRecoSample1WfCNXStripsCorr[chan][t]<0) Charge1WfFixedCNCorr += 0; // to avoid negative contributions due to noise
                Charge1WfFixedCNCorr += (AbsRecoSample1WfCNXStripsCorr[chan][t]*1e-3)/fImpedance*fTimeBin/1E-12; //charge in pC - renorm to the number of channel summed
                if(nsum>0 && nsum%NWFcumulative==0){
                  if(AbsRecoSampleCumCNXStripsCorr[chan][t]<0) ChargeCumFixedCNCorr[chan] += 0; // to avoid negative contributions due to noise
                  ChargeCumFixedCNCorr[chan] += (AbsRecoSampleCumCNXStripsCorr[chan][t]*1e-3)/fImpedance*fTimeBin/1E-12; //charge in pC
                  // cout << "AbsRecoSampleCumCNXStripsCorr: " << AbsRecoSampleCumCNXStripsCorr[chan][t] << endl;
                }
              }
              if(t>qStart1WfCNCorr && t<qStop1WfCNCorr){
                if(AbsRecoSample1WfCNXStripsCorr[chan][t]<0) Charge1WfSlidingCNCorr += 0; // to avoid negative contributions due to noise
                Charge1WfSlidingCNCorr += (AbsRecoSample1WfCNXStripsCorr[chan][t]*1e-3)/fImpedance*fTimeBin/1E-12; //charge in pC - renorm to the number of channel summed
              }
              if(nsum>0 && nsum%NWFcumulative==0){
                if(t>qStartCumCNCorr && t<qStopCumCNCorr){
                  if(AbsRecoSampleCumCNXStripsCorr[chan][t]<0) ChargeCumSlidingCNCorr[chan] += 0; // to avoid negative contributions due to noise
                  ChargeCumSlidingCNCorr[chan] += (AbsRecoSampleCumCNXStripsCorr[chan][t]*1e-3)/fImpedance*fTimeBin/1E-12; //charge in pC
                  // cout << "AbsRecoSampleCumCNXStripsCorr: " << AbsRecoSampleCumCNXStripsCorr[chan][t] << endl;
                }
              }
            }
            if((int)chan>15){
              if(t>200 && t<700){
                if(AbsRecoSample1WfCNYStripsCorr[chan][t]<0) Charge1WfFixedCNCorr += 0; // to avoid negative contributions due to noise
                Charge1WfFixedCNCorr += (AbsRecoSample1WfCNYStripsCorr[chan][t]*1e-3)/fImpedance*fTimeBin/1E-12; //charge in pC - renorm to the number of channel summed
              if(nsum>0 && nsum%NWFcumulative==0){
                if(AbsRecoSampleCumCNYStripsCorr[chan][t]<0) ChargeCumFixedCNCorr[chan] += 0; // to avoid negative contributions due to noise
                ChargeCumFixedCNCorr[chan] += (AbsRecoSampleCumCNYStripsCorr[chan][t]*1e-3)/fImpedance*fTimeBin/1E-12; //charge in pC
                // cout << "AbsRecoSampleCumCNYStripsCorr: " << AbsRecoSampleCumCNYStripsCorr[chan][t] << endl;
              }
              }
              if(t>qStart1WfCNCorr && t<qStop1WfCNCorr){
                if(AbsRecoSample1WfCNYStripsCorr[chan][t]<0) Charge1WfSlidingCNCorr += 0; // to avoid negative contributions due to noise
                Charge1WfSlidingCNCorr += (AbsRecoSample1WfCNYStripsCorr[chan][t]*1e-3)/fImpedance*fTimeBin/1E-12; //charge in pC - renorm to the number of channel summed
              }
              if(nsum>0 && nsum%NWFcumulative==0){
                if(t>qStartCumCNCorr && t<qStopCumCNCorr){
                  if(AbsRecoSampleCumCNYStripsCorr[chan][t]<0) ChargeCumSlidingCNCorr[chan] += 0; // to avoid negative contributions due to noise
                  ChargeCumSlidingCNCorr[chan] += (AbsRecoSampleCumCNYStripsCorr[chan][t]*1e-3)/fImpedance*fTimeBin/1E-12; //charge in pC
                  // cout << "AbsRecoSampleCumCNYStripsCorr: " << AbsRecoSampleCumCNYStripsCorr[chan][t] << endl;
                }
              }
            }
          }

          idx = 1; //hardcoded for Target board

          Event.NTQCh1WfSlidingCNCorr[idx][chan]  = Charge1WfSlidingCNCorr;
          Event.NTQCh1WfFixedCNCorr[idx][chan]  = Charge1WfFixedCNCorr;
          Event.NTVMax1WfCNCorr[idx][chan] = VMax1WfCNCorr;
          Event.NTTMax1WfCNCorr[idx][chan] = TMax1WfCNCorr * digiTime;
          Event.NTVMaxCumCNCorr[idx][chan] = VMaxCumCNCorr;
          Event.NTTMaxCumCNCorr[idx][chan] = TMaxCumCNCorr * digiTime;
          
          if(nsum>0 && nsum%NWFcumulative==0){
            Event.NTQChCumSlidingCNCorr[idx][chan] = ChargeCumSlidingCNCorr[chan];
            Event.NTQChCumFixedCNCorr[idx][chan] = ChargeCumFixedCNCorr[chan];
            ChargeCumSlidingCNCorr[chan]=0;
            ChargeCumFixedCNCorr[chan]=0;
          }
          else {
            Event.NTQChCumSlidingCNCorr[idx][chan] = -999.;
            Event.NTQChCumFixedCNCorr[idx][chan] = -999.;
          }

          for(UShort_t s=0;s<NSAMPLETIME;s++){ 
          // waveforms storage in ntuple
            if(chan<16){
              if(s<NAVG){
                // cout << "chan= " << int(chan) << " - AbsRecoSample1Wf[chan][s]= " << AbsRecoSample1Wf[chan][s] << endl;
                // cout << "chan= " << int(chan) << " - AbsRecoSample1WfCNXStripsCorr[chan][s]= " << AbsRecoSample1WfCNXStripsCorr[chan][s] << endl;
                // Event.Waves1WfCNCorr[idx][chan][s] = AbsRecoSample1WfCNXStripsCorr[chan][s]; 
                // Event.Waves1WfCN[idx][s] = AbsRecoSampleCNXStrips[s];
                AbsRecoSample1Wf[chan][s]=0;
                AbsRecoSample1WfCNXStripsCorr[chan][s] = 0;
                if(nsum>0 && nsum%NWFcumulative==0){
                  // Event.WavesCumCNCorr[idx][chan][s] = AbsRecoSampleCumCNXStripsCorr[chan][s];
                  AbsRecoSampleCum[chan][s] = 0.;
                  AbsRecoSampleCumCNXStripsCorr[chan][s] = 0;
                } else {
                  // Event.WavesCumCNCorr[idx][chan][s] = 0.;
                }
              }else{
                // Event.Waves1WfCN[idx][s] = 0.;
                // Event.Waves1WfCNCorr[idx][chan][s] = 0.;
                // Event.WavesCumCNCorr[idx][chan][s] = 0.;
              }
            } else {
              if(s<NAVG){
                // cout << "chan= " << int(chan) << " - AbsRecoSample1Wf[chan][s]= " << AbsRecoSample1Wf[chan][s] << endl;
                // cout << "chan= " << int(chan) << " - AbsRecoSample1WfCNYStripsCorr[chan][s]= " << AbsRecoSample1WfCNYStripsCorr[chan][s] << endl;
                // Event.Waves1WfCNCorr[idx][chan][s] = AbsRecoSample1WfCNYStripsCorr[chan][s]; 
                // Event.Waves1WfCN[idx][s] = AbsRecoSampleCNXStrips[s];
                AbsRecoSample1Wf[chan][s] = 0;  
                AbsRecoSample1WfCNYStripsCorr[chan][s] = 0;
                if(nsum>0 && nsum%NWFcumulative==0){
                  // Event.WavesCumCNCorr[idx][chan][s] = AbsRecoSampleCumCNYStripsCorr[chan][s]; 
                  AbsRecoSampleCum[chan][s] = 0.;
                  AbsRecoSampleCumCNYStripsCorr[chan][s] = 0;
                } else {
                  // Event.WavesCumCNCorr[idx][chan][s] = 0.;
                }
              }else{
                // Event.Waves1WfCNCorr[idx][chan][s] = 0.;
                // Event.WavesCumCNCorr[idx][chan][s] = 0.;
              }
            }
          }
        }
      }
    } // end loop on boards
    
    if(nsum>0 && nsum%NWFcumulative==0){
      // cout << "Resetting nsum counter. Event number: " << iev << " nsum before reset: " << nsum << " nRND before reset: " << nRND << endl;
      nsum=0;
      nRND=0;
    }

    Event.NTNevent = evtNumber; // GetEventNumber from TChain of the RawEvent
    Event.NTEventTime = EvT;
    // Event.NTAbsDateEvent = AbsDateEv;
    // Event.NTAbsTimeEvent = AbsTimeEv;
    Event.NTAbsTimeEventUNIX = AbsTimeEvUNIX;
    tree->Fill();

    // Clear event
    rawEv->Clear("C");
    
  } // end loop on events

  histoFile->cd();
  tree->Write();
  // hTdiff->Write();

  
  TString baseName = outputFileName;
  Ssiz_t us = baseName.Last('_');
  if (us >= 0) baseName = baseName(us + 1, baseName.Length() - us - 1);
  Ssiz_t dot = baseName.Last('.');
  if (dot >= 0) baseName = baseName(0, dot);
  printf("%s\n",baseName.Data());

  histoFile->Write();

  // Save and close output file
  printf("Closing output file\n");
  printf("\t\t\t----------> root -l %s \n",outputFileName.Data());
  histoFile->Close();

  delete inputChain;
  delete rawEv;
  histoFile->Close();
  delete histoFile;
  return 0;
}