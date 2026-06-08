#ifndef RecoTMM_h
#define RecoTMM_h

#include "TRandom.h"
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

#define TMMCH_N_Readout 2

class RecoTMM {
public :
   TChain         *fTree;      //! pointer to the analyzed TTree
   // Int_t          fCurrent;    //! current Tree number in a TTree
   Bool_t         fOwnChain;   //! true if this class created the TTree
   int            RunID = 0;
   int            DetRunID = 0;
   int            maxEvents=0;
   TString        outputFileName = "Calibration_TMM.root";
   
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
  
   RecoTMM(TObjArray *inputFileNameList, int RunID=0, int DetRunID=0, int maxEvents=0, TString outputFileName="Calibration_TMM.root");   virtual ~RecoTMM();
   // virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TChain *tree);
   virtual bool     Notify();
   
   virtual void     StripFinder(int &iReadout, int &iStrip);
   virtual void     CoordinateFinder(int iStrip, const vector<short> &camp, int &iReadout, double &x_strip, double &q_strip);
   virtual void     GlobalCoordinate(int iReadout, double &x_strip);
   virtual void     ComputeBeamSpot(int iReadout, double &x_spot, double &x_rms, double &q_beam);
   virtual void     ComputeBeamSpotFromVectors(vector<double> &x_vec, vector<double> &q_vec, double &x_spot, double &x_rms, double &q_beam);
   virtual void     ComputeBeamStatsFromVectors(vector<double> &x_vec, vector<double> &q_vec, double &x_spot, double &err_x_spot, double &x_rms, double &err_x_rms, double &q_beam, double &err_q_beam);   
   virtual void     UpdateSummaryTxt(TString filename, int RunID, double qx, double err_qx, double qy, double err_qy, double x, double err_x, double y, double err_y, double sigmax, double err_sigmax, double sigmay, double err_sigmay);
   virtual void     LoopFileList(TObjArray &inputFileNameList);
   
   ///////ENVIROMENT VARIABLES//////
   TString tmm_tag[TMMCH_N_Readout] = {"X", "Y"};
   float pitch=0.250; //mm
   
   int maxStrip=358, maxStripApv=128;                                                                                      
   float xmax = maxStrip*pitch;

   // double d_firstcross_lastcross_strips = 89.262; //mm
   double d_firstcross_lastcross_strips = 0; //mm
   double GLOBAL_X_TRANSLATION = - (d_firstcross_lastcross_strips/2); //mm 
   double GLOBAL_Y_TRANSLATION = - (d_firstcross_lastcross_strips/2); //mm 

   //POSITION REFERENCE POINT 1  
   double xP1_cad = 101.074, yP1_cad = 129.949; //mm    CAD MEASUREMENTs
   double xP1_met = 101.69 , yP1_met = 130.26;  //mm    METROLOGY MEASUREMENTs (Report PADME 23-05-2025)
   //POSITION REFERENCE POINT 2
   double xP2_cad = 127.174, yP2_cad = -98.791; //mm    CAD MEASUREMENTs
   double xP2_med = 131.48 , yP2_med = -97.78;  //mm    METROLOGY MEASUREMENTs (Report PADME 23-05-2025)
   
   float clock = 25., ncamp=27; //with the 675ns time window (for the 450 one ncamp is 18)

   // prima calibrazione APV da ripetere in principio Run-per-Run
   // float APV1_fqx = 0.6355*0.8710; 
   // float APV2_fqx = 1;
   // float APV3_fqx = 0.3194*0.8261;

   // float APV1_fqy = 0.7271*0.9437;
   // float APV2_fqy = 1;
   // float APV3_fqy = 0.9104*0.9574;

   // float APV1_fqx = 1; 
   // float APV2_fqx = 1;
   // float APV3_fqx = 1;

   // float APV1_fqy = 1;
   // float APV2_fqy = 1;
   // float APV3_fqy = 1;

   float APV1_fqx = 1*0.569244*0.999954; // APV equalization performed twice on run677 21.05.26 - to be repeated on other runs
   float APV2_fqx = 1;                   // checking for stability run-by-run
   float APV3_fqx = 1*0.422527*0.99999; 

