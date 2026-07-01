#ifndef RecoTMM_h
#define RecoTMM_h

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
#include <vector>

using namespace std;

#define TMMCH_N_Readout 2

struct SliceFitResult {
  TH1D *amp = nullptr;
  TH1D *mean = nullptr;
  TH1D *sigma = nullptr;
  TH1D *chi2 = nullptr;  
};

class RecoTMM {
public :
   TChain         *fTree;      //! pointer to the analyzed TTree
   Bool_t         fOwnChain;   //! true if this class created the TTree
   int            RunID = 0;
   int            DetRunID = 0;
   int            maxEvents=0;
   TString        outputFileName = "RecoTMM.root";

   // Path to external calibration constants (mode 2 in overall, mode 2 in block view).
   // Settable from outside before LoopFileList; falls back to the historical default.
   TString        externalCalibFile = "/home/mancinima/BeamMonitorRun4/padme-fw/TMM_Calibration/outputTMM/TMMCalibration_run677_CalibrationConstant.txt";
   
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
  
   RecoTMM(TObjArray *inputFileNameList, int RunID=0, int DetRunID=0, int maxEvents=0, TString outputFileName="Monitor_TMM.root");   
   virtual ~RecoTMM();
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TChain *tree);
   virtual bool     Notify();
   
   virtual void     StripFinder(int &iReadout, int &iStrip);
   // CoordinateFinder also exposes the (remapped) local strip ID via iStripLocal,
   // so calibration arrays indexed by strip number can be used unambiguously.
   virtual void     CoordinateFinder(int iStrip, const vector<short> &camp, int &iReadout, double &x_strip, double &q_strip, int &iStripLocal);
   virtual void     GlobalCoordinate(int iReadout, double &x_strip);
   virtual void     LoopFileList(TObjArray &inputFileNameList, int NevtBlock=10000);
   virtual bool     LoadCalibrationConstants(const string &filename);
   virtual void     BuildFitRatio(TH1D *hMeanFull, TH1D *hSigmaFull, TF1 *fit, TGraphErrors *gRatio, TGraphErrors *gDiff);
   virtual void     FillBlockGraphsFromSlices(int iReadout);
   virtual bool     IsFitAccepted(TFitResultPtr fitResult, int maxFitStatusAccepted = 1,int minCovMatrixStatusAccepted = 2);

   virtual SliceFitResult RunFitSlicesY(TH2F *h2, TString tag);
   virtual TF1*     FitDoubleGaussian(TH1D *h, TString name, double xmin, double xmax, TFitResultPtr &fitResult);
   virtual double   VoigtIntegralPDF(double *x, double *par);
   virtual TF1*     FitVoigt(TH1D *h, TString name, double xmin, double xmax, TFitResultPtr &fitResult);

   ///////ENVIROMENT VARIABLES//////
   TString tmm_tag[TMMCH_N_Readout] = {"X", "Y"};
   float pitch = 0.250; //mm
   
   static const int MAXSTRIP = 358;
   const int maxStripApv = 128;                                                                                      
   // xmax = maxStripApv*pitch = 32 mm is wrong for the FULL detector axis
   float xFullmm = MAXSTRIP * pitch;   // 358 * 0.250 = 89.5 mm
   
   double d_firstcross_lastcross_strips = 89.262; //mm
   double X_metrology_corr = 2.770; //mm
   double Y_metrology_corr = -1.229; //mm
   double Theta_metrology_corr = 0.01658949417; //rad
   double GLOBAL_X_TRANSLATION = - (d_firstcross_lastcross_strips/2 + X_metrology_corr + cos(Theta_metrology_corr)); //mm
   double GLOBAL_Y_TRANSLATION = - (d_firstcross_lastcross_strips/2 - Y_metrology_corr + sin(Theta_metrology_corr)); //mm

   //POSITION REFERENCE POINT 1  
   double xP1_cad = 101.074, yP1_cad = 129.949;
   double xP1_met = 101.69 , yP1_met = 130.26;
   //POSITION REFERENCE POINT 2
   double xP2_cad = 127.174, yP2_cad = -98.791;
   double xP2_med = 131.48 , yP2_med = -97.78;
   
   float clock = 25., ncamp=27;

   // // first mean values between 677-534-415
   float APV1_fqx = 1*0.53563955; 
   float APV2_fqx = 1;                   
   float APV3_fqx = 1*0.38476545; 

   float APV1_fqy = 1*0.50290015;
   float APV2_fqy = 1;
   float APV3_fqy = 1*0.88373339;

   // run 534
   // float APV1_fqx = 1*0.501387*1.00069;
   // float APV2_fqx = 1;
   // float APV3_fqx = 1*0.39038*1.00047;
   
   // float APV1_fqy = 1*0.466846*1.00316;
   // float APV2_fqy = 1;
   // float APV3_fqy = 1*0.794514*1.00424;

   // fit and integration boundaries --> in terms of strip ID
   double StripMin = 20.;
   double StripMax = 340.;

   //vector collecting calibration constants from outside (external txt)
   double XcalibConst[MAXSTRIP];
   double XcalibErr[MAXSTRIP];

   double YcalibConst[MAXSTRIP];
   double YcalibErr[MAXSTRIP];  

   // strip calibration vectors
   vector<double> FitFullDiff[TMMCH_N_Readout];
   vector<double> FitFullRatio[TMMCH_N_Readout];

   // tf1 di fit
   vector<TF1*> f;
  
   /////// HISTOGRAMS //////
   // ---- Overall, event based ----
   // Mode 1: RAW
   TH2F *hqmaxstrip[TMMCH_N_Readout]      = {0}; //qmax vs xstrip (even strips only)
   TH2F *hqmaxstripFull[TMMCH_N_Readout]  = {0}; //qmax vs xstrip (all strips)

   // Mode 2: external txt calibration
   TH2F *hqmaxstrip_extcal[TMMCH_N_Readout]     = {0};
   TH2F *hqmaxstripFull_extcal[TMMCH_N_Readout] = {0};

   // Mode 3: run-based overall calibration (from g_RawCalibFullRatio)
   TH2F *hqmaxstrip_runcal[TMMCH_N_Readout]     = {0};
   TH2F *hqmaxstripFull_runcal[TMMCH_N_Readout] = {0};
   
   // event based FitSlicesY() histograms
   TH1D *hAmpslice[TMMCH_N_Readout]        = {0};
   TH1D *hMeanslice[TMMCH_N_Readout]       = {0};
   TH1D *hSigmaslice[TMMCH_N_Readout]      = {0};
   TH1D *hChi2slice[TMMCH_N_Readout]       = {0};
   TH1D *hAmpsliceFull[TMMCH_N_Readout]    = {0};
   TH1D *hMeansliceFull[TMMCH_N_Readout]   = {0};
   TH1D *hSigmasliceFull[TMMCH_N_Readout]  = {0};
   TH1D *hChi2sliceFull[TMMCH_N_Readout]   = {0};

   // ---- Block based ----
   // Mode 1: RAW
   vector<TH2F*> hBlockqmaxstrip[TMMCH_N_Readout];
   vector<TH2F*> hBlockqmaxstripFull[TMMCH_N_Readout];
    
   // FitSlicesY() per block
   vector<TH1D*> hBlockAmpslice[TMMCH_N_Readout];
   vector<TH1D*> hBlockMeanslice[TMMCH_N_Readout];
   vector<TH1D*> hBlockSigmaslice[TMMCH_N_Readout];
   vector<TH1D*> hBlockChi2slice[TMMCH_N_Readout];
   vector<TH1D*> hBlockAmpsliceFull[TMMCH_N_Readout];
   vector<TH1D*> hBlockMeansliceFull[TMMCH_N_Readout];
   vector<TH1D*> hBlockSigmasliceFull[TMMCH_N_Readout];
   vector<TH1D*> hBlockChi2sliceFull[TMMCH_N_Readout];

   // Mode 2 (block): external txt calibration
   vector<TH2F*> hBlockqmaxstrip_extcal[TMMCH_N_Readout];
   vector<TH2F*> hBlockqmaxstripFull_extcal[TMMCH_N_Readout];

   // Mode 3 (block): overall run-based calibration (same graph used in overall mode 3)
   vector<TH2F*> hBlockqmaxstrip_runcal[TMMCH_N_Readout];
   vector<TH2F*> hBlockqmaxstripFull_runcal[TMMCH_N_Readout];

   // Mode 4 (block): per-block calibration (from g_BlockRawCalibFullRatio)
   vector<TH2F*> hBlockqmaxstrip_blockcal[TMMCH_N_Readout];
   vector<TH2F*> hBlockqmaxstripFull_blockcal[TMMCH_N_Readout];
   
   ////// GRAPHS //////
   // block based graphs
   TGraphErrors *g_BlockBeamSpot[TMMCH_N_Readout]   = {0};
   TGraphErrors *g_BlockBeamSpread[TMMCH_N_Readout] = {0};
   TGraphErrors *g_BlockBeamCharge[TMMCH_N_Readout] = {0};

   //TGraphs for strip calibration (overall run-based)
   TGraphErrors *g_RawCalibFullDiff[TMMCH_N_Readout]  = {0};
   TGraphErrors *g_RawCalibFullRatio[TMMCH_N_Readout] = {0};
   
   //block based TGraphs for strip calibration (per-block)
   vector<TGraphErrors*> g_BlockRawCalibFullDiff[TMMCH_N_Readout];
   vector<TGraphErrors*> g_BlockRawCalibFullRatio[TMMCH_N_Readout];

};

#endif

#ifdef RecoTMM_cxx
RecoTMM::RecoTMM(TObjArray *inputFileNameList,
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
      cerr << "ERROR: empty input file list in RecoTMM constructor" << endl;
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

RecoTMM::~RecoTMM()
{
   if (!fTree) return;
}

Int_t RecoTMM::GetEntry(Long64_t entry)
{
   if (!fTree) return 0;
   return fTree->GetEntry(entry);
}

Long64_t RecoTMM::LoadTree(Long64_t entry)
{
   if (!fTree) return -5;

   Long64_t centry = fTree->LoadTree(entry);
   if (centry < 0) return centry;

   return centry;
}

void RecoTMM::Init(TChain *tree)
{
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
}

bool RecoTMM::Notify()
{
   return true;
}

#endif // #ifdef RecoTMM_cxx
