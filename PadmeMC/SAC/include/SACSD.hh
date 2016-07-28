#ifndef SACSD_h
#define SACSD_h 1

#include "G4VSensitiveDetector.hh"
#include "SACHit.hh"

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class SACSD : public G4VSensitiveDetector
{
  public:
      SACSD(G4String);
     ~SACSD();

      void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*, G4TouchableHistory*);
      G4int ClassifyTrack(G4Track* track);
      void EndOfEvent(G4HCofThisEvent*);

  private:
      SACHitsCollection* fSACCollection;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
