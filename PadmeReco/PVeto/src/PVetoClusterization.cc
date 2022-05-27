//Class to define PVetoClusterization
#include "PVetoClusterization.hh"
#include "PVetoClusterHits.hh"//Class to define Hits in a Cluster

#include <iostream>
#include <algorithm>

PVetoCluster::PVetoCluster(){
  //  std::cout<<"I cluster"<<std::endl;
  nhits=0;
  mostUpstreamChannel=100;
  mostDownstreamChannel=-1;
  averagetime=0;
  earlyhittime=1e9;
  latehittime=-1e9;
  energy = 0;
}


int PVetoCluster::InsertHit(PVetoClusterHits hit, int ihit){
  if(nhits>MAXHIT-1){
    std::cout<<"Too many hits, exiting"<<std::endl;
    return -1;
  }
  //  std::cout<<"I insert hits"<<std::endl;
  //  hitIndex[nhits]=ihit;
  hitIndex.push_back(ihit);
  //  if(nhits>0||ihit>0||hitIndex.size()>0) std::cout<<"ihit "<<ihit<<std::endl;
  if(hit.GetChannelId()<mostUpstreamChannel) mostUpstreamChannel=hit.GetChannelId();
  if(hit.GetChannelId()>mostDownstreamChannel) mostDownstreamChannel=hit.GetChannelId();
  if(hit.GetTime()<earlyhittime) earlyhittime=hit.GetTime();
  if(hit.GetTime()>latehittime) latehittime=hit.GetTime();
  averagetime = (nhits*averagetime+hit.GetTime())/(nhits+1);
  energy += hit.GetEnergy();
  nhits++;
}

int PVetoCluster::AddCluster(PVetoCluster* newcluster){

  int NMax=newcluster->GetNHits()+nhits;
  //  std::cout<<nhits<<" "<<newcluster->GetNHits()<<" "<<NMax<<std::endl;
  int goodreturn=1;
  if(newcluster->GetNHits()+nhits>MAXHIT){
    std::cout<<"MAXHIT too small, too many hits"<<std::endl;
    NMax=MAXHIT;
    goodreturn=-1;
  }
  for(int ii=0;ii<NMax;ii++){
    //    hitIndex[nhits+ii]=newcluster->GetHitIndex()[ii];
    hitIndex.push_back(newcluster->GetHitIndex()[ii]);
  }
  if(newcluster->GetMostUpstreamChannel()<mostUpstreamChannel) mostUpstreamChannel=newcluster->GetMostUpstreamChannel();
  if(newcluster->GetMostDownstreamChannel()>mostDownstreamChannel) mostDownstreamChannel=newcluster->GetMostDownstreamChannel();
  if(newcluster->GetEarlyHitTime()<earlyhittime) earlyhittime = newcluster->GetEarlyHitTime();
  if(newcluster->GetLateHitTime()>latehittime) latehittime = newcluster->GetLateHitTime();
  averagetime=(nhits*averagetime+newcluster->GetNHits()*newcluster->GetAverageTime())/(nhits+newcluster->GetNHits());
  energy+=newcluster->GetEnergy();
  nhits=NMax;

  return goodreturn;
}


PVetoClusterStructure::PVetoClusterStructure(){
  //  std::cout<<"Initialising PVetoClusterStructure"<<std::endl;
  HitVec.clear();
  ClusVec.clear();
  HitIndexVec.clear();
}

void PVetoClusterStructure::Clear(){
  //  std::cout<<"Clearing PVetoClusterStructure"<<std::endl;
  ClusVec.clear();
  HitVec.clear();//legit???
  HitIndexVec.clear();
}

