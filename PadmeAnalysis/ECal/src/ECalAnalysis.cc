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
  fAnnhilationDeltaTime=3.;
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
   

    nBin=600;
    min=0;
    max=10000;
    hname="ECal_EHitTot";
    hSvc->BookHisto(hname,nBin,min, max);
    hname="ECal_EClTot";
    hSvc->BookHisto(hname,nBin,min, max);

    min=-0.5;
    max= 40.5;
    nBin=41;
    hname = "ECal_NHitsInClus";
    hSvc->BookHisto(hname,nBin,min, max);

   
    
    
    //Histo for molteplicity studies
    min=-0.5;
    max= 35.5;
    nBin=36;
    hname = "ECal_NCluster";
    hSvc->BookHisto(hname,nBin,min, max);
    

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
  nBin=500;
  min=0;
  max=1000;
  hname="ECal_TwoPhotonEnergy_TimeCoincidenceRequest3ns";
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

  Double_t eMax  =  0.;
  Int_t    iLead = -1;
  Double_t eTot  =  0.;
  TRecoVHit* hit=NULL;
  TRecoVHit* hitn=NULL;
  Double_t MaxEnHit=0;
  Double_t TimeMaxEnHit=0;
  Double_t ChIDMaxEnHit=-1;
  std::string hname;
  Int_t evNumb = fRecoEvent->GetEventNumber();
  Int_t fNhits = fhitEvent->GetNHits();
  //std::cout<<"+++++++++++++++++++++++++++++++++++++++++ ECalAnalysis ... "<<fNhits<<std::endl;
  for (Int_t i=0; i<fNhits; ++i){

    hit = fhitEvent->Hit(i);
    Double_t energy = hit->GetEnergy();
    hname = "ECal_HitEnergy";
    hSvc->FillHisto(hname, energy, 1.);
    eTot += energy;
    
   }

   hname="ECal_EHitTot";
   hSvc->FillHisto(hname, eTot, 1.);
   if (fNhits<1) {std::cout<<"fNhits less than 1 "<<std::endl; return true;}
  
   TRecoVCluster* clu=NULL;
   TRecoVCluster* clun=NULL;
   //std::cout<<"ECal sanity checks:"
   Int_t seed;
   Int_t seedId;
   Double_t seedE;
   Double_t seedT;
   Double_t eTotCl;
   Int_t clSize;

   

   Int_t fNclus = fClColl->GetNElements();
   hname = "ECal_NCluster";
   hSvc->FillHisto(hname, fNclus,1.);
   for (Int_t i=0; i<fNclus; ++i){
    std::cout<< evNumb << std::endl;
     clu    = fClColl->Element(i);
     seed   = clu->GetSeed();
     seedId = clu->GetChannelId();
     seedE  = clu->GetEnergy();
     eTotCl += seedE;
     hname = "ECal_ClusterEnergy";
     hSvc->FillHisto(hname, clu->GetEnergy(), 1.);

     seedT  = clu->GetTime();
     clSize = clu->GetNHitsInClus();
     hname = "ECal_NHitsInClus";
     hSvc->FillHisto(hname, clSize,1.);
   
    }



   hname="ECal_EClTot";
   hSvc->FillHisto(hname, eTotCl, 1.);
   


     //fillHitsFlatNTP

  (hSvc->myEvt).NTNECal_Hits=fhitEvent->GetNHits();
  
  for (Int_t i=0; i<fNhits; ++i){
    hit    = fhitEvent->Hit(i);
    Double_t chId   = hit->GetChannelId();
    Double_t energy = hit->GetEnergy();
    Double_t time   = hit->GetTime();

   (hSvc->myEvt).NTECal_Hits_ChannelId[i]=(Double_t)chId;
   (hSvc->myEvt).NTECal_Hits_Energy[i]=hit->GetEnergy();
   (hSvc->myEvt).NTECal_Hits_Time[i]=hit->GetTime();
   (hSvc->myEvt).NTECal_Hits_Xpos[i]=hit->GetPosition().X();
   (hSvc->myEvt).NTECal_Hits_Ypos[i]=hit->GetPosition().Y();
   (hSvc->myEvt).NTECal_Hits_Zpos[i]=hit->GetPosition().Z();
  }

    //fillClustersFlatNTP 

  (hSvc->myEvt).NTNECal_Clusters= fClColl->GetNElements(); 

   for (Int_t j=0; j<fNclus; ++j){
     clu    = fClColl->Element(j);
     seedId = clu->GetChannelId();
  
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
     for(int j=i+1; j< fNclus; j++){
       clun   = fClColl->Element(j);
       if(fabs(clu->GetTime() - clun->GetTime())<3.)
       {
          hname="ECal_TwoPhotonEnergy_TimeCoincidenceRequest3ns";
          hSvcVal->FillHisto(hname, clu->GetEnergy()+clun->GetEnergy());
       }
     }
   }
   hname="ECal_ClusterTotEnergy";
   hSvcVal->FillHisto(hname, ETotCl, 1.);


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
   Double_t AngGamma1           = atan2((iyGamma1),(ixGamma1));//rad
   Double_t Radius2Gamma1       = (ixGamma1)*(ixGamma1)+(iyGamma1)*(iyGamma1);
   Int_t ixGamma2               = clun->GetPosition().X();
   Int_t iyGamma2               = clun->GetPosition().Y();
   Double_t AngGamma2           = atan2((iyGamma2),(ixGamma2));
   Double_t Radius2Gamma2       = (ixGamma2)*(ixGamma2)+(iyGamma2)*(iyGamma2);
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
   Double_t AngGamma1           = atan2((iyGamma1),(ixGamma1));//rad
   Double_t Radius2Gamma1       = (ixGamma1)*(ixGamma1)+(iyGamma1)*(iyGamma1);
   Int_t ixGamma2               = clun->GetPosition().X();
   Int_t iyGamma2               = clun->GetPosition().Y();
   Double_t AngGamma2           = atan2((iyGamma2),(ixGamma2));
   Double_t Radius2Gamma2       = (ixGamma2)*(ixGamma2)+(iyGamma2)*(iyGamma2);
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

