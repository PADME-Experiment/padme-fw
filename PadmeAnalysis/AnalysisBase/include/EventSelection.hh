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

  virtual Bool_t InitSelection(TRecoEvent* evHeader, TRecoVObject* targetev, TTargetRecoBeam* targetcl, TRecoVObject* ecalev, TRecoVClusCollection* ecalcl, TRecoVObject* pvetoev, TRecoVClusCollection* pvetocl, TRecoVObject* evetoev, TRecoVClusCollection* evetocl, TRecoVObject* hepvetoev, TRecoVClusCollection* hepvetocl,TRecoVObject* sacev, TRecoVClusCollection* saccl);
  

  virtual Bool_t InitHistosSelection();
  

  virtual Bool_t ProcessSelection();


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
