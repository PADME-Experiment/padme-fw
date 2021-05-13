#ifndef ScaleFactorMethod_h
#define ScaleFactorMethod_h 1

#include "TObject.h"
#include "TString.h"
#include "TVector3.h"
#include "TH1D.h"
#include "TFile.h"
#include <fstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TVector3;
class TRecoEvent;
class TRecoVObject;
class TRecoVClusCollection;
class TTargetRecoBeam;


class ScaleFactorMethod : public TObject
{

public:
  ScaleFactorMethod();
  ~ScaleFactorMethod();
  Bool_t Init( Double_t DataEffInnerRRange_r1inFR[8], Double_t DataEffOuterRRange_r1inFR[8], Double_t DataEffInnerRRange_r1r2inFR[8], Double_t DataEffOuterRRange_r1r2inFR[8], Double_t MCEffInnerRRange_r1inFR[8], Double_t MCEffOuterRRange_r1inFR[8], Double_t MCEffInnerRRange_r1r2inFR[8], Double_t MCEffOuterRRange_r1r2inFR[8]);
  virtual Bool_t InitHistos();
  virtual Bool_t Process(Double_t E1,Double_t phi1, Double_t R1,Double_t E2, Double_t phi2, Double_t R2);
  void FillGeneralHisto(std::string sufix);
  void FillHistoMC();

  Double_t ReturnEfficiencyR1inFR(Double_t radius, Double_t phi);
  Double_t ReturnEfficiencyR1R2inFR(Double_t radius, Double_t phi);
  Double_t ReturnEfficiencyR1inFR_MC(Double_t radius, Double_t phi);
  Double_t ReturnEfficiencyR1R2inFR_MC(Double_t radius, Double_t phi);
  void FillWeightedHisto_R1inFR(std::string sufix);
  void FillWeightedHisto_R1R2inFR(std::string sufix);
  void FillWeightedHisto_R1inFR_MC(std::string sufix);
  void FillWeightedHisto_R1R2inFR_MC(std::string sufix);


private:

  Double_t fE1;	 
  Double_t fR1;	 
  Double_t fphi1;
  Double_t fE2;	 
  Double_t fR2;	 
  Double_t fphi2;
  Double_t fDataEffInnerRRange_r1inFR[8];
  Double_t fDataEffOuterRRange_r1inFR[8];
  Double_t fDataEffInnerRRange_r1r2inFR[8];
  Double_t fDataEffOuterRRange_r1r2inFR[8];
  Double_t fMCEffInnerRRange_r1inFR[8];
  Double_t fMCEffOuterRRange_r1inFR[8];
  Double_t fMCEffInnerRRange_r1r2inFR[8];
  Double_t fMCEffOuterRRange_r1r2inFR[8];

  Double_t fdistanceTarget; // from calchep g4 hits studies
  Double_t fFRmin;
  Double_t fFRmid;
  Double_t fFRmax;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
