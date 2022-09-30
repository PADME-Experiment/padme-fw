// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2021-03-24
//
// --------------------------------------------------------------

#ifndef MCTruthVertex_h
#define MCTruthVertex_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"

#include "MCTruthParticle.hh"
#include <vector>

class MCTruthVertex
{
public:

  MCTruthVertex();
  MCTruthVertex(G4String,G4ThreeVector,G4double);
  ~MCTruthVertex();

public:

  void SetProcess(G4String t) { fProcess = t; }
  G4String GetProcess() { return fProcess; }

  void SetPosition(G4ThreeVector p) { fPosition = p; }
  G4ThreeVector GetPosition() { return fPosition; }
  G4double GetPosX() { return fPosition.x(); };
  G4double GetPosY() { return fPosition.y(); };
  G4double GetPosZ() { return fPosition.z(); };

  void SetTime(G4double t) { fTime = t; };
  G4double GetTime() { return fTime; };

  G4int GetNParticlesIn() { return fParticleIn.size(); }
  MCTruthParticle* AddParticleIn(G4int,G4double,G4ThreeVector);
  MCTruthParticle* ParticleIn(G4int);

  G4int GetNParticlesOut() { return fParticleOut.size(); }
  MCTruthParticle* AddParticleOut(G4int,G4double,G4ThreeVector);
  MCTruthParticle* ParticleOut(G4int);

private:

  void Clear();
  
  G4String      fProcess;
  G4ThreeVector fPosition;
  G4double      fTime;
  std::vector<MCTruthParticle*> fParticleIn;
  std::vector<MCTruthParticle*> fParticleOut;

};

#endif
