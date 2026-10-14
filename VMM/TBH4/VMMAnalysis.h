//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sun Jul 19 17:17:10 2026 by ROOT version 6.29/01
// from TTree hits/hits
// found on file: runNNN_YYYYMMDD_HHMMSS.root
//////////////////////////////////////////////////////////

#ifndef VMMAnalysis_h
#define VMMAnalysis_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class VMMAnalysis {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain
   TString  fRunName = "";
   // Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   //Hit             *hits;
   UInt_t          id;
   UChar_t         det;
   UChar_t         plane;
   UChar_t         fec;
   UChar_t         vmm;
   Double_t        readout_time;
   Double_t        time;
   Double_t        trigger_time;
   UChar_t         geo_id;
   UChar_t         ch;
   UShort_t        pos;
   UShort_t        bcid;
   UShort_t        tdc;
   UShort_t        adc;
   Bool_t          over_threshold;
   Double_t        chip_time;
   UShort_t        event_counter;

   // List of branches
   TBranch        *b_hits_id;   //!
   TBranch        *b_hits_det;   //!
   TBranch        *b_hits_plane;   //!
   TBranch        *b_hits_fec;   //!
   TBranch        *b_hits_vmm;   //!
   TBranch        *b_hits_readout_time;   //!
   TBranch        *b_hits_time;   //!
   TBranch        *b_hits_trigger_time;   //!
   TBranch        *b_hits_geo_id;   //!
   TBranch        *b_hits_ch;   //!
   TBranch        *b_hits_pos;   //!
   TBranch        *b_hits_bcid;   //!
   TBranch        *b_hits_tdc;   //!
   TBranch        *b_hits_adc;   //!
   TBranch        *b_hits_over_threshold;   //!
   TBranch        *b_hits_chip_time;   //!
   TBranch        *b_hits_event_counter;   //!

   VMMAnalysis(TString RunName, TTree *tree=0);
   virtual ~VMMAnalysis();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();   
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef VMMAnalysis_cxx
VMMAnalysis::VMMAnalysis(TString RunName, TTree *tree) : fChain(0), fRunName(RunName) {
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(Form("run%s.root", RunName.Data()));
      if (!f || !f->IsOpen()) {
         f = new TFile(Form("run%s.root", RunName.Data()));
      }
      f->GetObject("hits",tree);
   }
   Init(tree);
}

VMMAnalysis::~VMMAnalysis() {
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t VMMAnalysis::GetEntry(Long64_t entry) {
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}

Long64_t VMMAnalysis::LoadTree(Long64_t entry) {
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

void VMMAnalysis::Init(TTree *tree) {
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("id", &id, &b_hits_id);
   fChain->SetBranchAddress("det", &det, &b_hits_det);
   fChain->SetBranchAddress("plane", &plane, &b_hits_plane);
   fChain->SetBranchAddress("fec", &fec, &b_hits_fec);
   fChain->SetBranchAddress("vmm", &vmm, &b_hits_vmm);
   fChain->SetBranchAddress("readout_time", &readout_time, &b_hits_readout_time);
   fChain->SetBranchAddress("time", &time, &b_hits_time);
   fChain->SetBranchAddress("trigger_time", &trigger_time, &b_hits_trigger_time);
   fChain->SetBranchAddress("geo_id", &geo_id, &b_hits_geo_id);
   fChain->SetBranchAddress("ch", &ch, &b_hits_ch);
   fChain->SetBranchAddress("pos", &pos, &b_hits_pos);
   fChain->SetBranchAddress("bcid", &bcid, &b_hits_bcid);
   fChain->SetBranchAddress("tdc", &tdc, &b_hits_tdc);
   fChain->SetBranchAddress("adc", &adc, &b_hits_adc);
   fChain->SetBranchAddress("over_threshold", &over_threshold, &b_hits_over_threshold);
   fChain->SetBranchAddress("chip_time", &chip_time, &b_hits_chip_time);
   fChain->SetBranchAddress("event_counter", &event_counter, &b_hits_event_counter);
   Notify();
}

Bool_t VMMAnalysis::Notify() {
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void VMMAnalysis::Show(Long64_t entry) {
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}

Int_t VMMAnalysis::Cut(Long64_t entry) {
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef VMMAnalysis_cxx
