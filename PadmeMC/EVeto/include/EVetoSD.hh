#ifndef EVetoSD_h
#define EVetoSD_h 1

#include "G4VSensitiveDetector.hh"
#include "EVetoHit.hh" 

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EVetoSD : public G4VSensitiveDetector
{
  public:
      EVetoSD(G4String);
     ~EVetoSD();

      void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*, G4TouchableHistory*);
      void EndOfEvent(G4HCofThisEvent*);

  private: 
      EVetoHitsCollection* EVetoCollection;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
