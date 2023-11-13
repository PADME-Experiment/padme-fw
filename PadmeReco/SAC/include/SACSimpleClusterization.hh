#ifndef SACSIMPLE_CLUSTERIZATION_H
#define SACSIMPLE_CLUSTERIZATION_H
#include "PadmeVClusterization.hh"


class SACSimpleClusterization : public PadmeVClusterization{
public:
  virtual Int_t IsSeedNeig(Int_t seedID, Int_t cellID);  
};
#endif

