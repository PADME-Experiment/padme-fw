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

  void SetRunEnergyFile(TString f) { fRunEnergyFile = f; }
  TString GetRunEnergyFile() { return fRunEnergyFile; }

  Bool_t SetCurrentRun(Int_t);
  Int_t GetCurrentRun() { return fCurrentRun; }

  Double_t GetRunEnergy(Int_t);
  Double_t GetRunEnergy() { return fCurrentRunEnergy; }

private:

  Int_t fVerbose;

  TString fRunEnergyFile;
  std::map<Int_t,Double_t> fRunEnergy;

  Int_t fCurrentRun;
  Double_t fCurrentRunEnergy;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
