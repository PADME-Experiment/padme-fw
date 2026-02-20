// MMegaDigitizer.hh
// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
//
// --------------------------------------------------------------

#ifndef MMegaDigitizer_h
#define MMegaDigitizer_h 1

#include "G4VDigitizerModule.hh"
#include "MMegaDigi.hh" 
#include "TF1.h" 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class MMegaDigitizer : public G4VDigitizerModule
{
public:
  MMegaDigitizer(G4String);
  ~MMegaDigitizer();
  

  void Digitize();

private: 
  static void InitializeAmplificationFluctuation();
  G4double GetAmplificationFluctuation();
  static TF1 *ampl_dist;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