   float APV1_fqy = 1*0.564602*1.00291;
   float APV2_fqy = 1;
   float APV3_fqy = 1*0.930592*1.00401;
  
   //hit vectors
   vector<double> x_mean[TMMCH_N_Readout];
   vector<double> q_mean[TMMCH_N_Readout];
   double x_spot[TMMCH_N_Readout];
   double rms[TMMCH_N_Readout];
   double q_beam[TMMCH_N_Readout];
   double err_x_spot[TMMCH_N_Readout];
   double err_x_rms[TMMCH_N_Readout];
   double err_q_beam[TMMCH_N_Readout];

   // strip calibration vectors
   vector<double> FitFullDiff[TMMCH_N_Readout];
   vector<double> FitFullRatio[TMMCH_N_Readout];

   // tf1 di fit
   vector<TF1*> f;
  
   /////// HISTOGRAMS //////
   // event by event histograms
   TH2F *hqmaxstrip[TMMCH_N_Readout] = {0}; //qmax vs xstrip distribution
   TH2F *hqmaxstripFull[TMMCH_N_Readout] = {0}; //qmax vs xstrip distribution
   TH1F *hBeam[TMMCH_N_Readout] = {0}; //xstrip distribution charge weighted (selected strips)
   TH1F *hBeamFull[TMMCH_N_Readout] = {0}; //xstrip distribution charge weighted

   // FitSlicesY() histograms
   TH1D *hAmpslice[TMMCH_N_Readout] = {0};
   TH1D *hMeanslice[TMMCH_N_Readout] = {0};
   TH1D *hSigmaslice[TMMCH_N_Readout] = {0};
   TH1D *hAmpsliceFull[TMMCH_N_Readout] = {0};
   TH1D *hMeansliceFull[TMMCH_N_Readout] = {0};
   TH1D *hSigmasliceFull[TMMCH_N_Readout] = {0};

   //calibrated histograms
   TH2F *hqmaxstrip_cal[TMMCH_N_Readout] = {0}; //qmax vs xstrip distribution
   TH2F *hqmaxstripFull_cal[TMMCH_N_Readout] = {0}; //qmax vs xstrip distribution
   TH1F *hBeam_cal[TMMCH_N_Readout] = {0}; //xstrip distribution charge weighted (selected strips)
   TH1F *hBeamFull_cal[TMMCH_N_Readout] = {0}; //xstrip distribution charge weighted
   // calibrated histograms after threshold selection
   TH2F *hqmaxstrip_cal3s[TMMCH_N_Readout] = {0}; //qmax vs xstrip distribution
   TH2F *hqmaxstripFull_cal3s[TMMCH_N_Readout] = {0}; //qmax vs xstrip distribution
   TH1F *hBeam_cal3s[TMMCH_N_Readout] = {0}; //xstrip distribution charge weighted (selected strips)
   TH1F *hBeamFull_cal3s[TMMCH_N_Readout] = {0}; //xstrip distribution charge weighted

   ////// GRAPHS //////
   // event by event graphs
   TGraphErrors *g_BeamSpot[TMMCH_N_Readout] = {0};   //Beam spot over events
   TGraphErrors *g_BeamSpread[TMMCH_N_Readout] = {0}; //Beam spread over events
   TGraphErrors *g_BeamCharge[TMMCH_N_Readout] = {0}; //Beam charge over events
   

   //TGraphs for strip calibration
   TGraphErrors *g_FitFullDiff[TMMCH_N_Readout] = {0}; // to identify possible dead strips
   TGraphErrors *g_FitFullRatio[TMMCH_N_Readout] = {0}; // to define calibration constants
   TGraphErrors *g_FitFullDiff3s[TMMCH_N_Readout] = {0}; // to identify possible dead strips
   TGraphErrors *g_FitFullRatio3s[TMMCH_N_Readout] = {0}; // to define calibration constants after threshold selection

};

#endif

#ifdef RecoTMM_cxx
RecoTMM::RecoTMM(TObjArray *inputFileNameList,
                 int RunID,
                 int DetRunID,
                 int maxEvents,
                 TString outputFileName) :
   fTree(0),
   // fCurrent(-1),
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
// Read contents of entry.
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
   // fCurrent = -1;

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

bool RecoTMM::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return true;
}


#endif // #ifdef RecoTMM_cxx
