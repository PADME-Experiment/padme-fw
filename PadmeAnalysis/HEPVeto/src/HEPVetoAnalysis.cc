#include "HEPVetoAnalysis.hh"
#include "THEPVetoRecoEvent.hh"
#include "TRecoVHit.hh"
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
    HistoSvc* hSvc =  HistoSvc::GetInstance();
    std::string hname;
    Int_t nx, ny;
    Double_t xlow, ylow, xup, yup;
    
    ny = 10;
    ylow = -0.5;
    yup  = 19.5;
    nx = 100;
    xlow = -350.;
    xup  =  350.;
    hname = "HEPVeto_Hit_TimeVSch";
    hSvc->BookHisto2(hname, nx, xlow, xup, ny, ylow, yup);


    nx = 100.;
    xlow = -25.;
    xup  =  25.;
    hname = "HEPVeto_Hit_DTime_DCh0";
    hSvc->BookHisto(hname, nx, xlow, xup);

    hname = "HEPVeto_Hit_DTime_DCh1";
    hSvc->BookHisto(hname, nx, xlow, xup);

    hname = "HEPVeto_Hit_DTimeVSch_DCh1";
    hSvc->BookHisto2(hname, nx, xlow, xup, ny, ylow, yup);

    hname = "HEPVeto_Hit_DTime_DCh2";
    hSvc->BookHisto(hname, nx, xlow, xup);
    
    xlow = -350.;
    xup  =  350.;
    hname = "HEPVeto_Hit_DTime_DChLarge";
    hSvc->BookHisto(hname, nx, xlow, xup);

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
  
  for (Int_t i=0; i<fNhits; ++i){
    hit    = fhitEvent->Hit(i);
    chId   = hit->GetChannelId();
    energy = hit->GetEnergy();
    time   = hit->GetTime();

   (hSvc->myEvt).NTNHEPVeto_Hits=fhitEvent->GetNHits();
   (hSvc->myEvt).NTHEPVeto_Hits_ChannelId[i]=(Double_t)chId;
   (hSvc->myEvt).NTHEPVeto_Hits_Energy[i]=hit->GetEnergy();
   (hSvc->myEvt).NTHEPVeto_Hits_Time[i]=hit->GetTime();
   (hSvc->myEvt).NTHEPVeto_Hits_Xpos[i]=hit->GetPosition().X();
   (hSvc->myEvt).NTHEPVeto_Hits_Ypos[i]=hit->GetPosition().Y();
   (hSvc->myEvt).NTHEPVeto_Hits_Zpos[i]=hit->GetPosition().Z();
  }

    //fillClustersFlatNTP  

   for (Int_t j=0; j<fNclus; ++j){
     clu    = fClColl->Element(j);
     seedId = clu->GetChannelId();
  
   (hSvc->myEvt).NTNHEPVeto_Clusters= fClColl->GetNElements();
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
