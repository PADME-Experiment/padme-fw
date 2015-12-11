#include "MySimEvent.hh"


MySimEvent::MySimEvent(){
  ClearEvent();
}


void MySimEvent::ClearEvent() {
  ETotCal  = 0;
  for(int i=0;i<1000;i++) {
    ecry[i] = 0;
  }

  
  particles.clear();
  clusters.clear();
  trClus.clear();
}


void MySimEvent::SetCrystalEnergies(double *en, int n){
  if(n>1000) {
    std::cout << "Event with more than 1000 cells: storing only 1000 energies" << std::endl;
    n=1000;
  }
  for(int i=0;i<n;i++) {
    ecry[i] = en[i];
  }
}

void MySimEvent::AddCluster(double x, double y,double e){
  MyCaloCluster clus;
  clus.pos[0] = x;
  clus.pos[1] = y;
  clus.energy = e;
  clusters.push_back(clus);
}

void MySimEvent::AddTrHit(int l, double x, double y,double z,double e,double cut){
  //Checking whether to add this hit
  //create te tracker hit

  MyTrackerCluster clus(l,x,y,z,e);
  
  //  std::cout <<"Processing tracker hit with  " << clus.pos[0] << " " << clus.pos[1] << " " << clus.pos[2] << "  E = " << clus.e << "  in l=" << clus.layer<< std::endl;
  
  bool add=true;
  
  //Compare it to the rest in the cluster vector
  for(std::vector<MyTrackerCluster>::iterator  it = trClus.begin(); it != trClus.end();it++) {
    if ( clus - *it  < cut
	 //TrClsDist  
	 ) {
      *it += clus;
      add=false;
    }
  }
  if(add==true && trClus.size()< 999   ) {
    trClus.push_back(clus);
  }
}
