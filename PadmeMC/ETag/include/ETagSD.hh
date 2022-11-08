// ETagSD.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#ifndef ETagSD_h
#define ETagSD_h 1

#include "G4VSensitiveDetector.hh"
#include "ETagHit.hh" 

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ETagSD : public G4VSensitiveDetector
{

public:

  ETagSD(G4String);
  ~ETagSD();

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  G4int ClassifyTrack(G4Track* track);
  void EndOfEvent(G4HCofThisEvent*);

private: 

  ETagHitsCollection* ETagCollection;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
