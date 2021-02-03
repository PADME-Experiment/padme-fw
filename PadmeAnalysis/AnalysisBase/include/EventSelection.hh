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
  virtual Bool_t ProcessAnalysisIO();
  virtual Bool_t InitHistosIO();
  //virtual Bool_t ProcessAnalysisGC();

  Bool_t passPreselection();//{return true;}
/*
  Bool_t Proc_HEPVeto_vs_PVeto(Bool_t isMC);
  Bool_t Proc_SACclu_vs_all(Bool_t isMC);
  Bool_t Proc_SAChit_vs_PVeto(Bool_t isMC);
  Bool_t Proc_ECalclu_vs_all(Bool_t isMC);
  Bool_t Proc_PVetoclu_vs_AllVetos(Bool_t isMC);
  Bool_t Proc_EVetoclu_vs_HEPVeto(Bool_t isMC);
*/
  Bool_t phiSymmetricalInECal(int xChId, int aChId, double& distR, double& distPhi);
  Bool_t phiSymmetricalInECal(TVector3 P1, TVector3 P2, double& distPhi);
  Bool_t phiSymmetricalInECal(TVector3 P1, TVector3 P2, double& distR, double& distPhi, double& b);
  Double_t applyTimePVetoLinCorr(Double_t  xChId, Double_t xTime);
  
  void InitTimeCalPVeto  () {for(int i=0;i!=16;i++)fTimeOffsetPVeto[i]=0.;}
  void InitTimeCalEVeto  () {for(int i=0;i!=16;i++)fTimeOffsetEVeto[i]=0.;}
  void InitTimeCalHEPVeto() {for(int i=0;i!=32;i++)fTimeOffsetHEPVeto[i]=0.;}
  void InitTimeCalSAC    () {for(int i=0;i!=50;i++)fTimeOffsetSAC[i]=0.;}
  void InitTimeCalECal   () {for(int i=0;i!=3000;i++)fTimeOffsetECal[i]=0.;}
  void CalibrateTimeAndEnergy();
  /*void CalibrateTimePVeto  (Bool_t isMC);
  void CalibrateTimeEVeto  (Bool_t isMC);
  void CalibrateTimeHEPVeto(Bool_t isMC);
  void CalibrateTimeSAC    (Bool_t isMC);
  void CalibrateTimeECal   (Bool_t isMC);*/

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

  void HistoInDeltaPhi3ns(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoInDeltaPhi3nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoIn1cmCoG3ns(Double_t g1E, Double_t g2E, Double_t InvariantMass,Double_t XWeighted,Double_t YWeighted,Double_t angleBetweenTwoPhoton,Double_t phig1,Double_t phig2,Double_t thetag1,Double_t thetag2,Double_t g1x,Double_t g2x,Double_t g1y,Double_t g2y, Double_t CoG);
  void HistoIn1cmCoG3nsDeltaPhi(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoIn1cmCoG3nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass, Bool_t InDeltaPhi);
  void HistoIn1cmCoG3nsInFiducialRegion(Double_t g1E, Double_t g2E, Double_t InvariantMass, Double_t CoG);
  void HistoIn1cmCoG3nsInvariantMassRange(Double_t g1E, Double_t g2E,Double_t g1x, Double_t g2x, Double_t g1y, Double_t g2y, Double_t thetag1, Double_t thetag2, Double_t phig1, Double_t phig2);
  void HistoIn2cmCoG3ns(Double_t g1E, Double_t g2E, Double_t InvariantMass, Double_t angleBetweenTwoPhoton);
  void HistoIn2cmCoG3nsDeltaThetaDeltaPhi(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoIn2cmCoG3nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoIn3cmCoG3ns(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoIn3cmCoG3nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoIn3cmCoG3nsDeltaThetaDeltaPhi(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoIn5cmCoG3ns(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  
  void HistoInDeltaPhi20DegreeSecondMethod3ns(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoInDeltaPhi10DegreeSecondMethod3ns(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoInDeltaPhi10DegreeSecondMethod3nsThrEnergy(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoInDeltaPhi5DegreeSecondMethod3ns(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoInDeltaPhi20DegreeSecondMethod3nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoInDeltaPhi10DegreeSecondMethod3nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoInDeltaPhi5DegreeSecondMethod3nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  
  void HistoDeltaTheta_InDeltaPhi20Interval(Double_t angleBetweenTwoPhoton);
  void HistoDeltaPhi_InDeltaThetaInterval(Double_t dphi);

  Double_t CalculateMomentumPositron(Double_t z,Double_t x);
  Bool_t InGoodRegionECal(Int_t x, Int_t y);
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
