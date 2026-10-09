#ifndef RecoMM_h
#define RecoMM_h

#include "TBranch.h"
#include "TChain.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TString.h"
#include "TTree.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define MM_N_Layers 8

struct BlockTimeInfo {
  Long64_t nEvents = 0;

  ULong64_t firstEvt = 0;
  ULong64_t lastEvt  = 0;

  double firstSrs = 0.;
  double lastSrs  = 0.;
  double meanSrs  = 0.;

  double firstDaqSec = 0.;
  double lastDaqSec  = 0.;
  double meanDaqSec  = 0.;

  double firstDaq = 0.;
  double lastDaq  = 0.;
  double meanDaq  = 0.;

  long double sumSrs    = 0.;
  long double sumDaq    = 0.;
  long double sumDaqSec = 0.;
};

class RecoMM {
public:
  TChain  *fTree = nullptr;
  Bool_t   fOwnChain = kFALSE;
  int      RunID = 0;
  int      DetRunID = 0;
  int      maxEvents = 0;
  TString  outputFileName = "RecoMM.root";

  // ---------------------------------------------------------------------------
  // Input apv_raw branches actually used by RecoMM
  // ---------------------------------------------------------------------------
  ULong64_t evt = 0;
  Int_t daqTimeSec = 0;
  Int_t daqTimeMicroSec = 0;
  Int_t srsTimeStamp = 0;

  vector<int> *mmLayer = nullptr;
  vector<int> *mmStrip = nullptr;
  vector<vector<short>> *raw_q = nullptr;

  TBranch *b_evt = nullptr;
  TBranch *b_daqTimeSec = nullptr;
  TBranch *b_daqTimeMicroSec = nullptr;
  TBranch *b_srsTimeStamp = nullptr;
  TBranch *b_mmLayer = nullptr;
  TBranch *b_mmStrip = nullptr;
  TBranch *b_raw_q = nullptr;

  RecoMM(TObjArray *inputFileNameList, int RunID = 0, int DetRunID = 0, int maxEvents = 0, TString outputFileName = "RecoMM.root");
  virtual ~RecoMM();

  virtual Int_t GetEntry(Long64_t entry);
  virtual Long64_t LoadTree(Long64_t entry);
  virtual void Init(TChain *tree);
  virtual bool Notify();

  // ---------------------------------------------------------------------------
  // MM detector reconstruction
  // ---------------------------------------------------------------------------
  virtual double StripToX(int iStrip, int iLayer) const;
  virtual double LayerXMin(int iLayer) const;
  virtual double LayerXMax(int iLayer) const;
  virtual int LayerNBinsX(int iLayer) const;

  virtual void CoordinateFinder(int iStrip, int iLayer, const vector<short> &camp, double &t_strip, double &x_strip, double &z_strip, double &q_strip);

  virtual void LoopFileList(TObjArray &inputFileNameList, int NevtBlock = 10000);

  // ---------------------------------------------------------------------------
  // MM geometry / working point
  // ---------------------------------------------------------------------------
  TString mm_tag[MM_N_Layers] = {"L0", "L1", "L2", "L3", "L4", "L5", "L6", "L7"};

  static const int MAXSTRIP = 512;

  double pitch = 1.2; // mm

  // Central inactive gap used by the existing MM reconstruction.
  double geo_hole[MM_N_Layers] = {8.4, 8.4, 2.4, 2.4, 2.4, 2.4, 8.4, 8.4};

  double shift_coord[MM_N_Layers] = { 18.54, 18.54, 12.1, 12.1, 18.54, 18.54, 12.1, 12.1};

  double zm = 50.;
  double vd = 0.1002; // mm/ns
  double clock = 25.; // ns

  // Saturated hits remain in the RAW histograms, but are excluded from the
  // dedicated NoSat products and from the block maximum-charge-strip monitor.
  double SaturationThreshold = 1600.;

  vector<BlockTimeInfo> blockTimeInfo;

  // ---------------------------------------------------------------------------
  // Overall RAW products: no calibration is applied anywhere in RecoMM.
  // ---------------------------------------------------------------------------
  TH2F *hqmaxstripFull[MM_N_Layers] = {nullptr};
  TH2F *hqmaxstripFull_NoSat[MM_N_Layers] = {nullptr};

  TH1D *hQmaxSumNoSatStrip[MM_N_Layers] = {nullptr};

  TH2F *htimePositionFull[MM_N_Layers] = {nullptr};
  TH2F *hzPositionFull[MM_N_Layers] = {nullptr};

  // ---------------------------------------------------------------------------
  // Block-level seed monitors for the later analysis.
  // Definition of "maximum-charge strip": strip with the largest sum of q_max
  // over NON-SATURATED hits in that event-ID block.
  // ---------------------------------------------------------------------------
  TGraphErrors *g_BlockMaxChargeStrip[MM_N_Layers] = {nullptr};
  TGraphErrors *g_BlockMaxChargePosition[MM_N_Layers] = {nullptr};
  TGraphErrors *g_BlockMaxChargeValue[MM_N_Layers] = {nullptr};

