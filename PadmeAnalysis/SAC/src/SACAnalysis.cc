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
  fAnnhilationDeltaTime=1.;
  fAnnhilationDeltaSpaceGravCenter=1.;
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
    HistoSvc* hSvc =  HistoSvc::GetInstance();
    std::string hname;

    int nBin=500;
    int min=0;
    int max=3000;
    hname = "SAC_SeedEnergy";
    hSvc->BookHisto(hname,nBin,min, max);
    hname = "SAC_ClusterEnergy";
    hSvc->BookHisto(hname,nBin,min, max);
    hname = "SAC_HitEnergy";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="SAC_EMaxCl";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="SAC_EClLeftSize";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="SAC_EClTopBottomSize";
    hSvc->BookHisto(hname,nBin,min, max);
    hname = "SAC_ClusterEnergyInFiducialRegion";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="SAC_ClusterInTimeEnergy";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="SAC_ClusterInTimeEnergyPlusAloneCl";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="SAC_AnnihilationGravCenterCloseInZero";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="SAC_AnnihilationGravCenterCloseInZero_PlusAloneCl";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="SAC_ClusterInTimeEnergy_thrEne";
    hSvc->BookHisto(hname,nBin,min, max);
    //hname="SAC_ClusterInTimeEnergy_CoeffAngularRequest";
    //hSvc->BookHisto(hname,nBin,min, max);
    //hname="SAC_ClusterInTimeEnergy_CoeffAngularDistanceRequest";
    //hSvc->BookHisto(hname,nBin,min, max);
    //hname="SAC_ClusterInTimeEnergy_angularRequest_FiducialRegion";
    //hSvc->BookHisto(hname,nBin,min, max);
    //hname="SAC_ClusterInTimeEnergy_angularRequest_thrEne";
    //hSvc->BookHisto(hname,nBin,min, max);
    //hname="SAC_ClusterInTimeEnergy_angularRequest_thrEne_FiducialRegion";
    //hSvc->BookHisto(hname,nBin,min, max);
    //hname="SAC_ClusterInTimeEnergy_DeltaPhi20";
    //hSvc->BookHisto(hname,nBin,min, max);
    hname="SAC_SeedInTimeEnergy";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="SAC_SeedInTimeEnergy_AloneCl";
    hSvc->BookHisto(hname,nBin,min, max);
    //hname="SAC_SeedInTimeEnergy_DeltaPhi20";
    //hSvc->BookHisto(hname,nBin,min, max);
    hname="SAC_LeadingSubLeadingInTimeEnergy";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="SAC_LeadingSubLeadingInTimeEnergy_AloneCl";
    hSvc->BookHisto(hname,nBin,min, max);
    //hname="SAC_LeadingSubLeadingInTimeEnergy_DeltaPhi20";
    //hSvc->BookHisto(hname,nBin,min, max);
    //hname="SAC_LeadingSubLeadingInTimeEnergy_Gamma2inDeltaPhi45";
    //hSvc->BookHisto(hname,nBin,min, max);
    //hname="SAC_LeadingSubLeadingInTimeEnergy_DeltaPhi20_Gamma2inDeltaPhi45";
    //hSvc->BookHisto(hname,nBin,min, max);
    hname="SAC_SeedInTimeEnergy_sameEnergy";
    hSvc->BookHisto(hname,nBin,min, max);
    //hname="SAC_SeedInTimeEnergy_sameEnergy_DeltaPhi20";
    //hSvc->BookHisto(hname,nBin,min, max);
    /*
    nBin=1001;
    min=-0.5;
    max=1000.5;
    hname="SAC_NEvent_ClInTime";
    hSvc->BookHisto(hname, nBin, min, max); */

    nBin=600;
    min=0;
    max=10000;
    hname="SAC_EHitTot";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="SAC_EClTot";
    hSvc->BookHisto(hname,nBin,min, max);
    nBin=70;
    min=-50;
    max=50;
    //hname="SAC_DistanceDifferenceGamma1Gamma2-Gamma1Center";
    //hSvc->BookHisto(hname,nBin,min, max);

    nBin=6;
    min=-0.5;
    max=5.5;
    hname = "SAC_HitMap";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    hname = "SAC_WeigHitMap";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    hname = "SAC_ClusterMap";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    hname = "SAC_ClusterHitMap";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    hname = "SAC_ClusterHitMap_EnWeig";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    hname = "SAC_AnnihilationClusterMap";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);
    hname="SAC_AhhinilationMAPGravMethod";
    hSvc->BookHisto2(hname, nBin, min, max, nBin, min, max);


    min=-0.5;
    max= 201;
    nBin=201;
    hname = "SAC_NHitsInClus";
    hSvc->BookHisto(hname,nBin,min, max);
    min=-0.5;
    max= 100.5;
    nBin=101;
    hname = "SAC_NCluster";
    hSvc->BookHisto(hname,nBin,min, max);
    
    //to be finalized
    Int_t nx;
    Double_t xlow, xup;
    nx = 31.;
    xlow = -15.5;
    xup  =  15.5;
    hname = "SAC_inClus_DCHXseed";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "SAC_inClus_DCHYseed";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "SAC_inClus_DCHXhits";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "SAC_inClus_DCHYhits";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "SAC_Clus2Clus_seedDCHX";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "SAC_Clus2Clus_seedDCHY";
    hSvc->BookHisto(hname, nx, xlow, xup);
    nx = 100.;
    xlow = -25.0;
    xup  =  25.0;
    hname = "SAC_inClus_DTseed";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "SAC_inClus_DThits";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname = "SAC_Clus2Clus_seedDT";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname="SAC_DTimeEMaxClHit";
    hSvc->BookHisto(hname, nx, xlow, xup);
    hname="SAC_DTimeEMaxHitAllHit";
    hSvc->BookHisto(hname, nx, xlow, xup);
    //hname="SAC_WeigDstanceGammaGamma";
    //hSvc->BookHisto(hname, nx, xlow, xup);
    //nx=200;
    //xlow=0;
    //xup=800;
    //hname="SAC_GammaGammaGravMethod";
    //hSvc->BookHisto(hname, nx, xlow, xup);
    //nx=200;
    //xlow=-10;
    //xup=10;
    //hname="SAC_DistanceDeltaPhi";
    //hSvc->BookHisto(hname, nx, xlow, xup);
    //nx=100;
    //xlow=-4;
    //xup=4;
    //hname="SAC_AngularGamma1";
    //hSvc->BookHisto(hname, nx, xlow, xup);
    //hname="SAC_AngularGamma2";
    //hSvc->BookHisto(hname, nx, xlow, xup);
   /* nx=700;
    xlow=-4;
    xup=500;
    hname="SAC_LeadingDiffEnergy";
    hSvc->BookHisto(hname, nx, xlow, xup);*/

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
  std::cout<<"+++++++++++++++++++++++++++++++++++++++++ SACAnalysis ... "<<fNhits<<std::endl;
  for (Int_t i=0; i<fNhits; ++i){
    hit = fhitEvent->Hit(i);
    Int_t ix = hit->GetChannelId()/10;
    Int_t iy = hit->GetChannelId()%10;
    hname = "SAC_HitMap";
    hSvc->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);
    Double_t energy = hit->GetEnergy();
    hname = "SAC_HitEnergy";
    hSvc->FillHisto(hname, energy, 1.);
    if(energy>MaxEnHit){
       TimeMaxEnHit=hit->GetTime();
       ChIDMaxEnHit=hit->GetChannelId();
       MaxEnHit=energy;
     }
    eTot += energy;
    
    hname = "SAC_WeigHitMap";
    hSvc->FillHisto2(hname, (Double_t)ix, (Double_t)iy, energy);
    if (energy > eMax) {iLead = i; eMax=energy;}
    

   }

   for (Int_t i=0; i<fNhits; ++i){
     hit = fhitEvent->Hit(i);
     hname="SAC_DTimeEMaxHitAllHit";
     if(ChIDMaxEnHit != hit->GetChannelId()) hSvc->FillHisto(hname, TimeMaxEnHit-hit->GetTime(), 1.);
   }
   hname="SAC_EHitTot";
   hSvc->FillHisto(hname, eTot, 1.);
   if (fNhits<1) {std::cout<<"fNhits less than 1 "<<std::endl; return true;}
     


   TRecoVCluster* clu=NULL;
   TRecoVCluster* clun=NULL;
   //std::cout<<"SAC sanity checks:"
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
   hname = "SAC_NCluster";
   hSvc->FillHisto(hname, fNclus,1.);
   for (Int_t i=0; i<fNclus; ++i){
     clu    = fClColl->Element(i);
     seed   = clu->GetSeed();
     seedId = clu->GetChannelId();
     Int_t ix = clu->GetPosition().X();
     Int_t iy = clu->GetPosition().Y();
     hname = "SAC_ClusterMap";
     hSvc->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);

     Int_t checkseedId =  fhitEvent->Hit(seed)->GetChannelId();
     if ( checkseedId != seedId ) {
       std::cout<<"ERROR on SAC indices --- seed: clustID, hit(seed).id = "<<seed<<" "<<seedId<<" "<<checkseedId<<std::endl;
     }
     //Int_t evNumb = fRecoEvent->GetEventNumber();
     if(clu->GetEnergy() >MaxEn){
     std::cout<<"ClE> maxEn " << std::endl;
       MaxSubLeadT=TimeMaxEn;
       TimeMaxEn=clu->GetTime();
       MaxSubLeadEn=MaxEn;
       MaxEn=clu->GetEnergy();
       MaxSubLeadIndex=MaxIndex;
       MaxIndex=i;
     }
     eTotCl += clu->GetEnergy();
     hname = "SAC_SeedEnergy";
     hSvc->FillHisto(hname, fhitEvent->Hit(seed)->GetEnergy(), 1.);
     hname = "SAC_ClusterEnergy";
     hSvc->FillHisto(hname, clu->GetEnergy(), 1.);
     if((ix<10 || ix>18) && (iy<10 || iy>18)){
       hname = "SAC_ClusterEnergyInFiducialRegion";
       hSvc->FillHisto(hname, clu->GetEnergy(), 1.);
     }
      if(ix<9){
       hname="SAC_EClLeftSize";
       hSvc->FillHisto(hname, clu->GetEnergy(), 1.);
     }
     if(iy<3 || iy>24){
       hname="SAC_EClTopBottomSize";
       hSvc->FillHisto(hname, clu->GetEnergy(), 1.);
     }

     seedT  = clu->GetTime();
     clSize = clu->GetNHitsInClus();
     hname = "SAC_NHitsInClus";
     hSvc->FillHisto(hname, clSize,1.);

     std::vector<Int_t> vhinclu = clu->GetHitVecInClus();
     for(int ih=0;ih<clSize; ih++)
     {
       int ihit=vhinclu[ih];
       hitn=fhitEvent->Hit(ihit);
       Int_t ix = hitn->GetPosition().X();
       Int_t iy = hitn->GetPosition().Y();
       hname = "SAC_ClusterHitMap";
       hSvc->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);
       hname = "SAC_ClusterHitMap_EnWeig";
       hSvc->FillHisto2(hname, (Double_t)ix, (Double_t)iy, hitn->GetEnergy());
     }




     for (Int_t ih=0; ih<clSize; ++ih){
       Int_t hitInd    = vhinclu[ih];
       //std::cout<<"Hit INDEX in clu : " << hitInd << std::endl;
       if (hitInd == seed) continue;
       hit = fhitEvent->Hit(hitInd);
       hname = "SAC_inClus_DCHXseed";
       hSvc->FillHisto(hname,hit->GetChannelId()/10-seedId/10,1.);
       hname = "SAC_inClus_DCHYseed";
       hSvc->FillHisto(hname,hit->GetChannelId()%10-seedId%10,1.);
       hname = "SAC_inClus_DTseed";
       hSvc->FillHisto(hname,hit->GetTime()-seedT,1.);

       for (Int_t ihn=ih+1; ihn<clSize; ++ihn){
	 Int_t hitIndn    = vhinclu[ihn];
	 if (hitIndn == seed) continue;
	 hitn = fhitEvent->Hit(hitIndn);
	 //std::cout<<" hit comp. n "<<ih<<" vs comp in= "<<ihn<<" chId:  "<<hit->GetChannelId()<<"   "<<hitn->GetChannelId()<<std::endl;
	 hname = "SAC_inClus_DCHXhits";
	 hSvc->FillHisto(hname,hit->GetChannelId()/10-hitn->GetChannelId()/10,1.);
	 hname = "SAC_inClus_DCHYhits";
	 hSvc->FillHisto(hname,hit->GetChannelId()%10-hitn->GetChannelId()%10,1.);
	 hname = "SAC_inClus_DThits";
	 hSvc->FillHisto(hname,hit->GetTime()-hitn->GetTime(),1.);
       }
     }

     for (Int_t in=0; in<fNclus; ++in){
       clun    = fClColl->Element(in);
       //
       hname = "SAC_Clus2Clus_seedDCHX";
       hSvc->FillHisto(hname,clun->GetChannelId()/10-clu->GetChannelId()/10,1.);
       hname = "SAC_Clus2Clus_seedDCHY";
       hSvc->FillHisto(hname,clun->GetChannelId()%10-clu->GetChannelId()%10,1.);
       hname = "SAC_Clus2Clus_seedDT";
       hSvc->FillHisto(hname,clun->GetTime()-clu->GetTime(),1.);
     }

     
     Int_t indexHitBetterTime = GammaCloseInTime(i, seedT);

     if(indexHitBetterTime>-1){
       clun= fClColl->Element(indexHitBetterTime);
       hname="SAC_ClusterInTimeEnergy";
       hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
       //hname="SAC_ClusterInTimeEnergyPlusAloneCl";
       //hSvc->FillHisto(hname,clu->GetEnergy(),1.);
       //Int_t evNumb = fRecoEvent->GetEventNumber();
       //hname="SAC_NEvent_ClInTime";
       //hSvc->FillHisto(hname, evNumb, 1.);
       //std::cout<<"GammaGammaInTimeEventNumber " << evNumb << std::endl;

       if(fabs(clun->GetEnergy()-clu->GetEnergy())<550.*0.2){ // +/- 110 MeV
         hname="SAC_ClusterInTimeEnergy_thrEne";
         hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
       }



       Double_t ixGamma1            = clu->GetPosition().X();
       Int_t iyGamma1               = clu->GetPosition().Y();
       //Double_t AngGamma1           = atan2((iyGamma1),(ixGamma1));//rad
       Double_t Radius2Gamma1       = (ixGamma1)*(ixGamma1)+(iyGamma1)*(iyGamma1);
       Int_t ixGamma2               = clun->GetPosition().X();
       Int_t iyGamma2               = clun->GetPosition().Y();
       //Double_t AngGamma2           = atan2((iyGamma2),(ixGamma2));
       Double_t Radius2Gamma2       = (ixGamma2)*(ixGamma2)+(iyGamma2)*(iyGamma2);
       Double_t Distance2Gamma1_2   = (ixGamma1-ixGamma2)*(ixGamma1-ixGamma2)+(iyGamma1-iyGamma2)*(iyGamma1-iyGamma2);
       //hname="SAC_DistanceDifferenceGamma1Gamma2-Gamma1Center";
       //hSvc->FillHisto(hname, sqrt(Distance2Gamma1_2) - sqrt(Radius2Gamma1),1.);
       
       //if(indexHitBetterTime>-1) RatioPhiMethod(i, indexHitBetterTime, AngGamma1, AngGamma2, Distance2Gamma1_2, Radius2Gamma1);

       //if(indexHitBetterTime>-1)AnnihilationPhiSimmetry(i, indexHitBetterTime, AngGamma1, AngGamma2, Distance2Gamma1_2, Radius2Gamma1);

       if(indexHitBetterTime>-1)AnnihilationGravitationalMethod(i, indexHitBetterTime);
       
       Double_t GravCenter= (sqrt(Radius2Gamma1)*clu->GetEnergy()+ sqrt(Radius2Gamma2)*clun->GetEnergy() )/(clu->GetEnergy()+clun->GetEnergy());
       //std::cout << " radius 1, 2, centroid " << Radius2Gamma1 << " , " << Radius2Gamma2 << " , " << GravCenter << std::endl;
       if(fabs(GravCenter)<fAnnhilationDeltaSpaceGravCenter){  
          hname="SAC_AnnihilationGravCenterCloseInZero";
          hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy() ,1.);
          //hname="SAC_AnnihilationGravCenterCloseInZero_PlusAloneCl";
          //hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy() ,1.);       
       }

       //std::cout<<"Phi distance " << fabs(AngGamma1-AngGamma2) << std::endl;


     }
     else{
       hname="SAC_ClusterInTimeEnergyPlusAloneCl";
       hSvc->FillHisto(hname,clu->GetEnergy(),1.);
       Double_t ixGamma1            = clu->GetPosition().X();
       Int_t iyGamma1               = clu->GetPosition().Y();
       Double_t Radius2Gamma1       = (ixGamma1)*(ixGamma1)+(iyGamma1)*(iyGamma1);
       if(fabs(Radius2Gamma1)<fAnnhilationDeltaSpaceGravCenter){  
          hname="SAC_AnnihilationGravCenterCloseInZero_PlusAloneCl";
          hSvc->FillHisto(hname,clu->GetEnergy() ,1.);       
       }
     }
    //close loop on cl
   }



   hname="SAC_EClTot";
   hSvc->FillHisto(hname, eTotCl, 1.);
   hname="SAC_EMaxCl";
   hSvc->FillHisto(hname, MaxEn, 1.);
   for (Int_t i=0; i<fNhits; ++i){
    hit = fhitEvent->Hit(i);
    hname="SAC_DTimeEMaxClHit";
    hSvc->FillHisto(hname, TimeMaxEn-hit->GetTime(), 1.);
   }
   
   Int_t SeedIndexHitBetterTime=-1;
   SeedIndexHitBetterTime = GammaCloseInTime(MaxIndex, TimeMaxEn);
   std::cout<<"max index and indexInTime maxsubLeading " <<  MaxIndex << "  " << SeedIndexHitBetterTime << "  "<< MaxSubLeadIndex << std::endl;
   if(MaxIndex>-1)AnnihilationLeading(MaxIndex, SeedIndexHitBetterTime);
  
          
     if(MaxIndex>-1 )AnnihilationLeadingSubLeading(MaxIndex, MaxSubLeadIndex, TimeMaxEn, MaxSubLeadT, " ");
   
   /*
   if(MaxIndex>-1){
     Int_t LeadingDeltaPhi = ReserchGamma2InDeltaPhi(MaxIndex);
     Double_t timeLeadingDeltaPhi = (fClColl->Element(LeadingDeltaPhi))->GetTime();
     if(LeadingDeltaPhi >-1){
       if(fabs(TimeMaxEn-timeLeadingDeltaPhi)<fAnnhilationDeltaTime )AnnihilationLeadingSubLeading(MaxIndex, LeadingDeltaPhi, "_Gamma2inDeltaPhi45");
     }
   }
   */
   
   
  //flat ntupla filling 
  Int_t      chId;
  Double_t energy;
  Double_t   time;

  fNhits = fhitEvent->GetNHits();
  fNclus = fClColl->GetNElements();

  
  
  for (Int_t i=0; i<fNhits; ++i){
    hit    = fhitEvent->Hit(i);
    chId   = hit->GetChannelId();
    energy = hit->GetEnergy();
    time   = hit->GetTime();

   (hSvc->myEvt).NTNSAC_Hits=fhitEvent->GetNHits();
   (hSvc->myEvt).NTSAC_Hits_ChannelId[i]=(Double_t)chId;
   (hSvc->myEvt).NTSAC_Hits_Energy[i]=hit->GetEnergy();
   (hSvc->myEvt).NTSAC_Hits_Time[i]=hit->GetTime();
   (hSvc->myEvt).NTSAC_Hits_Xpos[i]=hit->GetPosition().X();
   (hSvc->myEvt).NTSAC_Hits_Ypos[i]=hit->GetPosition().Y();
   (hSvc->myEvt).NTSAC_Hits_Zpos[i]=hit->GetPosition().Z();
  }
  

  for (Int_t j=0; j<fNclus; ++j){
     clu    = fClColl->Element(j);
     seedId = clu->GetChannelId();
  
   (hSvc->myEvt).NTNSAC_Clusters= fClColl->GetNElements();
   (hSvc->myEvt).NTSAC_Clusters_ChannelId[j]=Double_t(clu->GetChannelId());
   (hSvc->myEvt).NTSAC_Clusters_Energy[j]=clu->GetEnergy();
   (hSvc->myEvt).NTSAC_Clusters_Time[j]=clu->GetTime();
   (hSvc->myEvt).NTSAC_Clusters_Xpos[j]=clu->GetPosition().X();
   (hSvc->myEvt).NTSAC_Clusters_Ypos[j]=clu->GetPosition().Y();
   (hSvc->myEvt).NTSAC_Clusters_Zpos[j]=clu->GetPosition().Z();
  }
  
  
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


