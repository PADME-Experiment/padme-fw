// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2017-03-15
//
// --------------------------------------------------------------

#ifndef TPixDigi_h
#define TPixDigi_h 1

#include "G4VDigi.hh"
#include "G4TDigiCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TPixDigi : public G4VDigi
{
  public:

      TPixDigi();
     ~TPixDigi();
      TPixDigi(const TPixDigi&);
      const TPixDigi& operator=(const TPixDigi&);
      G4int operator==(const TPixDigi&) const;

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

private:
    
  G4int         fChannelId;
  G4double      fTime;
  G4double      fEnergy;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4TDigiCollection<TPixDigi> TPixDigiCollection;

extern G4Allocator<TPixDigi> TPixDigiAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* TPixDigi::operator new(size_t)
{
  void* aDigi;
  aDigi = (void*) TPixDigiAllocator.MallocSingle();
  return aDigi;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void TPixDigi::operator delete(void* aDigi)
{
  TPixDigiAllocator.FreeSingle((TPixDigi*) aDigi);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
