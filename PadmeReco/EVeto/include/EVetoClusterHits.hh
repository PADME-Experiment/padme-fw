#ifndef EVetoClusterHits_h
#define EVetoClusterHits_h

#define HIT_STATUS_BIT_USED 0
#define HIT_STATUS_BIT_SEED 1

#include "Rtypes.h"//ROOT basic types
#include "TVector3.h"

class EVetoClusterHits {

 public :
 
  EVetoClusterHits();
  ~EVetoClusterHits();
  
 public :

  Int_t    GetChannelId() const { return fChannelId; };
  TVector3 GetPosition()  const { return fPosition;  };
  Double_t GetEnergy()    const { return fEnergy;    };
  Double_t GetTime()      const { return fTime;      };  

  void  SetChannelId(Int_t    value) { fChannelId = value; };
  void  SetPosition (TVector3 value) { fPosition = value;  };
  void  SetEnergy   (Double_t value) { fEnergy = value;    };
  //  void  AddEnergy   (Double_t value) { fEnergy += value;   };
  void  SetTime     (Double_t value) { fTime = value;      };  

  Bool_t IsUsed() { return fStatus & (1 << HIT_STATUS_BIT_USED); }
  Bool_t IsSeed() { return fStatus & (1 << HIT_STATUS_BIT_SEED); }

  void SetUsed()   { fStatus |=  (1 << HIT_STATUS_BIT_USED); }
  void ResetUsed() { fStatus &= ~(1 << HIT_STATUS_BIT_USED); }
  void SetSeed()   { fStatus |=  (1 << HIT_STATUS_BIT_SEED); }
  void ResetSeed() { fStatus &= ~(1 << HIT_STATUS_BIT_SEED); }

  void  Print();
  EVetoClusterHits(const EVetoClusterHits&);
  void Clear();

 private:

  Int_t fHitIndex;	 
  Int_t fXi;	 
  Int_t fYi;

  Int_t fStatus;

  Double_t fXCenter;
  Double_t fYCenter;
  Double_t fZCenter;

  Double_t fEnergy;
  Double_t fTime;	 
  Int_t fChannelId;
  TVector3 fPosition;
};
#endif
