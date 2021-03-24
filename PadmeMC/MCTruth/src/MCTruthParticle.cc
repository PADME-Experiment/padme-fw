// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2021-03-24
//
// --------------------------------------------------------------

#include "MCTruthParticle.hh"

MCTruthParticle::MCTruthParticle()
{
  fPDGCode = 0;
  fMomentum = G4ThreeVector(0.,0.,0.);
  fEnergy = 0.;
}

MCTruthParticle::MCTruthParticle(G4int pdg, G4double energy, G4ThreeVector momentum)
{
  fPDGCode = pdg;
  fEnergy = energy;
  fMomentum = momentum;
}

MCTruthParticle::~MCTruthParticle()
{;}
