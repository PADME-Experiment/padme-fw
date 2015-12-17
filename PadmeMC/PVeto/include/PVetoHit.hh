#ifndef PVetoHit_h
#define PVetoHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PVetoHit : public G4VHit
{
  public:

      PVetoHit();
     ~PVetoHit();
      PVetoHit(const PVetoHit&);
      const PVetoHit& operator=(const PVetoHit&);
      G4int operator==(const PVetoHit&) const;

      inline void* operator new(size_t);
      inline void  operator delete(void*);

      void Draw();
      void Print();

  public:
  
      void SetTrackID  (G4int track)      { VtrackID = track; };
      void SetPVetoNb  (G4int cry)        { PVetoNb = cry; };
      void SetEdep     (G4double de)      { edep = de; };
      void SetHitE     (G4double hite)    { eHit = hite; };
      void SetPos      (G4ThreeVector xyz){ pos = xyz; };
      void SetTime     (G4double time)    { VetoT = time; };
      
      G4int GetTrackID()    { return VtrackID; };
      G4int GetPVetoNb()    { return PVetoNb; };
      G4double GetEdep()    { return edep; };
      G4double GetHitE()    { return eHit; };
      G4double GetTime()    { return VetoT; };
      G4double GetX()       { return pos.x(); };
      G4double GetY()       { return pos.y(); };

      G4ThreeVector GetPos(){ return pos; };
      

  private:
  
      G4int         VtrackID;
      G4int         PVetoNb;
      G4double      VetoT;
      G4double      edep;
      G4double      eHit;
      G4ThreeVector pos;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<PVetoHit> PVetoHitsCollection;

extern G4Allocator<PVetoHit> PVetoHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* PVetoHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) PVetoHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void PVetoHit::operator delete(void *aHit)
{
  PVetoHitAllocator.FreeSingle((PVetoHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
