// ECalSD.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#ifndef ECalSD_h
#define ECalSD_h 1

#include "G4VSensitiveDetector.hh"
#include "ECalHit.hh"

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ECalSD : public G4VSensitiveDetector
{

public:

  ECalSD(G4String);
  ~ECalSD();

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  G4int ClassifyTrack(G4Track* track);
  void EndOfEvent(G4HCofThisEvent*);

private:

  ECalHitsCollection* fECalCollection;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
