// --------------------------------------------------------------
#ifndef TRecoVHit_H
#define TRecoVHit_H

#include "TObject.h"
#include "TVector3.h"
#include "TMCVHit.hh"

class TMCVDigi;

class TRecoVHit : public TMCVHit
{

public:

  TRecoVHit();
  //TRecoVHit(const TMCVDigi&);


  virtual ~TRecoVHit(){};

  void setBDCHid(unsigned int BDid, unsigned int CHid){fBDCHid = 100*BDid+CHid;}
  unsigned int getBDCHid(){return fBDCHid;}
  unsigned int getBDid(){return (unsigned int)(fBDCHid/100);}
  unsigned int getCHid(){return fBDCHid%100;}
  
  //Beth 8/5/2019
  Double_t GetPed()       const { return fPed;       };
  Double_t GetCharge()    const { return fCharge;    };
  Double_t GetAmplitude() const { return fAmplitude; };

  //Beth 14/5/19
  Double_t* GetWaveform()       { return fWaveform;} ;

  //Beth 8/5/19
  void SetPed       (Double_t value) { fPed = value;       };
  void SetCharge    (Double_t value) { fCharge = value;    };
  void SetAmplitude (Double_t value) { fAmplitude = value; };

  //Beth 14/5/19
  void SetWaveform  (Int_t index, Double_t value)    { fWaveform[index] = value;};

private:
  unsigned int fBDCHid;

  //Beth 8/5/2019
  Double_t fPed;
  Double_t fCharge;
  Double_t fAmplitude;

  //Beth 14/5/19
  Double_t fWaveform[1024];

public:
 
  ClassDef(TRecoVHit,1);
};
#endif
