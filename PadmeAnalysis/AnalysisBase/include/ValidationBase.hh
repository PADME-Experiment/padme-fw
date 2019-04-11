#ifndef ValidationBase_h
#define ValidationBase_h 1

#include "TObject.h"
#include "TString.h"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TRecoEvent;
class TRecoVObject;
class TRecoVClusCollection;

class ValidationBase : public TObject
{

public:
  ValidationBase();
  ValidationBase(Int_t processingMode, Int_t verbosityFlag);
  ~ValidationBase();
  virtual Bool_t Init(TRecoEvent* eventHeader, TRecoVObject* ev, TRecoVClusCollection* cl);
  virtual Bool_t Finalize(){return true;}

  virtual Bool_t InitHistos();
  virtual Bool_t InitHistosAnalysis(){return true;}
  virtual Bool_t InitHistosValidation();
  virtual Bool_t InitHistosValidation(TString name);
  virtual Bool_t InitHistosDataQuality(){return true;}

  virtual Bool_t Process();
  virtual Bool_t ProcessAnalysis(){return true;}
  virtual Bool_t ProcessValidation(){return true;}
  virtual Bool_t ProcessValidation(TString name);
  virtual Bool_t ProcessDataQuality(){return true;}

protected:
  TRecoEvent*           fRecoEvent;
  TRecoVObject*         fhitEvent;
  TRecoVClusCollection* fClColl;

  // fVerbose = 0 (minimal printout),  = 1 (info mode),  = 2 (debug quality)...
  TString  fAlgoName;
  Int_t    fVerbose;
  // fProcessingMode = 0 (standard analysis), fProcessingMode = 1 (validation mode), fProcessingMode = 2 (data quality)... 
  Int_t fProcessingMode;

  

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
