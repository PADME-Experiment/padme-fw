#include "HEPVetoAnalysis.hh"
#include "THEPVetoRecoEvent.hh"
#include "TRecoVHit.hh"
#include "TRecoEvent.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "HistoSvc.hh"
#include <iostream>

HEPVetoAnalysis::HEPVetoAnalysis(): ValidationBase()
{
  fAlgoName = "HEPVeto";
  InitHistos();
  return;
}
HEPVetoAnalysis::HEPVetoAnalysis(Int_t processingMode, Int_t verbosityFlag): ValidationBase(processingMode, verbosityFlag)
{
  fAlgoName = "HEPVeto";
  InitHistos();
  return;
}
HEPVetoAnalysis::~HEPVetoAnalysis()
{
}
Bool_t HEPVetoAnalysis::InitHistosAnalysis()
{

    return true;
}


Bool_t HEPVetoAnalysis::InitHistosValidation()
{
  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  std::string hname;
  int nBin, min, max;
  nBin=100;
  min=0;
  max=150;
  hname="HEPVeto_NHits";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="HEPVeto_NCluster";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=50;
  min=0.;
  max=50;
  hname = "HEPVeto_HitEnergy";
  hSvcVal->BookHisto(hname,nBin,min, max);
  hname = "HEPVeto_ClusterEnergy";
  hSvcVal->BookHisto(hname,nBin,min, max);
  nBin=800;
  min=-400;
  max=400;
  hname = "HEPVeto_HitTime";
  hSvcVal->BookHisto(hname,nBin, min, max);
  hname = "HEPVeto_ClusterTime";
  hSvcVal->BookHisto(hname,nBin, min, max);
  nBin=20;
  min=780;
  max=960;
  hname="HEPVeto_HitXPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="HEPVeto_ClusterXPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=3;
  min=-1.5;
  max=1.5;
  hname="HEPVeto_HitYPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="HEPVeto_ClusterYPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=20;
  min=2070;
  max=2120;
  hname="HEPVeto_HitZPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="HEPVeto_ClusterZPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=33;
  min=0;
  max=33;
  hname="HEPVeto_HitChannelId";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="HEPVeto_ClusterSeedChannelId";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=15;
  min=0;
  max=15;
  hname="HEPVeto_NHitInCluster";
  hSvcVal->BookHisto(hname, nBin, min, max);
  
  return true;
}



