#include <iostream>

#include "RunConditionSvc.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunConditionSvc* RunConditionSvc::fInstance = 0;
RunConditionSvc* RunConditionSvc::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new RunConditionSvc(); }
  return fInstance;
}

RunConditionSvc::RunConditionSvc()
  :fVerbose(0)
{;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunConditionSvc::~RunConditionSvc()
{;}

Bool_t RunConditionSvc::Initialize()
{
  if (fVerbose) printf("RunConditionSvc::Initialize - Initializing Run Condition Service\n");
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Bool_t RunConditionSvc::Finalize()
{
  if (fVerbose) printf("RunConditionSvc::Finalize - Finalizing Run Condition Service\n");
  return true;
}
