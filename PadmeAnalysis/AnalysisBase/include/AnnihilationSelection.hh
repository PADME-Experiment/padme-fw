#ifndef AnnihilationSelection_h
#define AnnihilationSelection_h 1

#include "TObject.h"
#include "TString.h"
#include "TVector3.h"
#include "TH1D.h"
#include "TFile.h"
#include <fstream>
#include "TRandom2.h"
#include "AnnihilationSelectionDataMCMethod.hh"
#include "ScaleFactorMethod.hh"
#include "CalchepTruthStudies_ggPileup.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TVector3;
class TRecoEvent;
class TRecoVObject;
class TRecoVClusCollection;
class TTargetRecoBeam;


class AnnihilationSelection : public TObject
{

public:
  AnnihilationSelection();
  AnnihilationSelection(Int_t processingMode, Int_t verbosityFlag, Bool_t NoTargetBoolt, Bool_t dataMCmethod, Bool_t scaleFMethod, Bool_t externalPass);
  ~AnnihilationSelection();
  virtual Bool_t Init(TRecoEvent* eventHeader, 
		      TRecoVObject* ECALev, TRecoVClusCollection* ECALcl,
		      TRecoVObject* SACev, TRecoVClusCollection* SACcl,
		      TRecoVObject* TARGETev, TTargetRecoBeam* TargetBeam);
  virtual Bool_t Finalize(){return true;}

  virtual Bool_t InitHistos(Bool_t allAnnPlot);
  virtual Bool_t Process(Bool_t isMC,  Bool_t makeClSelection, std::vector<int> selCl, Bool_t allAnnPlot);
  void selectEffSample();

  void SortCluster_byEnergy();

  void FillGeneralHisto(std::string sufix);
  void FillRadiusRangeHisto(std::string sufix);
  void FillTimeRangeHisto(std::string sufix);
  void FillTimeRangeCoGHisto(std::string sufix);
  void CheckNumberHitsOnCh(Int_t chSeed1, Int_t chSeed2, Int_t& countHit1, Int_t& countHit2);
  Bool_t phiSymmetricalInECal(TVector3 P1, TVector3 P2,  double& distPhi);
  Bool_t passPreselection(Bool_t isTargetOut, Bool_t isMC);
  Double_t ReturnEfficiencyR1inFR(Double_t radius, Double_t phi, Int_t plusSys);
  Double_t ReturnEfficiencyR1R2inFR(Double_t radius, Double_t phi, Int_t plusSys);
  Double_t ReturnEfficiencyTruthR1inFR(Double_t radius, Double_t phi);
  Double_t ReturnEfficiencyTruthR1R2inFR(Double_t radius, Double_t phi);
  Double_t ReturnIpoteticalEventEfficiencyR1inFR(Double_t phi);
  Double_t ReturnEfficiencyRooFitEThr90R1inFR(Double_t radius, Double_t phi);
  Double_t ReturnEfficiencyRooFitR1inFR(Double_t radius, Double_t phi);
  void FillWeightedHisto_R1inFR(std::string sufix);
  void FillWeightedTruthHisto_R1inFR(std::string sufix);
  void FillWeightedHisto_R2inFR(std::string sufix);
  void FillWeightedHisto_R1R2inFR(std::string sufix);
  void FillWeightedHisto_R1R2inFR2approach(std::string sufix);
  void FillWeightedTruthHisto_R1R2inFR(std::string sufix);
  void FillWeightedHistoRooFit_R1inFR(std::string sufix);

  Double_t ReturnAccettanceEffective_g1g2FR(Double_t radius);
  Double_t ReturnAccettanceEffective_g1FR(Double_t radius);
  void FillWeightedAccEffHisto_R1inFR(std::string sufix);
  void FillAccEffHisto_R1inFR(std::string sufix);
  void FillWeightedAccEffHisto_R1R2inFR(std::string sufix);
  void FillAccEffHisto_R1R2inFR(std::string sufix);

  void fillEffVector(Bool_t isMC);
  void fillEffVectorData(std::string fnameTagFR, std::string fnameTagProbeFR);
  void fillEffVectorMC(std::string fnameTagFR, std::string fnameTagProbeFR);
  void fillEffVectorTruth();
  void fillEffVectorRooFit();

  void FillWeightedHistoAsAFunctionOfPhi(Double_t effScaleFactor,std::string sufix2);

