#define apv_raw_cxx
#include "apv_raw.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void apv_raw::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L apv_raw.C
//      Root > apv_raw t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
   }
}

apv_raw::apv_raw(TTree *tree) : fChain(0)
{
    // if parameter tree is not specified (or zero), connect the file
    // used to generate this class and read the Tree.
    if (tree == 0) {
        TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("run691.root");
        if (!f || !f->IsOpen()) {
            f = new TFile("run691.root");
        }
        f->GetObject("apv_raw",tree);
        
    }
    Init(tree);
}

apv_raw::~apv_raw()
{
    if (!fChain) return;
    delete fChain->GetCurrentFile();
}

Int_t apv_raw::GetEntry(Long64_t entry)
{
    // Read contents of entry.
    if (!fChain) return 0;
    return fChain->GetEntry(entry);
}
Long64_t apv_raw::LoadTree(Long64_t entry)
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

void apv_raw::Init(TTree *tree)
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
    Notify();
}

Bool_t apv_raw::Notify()
{
    // The Notify() function is called when a new file is opened. This
    // can be either for a new TTree in a TChain or when when a new TTree
    // is started when using PROOF. It is normally not necessary to make changes
    // to the generated code, but the routine can be extended by the
    // user if needed. The return value is currently not used.
    
    return kTRUE;
}

void apv_raw::Show(Long64_t entry)
{
    // Print contents of entry.
    // If entry is not specified, print current entry
    if (!fChain) return;
    fChain->Show(entry);
}
Int_t apv_raw::Cut(Long64_t entry)
{
    (void) entry;
    // This function may be called from Loop.
    // returns  1 if entry is accepted.
    // returns -1 otherwise.
    return 1;
}
