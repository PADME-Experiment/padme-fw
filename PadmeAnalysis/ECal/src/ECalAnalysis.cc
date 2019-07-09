#include "ECalAnalysis.hh"

#include "TECalRecoEvent.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "TRecoEvent.hh"
#include "HistoSvc.hh"
#include <iostream>

ECalAnalysis::ECalAnalysis(): ValidationBase()
{
  //fAlgoName = "ECal";
  InitHistos();
  return;
}
ECalAnalysis::ECalAnalysis(Int_t processingMode, Int_t verbosityFlag): ValidationBase(processingMode, verbosityFlag)
{
  //fAlgoName = "ECal";
  fAnnhilationDeltaTime=2.;
  InitHistos();
  return;
}
ECalAnalysis::~ECalAnalysis()
{
}


Bool_t ECalAnalysis::InitHistosAnalysis()
{

    HistoSvc* hSvc =  HistoSvc::GetInstance();
    std::string hname;
    Int_t nx, ny;
    Double_t xlow, ylow, xup, yup;

    int nBin=500;
    int min=0;
    int max=3000;
    hname = "ECal_SeedEnergy";
    hSvc->BookHisto(hname,nBin,min, max);
    hname = "ECal_ClusterEnergy";
    hSvc->BookHisto(hname,nBin,min, max);
    hname = "ECal_HitEnergy";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_EMaxCl";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_EClLeftSize";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_EClTopBottomSize";
    hSvc->BookHisto(hname,nBin,min, max);
    hname = "ECal_ClusterEnergyInFiducialRegion";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_ClusterInTimeEnergy";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_ClusterInTimeEnergyPlusAloneCl";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_ClusterInTimeEnergy_thrEne";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_ClusterInTimeEnergy_CoeffAngularRequest";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_ClusterInTimeEnergy_CoeffAngularDistanceRequest";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_ClusterInTimeEnergy_angularRequest_FiducialRegion";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_ClusterInTimeEnergy_angularRequest_thrEne";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_ClusterInTimeEnergy_angularRequest_thrEne_FiducialRegion";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_ClusterInTimeEnergy_DeltaPhi20";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_SeedInTimeEnergy";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_SeedInTimeEnergy_DeltaPhi20";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_LeadingSubLeadingInTimeEnergy";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_LeadingSubLeadingInTimeEnergy_DeltaPhi20";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_LeadingSubLeadingInTimeEnergy_Gamma2inDeltaPhi45";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_LeadingSubLeadingInTimeEnergy_DeltaPhi20_Gamma2inDeltaPhi45";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_SeedInTimeEnergy_sameEnergy";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_SeedInTimeEnergy_sameEnergy_DeltaPhi20";
    hSvc->BookHisto(hname,nBin,min, max);
    /*
    nBin=1001;
    min=-0.5;
    max=1000.5;
    hname="ECal_NEvent_ClInTime";
    hSvc->BookHisto(hname, nBin, min, max); */

    nBin=600;
    min=0;
    max=10000;
    hname="ECal_EHitTot";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_EClTot";
    hSvc->BookHisto(hname,nBin,min, max);
    nBin=70;
    min=-50;
    max=50;
    hname="ECAL_DistanceDifferenceGamma1Gamma2-Gamma1Center";
    hSvc->BookHisto(hname,nBin,min, max);

    nBin=31;
    min=-0.5;
    max=30.5;
    hname = "ECal_HitMap";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    hname = "ECal_WeigHitMap";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    hname = "ECal_ClusterMap";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    hname = "ECal_ClusterHitMap";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    hname = "ECal_ClusterHitMap_EnWeig";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    hname = "ECal_AnnihilationClusterMap";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    /*
    hname = "ECal_ClusterMapEv225";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    hname = "ECal_ClusterMapEv237";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    hname = "ECal_ClusterMapEv238";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    hname = "ECal_ClusterMapEv243";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    hname = "ECal_AClusterMapEv260";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    */
    nBin=31;
    min=-15.5;
    max=15.5;
    hname="ECAL_AhhinilationMAPGravMethod";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);


    min=-0.5;
    max= 40.5;
    nBin=41;
    hname = "ECal_NHitsInClus";
    hSvc->BookHisto(hname,nBin,min, max);
    min=35.4;
    max=37.3;
    nBin=100;
    hname= "ECal_SeedTime";
    hSvc->BookHisto(hname,nBin,min, max);
    //to be finalized
    nx = 31.;
    xlow = -15.5;
    xup  =  15.5;
    hname = "ECal_inClus_DCHXseed";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "ECal_inClus_DCHYseed";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "ECal_inClus_DCHXhits";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "ECal_inClus_DCHYhits";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "ECal_Clus2Clus_seedDCHX";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "ECal_Clus2Clus_seedDCHY";
    hSvc->BookHisto(hname, nx, xlow, xup);
    nx = 100.;
    xlow = -25.0;
    xup  =  25.0;
    hname = "ECal_inClus_DTseed";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "ECal_inClus_DThits";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "ECal_Clus2Clus_seedDT";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname="ECal_DTimeEMaxClHit";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname="ECal_DTimeEMaxHitAllHit";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname="ECAL_WeigDstanceGammaGamma";
    hSvc->BookHisto(hname, nx, xlow, xup);
    nx=200;
    xlow=0;
    xup=800;
    hname="ECAL_GammaGammaGravMethod";
    hSvc->BookHisto(hname, nx, xlow, xup);
    nx=200;
    xlow=-10;
    xup=10;
    hname="ECAL_DistanceDeltaPhi";
    hSvc->BookHisto(hname, nx, xlow, xup);
    nx=100;
    xlow=-4;
    xup=4;
    hname="ECAL_AngularGamma1";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname="ECAL_AngularGamma2";
    hSvc->BookHisto(hname, nx, xlow, xup);
    nx=700;
    xlow=-4;
    xup=500;
    hname="ECal_LeadingDiffEnergy";
    hSvc->BookHisto(hname, nx, xlow, xup);
    
    
    //Histo for molteplicity studies
    min=-0.5;
    max= 35.5;
    nBin=36;
    hname = "ECal_NCluster";
    hSvc->BookHisto(hname,nBin,min, max);
    Int_t nBinEn=300;
    Int_t minEn=-50;
    Int_t maxEn=1400;
    Int_t nBinClsize=31;
    Int_t minClsize=-0.5;
    Int_t maxClsize=30.5;
    hname = "ECal_ClSize:ClEnergy";
    hSvc->BookHisto2(hname, nBinEn, minEn, maxEn, nBinClsize, minClsize, maxClsize);
    Int_t nBinX=100;
    Int_t minX=-5;
    Int_t maxX=80;
    hname = "ECal_ClSize:ClX";
    hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinClsize, minClsize, maxClsize);
    Int_t nBinY=100;
    Int_t minY=-5;
    Int_t maxY=80;
    hname = "ECal_ClSize:ClY";
    hSvc->BookHisto2(hname, nBinY, minY, maxY, nBinClsize, minClsize, maxClsize);
    Int_t nBinR=100;
    Int_t minR=-5;
    Int_t maxR=80;
    hname = "ECal_ClSize:ClRadius";
    hSvc->BookHisto2(hname, nBinR, minR, maxR, nBinClsize, minClsize, maxClsize);
    nBin=31;
    min=-0.5;
    max=30.5;
    hname = "ECal_WeigClusterMap";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    min=-400;
    max=400;
    nBin=500;
    hname= "ECal_ClTime";
    hSvc->BookHisto(hname,nBin,min, max);
    nBinEn=500;
    minEn=-50;
    maxEn=2500;
    nBinClsize=36;
    minClsize=-0.5;
    maxClsize=35.5;
    hname = "ECal_NCl:ClTotEnergy";
    hSvc->BookHisto2(hname, nBinEn, minEn, maxEn, nBinClsize, minClsize, maxClsize);

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
  hname="ECal_NCluster";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=500;
  min=0;
  max=500;
  hname = "ECal_HitEnergy";
  hSvcVal->BookHisto(hname,nBin,min, max);
  hname = "ECal_ClusterEnergy";
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

  Double_t eMax  =  0.;
  Int_t    iLead = -1;
  Double_t eTot  =  0.;
  TRecoVHit* hit=NULL;
  TRecoVHit* hitn=NULL;
  Double_t MaxEnHit=0;
  Double_t TimeMaxEnHit=0;
  Double_t ChIDMaxEnHit=-1;
  std::string hname;
  Int_t fNhits = fhitEvent->GetNHits();
  std::cout<<"+++++++++++++++++++++++++++++++++++++++++ ECalAnalysis ... "<<fNhits<<std::endl;
  for (Int_t i=0; i<fNhits; ++i){
    hit = fhitEvent->Hit(i);
    Int_t ix = hit->GetChannelId()/100;
    Int_t iy = hit->GetChannelId()%100;
    hname = "ECal_HitMap";
    hSvc->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);
    Double_t energy = hit->GetEnergy();
    hname = "ECal_HitEnergy";
    hSvc->FillHisto(hname, energy, 1.);
    if(energy>MaxEnHit){
       TimeMaxEnHit=hit->GetTime();
       ChIDMaxEnHit=hit->GetChannelId();
       MaxEnHit=energy;
     }
    eTot += energy;
    
    hname = "ECal_WeigHitMap";
    hSvc->FillHisto2(hname, (Double_t)ix, (Double_t)iy, energy);
    if (energy > eMax) {iLead = i; eMax=energy;}
    Double_t time = hit->GetTime();
    hname= "ECal_ClTime";
    hSvc->FillHisto(hname, time, 1.);
   
   }

   for (Int_t i=0; i<fNhits; ++i){
     hit = fhitEvent->Hit(i);
     hname="ECal_DTimeEMaxHitAllHit";
     if(ChIDMaxEnHit != hit->GetChannelId()) hSvc->FillHisto(hname, TimeMaxEnHit-hit->GetTime(), 1.);
   }
   hname="ECal_EHitTot";
   hSvc->FillHisto(hname, eTot, 1.);
   if (fNhits<1) {std::cout<<"fNhits less than 1 "<<std::endl; return true;}
   TRecoVHit* lHit = fhitEvent->Hit(iLead);
   Int_t lix = lHit->GetChannelId()/100;
   Int_t liy = lHit->GetChannelId()%100;
   Double_t lenergy = lHit->GetEnergy();





   TRecoVCluster* clu=NULL;
   TRecoVCluster* clun=NULL;
   //std::cout<<"ECal sanity checks:"
   Int_t seed;
   Int_t seedId;
   Double_t seedE;
   Double_t seedT;
   Double_t eTotCl;
   Int_t clSize;

   Double_t MaxEn=0;
   Double_t MaxSubLeadEn=0;
   Double_t TimeMaxEn=0;
   Double_t MaxSubLeadT=0;
   Int_t MaxIndex =-1;
   Int_t MaxSubLeadIndex =-1;
   

   Int_t fNclus = fClColl->GetNElements();
   hname = "ECal_NCluster";
   hSvc->FillHisto(hname, fNclus,1.);
   for (Int_t i=0; i<fNclus; ++i){
     clu    = fClColl->Element(i);
     seed   = clu->GetSeed();
     seedId = clu->GetChannelId();
     Int_t ix = clu->GetPosition().X();
     Int_t iy = clu->GetPosition().Y();
     hname = "ECal_ClusterMap";
     hSvc->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);
     hname = "ECal_WeigClusterMap";
    hSvc->FillHisto2(hname, (Double_t)ix, (Double_t)iy, clu->GetEnergy());

     Int_t checkseedId =  fhitEvent->Hit(seed)->GetChannelId();
     if ( checkseedId != seedId ) {
       std::cout<<"ERROR on ECal indices --- seed: clustID, hit(seed).id = "<<seed<<" "<<seedId<<" "<<checkseedId<<std::endl;
     }
     Int_t evNumb = fRecoEvent->GetEventNumber();
     /*if(evNumb==225){
     hname = "ECal_ClusterMapEv225";
     hSvc->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);}
     if(evNumb==237){
     hname = "ECal_ClusterMapEv237";
     hSvc->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);}
     if(evNumb==238){
     hname = "ECal_ClusterMapEv238";
     hSvc->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);}
     if(evNumb==243){
     hname = "ECal_ClusterMapEv243";
     hSvc->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);}
     if(evNumb==260){
     hname = "ECal_AClusterMapEv260";
     hSvc->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);}
     */
     seedE  = clu->GetEnergy();
     hname = "ECal_ClSize:ClEnergy";
     hSvc->FillHisto2(hname, clu->GetEnergy(),clu->GetNHitsInClus(), 1.);
     hname = "ECal_ClSize:ClX";
     hSvc->FillHisto2(hname, clu->GetEnergy(),ix, 1.);
     hname = "ECal_ClSize:ClY";
     hSvc->FillHisto2(hname, clu->GetEnergy(),iy, 1.);
     Double_t radius = sqrt(ix*ix+iy*iy);
     hname = "ECal_ClSize:ClRadius";
     hSvc->FillHisto2(hname, clu->GetEnergy(),radius, 1.);
     if(seedE>MaxEn){
       MaxSubLeadT=TimeMaxEn;
       TimeMaxEn=clu->GetTime();
       MaxSubLeadEn=MaxEn;
       MaxEn=seedE;
       MaxSubLeadIndex=MaxIndex;
       MaxIndex=i;
     }
     eTotCl += seedE;
     hname = "ECal_SeedEnergy";
     hSvc->FillHisto(hname, fhitEvent->Hit(seed)->GetEnergy(), 1.);
     hname = "ECal_ClusterEnergy";
     hSvc->FillHisto(hname, clu->GetEnergy(), 1.);
     if((ix<10 || ix>18) && (iy<10 || iy>18)){
       hname = "ECal_ClusterEnergyInFiducialRegion";
       hSvc->FillHisto(hname, clu->GetEnergy(), 1.);
     }
      if(ix<9){
       hname="ECal_EClLeftSize";
       hSvc->FillHisto(hname, clu->GetEnergy(), 1.);
     }
     if(iy<3 || iy>24){
       hname="ECal_EClTopBottomSize";
       hSvc->FillHisto(hname, clu->GetEnergy(), 1.);
     }

     seedT  = clu->GetTime();
     clSize = clu->GetNHitsInClus();
     hname = "ECal_NHitsInClus";
     hSvc->FillHisto(hname, clSize,1.);
     hname= "ECal_SeedTime";
     hSvc->FillHisto(hname, fhitEvent->Hit(seed)->GetTime(),1.);
     //std::cout<<"Clus seed,                 energy: " << clu->GetEnergy() << " , time: " << clu->GetTime() << "    position (x,y):   ( "<< (Int_t)clu->GetChannelId()/100 <<" , "<< (Int_t)clu->GetChannelId()%100<<" )"<< std::endl;

     std::vector<Int_t> vhinclu = clu->GetHitVecInClus();
     for(int ih=0;ih<clSize; ih++)
     {
       int ihit=vhinclu[ih];
       hitn=fhitEvent->Hit(ihit);
       Int_t ix = hitn->GetPosition().X();
       Int_t iy = hitn->GetPosition().Y();
       hname = "ECal_ClusterHitMap";
       hSvc->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);
       hname = "ECal_ClusterHitMap_EnWeig";
       hSvc->FillHisto2(hname, (Double_t)ix, (Double_t)iy, hitn->GetEnergy());
       //std::cout<<"Hit In Clus,index: " << ihit <<" , energy: " << hitn->GetEnergy() << " , time: " << hitn->GetTime() << "    position (x,y):   ( "<< ix <<" , "<< iy<<" )"<< std::endl;
       //std::cout<<"energy in vector hit " << hitn->GetEnergy() <<std::endl;
       //std::cout<<"channelID seed: " << seedId << " channelId hit in cluster: " << hitn->GetChannelId() << " difference X " << hitn->GetChannelId()/100-seedId/100 << std::endl;
     }




     for (Int_t ih=0; ih<clSize; ++ih){
       Int_t hitInd    = vhinclu[ih];
       //std::cout<<"Hit INDEX in clu : " << hitInd << std::endl;
       if (hitInd == seed) continue;
       hit = fhitEvent->Hit(hitInd);
       hname = "ECal_inClus_DCHXseed";
       hSvc->FillHisto(hname,hit->GetChannelId()/100-seedId/100,1.);
       hname = "ECal_inClus_DCHYseed";
       hSvc->FillHisto(hname,hit->GetChannelId()%100-seedId%100,1.);
       hname = "ECal_inClus_DTseed";
       hSvc->FillHisto(hname,hit->GetTime()-seedT,1.);

       for (Int_t ihn=ih+1; ihn<clSize; ++ihn){
	 Int_t hitIndn    = vhinclu[ihn];
	 if (hitIndn == seed) continue;
	 hitn = fhitEvent->Hit(hitIndn);
	 //std::cout<<" hit comp. n "<<ih<<" vs comp in= "<<ihn<<" chId:  "<<hit->GetChannelId()<<"   "<<hitn->GetChannelId()<<std::endl;
	 hname = "ECal_inClus_DCHXhits";
	 hSvc->FillHisto(hname,hit->GetChannelId()/100-hitn->GetChannelId()/100,1.);
	 hname = "ECal_inClus_DCHYhits";
	 hSvc->FillHisto(hname,hit->GetChannelId()%100-hitn->GetChannelId()%100,1.);
	 hname = "ECal_inClus_DThits";
	 hSvc->FillHisto(hname,hit->GetTime()-hitn->GetTime(),1.);
       }
     }

     for (Int_t in=0; in<fNclus; ++in){
       clun    = fClColl->Element(in);
       //
       hname = "ECal_Clus2Clus_seedDCHX";
       hSvc->FillHisto(hname,clun->GetChannelId()/100-clu->GetChannelId()/100,1.);
       hname = "ECal_Clus2Clus_seedDCHY";
       hSvc->FillHisto(hname,clun->GetChannelId()%100-clu->GetChannelId()%100,1.);
       hname = "ECal_Clus2Clus_seedDT";
       hSvc->FillHisto(hname,clun->GetTime()-clu->GetTime(),1.);
     }

     Int_t indexHitBetterTime = GammaCloseInTime(i, seedT);

     if(indexHitBetterTime>-1){
       clun= fClColl->Element(indexHitBetterTime);
       hname="ECal_ClusterInTimeEnergy";
       hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
       //Int_t evNumb = fRecoEvent->GetEventNumber();
       //hname="ECal_NEvent_ClInTime";
       //hSvc->FillHisto(hname, evNumb, 1.);
       //std::cout<<"GammaGammaInTimeEventNumber " << evNumb << std::endl;

       if(fabs(clun->GetEnergy()-clu->GetEnergy())<550.*0.3){
         hname="ECal_ClusterInTimeEnergy_thrEne";
         hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
       }



       Double_t ixGamma1            = clu->GetPosition().X();
       Int_t iyGamma1               = clu->GetPosition().Y();
       Double_t AngGamma1           = atan2((iyGamma1-14.),(ixGamma1-14.));//rad
       Double_t Radius2Gamma1       = (ixGamma1-14)*(ixGamma1-14)+(iyGamma1-14)*(iyGamma1-14);
       Int_t ixGamma2               = clun->GetPosition().X();
       Int_t iyGamma2               = clun->GetPosition().Y();
       Double_t AngGamma2           = atan2((iyGamma2-14.),(ixGamma2-14.));
       Double_t Radius2Gamma2       = (ixGamma2-14)*(ixGamma2-14)+(iyGamma2-14)*(iyGamma2-14);
       Double_t Distance2Gamma1_2   = (ixGamma1-ixGamma2)*(ixGamma1-ixGamma2)+(iyGamma1-iyGamma2)*(iyGamma1-iyGamma2);
       hname="ECAL_DistanceDifferenceGamma1Gamma2-Gamma1Center";
       hSvc->FillHisto(hname, sqrt(Distance2Gamma1_2) - sqrt(Radius2Gamma1),1.);

       RatioPhiMethod(i, indexHitBetterTime, AngGamma1, AngGamma2, Distance2Gamma1_2, Radius2Gamma1);

       AnnihilationPhiSimmetry(i, indexHitBetterTime, AngGamma1, AngGamma2, Distance2Gamma1_2, Radius2Gamma1);

       AnnihilationGravitationalMethod(i, indexHitBetterTime);

       //std::cout<<"Phi distance " << fabs(AngGamma1-AngGamma2) << std::endl;


     }
     else{
       hname="ECAL_ClusterInTimeEnergyPlusAloneCl";
       hSvc->FillHisto(hname,clu->GetEnergy(),1.);
     }
    //close loop on cl
   }



   hname="ECal_EClTot";
   hSvc->FillHisto(hname, eTotCl, 1.);
   hname = "ECal_NCl:ClTotEnergy";
   hSvc->FillHisto2(hname, eTotCl, fNclus, 1.);
   hname="ECal_EMaxCl";
   hSvc->FillHisto(hname, MaxEn, 1.);
   for (Int_t i=0; i<fNhits; ++i){
    hit = fhitEvent->Hit(i);
    hname="ECal_DTimeEMaxClHit";
    hSvc->FillHisto(hname, TimeMaxEn-hit->GetTime(), 1.);
   }
   
   Int_t SeedIndexHitBetterTime=-1;
   SeedIndexHitBetterTime = GammaCloseInTime(MaxIndex, TimeMaxEn);
   if(SeedIndexHitBetterTime>-1)AnnihilationLeading(MaxIndex, SeedIndexHitBetterTime);
  
          
     if(MaxSubLeadIndex >-1 && MaxIndex>-1 ){
     if(fabs(TimeMaxEn-MaxSubLeadT)<fAnnhilationDeltaTime )AnnihilationLeadingSubLeading(MaxIndex, MaxSubLeadIndex, " ");
   }
   
   if(MaxIndex>-1){
     Int_t LeadingDeltaPhi = ReserchGamma2InDeltaPhi(MaxIndex);
     Double_t timeLeadingDeltaPhi = (fClColl->Element(LeadingDeltaPhi))->GetTime();
     if(LeadingDeltaPhi >-1){
       if(fabs(TimeMaxEn-timeLeadingDeltaPhi)<fAnnhilationDeltaTime )AnnihilationLeadingSubLeading(MaxIndex, LeadingDeltaPhi, "_Gamma2inDeltaPhi45");
     }
   }
   
  Int_t      chId; 
  Double_t energy;
  Double_t   time;
  Int_t      chIdn;
  Double_t energyn;
  Double_t   timen;

  fNhits = fhitEvent->GetNHits();
  fNclus = fClColl->GetNElements();


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
  Int_t fNhits = fhitEvent->GetNHits();
  for (Int_t i=0; i<fNhits; ++i){
    hit = fhitEvent->Hit(i);
    Int_t ix = hit->GetChannelId()/100;
    Int_t iy = hit->GetChannelId()%100;
    //Int_t ix=position.X();
    //Int_t iy=position.Y();

    hname = "ECal_HitMap";
    hSvcVal->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);


   }


   TRecoVCluster* clu=NULL;
   Int_t fNclus = fClColl->GetNElements();

   std::cout<<"NClusters:  " << fNclus << std::endl;
   for (Int_t i=0; i<fNclus; ++i){
     clu    = fClColl->Element(i);
     Int_t ix = clu->GetChannelId()/100;
     Int_t iy = clu->GetChannelId()%100;
     //Int_t ix=position.X();
     //Int_t iy=position.Y();
     hname = "ECal_ClusterMap";
     hSvcVal->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);
   }


   return retCode;
}