  TGraphErrors *g_DaqTimeMaxChargeStrip[MM_N_Layers] = {nullptr};
  TGraphErrors *g_DaqTimeMaxChargePosition[MM_N_Layers] = {nullptr};
  TGraphErrors *g_DaqTimeMaxChargeValue[MM_N_Layers] = {nullptr};

  // ---------------------------------------------------------------------------
  // Event/time and block/time association.
  // ---------------------------------------------------------------------------
  TGraphErrors *g_DaqTime_iev = nullptr;
  TGraphErrors *g_SrsTimeStamp_evt = nullptr;
  TGraphErrors *g_DaqTimeSec_evt = nullptr;
  TGraphErrors *g_DaqTimeMicroSec_evt = nullptr;
  TGraphErrors *g_DaqTime_evt = nullptr;
  TGraphErrors *g_evt_vs_iev = nullptr;

  TGraphErrors *g_BlockMeanDaqSec = nullptr;
  TGraphErrors *g_BlockMeanDaqTime = nullptr;
  TGraphErrors *g_BlockMeanSrsTime = nullptr;
};

#endif

#ifdef RecoMM_cxx

RecoMM::RecoMM(TObjArray *inputFileNameList,
               int RunID,
               int DetRunID,
               int maxEvents,
               TString outputFileName)
    : fTree(nullptr),
      fOwnChain(kFALSE),
      RunID(RunID),
      DetRunID(DetRunID),
      maxEvents(maxEvents),
      outputFileName(outputFileName)
{
  if (!inputFileNameList || inputFileNameList->GetEntries() == 0) {
    cerr << "ERROR: empty input file list in RecoMM constructor" << endl;
    return;
  }

  fprintf(stdout, "=== === === Chain of input files === === ===\n");

  TChain *chain = new TChain("apv_raw");
  Long64_t totalEntries = 0;

  for (Int_t iFile = 0; iFile < inputFileNameList->GetEntries(); ++iFile) {
    TString fileName = ((TObjString *)inputFileNameList->At(iFile))->GetString();
    fprintf(stdout, "%4d %s\n", iFile, fileName.Data());

    TFile *file = TFile::Open(fileName.Data(), "READ");
    if (!file || file->IsZombie()) {
      cerr << "WARNING: cannot open input file: " << fileName << endl;
      if (file) {
        file->Close();
        delete file;
      }
      continue;
    }

    TTree *tree = dynamic_cast<TTree *>(file->Get("apv_raw"));
    if (!tree) {
      cerr << "WARNING: cannot find tree apv_raw in file: " << fileName << endl;
      file->Close();
      delete file;
      continue;
    }

    const Long64_t nEntries = tree->GetEntries();
    file->Close();
    delete file;

    if (nEntries <= 0) {
      cerr << "WARNING: tree apv_raw has zero entries in file: " << fileName << endl;
      continue;
    }

    const Int_t added = chain->Add(fileName.Data());
    if (added == 0) {
      cerr << "WARNING: could not add file to chain: " << fileName << endl;
      continue;
    }

    totalEntries += nEntries;
    cout << "Added " << fileName << " entries = " << nEntries << " total = " << totalEntries << endl;
  }

  if (totalEntries <= 0) {
    cerr << "ERROR: no valid entries found while building input chain" << endl;
    delete chain;
    return;
  }

  cout << "Total validated entries in chain = " << totalEntries << endl;

  fOwnChain = kTRUE;
  Init(chain);
}

RecoMM::~RecoMM()
{
  if (fOwnChain && fTree) {
    delete fTree;
    fTree = nullptr;
  }
}

Int_t RecoMM::GetEntry(Long64_t entry)
{
  if (!fTree) return 0;
  return fTree->GetEntry(entry);
}

Long64_t RecoMM::LoadTree(Long64_t entry)
{
  if (!fTree) return -5;
  return fTree->LoadTree(entry);
}

void RecoMM::Init(TChain *tree)
{
  if (!tree) return;

  fTree = tree;

  mmLayer = nullptr;
  mmStrip = nullptr;
  raw_q = nullptr;

  fTree->SetBranchStatus("*", 0);
  fTree->SetBranchStatus("evt", 1);
  fTree->SetBranchStatus("daqTimeSec", 1);
  fTree->SetBranchStatus("daqTimeMicroSec", 1);
  fTree->SetBranchStatus("srsTimeStamp", 1);
  fTree->SetBranchStatus("mmLayer", 1);
  fTree->SetBranchStatus("mmStrip", 1);
  fTree->SetBranchStatus("raw_q", 1);

  fTree->SetBranchAddress("evt", &evt, &b_evt);
  fTree->SetBranchAddress("daqTimeSec", &daqTimeSec, &b_daqTimeSec);
  fTree->SetBranchAddress("daqTimeMicroSec", &daqTimeMicroSec, &b_daqTimeMicroSec);
  fTree->SetBranchAddress("srsTimeStamp", &srsTimeStamp, &b_srsTimeStamp);
  fTree->SetBranchAddress("mmLayer", &mmLayer, &b_mmLayer);
  fTree->SetBranchAddress("mmStrip", &mmStrip, &b_mmStrip);
  fTree->SetBranchAddress("raw_q", &raw_q, &b_raw_q);

  Notify();
}

bool RecoMM::Notify()
{
  return true;
}

#endif // RecoMM_cxx
