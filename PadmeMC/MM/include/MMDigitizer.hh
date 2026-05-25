// MMDigitizer.hh
// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
//
// --------------------------------------------------------------

#ifndef MMDigitizer_h
#define MMDigitizer_h 1

#include "G4VDigitizerModule.hh"
#include "MMDigi.hh" 
#include "TF1.h" 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class MMDigitizer : public G4VDigitizerModule
{
public:
  MMDigitizer(G4String);
  ~MMDigitizer();
  

  void Digitize();

private: 
  static void InitializeAmplificationFluctuation();
  G4double GetAmplificationFluctuation();
  static TF1 *ampl_dist;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
