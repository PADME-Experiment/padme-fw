#ifndef CalchepTruthStudies_ggPileup_h
#define CalchepTruthStudies_ggPileup_h 1

#include "TObject.h"
#include "TString.h"
#include "TVector3.h"
#include "TH1D.h"
#include "TFile.h"
#include <fstream>
#include "TRandom2.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TVector3;
class TRecoEvent;
class TRecoVObject;
class TRecoVClusCollection;


class CalchepTruthStudies_ggPileup : public TObject
{

public:
  CalchepTruthStudies_ggPileup();
  CalchepTruthStudies_ggPileup(Int_t processingMode, Int_t verbosityFlag );
  ~CalchepTruthStudies_ggPileup();
   Bool_t Init( Double_t DataEffInnerRRange_r1inFR[8], Double_t DataEffOuterRRange_r1inFR[8], Double_t DataEffInnerRRange_r1r2inFR[8], Double_t DataEffOuterRRange_r1r2inFR[8], Double_t MCEffInnerRRange_r1inFR[8], Double_t MCEffOuterRRange_r1inFR[8], Double_t MCEffInnerRRange_r1r2inFR[8], Double_t MCEffOuterRRange_r1r2inFR[8]);
  virtual Bool_t Finalize(){return true;}

  virtual Bool_t InitHistos();
  virtual Bool_t GetTruthEvent();
  void FillHisto();
  Bool_t GetReco(Double_t E1,Double_t phi1, Double_t R1,Double_t E2, Double_t phi2, Double_t R2);
  Bool_t ComparePhotons(Double_t phig1, Double_t e1T, Double_t phig2, Double_t e2T);

  void getCalchepTruth();
  void FillWeightedHisto_R1inFR_MC();
  void FillWeightedHisto_R1R2inFR_MC();
  Double_t ReturnEfficiencyR1inFR_MC(Double_t radius, Double_t phi);
  Double_t ReturnEfficiencyR1R2inFR_MC(Double_t radius, Double_t phi);
 
  void checkScaleFactorsMethod();
  Int_t checkIfItIsDataOrMC(Double_t phi1,Double_t phi2);
  void FillHistoData();
  void FillHistoMC();
  void FillWeightedHisto_R1inFR(std::string sufix);
  void FillWeightedHisto_R1R2inFR(std::string sufix);
  Double_t ReturnEfficiencyR1inFR(Double_t radius, Double_t phi);
  Double_t ReturnEfficiencyR1R2inFR(Double_t radius, Double_t phi);
  void FillWeightedHisto_R1inFR_DataMC(std::string sufix);
  void FillWeightedHisto_R1R2inFR_DataMC(std::string sufix);
  Double_t ReturnEfficiencyR1inFR_DataMC(Double_t radius, Double_t phi);
  Double_t ReturnEfficiencyR1R2inFR_DataMC(Double_t radius, Double_t phi);

protected:
  TRecoEvent*           fRecoEvent;
  TRecoVObject*         fECal_hitEvent   ;

  TRecoVClusCollection* fECal_ClColl      ;

  // fVerbose = 0 (minimal printout),  = 1 (info mode),  = 2 (debug quality)...
  TString  fAlgoName;
  Int_t    fVerbose;
  Int_t    fProcessingMode;

  ifstream fCalchep430;

private:
  Double_t fdistanceTarget;
  Double_t fFRmin;
  Double_t fFRmid;
  Double_t fFRmax;
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



  Double_t fg1E;
  Double_t fg2E;
  Double_t fg1Recal;
  Double_t fg2Recal;
  Double_t fg1phi;
  Double_t fg2phi;

  int sameEv1;
  int sameEv2;
  int sameEv3;


};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
