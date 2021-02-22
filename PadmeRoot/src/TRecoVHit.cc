#include "TRecoVHit.hh"
#include "TMCVDigi.hh"

#include "Riostream.h"

ClassImp(TRecoVHit)

TRecoVHit::TRecoVHit():TMCVHit()
{
  fBDCHid = 100000000;
  fStatus = 0;
}
