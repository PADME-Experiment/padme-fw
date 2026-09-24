
#ifndef CalibMM_h
#define CalibMM_h

#include "TRandom.h"
#include "TRandom3.h"
#include "TMarker.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TTreeIndex.h"
#include "TBranch.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH2.h"
#include "TProfile.h"
#include "TMath.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TSpline.h"
#include "TString.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TPaveText.h"
#include "TObject.h"
#include "TF1.h"
#include <TMatrixDSym.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include "TButton.h"
#include "TTimeStamp.h"
#include "Riostream.h"
#include "TMinuit.h"

#include "TInterpreter.h"
#include "TROOT.h"

#include <algorithm>
// #include <chrono>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <cmath>
#include <sys/stat.h>
// #include <thread>
#include <vector>

using namespace std;

#define MM_N_Layers 8

struct SliceFitResult {
  TH1D *amp = nullptr;
  TH1D *mean = nullptr;
  TH1D *sigma = nullptr;
  TH1D *chi2 = nullptr;  
};

// struct BeamFitSummary {
//   bool   valid       = false;
//   double charge      = 0.;
//   double err_charge  = 0.;
//   double position    = 0.;
//   double err_position= 0.;
//   double spread      = 0.;
//   double err_spread  = 0.;
// };

class CalibMM {
public :
   TChain         *fTree;      //! pointer to the analyzed TTree
   // Int_t          fCurrent;    //! current Tree number in a TTree
   Bool_t         fOwnChain;   //! true if this class created the TTree
   int            RunID = 0;
   int            DetRunID = 0;
   int            maxEvents=0;
   TString        outputFileName = "CalibrationMM.root";

   // Branches declaration 
   ULong64_t       evt;
   UInt_t          error;
   Int_t           daqTimeSec;
   Int_t           daqTimeMicroSec;
   Int_t           srsTimeStamp;
   UInt_t          srsTrigger;
   vector<unsigned int> *srsFec;
   vector<unsigned int> *srsChip;
   vector<unsigned int> *srsChan;
   vector<string>  *mmChamber;
   vector<int>     *mmLayer;
   vector<char>    *mmReadout;
   vector<int>     *mmStrip;
   vector<vector<short> > *raw_q;
   vector<short>   *max_q;
   vector<int>     *t_max_q;

   // List of branches
   TBranch         *b_evt;   //!
   TBranch         *b_error;   //!
   TBranch         *b_daqTimeSec;   //!
   TBranch         *b_daqTimeMicroSec;   //!
   TBranch         *b_srsTimeStamp;   //!
   TBranch         *b_srsTrigger;   //!
   TBranch         *b_srsFec;   //!
   TBranch         *b_srsChip;   //!
   TBranch         *b_srsChan;   //!
   TBranch         *b_mmChamber;   //!
   TBranch         *b_mmLayer;   //!
   TBranch         *b_mmReadout;   //!
   TBranch         *b_mmStrip;   //!
   TBranch         *b_raw_q;   //!
   TBranch         *b_max_q;   //!
   TBranch         *b_t_max_q;   //!

   CalibMM(TObjArray *inputFileNameList, int RunID=0, int DetRunID=0, int maxEvents=0, TString outputFileName="CalibrationMM.root");   
   virtual ~CalibMM();
   // virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TChain *tree);
   virtual bool     Notify();

   virtual void     CoordinateFinder(int iStrip, int iLayer, const vector<short> &camp, double &x_strip, double &q_strip, double &t_strip);
   virtual void     LoopFileList(TObjArray &inputFileNameList, int NevtBlock=10000);
   virtual void     BuildFitRatio(TH1D *hMeanFull, TH1D *hSigmaFull, TF1 *fit, TGraphErrors *gRatio, TGraphErrors *gDiff);
   virtual void     FillBlockGraphsFromSlices(int iLayer);
   virtual void     WriteCalibrationGraphTxt(ofstream &out, TGraphErrors *g, int runID, TString view);
   virtual void     WriteCalibrationConstants(TString filename, int runID);
   virtual bool     IsFitAccepted(TFitResultPtr fitResult, int maxFitStatusAccepted = 1,int minCovMatrixStatusAccepted = 2);

   virtual SliceFitResult RunFitSlicesY(TH2F *h2, TString tag);
   virtual TF1*     FitDoubleGaussian(TH1D *h, TString name, double xmin, double xmax, TFitResultPtr &fitResult);

   virtual double   VoigtIntegralPDF(double *x, double *par);
   virtual TF1*     FitVoigt(TH1D *h, TString name, double xmin, double xmax, TFitResultPtr &fitResult);


   ///////ENVIRONMENT VARIABLES//////
   // MM: 8 independent strip layers, 512 strips per layer.
   // Calibration is performed versus strip ID; physical x/z reconstruction
   // (central holes, shifts, drift velocity) remains a Reco-level operation.
   TString mm_tag[MM_N_Layers] = {"L0", "L1", "L2", "L3", "L4", "L5", "L6", "L7"};

