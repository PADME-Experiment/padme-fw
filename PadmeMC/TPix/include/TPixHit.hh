#ifndef TPixHit_h
#define TPixHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TPixHit : public G4VHit
{

public:

  TPixHit();
  ~TPixHit();
  TPixHit(const TPixHit&);
  const TPixHit& operator=(const TPixHit&);
  G4int operator==(const TPixHit&) const;

  inline void* operator new(size_t);
  inline void  operator delete(void*);

  void Draw();
  void Print();

public:
  
  void SetChannelId (G4int id)          { fChannelId = id;  }
  void SetEnergy    (G4double e)        { fEnergy    = e;    }
  void SetTime      (G4double time)     { fTime      = time; }
  void SetPosition  (G4ThreeVector xyz) { fPosition  = xyz;  }

  G4int         GetChannelId() { return fChannelId; }
  G4double      GetEnergy()    { return fEnergy;    }
  G4double      GetTime()      { return fTime;      }
  G4ThreeVector GetPosition()  { return fPosition;  }
      
private:
  
  G4int         fChannelId;
  G4double      fEnergy;
  G4double      fTime;
  G4ThreeVector fPosition;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<TPixHit> TPixHitsCollection;

extern G4Allocator<TPixHit> TPixHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* TPixHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) TPixHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void TPixHit::operator delete(void *aHit)
{
  TPixHitAllocator.FreeSingle((TPixHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
