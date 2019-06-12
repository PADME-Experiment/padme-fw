#ifndef BeWSD_h
#define BeWSD_h 1

#include "G4VSensitiveDetector.hh"

#include "BeWHit.hh"

class G4Step;
class G4HCofThisEvent;

class BeWSD : public G4VSensitiveDetector
{
  public:
      BeWSD(G4String);
     ~BeWSD();

      void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*, G4TouchableHistory*);
      void EndOfEvent(G4HCofThisEvent*);

  private:
      BeWHitsCollection* fBeWCollection;
};

#endif
