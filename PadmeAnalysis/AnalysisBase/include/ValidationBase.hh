#ifndef ValidationBase_h
#define ValidationBase_h 1

#include "TObject.h"
#include "TString.h"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TRecoVObject;
class TRecoVClusCollection;

class ValidationBase : public TObject
{

public:
  ValidationBase();
  ~ValidationBase();
  //virtual Bool_t Init(TRecoVObject* ev, TRecoVClusCollection* cl, Int_t verb);
  //virtual Bool_t InitValidation();
  virtual Bool_t ProcessValidation(TString name);
  virtual Bool_t InitHistosValidation(TString name);
  virtual Bool_t Finalize(){return true;}
  

protected:
  TRecoVObject* fhitEvent;
  TRecoVClusCollection* fClColl;
  Int_t fVerbose;
  Int_t fValidation;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
