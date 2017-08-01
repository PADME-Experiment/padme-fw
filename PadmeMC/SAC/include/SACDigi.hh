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

#define SACDIGI_N_BINS 1024

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

  void ResetQHisto();
  void PrintQHisto();
  void SetQHistoStart(G4double v) { fQHistoStart = v; }
  void SetQHistoStep(G4double v)  { fQHistoStep = v; }
  G4int    GetQHistoNBins() { return SACDIGI_N_BINS; };
  G4double GetQHistoStart() { return fQHistoStart; };
  G4double GetQHistoStep()  { return fQHistoStep; };
  G4double GetQHistoEnd()   { return fQHistoStart+fQHistoStep*SACDIGI_N_BINS; };
  void     SetQHistoBin(G4int i,G4double q) { fQHisto[i] = q; }
  G4double GetQHistoBin(G4int i) { return fQHisto[i]; }
  void     AddChargeAtTime(G4double,G4double); // Charge,Time
  G4double GetChargeAtTime(G4double); // Time

private:
    
  G4int    fChannelId;
  G4double fTime;
  G4double fEnergy;

  G4double fQHistoStart;
  G4double fQHistoStep;
  G4double fQHisto[SACDIGI_N_BINS];

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
