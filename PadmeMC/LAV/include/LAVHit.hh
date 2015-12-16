// LAVHit.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#ifndef LAVHit_h
#define LAVHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class LAVHit : public G4VHit
{
  public:

      LAVHit();
     ~LAVHit();
      LAVHit(const LAVHit&);
      const LAVHit& operator=(const LAVHit&);
      G4int operator==(const LAVHit&) const;

      inline void* operator new(size_t);
      inline void  operator delete(void*);

      void Draw();
      void Print();

  public:
  
      void SetTrackID  (G4int track) { VtrackID = track; };
      void SetLAVNb(G4int cry)  { LAVNb = cry; };
      void SetPType(G4int typ)       { PType = typ; };
      void SetEdep(G4double de)      { edep = de; };
      void SetPos(G4ThreeVector xyz) { pos = xyz; };
      void SetTime(G4double time)    { VetoT = time; };
      
      G4int GetTrackID()    { return VtrackID; };
      G4int GetLAVNb() { return LAVNb; };
      G4double GetEdep()    { return edep; };      
      G4double GetTime()    { return VetoT; };
      G4int GetPType()      { return PType; };
      G4ThreeVector GetPos(){ return pos; };
      G4double GetX(){ return pos.x(); };
      G4double GetY(){ return pos.y(); };
      G4double GetZ(){ return pos.z(); };

  private:
  
      G4int         VtrackID;
      G4int         PType;
      G4int         LAVNb;
      G4double      VetoT;
      G4double      edep;
      G4ThreeVector pos;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<LAVHit> LAVHitsCollection;

extern G4Allocator<LAVHit> LAVHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* LAVHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) LAVHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void LAVHit::operator delete(void *aHit)
{
  LAVHitAllocator.FreeSingle((LAVHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
