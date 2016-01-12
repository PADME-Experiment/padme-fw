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
  
      void SetTrackID  (G4int track)       { fHitTrackId  = track; }
      void SetTime     (G4double time)     { fHitTime     = time;  }
      void SetEnergy   (G4double de)       { fHitEnergy   = de;    }
      void SetPosition (G4ThreeVector xyz) { fHitPosition = xyz;   }

      G4int         GetTrackID()  { return fHitTrackId;  }
      G4double      GetEnergy()   { return fHitEnergy;   }
      G4double      GetTime()     { return fHitTime;     }
      G4ThreeVector GetPosition() { return fHitPosition; }

      G4double      GetX() { return fHitPosition.x(); }
      G4double      GetY() { return fHitPosition.y(); }

  private:
  
      G4int         fHitTrackId;
      G4double      fHitTime;
      G4double      fHitEnergy;
      G4ThreeVector fHitPosition;

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
