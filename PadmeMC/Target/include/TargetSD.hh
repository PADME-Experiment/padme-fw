#ifndef TargetSD_h
#define TargetSD_h 1

#include "G4VSensitiveDetector.hh"

#include "TargetHit.hh"

class G4Step;
class G4HCofThisEvent;

class TargetSD : public G4VSensitiveDetector
{
  public:
      TargetSD(G4String);
     ~TargetSD();

      void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*, G4TouchableHistory*);
      void EndOfEvent(G4HCofThisEvent*);

  private:
      TargetHitsCollection* fTargetCollection;
};

#endif
