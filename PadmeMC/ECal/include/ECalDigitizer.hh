// ECalDigitizer.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2106-07-21
// --------------------------------------------------------------

#ifndef ECalDigitizer_h
#define ECalDigitizer_h 1

#include "G4VDigitizerModule.hh"
#include "ECalDigi.hh" 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ECalDigitizer : public G4VDigitizerModule
{
public:
  ECalDigitizer(G4String);
  ~ECalDigitizer();

  void Digitize();

private: 

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
