#include "ECalClusterFinderBox.hh"

#include <iostream>

#include "ECalParameters.hh"

ECalClusterFinderBox::ECalClusterFinderBox(ECalCrystalHandler* cryH,ECalClusterHandler* cluH)
{

  // Save crystal and cluster handlers
  fCrystalHandler = cryH;
  fClusterHandler = cluH;

  // Set defaul energy thresholds for crystals and seeds. Currently both at 0.
  // Change this using SetEThreshold and SetEThresholdSeed
  fEThreshold = CLUSTERFINDERBOX_DEFAULT_ETHRESHOLD;
  fEThresholdSeed = CLUSTERFINDERBOX_DEFAULT_ETHRESHOLDSEED;

  // Get size of crystals from geometry
  ECalParameters* para = ECalParameters::GetInstance();
  fCryXSize = para->GetCryXSize();
  fCryYSize = para->GetCryYSize();
  fCryXSize2 = fCryXSize*fCryXSize;
  fCryYSize2 = fCryYSize*fCryYSize;

  // Set radius to its default value
  fNNeighbor = 0;
  SetBoxRadius(CLUSTERFINDERBOX_DEFAULT_RADIUS);

  // Show neighbor map (mostly for debugging, can be disabled)
  PrintNeighborMap();

}

ECalClusterFinderBox::~ECalClusterFinderBox()
{;}

Int_t ECalClusterFinderBox::FindClusters()
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

void ECalClusterFinderBox::ExpandCluster(ECalCluster* clu,ECalCrystal* cry)
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
void ECalClusterFinderBox::SetBoxRadius(Double_t boxRad)
{

  fBoxRadius = boxRad;
  Double_t boxRadius2 = boxRad*boxRad;

  // Get x and y range around seed crystal
  Int_t cryXRange = (Int_t)(fBoxRadius/fCryXSize);
  Int_t cryYRange = (Int_t)(fBoxRadius/fCryYSize);

  // Create map of crystals within the given radius from a generic crystal center
  fNNeighbor = 0;
  for(Int_t dx=-cryXRange;dx<=cryXRange;dx++){
    for(Int_t dy=-cryYRange;dy<=cryYRange;dy++){

      if (dx==0 && dy==0) continue; // Central crystal is not a neighbor

      // See if crystal center is within box radius from seed
      Double_t r2 = fCryXSize2*(Double_t)(dx*dx)+fCryYSize2*(Double_t)(dy*dy);
      if (r2<=boxRadius2){
	if (fNNeighbor<CLUSTERFINDERBOX_NEIGHBORS_MAX){
	  fXNeighbor[fNNeighbor] = dx;
	  fYNeighbor[fNNeighbor] = dy;
	  fNNeighbor++;
	} else {
	  std::cout << "ERROR ClusterFinderBox: more than " << CLUSTERFINDERBOX_NEIGHBORS_MAX
		    << " neighbors found" << std::endl;
	}
      }

    }
  }

}

// Show map of neighbor crystals
void ECalClusterFinderBox::PrintNeighborMap()
{
  //  std::cout<<"ClusterFinderBox - Radius "<<fBoxRadius<<" cm - Neighbors "<<fNNeighbor<<std::endl;
  for(Int_t in=0;in<fNNeighbor;in++){
    //    std::cout<<in<<" "<<fXNeighbor[in]<<" "<<fYNeighbor[in]<<std::endl;
  }
}
