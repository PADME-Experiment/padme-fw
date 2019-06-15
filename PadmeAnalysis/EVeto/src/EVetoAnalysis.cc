#include "EVetoAnalysis.hh"
#include "TEVetoRecoEvent.hh"
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
    HistoSvc* hSvc =  HistoSvc::GetInstance();
    std::string hname;
    Int_t nx, ny;
    Double_t xlow, ylow, xup, yup;
    
    ny = 100;
    ylow = -0.5;
    yup  = 99.5;
    nx = 100;
    xlow = -350.;
    xup  =  350.;
    hname = "EVeto_Hit_TimeVSch";
    hSvc->BookHisto2(hname, nx, xlow, xup, ny, ylow, yup);


    nx = 100.;
    xlow = -25.;
    xup  =  25.;
    hname = "EVeto_Hit_DTime_DCh0";
    hSvc->BookHisto(hname, nx, xlow, xup);

    nx = 200.;
    hname = "EVeto_Hit_DTime_DCh1";
    hSvc->BookHisto(hname, nx, xlow, xup);

    nx = 100.;
    hname = "EVeto_Hit_DTimeVSch_DCh1";
    hSvc->BookHisto2(hname, nx, xlow, xup, ny, ylow, yup);

    nx = 100.;
    hname = "EVeto_Hit_DTime_DCh2";
    hSvc->BookHisto(hname, nx, xlow, xup);
    
    xlow = -350.;
    xup  =  350.;
    hname = "EVeto_Hit_DTime_DChLarge";
    hSvc->BookHisto(hname, nx, xlow, xup);

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
  
  for (Int_t i=0; i<fNhits; ++i){
    hit    = fhitEvent->Hit(i);
    chId   = hit->GetChannelId();
    energy = hit->GetEnergy();
    time   = hit->GetTime();

   (hSvc->myEvt).NTNEVeto_Hits=fhitEvent->GetNHits();
   (hSvc->myEvt).NTEVeto_Hits_ChannelId[i]=(Double_t)chId;
   (hSvc->myEvt).NTEVeto_Hits_Energy[i]=hit->GetEnergy();
   (hSvc->myEvt).NTEVeto_Hits_Time[i]=hit->GetTime();
   (hSvc->myEvt).NTEVeto_Hits_Xpos[i]=hit->GetPosition().X();
   (hSvc->myEvt).NTEVeto_Hits_Ypos[i]=hit->GetPosition().Y();
   (hSvc->myEvt).NTEVeto_Hits_Zpos[i]=hit->GetPosition().Z();
  }

    //fillClustersFlatNTP  

   for (Int_t j=0; j<fNclus; ++j){
     clu    = fClColl->Element(j);
     seedId = clu->GetChannelId();
  
   (hSvc->myEvt).NTNEVeto_Clusters= fClColl->GetNElements();
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
