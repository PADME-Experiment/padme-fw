// HEPVetoSD.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#ifndef HEPVetoSD_h
#define HEPVetoSD_h 1

#include "G4VSensitiveDetector.hh"
#include "HEPVetoHit.hh" 

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HEPVetoSD : public G4VSensitiveDetector
{

public:

  HEPVetoSD(G4String);
  ~HEPVetoSD();

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);

private: 

  HEPVetoHitsCollection* fHEPVetoCollection;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
