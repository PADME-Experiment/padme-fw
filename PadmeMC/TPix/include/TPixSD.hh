#ifndef TPixSD_h
#define TPixSD_h 1

#include "G4VSensitiveDetector.hh"

#include "TPixHit.hh" 

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TPixSD : public G4VSensitiveDetector
{
  public:
      TPixSD(G4String);
     ~TPixSD();

      void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*, G4TouchableHistory*);
      void EndOfEvent(G4HCofThisEvent*);

  private: 
      TPixHitsCollection* fTPixCollection;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
