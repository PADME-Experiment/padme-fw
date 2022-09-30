// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2021-03-24
//
// --------------------------------------------------------------

#ifndef MCTruthManager_H
#define MCTruthManager_H 1

#include "globals.hh"
#include "G4ThreeVector.hh"

#include "MCTruthVertex.hh"
#include <vector>

class MCTruthManager
{
public:

  virtual ~MCTruthManager();
  static MCTruthManager* GetInstance();

  void Clear();

private:

  static MCTruthManager* fInstance;

protected:

  MCTruthManager();

public:

  G4double GetEventWeight() { return fEventWeight; }
  void SetEventWeight(G4double val) { fEventWeight = val; }

  G4int GetNVertices() { return fVertex.size(); }
  MCTruthVertex* AddVertex(G4String,G4ThreeVector,G4double);
  MCTruthVertex* Vertex(G4int);

  void Enable() { fEnabled = true; }
  void Disable() { fEnabled = false; }
  G4bool IsEnabled() { return fEnabled; }

private:

  G4bool fEnabled;
  G4double fEventWeight;
  std::vector<MCTruthVertex*> fVertex;

};
#endif // MCTruthManager_H
