#ifndef RunConditionSvc_h
#define RunConditionSvc_h 1

#include <map>

#include <Rtypes.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class RunConditionSvc
{

protected:
  RunConditionSvc();

public:

  ~RunConditionSvc();
  static RunConditionSvc* GetInstance();

private:

  static RunConditionSvc* fInstance;

public:

  Bool_t Initialize();
  Bool_t Finalize();

  void SetVerbose(Int_t v){ fVerbose = v; }

private:

  Int_t fVerbose;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
