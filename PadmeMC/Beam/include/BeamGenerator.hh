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

class G4ParticleGun;
class G4Event;

class BeamMessenger;
class DetectorConstruction;

class BeamGenerator
{
public:

  BeamGenerator(DetectorConstruction*);
  ~BeamGenerator();
  
public:

  void GenerateBeam(G4Event*);

  G4double GetUbosonMass() { return BeamParameters::GetInstance()->GetUbosonMass(); }

private:

  void GeneratePrimaryPositron(G4ParticleGun*);
  void CreateFinalStateUboson(G4ParticleGun*);
  void CreateFinalStateThreeGamma(G4ParticleGun*);

  G4Event* fEvent;

  DetectorConstruction* fDetector;

  BeamMessenger* fBeamMessenger;

};
#endif
