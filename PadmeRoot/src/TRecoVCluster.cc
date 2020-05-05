#include "TRecoVCluster.hh"

#include "Riostream.h"

ClassImp(TRecoVCluster)

TRecoVCluster::TRecoVCluster():TMCVHit()
{
  fSeed = -1;
  fNHitsInClus = 0;
  fStatus = 0;
}
