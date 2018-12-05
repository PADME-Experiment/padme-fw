#ifndef SACAnalysis_h
#define SACAnalysis_h 1

#include "TObject.h"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TSACRecoEvent;

class SACAnalysis : public TObject
{

public:
  SACAnalysis();
  ~SACAnalysis();

  Bool_t Init(Int_t nh, TSACRecoEvent* ev);
  Bool_t Process();
  
  
  
private:
  Int_t fNhits;
  TSACRecoEvent* fhitEvent;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

