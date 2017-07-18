// HEPVetoDigitizer.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2106-07-22
// --------------------------------------------------------------

#ifndef HEPVetoDigitizer_h
#define HEPVetoDigitizer_h 1

#include "G4VDigitizerModule.hh"
#include "HEPVetoDigi.hh" 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HEPVetoDigitizer : public G4VDigitizerModule
{
public:
  HEPVetoDigitizer(G4String);
  ~HEPVetoDigitizer();

  void Digitize();

private: 

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
