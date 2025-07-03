#include "Riostream.h"
#include "TRecoVHit.hh"
#include "TRecoVCluster.hh"
#include "MMClusterization.hh"

void MMClusterization::PrintConfig(){
  PadmeVClusterization::PrintConfig();
}

void MMClusterization::Reconstruct(std::vector<TRecoVHit *> &inputHits, std::vector<TRecoVCluster *> &myClusters){
  // ASSUME HITS COME SORTED IN (BOARDSN, STRIP) with the priority given
  //  std::cout << "*********** Entering the correct Reconstruct method " << std::endl;
  myClusters.clear();

  // clear the vector keeping track of hits already used in clusters,  or out of time, or below threshold for used. 
  fhUsed.clear();
  for(unsigned int i =  0; i < inputHits.size(); ++i) fhUsed.push_back(0);

  // temporary vector with hit indices for a given cluster
  std::vector<Int_t> clHitIndices;
  // temporary count of number of jumps in the cluster
  ULong64_t njumps = 0;
  
  for(unsigned int iHit =  0; iHit < inputHits.size(); ++iHit) {
    TRecoVHit* hit = inputHits[iHit];
    if (hit->GetEnergy() < fClusterEthrForSeed) continue;
      
    int bdsn  = (hit->GetChannelId() & 0xF00 ) >> 8; // board SN 0-15
    int strip = (hit->GetChannelId() & 0x0FF); // strip 0-255
    if (clHitIndices.size()) {
      TRecoVHit* lastHitInClu = inputHits[clHitIndices.back()];
      int bdsnoflastHitInClu = (lastHitInClu->GetChannelId() & 0xF00 ) >> 8; // board SN 0-15
      int stripoflastHitInClu = (lastHitInClu->GetChannelId() & 0x0FF ); // strip 0-255
      //      std::cout << " Condition of being in the cluster # " << myClusters.size() << " bdsn " << bdsn << " strip " << strip << " bdold " << bdsnoflastHitInClu << " stripold " << stripoflastHitInClu << " " << fClusterDcellMax << std::endl;
      if ((bdsnoflastHitInClu != bdsn) || (strip - stripoflastHitInClu > fClusterDcellMax)) { // board changed || hits are too distant
	// close old cluster
	TRecoVCluster* newclu = new TRecoVCluster();
	newclu->SetNHitsInClus(clHitIndices.size());
	newclu->SetHitVecInClus(clHitIndices);	  
	newclu->setStatus(njumps);
	myClusters.push_back(newclu);

	// start filling new hit-indices array
	clHitIndices.clear();
	njumps = 0;
	clHitIndices.push_back(iHit);
	fhUsed[iHit] = 1;	
      } else {
	njumps += stripoflastHitInClu - strip - 1;
	clHitIndices.push_back(iHit);
	fhUsed[iHit] = 1;	
      }
    } else {
      clHitIndices.push_back(iHit);
      njumps = 0;
      fhUsed[iHit] = 1;	
      
    }
  }

  // close last cluster
  if (clHitIndices.size()) {
    TRecoVCluster* newclu = new TRecoVCluster();
    newclu->SetNHitsInClus(clHitIndices.size());
    newclu->SetHitVecInClus(clHitIndices);	  
    newclu->setStatus(njumps);
    myClusters.push_back(newclu);
  }
  clHitIndices.clear();

  //  std::cout << "*********** At the end of the Reconstruct method, from " << inputHits.size() << " hits, you have  " << myClusters.size() << " clusters " << std::endl;
  
}

