#ifndef TDumpHit_h
#define TDumpHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TDumpHit : public G4VHit
{
public:

  TDumpHit();
  ~TDumpHit();
  TDumpHit(const TDumpHit&);
  const TDumpHit& operator=(const TDumpHit&);
  G4int operator==(const TDumpHit&) const;

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
  void SetPType(G4int typ)       { fTrackType = typ; }; 
  G4int GetPType()      { return fTrackType; };

  void SetEdep(G4double de)      { fEnergy = de; };
  G4double GetEdep()    { return fEnergy; };

  void SetPos(G4ThreeVector xyz) { fPosition = xyz; };
  G4ThreeVector GetPos(){ return fPosition; };
  G4double GetX()       { return fPosition.x(); }
  G4double GetY()       { return fPosition.y(); }
  G4double GetZ()       { return fPosition.z(); }

  // Obsolete and probably not used
  void SetTrackID(G4int track)   { fTrackId = track; };
  G4int GetTrackID()    { return fTrackId; };
      
private:
    
  G4int         fTrackType;
  G4int         fChannelId;
  G4double      fTime;
  G4double      fEnergy;
  G4ThreeVector fPosition;
  G4ThreeVector fLocalPosition;

  // Obsolete and probably not used
  G4int         fTrackId;
  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<TDumpHit> TDumpHitsCollection;

extern G4Allocator<TDumpHit> TDumpHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* TDumpHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) TDumpHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void TDumpHit::operator delete(void *aHit)
{
  TDumpHitAllocator.FreeSingle((TDumpHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
