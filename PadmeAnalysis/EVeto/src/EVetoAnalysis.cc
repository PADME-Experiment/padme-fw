#include "EVetoAnalysis.hh"
#include "TEVetoRecoEvent.hh"
#include "TRecoEvent.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "HistoSvc.hh"
#include <iostream>

EVetoAnalysis::EVetoAnalysis(): ValidationBase()
{
  fAlgoName = "EVeto";
  InitHistos();
  return;
}
EVetoAnalysis::EVetoAnalysis(Int_t processingMode, Int_t verbosityFlag): ValidationBase(processingMode, verbosityFlag)
{
  fAlgoName = "EVeto";
  InitHistos();
  return;
}
EVetoAnalysis::~EVetoAnalysis()
{
}
Bool_t EVetoAnalysis::InitHistosAnalysis()
{
    return true;
}
Bool_t EVetoAnalysis::InitHistosDataQuality()
{

    HistoSvc* hSvc =  HistoSvc::GetInstance();
    std::string hname;
    Double_t nx, xlow, xup;

    nx = 100.;
    xlow = 0;
    xup  =  100.0;
   
    hname = "EVeto_ChId_Hits_BTFtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "EVeto_ChId_HitsEnergy_BTFtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
     hname = "EVeto_ChId_Hits_CRtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "EVeto_ChId_HItsEnergy_CRtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
     hname = "EVeto_ChId_Hits_AUTOtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "EVeto_ChId_HitsEnergy_AUTOtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);


    hname = "EVeto_ChId_Clusters_BTFtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "EVeto_ChId_ClustersEnergy_BTFtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
     hname = "EVeto_ChId_Clusters_CRtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "EVeto_ChId_ClustersEnergy_CRtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
     hname = "EVeto_ChId_Clusters_AUTOtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "EVeto_ChId_ClustersEnergy_AUTOtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    

    nx = 300.;
    xlow = 0;
    xup  =  1000.0;
    hname = "EVeto_HitsEnergy_BTFtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "EVeto_HitsEnergy_CRtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "EVeto_HitsEnergy_AUTOtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);

    hname = "EVeto_ClustersEnergy_BTFtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "EVeto_ClustersEnergy_CRtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "EVeto_ClustersEnergy_AUTOtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);

    int binX =  100;
    int binY =  600;
    int minX =    0;
    int maxX =  100;
    int minY = -300;
    int maxY =  300;
    hname="EVeto_HitsTimevsChIdHits";
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    hname="EVeto_ClustersTimevsChIdClusters";
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
     
     
    return true;

}
Bool_t EVetoAnalysis::ProcessAnalysis()
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

    //fillHitsFlatNTP

  (hSvc->myEvt).NTNEVeto_Hits=fhitEvent->GetNHits();
  
  for (Int_t i=0; i<fNhits; ++i){
    hit    = fhitEvent->Hit(i);
    chId   = hit->GetChannelId();
    energy = hit->GetEnergy();
    time   = hit->GetTime();

   (hSvc->myEvt).NTEVeto_Hits_ChannelId[i]=(Double_t)chId;
   (hSvc->myEvt).NTEVeto_Hits_Energy[i]=hit->GetEnergy();
   (hSvc->myEvt).NTEVeto_Hits_Time[i]=hit->GetTime();
   (hSvc->myEvt).NTEVeto_Hits_Xpos[i]=hit->GetPosition().X();
   (hSvc->myEvt).NTEVeto_Hits_Ypos[i]=hit->GetPosition().Y();
   (hSvc->myEvt).NTEVeto_Hits_Zpos[i]=hit->GetPosition().Z();
  }

    //fillClustersFlatNTP  

  (hSvc->myEvt).NTNEVeto_Clusters= fClColl->GetNElements();

   for (Int_t j=0; j<fNclus; ++j){
     clu    = fClColl->Element(j);
     seedId = clu->GetChannelId();
  
   (hSvc->myEvt).NTEVeto_Clusters_ChannelId[j]=Double_t(clu->GetChannelId());
   (hSvc->myEvt).NTEVeto_Clusters_Energy[j]=clu->GetEnergy();
   (hSvc->myEvt).NTEVeto_Clusters_Time[j]=clu->GetTime();
   (hSvc->myEvt).NTEVeto_Clusters_Xpos[j]=clu->GetPosition().X();
   (hSvc->myEvt).NTEVeto_Clusters_Ypos[j]=clu->GetPosition().Y();
   (hSvc->myEvt).NTEVeto_Clusters_Zpos[j]=clu->GetPosition().Z();
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


  //   hname = "EVeto_Hit_TimeVSch";
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
  // 	  hname = "EVeto_Hit_DTime_DCh0";
  // 	  hSvc->FillHisto(hname,time-timen,  1.);
  // 	}
  // 	else if (abs(chIdn-chId)<2){
  // 	  hname = "EVeto_Hit_DTime_DCh1";
  // 	  hSvc->FillHisto(hname,time-timen,  1.);
  // 	  hname = "EVeto_Hit_DTimeVSch_DCh1";
  // 	  hSvc->FillHisto2(hname,time-timen,(float)chId,  1.);
  // 	}
  // 	else if (abs(chIdn-chId)<3){
  // 	  hname = "EVeto_Hit_DTime_DCh2";
  // 	  hSvc->FillHisto(hname,time-timen,  1.);
  // 	}
  // 	else{
  // 	  hname = "EVeto_Hit_DTime_DChLarge";
  // 	  hSvc->FillHisto(hname,time-timen,  1.);
  // 	}
	
  //     }

  //   if (energy > eMax) {iLead = i; eMax=energy;}

  //   hname = "EVeto_HitEnergy";
  //   hSvc->FillHisto(hname,energy,1.);
    
  //   hname = "EVeto_HitMap";
  //   hSvc->FillHisto(hname, (Double_t)chId, 1.);

  //   /*
  //   // ntuple variables
  //   if (i<1000)
  //     {
  // 	hSvc->myEvt.NTEVetoTrkFinger[i]=hit->GetChannelId();
  // 	hSvc->myEvt.NTEVetoTrkTime[i]  =hit->GetTime();
  //     }
  //   */
  //  }
  //  hSvc->myEvt.NTNEVetoTracks=fNhits;

  
   return retCode;
}

