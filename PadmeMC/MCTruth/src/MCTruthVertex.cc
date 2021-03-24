// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2021-03-24
//
// --------------------------------------------------------------

#include "MCTruthVertex.hh"

#include "MCTruthParticle.hh"

MCTruthVertex::MCTruthVertex()
{
  Clear();
}

MCTruthVertex::MCTruthVertex(G4String process, G4ThreeVector position, G4double time)
{
  Clear();
  fProcess = process;
  fPosition = position;
  fTime = time;
}

MCTruthVertex::~MCTruthVertex()
{
  Clear();
}

void MCTruthVertex::Clear()
{
  fProcess = "";
  fPosition = G4ThreeVector(0.,0.,0.);
  fTime = 0.;
  for(std::vector<MCTruthParticle*>::iterator it = fParticleIn.begin(); it != fParticleIn.end(); ++it) {
    if (*it) delete *it;
  }
  for(std::vector<MCTruthParticle*>::iterator it = fParticleOut.begin(); it != fParticleOut.end(); ++it) {
    if (*it) delete *it;
  }
}

MCTruthParticle* MCTruthVertex::AddParticleIn(G4int pdgCode,G4double energy, G4ThreeVector momentum)
{
  MCTruthParticle* part = new MCTruthParticle(pdgCode,energy,momentum);
  fParticleIn.push_back(part);
  return part;
}

MCTruthParticle* MCTruthVertex::ParticleIn(G4int i)
{
  if (i >= fParticleIn.size()) {
    G4cout << "MCTruthVertex::ParticleIn - ERROR - Index " << i << " is out of bound. Currently only " << fParticleIn.size() << " particles are defined." << G4endl;
    return 0;
  }
  return fParticleIn[i];
}

MCTruthParticle* MCTruthVertex::AddParticleOut(G4int pdgCode,G4double energy, G4ThreeVector momentum)
{
  MCTruthParticle* part = new MCTruthParticle(pdgCode,energy,momentum);
  fParticleOut.push_back(part);
  return part;
}

MCTruthParticle* MCTruthVertex::ParticleOut(G4int i)
{
  if (i >= fParticleOut.size()) {
    G4cout << "MCTruthVertex::ParticleOut - ERROR - Index " << i << " is out of bound. Currently only " << fParticleOut.size() << " particles are defined." << G4endl;
    return 0;
  }
  return fParticleOut[i];
}
