#include "ECalCluster.hh"
#include "ECalCrystal.hh"

#include <iostream>
#include "cstdlib"
#include "math.h"

ECalCluster::ECalCluster()
{

  fRawEnergy=0;
  fEnergy=-1;
  fTime=-1;
  fXCenter  =-99.;	 
  fYCenter  =-99.;	
  fNCrystals=  0;

}

ECalCluster::~ECalCluster()
{;}

Int_t ECalCluster::AddCrystal(ECalCrystal* crystal)
{
  if (crystal->IsUsed()) {
    std::cout << "ERROR - ECalCluster::AddCrystal - Try to add already USED crystal" << std::endl;
    return -1;
  }
  if(fNCrystals<CLUSTER_N_MAX_CRYSTALS_IN_CLUSTER){
    fCrystalList[fNCrystals]=crystal;
    fNCrystals++;
    crystal->SetUsed();
    fRawEnergy += crystal->GetEnergy();
    return fNCrystals;
  } else {
    std::cout << "WARNING - ECalCluster::AddCrystal - Too many crystals in cluster: " << fNCrystals << std::endl;
    return fNCrystals;
  }
}

void ECalCluster::Print()
{
  std::cout << "ECal Cluster - X " << fXCenter << " Y " << fYCenter << " Energy " << fEnergy
	    << " Raw Energy " << fRawEnergy << " Time " << fTime << " NCry " << fNCrystals << std::endl;
}

Double_t ECalCluster::ComputeTime()
{
  std::cout << "WARNING - ECalCluster::ComputeTime - Not implemented" << std::endl;
  return 0.;
}

Double_t ECalCluster::ComputeEnergy()
{
  std::cout << "WARNING - ECalCluster::ComputeEnergy - Not implemented" << std::endl;
  return 0.;
}

Double_t ECalCluster::ComputeCenter()
{
  std::cout << "WARNING - ECalCluster::ComputeCenter - Not implemented" << std::endl;
  return 0.;
}
