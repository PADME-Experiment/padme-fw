#ifndef SACAnalysis_h
#define SACAnalysis_h 1

#include "TObject.h"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TSACRecoEvent;
class TRecoVClusCollection;

class SACAnalysis : public TObject
{

public:
  SACAnalysis();
  ~SACAnalysis();

  Bool_t Init(Int_t nh, TSACRecoEvent* ev, TRecoVClusCollection* cl);
  Bool_t Process();
  
  
  
private:
  Int_t fNhits;
  TSACRecoEvent* fhitEvent;
  TRecoVClusCollection* fClColl;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

