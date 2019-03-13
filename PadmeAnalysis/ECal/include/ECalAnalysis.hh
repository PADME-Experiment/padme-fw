#ifndef ECalAnalysis_h
#define ECalAnalysis_h 1

#include "TObject.h"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TECalRecoEvent;
class TRecoVClusCollection;

class ECalAnalysis : public TObject
{

public:
  ECalAnalysis();
  ~ECalAnalysis();

  Bool_t Init(TECalRecoEvent* ev, TRecoVClusCollection* cl, Int_t v);
  Bool_t Process();
  Bool_t InitHistos();
  Bool_t Finalize(){return true;}
  
  
private:
  TECalRecoEvent* fhitEvent;
  TRecoVClusCollection* fClColl;

  Int_t fVerbose;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

