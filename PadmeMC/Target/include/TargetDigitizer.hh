// TargetDigitizer.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2106-07-22
// --------------------------------------------------------------

#ifndef TargetDigitizer_h
#define TargetDigitizer_h 1

#include "G4VDigitizerModule.hh"
#include "TargetDigi.hh" 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TargetDigitizer : public G4VDigitizerModule
{
public:
  TargetDigitizer(G4String);
  ~TargetDigitizer();

  void Digitize();

private: 

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
