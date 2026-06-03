//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Dec  2 12:11:33 2024 by ROOT version 6.24/08
// from TTree apv_raw/APVRawData
// found on file: ../raw/run2189.root
//////////////////////////////////////////////////////////

#ifndef Reco_h
#define Reco_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TLine.h>
#include <TCanvas.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <stddef.h>
#include <TSystem.h>
#include <TPaveText.h>
#include <thread>
#include <chrono>
#include <TF1.h>
#include <TProfile.h>
#include <algorithm> 
#include <vector>
#include <string>
#include <numeric>
#include <map>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TObject.h>
#include <TMath.h>

// Header file for the classes stored in the TTree if any.


class Reco {
public :
   TTree         *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t          fCurrent; //!current Tree number in a TChain
   Bool_t         fOwnChain;   //! true if this class created the TChain
   int            RunID;
   int            DetRunID;  
   Long64_t       maxEvents;

   // Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
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
   TBranch        *b_evt;   //!
   TBranch        *b_error;   //!
   TBranch        *b_daqTimeSec;   //!
   TBranch        *b_daqTimeMicroSec;   //!
   TBranch        *b_srsTimeStamp;   //!
   TBranch        *b_srsTrigger;   //!
   TBranch        *b_srsFec;   //!
   TBranch        *b_srsChip;   //!
   TBranch        *b_srsChan;   //!
   TBranch        *b_mmChamber;   //!
   TBranch        *b_mmLayer;   //!
   TBranch        *b_mmReadout;   //!
   TBranch        *b_mmStrip;   //!
   TBranch        *b_raw_q;   //!
   TBranch        *b_max_q;   //!
   TBranch        *b_t_max_q;   //!
  
   RecoTMM(TTree *tree=0, int RunID=0, int DetRunID=0, Int_t maxFileCounter=0, Long64_t maxEvents=0);
   virtual ~Reco();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(TString padmenumber1, TString runnumber1);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

   //virtual void     CoordinateFinder(int iStrip, int iLayer, vector<double> camp, double &t_strip, double &x_strip, double &z_strip, double &q_strip);
   virtual void     CoordinateFinder(int iStrip, int iLayer, const std::vector<short> &camp, double &t_strip, double &x_strip, double &z_strip, double &q_strip);

   virtual void FitBeamBlock(TH1F* h_block, int blockID, TH1F* h_mu_vs_time, TH1F* h_sigma_vs_time, TH1F* h_charge_vs_time);
   
   virtual void FitBeamBlock2D(TH2F* h_block, int blockID, TH1F* h_muX_vs_time, TH1F* h_sigmaX_vs_time, TH1F* h_muY_vs_time, TH1F* h_sigmaY_vs_time);
   virtual TH1F* ExtractChargePeakPerStrip(TH2F* h_charge_pos_2D, int blockID, double xmin, double xmax);
   
   // Vettori per plot finale
   vector<double> beamMeanXBlocks;
   vector<double> beamMeanYBlocks;
   vector<double> beamSigmaXBlocks;
   vector<double> beamSigmaYBlocks;

   TH2F* hBeamXY = nullptr; // istogramma 2D cumulativo
  
   ///////ENVIROMENT VARIABLES//////
   float pitch=1.2; //mm

   //float geo_hole[8] = {2.4,2.4,8.4,8.4,8.4,8.4,2.4,2.4};//mm
   float geo_hole[8] = {8.4,8.4,2.4,2.4,2.4,2.4,8.4,8.4};//mm      
      
   int maxStrip=512, maxStripApv=128;
   float xmax = maxStrip*pitch;

   float z0=0, zm=50., z2=100.; //mm          z coord of the chamber starting from bottom to top 
   //float vd=0.105; //mm/ns in place of 0.105      
   
   float vd=0.1002; //mm/ns for run 2292 and 2271
   //float vd=0.1210; //mm/ns for run 2417 
   
   float clock = 25., ncamp=27; //with the 675ns time window (for the 450 one ncamp is 18)                               
   double qnoisecut = 10000;

   int maxholes=2;
   double xnstrip=3;

   float shift_coord[8] = {18.54,18.54,12.1,12.1,18.54,18.54,12.1,12.1};//mm
      
      
   double shift_x = 12.1 ;//mm
   double shift_y = 18.54; //mm
   double rot_angle = -0.26; //rad (rotazione oraria con targhetta alle spalle)
   
   double sigma_x = 0.5; //mm (per hit adjoint)

   //// T0 calibration
   int nsample = 4;
   int sample = 512/nsample;              
   std::vector<double> T0, T0_err, TD, TD_err;
   //  std::vector<int> icluster_ini, icluster_lenght, icluster_type, icluster_nholes;
};

#endif

