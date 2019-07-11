#include "ECalAnalysis.hh"
#include "TECalRecoEvent.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "HistoSvc.hh"
#include <iostream>

ECalAnalysis::ECalAnalysis(): ValidationBase()
{
  fAlgoName = "ECal";
  InitHistos();
  return;
}
ECalAnalysis::ECalAnalysis(Int_t processingMode, Int_t verbosityFlag): ValidationBase(processingMode, verbosityFlag)
{
  fAlgoName = "ECal";
  InitHistos();
  return;
}
ECalAnalysis::~ECalAnalysis()
{
}

Bool_t ECalAnalysis::InitHistosAnalysis()
{


    return true;
}

Bool_t ECalAnalysis::InitHistosValidation()
{
  //Validation histos
  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  std::string hname;
  int nBin, min, max;
  nBin=200;
  min=150;
  max=350;
  hname="ECal_NHits";
  hSvcVal->BookHisto(hname, nBin, min, max);
  min=40;
  max=200;
  hname="ECal_NCluster";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=500;
  min=0;
  max=500;
  hname = "ECal_HitEnergy";
  hSvcVal->BookHisto(hname,nBin,min, max);
  hname = "ECal_ClusterEnergy";
  hSvcVal->BookHisto(hname,nBin,min, max);
  nBin=500;
  min=0;
  max=1000;
  hname="ECal_TwoPhotonEnergy_TimeCoincidenceRequest3ns()";
  hSvcVal->BookHisto(hname,nBin,min, max);
  nBin=1000;
  min=0;
  max=15000;
  hname = "ECal_HitTotEnergy";
  hSvcVal->BookHisto(hname,nBin,min, max);
  hname = "ECal_ClusterTotEnergy";
  hSvcVal->BookHisto(hname,nBin,min, max);
  nBin=800;
  min=-400;
  max=400;
  hname = "ECal_HitTime";
  hSvcVal->BookHisto(hname,nBin, min, max);
  hname = "ECal_ClusterTime";
  hSvcVal->BookHisto(hname,nBin, min, max);
  nBin=30;
  min=0;
  max=30;
  hname = "ECal_HitMap";
  hSvcVal->BookHisto2(hname, nBin, min, max, nBin, min, max);
  hname = "ECal_ClusterMap";
  hSvcVal->BookHisto2(hname, nBin, min, max, nBin, min, max);
  nBin=300;
  min=-1;
  max=30;
  hname="ECal_HitXPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="ECal_ClusterXPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="ECal_HitYPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="ECal_ClusterYPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=300;
  min=-1;
  max=30;
  hname="ECal_HitZPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="ECal_ClusterZPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=3000;
  min=0;
  max=3000;
  hname="ECal_HitChannelId";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="ECal_ClusterSeedChannelId";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=16;
  min=0;
  max=15;
  hname="ECal_NHitInCluster";
  hSvcVal->BookHisto(hname, nBin, min, max);
  return true;
}


