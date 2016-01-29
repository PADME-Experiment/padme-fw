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
  
  void SetTrackID(G4int track)   { trackID = track; };
  void SetPType(G4int typ)       { PType = typ; }; 
  void SetEdep(G4double de)      { edep = de; };
  void SetTime(G4double HitT)    { TDumpHitT = HitT; };
  void SetPos(G4ThreeVector xyz) { pos = xyz; };
      
  G4int GetTrackID()    { return trackID; };
  G4double GetEdep()    { return edep; };
  G4double GetTime()    { return TDumpHitT; };
  G4int GetPType()      { return PType; };
  G4ThreeVector GetPos(){ return pos; };
  G4double GetX()       { return pos.x(); }
  G4double GetY()       { return pos.y(); }
  G4double GetZ()       { return pos.z(); }
      
private:
  
  G4int         trackID;
  G4int         PType;
  G4double      edep;
  G4double      TDumpHitT;
  G4ThreeVector pos;
  
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
