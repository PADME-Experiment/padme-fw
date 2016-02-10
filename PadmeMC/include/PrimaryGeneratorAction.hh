#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

class DetectorConstruction;
class TargetGeometry;
//class MyEventGenerator;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:

  PrimaryGeneratorAction(DetectorConstruction*);    
  ~PrimaryGeneratorAction();
  
public:

  void GeneratePrimaries(G4Event*);

  void     SetUbosonMass(G4double m) { fUbosonMass = m;    }
  G4double GetUbosonMass()           { return fUbosonMass; }

private:

  int SetPositronBeamParameters();

  void CreateFinalStateUboson();
  void CreateFinalStateThreeGamma();

  //MyEventGenerator *evt;

  G4Event* fEvent;

  DetectorConstruction* fDetector;

  G4ParticleGun* fParticleGun;

  // Need access to target position
  TargetGeometry* fTargetGeometry;

  // Mass of the UBoson
  G4double fUbosonMass;

};
#endif
