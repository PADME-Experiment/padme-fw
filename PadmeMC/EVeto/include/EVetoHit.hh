#ifndef EVetoHit_h
#define EVetoHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EVetoHit : public G4VHit
{
  public:

      EVetoHit();
     ~EVetoHit();
      EVetoHit(const EVetoHit&);
      const EVetoHit& operator=(const EVetoHit&);
      G4int operator==(const EVetoHit&) const;

      inline void* operator new(size_t);
      inline void  operator delete(void*);

      void Draw();
      void Print();

  public:
  
      void SetTrackID  (G4int track)      { VtrackID = track; };
      void SetEVetoNb  (G4int cry)        { EVetoNb = cry; };
      void SetEdep     (G4double de)      { edep = de; };
      void SetHitE     (G4double hite)    { eHit = hite; };
      void SetPos      (G4ThreeVector xyz){ pos = xyz; };
      void SetTime     (G4double time)    { VetoT = time; };
      
      G4int GetTrackID()    { return VtrackID; };
      G4int GetEVetoNb()    { return EVetoNb; };
      G4double GetEdep()    { return edep; };
      G4double GetHitE()    { return eHit; };
      G4double GetTime()    { return VetoT; };
      G4double GetX()       { return pos.x(); };
      G4double GetY()       { return pos.y(); };

      G4ThreeVector GetPos(){ return pos; };
      

  private:
  
      G4int         VtrackID;
      G4int         EVetoNb;
      G4double      VetoT;
      G4double      edep;
      G4double      eHit;
      G4ThreeVector pos;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<EVetoHit> EVetoHitsCollection;

extern G4Allocator<EVetoHit> EVetoHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* EVetoHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) EVetoHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void EVetoHit::operator delete(void *aHit)
{
  EVetoHitAllocator.FreeSingle((EVetoHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
