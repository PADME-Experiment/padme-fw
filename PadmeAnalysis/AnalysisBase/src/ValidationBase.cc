#include "ValidationBase.hh"

#include "TECalRecoEvent.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "HistoSvc.hh"
#include "TVector3.h"
#include <iostream>


ValidationBase::ValidationBase()
{
  fhitEvent=NULL;
  fClColl=NULL;
  fValidation = 0;
  fVerbose    = 0;
}
ValidationBase::~ValidationBase()
{
}

/*
Bool_t ValidationBase::Init(TRecoVObject* ev, TRecoVClusCollection* cl, Int_t verb)
{
  Bool_t retCode = 0;
  fhitEvent = ev;
  fClColl = cl;
  fVerbose = verb;

  return retCode;
}
Bool_t ValidationBase::InitValidation()
{
  InitHistosValidation(fname);
}
*/

Bool_t ValidationBase::InitHistosValidation(TString name)
{
  //Validation histos
  //std::cout <<"I'm in initHistoValidation ValidationBase" << std::endl;
  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  std::string hname;
  int nBin, min, max;
  nBin=60;
  min=0;
  max=59;
  hname=name+"_NHits";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname=name+"_NCluster";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=500;
  min=0;
  max=500;
  hname = name+"_HitEnergy";
  hSvcVal->BookHisto(hname,nBin,min, max);
  hname = name+"_ClusterEnergy";
  hSvcVal->BookHisto(hname,nBin,min, max);
  nBin=800;
  min=-400;
  max=400;
  hname = name+"_HitTime";
  hSvcVal->BookHisto(hname,nBin, min, max);
  hname = name+"_ClusterTime";
  hSvcVal->BookHisto(hname,nBin, min, max);
  nBin=100;
  min=0;
  max=100;
  hname=name+"_HitXPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname=name+"_ClusterXPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname=name+"_HitYPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname=name+"_ClusterYPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=100;
  min=0;
  max=100;
  hname=name+"_HitZPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname=name+"_ClusterZPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname=name+"_HitChannelId";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname=name+"_ClusterSeedChannelId";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=16;
  min=0;
  max=15;
  hname=name+"_NHitInCluster";
  hSvcVal->BookHisto(hname, nBin, min, max);
  
  return true;
}


Bool_t ValidationBase::ProcessValidation(TString name)
{
  //std::cout << "I'm in process Validation ValidationBase" << std::endl;
  Bool_t retCode = 0;

  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  Double_t eMax  =  0.;
  Int_t    iLead = -1;
  Double_t eTot  =  0.;
  TVector3 position;
  TRecoVHit* hit=NULL;
  std::string hname;
  //if (fhitEvent) std::cout << "hit is no empty" << std::endl;
  Int_t fNhits = fhitEvent->GetNHits();
  hname=name+"_NHits";
  hSvcVal->FillHisto(hname,fNhits,1.);
  //std::cout<<"Process Validation Hit-> Number of hits: " <<fNhits << std::endl;
  for (Int_t i=0; i<fNhits; ++i){
    hit = fhitEvent->Hit(i);
    position=hit->GetPosition();
    Int_t ix=position.X();
    Int_t iy=position.Y();
    Int_t iz=position.Z();
    Double_t energy = hit->GetEnergy();
    //std::cout<<"Process Validation Hit-> Energy: " << energy << std::endl;
    eTot += energy;
    Double_t time = hit->GetTime();
    Int_t chId =hit->GetChannelId();

    hname = name+"_HitEnergy";
    hSvcVal->FillHisto(hname,energy,1.);
    hname = name+"_HitTime";
    hSvcVal->FillHisto(hname,time,  1.);
    hname=name+"_HitXPos";
    hSvcVal->FillHisto(hname,ix,  1.);
    hname=name+"_HitYPos";
    hSvcVal->FillHisto(hname,iy,  1.);
    hname=name+"_HitZPos";
    hSvcVal->FillHisto(hname,iz,  1.);
    hname=name+"_HitChannelId";
    hSvcVal->FillHisto(hname,chId,  1.);

   }


   TRecoVCluster* clu=NULL;
   //std::cout<<"ECal sanity checks:"
   Int_t seed;
   Int_t seedId;
   Double_t seedE;
   Double_t seedT;
   Int_t clSize;
   Int_t fNclus = fClColl->GetNElements();
   hname=name+"_NCluster";
   hSvcVal->FillHisto(hname,fNclus,1.);
   Double_t eTotCl=0.;

   //std::cout<<"Process Validation -> Number of cluster: " <<fNclus << std::endl;
   for (Int_t i=0; i<fNclus; ++i){
     clu    = fClColl->Element(i);
     seed   = clu->GetSeed();
     seedId = clu->GetChannelId();
     seedE  = clu->GetEnergy();
     //std::cout<<"Process Validation cluster-> Energy: " << seedE << std::endl;
     eTotCl+=seedE;
     seedT  = clu->GetTime();
     clSize = clu->GetNHitsInClus();
     Int_t ix=position.X();
     Int_t iy=position.Y();
     Int_t iz=position.Z();
     hname = name+"_ClusterEnergy";
     hSvcVal->FillHisto(hname,seedE,1.);
     hname = name+"_ClusterTime";
     hSvcVal->FillHisto(hname,seedT,  1.);
     
    hname=name+"_ClusterXPos";
    hSvcVal->FillHisto(hname,ix,  1.);
    hname=name+"_ClusterYPos";
    hSvcVal->FillHisto(hname,iy,  1.);
    hname=name+"_ClusterZPos";
    hSvcVal->FillHisto(hname,iz,  1.);
    hname=name+"_ClusterSeedChannelId";
    hSvcVal->FillHisto(hname,seedId,  1.);
    
    hname=name+"_NHitInCluster";
    hSvcVal->FillHisto(hname,clSize,  1.);
    
    
    

   }

   return retCode;
}

