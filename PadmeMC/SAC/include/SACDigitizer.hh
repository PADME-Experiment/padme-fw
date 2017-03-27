// SACDigitizer.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2106-07-22
//
// --------------------------------------------------------------

#ifndef SACDigitizer_h
#define SACDigitizer_h 1

#include "G4VDigitizerModule.hh"
#include "SACDigi.hh" 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class SACDigitizer : public G4VDigitizerModule
{
public:
  SACDigitizer(G4String);
  ~SACDigitizer();

  void Digitize();

private: 

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
