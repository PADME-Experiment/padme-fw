// --------------------------------------------------------------
// Dummy cluster algorithm for the ETag Clusters:
//
// Created by Mauro Raggi (mauro.raggi@roma1.infn.it) 2022-06-16
//
// --------------------------------------------------------------

#include "ETagSimpleClusterization.hh"
#include "TRecoVHit.hh"
#include "TRecoVCluster.hh"

#include <vector>
#include <algorithm>
#include <iostream>
//#include "TMath.h"

void ETagSimpleClusterization::PrintConfig(){
  std::cout << "EtagClusterization-  ClusterDeltaTimeMax              " << fClusterDtMax       << " ns" << std::endl;
  std::cout << "PadmeVClusterization-  ClusterDeltaCellMax            " << fClusterDcellMax    << " " << std::endl;
  std::cout << "PadmeVClusterization-  ClusterEnergyThresholdForHit   " << fClusterEthrForHit  << " unit ?? " << std::endl;
  //  std::cout << "PadmeVClusterization-  ClusterEnergyThresholdForSeed  " << fClusterEthrForSeed << " unit ??" << std::endl;
  std::cout << "PadmeVClusterization-  ClusterLowerTimeThrForHit      " << fClusterTLowForHit   << " ns" << std::endl;
  std::cout << "PadmeVClusterization-  ClusterUpperTimeThrForHit      " << fClusterTHighForHit  << " ns" << std::endl;
  std::cout << "*******************************************************" <<  std::endl;

}


void ETagSimpleClusterization::Init(PadmeVRecoConfig *cfg)
{
  if (cfg==NULL) {std::cout<<" NULL POINTER "<<std::endl; return;}
  fClusterDtMax        = cfg->GetParOrDefault("RECOCLUSTER", "ClusterDeltaTimeMax"          , 2.);
  fClusterDcellMax     = cfg->GetParOrDefault("RECOCLUSTER", "ClusterDeltaCellMax"          , 2 );
  fClusterEthrForHit   = cfg->GetParOrDefault("RECOCLUSTER", "ClusterEnergyThresholdForHit" , 0.1);
  //  fClusterEthrForSeed  = cfg->GetParOrDefault("RECOCLUSTER", "ClusterEnergyThresholdForSeed", 0.5);
  fClusterTLowForHit   = cfg->GetParOrDefault("RECOCLUSTER", "ClusterTLowForHit" , -100.);
  fClusterTHighForHit  = cfg->GetParOrDefault("RECOCLUSTER", "ClusterTHighForHit",  200.);    
  std::cout << cfg->GetName() << "*******************************" <<  std::endl;
  PrintConfig();
}

//void ETagSimplelusterization::Reconstruct(std::vector<TRecoVHit *> &inputHits, std::vector<TRecoVCluster *> &myClusters)
void ETagSimpleClusterization::Reconstruct(std::vector<TRecoVHit *> &Hits, std::vector<TRecoVCluster *> &myClusters)
{
  // clear the vector keeping track of hits already used in clusters,  or out of time, or below threshold for used. 
  // remove out of time hits
  //  std::cout<<"PadmeVClusterization   inputHits size "<<Hits.size()<<std::endl;
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
  
  // std::cout<<"PadmeVClusterization::Reconstruct --------------------------------------------------------------------- nHits = "<<inputHits.size()<<";  in time "<<Hits.size()<<std::endl; 
  //  seedT  = Hits[iSeed]->GetTime();    
  //  seedID = Hits[iSeed]->GetChannelId();
  
  Double_t hitTime   = 0.;
  Double_t hitEnergy = 0.;
  Int_t    hitID     = 0 ;
  
  // for this seed try and associate unused hits .... 
  for(unsigned int iHit =  0; iHit < Hits.size(); ++iHit) {
    clE     = Hits[iHit]->GetEnergy();
    //    if (hitEnergy < fClusterEthrForHit) continue;
    hitID   = Hits[iHit]->GetChannelId();
    clX     = Hits[iHit]->GetPosition().X();
    clY     = Hits[iHit]->GetPosition().Y();
    clZ     = Hits[iHit]->GetPosition().Z();
    clT     = Hits[iHit]->GetTime();    

    TRecoVCluster* myCl = new TRecoVCluster();
    myCl->SetChannelId   ( hitID );
    myCl->SetEnergy      ( clE );
    myCl->SetTime        ( clT );
    myCl->SetPosition    ( TVector3( clX, clY, clZ) );
    myCl->SetNHitsInClus ( 1. );
    myCl->SetHitVecInClus( clHitIndices );
    myClusters.push_back ( myCl );
    //    std::cout<<"Reconstruct --- New cluster stored with seed="<<clE<<" @CHid "<<hitID<<" clSize="<<1.<<std::endl;
  }  //end of while loop on seeds
}