Int_t SACAnalysis::GammaCloseInTime(Int_t i, Double_t timeGamma1){
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

void SACAnalysis::RatioPhiMethod(Int_t indexGamma1, Int_t indexGamma2,Double_t AngGamma1, Double_t AngGamma2, Double_t Distance2Gamma1_2, Double_t Radius2Gamma1)
{
  //HistoSvc* hSvc =  HistoSvc::GetInstance();
  //std::string hname;
  TRecoVCluster* clu=NULL;
  TRecoVCluster* clun=NULL;
  clu     = fClColl->Element(indexGamma1);
  clun    = fClColl->Element(indexGamma2);
/*
  if((AngGamma2*AngGamma1<0 && (fabs(AngGamma1-AngGamma2)-3.14)<0.1) || (AngGamma2*AngGamma1>0 && (fabs(AngGamma1+AngGamma2)-3.14)< 0.1))
  {
   //hname="SAC_ClusterInTimeEnergy_CoeffAngularRequest";
   //hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);

   if(Distance2Gamma1_2 > Radius2Gamma1){
     hname="SAC_ClusterInTimeEnergy_CoeffAngularDistanceRequest";
     hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
     hname = "SAC_AnnihilationClusterMap";
     hSvc->FillHisto2(hname, clu->GetPosition().X(), clu->GetPosition().Y(), 1.);
     hSvc->FillHisto2(hname, clun->GetPosition().X(), clun->GetPosition().Y(), 1.);

     if(fabs(clun->GetEnergy()-clu->GetEnergy())<550.*0.3){
       hname="SAC_ClusterInTimeEnergy_angularRequest_thrEne";
       hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);

     }
   }
 }*/

}

void SACAnalysis::AnnihilationPhiSimmetry(Int_t indexGamma1, Int_t indexGamma2, Double_t AngGamma1, Double_t AngGamma2, Double_t Distance2Gamma1_2, Double_t Radius2Gamma1)
{
  //HistoSvc* hSvc =  HistoSvc::GetInstance();
  //std::string hname;
  TRecoVCluster* clu=NULL;
  TRecoVCluster* clun=NULL;
  clu     = fClColl->Element(indexGamma1);
  clun    = fClColl->Element(indexGamma2);
/*
  if(sqrt(Distance2Gamma1_2) > sqrt(Radius2Gamma1)){
    hname="SAC_DistanceDeltaPhi";
    hSvc->FillHisto(hname,AngGamma1-AngGamma2,1.);
    hname="SAC_AngularGamma1";
    hSvc->FillHisto(hname,AngGamma1,1.);
    hname="SAC_AngularGamma2";
    hSvc->FillHisto(hname,AngGamma2,1.);
    if(AngGamma1*AngGamma2<0){
     if(fabs(fabs(AngGamma1-AngGamma2)-3.14)< 4*0.09) //0.09=5deg
     {
         hname="SAC_ClusterInTimeEnergy_DeltaPhi20";
         hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
     }
    }
   else{
     if(fabs(fabs(AngGamma1+AngGamma2)-3.14)< 4*0.09) //0.09=5deg
     {
         hname="SAC_ClusterInTimeEnergy_DeltaPhi20";
         hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
     }
   }
 }
*/
}


void SACAnalysis::AnnihilationLeading(Int_t indexGamma1, Int_t indexGamma2)
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  TRecoVCluster* clu=NULL;
  TRecoVCluster* clun=NULL;
  clu     = fClColl->Element(indexGamma1);
  if ( indexGamma2>-1) clun    = fClColl->Element(indexGamma2);
  
   Double_t ixGamma1            = clu->GetPosition().X();
   Int_t iyGamma1               = clu->GetPosition().Y();
   //Double_t AngGamma1           = atan2((iyGamma1),(ixGamma1));//rad
   //Double_t Radius2Gamma1       = (ixGamma1)*(ixGamma1)+(iyGamma1)*(iyGamma1);
   if (indexGamma2>-1){
     Int_t ixGamma2               = clun->GetPosition().X();
     Int_t iyGamma2               = clun->GetPosition().Y();
     //Double_t AngGamma2           = atan2((iyGamma2),(ixGamma2));
     //Double_t Radius2Gamma2       = (ixGamma2)*(ixGamma2)+(iyGamma2)*(iyGamma2);
     //Double_t Distance2Gamma1_2   = (ixGamma1-ixGamma2)*(ixGamma1-ixGamma2)+(iyGamma1-iyGamma2)*(iyGamma1-iyGamma2);

    hname="SAC_SeedInTimeEnergy";
    hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
    Bool_t sameEnergy=false;
    //hname="SAC_LeadingDiffEnergy";
    //hSvc->FillHisto(hname, clu->GetEnergy()-clun->GetEnergy(), 1. );
    if(fabs(clun->GetEnergy()-clu->GetEnergy())<100)
    {
       sameEnergy=true;
       hname="SAC_SeedInTimeEnergy_sameEnergy";
       hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
    }
   }
   if(indexGamma2== -1){
      hname="SAC_SeedInTimeEnergy_AloneCl";
      hSvc->FillHisto(hname,clu->GetEnergy(),1.);
   }
  /*
  if(sqrt(Distance2Gamma1_2) > sqrt(Radius2Gamma1) ){
    if(AngGamma1*AngGamma2<0){
     if(fabs(fabs(AngGamma1-AngGamma2)-3.14)< 4*0.09) //0.09=5deg
     {
         hname="SAC_SeedInTimeEnergy_DeltaPhi20";
         hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
         if(sameEnergy){
           hname="SAC_SeedInTimeEnergy_sameEnergy_DeltaPhi20";
           hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
         }
     }
    }
   else{
     if(fabs(fabs(AngGamma1+AngGamma2)-3.14)< 4*0.09) //0.09=5deg
     {
         hname="SAC_SeedInTimeEnergy_DeltaPhi20";
         hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
         if(sameEnergy){
           hname="SAC_SeedInTimeEnergy_sameEnergy_DeltaPhi20";
           hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
         }
     }
   }
 }*/

}

