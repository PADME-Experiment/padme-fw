#ifndef EVetoAnalysis_h
#define EVetoAnalysis_h 1

#include "TObject.h"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TEVetoRecoEvent;
class TRecoVClusCollection;

class EVetoAnalysis : public TObject
{

public:
  EVetoAnalysis();
  ~EVetoAnalysis();

  Bool_t Init(TEVetoRecoEvent* ev, TRecoVClusCollection* cl, Int_t v);
  Bool_t Process();
  Bool_t Finalize(){return true;}
  Bool_t InitHistos();
  
  
  
private:
  TEVetoRecoEvent* fhitEvent;
  TRecoVClusCollection* fClColl;
  Double_t fTmax;
  Double_t fTmin;
  
  Int_t fVerbose;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

