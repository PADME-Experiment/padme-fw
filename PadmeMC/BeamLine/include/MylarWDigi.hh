// --------------------------------------------------------------
// History:
//
// Created by Mauro Raggi (mauro.raggi@roma1.infn.it) 2019-04-29
//
// --------------------------------------------------------------

#ifndef MylarWDigi_h
#define MylarWDigi_h 1

#include "G4VDigi.hh"
#include "G4TDigiCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class MylarWDigi : public G4VDigi
{
  public: 

      MylarWDigi();
     ~MylarWDigi();
      MylarWDigi(const MylarWDigi&);
      const MylarWDigi& operator=(const MylarWDigi&);
      G4int operator==(const MylarWDigi&) const;

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

typedef G4TDigiCollection<MylarWDigi> MylarWDigiCollection;

extern G4Allocator<MylarWDigi> MylarWDigiAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* MylarWDigi::operator new(size_t)
{
  void* aDigi;
  aDigi = (void*) MylarWDigiAllocator.MallocSingle();
  return aDigi;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void MylarWDigi::operator delete(void* aDigi)
{
  MylarWDigiAllocator.FreeSingle((MylarWDigi*) aDigi);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