   float pitch = 1.2; // mm
   int maxStrip = 512, maxStripApv = 128;
   float xmax = maxStrip * pitch;

   float clock = 25., ncamp = 27; // 675 ns time window (450 ns -> ncamp = 18)

   // Fit/calibration boundaries in strip ID.
   // Keep the complete MM strip range; empty FitSlicesY bins do not contribute to the fit.
   double StripMin = 0.;
   double StripMax = 511.;

   // strip calibration vectors
   vector<double> FitFullDiff[MM_N_Layers];
   vector<double> FitFullRatio[MM_N_Layers];

   // tf1 di fit
   vector<TF1*> f;

   /////// HISTOGRAMS //////
   // event based histograms
   TH2F *hqmaxstrip[MM_N_Layers] = {0}; //qmax vs xstrip distribution
   TH2F *hqmaxstripFull[MM_N_Layers] = {0}; //qmax vs xstrip distribution

   TH2F *htmaxstrip[MM_N_Layers] = {0}; //tmax vs xstrip distribution
   TH2F *htmaxstripFull[MM_N_Layers] = {0}; //tmax vs xstrip distribution

   // event based FitSlicesY() histograms
   TH1D *hAmpslice[MM_N_Layers] = {0};
   TH1D *hMeanslice[MM_N_Layers] = {0};
   TH1D *hSigmaslice[MM_N_Layers] = {0};
   TH1D *hChi2slice[MM_N_Layers] = {0};
   TH1D *hAmpsliceFull[MM_N_Layers] = {0};
   TH1D *hMeansliceFull[MM_N_Layers] = {0};
   TH1D *hSigmasliceFull[MM_N_Layers] = {0};
   TH1D *hChi2sliceFull[MM_N_Layers] = {0};

   // block based histograms
   vector<TH2F*> hBlockqmaxstrip[MM_N_Layers]; //qmax vs xstrip distribution
   vector<TH2F*> hBlockqmaxstripFull[MM_N_Layers]; //qmax vs xstrip distribution

   // block based FitSlicesY() histograms
   vector<TH1D*> hBlockAmpslice[MM_N_Layers]; // amplitude slices
   vector<TH1D*> hBlockMeanslice[MM_N_Layers]; // mean slices
   vector<TH1D*> hBlockSigmaslice[MM_N_Layers]; // sigma slices
   vector<TH1D*> hBlockChi2slice[MM_N_Layers]; // sigma slices
   vector<TH1D*> hBlockAmpsliceFull[MM_N_Layers]; // amplitude slices (full)
   vector<TH1D*> hBlockMeansliceFull[MM_N_Layers]; // mean slices (full)
   vector<TH1D*> hBlockSigmasliceFull[MM_N_Layers]; // sigma slices (full)
   vector<TH1D*> hBlockChi2sliceFull[MM_N_Layers]; // sigma slices

   // event based calibrated histograms
   TH2F *hqmaxstrip_cal[MM_N_Layers] = {0}; //qmax vs xstrip distribution
   TH2F *hqmaxstripFull_cal[MM_N_Layers] = {0}; //qmax vs xstrip distribution

   // event based calibrated histograms selected in time
   TH2F *hqmaxstrip_sel[MM_N_Layers] = {0}; //qmax vs xstrip distribution
   TH2F *hqmaxstripFull_sel[MM_N_Layers] = {0}; //qmax vs xstrip distribution

   // block based calibrated histograms
   vector<TH2F*> hBlockqmaxstrip_cal[MM_N_Layers]; //qmax vs xstrip distribution
   vector<TH2F*> hBlockqmaxstripFull_cal[MM_N_Layers]; //qmax vs xstrip distribution

   // block based calibrated histograms - overlall calibration
   vector<TH2F*> hBlockqmaxstrip_Overallcal[MM_N_Layers]; //qmax vs xstrip distribution
   vector<TH2F*> hBlockqmaxstripFull_Overallcal[MM_N_Layers]; //qmax vs xstrip distribution

   ////// GRAPHS //////
   // block based graphs
   TGraphErrors *g_BlockBeamSpot[MM_N_Layers] = {0};  //Beam spot over blocks of events for stability monitoring
   TGraphErrors *g_BlockBeamSpread[MM_N_Layers] = {0}; //Beam spread over blocks of events for stability monitoring
   TGraphErrors *g_BlockBeamCharge[MM_N_Layers] = {0}; //Beam charge over blocks of events for stability monitoring

   //TGraphs for strip calibration
   TGraphErrors *g_FitFullDiff[MM_N_Layers] = {0}; // to identify possible dead strips
   TGraphErrors *g_FitFullRatio[MM_N_Layers] = {0}; // to define calibration constants