void PVetoClusterStructure::Clusterise(){
  //  std::cout<<"cljst "<<HitVec.size()<<std::endl;
  for (int ii=0; ii <HitVec.size(); ++ii ) {
    bool UsedHit = 0;
    //    std::cout<<"clusvec size "<<ClusVec.size()<<std::endl;
    //    std::cout<<"Hit "<<ii<<" ChID "<<HitVec.at(ii)->GetChannelId()<<" time "<<HitVec.at(ii)->GetTime()<<std::endl;
    for (int jj = 0; jj <ClusVec.size(); ++jj ) {//loop over all currently identified clusters to see whether this hit can be clusterised with any of those
      if(HitVec.at(ii).GetTime()-ClusVec.at(jj)->GetAverageTime()<ClusterDeltaT&&
	 (HitVec.at(ii).GetChannelId()-ClusVec.at(jj)->GetMostUpstreamChannel()==-1//
	  ||HitVec.at(ii).GetChannelId()-ClusVec.at(jj)->GetMostDownstreamChannel()==1)){	//4->parameter
	ClusVec.at(jj)->InsertHit(HitVec.at(ii), HitIndexVec.at(ii));
	UsedHit=1;
	//	std::cout<<"clusterising jj "<<jj<<" size "<<ClusVec.at(jj)->GetNHits()<<std::endl;
	break;
      }	
    }//end cluster loop
    if(!UsedHit) {
      PVetoCluster* NewPVetoCluster = new PVetoCluster();
      NewPVetoCluster->InsertHit(HitVec.at(ii), HitIndexVec.at(ii));
      ClusVec.push_back(NewPVetoCluster); 
    }
  }//end hit loop
}


void PVetoClusterStructure::MergeClusters(){
  Int_t noCompact = 0;
  //std::cout<<"Merging"<<std::endl;
  while(noCompact==0) {
    //  std::cout<<"Merging, ClusVec.size() "<<ClusVec.size()<<std::endl;
    noCompact = 1;
    int ii = 0;
    while(ii+1< ClusVec.size()){
      //std::cout<<"ii "<<ii<<" size "<<ClusVec.size()<<" "<<typeid(ClusVec.at(ii)).name()<<std::endl;
      //qualche volta anche se ClusVec.size()!=0, ClusVec.at(ii)->GetNhits() fa segmentation violation. Non lo fa sempre e non riesco a capire quando/perche'
      //std::cout<<"ii "<<ii<<" Nhits "<<ClusVec.at(ii)->GetNHits()<<std::endl;
      int iUp = ClusVec.at(ii)->GetMostUpstreamChannel();
      int iDown = ClusVec.at(ii)->GetMostDownstreamChannel();
      double iAvgT = ClusVec.at(ii)->GetAverageTime();
      int jj = ii+1; 
      while(jj < ClusVec.size()){
	int jUp = ClusVec.at(jj)->GetMostUpstreamChannel();
	int jDown = ClusVec.at(jj)->GetMostDownstreamChannel();
	double jAvgT = ClusVec.at(jj)->GetAverageTime();
	//	std::cout<<"jj "<<jj<<" jUp "<<jUp<<" jDown "<<jDown<<" jAvgT "<<jAvgT<<std::endl;
	if(std::fabs(iAvgT-jAvgT)<ClusterDeltaT&&(std::fabs(iUp-jDown==1)||std::fabs(jUp-iDown==1))){
	  int goodorbad = ClusVec.at(ii)->AddCluster(ClusVec.at(jj));//adds new hits to existing cluster & updates mostUpstreamChannel, mostDownstreamChannel and averagetime.
	  //std::cout<<"gonna erase "<<jj<<std::endl;
 	  //if(jj==1) std::cout<<"TAKE NOTICE OF ME!!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
	  if(jj<ClusVec.size()-1) ClusVec.erase(ClusVec.begin()+jj);//cluster jj has already been used so get rid of it, and move all other clusters down in the vector
	  if(jj==ClusVec.size()-1) ClusVec.pop_back();//
	  noCompact = 0;
	  for(int kk = 0;kk<ClusVec.size();kk++){
	    //    std::cout<<kk<<" upstream "<<ClusVec.at(kk)->GetMostUpstreamChannel()<<" downstream "<<ClusVec.at(kk)->GetMostDownstreamChannel()<<" avgT "<<ClusVec.at(kk)->GetAverageTime()<<std::endl;

	  }
	  //	  std::cout<<goodorbad<<std::endl;
	}//end if(clusterise?)
	else	++jj;
      }
      ++ii;
    }
  }
  // Int_t noCompact = 0;
  // while(noCompact==0) {
  //   std::cout<<"ClusVec.size() "<<ClusVec.size()<<std::endl;
  //   noCompact = 1;
  //   int indexii=0;
  //   for(std::vector<PVetoCluster*>::iterator ii = ClusVec.begin(); ii+1!= ClusVec.end();){
  //     indexii++;
  //     std::cout<<"ii "<<indexii<<" size "<<ClusVec.size()<<" nocompact "<<noCompact<<std::endl;//typeid(ii).name()<<std::endl;
  //     //qualche volta anche se ClusVec.size()!=0, ClusVec.at(ClusVec.begin()+ii)->GetNhits() fa segmentation violation. Non lo fa sempre e non riesco a capire quando/perche'
  //     std::cout<<"ii "<<indexii<<" Nhits "<<(*ii)->GetNHits()<<std::endl;
  //     int iUp = (*ii)->GetMostUpstreamChannel();
  //     int iDown = (*ii)->GetMostDownstreamChannel();
  //     double iAvgT = (*ii)->GetAverageTime();
  //     int indexjj=0;
  //     for(std::vector<PVetoCluster*>::iterator jj = ii+1; jj!=ClusVec.end();){
  // 	indexjj++;
  // 	int jUp = (*jj)->GetMostUpstreamChannel();
  // 	int jDown = (*jj)->GetMostDownstreamChannel();
  // 	double jAvgT = (*jj)->GetAverageTime();
  // 	std::cout<<"jj "<<indexjj<<" jUp "<<jUp<<" jDown "<<jDown<<" jAvgT "<<jAvgT<<" size "<<ClusVec.size()<<std::endl;
  // 	if(std::fabs(iAvgT-jAvgT)<PVetoClusterDeltaT&&(iUp-jDown==1||jUp-iDown==1)){
  // 	  int goodorbad = (*ii)->AddPVetoCluster(*jj);//adds new hits to existing cluster & updates mostUpstreamChannel, mostDownstreamChannel and averagetime.
  // 	  std::cout<<"gonna erase ii"<<indexii<<" jj "<<indexjj<<std::endl;
  // 	  //	  if(jj==1) std::cout<<"IT HAPPENS HERE!!!!!!!!!"<<std::endl;
  // 	  jj=ClusVec.erase(jj);//cluster jj has already been used so get rid of it, and move all other clusters down in the vector
  // 	  noCompact = 0;
  // 	  std::cout<<goodorbad<<" size "<<ClusVec.size()<<" noCompact "<<noCompact<<std::endl;
  // 	}//end if(clusterise?)
  // 	else{
  // 	++jj;
  // 	std::cout<<"else size "<<ClusVec.size()<<" noCompact "<<noCompact<<std::endl;	
  // 	}
  //     }//end jj loop
  //     ii++;
  //     std::cout<<"ii increase "<<ClusVec.size()<<" noCompact "<<noCompact<<std::endl;	
  //   }
  //  }
}

