#ifndef MagnetHit_h
#define MagnetHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class MagnetHit : public G4VHit
{

public:

  MagnetHit();
  ~MagnetHit();
  MagnetHit(const MagnetHit&);
  const MagnetHit& operator=(const MagnetHit&);
  G4int operator==(const MagnetHit&) const;

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

  void SetLocalPosition(G4ThreeVector p) { fLocalPosition = p; }
  G4ThreeVector GetLocalPosition() { return fLocalPosition; }
  G4double GetLocalPosX() { return fLocalPosition.x(); };
  G4double GetLocalPosY() { return fLocalPosition.y(); };
  G4double GetLocalPosZ() { return fLocalPosition.z(); };
 
  void SetTrackID(G4int track) { fTrackId = track; };
  G4int GetTrackID() { return fTrackId; };

  //G4double GetX() { return fHitPosition.x(); }
  //G4double GetY() { return fHitPosition.y(); }

private:
  
  G4int         fTrackId;
  G4double      fTime;
  G4double      fEnergy;
  G4ThreeVector fPosition;
  G4ThreeVector fLocalPosition;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<MagnetHit> MagnetHitsCollection;

extern G4Allocator<MagnetHit> MagnetHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* MagnetHit::operator new(size_t)
{
  void* aHit;
  aHit = (void *)MagnetHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void MagnetHit::operator delete(void *aHit)
{
  MagnetHitAllocator.FreeSingle((MagnetHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