Bool_t ECalAnalysis::ProcessAnalysis()
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
  
  for (Int_t i=0; i<fNhits; ++i){
    hit    = fhitEvent->Hit(i);
    chId   = hit->GetChannelId();
    energy = hit->GetEnergy();
    time   = hit->GetTime();

   (hSvc->myEvt).NTNECal_Hits=fhitEvent->GetNHits();
   (hSvc->myEvt).NTECal_Hits_ChannelId[i]=(Double_t)chId;
   (hSvc->myEvt).NTECal_Hits_Energy[i]=hit->GetEnergy();
   (hSvc->myEvt).NTECal_Hits_Time[i]=hit->GetTime();
   (hSvc->myEvt).NTECal_Hits_Xpos[i]=hit->GetPosition().X();
   (hSvc->myEvt).NTECal_Hits_Ypos[i]=hit->GetPosition().Y();
   (hSvc->myEvt).NTECal_Hits_Zpos[i]=hit->GetPosition().Z();
  }

    //fillClustersFlatNTP  

   for (Int_t j=0; j<fNclus; ++j){
     clu    = fClColl->Element(j);
     seedId = clu->GetChannelId();
  
   (hSvc->myEvt).NTNECal_Clusters= fClColl->GetNElements();
   (hSvc->myEvt).NTECal_Clusters_ChannelId[j]=Double_t(clu->GetChannelId());
   (hSvc->myEvt).NTECal_Clusters_Energy[j]=clu->GetEnergy();
   (hSvc->myEvt).NTECal_Clusters_Time[j]=clu->GetTime();
   (hSvc->myEvt).NTECal_Clusters_Xpos[j]=clu->GetPosition().X();
   (hSvc->myEvt).NTECal_Clusters_Ypos[j]=clu->GetPosition().Y();
   (hSvc->myEvt).NTECal_Clusters_Zpos[j]=clu->GetPosition().Z();
  }

  // HistoSvc* hSvc =  HistoSvc::GetInstance();

  // Double_t eMax  =  0.;
  // Int_t    iLead = -1;
  // Double_t eTot  =  0.;
  // TRecoVHit* hit=NULL;
  // TRecoVHit* hitn=NULL;
  // std::string hname;
  // Int_t fNhits = fhitEvent->GetNHits();
  // std::cout<<"+++++++++++++++++++++++++++++++++++++++++ ECalAnalysis ... "<<fNhits<<std::endl;
  // for (Int_t i=0; i<fNhits; ++i){
  //   hit = fhitEvent->Hit(i);
  //   Int_t ix = hit->GetChannelId()/100;
  //   Int_t iy = hit->GetChannelId()%100;
  //   Double_t energy = hit->GetEnergy();
  //   eTot += energy;
  //   if (energy > eMax) {iLead = i; eMax=energy;}
  //   Double_t time = hit->GetTime();

  //   // ntuple variables
  //   hSvc->myEvt.NTECluster[i]=hit->GetEnergy();
  //   hSvc->myEvt.NTTCluster[i]=hit->GetTime();
  //   hSvc->myEvt.NTXCluster[i]=hit->GetChannelId()/100;
  //   hSvc->myEvt.NTYCluster[i]=hit->GetChannelId()%100;

  //  }

  //  if (fNhits<1) return true;
  //  TRecoVHit* lHit = fhitEvent->Hit(iLead);
  //  Int_t lix = lHit->GetChannelId()/100;
  //  Int_t liy = lHit->GetChannelId()%100;
  //  Double_t lenergy = lHit->GetEnergy();

  //  hname = "ECal_LeadingHitTime";
  //  hSvc->FillHisto(hname,lHit->GetTime(),1.);
  //  hname = "ECal_LeadingHitEnergy";
  //  hSvc->FillHisto(hname,lenergy,1.);
  //  hname = "ECal_LeadingHitMap";
  //  hSvc->FillHisto2(hname, lix, liy, 1.);
  //  hname = "ECal_LeadingEneMap";
  //  hSvc->FillHisto2(hname, lix, liy, lenergy);


  //  TRecoVCluster* clu=NULL;
  //  TRecoVCluster* clun=NULL;
  //  //std::cout<<"ECal sanity checks:"
  //  Int_t seed;
  //  Int_t seedId;
  //  Double_t seedE;
  //  Double_t seedT;
  //  Double_t eTotCl;
  //  Int_t clSize;
  //  Int_t fNclus = fClColl->GetNElements();
  //  for (Int_t i=0; i<fNclus; ++i){
  //    clu    = fClColl->Element(i);
  //    seed   = clu->GetSeed();
  //    seedId = clu->GetChannelId();
  //    Int_t checkseedId =  fhitEvent->Hit(seed)->GetChannelId();
  //    if ( checkseedId != seedId ) {
  //      std::cout<<"ERROR on ECal indices --- seed: clustID, hit(seed).id = "<<seed<<" "<<seedId<<" "<<checkseedId<<std::endl;
  //    }
  //    //     else
  //    //       {
  //    //	 std::cout<<"++++++++++++++++++++     Sanity check OK ... "<<std::endl;
  //    //       }
  //    seedE  = clu->GetEnergy();
  //    seedT  = clu->GetTime();
  //    clSize = clu->GetNHitsInClus();
  //    //std::cout<<"ECal:: cluster n. "<<i<<" size, seed  "<<clSize<<" "<<seed<<"  chId, E, T "<<seedId<<" "<<seedE<<" "<<seedT<<std::endl;
  //    std::vector<Int_t> vhinclu = clu->GetHitVecInClus();
  //    for (Int_t ih=0; ih<clSize; ++ih){
  //      //std::cout<<"ECal:: cluster n. "<<i<<" hit comp. n "<<ih<<std::endl;
  //      Int_t hitInd    = vhinclu[ih];
  //      //std::cout<<"ECal:: cluster n. "<<i<<" hit comp. n "<<ih<<" hitIndex = "<<hitInd<<std::endl;
  //      if (hitInd == seed) {std::cout<<"ECal:: cluster n. "<<i<<" hit comp. n "<<ih<<"- skipping seed"<<std::endl;continue;}
  //      hit = fhitEvent->Hit(hitInd);
  //      //std::cout<<"ECal:: cluster n. "<<i<<" hit comp. n "<<ih<<" hitIndex = "<<hitInd<<" chid, E, T "
  //      //	<<hit->GetChannelId()<<" "<<hit->GetEnergy()<<" "<<hit->GetTime()<<std::endl;
  //      hname = "ECal_inClus_DCHXseed";
  //      hSvc->FillHisto(hname,hit->GetChannelId()/100-seedId/100,1.);
  //      hname = "ECal_inClus_DCHYseed";
  //      hSvc->FillHisto(hname,hit->GetChannelId()%100-seedId%100,1.);
  //      hname = "ECal_inClus_DTseed";
  //      hSvc->FillHisto(hname,hit->GetTime()-seedT,1.);
  //      for (Int_t ihn=ih+1; ihn<clSize; ++ihn){
  // 	 Int_t hitIndn    = vhinclu[ihn];
  // 	 //if (hitIndn == seed) continue;
  // 	 hitn = fhitEvent->Hit(hitIndn);
  // 	 //std::cout<<"ECal:: cluster n. "<<i<<" hit comp. n "<<ih<<" vs comp in="<<ihn<<" chId, T "<<hitn->GetChannelId()<<" "<<hitn->GetTime()<<std::endl;
  // 	 hname = "ECal_inClus_DCHXhits";
  // 	 hSvc->FillHisto(hname,hit->GetChannelId()/100-hitn->GetChannelId()/100,1.);
  // 	 hname = "ECal_inClus_DCHYhits";
  // 	 hSvc->FillHisto(hname,hit->GetChannelId()%100-hitn->GetChannelId()%100,1.);
  // 	 hname = "ECal_inClus_DThits";
  // 	 hSvc->FillHisto(hname,hit->GetTime()-hitn->GetTime(),1.);
  //      }
  //    }
  //    for (Int_t in=0; in<fNclus; ++in){
  //      clun    = fClColl->Element(in);
  //      //
  //      hname = "ECal_Clus2Clus_seedDCHX";
  //      hSvc->FillHisto(hname,clun->GetChannelId()/100-clu->GetChannelId()/100,1.);
  //      hname = "ECal_Clus2Clus_seedDCHY";
  //      hSvc->FillHisto(hname,clun->GetChannelId()/100-clu->GetChannelId()%100,1.);
  //      hname = "ECal_Clus2Clus_seedDT";
  //      hSvc->FillHisto(hname,clun->GetTime()-clu->GetTime(),1.);
  //    }
  //  }


   return retCode;
}


