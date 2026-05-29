// TMMDigitizer.hh
// --------------------------------------------------------------
// History:
//
// Created by Elisa Di Meco  (elisa.dimeco@lnf.infn.it) 2026-05-27
//
// --------------------------------------------------------------

#ifndef TMMDigitizer_h
#define TMMDigitizer_h 1

#include "G4VDigitizerModule.hh"
#include "TMMDigi.hh" 
#include "TF1.h" 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TMMDigitizer : public G4VDigitizerModule
{
public:
  TMMDigitizer(G4String);
  ~TMMDigitizer();
  

  void Digitize();

private: 
  static void InitializeAmplificationFluctuation();
  G4double GetAmplificationFluctuation();
  static TF1 *ampl_dist;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
