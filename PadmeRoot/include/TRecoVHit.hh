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
  
private:
  unsigned int fBDCHid; 

public:
 
  ClassDef(TRecoVHit,1);
};
#endif
