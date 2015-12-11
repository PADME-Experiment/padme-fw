#include "MyTrackerCluster.hh"

MyTrackerCluster::MyTrackerCluster(int l, double x, double y,double z,double ee){
  layer=l;
  pos[0] = x;
  pos[1] = y;
  pos[2] = z;
  e=ee;
}

double MyTrackerCluster::operator- (const MyTrackerCluster& cl){
  double dist = 0.;
  for(int i=0;i<3;i++){
    dist += (pos[i] - cl.pos[i])*(pos[i] - cl.pos[i]);
  }
  return dist ;
}

MyTrackerCluster& MyTrackerCluster::operator+= (const MyTrackerCluster& cl){
  for(int i=0;i<3;i++) {
    pos[i] =  (  e*pos[i] + cl.e*cl.pos[i])/ ( e + cl.e   );
  }
  e += cl.e;
  return *this;
}

MyTrackerCluster MyTrackerCluster::operator+ (const MyTrackerCluster& cl){
  MyTrackerCluster res;

  for(int i=0;i<3;i++) {
    res.pos[i] =  (  e*pos[i] + cl.e*cl.pos[i])/ ( e + cl.e   );
  }
  res.e += cl.e;
  res.layer = layer;

  return res;
}

MyTrackerCluster& MyTrackerCluster::operator= (const MyTrackerCluster& cl){
  for(int i=0;i<3;i++) {
    pos[i] = cl.pos[i];
  }
  e=cl.e;
  layer = cl.layer;
  return *this;

}

void MyTrackerCluster::clear() {
  for(int i=0;i<3;i++) {
    pos[i] = 0.;
  }
  e=0.;
  layer = 0;

}