Int_t ECalAnalysis::GammaCloseInTime(Int_t i, Double_t timeGamma1){
  Double_t betterDeltaTime=10.;
  Double_t distanceInTime=0.;
  Int_t indexHitBetterTime=-1;
  TRecoVCluster* clun=NULL;
  Int_t fNclus = fClColl->GetNElements();
  for (Int_t in=i+1; in<fNclus; ++in){
    clun    = fClColl->Element(in);
    distanceInTime=clun->GetTime()-timeGamma1;
    if(fabs(distanceInTime)<fAnnhilationDeltaTime)
    {
       //std::cout<<"In loop for better time, delta time "<< fabs(distanceInTime) << " betterDeltatime "<<  betterDeltaTime<<std::endl;
       if(abs(distanceInTime)< betterDeltaTime)
      {
         betterDeltaTime=distanceInTime;
         indexHitBetterTime=in;
       }
       //Annihilation=true;
       in=fNclus;
    }
  }
  return indexHitBetterTime;
}

void ECalAnalysis::RatioPhiMethod(Int_t indexGamma1, Int_t indexGamma2,Double_t AngGamma1, Double_t AngGamma2, Double_t Distance2Gamma1_2, Double_t Radius2Gamma1)
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  TRecoVCluster* clu=NULL;
  TRecoVCluster* clun=NULL;
  clu     = fClColl->Element(indexGamma1);
  clun    = fClColl->Element(indexGamma2);

  if((AngGamma2*AngGamma1<0 && (fabs(AngGamma1-AngGamma2)-3.14)<0.1) || (AngGamma2*AngGamma1>0 && (fabs(AngGamma1+AngGamma2)-3.14)< 0.1))
  {
   hname="ECal_ClusterInTimeEnergy_CoeffAngularRequest";
   hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);

   if(Distance2Gamma1_2 > Radius2Gamma1){
     hname="ECal_ClusterInTimeEnergy_CoeffAngularDistanceRequest";
     hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
     hname = "ECal_AnnihilationClusterMap";
     hSvc->FillHisto2(hname, clu->GetPosition().X(), clu->GetPosition().Y(), 1.);
     hSvc->FillHisto2(hname, clun->GetPosition().X(), clun->GetPosition().Y(), 1.);

     if(fabs(clun->GetEnergy()-clu->GetEnergy())<550.*0.3){
       hname="ECal_ClusterInTimeEnergy_angularRequest_thrEne";
       hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);

     }
   }
 }

}

