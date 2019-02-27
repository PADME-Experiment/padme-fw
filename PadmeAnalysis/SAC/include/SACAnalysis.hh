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

  Bool_t Init(TSACRecoEvent* ev, TRecoVClusCollection* cl, Int_t v);
  Bool_t Process();
  Bool_t Finalize(){return true;}

  
  
  
private:
  TSACRecoEvent* fhitEvent;
  TRecoVClusCollection* fClColl;

  Int_t fVerbose;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

