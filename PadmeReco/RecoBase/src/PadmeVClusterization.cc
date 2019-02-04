#include "PadmeVClusterization.hh"
#include "TRecoVHit.hh"
#include "TRecoVCluster.hh"

#include <vector>
#include <algorithm>
#include <iostream>
//#include "TMath.h"

struct fFarHits { 
  Int_t hitIndex;
  Int_t distFromSeed;
};
struct by_distance { 
    bool operator()(fFarHits const &a, fFarHits const &b) const noexcept { 
        return a.distFromSeed < b.distFromSeed;
    }
};

void PadmeVClusterization::PrintConfig(){
  std::cout << "PadmeVClusterization-  ClusterDeltaTimeMax            " << fClusterDtMax       << " ns" << std::endl;
  std::cout << "PadmeVClusterization-  ClusterDeltaCellMax            " << fClusterDcellMax    << " " << std::endl;
  std::cout << "PadmeVClusterization-  ClusterEnergyThresholdForHit   " << fClusterEthrForHit  << " unit ?? " << std::endl;
  std::cout << "PadmeVClusterization-  ClusterEnergyThresholdForSeed  " << fClusterEthrForSeed << " unit ??" << std::endl;
  std::cout << "PadmeVClusterization-  ClusterLowerTimeThrForHit      " << fClusterTLowForHit   << " ns" << std::endl;
  std::cout << "PadmeVClusterization-  ClusterUpperTimeThrForHit      " << fClusterTHighForHit  << " ns" << std::endl;
  std::cout << "*******************************************************" <<  std::endl;

}


void PadmeVClusterization::Init(PadmeVRecoConfig *cfg)
{
  if (cfg==NULL) {std::cout<<" NULL POINTER "<<std::endl; return;}
  fClusterDtMax        = cfg->GetParOrDefault("RECOCLUSTER", "ClusterDeltaTimeMax"          , 2.);
  fClusterDcellMax     = cfg->GetParOrDefault("RECOCLUSTER", "ClusterDeltaCellMax"          , 2 );
  fClusterEthrForHit   = cfg->GetParOrDefault("RECOCLUSTER", "ClusterEnergyThresholdForHit" , 0.1);
  fClusterEthrForSeed  = cfg->GetParOrDefault("RECOCLUSTER", "ClusterEnergyThresholdForSeed", 0.5);
  fClusterTLowForHit   = cfg->GetParOrDefault("RECOCLUSTER", "ClusterTLowForHit" , -100.);
  fClusterTHighForHit  = cfg->GetParOrDefault("RECOCLUSTER", "ClusterTHighForHit",  200.);    
  std::cout << cfg->GetName() << "*******************************" <<  std::endl;
  PrintConfig();
}



std::vector<TRecoVHit *>  PadmeVClusterization::removeOutOfTimeHits(std::vector<TRecoVHit *> &Hits)
{

  std::vector<TRecoVHit *> cleanedHits;
  Double_t htime = 0.;
  Double_t henergy = 0.;
  Int_t nskip = 0;
  for (unsigned int j=0; j<Hits.size(); ++j)
    {
      //Int_t ch = Hits[j]->GetChannelId();
      htime = Hits[j]->GetTime();
      if (htime < fClusterTLowForHit) {fhUsed[j]=1;++nskip;/* continue; */}
      if (htime > fClusterTHighForHit){fhUsed[j]=1;++nskip;/* continue; */}
      henergy = Hits[j]->GetEnergy();
      if (henergy < fClusterEthrForHit) {fhUsed[j]=1;++nskip;/* continue; */}
      cleanedHits.push_back(Hits[j]);
    }
  //std::cout<<" cleanedHits size = "<<cleanedHits.size()<<" inputHits size = "<<Hits.size()<<" hits skipped = "<<nskip<<std::endl;
  return cleanedHits;
}

