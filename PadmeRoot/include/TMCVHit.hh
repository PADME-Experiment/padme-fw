// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------
#ifndef TMCVHit_H
#define TMCVHit_H

#include "TObject.h"
#include "TVector3.h"

class TMCVHit : public TObject
{

public:

  TMCVHit();
  virtual ~TMCVHit(){};

  void Print(Option_t* option="") const;

public:
 
  Int_t    GetChannelId() const { return fChannelId; };
  TVector3 GetPosition()  const { return fPosition;  };
  Double_t GetEnergy()    const { return fEnergy;    };
  Double_t GetTime()      const { return fTime;      };

  void  SetChannelId(Int_t    value) { fChannelId = value; };
  void  SetPosition (TVector3 value) { fPosition = value;  };
  void  SetEnergy   (Double_t value) { fEnergy = value;    };
  void  AddEnergy   (Double_t value) { fEnergy += value;   };
  void  SetTime     (Double_t value) { fTime = value;      };
 
protected:

  Int_t    fChannelId;
  TVector3 fPosition;
  Double_t fEnergy;
  Double_t fTime;

  ClassDef(TMCVHit,1);
};
#endif
