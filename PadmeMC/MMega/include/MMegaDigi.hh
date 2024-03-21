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
    
  std::vector<G4double> GetTime(){return fTime;};
  void SetTime(const std::vector<G4double>& t){fTime = t;};

  G4double GetCharge(){return fCharge;};
  void SetCharge(G4double c){fCharge = c;};

  // G4double GetXCenter(){return fXCenter;};
  // void SetXCenter(double x){fXCenter = x;};

  // G4double GetYCenter(){return fYCenter;};
  // void SetYCenter(double y){fYCenter = y;};

  G4int GetXId(){return fXStripID;};
  void SetXId(int ix){fXStripID = ix;};

  G4int GetYId(){return fYStripID;};
  void SetYId(int iy){fYStripID = iy;};

  G4int GetisRear(){return fisRear;};
  void SetisRear(int is){fisRear = is;};
  
  G4int GetNHits(){return fNHits;};
  void SetNHits(int nh){fNHits = nh;};

private:
  
  std::vector<G4double> fTime;
  G4double fCharge;
  // G4double fXCenter;
  // G4double fYCenter;
  G4int    fXStripID;
  G4int    fYStripID;
  G4int    fisRear;
  G4int    fNHits;
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
