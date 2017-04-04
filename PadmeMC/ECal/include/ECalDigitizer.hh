// ECalDigitizer.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2106-07-21
// --------------------------------------------------------------

#ifndef ECalDigitizer_h
#define ECalDigitizer_h 1

#include "G4VDigitizerModule.hh"

class ECalHit;

class ECalDigitizer : public G4VDigitizerModule
{
public:
  ECalDigitizer(G4String);
  ~ECalDigitizer();

  void Digitize();

private: 

  // Compute number of photoelectrons produced by hit and time of first photoelectron
  void ComputeNpe(ECalHit*,G4double*,G4double*); // Hit, signal, time

  // Digitization parameters
  G4double fCrystalLength; // Length of BGO crystal along Z
  G4double fPropagationSpeed; // Speed of light in BGO
  G4double fEHtoNPEConversion; // Converts hit energy to #photoelectrons produced by photocathode
  G4double fPEtoSConversion; // Signal produced by 1 photoelectron
  // Collection probability map along Z
  G4int fCollMapNBins;
  G4double fCollMapBinLen;
  std::vector<G4double> fCollectionMap;

  // PMT parameters
  G4double fPMTTransitTime;
  G4double fPMTCableDelay;

  // Vectors to hold digi quantitites during digitization
  std::vector<G4int>    fDChannel;
  std::vector<G4double> fDTime;
  std::vector<G4double> fDEnergy;
  std::vector<G4double> fDSignal;

};

#endif
