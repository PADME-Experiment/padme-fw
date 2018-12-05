#ifndef PVetoAnalysis_h
#define PVetoAnalysis_h 1

#include "TObject.h"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TPVetoRecoEvent;

class PVetoAnalysis : public TObject
{

public:
  PVetoAnalysis();
  ~PVetoAnalysis();

  Bool_t Init(Int_t nh, TPVetoRecoEvent* ev);
  Bool_t Process();
  
  
  
private:
  Int_t fNhits;
  TPVetoRecoEvent* fhitEvent;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

