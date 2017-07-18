// SACDigitizer.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2106-07-22
//
// --------------------------------------------------------------

#ifndef SACDigitizer_h
#define SACDigitizer_h 1

#include "G4VDigitizerModule.hh"

class SACHit;

class SACDigitizer : public G4VDigitizerModule
{
public:
  SACDigitizer(G4String);
  ~SACDigitizer();

  void Digitize();

private: 

  // Compute number of photoelectrons produced by hit and time of first photoelectron
  void ComputeNpe(SACHit*,G4double*,G4double*); // Hit, signal, time

  // Digitization parameters
  G4double fCrystalLength; // Length of SAC crystal along Z
  G4double fPropagationSpeed; // Speed of light in SAC crystal
  G4double fEHtoNPEConversion; // Converts hit energy to #photoelectrons produced by photocathode
  G4double fPEtoSConversion; // Signal produced by 1 photoelectron
  // Collection probability map along Z
  G4int fCollMapNBins;
  G4double fCollMapBinLen;
  std::vector<G4double> fCollectionMap;

  // PMT parameters
  G4double fPMTTransitTime;
  G4double fPMTCableDelay;

  G4double fMinimumSignalTime; // Minimum time for SAC signal output

};

#endif
