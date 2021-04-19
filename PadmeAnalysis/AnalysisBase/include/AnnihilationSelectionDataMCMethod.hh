#ifndef AnnihilationSelectionDataMCMethod_h
#define AnnihilationSelectionDataMCMethod_h 1

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


class AnnihilationSelectionDataMCMethod : public TObject
{

public:
  AnnihilationSelectionDataMCMethod();
  ~AnnihilationSelectionDataMCMethod();
  Bool_t Init( Double_t EffInnerRRange_r1inFR[8], Double_t EffOuterRRange_r1inFR[8], Double_t EffInnerRRange_r1r2inFR[8], Double_t EffOuterRRange_r1r2inFR[8]);
  virtual Bool_t InitHistos();
  virtual Bool_t Process(Double_t E1,Double_t phi1, Double_t R1,Double_t E2, Double_t phi2, Double_t R2);
  Int_t checkIfItIsDataOrMC(Double_t phi1, Double_t phi2);
  void FillGeneralHisto(std::string sufix);
  void FillHistoData();
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
  Double_t fEffInnerRRange_r1inFR[8];
  Double_t fEffOuterRRange_r1inFR[8];
  Double_t fEffInnerRRange_r1r2inFR[8];
  Double_t fEffOuterRRange_r1r2inFR[8];

  Int_t fdata;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
