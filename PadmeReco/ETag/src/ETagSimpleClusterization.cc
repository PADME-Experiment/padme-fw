// --------------------------------------------------------------
// Simple cluster algorithm for the ETag Clusters:
//
// Created by Andre Frankenthal (a.franken@cern.ch) 2022-07-22
//
// --------------------------------------------------------------

#include "ETagSimpleClusterization.hh"
#include "TRecoVHit.hh"
#include "TRecoVCluster.hh"

#include <vector>
#include <algorithm>
#include <iostream>
#include "TDirectory.h"
//#include "TMath.h"
using std::vector;
using std::map;

struct {
    bool operator()(TRecoVHit * a, TRecoVHit * b) const {
		    return a->GetTime() < b->GetTime();
	}
} by_time;


void ETagSimpleClusterization::AddHisto(std::string name, TH1 *histo){
  fHistoMap[name] = histo;
}

void ETagSimpleClusterization::PrintConfig(){
  std::cout << "EtagClusterization-  ClusterDeltaTimeMax              " << fClusterDtMax       << " ns" << std::endl;
  std::cout << "PadmeVClusterization-  ClusterDeltaCellMax            " << fClusterDcellMax    << " " << std::endl;
  std::cout << "PadmeVClusterization-  ClusterEnergyThresholdForHit   " << fClusterEthrForHit  << " unit ?? " << std::endl;
  //  std::cout << "PadmeVClusterization-  ClusterEnergyThresholdForSeed  " << fClusterEthrForSeed << " unit ??" << std::endl;
  std::cout << "PadmeVClusterization-  ClusterLowerTimeThrForHit      " << fClusterTLowForHit   << " ns" << std::endl;
  std::cout << "PadmeVClusterization-  ClusterUpperTimeThrForHit      " << fClusterTHighForHit  << " ns" << std::endl;
  std::cout << "*******************************************************" <<  std::endl;

}

void ETagSimpleClusterization::HistoInit() {
	fHistoFile = new TFile("ETagClusterizationHists.root", "RECREATE");
	AddHisto("hInClusterTimeDiffs", new TH1F("hInClusterTimeDiffs", "In-cluster time differences (ns)", 100, -10., 10.));
}

TH1*  ETagSimpleClusterization::GetHisto(std::string name){
  return fHistoMap[name];
}


void ETagSimpleClusterization::HistoExit(){
 
  std::cout << "Entering HistoExit for ETagSimpleClusterization" << std::endl;
  
  if(fHistoFile == 0) 
    return;

  fHistoFile->cd();
  
  std::map<std::string, TH1*>::iterator itr;
  for(itr = fHistoMap.begin(); itr != fHistoMap.end(); itr++)
    {
      TH1* histo = itr->second;
      histo->Write();
      delete histo;
    }
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
  
  HistoInit();
}


