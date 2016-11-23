// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-07-22
//
// --------------------------------------------------------------

#ifndef SACDigi_h
#define SACDigi_h 1

#include "G4VDigi.hh"
#include "G4TDigiCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#define SACDIGI_N_BINS 500

class SACDigi : public G4VDigi
{
  public:

      SACDigi();
     ~SACDigi();
      SACDigi(const SACDigi&);
      const SACDigi& operator=(const SACDigi&);
      G4int operator==(const SACDigi&) const;

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
  void AddEnergy(G4double e) { fEnergy += e; }
  G4double GetEnergy() { return fEnergy; }

  void ResetEHisto();
  void PrintEHisto();
  void SetEHistoStart(G4double v) { fEHistoStart = v; }
  void SetEHistoStep(G4double v)  { fEHistoStep = v; }
  G4int    GetEHistoNBins() { return SACDIGI_N_BINS; };
  G4double GetEHistoStart() { return fEHistoStart; };
  G4double GetEHistoStep()  { return fEHistoStep; };
  G4double GetEHistoEnd()   { return fEHistoStart+fEHistoStep*SACDIGI_N_BINS; };
  void     SetEHistoBin(G4int i,G4double e) { fEHisto[i] = e; }
  G4double GetEHistoBin(G4int i) { return fEHisto[i]; }
  void     AddEnergyAtTime(G4double,G4double); // Energy,Time
  G4double GetEnergyAtTime(G4double); // Time

private:
    
  G4int         fChannelId;
  G4double      fTime;
  G4double      fEnergy;

  G4double fEHistoStart;
  G4double fEHistoStep;
  G4double fEHisto[SACDIGI_N_BINS];

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4TDigiCollection<SACDigi> SACDigiCollection;

extern G4Allocator<SACDigi> SACDigiAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* SACDigi::operator new(size_t)
{
  void* aDigi;
  aDigi = (void*) SACDigiAllocator.MallocSingle();
  return aDigi;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void SACDigi::operator delete(void* aDigi)
{
  SACDigiAllocator.FreeSingle((SACDigi*) aDigi);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