#ifdef Reco_cxx
Reco::Reco(TTree *tree,
                 int RunID,
                 int DetRunID,
                 Int_t maxFileCounter,
                 Long64_t maxEvents) :
   fChain(0),
   fCurrent(-1),
   fOwnChain(kFALSE),
   RunID(RunID),
   DetRunID(DetRunID),
   maxEvents(maxEvents)
{
   if (tree == 0) {

      TString basePath = "/data9Vd1/padme/mancinima/mmdata/2025";   
      // TString dir = "/data9Vd1/padme/mancinima/mmdata/2025/run_" + padmenumber + "/";

      TString dirPattern = Form("%s/run_0080%i_*", basePath.Data(), RunID);
      TString rundir = gSystem->GetFromPipe(
         Form("ls -d %s 2>/dev/null | head -1",
              dirPattern.Data())
      );

      if (rundir == "") {
         std::cerr << "ERROR: cannot find directory matching "
                   << dirPattern << std::endl;
         return;
      }

      TString dirName = gSystem->BaseName(rundir);

      TChain *chain = new TChain("apv_raw");
      fOwnChain = kTRUE;

      Long64_t totalEntries = 0;

      for (Int_t i = 0; i <= maxFileCounter; i++) {

         TString fileName;

         if (i == 0) {
            TString cmd = Form(
               "ls %s/run%i_%s_*.root 2>/dev/null | "
               "grep -E 'run%i_%s_[0-9]+\\.root$' | head -1",
               rundir.Data(),
               DetRunID,
               dirName.Data(),
               DetRunID,
               dirName.Data()
            );

            fileName = gSystem->GetFromPipe(cmd);
         }
         else {
            TString filePattern = Form("%s/run%i_%s_*_%d.root",
                                       rundir.Data(),
                                       DetRunID,
                                       dirName.Data(),
                                       i);

            fileName = gSystem->GetFromPipe(
               Form("ls %s 2>/dev/null | head -1",
                    filePattern.Data())
            );
         }

         if (fileName == "") {
            std::cerr << "WARNING: cannot find file for counter "
                      << i << std::endl;
            continue;
         }

         TFile testFile(fileName);
         TTree *testTree = (TTree*)testFile.Get("apv_raw");

         if (!testTree) {
            std::cerr << "WARNING: cannot find TTree apv_raw in "
                      << fileName << std::endl;
            continue;
         }

         Long64_t nEntriesFile = testTree->GetEntries();

         chain->Add(fileName);
         totalEntries += nEntriesFile;

         std::cout << "Adding file: " << fileName
                   << " entries=" << nEntriesFile
                   << " total=" << totalEntries
                   << std::endl;

         if (maxEvents > 0 && totalEntries >= maxEvents) {
            std::cout << "Reached requested maxEvents = "
                      << maxEvents << std::endl;
            break;
         }
      }

      if (chain->GetNtrees() == 0) {
         std::cerr << "ERROR: no files added to TChain" << std::endl;
         delete chain;
         fOwnChain = kFALSE;
         return;
      }

      tree = chain;
   }

   Init(tree);
}


// #ifdef Reco_cxx
// Reco::Reco(TTree *tree, TString padmenumber, TString runnumber) : fChain(0)
// {
//    if (tree == 0) {

//       TChain *chain = new TChain("apv_raw");

//       TString path = "/data9Vd1/padme/mancinima/mmdata/2025/run_" + padmenumber + "/run" + runnumber + "*.root";

//       chain->Add(path);

//       tree = chain;
//    }

//    Init(tree);
// }

// #ifdef Reco_cxx
// Reco::Reco(TTree *tree, TString runnumber) : fChain(0) 
// {
// // if parameter tree is not specified (or zero), connect the file
// // used to generate this class and read the Tree.
//    if (tree == 0) {
//       TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("../data/run"+runnumber+".root");
//       if (!f || !f->IsOpen()) {
//          f = new TFile("../data/run"+runnumber+".root");
//       }
//       f->GetObject("apv_raw",tree);

//    }
//    Init(tree);
// }

Reco::~Reco()
{
   if (!fChain) return;

   if (fOwnChain) {
      delete fChain;
      fChain = 0;
   }
}

Int_t Reco::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Reco::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void Reco::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   srsFec = 0;
   srsChip = 0;
   srsChan = 0;
   mmChamber = 0;
   mmLayer = 0;
   mmReadout = 0;
   mmStrip = 0;
   raw_q = 0;
   max_q = 0;
   t_max_q = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("evt", &evt, &b_evt);
   fChain->SetBranchAddress("error", &error, &b_error);
   fChain->SetBranchAddress("daqTimeSec", &daqTimeSec, &b_daqTimeSec);
   fChain->SetBranchAddress("daqTimeMicroSec", &daqTimeMicroSec, &b_daqTimeMicroSec);
   fChain->SetBranchAddress("srsTimeStamp", &srsTimeStamp, &b_srsTimeStamp);
   fChain->SetBranchAddress("srsTrigger", &srsTrigger, &b_srsTrigger);
   fChain->SetBranchAddress("srsFec", &srsFec, &b_srsFec);
   fChain->SetBranchAddress("srsChip", &srsChip, &b_srsChip);
   fChain->SetBranchAddress("srsChan", &srsChan, &b_srsChan);
   fChain->SetBranchAddress("mmChamber", &mmChamber, &b_mmChamber);
   fChain->SetBranchAddress("mmLayer", &mmLayer, &b_mmLayer);
   fChain->SetBranchAddress("mmReadout", &mmReadout, &b_mmReadout);
   fChain->SetBranchAddress("mmStrip", &mmStrip, &b_mmStrip);
   fChain->SetBranchAddress("raw_q", &raw_q, &b_raw_q);
   fChain->SetBranchAddress("max_q", &max_q, &b_max_q);
   fChain->SetBranchAddress("t_max_q", &t_max_q, &b_t_max_q);
   Notify();
}

Bool_t Reco::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Reco::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Reco::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}


#endif // #ifdef Reco_cxx
