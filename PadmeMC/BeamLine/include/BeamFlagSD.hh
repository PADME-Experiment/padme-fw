#ifndef BeamFlagSD_h
#define BeamFlagSD_h 1

#include "G4VSensitiveDetector.hh"

#include "BeamFlagHit.hh"

class G4Step;
class G4HCofThisEvent;

class BeamFlagSD : public G4VSensitiveDetector
{
  public:
      BeamFlagSD(G4String);
     ~BeamFlagSD();

      void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*, G4TouchableHistory*);
      void EndOfEvent(G4HCofThisEvent*);

  private:
      BeamFlagHitsCollection* fBeamFlagCollection;
};

#endif
