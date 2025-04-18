#ifndef ChamberEvent_h
#define ChamberEvent_h

#include <TTree.h>
#include <TBranch.h>

class ChamberEvent {

public :

  ChamberEvent();
  ~ChamberEvent();

  void     Init(TTree*);

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

private:

  TTree *fChain;   //!pointer to the analyzed TTree or TChain
  
  // List of branches
  TBranch        *b_evt;
  TBranch        *b_error;
  TBranch        *b_daqTimeSec;
  TBranch        *b_daqTimeMicroSec;
  TBranch        *b_srsTimeStamp;
  TBranch        *b_srsTrigger;
  TBranch        *b_srsFec;
  TBranch        *b_srsChip;
  TBranch        *b_srsChan;
  TBranch        *b_mmChamber;
  TBranch        *b_mmLayer;
  TBranch        *b_mmReadout;
  TBranch        *b_mmStrip;
  TBranch        *b_raw_q;
  TBranch        *b_max_q;
  TBranch        *b_t_max_q;

};

#endif