void SACAnalysis::AnnihilationLeadingSubLeading(Int_t indexGamma1, Int_t indexGamma2, Double_t TimeMaxEn, Double_t MaxSubLeadT, TString name)
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  TRecoVCluster* clu=NULL;
  TRecoVCluster* clun=NULL;
  clu     = fClColl->Element(indexGamma1);
  if(indexGamma2 >-1 ) clun    = fClColl->Element(indexGamma2);
  
   Double_t ixGamma1            = clu->GetPosition().X();
   Int_t iyGamma1               = clu->GetPosition().Y();
   //Double_t AngGamma1           = atan2((iyGamma1),(ixGamma1));//rad
   //Double_t Radius2Gamma1       = (ixGamma1)*(ixGamma1)+(iyGamma1)*(iyGamma1);
   if (indexGamma2 >-1 && fabs(TimeMaxEn-MaxSubLeadT)<fAnnhilationDeltaTime ){
     Int_t ixGamma2               = clun->GetPosition().X();
     Int_t iyGamma2               = clun->GetPosition().Y();
     //Double_t AngGamma2           = atan2((iyGamma2),(ixGamma2));
     //Double_t Radius2Gamma2       = (ixGamma2)*(ixGamma2)+(iyGamma2)*(iyGamma2);
     //Double_t Distance2Gamma1_2   = (ixGamma1-ixGamma2)*(ixGamma1-ixGamma2)+(iyGamma1-iyGamma2)*(iyGamma1-iyGamma2);

    hname="SAC_LeadingSubLeadingInTimeEnergy"+name;
    hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
    /*if(sqrt(Distance2Gamma1_2) > sqrt(Radius2Gamma1) ){
      if(AngGamma1*AngGamma2<0){
       if(fabs(fabs(AngGamma1-AngGamma2)-3.14)< 4*0.09) //0.09=5deg
       {
         hname="SAC_LeadingSubLeadingInTimeEnergy_DeltaPhi20"+name;
         hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
       }
      }
     else{
       if(fabs(fabs(AngGamma1+AngGamma2)-3.14)< 4*0.09) //0.09=5deg
       {
         hname="SAC_LeadingSubLeadingInTimeEnergy_DeltaPhi20"+name;
         hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);
       }
    }
   }*/
  }
  if(indexGamma2 == -1){
    hname="SAC_LeadingSubLeadingInTimeEnergy"+name+"_AloneCl";
    hSvc->FillHisto(hname,clu->GetEnergy(),1.);
  }
}