   //block based TGraphs for strip calibration
   vector<TGraphErrors*> g_BlockFitFullDiff[MM_N_Layers]; // to identify possible dead strips
   vector<TGraphErrors*> g_BlockFitFullRatio[MM_N_Layers]; // to define calibration constants

};

#endif

#ifdef CalibMM_cxx
CalibMM::CalibMM(TObjArray *inputFileNameList,
                 int RunID,
                 int DetRunID,
                 int maxEvents,
                 TString outputFileName) :
   fTree(0),
   fOwnChain(kFALSE),
   RunID(RunID),
   DetRunID(DetRunID),
   maxEvents(maxEvents),
   outputFileName(outputFileName)
{

   if (!inputFileNameList || inputFileNameList->GetEntries() == 0) {
      cerr << "ERROR: empty input file list in CalibMM constructor" << endl;
      return;
   }

   fprintf(stdout, "=== === === Chain of input files === === ===\n");
   TChain *chain = new TChain("apv_raw");
   Long64_t totalEntries = 0;
   for (Int_t iFile = 0; iFile < inputFileNameList->GetEntries(); iFile++) {
      TString fileName = ((TObjString*)inputFileNameList->At(iFile))->GetString();
      fprintf(stdout, "%4d %s\n", iFile, fileName.Data());
      TFile file(fileName.Data(), "READ");
      if (file.IsZombie()) {
         cerr << "WARNING: cannot open input file: " << fileName << endl;
         continue;
      }
      TTree *t = (TTree*)file.Get("apv_raw");
      if (!t) {
         cerr << "WARNING: cannot find tree apv_raw in file: " << fileName << endl;
         continue;
      }
      Long64_t n = t->GetEntries();
      if (n <= 0) {
         cerr << "WARNING: tree apv_raw has zero entries in file: " << fileName << endl;
         continue;
      }

      Int_t added = chain->Add(fileName.Data());
      if (added == 0) {
         cerr << "WARNING: could not add file to chain: "
              << fileName << endl;
         continue;
      }
      totalEntries += n;
      cout << "Added " << fileName << " entries = " << n << " total = " << totalEntries << endl;
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

CalibMM::~CalibMM()
{
   if (!fTree) return;
}

Int_t CalibMM::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fTree) return 0;
   return fTree->GetEntry(entry);
}

Long64_t CalibMM::LoadTree(Long64_t entry)
{
   if (!fTree) return -5;

   Long64_t centry = fTree->LoadTree(entry);
   if (centry < 0) return centry;

   return centry;
}

void CalibMM::Init(TChain *tree)
{
   //cout << "DEBUG Init: start" << endl;

   // Object pointers
   srsFec    = 0;
   srsChip   = 0;
   srsChan   = 0;
   mmChamber = 0;
   mmLayer   = 0;
   mmReadout = 0;
   mmStrip   = 0;
   raw_q     = 0;
   max_q     = 0;
   t_max_q   = 0;

   if (!tree) return;

   fTree = tree;

   // Keep all branches available
   fTree->SetBranchStatus("*", 1);

   fTree->SetBranchAddress("evt",             &evt,             &b_evt);
   fTree->SetBranchAddress("error",           &error,           &b_error);
   fTree->SetBranchAddress("daqTimeSec",      &daqTimeSec,      &b_daqTimeSec);
   fTree->SetBranchAddress("daqTimeMicroSec", &daqTimeMicroSec, &b_daqTimeMicroSec);
   fTree->SetBranchAddress("srsTimeStamp",    &srsTimeStamp,    &b_srsTimeStamp);
   fTree->SetBranchAddress("srsTrigger",      &srsTrigger,      &b_srsTrigger);

   fTree->SetBranchAddress("srsFec",          &srsFec,          &b_srsFec);
   fTree->SetBranchAddress("srsChip",         &srsChip,         &b_srsChip);
   fTree->SetBranchAddress("srsChan",         &srsChan,         &b_srsChan);
   fTree->SetBranchAddress("mmChamber",       &mmChamber,       &b_mmChamber);
   fTree->SetBranchAddress("mmLayer",         &mmLayer,         &b_mmLayer);
   fTree->SetBranchAddress("mmReadout",       &mmReadout,       &b_mmReadout);
   fTree->SetBranchAddress("mmStrip",         &mmStrip,         &b_mmStrip);
   fTree->SetBranchAddress("raw_q",           &raw_q,           &b_raw_q);
   fTree->SetBranchAddress("max_q",           &max_q,           &b_max_q);
   fTree->SetBranchAddress("t_max_q",         &t_max_q,         &b_t_max_q);

   Notify();

   // cout << "DEBUG Init: done" << endl;
}

bool CalibMM::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return true;
}

#endif // #ifdef CalibMM_cxx