  void CorrectEff(Double_t (&Inner)[8], Double_t (&Outer)[8], Double_t (&sysUpIn)[8],Double_t (&sysLowIn)[8], Double_t (&sysUpOut)[8],Double_t (&sysLowOut)[8]);
  void printCounters();

  Bool_t passAnnPreSelection(Double_t clE, Double_t clTheta);
  void FillCoGAsAFunctionOfPhi(std::string sufix);
 
protected:
  TRecoEvent*           fRecoEvent;
  TRecoVObject*         fECal_hitEvent   ;
  TRecoVObject*         fSAC_hitEvent    ;
  TRecoVObject*         fTarget_hitEvent ;

  TRecoVClusCollection* fECal_ClColl      ;
  TRecoVClusCollection* fSAC_ClColl       ;
  TTargetRecoBeam*      fTarget_RecoBeam  ;

  AnnihilationSelectionDataMCMethod* fDataMCMethod ;
  ScaleFactorMethod* fMCscaleF ;
  CalchepTruthStudies_ggPileup* fscaleFacCalchepPileup;


  // fVerbose = 0 (minimal printout),  = 1 (info mode),  = 2 (debug quality)...
  TString  fAlgoName;
  Int_t    fVerbose;
  Int_t    fProcessingMode;
  Bool_t   fNoTargetBool;
  Bool_t   fdataMCmethod;
  Bool_t   fScaleFMethod;
  Bool_t   fsFacCalchepPileup;
  Bool_t   fCorrectEfficiencyForTruth;
  Bool_t   fexternalPass;

  Bool_t fInitToComplete;  

private:
  TVector3 fpos1;
  TVector3 fpos2;
  Double_t fg1x    ;
  Double_t fg1y    ;
  Double_t fg1E    ;
  	   	  
  Double_t fg1t    ;
  Double_t fR_1    ;
  Double_t fg1Recal;
  Double_t fPx_1   ;
  Double_t fPy_1   ;
  Double_t fPz_1   ;
  Double_t fthetag1;
  Double_t fg2x        ;
  Double_t fg2y        ;
  Double_t fg2t        ;
  Double_t fg2Recal    ;
  Double_t fg2E        ;
  Double_t fXWeighted  ;
  Double_t fYWeighted  ;
  Double_t fCoG        ;
  Double_t fR_2        ;
  Double_t fPx_2       ;
  Double_t fPy_2       ;
  Double_t fPz_2       ;
  Double_t fangleBetweenTwoPhoton;
  Double_t fDeltaPhi             ;
  Double_t fthetag2              ;
  Double_t fphig1 ;

  Double_t fphig2 ;

  Double_t ftimerange1min;
  Double_t ftimerange1max;
  Double_t ftimerange2min;
  Double_t ftimerange2max;
  Double_t ftimerange3min;
  Double_t ftimerange3max;
  Double_t ftimerange4min;
  Double_t ftimerange4max;
  Double_t ftimerange5min;
  Double_t ftimerange5max;

  Double_t fdistanceTarget;
  Double_t fFRmin;
  Double_t fFRmid;
  Double_t fFRmax;

  Int_t fcountEvent;
  Int_t fcountDTime10;
  Int_t fcountDTime10g1InFR;
  Int_t fcountDTime10g1InFR5CoGX;
  Int_t fcountDTime10g1InFR5CoGXY;
  Int_t fcountDTime10g1InFR5CoGXYEgCut;
  Int_t fcountDTime10g1InFR5CoGXYEgCutESumCut;
  Int_t fcountDTime10g1InFR5CoGXYEgCutESumCutg2inFR;

  Bool_t fPreSelectionAnnihilation;


  Double_t fEffInnerRRange_r1inFR[8];	        
  Double_t fEffOuterRRange_r1inFR[8];	       
  Double_t fEffInnerRRange_r1r2inFR[8];	       
  Double_t fEffOuterRRange_r1r2inFR[8];

  Double_t ferrEffInnerRRange_r1inFR[8];	        
  Double_t ferrEffOuterRRange_r1inFR[8];	       
  Double_t ferrEffInnerRRange_r1r2inFR[8];	       
  Double_t ferrEffOuterRRange_r1r2inFR[8];	       
	                                       
  Double_t fEffInnerRRangeFromTruth_r1inFR[8]; 	
  Double_t fEffOuterRRangeFromTruth_r1inFR[8]; 	
  Double_t fEffInnerRRangeFromTruth_r1r2inFR[8];
  Double_t fEffOuterRRangeFromTruth_r1r2inFR[8];
  	                                       
