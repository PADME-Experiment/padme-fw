// LeadGlassSD.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2023-09-21
// --------------------------------------------------------------

#ifndef LeadGlassSD_h
#define LeadGlassSD_h 1

#include "G4VSensitiveDetector.hh"
#include "LeadGlassHit.hh" 

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class LeadGlassSD : public G4VSensitiveDetector
{

public:

  LeadGlassSD(G4String);
  ~LeadGlassSD();

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);

private: 

  LeadGlassHitsCollection* LeadGlassCollection;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
