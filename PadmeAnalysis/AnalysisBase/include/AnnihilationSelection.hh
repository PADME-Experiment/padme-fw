#ifndef AnnihilationSelection_h
#define AnnihilationSelection_h 1

#include "TObject.h"
#include "TString.h"
#include "TVector3.h"

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
  AnnihilationSelection(Int_t processingMode, Int_t verbosityFlag, Bool_t NoTargetBoolt );
  ~AnnihilationSelection();
  virtual Bool_t Init(TRecoEvent* eventHeader, 
		      TRecoVObject* ECALev, TRecoVClusCollection* ECALcl,
		      TRecoVObject* SACev, TRecoVClusCollection* SACcl,
		      TRecoVObject* TARGETev, TTargetRecoBeam* TargetBeam);
  virtual Bool_t Finalize(){return true;}

  virtual Bool_t InitHistos();
  virtual Bool_t Process(Bool_t isMC);

  void FillGeneralHisto(std::string sufix);
  void FillRadiusRangeHisto(std::string sufix);
  void FillTimeRangeHisto(std::string sufix);
  void CheckNumberHitsOnCh(Int_t chSeed1, Int_t chSeed2, Int_t& countHit1, Int_t& countHit2);
  Bool_t phiSymmetricalInECal(TVector3 P1, TVector3 P2,  double& distPhi);
  Bool_t passPreselection(Bool_t isTargetOut, Bool_t isMC);
  

protected:
  TRecoEvent*           fRecoEvent;
  TRecoVObject*         fECal_hitEvent   ;
  TRecoVObject*         fSAC_hitEvent    ;
  TRecoVObject*         fTarget_hitEvent ;

  TRecoVClusCollection* fECal_ClColl      ;
  TRecoVClusCollection* fSAC_ClColl       ;
  TTargetRecoBeam*      fTarget_RecoBeam  ;


  // fVerbose = 0 (minimal printout),  = 1 (info mode),  = 2 (debug quality)...
  TString  fAlgoName;
  Int_t    fVerbose;
  Int_t    fProcessingMode;
  Bool_t   fNoTargetBool;

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
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
