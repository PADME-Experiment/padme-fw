//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Dec  2 12:11:33 2024 by ROOT version 6.24/08
// from TTree apv_raw/APVRawData
// found on file: ../raw/run2189.root
//////////////////////////////////////////////////////////

#ifndef Chamber_h
#define Chamber_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class Chamber {

public :
  TTree          *fChain;   //!pointer to the analyzed TTree or TChain
  Int_t           fCurrent; //!current Tree number in a TChain

  // Fixed size dimensions of array or collections stored in the TTree if any.

  // Declaration of leaf types
  ULong64_t       evt;
  UInt_t          error;
  Int_t           daqTimeSec;
  Int_t           daqTimeMicroSec;
  Int_t           srsTimeStamp;
  UInt_t          srsTrigger;
  std::vector<unsigned int> *srsFec;
  std::vector<unsigned int> *srsChip;
  std::vector<unsigned int> *srsChan;
  std::vector<std::string>  *mmChamber;
  std::vector<int>     *mmLayer;
  std::vector<char>    *mmReadout; 
  std::vector<int>     *mmStrip;
  std::vector<std::vector<short> > *raw_q;
  std::vector<short>   *max_q;
  std::vector<int>     *t_max_q;
  
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

  Chamber(TString, TTree *tree=0);
  ~Chamber();
  Int_t    Cut(Long64_t entry);
  Int_t    GetEntry(Long64_t entry);
  Long64_t LoadTree(Long64_t entry);
  void     Init(TTree *tree);
  void     Loop();
  Bool_t   Notify();
  void     Show(Long64_t entry = -1);
};

#endif
