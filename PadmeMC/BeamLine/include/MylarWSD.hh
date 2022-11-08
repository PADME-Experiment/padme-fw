#ifndef MylarWSD_h
#define MylarWSD_h 1

#include "G4VSensitiveDetector.hh"

#include "MylarWHit.hh"

class G4Step;
class G4HCofThisEvent;

class MylarWSD : public G4VSensitiveDetector
{
  public:
      MylarWSD(G4String);
     ~MylarWSD();

      void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*, G4TouchableHistory*);
      void EndOfEvent(G4HCofThisEvent*);

  private:
      MylarWHitsCollection* fMylarWCollection;
};

#endif
