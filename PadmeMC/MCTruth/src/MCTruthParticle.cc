// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2021-03-24
//
// --------------------------------------------------------------

#include "MCTruthParticle.hh"

#include "G4SystemOfUnits.hh"

MCTruthParticle::MCTruthParticle()
{
  fPDGCode = 0;
  fEnergy = 0.;
  fMomentum = G4ThreeVector(0.,0.,0.);
}

MCTruthParticle::MCTruthParticle(G4int pdg, G4double energy, G4ThreeVector momentum)
{
  fPDGCode = pdg;
  fEnergy = energy;
  fMomentum = momentum;
  //G4cout << "MCTruthParticle " << fPDGCode << " " << fEnergy << " " << fMomentum << G4endl;
}

MCTruthParticle::~MCTruthParticle()
{;}
