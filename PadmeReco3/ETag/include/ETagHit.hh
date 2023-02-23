#ifndef ETagHit_h
#define ETagHit_h 1

#include <Rtypes.h>
#include <TVector3.h>

class TVector3;

class ETagHit
{
public:

  ETagHit();
  ~ETagHit();

  UChar_t   GetADCBoard()  { return fADCBoard;   }
  UChar_t   GetADCChannel(){ return fADCChannel; }
  Int_t     GetChannelId() { return fChannelId;  }
  ULong64_t GetStatus()    { return fStatus;     }
  TVector3  GetPosition()  { return fPosition;   }
  Double_t  GetEnergy()    { return fEnergy;     }
  Double_t  GetTime()      { return fTime;       }

  void SetADCBoard  (UChar_t   value) { fADCBoard   = value; }
  void SetADCChannel(UChar_t   value) { fADCChannel = value; }
  void SetChannelId (Int_t     value) { fChannelId  = value; }
  void SetStatus    (ULong64_t value) { fStatus     = value; }
  void SetPosition  (TVector3  value) { fPosition   = value; }
  void SetEnergy    (Double_t  value) { fEnergy     = value; }
  void AddEnergy    (Double_t  value) { fEnergy    += value; }
  void SetTime      (Double_t  value) { fTime       = value; }

private:

  UChar_t fADCBoard;
  UChar_t fADCChannel;

  Int_t   fChannelId;

  ULong64_t fStatus;

  TVector3 fPosition;
  Double_t fEnergy;
  Double_t fTime;

};
#endif // ETagHit_h
