#include "SinglePosRun.hh"

#include "TRecoEvent.hh"
#include "TRecoVObject.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "TTargetRecoBeam.hh"
#include "HistoSvc.hh"
#include "TVector3.h"
#include "TLorentzVector.h"
#include <iostream>


SinglePosRun::SinglePosRun()
{

  fInitToComplete=true;
  
  fRecoEvent   = NULL;

  fECal_hitEvent=0;
  fECal_ClColl=0;
  
  fProcessingMode = 0;
  fVerbose        = 0;


}

SinglePosRun::~SinglePosRun()
{
}


Bool_t SinglePosRun::Init(TRecoEvent* eventHeader, 
			  TRecoVObject* ECALev, TRecoVClusCollection* ECALcl)
{


  //  Bool_t retCode = 0;


  fRecoEvent = eventHeader;
  fECal_hitEvent      =ECALev   ;

  fECal_ClColl      =ECALcl   ;
   
  return true;
}

Bool_t SinglePosRun::Process()
{
 
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
 
  Bool_t isMC=false;
  //std::cout<<"in ProcessAnalysis ... evsel ... "<<fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)<<std::endl;
  if (fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) {
    isMC=true;
    //std::cout<<"input data are simulatetd "<<std::endl;
  }


  TRecoVHit* hit=NULL;
  TRecoVHit* hit2=NULL;
  TRecoVHit* hit3=NULL;
  Int_t fNhits = fECal_hitEvent->GetNHits();
  Double_t eTot  =  0.;
  Double_t eTotin14_16=0.;
  Double_t eTotin16_18=0.;
  Double_t eTotin18_20=0.;
  Double_t eTotin20_22=0.;
  Double_t leadEn=0.;
  Int_t leadPos=-1;
  hname = "ECal_NHits";
  hSvc->FillHisto(hname,fNhits , 1.);
  for (Int_t i=0; i<fNhits; ++i){
    hit = fECal_hitEvent->Hit(i);
    Double_t energy = hit->GetEnergy();
    if(energy>leadEn){leadEn=energy; leadPos=i;}
    hname = "ECal_HitEnergy";
    hSvc->FillHisto(hname, energy, 1.);
    eTot += energy;
    Double_t xhit=hit->GetPosition().X();
    Double_t yhit=hit->GetPosition().Y();
    Double_t rhit=sqrt(xhit*xhit+yhit*yhit);
    
    if(rhit>140 && rhit<160)eTotin14_16+=energy;
    if(rhit>180 && rhit<200)eTotin18_20+=energy;
    if(rhit>160 && rhit<180)eTotin16_18+=energy;
    if(rhit>200 && rhit<220)eTotin20_22+=energy;
    for(int j=i+1; j<fNhits; j++ ){
      if(hit->GetChannelId()== (fECal_hitEvent->Hit(j))->GetChannelId()){
	hname="ECAL_DHitTime_SameChId";
	hSvc->FillHisto(hname, (hit->GetTime()-(fECal_hitEvent->Hit(j))->GetTime()), 1.);
      }
    }
  }
  hname="ECal_EHitTot";
  hSvc->FillHisto(hname, eTot, 1.);
  hname="ECal_EHitTot_in14_16";
  hSvc->FillHisto(hname, eTotin14_16, 1.);
  hname="ECal_EHitTot_in18_20";
  hSvc->FillHisto(hname, eTotin18_20, 1.);
  hname="ECal_EHitTot_in16_18";
  hSvc->FillHisto(hname, eTotin16_18, 1.);
  hname="ECal_EHitTot_in20_22";
  hSvc->FillHisto(hname, eTotin20_22, 1.);
  hname="ECal_NHitsVSEHitTot";
  hSvc->FillHisto2(hname, eTot,fNhits, 1.);
   
  for (Int_t i=0; i<fNhits; ++i){
    hit = fECal_hitEvent->Hit(i);
    Double_t dTime=(fECal_hitEvent->Hit(leadPos))->GetTime()-hit->GetTime();
    if(i!=leadPos){
      hname="ECal_DTimeLead_otherHits";
      hSvc->FillHisto(hname, dTime, 1.);
    }
    Int_t nhitCh=1;
    for(int j=i+1; j<fNhits; j++ ){
      if(hit->GetChannelId()== (fECal_hitEvent->Hit(j))->GetChannelId()) nhitCh++; 
    }
    if(nhitCh>0){
      hname="ECAL_NHitInChannel";
      hSvc->FillHisto(hname, nhitCh, 1.);
      hname="ECAL_NHitInChannelVSETotHit";
      hSvc->FillHisto2(hname, eTot,nhitCh, 1.);
    }
  }
 
 
  TRecoVCluster* clu=NULL;
  TRecoVCluster* clu1=NULL;
  std::vector<Int_t> hitVInCl;
  Int_t seed;
  Int_t seedId;
  Double_t seedE;
  Double_t seedT;
  Double_t eTotCl=0.;
  Int_t clSize;
  Int_t fNclus = fECal_ClColl->GetNElements();
  hname = "ECal_NCluster";
  hSvc->FillHisto(hname, fNclus,1.);

  for (Int_t i=0; i<fNclus; ++i){
    clu    = fECal_ClColl->Element(i);
    seed   = clu->GetSeed();
    seedId = clu->GetChannelId();
    seedE  = clu->GetEnergy();
    eTotCl += seedE;
    hname = "ECal_ClusterEnergy";
    hSvc->FillHisto(hname, clu->GetEnergy(), 1.);
    hname = "ECal_ClusterTime";
    hSvc->FillHisto(hname, clu->GetTime(), 1.);

    seedT  = clu->GetTime();
    clSize = clu->GetNHitsInClus();
    hname = "ECal_NHitsInClus";
    hSvc->FillHisto(hname, clSize,1.);
    TRecoVHit* hitSeed;
    hitSeed =  fECal_hitEvent->Hit(clu->GetSeed());
    hname="ECal_hitSeedEnergy";
    hSvc->FillHisto(hname ,hitSeed->GetEnergy(), 1.);
    if(fNclus==1){
      hname="ECal_EClus_NClus1";
      hSvc->FillHisto(hname, clu->GetEnergy(), 1.);
      hname="ECal_EClus_clSizeNClus1";
      hSvc->FillHisto(hname, clu->GetNHitsInClus(), 1.);
      hitVInCl = ((fECal_ClColl->Element(0))->GetHitVecInClus());
      for (int ih=0; ih<((fECal_ClColl->Element(0))->GetNHitsInClus()); ++ih)
	{
	  if (hitVInCl[ih]==((fECal_ClColl->Element(0)))->GetSeed()) continue;
	  hname="ECAL_EhitInCl1_ncl1";
	  hSvc->FillHisto(hname,(fECal_hitEvent->Hit(hitVInCl[ih]))->GetEnergy(), 1.);
	}
    }
    if(fNclus==2){
      hname="ECal_EClus_NClus2";
      hSvc->FillHisto(hname, clu->GetEnergy(), 1.);
      hname="ECal_EClus_clSizeNClus2";
      hSvc->FillHisto(hname, clu->GetNHitsInClus(), 1.);
    }
    if(fNclus==3){
      hname="ECal_EClus_NClus3";
      hSvc->FillHisto(hname, clu->GetEnergy(), 1.);
      hname="ECal_EClus_clSizeNClus3";
      hSvc->FillHisto(hname, clu->GetNHitsInClus(), 1.);
    }
    if(fNclus==4){
      hname="ECal_EClus_NClus4";
      hSvc->FillHisto(hname, clu->GetEnergy(), 1.);
      hname="ECal_EClus_clSizeNClus4";
      hSvc->FillHisto(hname, clu->GetNHitsInClus(), 1.);
    }
    if(fNclus==5){
      hname="ECal_EClus_NClus5";
      hSvc->FillHisto(hname, clu->GetEnergy(), 1.);
      hname="ECal_EClus_clSizeNClus5";
      hSvc->FillHisto(hname, clu->GetNHitsInClus(), 1.);
    }
    if(fNclus==6){
      hname="ECal_EClus_NClus6";
      hSvc->FillHisto(hname, clu->GetEnergy(), 1.);
      hname="ECal_EClus_clSizeNClus6";
      hSvc->FillHisto(hname, clu->GetNHitsInClus(), 1.);
    }
    if(fNclus==7){
      hname="ECal_EClus_NClus7";
      hSvc->FillHisto(hname, clu->GetEnergy(), 1.);
      hname="ECal_EClus_clSizeNClus7";
      hSvc->FillHisto(hname, clu->GetNHitsInClus(), 1.);
    }
    if(fNclus==8){
      hname="ECal_EClus_NClus8";
      hSvc->FillHisto(hname, clu->GetEnergy(), 1.);
      hname="ECal_EClus_clSizeNClus8";
      hSvc->FillHisto(hname, clu->GetNHitsInClus(), 1.);
    }
    
   
  }
  hname="ECal_EClTot";
  hSvc->FillHisto(hname, eTotCl, 1.);

    hname="ECal_NHitsVSEClTot";
    hSvc->FillHisto2(hname, eTotCl,fNclus, 1.);
   
    Int_t NCleHitTot=0;
    if(eTot<200)NCleHitTot=0;
    else if(eTot>200 && eTot<=700)NCleHitTot=1;
    else if(eTot>700 && eTot<=1150)NCleHitTot=2;
    else if(eTot>1150 && eTot<=1650)NCleHitTot=3;
    else if(eTot>1650 && eTot<=2150)NCleHitTot=4;
    else if(eTot>2150 && eTot<=2600)NCleHitTot=5;
    else if(eTot>2600 && eTot<=3100)NCleHitTot=6;
    hname="ECal_NClNPosHitE";
    hSvc->FillHisto2(hname, NCleHitTot, fNclus);
     
    if(fNclus==2){
      Double_t dTime = (fECal_ClColl->Element(0))->GetTime() - (fECal_ClColl->Element(1))->GetTime();
      hname="ECal_nCl2_DTime";
      hSvc->FillHisto(hname, dTime, 1.);
      hit=fECal_hitEvent->Hit((fECal_ClColl->Element(0))->GetSeed());
      hit2=fECal_hitEvent->Hit((fECal_ClColl->Element(1))->GetSeed());
      Double_t r1=sqrt(hit->GetPosition().X()*hit->GetPosition().X()+hit->GetPosition().Y()*hit->GetPosition().Y());
      Double_t r2=sqrt(hit2->GetPosition().X()*hit2->GetPosition().X()+hit2->GetPosition().Y()*hit2->GetPosition().Y());
      hname="ECAL_DxSeed_ncl2";
      hSvc->FillHisto(hname, hit->GetPosition().X()-hit2->GetPosition().X(), 1.);
      hname="ECAL_DySeed_ncl2";
      hSvc->FillHisto(hname, hit->GetPosition().Y()-hit2->GetPosition().Y(), 1.);
      hname="ECAL_DtSeed_ncl2";
      hSvc->FillHisto(hname, hit->GetTime()-hit2->GetTime(), 1.);
      hname="ECAL_DRadiusVSDTime_ncl2";
      hSvc->FillHisto2(hname, hit->GetTime()-hit2->GetTime(), r1-r2, 1.);
      hname="ECAL_DxVSDTime_ncl2";
      hSvc->FillHisto2(hname, hit->GetTime()-hit2->GetTime(), hit->GetPosition().X()-hit2->GetPosition().X() , 1.);
      if(( hit->GetPosition().X()-hit2->GetPosition().X())<2.1){
	hname="ECAL_DyVSDTime_ncl2_Dx0";
	hSvc->FillHisto2(hname, hit->GetTime()-hit2->GetTime(), hit->GetPosition().Y()-hit2->GetPosition().Y() , 1.);
      }
       
      hname="ECAL_Map2ndCluster_ncl2";
      hSvc->FillHisto2(hname,(fECal_ClColl->Element(1))->GetPosition().X(), (fECal_ClColl->Element(1))->GetPosition().Y(), 1. );
      hname="ECAL_Map1stCluster_ncl2";
      hSvc->FillHisto2(hname,(fECal_ClColl->Element(0))->GetPosition().X(), (fECal_ClColl->Element(0))->GetPosition().Y(), 1. );
      hname="ECAL_Energy2ndCluster_ncl2";
      hSvc->FillHisto(hname, (fECal_ClColl->Element(1))->GetEnergy(), 1.);
      hname="ECAL_Energy1stCluster_ncl2";
      hSvc->FillHisto(hname, (fECal_ClColl->Element(0))->GetEnergy(), 1.);
      hitVInCl.clear();
      hitVInCl = ((fECal_ClColl->Element(0))->GetHitVecInClus());
      for (int ih=0; ih<((fECal_ClColl->Element(0)))->GetNHitsInClus(); ++ih)
	{
	  if (hitVInCl[ih]==((fECal_ClColl->Element(0))->GetSeed())) continue;
	  hname="ECAL_EhitInCl1_ncl2";
	  hSvc->FillHisto(hname,(fECal_hitEvent->Hit(hitVInCl[ih]))->GetEnergy(), 1.);
	}
      hitVInCl.clear();
      hitVInCl = ((fECal_ClColl->Element(1))->GetHitVecInClus());
      for (int ih=0; ih<((fECal_ClColl->Element(1)))->GetNHitsInClus(); ++ih)
	{
	  if (hitVInCl[ih]==((fECal_ClColl->Element(1))->GetSeed())) continue;
	  hname="ECAL_EhitInCl2_ncl2";
	  hSvc->FillHisto(hname,(fECal_hitEvent->Hit(hitVInCl[ih]))->GetEnergy(), 1.);
	}
      if(dTime>10){
	Double_t clsize1=(fECal_ClColl->Element(0))->GetNHitsInClus();
	hname="ECal_clSize1cl_dTimeHiger10";
	hSvc->FillHisto(hname, clsize1, 1.);
	Double_t clsize2=(fECal_ClColl->Element(1))->GetNHitsInClus();
	hname="ECal_clSize2cl_dTimeHiger10";
	hSvc->FillHisto(hname, clsize2, 1.);
      }
      if(dTime>20){
	Double_t clsize1=(fECal_ClColl->Element(0))->GetNHitsInClus();
	hname="ECal_clSize1cl_dTimeHiger20";
	hSvc->FillHisto(hname, clsize1, 1.);
	Double_t clsize2=(fECal_ClColl->Element(1))->GetNHitsInClus();
	hname="ECal_clSize2cl_dTimeHiger20";
	hSvc->FillHisto(hname, clsize2, 1.);
      }
      if(dTime>30){
	Double_t clsize1=(fECal_ClColl->Element(0))->GetNHitsInClus();
	hname="ECal_clSize1cl_dTimeHiger30";
	hSvc->FillHisto(hname, clsize1, 1.);
	Double_t clsize2=(fECal_ClColl->Element(1))->GetNHitsInClus();
	hname="ECal_clSize2cl_dTimeHiger30";
	hSvc->FillHisto(hname, clsize2, 1.);
      }
      if(dTime>40){
	Double_t clsize1=(fECal_ClColl->Element(0))->GetNHitsInClus();
	hname="ECal_clSize1cl_dTimeHiger40";
	hSvc->FillHisto(hname, clsize1, 1.);
	Double_t clsize2=(fECal_ClColl->Element(1))->GetNHitsInClus();
	hname="ECal_clSize2cl_dTimeHiger40";
	hSvc->FillHisto(hname, clsize2, 1.);
      }
    }
   
    if(fNclus==3){
      Double_t dTime1 = (fECal_ClColl->Element(0))->GetTime() - (fECal_ClColl->Element(2))->GetTime();
      Double_t dTime2 = (fECal_ClColl->Element(1))->GetTime() - (fECal_ClColl->Element(2))->GetTime();
      hname="ECal_nCl3_DTime13";
      hSvc->FillHisto(hname, dTime1, 1.);
      hname="ECal_nCl3_DTime23";
      hSvc->FillHisto(hname, dTime2, 1.);
    }
   
    if(eTot<700 && eTot>200){
      hname="ECal_EHitTot_1SingleParticle";
      hSvc->FillHisto(hname, eTot, 1.);
      hname="ECal_NCluster_SingleParticleOnEHitTot";
      hSvc->FillHisto(hname, fNclus, 1.);
      for (Int_t i=0; i<fNhits; ++i){
	hit = fECal_hitEvent->Hit(i);
	Double_t energy = hit->GetEnergy();
	Double_t xhit=hit->GetPosition().X();
	Double_t yhit=hit->GetPosition().Y();
	hname="ECal_Map_200ETotHit700";
	hSvc->FillHisto2(hname, xhit, yhit, 1.);
	hname="ECal_WMap_200ETotHit700";
	hSvc->FillHisto2(hname, xhit, yhit, energy);
	Double_t dTime=(fECal_hitEvent->Hit(leadPos))->GetTime()-hit->GetTime();
	Double_t dx=(fECal_hitEvent->Hit(leadPos))->GetPosition().X()-xhit;
	Double_t dy=(fECal_hitEvent->Hit(leadPos))->GetPosition().Y()-yhit;
	if(i!=leadPos && leadEn>200.){
	  hname="ECal_DTimeLead_otherHits_EHitTot1SP";
	  hSvc->FillHisto(hname, dTime, 1.);
	  hname="ECal_DxLead_otherHits_EHitTot1SP";
	  hSvc->FillHisto(hname, dx, 1.);
	  hname="ECal_DyLead_otherHits_EHitTot1SP";
	  hSvc->FillHisto(hname, dy, 1.);
	  if(energy>200){
	    hname="ECal_DTimeLead_otherHitsEThr200_EHitTot1SP";
	    hSvc->FillHisto(hname, dTime, 1.);
	    hname="ECal_DxLead_otherHitsEThr200_EHitTot1SP";
	    hSvc->FillHisto(hname, dx, 1.);
	    hname="ECal_DyLead_otherHitsEThr200_EHitTot1SP";
	    hSvc->FillHisto(hname, dy, 1.);
	  }
	   
	}
	for(int j=i+1; j<fNhits; j++){
	  Double_t dTimehits=hit->GetTime()-(fECal_hitEvent->Hit(j))->GetTime();
	  Double_t dxhits=xhit-(fECal_hitEvent->Hit(j))->GetPosition().X();
	  Double_t dyhits=yhit-(fECal_hitEvent->Hit(j))->GetPosition().Y();
	  hname="ECal_DHitsX_EHitTot1SP";
	  hSvc->FillHisto(hname, dxhits, 1.);
	  hname="ECal_DHitsY_EHitTot1SP";
	  hSvc->FillHisto(hname, dyhits, 1.);
	  hname="ECal_DHitsTime_EHitTot1SP";
	  hSvc->FillHisto(hname, dTimehits, 1.);
	  if(energy>200 && (fECal_hitEvent->Hit(j))->GetEnergy()>200){
	    hname="ECal_DHitsEThr200X_EHitTot1SP";
	    hSvc->FillHisto(hname, dxhits, 1.);
	    hname="ECal_DHitsEThr200Y_EHitTot1SP";
	    hSvc->FillHisto(hname, dyhits, 1.);
	    hname="ECal_DHitsEThr200Time_EHitTot1SP";
	    hSvc->FillHisto(hname, dTimehits, 1.);
	  }
	}
      }
    }
  
    if(eTot>700 && eTot<1150){
      hname="ECal_EHitTot_2SingleParticle";
      hSvc->FillHisto(hname, eTot, 1.);
      hname="ECal_NCluster_2SingleParticleOnEHitTot";
      hSvc->FillHisto(hname, fNclus, 1.);
     
      for (Int_t i=0; i<fNhits; ++i){
	hit = fECal_hitEvent->Hit(i);
	Double_t energy = hit->GetEnergy();
	Double_t xhit=hit->GetPosition().X();
	Double_t yhit=hit->GetPosition().Y();
	hname="ECal_Map_700ETotHit1150";
	hSvc->FillHisto2(hname, xhit, yhit, 1.);
	hname="ECal_WMap_700ETotHit1150";
	hSvc->FillHisto2(hname, xhit, yhit, energy);
	Double_t dTime=(fECal_hitEvent->Hit(leadPos))->GetTime()-hit->GetTime();
	Double_t dx=(fECal_hitEvent->Hit(leadPos))->GetPosition().X()-xhit;
	Double_t dy=(fECal_hitEvent->Hit(leadPos))->GetPosition().Y()-yhit;
	if(i!=leadPos){
	  hname="ECal_DTimeLead_otherHits_EHitTot2SP";
	  hSvc->FillHisto(hname, dTime, 1.);
	  hname="ECal_DxLead_otherHits_EHitTot2SP";
	  hSvc->FillHisto(hname, dx, 1.);
	  hname="ECal_DyLead_otherHits_EHitTot2SP";
	  hSvc->FillHisto(hname, dy, 1.);
	  if(energy>200. && leadEn>200. ){
	    hname="ECal_DTimeLead_otherHitsEThr200_EHitTot2SP";
	    hSvc->FillHisto(hname, dTime, 1.);
	    hname="ECal_DxLead_otherHitsEThr200_EHitTot2SP";
	    hSvc->FillHisto(hname, dx, 1.);
	    hname="ECal_DyLead_otherHitsEThr200_EHitTot2SP";
	    hSvc->FillHisto(hname, dy, 1.);
	  }
	}
	for(int j=i+1; j<fNhits; j++){
	  Double_t dTimehits=hit->GetTime()-(fECal_hitEvent->Hit(j))->GetTime();
	  Double_t dxhits=xhit-(fECal_hitEvent->Hit(j))->GetPosition().X();
	  Double_t dyhits=yhit-(fECal_hitEvent->Hit(j))->GetPosition().Y();
	  hname="ECal_DHitsX_EHitTot2SP";
	  hSvc->FillHisto(hname, dxhits, 1.);
	  hname="ECal_DHitsY_EHitTot2SP";
	  hSvc->FillHisto(hname, dyhits, 1.);
	  hname="ECal_DHitsTime_EHitTot2SP";
	  hSvc->FillHisto(hname, dTimehits, 1.);
	  if(energy>200 && (fECal_hitEvent->Hit(j))->GetEnergy()>200){
	    hname="ECal_DHitsEThr200X_EHitTot2SP";
	    hSvc->FillHisto(hname, dxhits, 1.);
	    hname="ECal_DHitsEThr200Y_EHitTot2SP";
	    hSvc->FillHisto(hname, dyhits, 1.);
	    hname="ECal_DHitsEThr200Time_EHitTot2SP";
	    hSvc->FillHisto(hname, dTimehits, 1.);
	  }
	}
      }
    }
     
    if(eTot>1150 && eTot<1650){
      hname="ECal_EHitTot_3SingleParticle";
      hSvc->FillHisto(hname, eTot, 1.);
      hname="ECal_NCluster_3SingleParticleOnEHitTot";
      hSvc->FillHisto(hname, fNclus, 1.);
      for (Int_t i=0; i<fNhits; ++i){
	hit = fECal_hitEvent->Hit(i);
	Double_t energy = hit->GetEnergy();
	Double_t xhit=hit->GetPosition().X();
	Double_t yhit=hit->GetPosition().Y();
	hname="ECal_Map_1150ETotHit1650";
	hSvc->FillHisto2(hname, xhit, yhit, 1.);
	hname="ECal_WMap_1150ETotHit1650";
	hSvc->FillHisto2(hname, xhit, yhit, energy);
	Double_t dTime=(fECal_hitEvent->Hit(leadPos))->GetTime()-hit->GetTime();
	Double_t dx=(fECal_hitEvent->Hit(leadPos))->GetPosition().X()-xhit;
	Double_t dy=(fECal_hitEvent->Hit(leadPos))->GetPosition().Y()-yhit;
	if(i!=leadPos){
	  hname="ECal_DTimeLead_otherHits_EHitTot3SP";
	  hSvc->FillHisto(hname, dTime, 1.);
	  hname="ECal_DxLead_otherHits_EHitTot3SP";
	  hSvc->FillHisto(hname, dx, 1.);
	  hname="ECal_DyLead_otherHits_EHitTot3SP";
	  hSvc->FillHisto(hname, dy, 1.);
	  if(energy>200. && leadEn>200.){
	    hname="ECal_DTimeLead_otherHitsEThr200_EHitTot3SP";
	    hSvc->FillHisto(hname, dTime, 1.);
	    hname="ECal_DxLead_otherHitsEThr200_EHitTot3SP";
	    hSvc->FillHisto(hname, dx, 1.);
	    hname="ECal_DyLead_otherHitsEThr200_EHitTot3SP";
	    hSvc->FillHisto(hname, dy, 1.);
	  }
	}
	for(int j=i+1; j<fNhits; j++){
	  Double_t dTimehits=hit->GetTime()-(fECal_hitEvent->Hit(j))->GetTime();
	  hname="ECal_DHitsTime_EHitTot3SP";
	  hSvc->FillHisto(hname, dTimehits, 1.);
	  Double_t dxhits=xhit-(fECal_hitEvent->Hit(j))->GetPosition().X();
	  Double_t dyhits=yhit-(fECal_hitEvent->Hit(j))->GetPosition().Y();
	  hname="ECal_DHitsX_EHitTot3SP";
	  hSvc->FillHisto(hname, dxhits, 1.);
	  hname="ECal_DHitsY_EHitTot3SP";
	  hSvc->FillHisto(hname, dyhits, 1.);
	  if(energy>200 && (fECal_hitEvent->Hit(j))->GetEnergy()>200){
	    hname="ECal_DHitsEThr200X_EHitTot3SP";
	    hSvc->FillHisto(hname, dxhits, 1.);
	    hname="ECal_DHitsEThr200Y_EHitTot3SP";
	    hSvc->FillHisto(hname, dyhits, 1.);
	    hname="ECal_DHitsEThr200Time_EHitTot3SP";
	    hSvc->FillHisto(hname, dTimehits, 1.);
	  }
	}
      }
    }
     
    if(fNclus==1){
      hname="ECal_ETotClus_NClus1";
      hSvc->FillHisto(hname, eTotCl, 1.);
    }
    if(fNclus==2){
      hname="ECal_ETotClus_NClus2";
      hSvc->FillHisto(hname,eTotCl , 1.);
    }
    if(fNclus==3){
      hname="ECal_TotEClus_NClus3";
      hSvc->FillHisto(hname ,eTotCl, 1.);
    }
  
  return true;
 
}



