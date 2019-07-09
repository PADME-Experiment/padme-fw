#ifndef ECalAnalysis_h
#define ECalAnalysis_h 1

#include "TObject.h"
#include "ValidationBase.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TECalRecoEvent;
class TRecoVClusCollection;

class ECalAnalysis : public ValidationBase
{

public:
  ECalAnalysis();
  ECalAnalysis(Int_t  Validation, Int_t verb);
  ~ECalAnalysis();

  Bool_t ProcessAnalysis();
  Bool_t ProcessValidation();
  Bool_t InitHistosAnalysis();
  Bool_t InitHistosValidation();
  Bool_t Finalize(){return true;}
  
  Int_t GammaCloseInTime(Int_t i, Double_t timeGamma1);
  void RatioPhiMethod(Int_t IndexGamma1, Int_t indexGamma2,Double_t AngGamma1, Double_t AngGamma2, Double_t Distance2Gamma1_2, Double_t Radius2Gamma1);
  void AnnihilationPhiSimmetry(Int_t IndexGamma1, Int_t indexGamma2,Double_t AngGamma1, Double_t AngGamma2, Double_t Distance2Gamma1_2, Double_t Radius2Gamma1);
  void AnnihilationLeading(Int_t IndexGamma1, Int_t indexGamma2);
  void AnnihilationLeadingSubLeading(Int_t IndexGamma1, Int_t indexGamma2, TString name);
  void AnnihilationGravitationalMethod(Int_t indexGamma1, Int_t indexGamma2);
  Int_t ReserchGamma2InDeltaPhi(Int_t indexGamma1);
  
  


private:
  Double_t fAnnhilationDeltaTime;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