Bool_t EVetoAnalysis::ProcessDataQuality()
{
  Bool_t retCode = 0;

  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::cout<<"In EVetoAnalysis::ProcessDataQuality"<<std::endl;

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

 // hname = "EVeto_ChId_Hits";
 // hSvc->FillHisto(hname, (Double_t)chId, 1. );
 // hname = "EVeto_ChId_Energy";
 // hSvc->FillHisto(hname, (Double_t)chId,  energy);
 // hname = "EVeto_Energy";
 // hSvc->FillHisto(hname, energy, 1.);

  hname ="EVeto_HitsTimevsChIdHits";
  hSvc->FillHisto2(hname,chId,time, 1.);

  if(fRecoEvent->GetTriggerMask()==1) {

  hname = "EVeto_ChId_Hits_BTFtrigger";
  hSvc->FillHisto(hname, (Double_t)chId, 1. );
  hname = "EVeto_ChId_HitsEnergy_BTFtrigger";
  hSvc->FillHisto(hname, (Double_t)chId,  energy);
  hname = "EVeto_HitsEnergy_BTFtrigger";
  hSvc->FillHisto(hname, energy, 1.);
}
  if(fRecoEvent->GetTriggerMask()==2) {

  hname = "EVeto_ChId_Hits_CRtrigger";
  hSvc->FillHisto(hname, (Double_t)chId, 1. );
  hname = "EVeto_ChId_HitsEnergy_CRtrigger";
  hSvc->FillHisto(hname, (Double_t)chId,  energy);
  hname = "EVeto_HitsEnergy_CRtrigger";
  hSvc->FillHisto(hname, energy, 1.);
}
if(fRecoEvent->GetTriggerMask()==128) {

  hname = "EVeto_ChId_Hits_AUTOtrigger";
  hSvc->FillHisto(hname, (Double_t)chId, 1. );
  hname = "EVeto_ChId_HitsEnergy_AUTOtrigger";
  hSvc->FillHisto(hname, (Double_t)chId,  energy);
  hname = "EVeto_HitsEnergy_AUTOtrigger";
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

  hname = "EVeto_ChId_Clusters_BTFtrigger";
  hSvc->FillHisto(hname, (Double_t)seedId, 1. );
  hname = "EVeto_ChId_ClustersEnergy_BTFtrigger";
  hSvc->FillHisto(hname, (Double_t)seedId,  seedE);
  hname = "EVeto_ClustersEnergy_BTFtrigger";
  hSvc->FillHisto(hname, seedE, 1.);
}
  if(fRecoEvent->GetTriggerMask()==2) {

  hname = "EVeto_ChId_Clusters_CRtrigger";
  hSvc->FillHisto(hname, (Double_t)seedId, 1. );
  hname = "EVeto_ChId_ClustersEnergy_CRtrigger";
  hSvc->FillHisto(hname, (Double_t)seedId,  seedE);
  hname = "EVeto_ClustersEnergy_CRtrigger";
  hSvc->FillHisto(hname, seedE, 1.);
}
if(fRecoEvent->GetTriggerMask()==128) {

  hname = "EVeto_ChId_Clusters_AUTOtrigger";
  hSvc->FillHisto(hname, (Double_t)seedId, 1. );
  hname = "EVeto_ChId_ClustersEnergy_AUTOtrigger";
  hSvc->FillHisto(hname, (Double_t)seedId,  seedE);
  hname = "EVeto_ClustersEnergy_AUTOtrigger";
  hSvc->FillHisto(hname, seedE, 1.);
}

}// end loop on clusters

  hname ="EVeto_ClustersTimevsChIdClusters";
  hSvc->FillHisto2(hname,seedId,seedT, 1.);

  return retCode;
}
