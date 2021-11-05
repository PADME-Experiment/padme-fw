#ifndef ECalAnalysis_h
#define ECalAnalysis_h 1

#include "TObject.h"
#include "ValidationBase.hh"
#include <map>
#include <vector>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TECalRecoEvent;
class TRecoVClusCollection;

class ECalAnalysis : public ValidationBase
{

public:
  ECalAnalysis();
  ECalAnalysis(Int_t  Validation, Int_t verb);
  ~ECalAnalysis();

  Bool_t ProcessAnalysis();
  Bool_t ProcessValidation();
  Bool_t InitHistosAnalysis();
  Bool_t InitHistosValidation();
  Bool_t Finalize(){return true;}
  void EnergyCalibration(Bool_t isMC, Bool_t SPAnalysis);

  void UploadClShift();
  void ShiftPositions(int RunN, Bool_t isMC);
  std::vector<int> passClCut();

private:

  Double_t fxECalSurvey;
  Double_t fyECalSurvey;

  std::map<Int_t, Double_t> fXBeamOnECalPlanePerRun;
  std::map<Int_t, Double_t> fYBeamOnECalPlanePerRun;
  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
