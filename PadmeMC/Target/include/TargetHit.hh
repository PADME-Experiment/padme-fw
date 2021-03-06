#ifndef TargetHit_h
#define TargetHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TargetHit : public G4VHit
{

public:

  TargetHit();
  ~TargetHit();
  TargetHit(const TargetHit&);
  const TargetHit& operator=(const TargetHit&);
  G4int operator==(const TargetHit&) const;

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

  //M. Raggi 2/04/2019 added track energy
  void SetTrackEnergy(G4double etr) { fTrackEnergy = etr; }
  G4double GetTrackEnergy() { return fTrackEnergy; }

  //M. Raggi 2/04/2019 added track energy
  G4double GetPX() { return fMomentumDirection.x(); };
  G4double GetPY() { return fMomentumDirection.y(); };
  G4double GetPZ() { return fMomentumDirection.z(); };
  void SetPDir(G4ThreeVector pdir) {fMomentumDirection=pdir;}

  void SetPrimary()    {fIsPrimary=true;}
  void SetNotPrimary() {fIsPrimary=false;}

  G4bool IsPrimary() {return fIsPrimary;}

  void SetLocalPosition(G4ThreeVector p) { fLocalPosition = p; }
  G4ThreeVector GetLocalPosition() { return fLocalPosition; }
  G4double GetLocalPosX() { return fLocalPosition.x(); };
  G4double GetLocalPosY() { return fLocalPosition.y(); };
  G4double GetLocalPosZ() { return fLocalPosition.z(); };
  
  void SetGlobalPositionPost(G4ThreeVector p) { fGlobalPositionPost = p; }
  G4ThreeVector GetGlobalPositionPost() { return fGlobalPositionPost; }
  G4double GetGlobalPosPostX() { return fGlobalPositionPost.x(); };
  G4double GetGlobalPosPostY() { return fGlobalPositionPost.y(); };
  G4double GetGlobalPosPostZ() { return fGlobalPositionPost.z(); };

  void SetLocalPositionPost(G4ThreeVector p) { fLocalPositionPost = p; }
  G4ThreeVector GetLocalPositionPost() { return fLocalPositionPost; }
  G4double GetLocalPosPostX() { return fLocalPositionPost.x(); };
  G4double GetLocalPosPostY() { return fLocalPositionPost.y(); };
  G4double GetLocalPosPostZ() { return fLocalPositionPost.z(); };
  
  // hit diagnostics
  void SetHitVal(G4int hv) { fHitVal = hv; }
  G4int GetHitVal() { return fHitVal; }

  // Kept for backward compatibility: will be removed
  void SetEdep(G4double e) { fEnergy = e; }
  G4double GetEdep() { return fEnergy; }
  void SetPos(G4ThreeVector xyz) { fPosition = xyz; };
  G4ThreeVector GetPos() { return fPosition; };
  G4double GetX() { return fPosition.x(); };
  G4double GetY() { return fPosition.y(); };
  G4double GetZ() { return fPosition.z(); };

private:

  G4double      fTime;
  G4double      fEnergy;
  G4double      fTrackEnergy; //M. Raggi 2/04/2019 added track energy
  G4ThreeVector fPosition;
  G4ThreeVector fLocalPosition;
  G4ThreeVector fGlobalPositionPost;
  G4ThreeVector fLocalPositionPost;
  G4ThreeVector fMomentumDirection; //M. Raggi 5/04/2019 added track energy
  G4bool fIsPrimary; //M. Raggi 3/05/2019 true if it's a primary positron
  
  //hit diagnostics
  G4int fHitVal;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<TargetHit> TargetHitsCollection;

extern G4Allocator<TargetHit> TargetHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* TargetHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) TargetHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void TargetHit::operator delete(void *aHit)
{
  TargetHitAllocator.FreeSingle((TargetHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
