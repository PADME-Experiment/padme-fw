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

  Bool_t Init(Int_t nh, TECalRecoEvent* ev, TRecoVClusCollection* cl);
  Bool_t Process();
  
  
  
private:
  Int_t fNhits;
  TECalRecoEvent* fhitEvent;
  TRecoVClusCollection* fClColl;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

