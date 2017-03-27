// PVetoDigitizer.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2106-07-20
// --------------------------------------------------------------

#ifndef PVetoDigitizer_h
#define PVetoDigitizer_h 1

#include "G4VDigitizerModule.hh"
#include "PVetoDigi.hh" 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PVetoDigitizer : public G4VDigitizerModule
{
public:
  PVetoDigitizer(G4String);
  ~PVetoDigitizer();

  void Digitize();

private: 

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
