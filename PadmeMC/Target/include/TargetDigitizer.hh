// TargetDigitizer.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2106-07-22
// --------------------------------------------------------------

#ifndef TargetDigitizer_h
#define TargetDigitizer_h 1

#include "G4VDigitizerModule.hh"
#include "TargetDigi.hh" 

#include "TargetMessenger.hh"
#include "TargetGeometry.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TargetDigitizer : public G4VDigitizerModule
{
public:
  TargetDigitizer(G4String);
  ~TargetDigitizer();

  void Digitize();

private: 

  TargetMessenger* fTargetMessenger;
  TargetGeometry* fTargetGeometry;

  G4bool fFastDigi;
  G4bool fSaveWaveforms;
  G4bool fReduceWaveforms;

  G4int PosToChannel(G4double pos);

  std::vector<G4double> NsRebin (std::vector<G4double>& traceI);
  std::vector<G4double> RCFilter (std::vector<G4double>& traceI);
  std::vector<G4double> CSAFilter (std::vector<G4double>& traceI);
  std::vector<G4double> AddNoiseAndBaseline (std::vector<G4double>& traceI);
  std::vector<G4double> fCSAKernel; // MR 26/11/2026 CSA response vs bin distance, computed once

  G4double stepfunCCD(G4double& ct, G4int& neh, G4int& cn, G4double& cd, G4double& os, G4int& cb);

  G4String digiCollectionName;
  // G4int nChannels; 
  // G4double fThreshold;
  // G4double nTrackDiv; 

  // MR 27/11/2026 removed the per-event waveform histograms dTraceHCCD/NHCCD/VHCCD[32]
  // (hardcoded 32 channels): the charges are now computed on the vectors, see Digitize()
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
