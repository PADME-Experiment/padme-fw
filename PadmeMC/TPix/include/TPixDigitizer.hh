// TPixDigitizer.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2017-03-15
// --------------------------------------------------------------

#ifndef TPixDigitizer_h
#define TPixDigitizer_h 1

#include "G4VDigitizerModule.hh"
#include "TPixDigi.hh" 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TPixDigitizer : public G4VDigitizerModule
{
public:
  TPixDigitizer(G4String);
  ~TPixDigitizer();

  void Digitize();

private: 

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
