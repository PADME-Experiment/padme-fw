#include "PVetoAnalysis.hh"
#include "TRecoEvent.hh"
#include "TPVetoRecoEvent.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "HistoSvc.hh"
#include <iostream>

PVetoAnalysis::PVetoAnalysis()  : ValidationBase()
{
  fAlgoName = "PVeto";
  InitHistos();
  return;
}
PVetoAnalysis::PVetoAnalysis(Int_t processingMode, Int_t verbosityFlag): ValidationBase(processingMode, verbosityFlag)
{
  fAlgoName = "PVeto";
  InitHistos();
  return;
}

PVetoAnalysis::~PVetoAnalysis()
{
}
Bool_t PVetoAnalysis::InitHistosAnalysis()
{

    return true;
}
Bool_t PVetoAnalysis::InitHistosDataQuality()
{

    HistoSvc* hSvc =  HistoSvc::GetInstance();
    std::string hname;
    Double_t nx, xlow, xup;

    nx = 100.;
    xlow = 0;
    xup  =  100.0;
   
    hname = "PVeto_ChId_Hits_BTFtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "PVeto_ChId_HitsEnergy_BTFtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
     hname = "PVeto_ChId_Hits_CRtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "PVeto_ChId_HItsEnergy_CRtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
     hname = "PVeto_ChId_Hits_AUTOtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "PVeto_ChId_HitsEnergy_AUTOtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);


    hname = "PVeto_ChId_Clusters_BTFtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "PVeto_ChId_ClustersEnergy_BTFtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
     hname = "PVeto_ChId_Clusters_CRtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "PVeto_ChId_ClustersEnergy_CRtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
     hname = "PVeto_ChId_Clusters_AUTOtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "PVeto_ChId_ClustersEnergy_AUTOtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    

    nx = 300.;
    xlow = 0;
    xup  =  1000.0;
    hname = "PVeto_HitsEnergy_BTFtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "PVeto_HitsEnergy_CRtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "PVeto_HitsEnergy_AUTOtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);

    hname = "PVeto_ClustersEnergy_BTFtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "PVeto_ClustersEnergy_CRtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "PVeto_ClustersEnergy_AUTOtrigger";
    hSvc->BookHisto(hname, nx, xlow, xup);

    int binX =  100;
    int binY =  600;
    int minX =    0;
    int maxX =  100;
    int minY = -300;
    int maxY =  300;
    hname="PVeto_HitsTimevsChIdHits";
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    hname="PVeto_ClustersTimevsChIdClusters";
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
     
     
    return true;

}
Bool_t PVetoAnalysis::ProcessAnalysis()
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

  (hSvc->myEvt).NTNPVeto_Hits=fhitEvent->GetNHits();  

  for (Int_t i=0; i<fNhits; ++i){
    hit    = fhitEvent->Hit(i);
    chId   = hit->GetChannelId();
    energy = hit->GetEnergy();
    time   = hit->GetTime();

   (hSvc->myEvt).NTPVeto_Hits_ChannelId[i]=(Double_t)chId;
   (hSvc->myEvt).NTPVeto_Hits_Energy[i]=hit->GetEnergy();
   (hSvc->myEvt).NTPVeto_Hits_Time[i]=hit->GetTime();
   (hSvc->myEvt).NTPVeto_Hits_Xpos[i]=hit->GetPosition().X();
   (hSvc->myEvt).NTPVeto_Hits_Ypos[i]=hit->GetPosition().Y();
   (hSvc->myEvt).NTPVeto_Hits_Zpos[i]=hit->GetPosition().Z();
  }

    //fillClustersFlatNTP  

   for (Int_t j=0; j<fNclus; ++j){
     clu    = fClColl->Element(j);
     seedId = clu->GetChannelId();
  
   (hSvc->myEvt).NTNPVeto_Clusters= fClColl->GetNElements();
   (hSvc->myEvt).NTPVeto_Clusters_ChannelId[j]=Double_t(clu->GetChannelId());
   (hSvc->myEvt).NTPVeto_Clusters_Energy[j]=clu->GetEnergy();
   (hSvc->myEvt).NTPVeto_Clusters_Time[j]=clu->GetTime();
   (hSvc->myEvt).NTPVeto_Clusters_Xpos[j]=clu->GetPosition().X();
   (hSvc->myEvt).NTPVeto_Clusters_Ypos[j]=clu->GetPosition().Y();
   (hSvc->myEvt).NTPVeto_Clusters_Zpos[j]=clu->GetPosition().Z();
  }

  // HistoSvc* hSvc =  HistoSvc::GetInstance();

  // Double_t eMax  =  0.;
  // Int_t    iLead = -1;
  // TRecoVHit* hit=NULL;
  // TRecoVHit* hitn=NULL;
  // TRecoVCluster* clu=NULL;
  // TRecoVCluster* clun=NULL;
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


  //   hname = "PVeto_Hit_TimeVSch";
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
  // 	  hname = "PVeto_Hit_DTime_DCh0";
  // 	  hSvc->FillHisto(hname,time-timen,  1.);
  // 	}
  // 	else if (abs(chIdn-chId)<2){
  // 	  hname = "PVeto_Hit_DTime_DCh1";
  // 	  hSvc->FillHisto(hname,time-timen,  1.);
  // 	  hname = "PVeto_Hit_DTimeVSch_DCh1";
  // 	  hSvc->FillHisto2(hname,time-timen,(float)chId,  1.);
  // 	}
  // 	else if (abs(chIdn-chId)<3){
  // 	  hname = "PVeto_Hit_DTime_DCh2";
  // 	  hSvc->FillHisto(hname,time-timen,  1.);
  // 	}
  // 	else{
  // 	  hname = "PVeto_Hit_DTime_DChLarge";
  // 	  hSvc->FillHisto(hname,time-timen,  1.);
  // 	}
	
  //     }

  //   if (energy > eMax) {iLead = i; eMax=energy;}

  //   hname = "PVeto_HitEnergy";
  //   hSvc->FillHisto(hname,energy,1.);
    
  //   hname = "PVeto_HitMap";
  //   hSvc->FillHisto(hname, (Double_t)chId, 1.);

  //   /*
  //   // ntuple variables
  //   if (i<1000)
  //     {
  // 	hSvc->myEvt.NTPVetoTrkFinger[i]=hit->GetChannelId();
  // 	hSvc->myEvt.NTPVetoTrkTime[i]  =hit->GetTime();
  //     }
  //   */
  //  }
  //  hSvc->myEvt.NTNPVetoTracks=fNhits;


  //  // 
  //  Int_t seed;
  //  Int_t seedId;
  //  Double_t seedE;
  //  Double_t seedT;
  //  Int_t clSize;
  //  Int_t fNclus = fClColl->GetNElements();
  //  for (Int_t i=0; i<fNclus; ++i){
  //    clu    = fClColl->Element(i);
  //    seed   = clu->GetSeed();
  //    seedId = clu->GetChannelId();
  //    Int_t checkseedId =  fhitEvent->Hit(seed)->GetChannelId();
  //    if ( checkseedId != seedId ) std::cout<<"ERROR on PVeto indices --- seed: clustID, hit(seed).id = "<<seed<<" "<<seedId<<" "<<checkseedId<<std::endl;
  //    seedE  = clu->GetEnergy();
  //    seedT  = clu->GetTime();
  //    clSize = clu->GetNHitsInClus();
  //    std::cout<<"PVeto:: cluster n. "<<i<<" size, seed  "<<clSize<<" "<<seed<<"  chId, E, T "<<seedId<<" "<<seedE<<" "<<seedT<<std::endl;
  //    std::vector<Int_t> vhinclu = clu->GetHitVecInClus();
  //    for (Int_t ih=0; ih<clSize; ++ih){
  //      std::cout<<"PVeto:: cluster n. "<<i<<" hit comp. n "<<ih<<std::endl;
  //      Int_t hitInd    = vhinclu[ih];
  //      std::cout<<"PVeto:: cluster n. "<<i<<" hit comp. n "<<ih<<" hitIndex = "<<hitInd<<std::endl;
  //      if (hitInd == seed) {std::cout<<"PVeto:: cluster n. "<<i<<" hit comp. n "<<ih<<"- skipping seed"<<std::endl;continue;}
  //      hit = fhitEvent->Hit(hitInd);
  //      std::cout<<"PVeto:: cluster n. "<<i<<" hit comp. n "<<ih<<" hitIndex = "<<hitInd<<" chid, E, T "
  // 		<<hit->GetChannelId()<<" "<<hit->GetEnergy()<<" "<<hit->GetTime()<<std::endl;
  //      hname = "PVeto_inClus_DCHseed";
  //      hSvc->FillHisto(hname,hit->GetChannelId()-seedId,1.);
  //      hname = "PVeto_inClus_DTseed";
  //      hSvc->FillHisto(hname,hit->GetTime()-seedT,1.);
  //      for (Int_t ihn=ih+1; ihn<clSize; ++ihn){
  // 	 Int_t hitIndn    = vhinclu[ihn];
  // 	 //if (hitIndn == seed) continue; 
  // 	 hitn = fhitEvent->Hit(hitIndn);
  // 	 std::cout<<"PVeto:: cluster n. "<<i<<" hit comp. n "<<ih<<" vs comp in="<<ihn<<" chId, T "<<hitn->GetChannelId()<<" "<<hitn->GetTime()<<std::endl;
  // 	 hname = "PVeto_inClus_DCHhits";
  // 	 hSvc->FillHisto(hname,hit->GetChannelId()-hitn->GetChannelId(),1.);
  // 	 hname = "PVeto_inClus_DThits";
  // 	 hSvc->FillHisto(hname,hit->GetTime()-hitn->GetTime(),1.);
  //      }
  //    }
  //    for (Int_t in=0; in<fNclus; ++in){
  //      clun    = fClColl->Element(in);
  //      //
  //      hname = "PVeto_Clus2Clus_seedDCH";
  //      hSvc->FillHisto(hname,clun->GetChannelId()-clu->GetChannelId(),1.);
  //      hname = "PVeto_Clus2Clus_seedDT";
  //      hSvc->FillHisto(hname,clun->GetTime()-clu->GetTime(),1.);
  //    }
  //  }

   return retCode;
}

