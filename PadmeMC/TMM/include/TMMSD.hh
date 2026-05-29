// TMMSD.hh
// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2024-03-19
//
// --------------------------------------------------------------

#ifndef TMMSD_h
#define TMMSD_h 1

#include "G4VSensitiveDetector.hh"
#include "TMMHit.hh" 

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TMMSD : public G4VSensitiveDetector
{

public:

  TMMSD(G4String);
  ~TMMSD();

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  G4int ClassifyTrack(G4Track* track);
  void EndOfEvent(G4HCofThisEvent*);

private: 

  TMMHitsCollection* TMMCollection;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