vector<vector<TRecoVHit*>> ETagSimpleClusterization::FindClustersByTimeSpread(map<int, vector<TRecoVHit*>> hitClusterMap) {
	vector<vector<TRecoVHit*>> clusters;
	int nNonEmptyChannels = 0;
	for (auto & map : hitClusterMap)
		nNonEmptyChannels += (map.second.size() > 0 ? 1 : 0);
	// require at least 3 hits per cluster
	while (nNonEmptyChannels > 2) {
		map<TRecoVHit*, float> timeDiffs, timeAvgs;
		map<TRecoVHit*, vector<TRecoVHit*>> usedHits;
		bool firstChannel = true;
		int nHits = 0;
		// take all the hits in a given bar and split into the 4 SiPM channels
		for (auto & map : hitClusterMap) {
			auto chId = map.first;
			auto hits = map.second;
			if (hits.empty())
				continue;
			// if it's first SiPM channel, initialize
			if (firstChannel) {
				firstChannel = false;
				for (auto & hit : hits) {
					timeDiffs[hit] = 0.;
					timeAvgs[hit] = hit->GetTime();
					usedHits[hit] = vector<TRecoVHit*>{hit};
					nHits++;
				}
			}
			// else, compute the new minimum time spread after adding 2nd, 3rd, and 4th channels
			// this is a greedy algorithm which does not look at all possible solutions by "brute force";
			// I can't prove it now, but I'm pretty sure this greedy algo optimizes the problem
			else {
				for (auto & timeDiff : timeDiffs) {
					auto hitRef = timeDiff.first;
					auto td = timeDiff.second;
					float newTimeDiff = 1e6, newTimeAvg = 0.;
					TRecoVHit* usedHit;
					for (auto & hit : hits) {
						if (abs(timeAvgs[hitRef] - hit->GetTime()) < newTimeDiff) {
							newTimeDiff = (timeAvgs[hitRef] - hit->GetTime()) * (nHits+1);
							newTimeAvg = (timeAvgs[hitRef] * nHits + hit->GetTime()) / (nHits+1);
							usedHit = hit;
						}
					}
					timeDiffs[hitRef] = newTimeDiff;
					timeAvgs[hitRef] = newTimeAvg;
					usedHits[hitRef].push_back(usedHit);
					nHits++;
				}
			}
		}
		// for each sequence of hits starting with hits in the first channel,
		// find the one that minimizes the spread
		float minTimeDiff = 1e6;
		TRecoVHit* minHit;
		for (auto & timeDiff : timeDiffs) {
			if (timeDiff.second < minTimeDiff) {
				minTimeDiff = timeDiff.second;
				minHit = timeDiff.first;
			}
		}
		float avgTime = 0.;
		for (auto & hit : usedHits[minHit])
			avgTime += hit->GetTime() / usedHits[minHit].size();
		// now require the best cluster candidate to fit within a time window
		float maxSpread = 1e7;
		for (auto & hit : usedHits[minHit]) {
			auto spread = avgTime - hit->GetTime();
			maxSpread = (abs(spread) < maxSpread ? abs(spread) : maxSpread);
		}
		if (maxSpread < 3) // ns
			clusters.push_back(usedHits[minHit]);
		// if cluster does not fit, try removing hits and see if we get a better cluster
		else {
			for (auto & hit : usedHits[minHit]) {
				maxSpread = 1e7;
				auto newAvgTime = (avgTime * usedHits[minHit].size() - hit->GetTime()) / (usedHits[minHit].size()-1);
				for (auto & hit2 : usedHits[minHit]) {
					if (hit == hit2) continue;
					auto spread = newAvgTime - hit2->GetTime();
					maxSpread = (abs(spread) < maxSpread ? abs(spread) : maxSpread);
				}
				// add cluster and break if we found something useful
				if (maxSpread < 3) { // ns
					usedHits[minHit].erase(std::remove(usedHits[minHit].begin(), usedHits[minHit].end(), hit), usedHits[minHit].end());
					clusters.push_back(usedHits[minHit]);
					break;
				}
			}
		}
		// remove all used hits
		for (auto & hit : usedHits[minHit])
				for (auto & map : hitClusterMap)
						map.second.erase(std::remove(map.second.begin(), map.second.end(), hit), map.second.end());
		// recompute number of remaining non-empty channels
		nNonEmptyChannels = 0;
		for (auto & map : hitClusterMap)
			nNonEmptyChannels += (map.second.size() > 0 ? 1 : 0);
	}
	return clusters;
}

