// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-07-22
//
// --------------------------------------------------------------

#ifndef TargetDigi_h
#define TargetDigi_h 1

#include "G4VDigi.hh"
#include "G4TDigiCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TargetDigi : public G4VDigi
{
  public:

      TargetDigi();
     ~TargetDigi();
      TargetDigi(const TargetDigi&);
      const TargetDigi& operator=(const TargetDigi&);
      G4int operator==(const TargetDigi&) const;

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
  
  void SetDNumber(G4int dn) { fDNumber = dn; }
  G4int GetDNumber() { return fDNumber; }

  void SetChargeT(G4double c) { fChargeT = c; }
  G4double GetChargeT() { return fChargeT; }

  void SetCharge(G4double c) { fCharge = c; }
  G4double GetCharge() { return fCharge; }

  void SetNoiseCharge(G4double nc) { fNCharge = nc; }
  G4double GetNoiseCharge() { return fNCharge; }

  //   void SetTimeTrace(const std::vector<G4double>& tt) { fTimeTrace = tt; }
  void SetWaveformRaw(const std::vector<G4double>& wfr) { fWaveformRaw = wfr; }

  const std::vector<G4double>& GetWaveformRaw() const { return fWaveformRaw; }

  void SetWaveform(const std::vector<G4double>& wf) { fWaveform = wf; }
  const std::vector<G4double>& GetWaveform() const { return fWaveform; }

  //  void SetTimeTraceV(const std::vector<G4int>& tt) { fTimeTraceV = tt; }
  // const std::vector<G4int>& GetTimeTraceV() const { return fTimeTraceV; }

private:
    
  G4int         fChannelId;
  G4double      fTime;
  G4double      fEnergy;
  G4int         fDNumber;
  G4double      fChargeT;
  G4double      fCharge;
  G4double      fNCharge;

  std::vector<G4double> fWaveformRaw;
  std::vector<G4double> fWaveform;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4TDigiCollection<TargetDigi> TargetDigiCollection;

extern G4Allocator<TargetDigi> TargetDigiAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* TargetDigi::operator new(size_t)
{
  void* aDigi;
  aDigi = (void*) TargetDigiAllocator.MallocSingle();
  return aDigi;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void TargetDigi::operator delete(void* aDigi)
{
  TargetDigiAllocator.FreeSingle((TargetDigi*) aDigi);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
