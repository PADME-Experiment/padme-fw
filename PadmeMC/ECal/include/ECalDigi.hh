// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-07-21
//
// --------------------------------------------------------------

#ifndef ECalDigi_h
#define ECalDigi_h 1

#include "G4VDigi.hh"
#include "G4TDigiCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ECalDigi : public G4VDigi
{
  public:

      ECalDigi();
     ~ECalDigi();
      ECalDigi(const ECalDigi&);
      const ECalDigi& operator=(const ECalDigi&);
      G4int operator==(const ECalDigi&) const;

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

  void SetSignal(G4double s) { fSignal = s; }
  G4double GetSignal() { return fSignal; }

private:
    
  G4int    fChannelId;
  G4double fTime;
  G4double fEnergy;
  G4double fSignal;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4TDigiCollection<ECalDigi> ECalDigiCollection;

extern G4Allocator<ECalDigi> ECalDigiAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* ECalDigi::operator new(size_t)
{
  void* aDigi;
  aDigi = (void*) ECalDigiAllocator.MallocSingle();
  return aDigi;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void ECalDigi::operator delete(void* aDigi)
{
  ECalDigiAllocator.FreeSingle((ECalDigi*) aDigi);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
