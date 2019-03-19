#ifndef PVetoAnalysis_h
#define PVetoAnalysis_h 1

#include "ValidationBase.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TPVetoRecoEvent;
class TRecoVClusCollection;

class PVetoAnalysis : public ValidationBase
{

public:
  PVetoAnalysis();
  PVetoAnalysis(Int_t valid, Int_t verb);
  ~PVetoAnalysis();

  Bool_t Init(TPVetoRecoEvent* ev, TRecoVClusCollection* cl);
  Bool_t Process();
  Bool_t Finalize(){return true;}
  Bool_t InitHistos();
  
  
  
private:
  Double_t fTmax;
  Double_t fTmin;
  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

