// EVetoDigitizer.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2106-07-22
// --------------------------------------------------------------

#ifndef EVetoDigitizer_h
#define EVetoDigitizer_h 1

#include "G4VDigitizerModule.hh"
#include "EVetoDigi.hh" 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EVetoDigitizer : public G4VDigitizerModule
{
public:
  EVetoDigitizer(G4String);
  ~EVetoDigitizer();

  void Digitize();

private: 

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
