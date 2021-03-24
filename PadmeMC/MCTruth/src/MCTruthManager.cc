// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2021-03-24
//
// --------------------------------------------------------------

#include "MCTruthManager.hh"

#include "MCTruthVertex.hh"

MCTruthManager* MCTruthManager::fInstance = 0;

MCTruthManager::MCTruthManager()
{
  Clear();
}

MCTruthManager::~MCTruthManager()
{
  for(std::vector<MCTruthVertex*>::iterator it = fVertex.begin(); it != fVertex.end(); ++it) {
    if (*it) delete *it;
  }
}

MCTruthManager* MCTruthManager::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new MCTruthManager(); }
  return fInstance;
}

void MCTruthManager::Clear()
{
  fEventWeight = 1.;
  fVertex.clear();
}

MCTruthVertex* MCTruthManager::AddVertex(G4String type, G4ThreeVector pos, G4double time)
{
  MCTruthVertex* vtx = new MCTruthVertex(type,pos,time);
  fVertex.push_back(vtx);
  return vtx;
}

MCTruthVertex* MCTruthManager::Vertex(G4int i)
{
  if (i >= fVertex.size()) {
    G4cout << "MCTruthManager::Vertex - ERROR - Index " << i << " is out of bound. Currently only " << fVertex.size() << " vertices are defined." << G4endl;
    return 0;
  }
  return fVertex[i];
}
