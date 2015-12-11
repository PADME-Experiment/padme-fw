#ifndef _MY_TRACKER_CLUSTER_
#define _MY_TRACKER_CLUSTER_

#include <vector>

class MyTrackerCluster{
public:				      
  MyTrackerCluster(){clear();};
  MyTrackerCluster(int layer, double x, double y,double z,double e);
  MyTrackerCluster  operator+ (const MyTrackerCluster& cl);
  MyTrackerCluster&  operator+= (const MyTrackerCluster& cl);
  double  operator- (const MyTrackerCluster& cl);
  //double  dist(const MyTrackerCluster& cl);
  MyTrackerCluster& operator= (const MyTrackerCluster& part);
  void clear();
  
  

  
public:
  double pos[3];
  double e;
  int layer;

  

};
#endif
