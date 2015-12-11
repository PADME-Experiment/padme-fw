#ifndef _MY_CALO_CLUSTER_
#define _MY_CALO_CLUSTER_

#include <vector>


class MyCaloCluster {
public:
  double pos[3];
  double energy;
  int ncells;
  double rmsx;
  double rmsy;
  double theta;
};
#endif
