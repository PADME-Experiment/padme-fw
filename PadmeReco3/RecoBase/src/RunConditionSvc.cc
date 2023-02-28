#include <iostream>
#include <fstream>      // std::ifstream
#include <sstream>      // std::istringstream

#include "TString.h"

#include "RunConditionSvc.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunConditionSvc* RunConditionSvc::fInstance = 0;
RunConditionSvc* RunConditionSvc::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new RunConditionSvc(); }
  return fInstance;
}

RunConditionSvc::RunConditionSvc()
  :fVerbose(0),fRunEnergyFile("RunEnergy.txt")
{;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunConditionSvc::~RunConditionSvc()
{;}

Bool_t RunConditionSvc::Initialize()
{
  if (fVerbose) printf("RunConditionSvc::Initialize - Initializing Run Condition Service\n");

  if (fVerbose) printf("RunConditionSvc::Initialize - Reading Run Energy file %s\n",fRunEnergyFile.Data());
  std::ifstream infile;
  std::string Line = "";
  infile.open(fRunEnergyFile.Data());
  if ( ! infile ) {
    std::cout << "RunConditionSvc::Initialize - ERROR - Condition file " << fRunEnergyFile << " not found" << std::endl;
    return false;
  }
  Int_t run;
  Double_t energy;
  while ( ! infile.eof() ) {
    getline(infile,Line);
    std::istringstream iss(Line);
    if ( iss >> run >> energy ) {
      if (fVerbose>2) printf("\tRun %d Energy %f MeV\n",run,energy);
      fRunEnergy[run] = energy;
    }
  }
  infile.close();

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Bool_t RunConditionSvc::Finalize()
{
  if (fVerbose) printf("RunConditionSvc::Finalize - Finalizing Run Condition Service\n");
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Bool_t RunConditionSvc::SetCurrentRun(Int_t run)
{
  if (fVerbose) printf("RunConditionSvc::SetCurrentRun - Setting conditions for run %d\n",run);
  fCurrentRun = run;
  fCurrentRunEnergy = GetRunEnergy(run);
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Double_t RunConditionSvc::GetRunEnergy(Int_t run)
{
  if ( fRunEnergy.count(run) ) {
    if (fVerbose) printf("RunConditionSvc::GetRunEnergy - Returning beam energy of %.3f MeV for run %d\n",fRunEnergy[run],run);
    return fRunEnergy[run];
  } else {
    if (fVerbose) printf("RunConditionSvc::GetRunEnergy - WARNING - No energy defined for run %d\n",run);
    return 0.;
  }
}
