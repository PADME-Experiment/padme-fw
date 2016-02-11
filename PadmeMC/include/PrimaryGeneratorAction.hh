#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

#include "BeamGenerator.hh"

class G4Event;

class DetectorConstruction;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:

  PrimaryGeneratorAction(DetectorConstruction*);    
  ~PrimaryGeneratorAction();
  
public:

  void GeneratePrimaries(G4Event*);

  G4double GetUbosonMass() { return fBeamGenerator->GetUbosonMass(); }

private:

  BeamGenerator* fBeamGenerator;

};
#endif
