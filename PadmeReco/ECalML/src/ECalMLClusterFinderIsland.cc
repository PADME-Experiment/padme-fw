#include "ECalMLClusterFinderIsland.hh"

#include "ECalMLParameters.hh"

#include <iostream>

ECalMLClusterFinderIsland::ECalMLClusterFinderIsland(ECalMLCrystalHandler* cryH,ECalMLClusterHandler* cluH)
{

  fCrystalHandler = cryH;
  fClusterHandler = cluH;

  // Get thresholds from ECalMLParameters
  fEThreshold = ECalMLParameters::GetInstance()->GetIslandEThreshold();
  fEThresholdSeed = ECalMLParameters::GetInstance()->GetIslandEThresholdSeed();

}

ECalMLClusterFinderIsland::~ECalMLClusterFinderIsland()
{;}

Int_t ECalMLClusterFinderIsland::FindClusters()
{

  Int_t nClusters = 0;

  // Clear results of (possible) previous cluster findings
  fCrystalHandler->ResetCrystals();
  fClusterHandler->ResetClusters();

  while ( ECalMLCrystal* seed = fCrystalHandler->GetSeed(fEThresholdSeed) ) {
    if ( ECalMLCluster* cluster = fClusterHandler->NewCluster() ) {
      nClusters++;
      cluster->AddCrystal(seed);
      cluster->SetSeed(seed->GetHitIndex());
      seed->SetSeed();
      ExpandCluster(cluster,seed);
    } else {
      std::cout << "WARNING - ClusterFinderIsland::FindClusters - Cannot create new cluster" << std::endl;
      break;
    }
  }

  return nClusters;

}

void ECalMLClusterFinderIsland::ExpandCluster(ECalMLCluster* clu,ECalMLCrystal* cry)
{
  // Get position of crystal
  Int_t cx = cry->GetXi();
  Int_t cy = cry->GetYi();

  // Loop over 4 adjacent neighbours
  Int_t dx,dy;
  for(Int_t i=0; i<4; i++){

    switch(i)
      {
      case 0: // Right
	dx =  0; dy =  1; break;
      case 1: // Up
	dx =  1; dy =  0; break;
      case 2: // Left
	dx =  0; dy = -1; break;
      case 3: // Down
	dx = -1; dy =  0; break;
      default:
	dx =  0; dy =  0;
	std::cout << "???????? ClusterFinder::ExpandCluster ??????????" << std::endl;
      }

    // Get address of neighbour crystal
    ECalMLCrystal* cry_nbr = fCrystalHandler->GetCrystal(cx+dx,cy+dy);

    // If neighbour crystal...
    if (
	cry_nbr &&                                // a) exists
	!cry_nbr->IsUsed() &&                     // b) is not used
	(cry_nbr->GetEnergy() > fEThreshold) &&   // c) has enough energy
	(cry_nbr->GetEnergy() < cry->GetEnergy()) // d) has less energy than current crystal
       ) {
      clu->AddCrystal(cry_nbr);                   // 1) add it to current cluster
      ExpandCluster(clu,cry_nbr);                 // 2) expand current cluster around it
    }
    
  }

}