void ECalAnalysis::AnnihilationPhiSimmetry(Int_t indexGamma1, Int_t indexGamma2, Double_t AngGamma1, Double_t AngGamma2, Double_t Distance2Gamma1_2, Double_t Radius2Gamma1)
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  TRecoVCluster* clu=NULL;
  TRecoVCluster* clun=NULL;
  clu     = fClColl->Element(indexGamma1);
  clun    = fClColl->Element(indexGamma2);

  if(sqrt(Distance2Gamma1_2) > sqrt(Radius2Gamma1)){
    hname="ECAL_DistanceDeltaPhi";
    hSvc->FillHisto(hname,AngGamma1-AngGamma2,1.);
    hname="ECAL_AngularGamma1";
    hSvc->FillHisto(hname,AngGamma1,1.);
    hname="ECAL_AngularGamma2";
    hSvc->FillHisto(hname,AngGamma2,1.);
    if(AngGamma1*AngGamma2<0){
     if(fabs(fabs(AngGamma1-AngGamma2)-3.14)< 4*0.09) //0.09=5deg
     {
         hname="ECal_ClusterInTimeEnergy_DeltaPhi20";
         hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
     }
    }
   else{
     if(fabs(fabs(AngGamma1+AngGamma2)-3.14)< 4*0.09) //0.09=5deg
     {
         hname="ECal_ClusterInTimeEnergy_DeltaPhi20";
         hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
     }
   }
 }

}


