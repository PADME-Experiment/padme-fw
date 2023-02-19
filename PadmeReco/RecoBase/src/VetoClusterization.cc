//Class to define VetoClusterization
#include "VetoClusterization.hh"
#include "VetoClusterHits.hh"//Class to define Hits in a Cluster

#include <iostream>
#include <algorithm>

VetoCluster::VetoCluster(){
  //  std::cout<<"I cluster"<<std::endl;
  nhits=0;
  mostUpstreamChannel=100;
  mostDownstreamChannel=-1;
  averagetime=0;
  earlyhittime=1e9;
  latehittime=-1e9;
  energy = 0;
  seedtime=-10000;
  seedch=-10000;
  seedenergy=-10000;
  hitIndexVector.clear();
  seedindex = -1000;
}


int VetoCluster::InsertHit(VetoClusterHits hit, int ihit, double seedtime){//CHECK SEEDTIME BUSINESS
  if(nhits>MAXHIT-1){
    std::cout<<"Too many hits, exiting"<<std::endl;
    return -1;
  }

  hitIndexVector.push_back(ihit);

  if(hit.GetChannelId()<mostUpstreamChannel){
    mostUpstreamChannel=hit.GetChannelId();
    seedtime = hit.GetTime();
  }
  if(hit.GetChannelId()>mostDownstreamChannel) mostDownstreamChannel=hit.GetChannelId();
  if(hit.GetTime()<earlyhittime) earlyhittime=hit.GetTime();
  if(hit.GetTime()>latehittime) latehittime=hit.GetTime();

  double oldenergy = energy;
  double newenergy = hit.GetEnergy();

  averagetime = (nhits*averagetime+hit.GetTime())/(nhits+1);
  //  averagetime = (oldenergy*nhits*averagetime+hit.GetTime()*newenergy)/((nhits+1)*(oldenergy+newenergy));//energy-weighting for time

  //  averagetime=seedtime;//CHECK SEEDTIME BUSINESS

  energy += hit.GetEnergy();
  nhits++;
}

int VetoCluster::AddCluster(VetoCluster* newcluster){

  int NMax=newcluster->GetNHits();
  int goodreturn=1;
  if(newcluster->GetNHits()+nhits>MAXHIT){
    std::cout<<"MAXHIT too small, too many hits"<<std::endl;
    NMax=MAXHIT;
    goodreturn=-1;
  }
  for(int ii=0;ii<NMax;ii++){
    hitIndexVector.push_back(newcluster->GetHitIndex()[ii]);
  }
  if(newcluster->GetMostUpstreamChannel()<mostUpstreamChannel){
    mostUpstreamChannel=newcluster->GetMostUpstreamChannel();
    seedtime = newcluster->GetSeedTime();
  }
  if(newcluster->GetMostDownstreamChannel()>mostDownstreamChannel) mostDownstreamChannel=newcluster->GetMostDownstreamChannel();
  if(newcluster->GetEarlyHitTime()<earlyhittime) earlyhittime = newcluster->GetEarlyHitTime();
  if(newcluster->GetLateHitTime()>latehittime) latehittime = newcluster->GetLateHitTime();

  double oldenergy = energy;
  double newenergy = newcluster->GetEnergy();

  averagetime=(nhits*averagetime+newcluster->GetNHits()*newcluster->GetAverageTime())/(nhits+newcluster->GetNHits());
  //  averagetime=(oldenergy*nhits*averagetime+newcluster->GetNHits()*newenergy*newcluster->GetAverageTime())/((nhits*oldenergy)+(newcluster->GetNHits())*newenergy));;//energy-weighting for time
  energy+=newcluster->GetEnergy();
  nhits+=NMax;
  return goodreturn;
}


VetoClusterStructure::VetoClusterStructure(){
  HitVec.clear();
  ClusVec.clear();
  HitIndexVec.clear();
}

void VetoClusterStructure::Clear(){
  ClusVec.clear();
  HitVec.clear();//legit???
  HitIndexVec.clear();
}

