// MMegaSD.hh
// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2024-03-19
//
// --------------------------------------------------------------

#ifndef MMegaSD_h
#define MMegaSD_h 1

#include "G4VSensitiveDetector.hh"
#include "MMegaHit.hh" 

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class MMegaSD : public G4VSensitiveDetector
{

public:

  MMegaSD(G4String);
  ~MMegaSD();

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  G4int ClassifyTrack(G4Track* track);
  void EndOfEvent(G4HCofThisEvent*);

private: 

  MMegaHitsCollection* MMegaCollection;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
