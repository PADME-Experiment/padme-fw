#ifndef ECalCrystal_h
#define ECalCrystal_h

#define CRYSTAL_STATUS_BIT_USED 0
#define CRYSTAL_STATUS_BIT_SEED 1

#include "Rtypes.h"

class ECalCrystal {

 public :
 
  ECalCrystal(Int_t ,Int_t);
  ~ECalCrystal();
  
 public :

  Double_t GetEnergy()  {return fEnergy;}
  Double_t GetTime()    {return fTime;}	
  Double_t GetCharge()  {return fCharge;}

  void SetEnergy(Double_t e)   { fEnergy=e; }
  void SetTime(Double_t t)     { fTime=t; }	
  void SetCharge(Double_t cha) { fCharge=cha; }

  Double_t GetXCenter() { return fXCenter; }
  Double_t GetYCenter() { return fYCenter; }
  Double_t GetZCenter() { return fZCenter; }

  Int_t GetXi() { return fXi; }
  Int_t GetYi() { return fYi; }

  Bool_t IsUsed() { return fStatus & (1 << CRYSTAL_STATUS_BIT_USED); }
  Bool_t IsSeed() { return fStatus & (1 << CRYSTAL_STATUS_BIT_SEED); }

  void SetUsed()   { fStatus |=  (1 << CRYSTAL_STATUS_BIT_USED); }
  void ResetUsed() { fStatus &= ~(1 << CRYSTAL_STATUS_BIT_USED); }
  void SetSeed()   { fStatus |=  (1 << CRYSTAL_STATUS_BIT_SEED); }
  void ResetSeed() { fStatus &= ~(1 << CRYSTAL_STATUS_BIT_SEED); }

  void  Print();

 private:

  Int_t fXi;	 
  Int_t fYi;

  Int_t fStatus;

  Double_t fXCenter;
  Double_t fYCenter;
  Double_t fZCenter;

  Double_t fEnergy;
  Double_t fTime;	 
  Double_t fCharge;

};
#endif
