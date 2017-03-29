#include "ECalClusterFinderRadius.hh"

#include <iostream>

#include "ECalParameters.hh"

ECalClusterFinderRadius::ECalClusterFinderRadius(ECalCrystalHandler* cryH,ECalClusterHandler* cluH)
{

  // Save crystal and cluster handlers
  fCrystalHandler = cryH;
  fClusterHandler = cluH;

  ECalParameters* para = ECalParameters::GetInstance();

  // Get thresholds from ECalParameters
  fEThreshold = para->GetRadiusEThreshold();
  fEThresholdSeed = para->GetRadiusEThresholdSeed();

  // Get size of crystals from ECalParameters
  fCryXSize = para->GetCrystalSizeX()+para->GetCrystalGap();
  fCryYSize = para->GetCrystalSizeY()+para->GetCrystalGap();

  // Set radius to its default value
  fNNeighbor = 0;
  SetRadius(para->GetRadiusRadius());

}

ECalClusterFinderRadius::~ECalClusterFinderRadius()
{;}

Int_t ECalClusterFinderRadius::FindClusters()
{

  Int_t nClusters = 0;

  // Clear results of (possible) previous cluster findings
  fCrystalHandler->ResetCrystals();
  fClusterHandler->ResetClusters();

  while ( ECalCrystal* seed = fCrystalHandler->GetSeed(fEThresholdSeed) ) {
    if ( ECalCluster* cluster = fClusterHandler->NewCluster() ) {
      nClusters++;
      cluster->AddCrystal(seed);
      seed->SetSeed();
      ExpandCluster(cluster,seed);
    } else {
      std::cout << "WARNING - ClusterFinder::FindClusters - Cannot create new cluster" << std::endl;
      break;
    }
  }

  return nClusters;

}

void ECalClusterFinderRadius::ExpandCluster(ECalCluster* clu,ECalCrystal* cry)
{

  // Get position of crystal
  Int_t cx = cry->GetXi();
  Int_t cy = cry->GetYi();

  // Loop over all neighbors within the box radius
  for(Int_t in=0;in<fNNeighbor;in++){

    // Get address of neighbour crystal
    ECalCrystal* cry_nbr = fCrystalHandler->GetCrystal(cx+fXNeighbor[in],cy+fYNeighbor[in]);

    // If neighbour crystal...
    if (
	cry_nbr &&                           // a) exists
	!cry_nbr->IsUsed() &&                // b) is not used
	(cry_nbr->GetEnergy() > fEThreshold) // c) has enough energy
       ) clu->AddCrystal(cry_nbr);           // ...add it to current cluster

  }

}

// Change radius used in the algorithm and update all related quantities
void ECalClusterFinderRadius::SetRadius(Double_t radius)
{

  fRadius = radius;
  Double_t radius2 = fRadius*fRadius;

  // Get x and y range around seed crystal
  Int_t cryXRange = (Int_t)(fRadius/fCryXSize);
  Int_t cryYRange = (Int_t)(fRadius/fCryYSize);

  // Create map of crystals within the given radius from a generic crystal center
  fNNeighbor = 0;
  for(Int_t dx=-cryXRange;dx<=cryXRange;dx++){
    for(Int_t dy=-cryYRange;dy<=cryYRange;dy++){

      if (dx==0 && dy==0) continue; // Central crystal is not a neighbor

      // See if crystal center is within box radius from seed
      Double_t r2 = (dx*fCryXSize)*(dx*fCryXSize)+(dy*fCryYSize)*(dy*fCryYSize);
      if (r2<=radius2){
	if (fNNeighbor<CLUSTERFINDERRADIUS_NEIGHBORS_MAX){
	  fXNeighbor[fNNeighbor] = dx;
	  fYNeighbor[fNNeighbor] = dy;
	  fNNeighbor++;
	} else {
	  std::cout << "ERROR ClusterFinderRadius: more than " << CLUSTERFINDERRADIUS_NEIGHBORS_MAX
		    << " neighbors found" << std::endl;
	}
      }

    }
  }

  // Show neighbor map (mostly for debugging, can be disabled)
  //PrintNeighborMap();

}

// Show map of neighbor crystals
void ECalClusterFinderRadius::PrintNeighborMap()
{
  std::cout << "ClusterFinderRadius - Radius " << fRadius << " mm - Neighbors " << fNNeighbor << std::endl;
  for (Int_t in=0;in<fNNeighbor;in++) {
    std::cout << in << " " << fXNeighbor[in] << " " << fYNeighbor[in] << std::endl;
  }
}
