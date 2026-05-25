// MMHit.hh
// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
//
// --------------------------------------------------------------

#ifndef MMHit_h
#define MMHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class MMHit : public G4VHit
{
public:

  MMHit();
  ~MMHit();
  MMHit(const MMHit&);
  const MMHit& operator=(const MMHit&);
  G4int operator==(const MMHit&) const;

  inline void* operator new(size_t);
  inline void  operator delete(void*);

  void Draw();
  void Print();

public:

  void SetTime(G4double t) { fTime = t; };
  G4double GetTime() { return fTime; };

  void SetEnergy(G4double e) { fEnergy = e; }
  G4double GetEnergy() { return fEnergy; }

  void SetPosition(G4ThreeVector p) { fPosition = p; }
  G4ThreeVector GetPosition() { return fPosition; }
  G4double GetPosX() { return fPosition.x(); };
  G4double GetPosY() { return fPosition.y(); };
  G4double GetPosZ() { return fPosition.z(); };

  void SetLocalPositionStart(G4ThreeVector p) { fLocalPositionStart = p; }
  G4ThreeVector GetLocalPositionStart() { return fLocalPositionStart; }
  G4double GetLocalStartPosX() { return fLocalPositionStart.x(); };
  G4double GetLocalStartPosY() { return fLocalPositionStart.y(); };
  G4double GetLocalStartPosZ() { return fLocalPositionStart.z(); };

  void SetLocalPositionEnd(G4ThreeVector p) { fLocalPositionEnd = p; }
  G4ThreeVector GetLocalPositionEnd() { return fLocalPositionEnd; }
  G4double GetLocalEndPosX() { return fLocalPositionEnd.x(); };
  G4double GetLocalEndPosY() { return fLocalPositionEnd.y(); };
  G4double GetLocalEndPosZ() { return fLocalPositionEnd.z(); };

  void SetPType(G4int typ) { fTrackType = typ; };
  G4int GetPType() { return fTrackType; };

  void SetTrackID(G4int track) { fTrackId = track; };
  G4int GetTrackID() { return fTrackId; };

  void SetParentID(G4int parent) { fParentId = parent; };
  G4int GetParentID() { return fParentId; };

  void SetEdep(G4double de) { fEnergy = de; };
  G4double GetEdep() { return fEnergy; };      

  void SetETrack(G4double de) { fETrack = de; };
  G4double GetETrack() { return fETrack; };      

  void SetPos(G4ThreeVector xyz) { fPosition = xyz; };
  G4ThreeVector GetPos(){ return fPosition; };
  void SetVertexPosition(G4ThreeVector xyz) { fVertexPos = xyz; };
  G4ThreeVector GetVertexPosition(){ return fVertexPos; };
  G4double GetX(){ return fPosition.x(); };
  G4double GetY(){ return fPosition.y(); };
  G4double GetZ(){ return fPosition.z(); };

private:

  G4int         fParentId;
  G4int         fTrackType;
  G4int         fTrackId;
  G4double      fTime;
  G4double      fEnergy;
  G4double      fETrack;
  G4ThreeVector fPosition;
  G4ThreeVector fVertexPos;
  G4ThreeVector fLocalPositionStart;
  G4ThreeVector fLocalPositionEnd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<MMHit> MMHitsCollection;

extern G4Allocator<MMHit> MMHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* MMHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) MMHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void MMHit::operator delete(void *aHit)
{
  MMHitAllocator.FreeSingle((MMHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