void PadmeVClusterization::Reconstruct(std::vector<TRecoVHit *> &inputHits, std::vector<TRecoVCluster *> &myClusters)
{

  // clear the vector keeping track of hits already used in clusters,  or out of time, or below threshold for used. 
  fhUsed.clear();
  for(unsigned int i =  0; i < inputHits.size(); ++i) fhUsed.push_back(0);
  // remove out of time hits
  //std::cout<<"PadmeVClusterization   inputHits size "<<inputHits.size()<<std::endl;
  std::vector<TRecoVHit *> Hits     = removeOutOfTimeHits(inputHits);
  //std::cout<<"PadmeVClusterization  inTimeHits size "<<Hits.size()<<std::endl;

  Double_t clE = 0.;
  Double_t clT = 0.;
  Double_t clX = 0.;
  Double_t clY = 0.;
  Double_t clZ = 0.;
  Int_t    clSize =  0;
  std::vector<Int_t> clHitIndices;
  
  Int_t    iSeed  = 0;
  Int_t    seedID = 0;
  Double_t seedT  = 0.;

  Double_t dtThr = fClusterDtMax;
  // start the loop over the unused hits looking for a new cluster seed 
  while(iSeed > -1){

    clE = 0.;
    clT = 0.;
    clX = 0.;
    clY = 0.;
    clZ = 0.;
    clSize = 0;
    iSeed  = 0;
    seedID = 0;
    seedT  = 0.;
    clSize = 0;
    clHitIndices.clear();
    
    // look for a new seed 
    iSeed = findSeed(Hits);
    if(iSeed < 0) break; 
    
    // new seed found 
    // seed properties
    seedT  = Hits[iSeed]->GetTime();    
    seedID = Hits[iSeed]->GetChannelId();

    //std::cout<<"Reconstruct --- Seed is hit n. "<<iSeed<<" @CHid "<<seedID<<std::endl;
    Double_t hitTime   = 0.;
    Double_t hitEnergy = 0.;
    Int_t    hitID     = 0 ;
    std::vector<Int_t>fDistantHits;
    fDistantHits.clear();
    std::vector<fFarHits> myDistHits;
    myDistHits.clear();

    // for this seed try and associate unused hits .... 
    for(unsigned int iHit1 =  0; iHit1 < Hits.size(); ++iHit1) {
      
      if (fhUsed[iHit1]==1) continue;
      
      hitEnergy = Hits[iHit1]->GetEnergy();
      if (hitEnergy < fClusterEthrForHit) continue;

      hitTime = Hits[iHit1]->GetTime();
      
      hitID   = Hits[iHit1]->GetChannelId();
      if( fabs( hitTime - seedT ) < dtThr ){
	Int_t dCell = IsSeedNeig( seedID, hitID);

	if ( dCell < 0 ) continue;
	if ( dCell <= 1 ){
	// the seed will be add to the cluster in this loop
	  //std::cout<<"Reconstruct --- let's add this hit n. "<<iHit1<<" @CHid "<<hitID<<" close in space to the seed"<<std::endl;
	  fhUsed[iHit1]=1;
	  
	  clE  += hitEnergy;
	  //clT  += Hits[iSeed]->GetTime();
	  clT  +=  Hits[iHit1]->GetTime()*hitEnergy;
	  clX  += (Hits[iHit1]->GetPosition().X())*hitEnergy;
	  clY  += (Hits[iHit1]->GetPosition().Y())*hitEnergy;
	  clZ  += (Hits[iHit1]->GetPosition().Z())*hitEnergy;
	  ++clSize;
	  clHitIndices.push_back(iHit1);
	}
 	else if ( dCell>1 )
	  {
 	    //fDistantHits.push_back(iHit1);
 	    //fDistFromSeed.push_back(dCell);
	    myDistHits.push_back( fFarHits() );
	    myDistHits.back().hitIndex=iHit1;
	    myDistHits.back().distFromSeed=dCell;
	  }
      }
    }
      
    if (clSize==0) break;
    std::sort(myDistHits.begin(), myDistHits.end(), by_distance());
    
    unsigned int nCloseHits=clHitIndices.size();
    Int_t startDCell=2; 
    for (unsigned int i2 =  0; i2 < myDistHits.size(); ++i2)
      {
    	Int_t iHit2         = myDistHits[i2].hitIndex;
    	Int_t iDistFromSeed = myDistHits[i2].distFromSeed;
    	if ( iDistFromSeed > startDCell )
    	  {
    	    nCloseHits = clHitIndices.size();
    	    startDCell = iDistFromSeed;
    	  }
    	if (fhUsed[iHit2]==1) continue;
    	hitEnergy=Hits[iHit2]->GetEnergy();
	
    	for (unsigned int iHit =  0; iHit < nCloseHits; ++iHit)
    	  {
    	    if ((Int_t)iHit==iSeed) continue;
    	    if (IsSeedNeig( Hits[iHit]->GetChannelId(), Hits[iHit2]->GetChannelId() ) == 1)
    	      {
		//std::cout<<"Reconstruct --- let's add this hit n. "<<iHit2<<" @CHid "<<Hits[iHit2]->GetChannelId()<<" close in space to already associated hits"<<std::endl;
    		fhUsed[iHit2]=1;
	  
    		clE  += hitEnergy;
    		//clT  += Hits[iSeed]->GetTime();
    		clT  +=  Hits[iHit2]->GetTime()*hitEnergy;
    		clX  += (Hits[iHit2]->GetPosition().X())*hitEnergy;
    		clY  += (Hits[iHit2]->GetPosition().Y())*hitEnergy;
    		clZ  += (Hits[iHit2]->GetPosition().Z())*hitEnergy;
    		++clSize;
    		clHitIndices.push_back(iHit2);
    		iHit = clHitIndices.size();
    	      }
    	  }
      }
    
    //// a good cluster was found 

    //clT=clX/(Float_t)clSize;
    clT=clT/clE;
    
    clX=clX/clE;
    clY=clY/clE;
    clZ=clZ/clE;

    TRecoVCluster* myCl = new TRecoVCluster();
    myCl->SetChannelId   ( seedID );
    myCl->SetEnergy      ( clE );
    myCl->SetTime        ( clT );
    myCl->SetPosition    ( TVector3( clX, clY, clZ) );
    myCl->SetSeed        ( iSeed );
    myCl->SetNHitsInClus ( clSize );
    myCl->SetHitVecInClus( clHitIndices );
    myClusters.push_back ( myCl );
    
  }  //end of while loop on seeds


  
  
}


