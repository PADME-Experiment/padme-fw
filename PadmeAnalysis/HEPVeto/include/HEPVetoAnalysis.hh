#ifndef HEPVetoAnalysis_h
#define HEPVetoAnalysis_h 1

#include "ValidationBase.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class THEPVetoRecoEvent;
class TRecoVClusCollection;

class HEPVetoAnalysis : public ValidationBase 
{

public:
  HEPVetoAnalysis();
  HEPVetoAnalysis(Int_t valid, Int_t verb);
  ~HEPVetoAnalysis();

  
  Bool_t Finalize(){return true;}
  Bool_t InitHistosAnalysis();
  Bool_t InitHistosDataQuality();
  Bool_t ProcessAnalysis();
  Bool_t ProcessDataQuality();
  
  
  
  
private:
  Double_t fTmax;
  Double_t fTmin;
  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