Bool_t HEPVetoAnalysis::InitHistosDataQuality()
{

    HistoSvc* hSvc =  HistoSvc::GetInstance();
    std::string hname;
    Double_t nx, xlow, xup;

    nx =  50.;
    xlow = 0;
    xup  = 50;
   
    hname = "HEPVeto_ChId_Hits_BTFtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "HEPVeto_ChId_HitsEnergy_BTFtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
     hname = "HEPVeto_ChId_Hits_CRtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "HEPVeto_ChId_HItsEnergy_CRtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
     hname = "HEPVeto_ChId_Hits_AUTOtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "HEPVeto_ChId_HitsEnergy_AUTOtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);


    hname = "HEPVeto_ChId_Clusters_BTFtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "HEPVeto_ChId_ClustersEnergy_BTFtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
     hname = "HEPVeto_ChId_Clusters_CRtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "HEPVeto_ChId_ClustersEnergy_CRtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
     hname = "HEPVeto_ChId_Clusters_AUTOtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "HEPVeto_ChId_ClustersEnergy_AUTOtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    

    nx = 300.;
    xlow = 0;
    xup  =  1000.0;
    hname = "HEPVeto_HitsEnergy_BTFtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "HEPVeto_HitsEnergy_CRtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "HEPVeto_HitsEnergy_AUTOtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);

    hname = "HEPVeto_ClustersEnergy_BTFtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "HEPVeto_ClustersEnergy_CRtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "HEPVeto_ClustersEnergy_AUTOtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);

    int binX =   50;
    int binY =  600;
    int minX =    0;
    int maxX =   50;
    int minY = -300;
    int maxY =  300;
    hname="HEPVeto_HitsTimevsChIdHits";
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    hname="HEPVeto_ClustersTimevsChIdClusters";
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
     
    return true;

}
Bool_t HEPVetoAnalysis::ProcessAnalysis()
{
  Bool_t retCode = 0;
 
  HistoSvc* hSvc =  HistoSvc::GetInstance();

  TRecoVHit* hit=NULL;
  TRecoVHit* hitn=NULL;
  TRecoVCluster* clu=NULL;
  TRecoVCluster* clun=NULL;
  std::string hname;
  Int_t      chId;
  Double_t energy;
  Double_t   time;
  Int_t      chIdn;
  Double_t energyn;
  Double_t   timen;

  Int_t fNhits = fhitEvent->GetNHits();
  Int_t fNclus = fClColl->GetNElements();
  Int_t seedId;
  Int_t clSize;

  (hSvc->myEvt).NTNHEPVeto_Hits=fhitEvent->GetNHits();
  
  for (Int_t i=0; i<fNhits; ++i){
    hit    = fhitEvent->Hit(i);
    chId   = hit->GetChannelId();
    energy = hit->GetEnergy();
    time   = hit->GetTime();

   (hSvc->myEvt).NTHEPVeto_Hits_ChannelId[i]=(Double_t)chId;
   (hSvc->myEvt).NTHEPVeto_Hits_Energy[i]=hit->GetEnergy();
   (hSvc->myEvt).NTHEPVeto_Hits_Time[i]=hit->GetTime();
   (hSvc->myEvt).NTHEPVeto_Hits_Xpos[i]=hit->GetPosition().X();
   (hSvc->myEvt).NTHEPVeto_Hits_Ypos[i]=hit->GetPosition().Y();
   (hSvc->myEvt).NTHEPVeto_Hits_Zpos[i]=hit->GetPosition().Z();
  }

    //fillClustersFlatNTP  

  (hSvc->myEvt).NTNHEPVeto_Clusters= fClColl->GetNElements();

   for (Int_t j=0; j<fNclus; ++j){
     clu    = fClColl->Element(j);
     seedId = clu->GetChannelId();
  
   (hSvc->myEvt).NTHEPVeto_Clusters_ChannelId[j]=Double_t(clu->GetChannelId());
   (hSvc->myEvt).NTHEPVeto_Clusters_Energy[j]=clu->GetEnergy();
   (hSvc->myEvt).NTHEPVeto_Clusters_Time[j]=clu->GetTime();
   (hSvc->myEvt).NTHEPVeto_Clusters_Xpos[j]=clu->GetPosition().X();
   (hSvc->myEvt).NTHEPVeto_Clusters_Ypos[j]=clu->GetPosition().Y();
   (hSvc->myEvt).NTHEPVeto_Clusters_Zpos[j]=clu->GetPosition().Z();
  }

  // HistoSvc* hSvc =  HistoSvc::GetInstance();

  // Double_t eMax  =  0.;
  // Int_t    iLead = -1;
  // TRecoVHit* hit=NULL;
  // TRecoVHit* hitn=NULL;
  // std::string hname;

  // Int_t      chId;
  // Double_t energy;
  // Double_t   time;
  // Int_t      chIdn;
  // Double_t energyn;
  // Double_t   timen;

  
  // Int_t fNhits = fhitEvent->GetNHits();
  // for (Int_t i=0; i<fNhits; ++i){
  //   hit    = fhitEvent->Hit(i);
  //   chId   = hit->GetChannelId();
  //   energy = hit->GetEnergy();
  //   time   = hit->GetTime();


  //   hname = "HEPVeto_Hit_TimeVSch";
  //   hSvc->FillHisto2(hname,time, (float)chId,  1.);
    
  //   if (time<fTmin || time>fTmax) continue;
  //   for (Int_t j=0; j<fNhits; ++j)
  //     {
  // 	if (i==j) continue;
  // 	hitn = fhitEvent->Hit(j);
  // 	chIdn   = hitn->GetChannelId();
  // 	energyn = hitn->GetEnergy();
  // 	timen   = hitn->GetTime();

  // 	if (chIdn==chId){
  // 	  hname = "HEPVeto_Hit_DTime_DCh0";
  // 	  hSvc->FillHisto(hname,time-timen,  1.);
  // 	}
  // 	else if (abs(chIdn-chId)<2){
  // 	  hname = "HEPVeto_Hit_DTime_DCh1";
  // 	  hSvc->FillHisto(hname,time-timen,  1.);
  // 	  hname = "HEPVeto_Hit_DTimeVSch_DCh1";
  // 	  hSvc->FillHisto2(hname,time-timen,(float)chId,  1.);
  // 	}
  // 	else if (abs(chIdn-chId)<3){
  // 	  hname = "HEPVeto_Hit_DTime_DCh2";
  // 	  hSvc->FillHisto(hname,time-timen,  1.);
  // 	}
  // 	else{
  // 	  hname = "HEPVeto_Hit_DTime_DChLarge";
  // 	  hSvc->FillHisto(hname,time-timen,  1.);
  // 	}
	
  //     }

  //   if (energy > eMax) {iLead = i; eMax=energy;}

  //   hname = "HEPVeto_HitEnergy";
  //   hSvc->FillHisto(hname,energy,1.);
    
  //   hname = "HEPVeto_HitMap";
  //   hSvc->FillHisto(hname, (Double_t)chId, 1.);

  //   /*
  //   // ntuple variables
  //   if (i<1000)
  //     {
  // 	hSvc->myEvt.NTHEPVetoTrkFinger[i]=hit->GetChannelId();
  // 	hSvc->myEvt.NTHEPVetoTrkTime[i]  =hit->GetTime();
  //     }
  //   */
  //  }
  //  hSvc->myEvt.NTNHEPVetoTracks=fNhits;

  
   return retCode;
}
Bool_t HEPVetoAnalysis::ProcessDataQuality()
{
  Bool_t retCode = 0;

  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::cout<<"In HEPVetoAnalysis::ProcessDataQuality"<<std::endl;

  Double_t eMax  =  0.;
  Int_t    iLead = -1;
  TRecoVHit* hit=NULL;
  TRecoVHit* hitn=NULL;
  TRecoVCluster* clu=NULL;
  TRecoVCluster* clun=NULL;
  std::string hname;

  Int_t      chId;
  Double_t energy;
  Double_t   time;
  //Int_t      chIdn;
  Double_t energyn;
  Double_t   timen;

  Int_t fNhits = fhitEvent->GetNHits();
  Int_t fNclus = fClColl->GetNElements();


  for (Int_t i=0; i<fNhits; ++i){  

  hit = fhitEvent->Hit(i);
  chId =hit->GetChannelId();
  energy=hit->GetEnergy();
  time   = hit->GetTime();

 // hname = "HEPVeto_ChId_Hits";
 // hSvc->FillHisto(hname, (Double_t)chId, 1. );
 // hname = "HEPVeto_ChId_Energy";
 // hSvc->FillHisto(hname, (Double_t)chId,  energy);
 // hname = "HEPVeto_Energy";
 // hSvc->FillHisto(hname, energy, 1.);

  hname ="HEPVeto_HitsTimevsChIdHits";
  hSvc->FillHisto2(hname,chId,time, 1.);

  if(fRecoEvent->GetTriggerMask()==1) {

  hname = "HEPVeto_ChId_Hits_BTFtrigger";
  hSvc->FillHisto(hname, (Double_t)chId, 1. );
  hname = "HEPVeto_ChId_HitsEnergy_BTFtrigger";
  hSvc->FillHisto(hname, (Double_t)chId,  energy);
  hname = "HEPVeto_HitsEnergy_BTFtrigger";
  hSvc->FillHisto(hname, energy, 1.);
}
  if(fRecoEvent->GetTriggerMask()==2) {

  hname = "HEPVeto_ChId_Hits_CRtrigger";
  hSvc->FillHisto(hname, (Double_t)chId, 1. );
  hname = "HEPVeto_ChId_HitsEnergy_CRtrigger";
  hSvc->FillHisto(hname, (Double_t)chId,  energy);
  hname = "HEPVeto_HitsEnergy_CRtrigger";
  hSvc->FillHisto(hname, energy, 1.);
}
if(fRecoEvent->GetTriggerMask()==128) {

  hname = "HEPVeto_ChId_Hits_AUTOtrigger";
  hSvc->FillHisto(hname, (Double_t)chId, 1. );
  hname = "HEPVeto_ChId_HitsEnergy_AUTOtrigger";
  hSvc->FillHisto(hname, (Double_t)chId,  energy);
  hname = "HEPVeto_HitsEnergy_AUTOtrigger";
  hSvc->FillHisto(hname, energy, 1.);
}

} //end loop On Hits

  Int_t seed;
  Int_t seedId;
  Double_t seedE;
  Double_t seedT;
  Int_t clSize;

 for (Int_t j=0; j<fNclus; ++j){
     clu    = fClColl->Element(j);
     seedId = clu->GetChannelId();
     seedE  = clu->GetEnergy();
     seedT  = clu->GetTime();

  if(fRecoEvent->GetTriggerMask()==1) {

  hname = "HEPVeto_ChId_Clusters_BTFtrigger";
  hSvc->FillHisto(hname, (Double_t)seedId, 1. );
  hname = "HEPVeto_ChId_ClustersEnergy_BTFtrigger";
  hSvc->FillHisto(hname, (Double_t)seedId,  seedE);
  hname = "HEPVeto_ClustersEnergy_BTFtrigger";
  hSvc->FillHisto(hname, seedE, 1.);
}
  if(fRecoEvent->GetTriggerMask()==2) {

  hname = "HEPVeto_ChId_Clusters_CRtrigger";
  hSvc->FillHisto(hname, (Double_t)seedId, 1. );
  hname = "HEPVeto_ChId_ClustersEnergy_CRtrigger";
  hSvc->FillHisto(hname, (Double_t)seedId,  seedE);
  hname = "HEPVeto_ClustersEnergy_CRtrigger";
  hSvc->FillHisto(hname, seedE, 1.);
}
if(fRecoEvent->GetTriggerMask()==128) {

  hname = "HEPVeto_ChId_Clusters_AUTOtrigger";
  hSvc->FillHisto(hname, (Double_t)seedId, 1. );
  hname = "HEPVeto_ChId_ClustersEnergy_AUTOtrigger";
  hSvc->FillHisto(hname, (Double_t)seedId,  seedE);
  hname = "HEPVeto_ClustersEnergy_AUTOtrigger";
  hSvc->FillHisto(hname, seedE, 1.);
}

  hname ="HEPVeto_ClustersTimevsChIdClusters";
  hSvc->FillHisto2(hname,seedId,seedT, 1.);


}// end loop on clusters

  

 return retCode;
}
