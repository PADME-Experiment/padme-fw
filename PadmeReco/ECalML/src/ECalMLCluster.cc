#include "ECalMLCluster.hh"
#include "ECalMLCrystal.hh"

#include <iostream>
#include "cstdlib"
#include "math.h"

ECalMLCluster::ECalMLCluster()
{

  fSeed = -1;
  fRawEnergy=0;
  fEnergy=-1;
  fTime=-1;
  fXCenter  =-99.;	 
  fYCenter  =-99.;	
  fNCrystals=  0;

}

ECalMLCluster::~ECalMLCluster()
{;}

Int_t ECalMLCluster::AddCrystal(ECalMLCrystal* crystal)
{
  if (crystal->IsUsed()) {
    std::cout << "ERROR - ECalMLCluster::AddCrystal - Try to add already USED crystal" << std::endl;
    return -1;
  }
  if(fNCrystals<CLUSTER_N_MAX_CRYSTALS_IN_CLUSTER){
    fCrystalList[fNCrystals]=crystal;
    fNCrystals++;
    crystal->SetUsed();
    if (fNCrystals==1) fTime = 0.;
    fTime = fTime*fEnergy + crystal->GetTime()*crystal->GetEnergy();
    fXCenter = fXCenter*fEnergy + crystal->GetEnergy()*crystal->GetXCenter();
    fYCenter = fYCenter*fEnergy + crystal->GetEnergy()*crystal->GetYCenter();
    fEnergy += crystal->GetEnergy();
    fRawEnergy += crystal->GetEnergy();
    
    fTime = fTime/fEnergy;
    fXCenter = fXCenter/fEnergy;
    fYCenter = fYCenter/fEnergy;

    return fNCrystals;
  } else {
    std::cout << "WARNING - ECalMLCluster::AddCrystal - Too many crystals in cluster: " << fNCrystals << std::endl;
    return fNCrystals;
  }
}

void ECalMLCluster::Print()
{
  std::cout << "ECalML Cluster - X " << fXCenter << " Y " << fYCenter << " Energy " << fEnergy
	    << " Raw Energy " << fRawEnergy << " Time " << fTime << " NCry " << fNCrystals << std::endl;
}

Double_t ECalMLCluster::ComputeTime()
{
  std::cout << "WARNING - ECalMLCluster::ComputeTime - Not implemented" << std::endl;
  return 0.;
}

Double_t ECalMLCluster::ComputeEnergy()
{
  std::cout << "WARNING - ECalMLCluster::ComputeEnergy - Not implemented" << std::endl;
  return 0.;
}

Double_t ECalMLCluster::ComputeCenter()
{
  std::cout << "WARNING - ECalMLCluster::ComputeCenter - Not implemented" << std::endl;
  return 0.;
}
