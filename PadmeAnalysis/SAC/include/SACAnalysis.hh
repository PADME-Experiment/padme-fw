#ifndef SACAnalysis_h
#define SACAnalysis_h 1

#include "ValidationBase.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TSACRecoEvent;
class TRecoVClusCollection;

class SACAnalysis : public ValidationBase
{

public:
  SACAnalysis();
  SACAnalysis(Int_t valid, Int_t verb);
  ~SACAnalysis();

  Bool_t InitHistosAnalysis();
  Bool_t InitHistosValidation();
  Bool_t ProcessAnalysis();
  Bool_t ProcessValidation();
  Bool_t Finalize(){return true;}
  
  Int_t GammaCloseInTime(Int_t i, Double_t timeGamma1);
  void RatioPhiMethod(Int_t IndexGamma1, Int_t indexGamma2,Double_t AngGamma1, Double_t AngGamma2, Double_t Distance2Gamma1_2, Double_t Radius2Gamma1);
  void AnnihilationPhiSimmetry(Int_t IndexGamma1, Int_t indexGamma2,Double_t AngGamma1, Double_t AngGamma2, Double_t Distance2Gamma1_2, Double_t Radius2Gamma1);
  void AnnihilationLeading(Int_t IndexGamma1, Int_t indexGamma2);
  void AnnihilationLeadingSubLeading(Int_t IndexGamma1, Int_t indexGamma2,Double_t TimeMaxEn, Double_t MaxSubLeadT, TString name);
  void AnnihilationGravitationalMethod(Int_t indexGamma1, Int_t indexGamma2);
  Int_t ReserchGamma2InDeltaPhi(Int_t indexGamma1);

  
  
  
private:
  Double_t fAnnhilationDeltaTime;
  Double_t fAnnhilationDeltaSpaceGravCenter;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

