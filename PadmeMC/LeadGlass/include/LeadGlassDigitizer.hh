// LeadGlassDigitizer.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2023-09-21
// --------------------------------------------------------------

#ifndef LeadGlassDigitizer_h
#define LeadGlassDigitizer_h 1

#include "G4VDigitizerModule.hh"
#include "LeadGlassDigi.hh" 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class LeadGlassDigitizer : public G4VDigitizerModule
{
public:
  LeadGlassDigitizer(G4String);
  ~LeadGlassDigitizer();

  void Digitize();

private: 

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
