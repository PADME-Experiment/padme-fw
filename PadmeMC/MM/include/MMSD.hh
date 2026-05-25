// MMSD.hh
// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2024-03-19
//
// --------------------------------------------------------------

#ifndef MMSD_h
#define MMSD_h 1

#include "G4VSensitiveDetector.hh"
#include "MMHit.hh" 

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class MMSD : public G4VSensitiveDetector
{

public:

  MMSD(G4String);
  ~MMSD();

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  G4int ClassifyTrack(G4Track* track);
  void EndOfEvent(G4HCofThisEvent*);

private: 

  MMHitsCollection* MMCollection;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
