#ifndef ETagCluster_h
#define ETagCluster_h 1

#include <Rtypes.h>
#include <TVector3.h>

class ETagCluster
{
public:

  ETagCluster();
  ~ETagCluster();

  ULong64_t GetStatus()    { return fStatus;     }
  TVector3  GetPosition()  { return fPosition;   }
  Double_t  GetEnergy()    { return fEnergy;     }
  Double_t  GetTime()      { return fTime;       }

  void SetStatus   (ULong64_t value) { fStatus     = value; }
  void SetPosition (TVector3  value) { fPosition   = value; }
  void SetEnergy   (Double_t  value) { fEnergy     = value; }
  void AddEnergy   (Double_t  value) { fEnergy    += value; }
  void SetTime     (Double_t  value) { fTime       = value; }

  void SetNHits(UInt_t nh) { fNHits = nh; }
  UInt_t GetNHits() { return fNHits; }

  Int_t GetSeed() { return fSeed; }
  void SetSeed(Int_t i) { fSeed=i; }

  void SetHitsVector(std::vector<Int_t> v) { fHits=v; }
  std::vector<Int_t> GetHitsVector() { return fHits; }

  void Print();

private:

  ULong64_t fStatus;

  TVector3 fPosition;
  Double_t fEnergy;
  Double_t fTime;

  Int_t fSeed;              // index of hit selected as seed of this cluster
  UInt_t fNHits;            // number of hits in this cluster
  std::vector<Int_t> fHits; // vector of indices of hits belonging to this cluster

};
#endif // ETagCluster_h