void PVetoClusterStructure::HitSort(){
  //  std::cout<<"sorting"<<std::endl;
  std::vector<PVetoClusterHits> HitVecCopy;
 
  for(int ii=0;ii<HitVec.size();ii++){
    HitVecCopy.push_back(HitVec.at(ii));
    // std::cout<<"HitVec[ii] Ch "<<HitVec[ii]->GetChannelId()<<" time "<<HitVec[ii]->GetTime()<<std::endl;
  }
    
  std::vector<int> index(HitVec.size(), 0);
  
  for (int i = 0 ; i != index.size() ; i++) {
    index[i] = i;
  }
  
  sort(index.begin(), index.end(),
       [&](const int& a, const int& b) {
	 return (HitVec[a].GetTime() < HitVec[b].GetTime());
	 }
       );
  
  for (int ii = 0 ; ii != index.size() ; ++ii) {
    HitVec[ii]=(HitVecCopy[index[ii]]);
    //    std::cout<<"     HitVec[ii] Ch "<<HitVec[ii].GetChannelId()<<" time "<<HitVec[ii].GetTime();//<<std::endl;
    //    std::cout<<" HitVecCopy[ii] Ch "<<HitVecCopy[ii].GetChannelId()<<" time "<<HitVecCopy[ii].GetTime()<<std::endl;
    HitIndexVec[ii]=index[ii];
    //    std::cout<<"ii "<<ii<<" Time[ii] "<<HitVec[ii]->GetTime()<<std::endl;
    if(ii>0&&HitVec[ii].GetTime()-HitVec[ii-1].GetTime()<0) {
      std::cout<<"----------------YOU'RE IN A MESS MY FRIEND---------------"<<std::endl;
      return;
    }
  }

}