Int_t PadmeVClusterization::IsSeedNeig(Int_t seedID, Int_t cellID) {

  Int_t dChIDmax=fClusterDcellMax;
  //uses cellID to find neig cells wrt seed of the cluster
  Int_t IsNeig=-1;
  //Int_t SeedRow=seedID/100;
  Int_t SeedCol=seedID;

  //Int_t CellRow=cellID/100;
  Int_t CellCol=cellID;
  //excludes the seed cell 
  if( abs(SeedCol-CellCol)<=dChIDmax) IsNeig = abs(SeedCol-CellCol); 
  //std::cout<<"PadmeVClusterization::IsSeedNeig-  seedID "<<seedID<<" cellID "<<cellID<<" Is Neig "<<IsNeig<<std::endl;
  return IsNeig;
}

Int_t PadmeVClusterization::findSeed(std::vector<TRecoVHit *> &Hits)
{
  /// find the index of the Hit with highest energy (above threshold) not yet used in a cluster

  Int_t iSeed   =-1;
  Float_t eneMax=0.;
  Float_t eneThr=fClusterEthrForSeed;
  for(unsigned int iHit =  0; iHit < Hits.size(); ++iHit) {
    if (fhUsed[iHit]==0)
      {
	Float_t energy = Hits[iHit]->GetEnergy();
	if (energy > eneThr && energy > eneMax)
	  {
	    iSeed=iHit;
	    eneMax=energy;
	  }
      }
  }
  //  if (iSeed <0 ) std::cout<<"ISeed not found ... "<<std::endl;
  //  else std::cout<<"ISeed is ... "<<iSeed<<std::endl;
  return iSeed;
}
