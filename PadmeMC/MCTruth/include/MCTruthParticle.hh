// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2021-03-24
//
// --------------------------------------------------------------

#ifndef MCTruthParticle_h
#define MCTruthParticle_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"

class MCTruthParticle
{
public:

  MCTruthParticle();
  MCTruthParticle(G4int,G4double,G4ThreeVector);
  ~MCTruthParticle();

public:

  void SetPDGCode(G4int i) { fPDGCode = i; }
  G4int GetPDGCode() { return fPDGCode; }

  void SetMomentum(G4ThreeVector p) { fMomentum = p; }
  G4ThreeVector GetMomentum() { return fMomentum; }
  G4double GetMomX() { return fMomentum.x(); };
  G4double GetMomY() { return fMomentum.y(); };
  G4double GetMomZ() { return fMomentum.z(); };

  void SetEnergy(G4double t) { fEnergy = t; };
  G4double GetEnergy() { return fEnergy; };

private:
  
  G4int         fPDGCode;
  G4ThreeVector fMomentum;
  G4double      fEnergy;

};

#endif