void ECalAnalysis::AnnihilationLeading(Int_t indexGamma1, Int_t indexGamma2)
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  TRecoVCluster* clu=NULL;
  TRecoVCluster* clun=NULL;
  clu     = fClColl->Element(indexGamma1);
  clun    = fClColl->Element(indexGamma2);
  
   Double_t ixGamma1            = clu->GetPosition().X();
   Int_t iyGamma1               = clu->GetPosition().Y();
   Double_t AngGamma1           = atan2((iyGamma1-14.),(ixGamma1-14.));//rad
   Double_t Radius2Gamma1       = (ixGamma1-14)*(ixGamma1-14)+(iyGamma1-14)*(iyGamma1-14);
   Int_t ixGamma2               = clun->GetPosition().X();
   Int_t iyGamma2               = clun->GetPosition().Y();
   Double_t AngGamma2           = atan2((iyGamma2-14.),(ixGamma2-14.));
   Double_t Radius2Gamma2       = (ixGamma2-14)*(ixGamma2-14)+(iyGamma2-14)*(iyGamma2-14);
   Double_t Distance2Gamma1_2   = (ixGamma1-ixGamma2)*(ixGamma1-ixGamma2)+(iyGamma1-iyGamma2)*(iyGamma1-iyGamma2);

  hname="ECal_SeedInTimeEnergy";
  hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
  Bool_t sameEnergy=false;
  hname="ECal_LeadingDiffEnergy";
  hSvc->FillHisto(hname, clu->GetEnergy()-clun->GetEnergy(), 1. );
  if(fabs(clun->GetEnergy()-clu->GetEnergy())<100)
  {
     sameEnergy=true;
     hname="ECal_SeedInTimeEnergy_sameEnergy";
     hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
  }
  
  
  if(sqrt(Distance2Gamma1_2) > sqrt(Radius2Gamma1) ){
    if(AngGamma1*AngGamma2<0){
     if(fabs(fabs(AngGamma1-AngGamma2)-3.14)< 4*0.09) //0.09=5deg
     {
         hname="ECal_SeedInTimeEnergy_DeltaPhi20";
         hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
         if(sameEnergy){
           hname="ECal_SeedInTimeEnergy_sameEnergy_DeltaPhi20";
           hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
         }
     }
    }
   else{
     if(fabs(fabs(AngGamma1+AngGamma2)-3.14)< 4*0.09) //0.09=5deg
     {
         hname="ECal_SeedInTimeEnergy_DeltaPhi20";
         hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
         if(sameEnergy){
           hname="ECal_SeedInTimeEnergy_sameEnergy_DeltaPhi20";
           hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
         }
     }
   }
 }

}

