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
  SACAnalysis(Int_t valid, Int_t verb);//CT
  ~SACAnalysis();

  Bool_t InitHistosAnalysis();
  Bool_t InitHistosValidation();
  Bool_t ProcessAnalysis();
  Bool_t ProcessValidation();
  Bool_t InitHistosCosmics();//return true va cancellato!!!
  Bool_t ProcessCosmics();//CT
  Bool_t Finalize(){return true;}
  Bool_t InitHistosDataQuality();
  Bool_t ProcessDataQuality();
    
     

   
  
  
  
private:

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

