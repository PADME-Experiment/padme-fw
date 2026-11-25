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

  // type = 0, 1 for track/daughters_of_track
  void AddG4TrackSecondaryToVertex(G4int it, G4int iv, G4int position, G4int type){
    std::pair<G4int,G4int> pairin = {iv,position};
    std::pair<G4int,std::pair<G4int,G4int>> mapvalue = {it,pairin};
    if (type == 0) fG4TrackSecondaryMap.insert(mapvalue); // track in the map
    else           fG4TrackSecondaryDaughtersMap.insert(mapvalue); // daughter of track in the map    
  }

  // type = 0, 1 for track/daughters_of_track
  bool IsG4TrackInVertex(G4int g4trackid, G4int type){    
    if (type == 0) {
      if (fG4TrackSecondaryMap.find(g4trackid) == fG4TrackSecondaryMap.end()) return false;
      return true;
    } else {
      if (fG4TrackSecondaryDaughtersMap.find(g4trackid) == fG4TrackSecondaryDaughtersMap.end()) return false;
      return true;
    }
  }

  // type = 0, 1 for track/daughters_of_track
  std::pair <G4int,G4int> GetG4TrackInVertex(G4int g4trackid, G4int type){
    std::pair<G4int,G4int> nullpair = {-1,-1};
    if (type == 0) {
      auto it = fG4TrackSecondaryMap.find(g4trackid);
      if (it == fG4TrackSecondaryMap.end()){ // not found
	return nullpair;
      } else {     
	return it->second; 
      }
    } else {
      auto it = fG4TrackSecondaryDaughtersMap.find(g4trackid);
      if (it == fG4TrackSecondaryDaughtersMap.end()){ // not found
	return nullpair;
      } else {     
	return it->second; 
      }
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
  std::map<G4int, std::pair<G4int,G4int>> fG4TrackSecondaryDaughtersMap;  
  // might extend further the generation-level to the G4Tracks whose parent of parent is in the previous map
};
#endif // MCTruthManager_H
