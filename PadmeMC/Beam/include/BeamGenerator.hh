// BeamGenerator.hh
// --------------------------------------------------------------
// History:
//
// 2016-02-11 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#ifndef BeamGenerator_h
#define BeamGenerator_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

#include "BeamParameters.hh"

class G4Event;

class BeamMessenger;
class DetectorConstruction;

struct BeamPrimaryPositron
{
  G4double t;
  G4ThreeVector pos;
  G4double E;
  G4double m;
  G4double P;
  G4ThreeVector p;
  G4ThreeVector dir;
};

class BeamGenerator
{
public:

  BeamGenerator(DetectorConstruction*);
  ~BeamGenerator();
  
public:

  void GenerateBeam(G4Event*);

  G4double GetUbosonMass() { return BeamParameters::GetInstance()->GetUbosonMass(); }

private:

  void GeneratePrimaryPositron();
  void CreateFinalStateUboson();
  void CreateFinalStateThreeGamma();

  G4Event* fEvent;

  DetectorConstruction* fDetector;

  BeamMessenger* fBeamMessenger;

  BeamPrimaryPositron fPositron;

};
#endif
