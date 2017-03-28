// ECalDigitizer.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2106-07-21
// --------------------------------------------------------------

#ifndef ECalDigitizer_h
#define ECalDigitizer_h 1

#include "G4VDigitizerModule.hh"
//#include "ECalDigi.hh" 

class ECalHit;

class ECalDigitizer : public G4VDigitizerModule
{
public:
  ECalDigitizer(G4String);
  ~ECalDigitizer();

  void Digitize();

private: 

  // Compute time and signal contribution of an hit
  void ComputeSignal(ECalHit*,G4double*,G4double*); // Hit, signal, time

  // Digitization parameters
  G4double fCrystalLength;
  G4double fPropagationSpeed;
  G4double fConversionFactor;
  G4int fCollMapNBins;
  G4double fCollMapBinLen;
  std::vector<G4double> fCollectionMap;

  // Vectors to hold digi quantitites during digitization
  std::vector<G4int>    fDChannel;
  std::vector<G4double> fDTime;
  std::vector<G4double> fDEnergy;
  std::vector<G4double> fDSignal;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
