#include "PadmeAnaInMCHitsEvent.hh"

#include "TMCEvent.hh"
#include "TTargetMCEvent.hh"
#include "TEVetoMCEvent.hh"
#include "TPVetoMCEvent.hh"
#include "THEPVetoMCEvent.hh"
#include "TECalMCEvent.hh"
#include "TSACMCEvent.hh"


PadmeAnaInMCHitsEvent::PadmeAnaInMCHitsEvent(){
  
  evType = MCHits;
  fMCTree             = NULL;
  fMCEvent            = NULL;
  fTargetMCEvent      = NULL;
  fEVetoMCEvent       = NULL;
  fPVetoMCEvent       = NULL;
  fHEPVetoMCEvent     = NULL;
  fECalMCEvent        = NULL;
  fSACMCEvent         = NULL;
}

PadmeAnaInMCHitsEvent::PadmeAnaInMCHitsEvent(TTree *tree){
  evType = MCHits;

  fMCTree=tree;
  
  // fMCEvent            = new TMCEvent();
  // fTargetMCEvent      = new TTargetMCEvent();
  // fEVetoMCEvent       = new TEVetoMCEvent();
  // fPVetoMCEvent       = new TPVetoMCEvent();
  // fHEPVetoMCEvent     = new THEPVetoMCEvent();
  // fECalMCEvent        = new TECalMCEvent();
  // fSACMCEvent         = new TSACMCEvent();
  
  fMCEvent            = NULL;
  fTargetMCEvent      = NULL;
  fEVetoMCEvent       = NULL;
  fPVetoMCEvent       = NULL;
  fHEPVetoMCEvent     = NULL;
  fECalMCEvent        = NULL;
  fSACMCEvent         = NULL;

  
  fEntry = 0;
  
  TObjArray* branches = tree->GetListOfBranches();

  for(Int_t iBranch = 0; iBranch < branches->GetEntries(); iBranch++){
    TString branchName = ((TBranch*)(*branches)[iBranch])->GetName();
    TClass* branchObjectClass = TClass::GetClass(((TBranch*)(*branches)[iBranch])->GetClassName());
    std::cout << "Found Branch " << branchName.Data() << " containing " << branchObjectClass->GetName() << std::endl;
    if (branchName=="Event") {
      tree->SetBranchAddress(branchName.Data(),&fMCEvent);
    } else if (branchName=="Target") {
      tree->SetBranchAddress(branchName.Data(),&fTargetMCEvent);
    } else if (branchName=="EVeto") {
      tree->SetBranchAddress(branchName.Data(),&fEVetoMCEvent);
    } else if (branchName=="PVeto") {
      tree->SetBranchAddress(branchName.Data(),&fPVetoMCEvent);
    } else if (branchName=="HEPVeto") {
      tree->SetBranchAddress(branchName.Data(),&fHEPVetoMCEvent);
    } else if (branchName=="ECal") {
      tree->SetBranchAddress(branchName.Data(),&fECalMCEvent);
    } else if (branchName=="SAC") {
      tree->SetBranchAddress(branchName.Data(),&fSACMCEvent);
    }
  }

}

int PadmeAnaInMCHitsEvent::NextEvent(){
  return fMCTree->GetEntry(fEntry++);
}



PadmeAnaInMCHitsEvent::~PadmeAnaInMCHitsEvent(){
  //  if (fMCTree           	)delete fMCTree             ; //Part of the file
  if (fMCEvent           	)delete fMCEvent            ;
  if (fTargetMCEvent     	)delete fTargetMCEvent      ;
  if (fEVetoMCEvent      	)delete fEVetoMCEvent       ;
  if (fPVetoMCEvent      	)delete fPVetoMCEvent       ;
  if (fHEPVetoMCEvent    	)delete fHEPVetoMCEvent     ;
  if (fECalMCEvent       	)delete fECalMCEvent        ;
  if (fSACMCEvent               )delete fSACMCEvent         ; 
}


void PadmeAnaInMCHitsEvent::Init(){
  
}