void ECalAnalysis::AnnihilationLeadingSubLeading(Int_t indexGamma1, Int_t indexGamma2, TString name)
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  TRecoVCluster* clu=NULL;
  TRecoVCluster* clun=NULL;
  clu     = fClColl->Element(indexGamma1);
  clun    = fClColl->Element(indexGamma2);
  
   Double_t ixGamma1            = clu->GetPosition().X();
   Int_t iyGamma1               = clu->GetPosition().Y();
   Double_t AngGamma1           = atan2((iyGamma1-14.),(ixGamma1-14.));//rad
   Double_t Radius2Gamma1       = (ixGamma1-14)*(ixGamma1-14)+(iyGamma1-14)*(iyGamma1-14);
   Int_t ixGamma2               = clun->GetPosition().X();
   Int_t iyGamma2               = clun->GetPosition().Y();
   Double_t AngGamma2           = atan2((iyGamma2-14.),(ixGamma2-14.));
   Double_t Radius2Gamma2       = (ixGamma2-14)*(ixGamma2-14)+(iyGamma2-14)*(iyGamma2-14);
   Double_t Distance2Gamma1_2   = (ixGamma1-ixGamma2)*(ixGamma1-ixGamma2)+(iyGamma1-iyGamma2)*(iyGamma1-iyGamma2);

  hname="ECal_LeadingSubLeadingInTimeEnergy"+name;
  hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
  if(sqrt(Distance2Gamma1_2) > sqrt(Radius2Gamma1) ){
    if(AngGamma1*AngGamma2<0){
     if(fabs(fabs(AngGamma1-AngGamma2)-3.14)< 4*0.09) //0.09=5deg
     {
         hname="ECal_LeadingSubLeadingInTimeEnergy_DeltaPhi20"+name;
         hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
     }
    }
   else{
     if(fabs(fabs(AngGamma1+AngGamma2)-3.14)< 4*0.09) //0.09=5deg
     {
         hname="ECal_LeadingSubLeadingInTimeEnergy_DeltaPhi20"+name;
         hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
     }
   }
 }

}



