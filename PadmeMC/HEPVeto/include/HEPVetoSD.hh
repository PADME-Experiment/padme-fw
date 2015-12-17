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
      HEPVetoHitsCollection* HEPVetoCollection;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
