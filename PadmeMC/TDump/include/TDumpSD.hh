#ifndef TDumpSD_h
#define TDumpSD_h 1

#include "G4VSensitiveDetector.hh"
#include "TDumpHit.hh"

class G4Step;
class G4HCofThisEvent;

class TDumpSD : public G4VSensitiveDetector
{
  public:

      TDumpSD(G4String);
     ~TDumpSD();

      void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*, G4TouchableHistory*);
      G4int ClassifyTrack(G4Track* track);
      void EndOfEvent(G4HCofThisEvent*);

  private:

      TDumpHitsCollection* fTDumpCollection;

};

#endif
