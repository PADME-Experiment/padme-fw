#ifndef ECalAnalysis_h
#define ECalAnalysis_h 1

#include "TObject.h"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TECalRecoEvent;

class ECalAnalysis : public TObject
{

public:
  ECalAnalysis();
  ~ECalAnalysis();

  Bool_t Init(Int_t nh, TECalRecoEvent* ev);
  Bool_t Process();
  
  
  
private:
  Int_t fNhits;
  TECalRecoEvent* fhitEvent;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

