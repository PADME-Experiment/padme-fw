#ifndef TargetAnalysis_h
#define TargetAnalysis_h 1

#include "ValidationBase.hh"
#include <map>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TTargetRecoEvent;
class TTargetRecoBeam;

class TargetAnalysis : public ValidationBase
{

public:
  TargetAnalysis();
  TargetAnalysis(Int_t valid, Int_t verb);
  ~TargetAnalysis();

  Bool_t Init(TRecoEvent* EventHeader, TTargetRecoEvent* ev, TTargetRecoBeam* b);
  Bool_t InitHistosAnalysis();
  Bool_t InitHistosValidation();
  Bool_t InitHistosDataQuality();
  Bool_t ProcessAnalysis();
  Bool_t ProcessValidation();
  Bool_t ProcessDataQuality();
  Bool_t Finalize();
  
  Bool_t PassEvent();
  
private:
  TTargetRecoBeam* fRecoBeam;

  // my analysis variables
  Int_t nEvsTarget;
  Int_t nEvsGoodX;
  Int_t nEvsGoodY;
  Int_t nEvsGoodXY;
  Double_t nPOTgoodXY_job;
  Double_t nPOTErrgoodXY_job;
  Double_t nPOTbadXorY_job;
  Double_t nPOTErrbadXorY_job;

  std::map<Int_t, Double_t> fnPOTmean;
  std::map<Int_t, Double_t> fnPOTsigma;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

