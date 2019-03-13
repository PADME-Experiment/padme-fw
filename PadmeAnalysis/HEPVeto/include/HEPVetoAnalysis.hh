#ifndef HEPVetoAnalysis_h
#define HEPVetoAnalysis_h 1

#include "TObject.h"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class THEPVetoRecoEvent;
class TRecoVClusCollection;

class HEPVetoAnalysis : public TObject
{

public:
  HEPVetoAnalysis();
  ~HEPVetoAnalysis();

  Bool_t Init(THEPVetoRecoEvent* ev, TRecoVClusCollection* cl, Int_t v);
  Bool_t Process();
  Bool_t Finalize(){return true;}
  Bool_t InitHistos();
  
  
  
private:
  THEPVetoRecoEvent* fhitEvent;
  TRecoVClusCollection* fClColl;
  Double_t fTmax;
  Double_t fTmin;
  
  Int_t fVerbose;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

