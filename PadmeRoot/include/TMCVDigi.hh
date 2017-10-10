// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-07-21
//
// --------------------------------------------------------------
#ifndef TMCVDigi_H
#define TMCVDigi_H

#include "TObject.h"

class TMCVDigi : public TObject
{

public:

  TMCVDigi();
  virtual ~TMCVDigi(){};

  void Print(Option_t* option="") const;

public:
 
  Int_t    GetChannelId() const { return fChannelId; };
  Double_t GetEnergy()    const { return fEnergy;    };
  Double_t GetTime()      const { return fTime;      };

  void  SetChannelId(Int_t    value) { fChannelId = value; };
  void  SetEnergy   (Double_t value) { fEnergy = value;    };
  void  SetTime     (Double_t value) { fTime = value;      };
 
protected:

  Int_t    fChannelId;
  Double_t fEnergy;
  Double_t fTime;

  ClassDef(TMCVDigi,1);
};
#endif
