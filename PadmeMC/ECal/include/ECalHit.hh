// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-17
//
// --------------------------------------------------------------

#ifndef ECalHit_h
#define ECalHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ECalHit : public G4VHit
{
public:

  ECalHit();
  ~ECalHit();
  ECalHit(const ECalHit&);
  const ECalHit& operator=(const ECalHit&);
  G4int operator==(const ECalHit&) const;

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
  //void SetCryNb(G4int cry) { fChannelId = cry; }; 
  //G4int GetCryNb() { return fChannelId; };

  void SetPType(G4int typ) { fTrackType = typ; }; 
  G4int GetPType() { return fTrackType; };

  void SetEdep(G4double de) { fEnergy = de; };
  G4double GetEdep() { return fEnergy; };

  void SetPos(G4ThreeVector xyz) { fPosition = xyz; };      
  G4ThreeVector GetPos() { return fPosition; };

  // Obsolete and probably not used
  void SetTrackID(G4int track) { trackID = track; };
  G4int GetTrackID() { return trackID; };

private:
  
  G4int         fTrackType;
  G4int         fChannelId;
  G4double      fTime;
  G4double      fEnergy;
  G4ThreeVector fPosition;
  G4ThreeVector fLocalPosition;
  
  // Obsolete and probably not used
  G4int         trackID;
  G4double      ECalHitT;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<ECalHit> ECalHitsCollection;

extern G4Allocator<ECalHit> ECalHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* ECalHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) ECalHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void ECalHit::operator delete(void *aHit)
{
  ECalHitAllocator.FreeSingle((ECalHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
