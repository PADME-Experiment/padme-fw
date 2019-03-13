#ifndef TargetAnalysis_h
#define TargetAnalysis_h 1

#include "TObject.h"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TTargetRecoEvent;
class TTargetRecoBeam;

class TargetAnalysis : public TObject
{

public:
  TargetAnalysis();
  ~TargetAnalysis();

  Bool_t Init(TTargetRecoEvent* ev, TTargetRecoBeam* b, Int_t v);
  Bool_t Process();
  Bool_t Finalize();
  
  
  
private:
  TTargetRecoEvent* fhitEvent;
  TTargetRecoBeam* fRecoBeam;

  Int_t fVerbose;


  // my analysis variables
  Int_t nEvsTarget;
  Int_t nEvsGoodX;
  Int_t nEvsGoodY;
  Int_t nEvsGoodXY;
  Double_t nPOTgoodXY_job;
  Double_t nPOTErrgoodXY_job;
  Double_t nPOTbadXorY_job;
  Double_t nPOTErrbadXorY_job;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

