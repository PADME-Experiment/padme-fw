// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-07-21
//
// --------------------------------------------------------------
#ifndef TECalMCDigi_H
#define TECalMCDigi_H

#include "TMCVDigi.hh"

class TECalMCDigi : public TMCVDigi
{

public:

  TECalMCDigi();
  ~TECalMCDigi();

  void Print(Option_t* option="") const;

  void SetSignal (Double_t value) { fSignal = value; };
  Double_t GetSignal() const { return fSignal; };

private:

  Double_t fSignal;

  ClassDef(TECalMCDigi,1);
};
#endif