void VetoClusterStructure::Clusterise(){
  for (int ii=0; ii <HitVec.size(); ++ii ) {
    bool UsedHit = 0;
    double myseedtime;

    if(HitVec.at(ii).GetEnergy()<0.9)    std::cout<<"Hit "<<ii<<" ChID "<<HitVec.at(ii).GetChannelId()<<" time "<<HitVec.at(ii).GetTime()<<" Energy "<<HitVec.at(ii).GetEnergy()<<std::endl;
    for (int jj = 0; jj <ClusVec.size(); ++jj ) {//loop over all currently identified clusters to see whether this hit can be clusterised with any of those
      if(HitVec.at(ii).GetTime()-ClusVec.at(jj)->GetAverageTime()<ClusterDeltaT&&
	 (HitVec.at(ii).GetChannelId()-ClusVec.at(jj)->GetMostUpstreamChannel()==-1//
	  ||HitVec.at(ii).GetChannelId()-ClusVec.at(jj)->GetMostDownstreamChannel()==1)){
	if(HitVec.at(ii).GetChannelId()-ClusVec.at(jj)->GetMostUpstreamChannel()==-1) myseedtime = HitVec.at(ii).GetTime();
	//Beth 14/2/23: I've given VetoClusterHit a SetIndex and a GetIndex, so I think the index vector can be removed everywhere
	ClusVec.at(jj)->InsertHit(HitVec.at(ii), HitIndexVec.at(ii),myseedtime);//CHECK SEEDTIME BUSINESS
	
	UsedHit=1;
	break;
      }	
    }//end cluster loop
    if(!UsedHit) {
      myseedtime = HitVec.at(ii).GetTime();
      VetoCluster* NewVetoCluster = new VetoCluster();
      NewVetoCluster->InsertHit(HitVec.at(ii), HitIndexVec.at(ii), myseedtime);//CHECK SEEDTIME BUSINESS
      ClusVec.push_back(NewVetoCluster); 
    }
  }//end hit loop
}


void VetoClusterStructure::MergeClusters(){
  Int_t noCompact = 0;
  while(noCompact==0) {
    noCompact = 1;
    int ii = 0;
    while(ii+1< ClusVec.size()){
      int iUp = ClusVec.at(ii)->GetMostUpstreamChannel();
      int iDown = ClusVec.at(ii)->GetMostDownstreamChannel();
      double iAvgT = ClusVec.at(ii)->GetAverageTime();
      int jj = ii+1; 
      while(jj < ClusVec.size()){
	int jUp = ClusVec.at(jj)->GetMostUpstreamChannel();
	int jDown = ClusVec.at(jj)->GetMostDownstreamChannel();
	double jAvgT = ClusVec.at(jj)->GetAverageTime();
	if(std::fabs(iAvgT-jAvgT)<ClusterDeltaT&&(std::fabs(iUp-jDown)==1||std::fabs(jUp-iDown)==1)){//Beth & Tommaso: 11/10/22 parenthese of channel fabs changed
	  int goodorbad = ClusVec.at(ii)->AddCluster(ClusVec.at(jj));//adds new hits to existing cluster & updates mostUpstreamChannel, mostDownstreamChannel and averagetime.
	  if(jj<ClusVec.size()-1) ClusVec.erase(ClusVec.begin()+jj);//cluster jj has already been used so get rid of it, and move all other clusters down in the vector
	  if(jj==ClusVec.size()-1) ClusVec.pop_back();//
	  noCompact = 0;
	  for(int kk = 0;kk<ClusVec.size();kk++){
	  }

	}//end if(clusterise?)
	else	++jj;
      }
      ++ii;
    }
  }
}

/**void VetoClusterStructure::HitSort(){ //Beth 14/2/23: I'm changing the way the hit indexing is done. If we go back to sorting in time instead of in energy we'll have to import this change too.
  //  std::cout<<"sorting"<<std::endl;
  std::vector<VetoClusterHits> HitVecCopy;
 
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

  }**/

void VetoClusterStructure::HitSort(){

  std::vector<VetoClusterHits> HitVecCopy;
 
  for(int ii=0;ii<HitVec.size();ii++)    HitVecCopy.push_back(HitVec.at(ii));
    
  std::vector<int> sortindex(HitVec.size(), 0);
  
  for (int i = 0 ; i != sortindex.size() ; i++) {
    sortindex[i] = i;
  }
  
  sort(sortindex.begin(), sortindex.end(),
       [&](const int& a, const int& b) {
	 return (HitVec[a].GetEnergy() < HitVec[b].GetEnergy());
	 }
       );

  for (int ii = 0 ; ii != sortindex.size() ; ++ii) {
    HitVec[ii]=(HitVecCopy[sortindex[ii]]);
    HitIndexVec[ii]=HitVecCopy[sortindex[ii]].GetIndex();

    if(ii>0&&HitVec[ii].GetEnergy()-HitVec[ii-1].GetEnergy()<0) {
      std::cout<<"----------------YOU'RE IN A MESS MY FRIEND---------------"<<std::endl;
      return;
    }
  }

}
