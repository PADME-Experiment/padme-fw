#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include "Chamber.hh"

//Chamber::Chamber(TTree *tree) : fChain(0) 
Chamber::Chamber(TString fileName, TTree *tree) : fChain(0) 
{
  //TFile* f = new TFile("run30.root");
  printf("Opening file %s\n",fileName.Data());
  TFile* f = new TFile(fileName);
  f->GetObject("apv_raw",tree);
  Init(tree);
}

Chamber::~Chamber()
{
  if (!fChain) return;
  delete fChain->GetCurrentFile();
}

Int_t Chamber::GetEntry(Long64_t entry)
{
  // Read contents of entry.
  if (!fChain) return 0;
  return fChain->GetEntry(entry);
}

Long64_t Chamber::LoadTree(Long64_t entry)
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

void Chamber::Init(TTree *tree)
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

Bool_t Chamber::Notify()
{
  // The Notify() function is called when a new file is opened. This
  // can be either for a new TTree in a TChain or when when a new TTree
  // is started when using PROOF. It is normally not necessary to make changes
  // to the generated code, but the routine can be extended by the
  // user if needed. The return value is currently not used.
  
  return kTRUE;
}

void Chamber::Show(Long64_t entry)
{
  // Print contents of entry.
  // If entry is not specified, print current entry
  if (!fChain) return;
  fChain->Show(entry);
}

Int_t Chamber::Cut(Long64_t entry)
{
  // This function may be called from Loop.
  // returns  1 if entry is accepted.
  // returns -1 otherwise.
  return 1;
}
