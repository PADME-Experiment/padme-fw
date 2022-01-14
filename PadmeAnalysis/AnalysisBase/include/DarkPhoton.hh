#ifndef DarkPhoton_h
#define DarkPhoton_h 1

#include "TObject.h"
#include "TString.h"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TVector3;
class TRecoEvent;
class TRecoVObject;
class TRecoVClusCollection;
class TTargetRecoBeam;

class DarkPhoton : public TObject
{

public:
  DarkPhoton();
  DarkPhoton(Int_t processingMode, Int_t verbosityFlag);
  ~DarkPhoton();
  virtual Bool_t Init(TRecoEvent* eventHeader, 
		      TRecoVObject* ECALev, TRecoVClusCollection* ECALcl,
		      TRecoVObject* PVetoev, TRecoVClusCollection* PVetocl,
		      TRecoVObject* EVetoev, TRecoVClusCollection* EVetocl,
		      TRecoVObject* HEPVetoev, TRecoVClusCollection* HEPVetocl,
		      TRecoVObject* SACev, TRecoVClusCollection* SACcl,
		      TRecoVObject* TARGETev, TTargetRecoBeam* TargetBeam);
  virtual Bool_t Finalize(){return true;}

  virtual Bool_t InitHistos();
  virtual Bool_t ProcessDarkPhoton(Bool_t isTargetOut, Bool_t externalPass, Bool_t makeClSelection ,std::vector<int> selCl);
  Double_t CalculateMomentumPositron(Double_t z,Double_t x);
  Bool_t phiSymmetricalInECal(TVector3 P1, TVector3 P2,  double& distPhi);
   
  Bool_t passPreselection(Bool_t isTargetOut, Bool_t isMC, Bool_t externalPass);
  Double_t MissingMass(TVector3 V, Double_t E);

  Bool_t passAnnPreSelection(Double_t clE, Double_t clTheta);
  Double_t extractEff(Double_t R, Double_t phi);

  void checkAMassBelonging(Double_t MMiss, Bool_t (&boolMass)[7]);
  void checkAEnergyBelonging(Double_t ene, Bool_t (&boolene)[7]);
  void checkADeltaEnergyBelonging(Double_t ene, Double_t R, Bool_t boolene [7], Double_t (&dE)[7], Bool_t (&boolDene)[7]);
  void FillAMassRange(Bool_t MRange[7], Double_t var1, Double_t var2, std::string hPartialName);
  void FillAMassRangeSimple(Bool_t MRange[7], Double_t var1, std::string hPartialName);
  void FillAMassRangeSimple(Bool_t MRange[7], Double_t var1[7], std::string hPartialName);
  void checkBrem(Double_t E, Double_t t, Bool_t boolDEne[7],std::string hPartialName);

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

  
  // fVerbose = 0 (minimal printout),  = 1 (info mode),  = 2 (debug quality)...
  TString  fAlgoName;
  Int_t    fVerbose;
  Int_t    fProcessingMode;

  Bool_t fInitToComplete;
  Double_t fdistanceTarget;//from calchep g4hits studies
  Double_t fFRmin;
  Double_t fFRmid;
  Double_t fFRmax;
  Double_t fEBeam;

  Bool_t   fPreSelectionAnnihilation;

  Double_t fminArange[7];
  Double_t fmaxArange[7];

  Double_t fminE[7];
  Double_t fmaxE[7];

  Double_t fFitC[7]; //from expo fit on ProfileX UBoson EvsR
  Double_t fFitSlope[7];
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
