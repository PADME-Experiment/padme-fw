// ETagDigitizer.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2106-07-20
// --------------------------------------------------------------

#ifndef ETagDigitizer_h
#define ETagDigitizer_h 1

#include "G4VDigitizerModule.hh"
#include "ETagDigi.hh" 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ETagDigitizer : public G4VDigitizerModule
{
public:
  ETagDigitizer(G4String);
  ~ETagDigitizer();

  void Digitize();

private: 

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
