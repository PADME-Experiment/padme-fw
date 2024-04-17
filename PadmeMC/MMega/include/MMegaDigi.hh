// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2024-03-08
//
// --------------------------------------------------------------

#ifndef MMegaDigi_h
#define MMegaDigi_h 1

#include "G4VDigi.hh"
#include "G4TDigiCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class MMegaDigi : public G4VDigi
{
  public:

      MMegaDigi();
     ~MMegaDigi();
      MMegaDigi(const MMegaDigi&);
      const MMegaDigi& operator=(const MMegaDigi&);
      G4int operator==(const MMegaDigi&) const;

      inline void* operator new(size_t);
      inline void  operator delete(void*);

      void Draw();
      void Print();

  public:
    
  std::vector<G4double> GetTimes(){return fTimes;};
  void SetTimes(const std::vector<G4double>& t){fTimes = t;};

  G4double GetCharge(){return fCharge;};
  void SetCharge(G4float c){fCharge = c;};

  // G4double GetCenter(){return fCenter;};
  // void SetCenter(double x){fCenter = x;};
  
  G4int GetNHits(){return fNHits;};
  void SetNHits(int nh){fNHits = nh;};

  G4int GetID(){return fID;};
  void SetID(int id){fID = id;};

private:
  
  std::vector<G4double> fTimes; //drift electron arrival times
  G4int    fNHits;              //number of collected electrons
  G4double fCharge;             //collected charge
  // G4double fCenter;          //strip(pad) center (ID accounts for which plane we are referring to)
  G4int    fID;                 //strip(pad) ID
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4TDigiCollection<MMegaDigi> MMegaDigiCollection;

extern G4Allocator<MMegaDigi> MMegaDigiAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* MMegaDigi::operator new(size_t)
{
  void* aDigi;
  aDigi = (void*) MMegaDigiAllocator.MallocSingle();
  return aDigi;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void MMegaDigi::operator delete(void* aDigi)
{
  MMegaDigiAllocator.FreeSingle((MMegaDigi*) aDigi);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
