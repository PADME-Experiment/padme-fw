// ETagHit.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#ifndef ETagHit_h
#define ETagHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ETagHit : public G4VHit
{
public:

  ETagHit();
  ~ETagHit();
  ETagHit(const ETagHit&);
  const ETagHit& operator=(const ETagHit&);
  G4int operator==(const ETagHit&) const;

  inline void* operator new(size_t);
  inline void  operator delete(void*);

  void Draw();
  void Print();

public:

  void SetChannelId(G4int c) { fChannelId = c; }
  G4int GetChannelId() { return fChannelId; }

  void SetTime(G4double t) { fTime = t; };
  G4double GetTime() { return fTime; };

  void SetEnergy(G4double e) { fEnergy = e; }
  G4double GetEnergy() { return fEnergy; }

  void SetPosition(G4ThreeVector p) { fPosition = p; }
  G4ThreeVector GetPosition() { return fPosition; }
  G4double GetPosX() { return fPosition.x(); };
  G4double GetPosY() { return fPosition.y(); };
  G4double GetPosZ() { return fPosition.z(); };

  void SetLocalPosition(G4ThreeVector p) { fLocalPosition = p; }
  G4ThreeVector GetLocalPosition() { return fLocalPosition; }
  G4double GetLocalPosX() { return fLocalPosition.x(); };
  G4double GetLocalPosY() { return fLocalPosition.y(); };
  G4double GetLocalPosZ() { return fLocalPosition.z(); };
 
  // Kept for backward compatibility: will be removed
  void SetETagNb(G4int cry) { fChannelId = cry; };
  G4int GetETagNb() { return fChannelId; };

  void SetPType(G4int typ) { fTrackType = typ; };
  G4int GetPType() { return fTrackType; };

  void SetTrackID(G4int track) { fTrackId = track; };
  G4int GetTrackID() { return fTrackId; };

  void SetEdep(G4double de) { fEnergy = de; };
  G4double GetEdep() { return fEnergy; };      

  void SetETrack(G4double de) { fETrack = de; };
  G4double GetETrack() { return fETrack; };      

  void SetPos(G4ThreeVector xyz) { fPosition = xyz; };
  G4ThreeVector GetPos(){ return fPosition; };
  G4double GetX(){ return fPosition.x(); };
  G4double GetY(){ return fPosition.y(); };
  G4double GetZ(){ return fPosition.z(); };

private:

  G4int         fTrackType;
  G4int         fTrackId;
  G4int         fChannelId;
  G4double      fTime;
  G4double      fEnergy;
  G4double      fETrack;
  G4ThreeVector fPosition;
  G4ThreeVector fLocalPosition;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<ETagHit> ETagHitsCollection;

extern G4Allocator<ETagHit> ETagHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* ETagHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) ETagHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void ETagHit::operator delete(void *aHit)
{
  ETagHitAllocator.FreeSingle((ETagHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
