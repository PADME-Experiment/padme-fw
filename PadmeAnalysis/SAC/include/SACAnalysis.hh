#ifndef SACAnalysis_h
#define SACAnalysis_h 1

#include "ValidationBase.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TSACRecoEvent;
class TRecoVClusCollection;

class SACAnalysis : public ValidationBase
{

public:
  SACAnalysis();
  SACAnalysis(Int_t valid, Int_t verb);
  ~SACAnalysis();

  Bool_t InitHistosAnalysis();
  Bool_t InitHistosValidation();
  Bool_t ProcessAnalysis();
  Bool_t ProcessValidation();
  Bool_t Finalize(){return true;}
  //Once these are declared here they'll be called automatically by the main
  Bool_t InitHistosDataQuality();//hackathon 11/6
  Bool_t ProcessDataQuality();//hackathon 11/6
  
  
  
private:

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

