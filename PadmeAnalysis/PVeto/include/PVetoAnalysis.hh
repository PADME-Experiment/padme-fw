#ifndef PVetoAnalysis_h
#define PVetoAnalysis_h 1

#include "TObject.h"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TPVetoRecoEvent;
class TRecoVClusCollection;

class PVetoAnalysis : public TObject
{

public:
  PVetoAnalysis();
  ~PVetoAnalysis();

  Bool_t Init(TPVetoRecoEvent* ev, TRecoVClusCollection* cl, Int_t v);
  Bool_t Process();
  Bool_t Finalize(){return true;}
  Bool_t InitHistos();
  
  
  
private:
  TPVetoRecoEvent* fhitEvent;
  TRecoVClusCollection* fClColl;
  Double_t fTmax;
  Double_t fTmin;
  
  Int_t fVerbose;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

