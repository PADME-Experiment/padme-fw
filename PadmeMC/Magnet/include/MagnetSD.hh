#ifndef MagnetSD_h
#define MagnetSD_h 1

#include "G4VSensitiveDetector.hh"
#include "MagnetHit.hh" 

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class MagnetSD : public G4VSensitiveDetector
{
public:

  MagnetSD(G4String);
  ~MagnetSD();

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);

private:

  MagnetHitsCollection* fMagnetCollection;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
