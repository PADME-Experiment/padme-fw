#include "SACAnalysis.hh"

#include "TSACRecoEvent.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "TRecoEvent.hh"
#include "HistoSvc.hh"
#include <iostream>

SACAnalysis::SACAnalysis(): ValidationBase()
{
  fAlgoName = "SAC";
  InitHistos();
  return;
}
SACAnalysis::SACAnalysis(Int_t processingMode, Int_t verbosityFlag): ValidationBase(processingMode, verbosityFlag)
{
  fAlgoName = "SAC";
  InitHistos();
  return;
}

SACAnalysis::~SACAnalysis()
{
  fhitEvent=NULL;
  fClColl=NULL;
}
Bool_t SACAnalysis::InitHistosAnalysis()
{
    return true;
}

Bool_t SACAnalysis::InitHistosDataQuality()
{

    HistoSvc* hSvc =  HistoSvc::GetInstance();    
    std::string hname;
    int nBin;
    Double_t min, max;

    nBin=250;
    min=0.;
    max=1000;
    hname="SAC_HitEnergy";
    hSvc->BookHisto(hname, nBin, min, max);

    nBin=250;
    min=0.;
    max=1000;
    hname="SAC_ClEnergy";
    hSvc->BookHisto(hname, nBin, min, max);

    nBin=250;
    min=0.;
    max=10000;
    hname="SAC_SumHitEnergy";
    hSvc->BookHisto(hname, nBin, min, max);

    nBin=250;
    min=0.;
    max=10000;
    hname="SAC_SumHitEnergy_BTFtrg";
    hSvc->BookHisto(hname, nBin, min, max);

    hname = "SAC_HitOccupancyEWeig";
    nBin=7;
    min=-1.5;
    max=5.5;
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);

    hname = "SAC_HitOccupancy";
    nBin=7;
    min=-1.5;
    max=5.5;
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    
    return true;
}
Bool_t SACAnalysis::ProcessDataQuality()
{
    HistoSvc* hSvc =  HistoSvc::GetInstance();
    std::cout<<"In SACAnalysis::ProcessDataQuality"<<std::endl;
    

    Int_t fNhits = fhitEvent->GetNHits();

    std::string hname;

    Double_t EHitSum = 0.;
    TRecoVHit* h;
    for (Int_t j=0; j<fhitEvent->GetNHits(); ++j)
    {
      h = fhitEvent->Hit(j);
      hname = "SAC_HitEnergy";
      hSvc->FillHisto(hname, h->GetEnergy());
      EHitSum = EHitSum+h->GetEnergy();
      hname = "SAC_HitOccupancyEWeig";
      int chid =  h->GetChannelId();
      int chidx =  chid/10;
      int chidy =  chid%10;
      hSvc->FillHisto2(hname, float(chidx),  float(chidy), h->GetEnergy());	
      hname = "SAC_HitOccupancy";
      hSvc->FillHisto2(hname, float(chidx),  float(chidy));
    }
    hname = "SAC_SumHitEnergy";
    hSvc->FillHisto(hname, EHitSum);
    UInt_t trgMask = fRecoEvent->GetTriggerMask();
    if (trgMask==1)
    {
	hname = "SAC_SumHitEnergy_BTFtrg";
	hSvc->FillHisto(hname, EHitSum);
    }
    

    hname = "SAC_ClEnergy";
    TRecoVCluster* k;
    for (Int_t j=0; j<fClColl->GetNElements(); ++j)
    {
      k = fClColl->Element(j);
      hSvc->FillHisto(hname, k->GetEnergy());
    }

    
    return true;
}