void ETagSimpleClusterization::putClustersIntoEvent(vector<TRecoVHit *> &Hits, vector<TRecoVCluster *> &myClusters, map<int, vector<int>> clusterHitsIdx, bool leftOrRight) {
	Double_t clE = 0.;
	Double_t clT = 0.;
	Double_t clX = 0.;
	Double_t clY = 0.;
	Double_t clZ = 0.;
	Int_t    clSize =  0;
	vector<Int_t> clHitIndices;
	Int_t    iSeed  = 0;
	Int_t    seedID = 0;
	Double_t seedT  = 0.;

	Double_t hitTime   = 0.;
	Double_t hitEnergy = 0.;
	Int_t    hitID     = 0 ;

	// loop over ETag bar hits (either left or right)
	for (auto & item : clusterHitsIdx) {
		auto barID = item.first;
		auto hitIdxs = item.second;
		if (hitIdxs.size() == 0) continue;
		// make hits vec and sort hits in time
		vector<TRecoVHit*> hits;
		for (auto & hitIdx : hitIdxs)
			hits.push_back(Hits[hitIdx]);
		std::sort(hits.begin(), hits.end(), by_time);
		// split hits in each bar according to the original SiPM channel
		map<int, vector<TRecoVHit*>> hitClusterMap;
		for (auto & hit : hits) {
			if (hitClusterMap.find(hit->GetChannelId()) != hitClusterMap.end())
				hitClusterMap[hit->GetChannelId()].push_back(hit);
			else
				hitClusterMap[hit->GetChannelId()] = vector<TRecoVHit*>{hit};
		}
		// find combinations of hits from each of the SiPM channels (up to 4 per cluster)
		// figure out the "best" one (i.e. the one in which the diff. between all 4 hits timing is minimized)
		// then make a cluster iff this time spread is < some time window, and remove hits from consideration; then repeat
		auto clusters = FindClustersByTimeSpread(hitClusterMap);

		for (auto & hits : clusters) {
			clE = 0., clX = 0., clY = 0., clZ = 0., clT = 0.;
			for (auto & hit : hits) {
				clE += hit->GetEnergy();
				clX += hit->GetPosition().X();
				clY += hit->GetPosition().Y();
				clZ += hit->GetPosition().Z();
				clT += hit->GetTime();
			}
			clX /= hits.size();
			clY /= hits.size();
			clZ /= hits.size();
			clT /= hits.size();
			TRecoVCluster* myCl = new TRecoVCluster();
			myCl->SetChannelId   ( (leftOrRight == 1 ? -barID : barID) );
			myCl->SetEnergy      ( clE );
			myCl->SetTime        ( clT );
			myCl->SetPosition    ( TVector3( clX, clY, clZ) );
			myCl->SetNHitsInClus ( hits.size() );
			myCl->SetHitVecInClus( std::vector<int>{} );
			myClusters.push_back ( myCl );
		}
		for (auto & hits : clusters) {
			float timeAvg = 0.;
			for (auto & hit : hits)
				timeAvg += hit->GetTime() / hits.size();
			for (auto & hit : hits)
				GetHisto("hInClusterTimeDiffs")->Fill(hit->GetTime() - timeAvg);
		}
	}
}

void ETagSimpleClusterization::Reconstruct(vector<TRecoVHit *> &Hits, vector<TRecoVCluster *> &myClusters)
{

  map<int, vector<int>> clusterHitsLeftIdx;
  map<int, vector<int>> clusterHitsRightIdx;
  for (auto & item : HitToClusterMapLeft) {
	auto chnID = item.first, barID = item.second;
	clusterHitsLeftIdx[barID] = vector<int>{};
  }
  for (auto & item : HitToClusterMapRight) {
	auto chnID = item.first, barID = item.second;
	clusterHitsRightIdx[barID] = vector<int>{};
  }

  for (auto hitIdx = 0; hitIdx < Hits.size(); hitIdx++) {
	if (HitToClusterMapLeft.find(Hits[hitIdx]->GetChannelId()) != HitToClusterMapLeft.end())
		clusterHitsLeftIdx[HitToClusterMapLeft[Hits[hitIdx]->GetChannelId()]].push_back(hitIdx);
	else if (HitToClusterMapRight.find(Hits[hitIdx]->GetChannelId()) != HitToClusterMapRight.end())
		clusterHitsRightIdx[HitToClusterMapRight[Hits[hitIdx]->GetChannelId()]].push_back(hitIdx);
	else {
		std::cout << "[ETagSimpleClusterization::Reconstruct] WARNING: Hit channel ID was not found in map! " << Hits[hitIdx]->GetChannelId() << std::endl;
	}
  }

  putClustersIntoEvent(Hits, myClusters, clusterHitsLeftIdx, true);
  putClustersIntoEvent(Hits, myClusters, clusterHitsRightIdx, false);

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