Bool_t PVetoAnalysis::ProcessDataQuality()
{
  Bool_t retCode = 0;

  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::cout<<"In PVetoAnalysis::ProcessDataQuality"<<std::endl;

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

 // hname = "PVeto_ChId_Hits";
 // hSvc->FillHisto(hname, (Double_t)chId, 1. );
 // hname = "PVeto_ChId_Energy";
 // hSvc->FillHisto(hname, (Double_t)chId,  energy);
 // hname = "PVeto_Energy";
 // hSvc->FillHisto(hname, energy, 1.);

  hname ="PVeto_HitsTimevsChIdHits";
  hSvc->FillHisto2(hname,chId,time, 1.);

  if(fRecoEvent->GetTriggerMask()==1) {

  hname = "PVeto_ChId_Hits_BTFtrigger";
  hSvc->FillHisto(hname, (Double_t)chId, 1. );
  hname = "PVeto_ChId_HitsEnergy_BTFtrigger";
  hSvc->FillHisto(hname, (Double_t)chId,  energy);
  hname = "PVeto_HitsEnergy_BTFtrigger";
  hSvc->FillHisto(hname, energy, 1.);
}
  if(fRecoEvent->GetTriggerMask()==2) {

  hname = "PVeto_ChId_Hits_CRtrigger";
  hSvc->FillHisto(hname, (Double_t)chId, 1. );
  hname = "PVeto_ChId_HitsEnergy_CRtrigger";
  hSvc->FillHisto(hname, (Double_t)chId,  energy);
  hname = "PVeto_HitsEnergy_CRtrigger";
  hSvc->FillHisto(hname, energy, 1.);
}
if(fRecoEvent->GetTriggerMask()==128) {

  hname = "PVeto_ChId_Hits_AUTOtrigger";
  hSvc->FillHisto(hname, (Double_t)chId, 1. );
  hname = "PVeto_ChId_HitsEnergy_AUTOtrigger";
  hSvc->FillHisto(hname, (Double_t)chId,  energy);
  hname = "PVeto_HitsEnergy_AUTOtrigger";
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

  hname = "PVeto_ChId_Clusters_BTFtrigger";
  hSvc->FillHisto(hname, (Double_t)seedId, 1. );
  hname = "PVeto_ChId_ClustersEnergy_BTFtrigger";
  hSvc->FillHisto(hname, (Double_t)seedId,  seedE);
  hname = "PVeto_ClustersEnergy_BTFtrigger";
  hSvc->FillHisto(hname, seedE, 1.);
}
  if(fRecoEvent->GetTriggerMask()==2) {

  hname = "PVeto_ChId_Clusters_CRtrigger";
  hSvc->FillHisto(hname, (Double_t)seedId, 1. );
  hname = "PVeto_ChId_ClustersEnergy_CRtrigger";
  hSvc->FillHisto(hname, (Double_t)seedId,  seedE);
  hname = "PVeto_ClustersEnergy_CRtrigger";
  hSvc->FillHisto(hname, seedE, 1.);
}
if(fRecoEvent->GetTriggerMask()==128) {

  hname = "PVeto_ChId_Clusters_AUTOtrigger";
  hSvc->FillHisto(hname, (Double_t)seedId, 1. );
  hname = "PVeto_ChId_ClustersEnergy_AUTOtrigger";
  hSvc->FillHisto(hname, (Double_t)seedId,  seedE);
  hname = "PVeto_ClustersEnergy_AUTOtrigger";
  hSvc->FillHisto(hname, seedE, 1.);
}

}// end loop on clusters

  hname ="PVeto_ClustersTimevsChIdClusters";
  hSvc->FillHisto2(hname,seedId,seedT, 1.);

  return retCode;
}
