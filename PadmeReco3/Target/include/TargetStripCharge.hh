#ifndef TargetStripCharge_h
#define TargetStripCharge_h 1

#include <Rtypes.h>
#include <TVector3.h>

class TVector3;

class TargetStripCharge
{
public:

  TargetStripCharge();
  ~TargetStripCharge();

  void Print();

  UChar_t   GetADCBoard()  { return fADCBoard;   }
  UChar_t   GetADCChannel(){ return fADCChannel; }
  Int_t     GetChannelId() { return fChannelId;  }
  ULong64_t GetStatus()    { return fStatus;     }
  TVector3  GetPosition()  { return fPosition;   }
  //commented by Beth 21/3/24
  /*  Double_t  GetEnergy()    { return fEnergy;     }
      Double_t  GetTime()      { return fTime;       }*/
  Double_t  GetCharge()    { return fCharge;     }

  void SetADCBoard  (UChar_t   value) { fADCBoard   = value; }
  void SetADCChannel(UChar_t   value) { fADCChannel = value; }
  void SetChannelId (Int_t     value) { fChannelId  = value; }
  void SetStatus    (ULong64_t value) { fStatus     = value; }
  void SetPosition  (TVector3  value) { fPosition   = value; }
  //commented by Beth 21/3/24
  /*  void SetEnergy    (Double_t  value) { fEnergy     = value; }
  void AddEnergy    (Double_t  value) { fEnergy    += value; }
  void SetTime      (Double_t  value) { fTime       = value; }*/
  void SetCharge    (Double_t  value) { fCharge     = value; }
  void AddCharge    (Double_t  value) { fCharge    += value; }
private:

  UChar_t fADCBoard;
  UChar_t fADCChannel;

  Int_t   fChannelId;

  ULong64_t fStatus;

  TVector3 fPosition;
  //commented by Beth 21/3/24
  //  Double_t fEnergy;
  Double_t fCharge;
  Double_t fTime;

};
#endif // TargetStripCharge_h
