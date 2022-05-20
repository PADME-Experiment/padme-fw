#include "ValidationBase.hh"

#include "TRecoEvent.hh"
#include "TRecoVObject.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "HistoSvc.hh"
#include "TVector3.h"
#include <iostream>


ValidationBase::ValidationBase()
{
  fRecoEvent   = NULL;
  fhitEvent    = NULL;
  fClColl      = NULL;
  fProcessingMode = 0;
  fVerbose        = 0;
}
ValidationBase::ValidationBase(Int_t processingMode, Int_t verbosityFlag)
{
  fRecoEvent   = NULL;
  fhitEvent    = NULL;
  fClColl      = NULL;
  fProcessingMode = processingMode;
  fVerbose        = verbosityFlag;
}
ValidationBase::~ValidationBase()
{
}

Bool_t ValidationBase::Init(TRecoEvent* evHeader, TRecoVObject* ev, TRecoVClusCollection* cl)
{
  Bool_t retCode = 0;
  fRecoEvent = evHeader;
  fhitEvent = ev;
  fClColl = cl;
  return retCode;
}
/*
Bool_t ValidationBase::InitValidation()
{
  InitHistosValidation(fname);
}
*/

Bool_t ValidationBase::InitHistos()
{
  if (fProcessingMode==1)
    {
      return InitHistosValidation();
    }
  else if (fProcessingMode==2)
    {
      return InitHistosDataQuality();
    }
  else if ( fProcessingMode > 2 || fProcessingMode < 0 )
    {
      std::cout<<"Error:: fProcessingModel = "<<fProcessingMode<<" out of range "<<std::endl;
      return true;
    }
  return InitHistosAnalysis();
  // TO DO move here from HistSvc ;
}

Bool_t ValidationBase::InitHistosValidation()
{
  return InitHistosValidation(fAlgoName);
}
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


Bool_t ValidationBase::Process()
{
  Bool_t retCode = 0;
  if (fhitEvent == NULL)
    {
      std::cout<<fAlgoName<<"::Process - ERROR - no data found"<<std::endl;
      return true;
    }
  
  if (fProcessingMode==1)
    {
      return ProcessValidation();
    }
  else if (fProcessingMode==2)
    {
      return ProcessDataQuality();
    }
  else if ( fProcessingMode > 2 || fProcessingMode < 0 )
    {
      std::cout<<"Error:: fProcessingModel = "<<fProcessingMode<<" out of range "<<std::endl;
      return true;
    }
  return ProcessAnalysis();
}


Bool_t ValidationBase::ProcessValidation()
{
  return ProcessValidation(fAlgoName);
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
  //std::cout << "in ProcessValidation("<<name<<")" << std::endl;
  //if (fhitEvent) std::cout << "in ProcessValidation("<<name<<")" << std::endl;
  //else
  //  {
  //    std::cout<<" no data "<<std::endl;
  //    return true;
  //  }
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
     position=clu->GetPosition();   //MR BUG FOUND
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

