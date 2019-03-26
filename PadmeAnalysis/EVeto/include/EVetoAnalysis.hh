#ifndef EVetoAnalysis_h
#define EVetoAnalysis_h 1

#include "ValidationBase.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TEVetoRecoEvent;
class TRecoVClusCollection;

class EVetoAnalysis : public ValidationBase
{

public:
  EVetoAnalysis();
  EVetoAnalysis(Int_t  Validation, Int_t Verbose);
  ~EVetoAnalysis();

  Bool_t Init(TEVetoRecoEvent* ev, TRecoVClusCollection* cl);
  Bool_t Process();
  Bool_t Finalize(){return true;}
  Bool_t InitHistos();
  
  
  
private:
  Double_t fTmax;
  Double_t fTmin;
  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

