#ifndef EventSelection_h
#define EventSelection_h 1

#include "TObject.h"
#include "TString.h"
#include "TRecoVObject.hh"
#include "TRecoEvent.hh"
#include "TRecoVClusCollection.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TRecoEvent;
class TRecoVObject;
class TRecoVClusCollection;

class EventSelection : public TObject
{

public:
  EventSelection();
  EventSelection(Int_t processingMode, Int_t verbosityFlag);
  ~EventSelection();

  Double_t applyTimePVetoLinCorr(Double_t  xChId, Double_t xTime);

  virtual Bool_t InitSelection(TRecoEvent* evHeader, TRecoVObject* targetev, TTargetRecoBeam* targetcl, TRecoVObject* ecalev, TRecoVClusCollection* ecalcl, TRecoVObject* pvetoev, TRecoVClusCollection* pvetocl, TRecoVObject* evetoev, TRecoVClusCollection* evetocl, TRecoVObject* hepvetoev, TRecoVClusCollection* hepvetocl,TRecoVObject* sacev, TRecoVClusCollection* saccl);
  

  virtual Bool_t InitHistosSelection();
  

  virtual Bool_t ProcessSelection();
  
  Bool_t phiSymmetricalInECal(TVector3 P1, TVector3 P2, double& distPhi);
  
  
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
  
  void HistoInDeltaPhi1_5ns(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoInDeltaPhi1_5nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoIn1cmCoG1_5ns(Double_t g1E, Double_t g2E, Double_t InvariantMass,Double_t XWeighted,Double_t YWeighted,Double_t angleBetweenTwoPhoton,Double_t phig1,Double_t phig2,Double_t thetag1,Double_t thetag2,Double_t g1x,Double_t g2x,Double_t g1y,Double_t g2y,Double_t CoG);
  void HistoIn1cmCoG1_5nsDeltaPhi(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoIn1cmCoG1_5nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass, Bool_t InDeltaPhi);
  void HistoIn1cmCoG1_5nsInFiducialRegion(Double_t g1E, Double_t g2E, Double_t InvariantMass, Double_t CoG);
  void HistoIn1cmCoG1_5nsInvariantMassRange(Double_t g1E, Double_t g2E,Double_t g1x, Double_t g2x, Double_t g1y, Double_t g2y, Double_t thetag1, Double_t thetag2, Double_t phig1, Double_t phig2);
  void HistoIn2cmCoG1_5ns(Double_t g1E, Double_t g2E, Double_t InvariantMass, Double_t angleBetweenTwoPhoton);
  void HistoIn2cmCoG1_5nsDeltaThetaDeltaPhi(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoIn2cmCoG1_5nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoIn3cmCoG1_5ns(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoIn3cmCoG1_5nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoIn3cmCoG1_5nsDeltaThetaDeltaPhi(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoIn5cmCoG1_5ns(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  
  void HistoInDeltaPhi20DegreeSecondMethod1_5ns(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoInDeltaPhi10DegreeSecondMethod1_5ns(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoInDeltaPhi5DegreeSecondMethod1_5ns(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoInDeltaPhi20DegreeSecondMethod1_5nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoInDeltaPhi10DegreeSecondMethod1_5nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass);
  void HistoInDeltaPhi5DegreeSecondMethod1_5nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass);


protected:
  TRecoEvent*           fRecoEvent;

  TRecoVObject*         fhitEventTarget;
  TTargetRecoBeam*      fClCollTarget;

  TRecoVObject*         fhitEventECal;
  TRecoVClusCollection* fClCollECal;

  TRecoVObject*         fhitEventPVeto;
  TRecoVClusCollection* fClCollPVeto;

  TRecoVObject*         fhitEventEVeto;
  TRecoVClusCollection* fClCollEVeto;

  TRecoVObject*         fhitEventHEPVeto;
  TRecoVClusCollection* fClCollHEPVeto;
 
  TRecoVObject*         fhitEventSAC;
  TRecoVClusCollection* fClCollSAC;
  

  // fVerbose = 0 (minimal printout),  = 1 (info mode),  = 2 (debug quality)...
  TString  fAlgoName;
  Int_t    fVerbose;
  // fProcessingMode = 0 (standard analysis), fProcessingMode = 1 (validation mode), fProcessingMode = 2 (data quality)... 
  Int_t fProcessingMode;

  

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
