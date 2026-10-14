//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Jul 21 11:51:48 2026 by ROOT version 6.38.04
// from TTree events/VMM hits grouped by event_counter
// found on file: Events_run104_20260721104837.root
//////////////////////////////////////////////////////////

#ifndef events_h
#define events_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TObject.h>
#include <TGraph.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TString.h>
#include <TMath.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <vector>

class events {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain
   TString  fRunName = "";

   double DriftVelocity = 0.047; // mm/ns
   double T0 = 93100; // ns 
   double driftgap = 50; // mm
   double pitch = 1.2; // mm

   // Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           event;
   UShort_t        event_counter16;
   UInt_t          counter_epoch;
   Int_t           n_hits;
   Double_t        event_time;
   Double_t        event_time_min;
   Double_t        event_time_max;
   Double_t        event_readout_time_min;
   Double_t        event_readout_time_max;
   vector<unsigned int> *id;
   vector<unsigned char> *det;
   vector<unsigned char> *plane;
   vector<unsigned char> *fec;
   vector<unsigned char> *vmm;
   vector<double>  *readout_time;
   vector<double>  *time;
   vector<double>  *trigger_time;
   vector<unsigned char> *geo_id;
   vector<unsigned char> *ch;
   vector<unsigned short> *pos;
   vector<unsigned short> *bcid;
   vector<unsigned short> *tdc;
   vector<unsigned short> *adc;
   vector<unsigned char> *over_threshold;
   vector<double>  *chip_time;

   // List of branches
   TBranch        *b_event;   //!
   TBranch        *b_event_counter16;   //!
   TBranch        *b_counter_epoch;   //!
   TBranch        *b_n_hits;   //!
   TBranch        *b_event_time;   //!
   TBranch        *b_event_time_min;   //!
   TBranch        *b_event_time_max;   //!
   TBranch        *b_event_readout_time_min;   //!
   TBranch        *b_event_readout_time_max;   //!
   TBranch        *b_id;   //!
   TBranch        *b_det;   //!
   TBranch        *b_plane;   //!
   TBranch        *b_fec;   //!
   TBranch        *b_vmm;   //!
   TBranch        *b_readout_time;   //!
   TBranch        *b_time;   //!
   TBranch        *b_trigger_time;   //!
   TBranch        *b_geo_id;   //!
   TBranch        *b_ch;   //!
   TBranch        *b_pos;   //!
   TBranch        *b_bcid;   //!
   TBranch        *b_tdc;   //!
   TBranch        *b_adc;   //!
   TBranch        *b_over_threshold;   //!
   TBranch        *b_chip_time;   //!

   events(TString RunName, TTree *tree=0);
   virtual ~events();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual bool     Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef events_cxx
events::events(TString RunName, TTree *tree) : fChain(0), fRunName(RunName) {
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(Form("New_Events_run%s.root", RunName.Data()));
      if (!f || !f->IsOpen()) {
         f = new TFile(Form("New_Events_run%s.root", RunName.Data()));
      }
      f->GetObject("events",tree);

   }
   Init(tree);
}

events::~events()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t events::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t events::LoadTree(Long64_t entry)
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

void events::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.

   // Set object pointer
   id = 0;
   det = 0;
   plane = 0;
   fec = 0;
   vmm = 0;
   readout_time = 0;
   time = 0;
   trigger_time = 0;
   geo_id = 0;
   ch = 0;
   pos = 0;
   bcid = 0;
   tdc = 0;
   adc = 0;
   over_threshold = 0;
   chip_time = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("event", &event, &b_event);
   fChain->SetBranchAddress("event_counter16", &event_counter16, &b_event_counter16);
   fChain->SetBranchAddress("counter_epoch", &counter_epoch, &b_counter_epoch);
   fChain->SetBranchAddress("n_hits", &n_hits, &b_n_hits);
   fChain->SetBranchAddress("event_time", &event_time, &b_event_time);
   fChain->SetBranchAddress("event_time_min", &event_time_min, &b_event_time_min);
   fChain->SetBranchAddress("event_time_max", &event_time_max, &b_event_time_max);
   fChain->SetBranchAddress("event_readout_time_min", &event_readout_time_min, &b_event_readout_time_min);
   fChain->SetBranchAddress("event_readout_time_max", &event_readout_time_max, &b_event_readout_time_max);
   fChain->SetBranchAddress("id", &id, &b_id);
   fChain->SetBranchAddress("det", &det, &b_det);
   fChain->SetBranchAddress("plane", &plane, &b_plane);
   fChain->SetBranchAddress("fec", &fec, &b_fec);
   fChain->SetBranchAddress("vmm", &vmm, &b_vmm);
   fChain->SetBranchAddress("readout_time", &readout_time, &b_readout_time);
   fChain->SetBranchAddress("time", &time, &b_time);
   fChain->SetBranchAddress("trigger_time", &trigger_time, &b_trigger_time);
   fChain->SetBranchAddress("geo_id", &geo_id, &b_geo_id);
   fChain->SetBranchAddress("ch", &ch, &b_ch);
   fChain->SetBranchAddress("pos", &pos, &b_pos);
   fChain->SetBranchAddress("bcid", &bcid, &b_bcid);
   fChain->SetBranchAddress("tdc", &tdc, &b_tdc);
   fChain->SetBranchAddress("adc", &adc, &b_adc);
   fChain->SetBranchAddress("over_threshold", &over_threshold, &b_over_threshold);
   fChain->SetBranchAddress("chip_time", &chip_time, &b_chip_time);
   Notify();
}

bool events::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be for a new TTree in a TChain. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return true;
}

void events::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t events::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef events_cxx