Bool_t ECalAnalysis::ProcessValidation()
{
  std::cout <<"I'm in ECalAnalysis::ProcessValidation" << std::endl;
  
  ValidationBase::ProcessValidation("ECal");
  
  
  Bool_t retCode = 0;

  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  TRecoVHit* hit=NULL;
  std::string hname;
  Double_t ETotHit=0.;
  Int_t fNhits = fhitEvent->GetNHits();
  for (Int_t i=0; i<fNhits; ++i){
    hit = fhitEvent->Hit(i);
    Int_t ix = hit->GetPosition().X();
    Int_t iy = hit->GetPosition().Y();
    ETotHit += hit->GetEnergy();
    //Int_t ix=position.X();
    //Int_t iy=position.Y();
  
    hname = "ECal_HitMap";
    hSvcVal->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);

    
   }
   hname="ECal_HitTotEnergy";
   hSvcVal->FillHisto(hname, ETotHit, 1.);

   TRecoVCluster* clu=NULL;
   TRecoVCluster* clun=NULL;
   Int_t fNclus = fClColl->GetNElements();
   Double_t ETotCl=0.;
   for (Int_t i=0; i<fNclus; ++i){
     clu    = fClColl->Element(i);
     Int_t ix = clu->GetPosition().X();
     Int_t iy = clu->GetPosition().Y();
     ETotCl+=clu->GetEnergy();
     //Int_t ix=position.X();
     //Int_t iy=position.Y();
     hname = "ECal_ClusterMap";
     hSvcVal->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);
     for(int j=i; j< fNclus; j++){
       clun   = fClColl->Element(j);
       if(fabs(clu->GetTime() - clun->GetTime())<3.)
       {
          hname="ECal_TwoPhotonEnergy_TimeCoincidenceRequest3ns()";
          hSvcVal->FillHisto(hname, clu->GetEnergy()+clun->GetEnergy());
       }
     }
   }
   hname="ECal_ClusterTotEnergy";
   hSvcVal->FillHisto(hname, ETotCl, 1.);


   return retCode;
}
