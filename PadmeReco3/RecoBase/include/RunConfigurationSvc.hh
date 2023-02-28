#ifndef RunConfigurationSvc_h
#define RunConfigurationSvc_h 1

#include <map>

#include <Rtypes.h>
#include <TString.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class RunConfigurationSvc
{

protected:

  RunConfigurationSvc();

public:

  ~RunConfigurationSvc();
  static RunConfigurationSvc* GetInstance();

private:

  static RunConfigurationSvc* fInstance;

public:

  Bool_t Initialize();
  Bool_t Finalize();

  void SetVerbose(Int_t v) { fVerbose = v; }

  void SetRecoInput(TString i) { fRecoInput = i; }
  TString GetRecoInput() { return fRecoInput; }

  void SetRecoMode    (Bool_t m) { fRecoMode     = m; }
  void SetCosmicsMode (Bool_t m) { fCosmicsMode  = m; }
  void SetPedestalMode(Bool_t m) { fPedestalMode = m; }
  void SetMonitorMode (Bool_t m) { fMonitorMode  = m; }

  Bool_t IsRecoMode()     { return fRecoMode;     }
  Bool_t IsCosmicsMode()  { return fCosmicsMode;  }
  Bool_t IsPedestalMode() { return fPedestalMode; }
  Bool_t IsMonitorMode()  { return fMonitorMode;  }

  void SetDebugMode(Int_t d) { fDebugMode = d; }
  Int_t GetDebugMode() { return fDebugMode; }

  // Use this to set all flags at once
  void SetRunningMode(Int_t,Int_t,Int_t,Int_t);

private:

  Int_t fVerbose;

  TString fRecoInput;

  Bool_t fRecoMode;
  Bool_t fCosmicsMode;
  Bool_t fPedestalMode;
  Bool_t fMonitorMode;

  Int_t fDebugMode;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
