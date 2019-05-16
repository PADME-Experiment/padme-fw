#include "TRecoVHit.hh"
#include "TMCVDigi.hh"

#include "Riostream.h"

ClassImp(TRecoVHit)

TRecoVHit::TRecoVHit():TMCVHit()
{
  fBDCHid = 100000000;
  // // Initialize to invalid channel
  // fChannelId = -1;
  // fEnergy    = 0.;
  // fTime      = 1.E9;
}

/*
TRecoVHit::TRecoVHit(const TMCVDigi& dig)
{
  fBDCHid = 100000000;
  fChannelId = dig.GetChannelId();
  fEnergy    = dig.GetEnergy();
  fTime      = dig.GetTime();
}
*/
// void TRecoVHit::Print(Option_t *) const {
//   std::cout << "Hit - ChannelId = " << fChannelId
// 	    << " HitPosition = (" << fPosition.X() << "," << fPosition.Y() << "," << fPosition.Z() << ")"
// 	    << " Energy = " << fEnergy
// 	    << " Time = " << fTime << std::endl;
// }

