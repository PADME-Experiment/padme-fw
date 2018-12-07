#include "SACAnalysis.hh"

#include "TSACRecoEvent.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "HistoSvc.hh"
#include <iostream>

SACAnalysis::SACAnalysis()
{
  fNhits=0;
  fhitEvent=NULL;
}
SACAnalysis::~SACAnalysis()
{
  fNhits=0;
  fhitEvent=NULL;
}
Bool_t SACAnalysis::Init(Int_t nh, TSACRecoEvent* ev, TRecoVClusCollection* cl)
{
  Bool_t retCode = 0;
  fNhits    = nh;
  fhitEvent = ev;
  fClColl = cl;

  return retCode;
}
Bool_t SACAnalysis::Process()
{

  HistoSvc* hSvc =  HistoSvc::GetInstance();


  std::string hname;
  Bool_t retCode = 0;

  hname = "SAC_NHits";
  hSvc->FillHisto(hname,fhitEvent->GetNHits());
  
  for (unsigned int j=0; j<fhitEvent->GetNHits(); ++j)
    {
      TRecoVHit* h = fhitEvent->Hit(j);
      std::cout<<"SAChits "<<j<<" "<<h->GetChannelId()<<" "<<h->GetEnergy()<<" "<<h->GetTime()<<std::endl;
    }

  
  TRecoVClusCollection* clColl = fClColl;// fhitEvent->getClusCollection();
  std::cout<<" from the SACRecoEvent N elements:  "<<clColl->GetNElements()<< std::endl;
  hname = "SAC_NClusters";
  hSvc->FillHisto(hname,clColl->GetNElements());
  for (Int_t j=0; j<clColl->GetNElements(); ++j)
    {
      Int_t clSize =  (clColl->Element(j)->GetHitVecInClus()).size();
      
      std::cout<<"... in the tree ... SAC_Cluster "<<j<<"  chId/energy/time " << clColl->Element(j)->GetChannelId() <<" "<<clColl->Element(j)->GetEnergy() <<" "<<clColl->Element(j)->GetTime() << std::endl;
      std::cout <<"the index of the seed hits is " << clColl->Element(j)->GetSeed()<< std::endl;
      std::cout <<"the number of hit belonging to this cluster is " << clColl->Element(j)->GetNHitsInClus() << std::endl;
      std::cout << "size of vector of hits in cl " << clSize << std::endl;
      //      std::cout << "the list of hits in the cluster is ";
      std::cout<<" cluster "<<j<<" id/e/t/nhit/hits"<<clColl->Element(j)->GetChannelId()<<"/"<<clColl->Element(j)->GetEnergy()<<"/"<<clColl->Element(j)->GetTime()<<"/"<<clColl->Element(j)->GetNHitsInClus()<<"/ ";
      for (int i=0; i<(clColl->Element(j)->GetHitVecInClus()).size(); i++) std::cout << " "<<(clColl->Element(j)->GetHitVecInClus()).at(i);
      std::cout<<std::endl;
      hname = "SAC_Cluster_size";
      hSvc->FillHisto(hname,(Double_t)clSize);
      hname = "SAC_Cluster_time";
      hSvc->FillHisto(hname,clColl->Element(j)->GetTime());
      hname = "SAC_Cluster_chId";
      hSvc->FillHisto(hname,clColl->Element(j)->GetChannelId());
      hname = "SAC_Cluster_energy";
      hSvc->FillHisto(hname,clColl->Element(j)->GetEnergy());
      hname = "SAC_Cluster_XY";
      hSvc->FillHisto2(hname,clColl->Element(j)->GetPosition().x(),clColl->Element(j)->GetPosition().y());
      hname = "SAC_Cluster_EwXY";
      hSvc->FillHisto2(hname,clColl->Element(j)->GetPosition().x(),clColl->Element(j)->GetPosition().y(),
		       clColl->Element(j)->GetEnergy());

      Int_t iSeed = clColl->Element(j)->GetSeed();
      Int_t chIdSeed = fhitEvent->Hit(iSeed)->GetChannelId(); 
      Int_t chIdClus = clColl->Element(j)->GetChannelId(); 
      if (chIdSeed!=chIdClus)std::cout<<" WARNING .... chIdSeed = "<<chIdSeed<<" chIdClus = "<<chIdClus<<std::endl;

    }
  
  
  
  return retCode;
}
