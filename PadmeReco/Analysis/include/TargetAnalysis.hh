#ifndef TargetAnalysis_h
#define TargetAnalysis_h 1

#include "TObject.h"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TTargetRecoEvent;

class TargetAnalysis : public TObject
{

public:
  TargetAnalysis();
  ~TargetAnalysis();

  Bool_t Init(Int_t nh, TTargetRecoEvent* ev);
  Bool_t Process();
  
  
  
private:
  Int_t fNhits;
  TTargetRecoEvent* fhitEvent;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