void ECalAnalysis::AnnihilationGravitationalMethod(Int_t indexGamma1, Int_t indexGamma2)
{
  if(indexGamma1<0) return;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  TRecoVCluster* clu=NULL;
  TRecoVCluster* clun=NULL;
  clu     = fClColl->Element(indexGamma1);
  clun    = fClColl->Element(indexGamma2);

  Int_t ixGamma1               = clu->GetPosition().X();
  Int_t iyGamma1               = clu->GetPosition().Y();
  Int_t ixGamma2               = clun->GetPosition().X();
  Int_t iyGamma2               = clun->GetPosition().Y();
  Double_t WeigX               = (ixGamma1*clu->GetEnergy()+ixGamma2*clun->GetEnergy())/(clu->GetEnergy()*clun->GetEnergy());
  Double_t WeigY               = (iyGamma1*clu->GetEnergy()+iyGamma2*clun->GetEnergy())/(clu->GetEnergy()*clun->GetEnergy());
  hname="ECAL_AhhinilationMAPGravMethod";
  hSvc->FillHisto2(hname, WeigX, WeigY, 1.);
  Double_t WeigDistance        = sqrt(WeigX*WeigX+WeigY*WeigY);
  hname="ECAL_WeigDstanceGammaGamma";
  hSvc->FillHisto(hname,WeigDistance,1.);
  hname="ECAL_GammaGammaGravMethod";
  hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);

}

