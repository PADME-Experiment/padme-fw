#include "ECalMLClusters.hh"
#include "HistoSvc.hh"
#include <iostream>

static ECalMLClusters ThisAnalyser("ECalMLClusters");

ECalMLClusters::ECalMLClusters(const char *s,int valid, int verb)
  : PadmeVAnalyser(s,valid,verb)
{;}

ECalMLClusters::ECalMLClusters(const char *s):
  PadmeVAnalyser(s)
{;}

Bool_t ECalMLClusters::InitHistos(){

  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  hSvcVal->makeFileDir(GetName());
  hSvcVal->BookHisto(this->GetName()+"_ECalML_NHits",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_NHits",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalClusters_NHits",100,0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalMLClusters_NHits",100,0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalMLClusters_Energy",100,0.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalClusters_Energy",100,0.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_NClusters",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_NClusters",100,0.0,100.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECalvsECalML_NClusters",100,0.0,100.0,100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2Clusters_TimeDiff",800,-100.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2Clusters_TimeDiff",800,-100.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_TotalClusterEnergy",1000,0.0,2000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_TotalClusterEnergy",1000,0.0,2000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_TotalClusterEnergy_2Clus",1000,0.0,2000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_TotalClusterEnergy_2Clus2ns",1000,0.0,2000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_TotalClusterEnergy_2Clus",1000,0.0,2000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_TotalClusterEnergy_2Clus2ns",1000,0.0,2000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_TotalHitsEnergy",1000,0.0,2000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_TotalHitsEnergy",1000,0.0,2000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_InvariantMassSQ",2000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_InvariantMass",2000,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_InvariantMassSQ",2000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_InvariantMass",2000,0.0,100.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_InvMass_vs_EnergySum",100,0.0,100.0,1000,0.0,1000.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_InvMass_vs_EnergySum",100,0.0,100.0,1000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalClusters_NHits_invMass", 100, 0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalMLClusters_NHits_invMass", 100, 0.0,100.0);
  
  return true;
}

// double ECalMLClusters::GetVertex(std::vector<double> Ei,std::vector<double> PosX,std::vector<double> PosY)
// {
//   Double_t Zv=0;
//   Double_t Rij= sqrt(  (PosX[0]-PosX[1])*(PosX[0]-PosX[1]) + (PosY[0]-PosY[1])*(PosY[0]-PosY[1]) ); 
//   Zv = -sqrt(Ei[0]*Ei[1]*Rij*Rij)/sqrt(2*0.5111*fBeamE);  //Need to use the correct beam energy
//   //  std::cout<<"Size of vectors "<<Ei.size()<<" "<<PosX.size()<<" "<<Ei[0]<<" "<<PosY[0]<<" "<<Zv<<" EBeam "<<fBeamE<<std::endl;
//   return Zv;
// }


double ECalMLClusters::ComputeInvariantMass(Double_t E1, Double_t E2, Double_t PosX1, Double_t PosX2, Double_t PosY1, Double_t PosY2)
{
  Double_t M=0;
  Double_t Z  = 3534;
  Double_t R = sqrt( (PosX1-PosX2)*(PosX1-PosX2) + (PosY1-PosY2)*(PosY1-PosY2) ); 
  M = (E1*E2*R*R)/(Z*Z); 
  return M;
}

