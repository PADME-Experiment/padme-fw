// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-17
//
// --------------------------------------------------------------

#ifndef SACHit_h
#define SACHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class SACHit : public G4VHit
{
public:

  SACHit();
  ~SACHit();
  SACHit(const SACHit&);
  const SACHit& operator=(const SACHit&);
  G4int operator==(const SACHit&) const;

  inline void* operator new(size_t);
  inline void  operator delete(void*);

  void Draw();
  void Print();

public:
  
  void SetTrackID(G4int track)   { fTrackId = track; };
  void SetCryNb(G4int cry)       { fChannelId = cry; }; 
  void SetChannelId(G4int cry)       { fChannelId = cry; }; 
  void SetPType(G4int typ)       { fParticleType = typ; }; 
  void SetEdep(G4double de)      { fEnergy = de; };
  void SetEnergy(G4double de)      { fEnergy = de; };
  void SetTime(G4double HitT)    { fTime = HitT; };
  void SetPos(G4ThreeVector xyz) { fPosition = xyz; };
  void SetPosition(G4ThreeVector xyz) { fPosition = xyz; };
      
  G4int GetTrackID()    { return fTrackId; };
  G4int GetCryNb()      { return fChannelId; };
  G4int GetChannelId()      { return fChannelId; };
  G4double GetEdep()    { return fEnergy; };
  G4double GetEnergy()    { return fEnergy; };
  G4double GetTime()    { return fTime; };
  G4int GetPType()      { return fParticleType; };

  G4ThreeVector GetPos() { return fPosition; };
  G4ThreeVector GetPosition() { return fPosition; };
  G4double GetX() { return fPosition.x(); }
  G4double GetY() { return fPosition.y(); }
  G4double GetZ() { return fPosition.z(); }
      
private:

  G4int         fTrackId;
  G4int         fParticleType;
  G4int         fChannelId;
  G4double      fEnergy;
  G4double      fTime;
  G4ThreeVector fPosition;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<SACHit> SACHitsCollection;

extern G4Allocator<SACHit> SACHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* SACHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) SACHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void SACHit::operator delete(void *aHit)
{
  SACHitAllocator.FreeSingle((SACHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
