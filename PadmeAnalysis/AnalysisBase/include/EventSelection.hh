#ifndef EventSelection_h
#define EventSelection_h 1

#include "TObject.h"
#include "TString.h"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TVector3;
class TRecoEvent;
class TRecoVObject;
class TRecoVClusCollection;
class TTargetRecoBeam;


enum preSelCutNumbering
{
  ps_cut_all=0,
  ps_cut_trg=1,
  ps_cut_POT=2,
};
enum selCutNumbering
{
  cut_all=0,
  cut_Presel=1,
  cut_ge1ECalCl=2,
};
enum selPhotonCutNumbering
{
  cut_g_all=0,
  cut_g_Presel=1
};



class EventSelection : public TObject
{

public:
  EventSelection();
  EventSelection(Int_t processingMode, Int_t verbosityFlag);
  ~EventSelection();
  virtual Bool_t Init(TRecoEvent* eventHeader, 
		      TRecoVObject* ECALev, TRecoVClusCollection* ECALcl,
		      TRecoVObject* PVetoev, TRecoVClusCollection* PVetocl,
		      TRecoVObject* EVetoev, TRecoVClusCollection* EVetocl,
		      TRecoVObject* HEPVetoev, TRecoVClusCollection* HEPVetocl,
		      TRecoVObject* SACev, TRecoVClusCollection* SACcl,
		      TRecoVObject* TARGETev, TTargetRecoBeam* TargetBeam);
  virtual Bool_t Finalize(){return true;}

  virtual Bool_t InitHistos();
  virtual Bool_t InitHistosAnalysis();

  virtual Bool_t Process();
  virtual Bool_t ProcessAnalysis();
  virtual Bool_t ProcessAnalysisSS();
  virtual Bool_t ProcessAnalysisGC();

  Bool_t passPreselection();//{return true;}

  Bool_t Proc_HEPVeto_vs_PVeto(Bool_t isMC);
  Bool_t Proc_SACclu_vs_all(Bool_t isMC);
  Bool_t Proc_SAChit_vs_PVeto(Bool_t isMC);
  Bool_t Proc_ECalclu_vs_all(Bool_t isMC);
  Bool_t Proc_PVetoclu_vs_AllVetos(Bool_t isMC);
  Bool_t Proc_EVetoclu_vs_HEPVeto(Bool_t isMC);
  
  Bool_t phiSymmetricalInECal(int xChId, int aChId, double& distR, double& distPhi);
  Bool_t phiSymmetricalInECal(TVector3 P1, TVector3 P2, double& distR, double& distPhi, double& b);
  Double_t applyTimePVetoLinCorr(Double_t  xChId, Double_t xTime);
  
  void InitTimeCalPVeto  () {for(int i=0;i!=16;i++)fTimeOffsetPVeto[i]=0.;}
  void InitTimeCalEVeto  () {for(int i=0;i!=16;i++)fTimeOffsetEVeto[i]=0.;}
  void InitTimeCalHEPVeto() {for(int i=0;i!=32;i++)fTimeOffsetHEPVeto[i]=0.;}
  void InitTimeCalSAC    () {for(int i=0;i!=50;i++)fTimeOffsetSAC[i]=0.;}
  void InitTimeCalECal   () {for(int i=0;i!=3000;i++)fTimeOffsetECal[i]=0.;}
  void CalibrateTimeAndEnergy();
  void CalibrateTimePVeto  (Bool_t isMC);
  void CalibrateTimeEVeto  (Bool_t isMC);
  void CalibrateTimeHEPVeto(Bool_t isMC);
  void CalibrateTimeSAC    (Bool_t isMC);
  void CalibrateTimeECal   (Bool_t isMC);

  void SetCalibTimePVeto  (Bool_t isMC);
  void SetCalibTimeEVeto  (Bool_t isMC);
  void SetCalibTimeHEPVeto(Bool_t isMC);
  void SetCalibTimeSAC    (Bool_t isMC);
  void SetCalibTimeECal   (Bool_t isMC);

  void ApplyCalibTimePVeto  ();
  void ApplyCalibTimeEVeto  ();
  void ApplyCalibTimeHEPVeto();
  void ApplyCalibTimeSAC    ();
  void ApplyCalibTimeECal   ();
  void ApplyCalibTimeEnergyECal   (Bool_t isMC);
  

  Double_t InvariantDECal(TVector3 V1, TVector3 V2, Double_t E1, Double_t E2);
  Double_t InvariantMassECal(TVector3 V1, TVector3 V2, Double_t E1, Double_t E2);
  Double_t VetoMomentum(Double_t z);

  Bool_t Phi1Phi2ECal(TVector3 V1, TVector3 V2, double& Phi1, double& Phi2);
  Double_t CosDeltaThetaECal(TVector3 V1, TVector3 V2);
  Double_t MissingMass(TVector3 V, Double_t E);
  Double_t CosDeltaPhiECal(TVector3 V1, TVector3 V2);
  Double_t PhotonRadius(TVector3 V);

  void   SetVersion(Int_t fv);
  Int_t  GetVersion(){return fVersion;}

protected:
  TRecoEvent*           fRecoEvent;
  TRecoVObject*         fECal_hitEvent   ;
  TRecoVObject*         fPVeto_hitEvent  ;
  TRecoVObject*         fEVeto_hitEvent  ;
  TRecoVObject*         fSAC_hitEvent    ;
  TRecoVObject*         fTarget_hitEvent ;
  TRecoVObject*         fHEPVeto_hitEvent;

  TRecoVClusCollection* fECal_ClColl      ;
  TRecoVClusCollection* fPVeto_ClColl     ;
  TRecoVClusCollection* fEVeto_ClColl     ;
  TRecoVClusCollection* fSAC_ClColl       ;
  TTargetRecoBeam*      fTarget_RecoBeam  ;
  TRecoVClusCollection* fHEPVeto_ClColl   ;

  //version of analysis 1=SS, 2=GC
  Int_t fVersion;
  
  // fVerbose = 0 (minimal printout),  = 1 (info mode),  = 2 (debug quality)...
  TString  fAlgoName;
  Int_t    fVerbose;
  Int_t    fProcessingMode;

  Bool_t fInitToComplete;


  double fTimeOffsetPVeto[96];  
  double fTimeOffsetEVeto[96];
  double fTimeOffsetHEPVeto[32];
  double fTimeOffsetECal[3000];
  double fTimeOffsetSAC[50];
  double fEnergyGainSAC[50];
  

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