  Double_t fEffUpperSysInnerRRange_r1inFR[8];  
  Double_t fEffUpperSysOuterRRange_r1inFR[8];  
  Double_t fEffUpperSysInnerRRange_r1r2inFR[8];
  Double_t fEffUpperSysOuterRRange_r1r2inFR[8];
	                                       
  Double_t fEffLowerSysInnerRRange_r1inFR[8];  
  Double_t fEffLowerSysOuterRRange_r1inFR[8];  
  Double_t fEffLowerSysInnerRRange_r1r2inFR[8];
  Double_t fEffLowerSysOuterRRange_r1r2inFR[8];


  Double_t fDataEffInnerRRange_r1inFR[8];
  Double_t fDataEffOuterRRange_r1inFR[8];
  Double_t fDataEffInnerRRange_r1r2inFR[8];
  Double_t fDataEffOuterRRange_r1r2inFR[8];

  Double_t ferrDataEffInnerRRange_r1inFR[8];
  Double_t ferrDataEffOuterRRange_r1inFR[8];
  Double_t ferrDataEffInnerRRange_r1r2inFR[8];
  Double_t ferrDataEffOuterRRange_r1r2inFR[8];
	    
  Double_t fDataEffInnerRRangeFromTruth_r1inFR[8];	
  Double_t fDataEffOuterRRangeFromTruth_r1inFR[8];	
  Double_t fDataEffInnerRRangeFromTruth_r1r2inFR[8];
  Double_t fDataEffOuterRRangeFromTruth_r1r2inFR[8];
  	    
  Double_t fDataEffUpperSysInnerRRange_r1inFR[8];
  Double_t fDataEffUpperSysOuterRRange_r1inFR[8];
  Double_t fDataEffUpperSysInnerRRange_r1r2inFR[8];
  Double_t fDataEffUpperSysOuterRRange_r1r2inFR[8];
	    
  Double_t fDataEffLowerSysInnerRRange_r1inFR[8];
  Double_t fDataEffLowerSysOuterRRange_r1inFR[8];
  Double_t fDataEffLowerSysInnerRRange_r1r2inFR[8];
  Double_t fDataEffLowerSysOuterRRange_r1r2inFR[8];


  Double_t fMCEffInnerRRange_r1inFR[8];
  Double_t fMCEffOuterRRange_r1inFR[8];
  Double_t fMCEffInnerRRange_r1r2inFR[8];
  Double_t fMCEffOuterRRange_r1r2inFR[8];

  Double_t ferrMCEffInnerRRange_r1inFR[8];
  Double_t ferrMCEffOuterRRange_r1inFR[8];
  Double_t ferrMCEffInnerRRange_r1r2inFR[8];
  Double_t ferrMCEffOuterRRange_r1r2inFR[8];
	    
  Double_t fMCEffInnerRRangeFromTruth_r1inFR[8];	
  Double_t fMCEffOuterRRangeFromTruth_r1inFR[8];	
  Double_t fMCEffInnerRRangeFromTruth_r1r2inFR[8];
  Double_t fMCEffOuterRRangeFromTruth_r1r2inFR[8];
  	    
  Double_t fMCEffUpperSysInnerRRange_r1inFR[8];
  Double_t fMCEffUpperSysOuterRRange_r1inFR[8];
  Double_t fMCEffUpperSysInnerRRange_r1r2inFR[8];
  Double_t fMCEffUpperSysOuterRRange_r1r2inFR[8];
	    
  Double_t fMCEffLowerSysInnerRRange_r1inFR[8];
  Double_t fMCEffLowerSysOuterRRange_r1inFR[8];
  Double_t fMCEffLowerSysInnerRRange_r1r2inFR[8];
  Double_t fMCEffLowerSysOuterRRange_r1r2inFR[8];
  

  Double_t fEffInnerRooFit_r1inFR[8];      
  Double_t fEffOuterRooFit_r1inFR[8];      
  Double_t fEffInnerRooFitEThr90_r1inFR[8];
  Double_t fEffOuterRooFitEThr90_r1inFR[8];


  TH1D *hAccEffFromCalchep_g1g2FR;
  TH1D *hAccEffFromCalchep_g1FR;

  std::vector<int> fIdCl_SortByEnergy;

  TRandom2 * fr;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
