#ifndef ECALSIMPLE_CLUSTERIZATION_H
#define ECALSIMPLE_CLUSTERIZATION_H
#include "PadmeVClusterization.hh"

class ECalMLSimpleClusterization : public PadmeVClusterization{
public:

  virtual Int_t IsSeedNeig(Int_t seedID, Int_t cellID);  

};
#endif

