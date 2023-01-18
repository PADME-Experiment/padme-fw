// --------------------------------------------------------------
#ifndef TRecoVHit_H
#define TRecoVHit_H

#include "TObject.h"
//#include "TVector3.h"
#include "TMCVHit.hh"

class TMCVDigi;

class TRecoVHit : public TMCVHit
{

public:

  TRecoVHit();

  virtual ~TRecoVHit(){};

  void setBDCHid(UInt_t BDid, UInt_t CHid) { fBDCHid = 100*BDid+CHid; }
  UInt_t getBDCHid() { return fBDCHid; }
  UInt_t getBDid() { return (UInt_t)(fBDCHid/100); }
  UInt_t getCHid() { return fBDCHid%100; }

  void setStatus(ULong64_t status) { fStatus = status; }
  ULong64_t getStatus() { return fStatus; }

private:
  UInt_t fBDCHid;
  ULong64_t fStatus;

public:
 
  ClassDef(TRecoVHit,2);
};
#endif
