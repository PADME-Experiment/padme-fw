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
  return true;
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
  hSvc->FillHisto(this->GetName()+"_ECalML_NClusters",NclusML);
  hSvc->FillHisto(this->GetName()+"_ECal_NClusters",Nclus);
  hSvc->FillHisto2(this->GetName()+"_ECalvsECalML_NClusters",Nclus,NclusML);

  Double_t eTotCl=0;
  Double_t eTotClML=0;
  
  //if(NclusML==2){
  for (Int_t i=0; i<NclusML; ++i){
    cluML    = evt->ECalMLRecoCl->Element(i);
    if(cluML) {
      hSvc->FillHisto(this->GetName()+"_ECalMLClusters_Energy", cluML->GetEnergy());
      eTotClML += cluML->GetEnergy();
    }
  }
  // }
  hSvc->FillHisto(this->GetName()+"_ECalML_TotalClusterEnergy", eTotClML);

  for (Int_t i=0; i<Nclus; ++i){
    clu    = evt->ECalRecoCl->Element(i);
    if(clu) {
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
      
      if(fabs(cluML->GetTime() - cluML2->GetTime())<5.)
	{
	  eTotClML2 = cluML->GetEnergy()+cluML2->GetEnergy();
	  hSvc->FillHisto(this->GetName()+"_ECalML_TotalClusterEnergy_2Clus", eTotClML2);
 	}
      if(fabs(cluML->GetTime() - cluML2->GetTime())<2.)
	{
	  eTotClML2prim = cluML->GetEnergy()+cluML2->GetEnergy();
	  hSvc->FillHisto(this->GetName()+"_ECalML_TotalClusterEnergy_2Clus2ns", eTotClML2prim);
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
	}
    }
  }
  fResult = true;
  return fResult;
}
