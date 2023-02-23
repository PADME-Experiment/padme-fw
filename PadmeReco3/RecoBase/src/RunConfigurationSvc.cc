#include <iostream>

#include "RunConfigurationSvc.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunConfigurationSvc* RunConfigurationSvc::fInstance = 0;
RunConfigurationSvc* RunConfigurationSvc::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new RunConfigurationSvc(); }
  return fInstance;
}

RunConfigurationSvc::RunConfigurationSvc()
  :fVerbose(0),fRecoMode(true),fCosmicsMode(false),fPedestalMode(false),fMonitorMode(false),fDebugMode(0)
{;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunConfigurationSvc::~RunConfigurationSvc()
{;}

Bool_t RunConfigurationSvc::Initialize()
{
  if (fVerbose) printf("RunConfigurationSvc::Initialize - Initializing Run Configuration Service\n");
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Bool_t RunConfigurationSvc::Finalize()
{
  if (fVerbose) printf("RunConfigurationSvc::Finalize - Finalizing Run Configuration Service\n");
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunConfigurationSvc::SetRunningMode(Int_t reco, Int_t cosmics, Int_t pedestal, Int_t monitor)
{
  if (reco)     { SetRecoMode(true);     } else { SetRecoMode(false);     }
  if (cosmics)  { SetCosmicsMode(true);  } else { SetCosmicsMode(false);  }
  if (pedestal) { SetPedestalMode(true); } else { SetPedestalMode(false); }
  if (monitor)  { SetMonitorMode(true);  } else { SetMonitorMode(false);  }
}