Bool_t ECalMLClusters::Process(){
  HistoSvc* hSvc =  HistoSvc::GetInstance();

  if(evt->ECalMLRecoEvent && evt->ECalRecoEvent) {
    hSvc->FillHisto(this->GetName()+"_ECalML_NHits",evt->ECalMLRecoEvent->GetNHits());
    hSvc->FillHisto(this->GetName()+"_ECal_NHits",evt->ECalRecoEvent->GetNHits());
  } else {
    return true;
  }
  
  TRecoVHit* ECalhit=NULL;
  TRecoVHit* ECalMLhit=NULL;

  Int_t ECalHits;
  Int_t ECalMLHits;


  ECalHits=evt->ECalRecoEvent->GetNHits();
  ECalMLHits=evt->ECalMLRecoEvent->GetNHits();

  Double_t eTotHits=0;
  Double_t eTotHitsML=0;

  for (int i=0;i<ECalHits;i++){
    ECalhit=evt->ECalRecoEvent->Hit(i);
    eTotHits += ECalhit->GetEnergy();
  }
 
  for (int i=0;i<ECalMLHits;i++){
    ECalMLhit=evt->ECalMLRecoEvent->Hit(i);
    eTotHitsML +=ECalMLhit->GetEnergy();
  }
  hSvc->FillHisto(this->GetName()+"_ECal_TotalHitsEnergy", eTotHits);
  hSvc->FillHisto(this->GetName()+"_ECalML_TotalHitsEnergy", eTotHitsML);
 
  TRecoVCluster* clu=NULL;
  TRecoVCluster* clu2=NULL;
  TRecoVCluster* cluML=NULL;
  TRecoVCluster* cluML2=NULL;
  
  Int_t NclusML = evt->ECalMLRecoCl->GetNElements();
  Int_t Nclus = evt->ECalRecoCl->GetNElements();

  Int_t NHitsInClus=0;
  Int_t NHitsInClusML=0;
  
  hSvc->FillHisto(this->GetName()+"_ECalML_NClusters",NclusML);
  hSvc->FillHisto(this->GetName()+"_ECal_NClusters",Nclus);
  hSvc->FillHisto2(this->GetName()+"_ECalvsECalML_NClusters",Nclus,NclusML);

  Double_t eTotCl=0;
  Double_t eTotClML=0;
  
  //if(NclusML==2){
  for (Int_t i=0; i<NclusML; ++i){
    cluML    = evt->ECalMLRecoCl->Element(i);
    if(cluML) {
      hSvc->FillHisto(this->GetName()+"_ECalMLClusters_NHits", cluML->GetNHitsInClus());
      hSvc->FillHisto(this->GetName()+"_ECalMLClusters_Energy", cluML->GetEnergy());
      eTotClML += cluML->GetEnergy();
      
    }
  }
  // }
  hSvc->FillHisto(this->GetName()+"_ECalML_TotalClusterEnergy", eTotClML);

  for (Int_t i=0; i<Nclus; ++i){
    clu    = evt->ECalRecoCl->Element(i);
    if(clu) {
      hSvc->FillHisto(this->GetName()+"_ECalClusters_NHits", clu->GetNHitsInClus());
      hSvc->FillHisto(this->GetName()+"_ECalClusters_Energy", clu->GetEnergy());
      eTotCl += clu->GetEnergy();
    }
  }
 
  hSvc->FillHisto(this->GetName()+"_ECal_TotalClusterEnergy", eTotCl);
  Double_t eTotClML2=0;
  Double_t eTotClML2prim=0;
  Double_t time1=0;
  Double_t time2=0;
  Double_t eTotCl2=0;
  Double_t eTotCl2prim=0;

  Double_t MinvSQ=0;
  Double_t MinvSQML=0;

  TVector3 pos1ML;
  TVector3 pos2ML;

  TVector3 pos1;
  TVector3 pos2;


 
  
  if(NclusML==2){
    //for (Int_t i=0; i<NclusML; ++i){
    cluML    = evt->ECalMLRecoCl->Element(0);
    cluML2    = evt->ECalMLRecoCl->Element(1);
    if(cluML&&cluML2) {
      //hSvc->FillHisto(this->GetName()+"_ECalML_2Clusters_Energy", cluML->GetEnergy());
      time1 = cluML->GetTime();
      time2 = cluML2->GetTime();
      hSvc->FillHisto(this->GetName()+"_ECalML_2Clusters_TimeDiff", time2-time1);

      //eTotClML2 += cluML->GetEnergy();

      pos1ML=cluML->GetPosition();
      pos2ML=cluML2->GetPosition();

      if(fabs(cluML->GetTime() - cluML2->GetTime())<5.)
	{
	  eTotClML2 = cluML->GetEnergy()+cluML2->GetEnergy();
	  hSvc->FillHisto(this->GetName()+"_ECalML_TotalClusterEnergy_2Clus", eTotClML2);
 	}
      if(fabs(cluML->GetTime() - cluML2->GetTime())<2.)
	{
	  eTotClML2prim = cluML->GetEnergy()+cluML2->GetEnergy();
	  hSvc->FillHisto(this->GetName()+"_ECalML_TotalClusterEnergy_2Clus2ns", eTotClML2prim);
	  MinvSQML=ComputeInvariantMass(cluML->GetEnergy(),cluML2->GetEnergy(),pos1ML.X(),pos2ML.X(),pos1ML.Y(),pos2ML.Y());
	  hSvc->FillHisto(this->GetName()+"_ECalML_InvariantMassSQ", MinvSQML);
	  hSvc->FillHisto(this->GetName()+"_ECalML_InvariantMass", sqrt(MinvSQML));
	  hSvc->FillHisto2(this->GetName()+"_ECalML_InvMass_vs_EnergySum",sqrt(MinvSQML),eTotClML2prim);
	  if(sqrt(MinvSQML)>=17&&sqrt(MinvSQML)<=23){
	    hSvc->FillHisto(this->GetName()+"_ECalMLClusters_NHits_invMass", cluML->GetNHitsInClus());
	    hSvc->FillHisto(this->GetName()+"_ECalMLClusters_NHits_invMass", cluML2->GetNHitsInClus());
	  }
	}
    }
  }
  if(Nclus==2){
    // hSvc->FillHisto(this->GetName()+"_ECalML_TotalClusterEnergy_2Clus", eTotClML2);
    clu    = evt->ECalRecoCl->Element(0);
    clu2    = evt->ECalRecoCl->Element(1);
    if(clu&&clu2) {
      //hSvc->FillHisto(this->GetName()+"_ECalML_2Clusters_Energy", cluML->GetEnergy());
      time1 = clu->GetTime();
      time2 = clu2->GetTime();
      hSvc->FillHisto(this->GetName()+"_ECal_2Clusters_TimeDiff", time2-time1);

      pos1=clu->GetPosition();
      pos2=clu2->GetPosition();
      //eTotClML2 += cluML->GetEnergy();
      
      if(fabs(clu->GetTime() - clu2->GetTime())<5.)
	{
	  eTotCl2 = clu->GetEnergy()+clu2->GetEnergy();
	  hSvc->FillHisto(this->GetName()+"_ECal_TotalClusterEnergy_2Clus", eTotCl2);
	}
      if(fabs(clu->GetTime() - clu2->GetTime())<2.)
	{
	  eTotCl2prim = clu->GetEnergy()+clu2->GetEnergy();
	  hSvc->FillHisto(this->GetName()+"_ECal_TotalClusterEnergy_2Clus2ns", eTotCl2prim);
	  MinvSQ=ComputeInvariantMass(clu->GetEnergy(),clu2->GetEnergy(),pos1.X(),pos2.X(),pos1.Y(),pos2.Y());
	  hSvc->FillHisto(this->GetName()+"_ECal_InvariantMassSQ", MinvSQ);
	  hSvc->FillHisto(this->GetName()+"_ECal_InvariantMass", sqrt(MinvSQ));
	  hSvc->FillHisto2(this->GetName()+"_ECal_InvMass_vs_EnergySum",sqrt(MinvSQ),eTotCl2prim);
	  if(sqrt(MinvSQ)>=19&&sqrt(MinvSQ)<=22){
	    hSvc->FillHisto(this->GetName()+"_ECalClusters_NHits_invMass", clu->GetNHitsInClus());
	    hSvc->FillHisto(this->GetName()+"_ECalClusters_NHits_invMass", clu2->GetNHitsInClus());
	  } 
	}
    }
  }
  fResult = true;
  return fResult;
}