Bool_t SinglePosRun::InitHistos()
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  int nBin=500;
  int min=0;
  int max=3000;
  hname = "ECal_ClusterEnergy";
  hSvc->BookHisto(hname,nBin,min, max);
  hname = "ECal_HitEnergy";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_hitSeedEnergy"; 
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
  min=-0.5;
  max= 80.5;
  nBin=81;
  hname = "ECal_NHits";
  hSvc->BookHisto(hname,nBin,min, max);

  //Histo for molteplicity studies
  min=-0.5;
  max= 35.5;
  nBin=36;
  hname = "ECal_NCluster";
  hSvc->BookHisto(hname,nBin,min, max);

 
  nBin=600;
  min=0;
  max=10000;
  hname="ECal_EHitTot_in14_16";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_EHitTot_in16_18";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_EHitTot_in18_20";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_EHitTot_in20_22";
  hSvc->BookHisto(hname,nBin,min, max);
   
  Double_t  minX_=0.;
  Double_t maxX_= 4000;
  Double_t nBinX_=1000;
  Double_t  minY_=-0.5;
  Double_t maxY_= 60.5;
  Double_t nBinY_=61;
  hname="ECal_NHitsVSEHitTot";
  hSvc->BookHisto2(hname,nBinX_,minX_, maxX_,nBinY_,minY_, maxY_);
   minX_=0.;
  maxX_= 4000;
  nBinX_=100;
  minY_=-0.5;
  maxY_= 9.5;
  nBinY_=10;
  hname="ECal_NHitsVSEClTot";
  hSvc->BookHisto2(hname,nBinX_,minX_, maxX_,nBinY_,minY_, maxY_);
  
  minX_=-0.5;
  maxX_= 8.5;
  nBinX_=9;
  minY_=-0.5;
  maxY_= 8.5;
  nBinY_=9;
  hname="ECal_NClNPosHitE";
  hSvc->BookHisto2(hname,nBinX_,minX_, maxX_,nBinY_,minY_, maxY_);
 
  min=-0.5;
  max= 2000.5;
  nBin=600;
  hname="ECal_EHitTot_1SingleParticle";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_EHitTot_2SingleParticle";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_EHitTot_3SingleParticle";
  hSvc->BookHisto(hname,nBin,min, max);
  min=-0.5;
  max= 10.5;
  nBin=11;
  hname="ECal_NCluster_SingleParticleOnEHitTot";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_NCluster_2SingleParticleOnEHitTot";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_NCluster_3SingleParticleOnEHitTot";
  hSvc->BookHisto(hname,nBin,min, max);

  min=-0.5;
  max= 2000.5;
  nBin=600;
  hname="ECal_ETotClus_NClus1";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_ETotClus_NClus2";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_TotEClus_NClus3";
  hSvc->BookHisto(hname,nBin,min, max);

  min=-0.5;
  max= 2000;
  nBin=600;
  hname="ECal_EClus_NClus1";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_EClus_NClus2";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_EClus_NClus3";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_EClus_NClus4";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_EClus_NClus5";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_EClus_NClus6";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_EClus_NClus7";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_EClus_NClus8";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECAL_EhitInCl1_ncl1";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECAL_EhitInCl1_ncl2";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECAL_EhitInCl2_ncl2";
  hSvc->BookHisto(hname,nBin,min, max);

  min=-325.5;
  max= 325.5;
  nBin=31;
  hname="ECal_Map_700ETotHit1150";
  hSvc->BookHisto2(hname,nBin,min, max,nBin,min, max);
  hname="ECal_Map_1150ETotHit1650";
  hSvc->BookHisto2(hname,nBin,min, max,nBin,min, max);
  hname="ECal_Map_200ETotHit700";
  hSvc->BookHisto2(hname,nBin,min, max,nBin,min, max);
  hname="ECal_WMap_700ETotHit1150";
  hSvc->BookHisto2(hname,nBin,min, max,nBin,min, max);
  hname="ECal_WMap_1150ETotHit1650";
  hSvc->BookHisto2(hname,nBin,min, max,nBin,min, max);
  hname="ECal_WMap_200ETotHit700";
  hSvc->BookHisto2(hname,nBin,min, max,nBin,min, max);

  min=-0-5;
  max= 30.5;
  nBin=31;
  hname="ECal_EClus_clSizeNClus1";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_EClus_clSizeNClus2";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_EClus_clSizeNClus3";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_EClus_clSizeNClus4";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_EClus_clSizeNClus5";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_EClus_clSizeNClus6";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_EClus_clSizeNClus7";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_EClus_clSizeNClus8";
  hSvc->BookHisto(hname,nBin,min, max);

  hname="ECal_clSize1cl_dTimeHiger10";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_clSize2cl_dTimeHiger10";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_clSize1cl_dTimeHiger20";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_clSize2cl_dTimeHiger20";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_clSize1cl_dTimeHiger30";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_clSize2cl_dTimeHiger30";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_clSize1cl_dTimeHiger40";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_clSize2cl_dTimeHiger40";
  hSvc->BookHisto(hname,nBin,min, max);

  min=-150;
  max= 150;
  nBin=300;
  hname="ECal_nCl2_DTime";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_nCl3_DTime13";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_nCl3_DTime23";
  hSvc->BookHisto(hname,nBin,min, max);

  hname="ECal_DTimeLead_otherHits";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DTimeLead_otherHits_EHitTot1SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DTimeLead_otherHits_EHitTot2SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DTimeLead_otherHits_EHitTot3SP";
  hSvc->BookHisto(hname,nBin,min, max);
 
  hname="ECal_DTimeLead_otherHitsEThr200_EHitTot1SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DTimeLead_otherHitsEThr200_EHitTot2SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DTimeLead_otherHitsEThr200_EHitTot3SP";
  hSvc->BookHisto(hname,nBin,min, max);


  hname="ECal_DHitsTime_EHitTot1SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DHitsTime_EHitTot2SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DHitsTime_EHitTot3SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DHitsEThr200Time_EHitTot1SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DHitsEThr200Time_EHitTot2SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DHitsEThr200Time_EHitTot3SP";
  hSvc->BookHisto(hname,nBin,min, max);

  min=-220.5;
  max= 220.5;
  nBin=21;
  hname="ECal_DxLead_otherHits_EHitTot1SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DyLead_otherHits_EHitTot1SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DxLead_otherHits_EHitTot2SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DyLead_otherHits_EHitTot2SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DxLead_otherHits_EHitTot3SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DyLead_otherHits_EHitTot3SP";
  hSvc->BookHisto(hname,nBin,min, max);
 
 
  hname="ECal_DxLead_otherHitsEThr200_EHitTot1SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DyLead_otherHitsEThr200_EHitTot1SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DxLead_otherHitsEThr200_EHitTot2SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DyLead_otherHitsEThr200_EHitTot2SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DxLead_otherHitsEThr200_EHitTot3SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DyLead_otherHitsEThr200_EHitTot3SP";
  hSvc->BookHisto(hname,nBin,min, max);

  hname="ECal_DHitsX_EHitTot1SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DHitsY_EHitTot1SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DHitsX_EHitTot2SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DHitsY_EHitTot2SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DHitsX_EHitTot3SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DHitsY_EHitTot3SP";
  hSvc->BookHisto(hname,nBin,min, max);

  hname="ECal_DHitsEThr200X_EHitTot1SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DHitsEThr200Y_EHitTot1SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DHitsEThr200X_EHitTot2SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DHitsEThr200Y_EHitTot2SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DHitsEThr200X_EHitTot3SP";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_DHitsEThr200Y_EHitTot3SP";
  hSvc->BookHisto(hname,nBin,min, max);
 
  min=-325.55;
  max= 325.55;
  nBin=31;
  hname="ECAL_DxSeed_ncl2";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECAL_DySeed_ncl2";
  hSvc->BookHisto(hname,nBin,min, max);
  min=-150;
  max= 150;
  nBin=300;
  hname="ECAL_DtSeed_ncl2";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECAL_DHitTime_SameChId";
  hSvc->BookHisto(hname,nBin,min, max);

  hname="ECAL_DRadiusVSDTime_ncl2";
  hSvc->BookHisto2(hname,nBin,min, max,21, -220.5, 220.5 );
  hname="ECAL_DxVSDTime_ncl2";
  hSvc->BookHisto2(hname,nBin,min, max,31, -325.55, 325.55 );
  hname="ECAL_DyVSDTime_ncl2_Dx0";
  hSvc->BookHisto2(hname,nBin,min, max,31, -325.5, 325.5 );
 
  hname="ECAL_Map2ndCluster_ncl2";
  hSvc->BookHisto2(hname,31, -325.5, 325.5 ,31, -325.5, 325.5 );
  hname="ECAL_Map1stCluster_ncl2";
  hSvc->BookHisto2(hname,31, -325.5, 325.5 ,31, -325.5, 325.5 );
  min=0.;
  max= 2000;
  nBin=800;
  hname="ECAL_Energy2ndCluster_ncl2";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECAL_Energy1stCluster_ncl2";
  hSvc->BookHisto(hname,nBin,min, max);
  min=0.;
  max= 8;
  nBin=8;
  hname="ECAL_NHitInChannel";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECAL_NHitInChannelVSETotHit";
  hSvc->BookHisto2(hname,800, 0., 3000., nBin,min, max);
  return true;
}


void SinglePosRun::CheckNumberHitsOnCh(Int_t chSeed1, Int_t chSeed2, Int_t& countHit1, Int_t& countHit2){
  TRecoVHit* hit=NULL;
  Int_t countHitForCh1=0;
  Int_t countHitForCh2=0;
  Int_t fNhits = fECal_hitEvent->GetNHits();
  for (Int_t i=0; i<fNhits; ++i){
    hit = fECal_hitEvent->Hit(i);
    Int_t chId=hit->GetChannelId();
    if(chSeed1==chId){countHitForCh1++;}
    if(chSeed2==chId){countHitForCh2++;}
  }
  countHit1=countHitForCh1;
  countHit2=countHitForCh2;
  return; 
}

