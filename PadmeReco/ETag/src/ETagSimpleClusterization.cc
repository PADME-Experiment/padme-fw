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

struct {
    bool operator()(TRecoVHit * a, TRecoVHit * b) const {
		    return a->GetTime() < b->GetTime();
	}
} by_time;

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

  std::map<int, int> HitToClusterMapLeft = {
	{0, 0}, {1, 0}, {2, 0}, {3, 0},
	{100, 1}, {101, 1}, {102, 1}, {103, 1},
	{200, 2}, {201, 2}, {202, 2}, {203, 2},
	{300, 3}, {301, 3}, {302, 3}, {303, 3},
	{400, 4}, {401, 4}, {402, 4}, {403, 4},
	{500, 5}, {501, 5}, {502, 5}, {503, 5},
	{600, 60}, {601, 60}, {602, 60}, {603, 60},
	{700, 70}, {701, 70}, {702, 70}, {703, 70},
	{800, 80}, {801, 80}, {802, 80}, {803, 80},
	{900, 9}, {901, 9}, {902, 9}, {903, 9},
	{1000, 10}, {1001, 10}, {1002, 10}, {1003, 10},
	{1100, 11}, {1101, 11}, {1102, 11}, {1103, 11},
	{1200, 12}, {1201, 12}, {1202, 12}, {1203, 12},
	{1300, 13}, {1301, 13}, {1302, 13}, {1303, 13},
	{1400, 14}, {1401, 14}, {1402, 14}, {1403, 14},
  };
  std::map<int, int> HitToClusterMapRight = {
	{10, 0}, {11, 0}, {12, 0}, {13, 0},
	{110, 1}, {111, 1}, {112, 1}, {113, 1},
	{210, 2}, {211, 2}, {212, 2}, {213, 2},
	{310, 3}, {311, 3}, {312, 3}, {313, 3},
	{410, 4}, {411, 4}, {412, 4}, {413, 4},
	{510, 5}, {511, 5}, {512, 5}, {513, 5},
	{610, 61}, {611, 61}, {612, 61}, {613, 61},
	{710, 71}, {711, 71}, {712, 71}, {713, 71},
	{810, 81}, {811, 81}, {812, 81}, {813, 81},
	{910, 9}, {911, 9}, {912, 9}, {913, 9},
	{1010, 10}, {1011, 10}, {1012, 10}, {1013, 10},
	{1110, 11}, {1111, 11}, {1112, 11}, {1113, 11},
	{1210, 12}, {1211, 12}, {1212, 12}, {1213, 12},
	{1310, 13}, {1311, 13}, {1312, 13}, {1313, 13},
	{1410, 14}, {1411, 14}, {1412, 14}, {1413, 14},
  };
  std::map<int, std::vector<int>> clusterHitsLeftIdx;
  std::map<int, std::vector<int>> clusterHitsRightIdx;
  for (auto & item : HitToClusterMapLeft) {
	auto chnID = item.first, barID = item.second;
	clusterHitsLeftIdx[barID] = std::vector<int>{};
  }
  for (auto & item : HitToClusterMapRight) {
	auto chnID = item.first, barID = item.second;
	clusterHitsRightIdx[barID] = std::vector<int>{};
  }

  for (auto hitIdx = 0; hitIdx < Hits.size(); hitIdx++) {
	if (HitToClusterMapLeft.find(Hits[hitIdx]->GetChannelId()) != HitToClusterMapLeft.end())
		clusterHitsLeftIdx[HitToClusterMapLeft[Hits[hitIdx]->GetChannelId()]].push_back(hitIdx);
	else if (HitToClusterMapRight.find(Hits[hitIdx]->GetChannelId()) != HitToClusterMapRight.end())
		clusterHitsRightIdx[HitToClusterMapRight[Hits[hitIdx]->GetChannelId()]].push_back(hitIdx);
	else {
		std::cout << "[ETagSimpleClusterization] WARNING: Hit channel ID was not found in map! " << Hits[hitIdx]->GetChannelId() << std::endl;
	}
  }

  for (auto & item : clusterHitsLeftIdx) {
	auto barID = item.first;
	auto hitIdxs = item.second;
	if (hitIdxs.size() == 0) continue;
	// sort hits in time
	std::vector<TRecoVHit*> hits;
	for (auto & hitIdx : hitIdxs) {
		hits.push_back(Hits[hitIdx]);
	}
	std::sort(hits.begin(), hits.end(), by_time);
	clE = 0., clX = 0., clY = 0., clZ = 0., clT = 0.;
	for (auto & hitIdx : hitIdxs) {
		clE += Hits[hitIdx]->GetEnergy();
		clX += Hits[hitIdx]->GetPosition().X();
		clY += Hits[hitIdx]->GetPosition().Y();
		clZ += Hits[hitIdx]->GetPosition().Z();
    		clT += Hits[hitIdx]->GetTime();    
	}
	clX /= hitIdxs.size();
	clY /= hitIdxs.size();
	clZ /= hitIdxs.size();
	clT /= hitIdxs.size();
	TRecoVCluster* myCl = new TRecoVCluster();
	myCl->SetChannelId   ( -barID );
	myCl->SetEnergy      ( clE );
	myCl->SetTime        ( clT );
	myCl->SetPosition    ( TVector3( clX, clY, clZ) );
	myCl->SetNHitsInClus ( hitIdxs.size() );
	myCl->SetHitVecInClus( hitIdxs );
	myClusters.push_back ( myCl );
  }
  
  // for this seed try and associate unused hits .... 
  //for(unsigned int iHit =  0; iHit < Hits.size(); ++iHit) {
  //  clE     = Hits[iHit]->GetEnergy();
  //  //    if (hitEnergy < fClusterEthrForHit) continue;
  //  hitID   = Hits[iHit]->GetChannelId();
  //  clX     = Hits[iHit]->GetPosition().X();
  //  clY     = Hits[iHit]->GetPosition().Y();
  //  clZ     = Hits[iHit]->GetPosition().Z();
  //  clT     = Hits[iHit]->GetTime();    

  //  TRecoVCluster* myCl = new TRecoVCluster();
  //  myCl->SetChannelId   ( hitID );
  //  myCl->SetEnergy      ( clE );
  //  myCl->SetTime        ( clT );
  //  myCl->SetPosition    ( TVector3( clX, clY, clZ) );
  //  myCl->SetNHitsInClus ( 1. );
  //  myCl->SetHitVecInClus( clHitIndices );
  //  myClusters.push_back ( myCl );
    //    std::cout<<"Reconstruct --- New cluster stored with seed="<<clE<<" @CHid "<<hitID<<" clSize="<<1.<<std::endl;
  //}  //end of while loop on seeds
}
