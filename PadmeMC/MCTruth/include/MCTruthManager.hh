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

  void AddG4TrackSecondaryToNewVertex(G4int it, G4int position){
    std::pair<G4int,G4int> pairin = {it,position};
    std::pair<G4int,std::pair<G4int,G4int>> mapvalue = {(G4int)fVertex.size()-1,pairin};
    fG4TrackSecondaryMap.insert(mapvalue);
  }

  bool IsG4TrackInVertex(G4int g4trackid){    
    if (fG4TrackSecondaryMap.find(g4trackid) == fG4TrackSecondaryMap.end()) return false;
    return true;
  }

  std::pair <G4int,G4int> GetG4TrackInVertex(G4int g4trackid){
    auto it = fG4TrackSecondaryMap.find(g4trackid);
    if (it == fG4TrackSecondaryMap.end()){ // not found
      std::pair<G4int,G4int> nullpair = {-1,-1};
      return nullpair;
    } else {     
      return it->second; 
    }
  }

  MCTruthVertex* Vertex(G4int);

  void Enable() { fEnabled = true; }
  void Disable() { fEnabled = false; }
  G4bool IsEnabled() { return fEnabled; }

private:

  G4bool fEnabled;
  G4double fEventWeight;
  std::vector<MCTruthVertex*> fVertex;
  // establish a map linking a G4Track index to a specific outgoing particle of a specific Vertex, providing the position in the fVertex vector and the position in the secondary list
  std::map<G4int, std::pair<G4int, G4int>> fG4TrackSecondaryMap;  
  // establish a map linking a G4Track to the position of its parent in the previous map (only filled if the G4Track ID of such parent is in the above map)
  //  std::map<G4int, G4int> fG4TrackSecondaryDaughtersMap;  
  // might extend further the generation-level to the G4Tracks whose parent of parent is in the previous map
  //  std::map<G4int, G4int> fG4TrackSecondaryNephewsMap;
};
#endif // MCTruthManager_H