void SACAnalysis::AnnihilationGravitationalMethod(Int_t indexGamma1, Int_t indexGamma2)
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
  hname="SAC_AhhinilationMAPGravMethod";
  hSvc->FillHisto2(hname, WeigX, WeigY, 1.);
  Double_t WeigDistance        = sqrt(WeigX*WeigX+WeigY*WeigY);
  //hname="SAC_WeigDstanceGammaGamma";
  //hSvc->FillHisto(hname,WeigDistance,1.);
  //hname="SAC_GammaGammaGravMethod";
  //hSvc->FillHisto(hname,clun->GetEnergy()+clu->GetEnergy(),1.);

}

Int_t SACAnalysis::ReserchGamma2InDeltaPhi(Int_t indexGamma1){

   TRecoVCluster* clu=NULL;
   TRecoVCluster* clun=NULL;
   Double_t maxEne=0;
   Int_t maxIndex=0;
   clu     = fClColl->Element(indexGamma1);
   Double_t ixGamma1            = clu->GetPosition().X();
   Int_t iyGamma1               = clu->GetPosition().Y();
   Double_t AngGamma1           = atan2((iyGamma1),(ixGamma1));//rad
   Double_t Radius2Gamma1       = (ixGamma1)*(ixGamma1)+(iyGamma1)*(iyGamma1);
   
   Int_t fNclus = fClColl->GetNElements();
   for (Int_t in=indexGamma1+1; in<fNclus; ++in){
     clun    = fClColl->Element(in);
     Int_t ixGamma2               = clun->GetPosition().X();
     Int_t iyGamma2               = clun->GetPosition().Y();
     Double_t AngGamma2           = atan2((iyGamma2),(ixGamma2));
     Double_t Radius2Gamma2       = (ixGamma2)*(ixGamma2)+(iyGamma2)*(iyGamma2);
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

