#include "TRecoVCluster.hh"

#include "Riostream.h"

ClassImp(TRecoVCluster)

TRecoVCluster::TRecoVCluster():TMCVHit()
{
  // // Initialize to invalid channel
  // fChannelId = -1;
  // fEnergy    = 0.;
  // fTime      = 1.E9;
}


// void TRecoVCluster::Print(Option_t *) const {
//   std::cout << "Hit - ChannelId = " << fChannelId
// 	    << " HitPosition = (" << fPosition.X() << "," << fPosition.Y() << "," << fPosition.Z() << ")"
// 	    << " Energy = " << fEnergy
// 	    << " Time = " << fTime << std::endl;
// }