Bool_t SACAnalysis::InitHistosValidation()
{
    HistoSvc* hSvc =  HistoSvc::GetInstance();
    std::string hname;
    int nBin, min, max;
    nBin=300;
    min=0;
    max=300;
    hname="SAC_NHits";
    hSvc->BookHisto(hname, nBin, min, max);
    hname="SAC_NCluster";
    hSvc->BookHisto(hname, nBin, min, max);
    nBin=500;
    min=0;
    max=500;
    hname = "SAC_HitEnergy";
    hSvc->BookHisto(hname,nBin,min, max);
    hname = "SAC_ClusterEnergy";
    hSvc->BookHisto(hname,nBin,min, max);
    nBin=700;
    min=-300;
    max=400;
    hname = "SAC_HitTime";
    hSvc->BookHisto(hname,nBin, min, max);
    hname = "SAC_ClusterTime";
    hSvc->BookHisto(hname,nBin, min, max);
    nBin=60;
    min=0;
    max=6;
    hname = "SAC_HitMap";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    hname = "SAC_ClusterMap";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    nBin=100;
    min=0;
    max=100;
    hname="SAC_HitXPos";
    hSvc->BookHisto(hname, nBin, min, max);
    hname="SAC_ClusterXPos";
    hSvc->BookHisto(hname, nBin, min, max);
    hname="SAC_HitYPos";
    hSvc->BookHisto(hname, nBin, min, max);
    hname="SAC_ClusterYPos";
    hSvc->BookHisto(hname, nBin, min, max);
    nBin=100;
    min=0;
    max=60;
    hname="SAC_HitZPos";
    hSvc->BookHisto(hname, nBin, min, max);
    hname="SAC_ClusterZPos";
    hSvc->BookHisto(hname, nBin, min, max);
    hname="SAC_HitChannelId";
    hSvc->BookHisto(hname, nBin, min, max);
    hname="SAC_ClusterSeedChannelId";
    hSvc->BookHisto(hname, nBin, min, max);
    nBin=16;
    min=0;
    max=15;
    hname="SAC_NHitInCluster";
    hSvc->BookHisto(hname, nBin, min, max);
    
    

    return true;
}
Bool_t SACAnalysis::ProcessAnalysis()
{

  Bool_t retCode = 0;
  
  // HistoSvc* hSvc =  HistoSvc::GetInstance();

  // Int_t fNhits = fhitEvent->GetNHits();
  // std::string hname;

  // hname = "SAC_NHits";
  // hSvc->FillHisto(hname,fhitEvent->GetNHits());
  
  // for (Int_t j=0; j<fhitEvent->GetNHits(); ++j)
  //   {
  //     TRecoVHit* h = fhitEvent->Hit(j);
  //     std::cout<<"SAChits "<<j<<" "<<h->GetChannelId()<<" "<<h->GetEnergy()<<" "<<h->GetTime()<<std::endl;
  //   }

  
  // TRecoVClusCollection* clColl = fClColl;fhitEvent->getClusCollection();
  // std::cout<<" from the SACRecoEvent N elements:  "<<clColl->GetNElements()<< std::endl;
  // hname = "SAC_NClusters";
  // hSvc->FillHisto(hname,clColl->GetNElements());
  // for (Int_t j=0; j<clColl->GetNElements(); ++j)
  //   {
  //     Int_t clSize =  (clColl->Element(j)->GetHitVecInClus()).size();
      
  //     std::cout<<"... in the tree ... SAC_Cluster "<<j<<"  chId/energy/time " << clColl->Element(j)->GetChannelId() <<" "<<clColl->Element(j)->GetEnergy() <<" "<<clColl->Element(j)->GetTime() << std::endl;
  //     std::cout <<"the index of the seed hits is " << clColl->Element(j)->GetSeed()<< std::endl;
  //     std::cout <<"the number of hit belonging to this cluster is " << clColl->Element(j)->GetNHitsInClus() << std::endl;
  //     std::cout << "size of vector of hits in cl " << clSize << std::endl;
  //          std::cout << "the list of hits in the cluster is ";
  //     std::cout<<" cluster "<<j<<" id/e/t/nhit/hits"<<clColl->Element(j)->GetChannelId()<<"/"<<clColl->Element(j)->GetEnergy()<<"/"<<clColl->Element(j)->GetTime()<<"/"<<clColl->Element(j)->GetNHitsInClus()<<"/ ";
  //     for (unsigned int i=0; i<(clColl->Element(j)->GetHitVecInClus()).size(); i++) std::cout << " "<<(clColl->Element(j)->GetHitVecInClus()).at(i);
  //     std::cout<<std::endl;
  //     hname = "SAC_Cluster_size";
  //     hSvc->FillHisto(hname,(Double_t)clSize);
  //     hname = "SAC_Cluster_time";
  //     hSvc->FillHisto(hname,clColl->Element(j)->GetTime());
  //     hname = "SAC_Cluster_chId";
  //     hSvc->FillHisto(hname,clColl->Element(j)->GetChannelId());
  //     hname = "SAC_Cluster_energy";
  //     hSvc->FillHisto(hname,clColl->Element(j)->GetEnergy());
  //     hname = "SAC_Cluster_XY";
  //     hSvc->FillHisto2(hname,clColl->Element(j)->GetPosition().x(),clColl->Element(j)->GetPosition().y());
  //     hname = "SAC_Cluster_EwXY";
  //     hSvc->FillHisto2(hname,clColl->Element(j)->GetPosition().x(),clColl->Element(j)->GetPosition().y(),
  // 		       clColl->Element(j)->GetEnergy());

  //     Int_t iSeed = clColl->Element(j)->GetSeed();
  //     Int_t chIdSeed = fhitEvent->Hit(iSeed)->GetChannelId(); 
  //     Int_t chIdClus = clColl->Element(j)->GetChannelId(); 
  //     if (chIdSeed!=chIdClus)std::cout<<" WARNING .... chIdSeed = "<<chIdSeed<<" chIdClus = "<<chIdClus<<std::endl;

  //   }
  
  
  // std::cout<<"Out of SAC"<<std::endl;
  return retCode;
}





Bool_t SACAnalysis::ProcessValidation()
{
  ValidationBase::ProcessValidation("SAC");
  Bool_t retCode = 0;

  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  TRecoVHit* hit=NULL;
  std::string hname;
  Int_t fNhits = fhitEvent->GetNHits();
  for (Int_t i=0; i<fNhits; ++i){
    hit = fhitEvent->Hit(i);
    Int_t ix = hit->GetChannelId()/10;
    Int_t iy = hit->GetChannelId()%10;
    //Int_t ix=position.X();
    //Int_t iy=position.Y();
  
    hname = "SAC_HitMap";
    hSvcVal->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);

    
   }


   TRecoVCluster* clu=NULL;
   Int_t fNclus = fClColl->GetNElements();

   std::cout<<"NClusters:  " << fNclus << std::endl;
   for (Int_t i=0; i<fNclus; ++i){
     clu    = fClColl->Element(i);
     Int_t ix = clu->GetChannelId()/10;
     Int_t iy = clu->GetChannelId()%10;
     //Int_t ix=position.X();
     //Int_t iy=position.Y();
     hname = "SAC_ClusterMap";
     hSvcVal->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);
   }
   return retCode;
}