Int_t ECalAnalysis::ReserchGamma2InDeltaPhi(Int_t indexGamma1){

   TRecoVCluster* clu=NULL;
   TRecoVCluster* clun=NULL;
   Double_t maxEne=0;
   Int_t maxIndex=0;
   clu     = fClColl->Element(indexGamma1);
   Double_t ixGamma1            = clu->GetPosition().X();
   Int_t iyGamma1               = clu->GetPosition().Y();
   Double_t AngGamma1           = atan2((iyGamma1-14.),(ixGamma1-14.));//rad
   Double_t Radius2Gamma1       = (ixGamma1-14)*(ixGamma1-14)+(iyGamma1-14)*(iyGamma1-14);
   
   Int_t fNclus = fClColl->GetNElements();
   for (Int_t in=indexGamma1+1; in<fNclus; ++in){
     clun    = fClColl->Element(in);
     Int_t ixGamma2               = clun->GetPosition().X();
     Int_t iyGamma2               = clun->GetPosition().Y();
     Double_t AngGamma2           = atan2((iyGamma2-14.),(ixGamma2-14.));
     Double_t Radius2Gamma2       = (ixGamma2-14)*(ixGamma2-14)+(iyGamma2-14)*(iyGamma2-14);
     Double_t Distance2Gamma1_2   = (ixGamma1-ixGamma2)*(ixGamma1-ixGamma2)+(iyGamma1-iyGamma2)*(iyGamma1-iyGamma2);

    if(sqrt(Distance2Gamma1_2) > sqrt(Radius2Gamma1) ){
      if(AngGamma1*AngGamma2<0){
       if(fabs(fabs(AngGamma1-AngGamma2)-3.14)< 9*0.09) //0.09=5deg
       {
           if(clun->GetEnergy()>maxEne) maxIndex=in;
       }
      }
     else{
       if(fabs(fabs(AngGamma1+AngGamma2)-3.14)< 9*0.09) //0.09=5deg
       {
         if(clun->GetEnergy()>maxEne) maxIndex=in;
       }
     }
   }
 }
 return maxIndex;
}

