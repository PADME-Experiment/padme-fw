#include "ECalAnalysis.hh"

#include "TECalRecoEvent.hh"
#include "TRecoVHit.hh"
#include "HistoSvc.hh"
#include <iostream>

ECalAnalysis::ECalAnalysis()
{
  fNhits=0;
  fhitEvent=NULL;
}
ECalAnalysis::~ECalAnalysis()
{
  fNhits=0;
  fhitEvent=NULL;
}
Bool_t ECalAnalysis::Init(Int_t nh, TECalRecoEvent* ev)
{
  Bool_t retCode = 0;
  fNhits    = nh;
  fhitEvent = ev;

  return retCode;
}
Bool_t ECalAnalysis::Process()
{
  Bool_t retCode = 0;

  HistoSvc* hSvc =  HistoSvc::GetInstance();

  Double_t eMax  =  0.;
  Int_t    iLead = -1;
  Double_t eTot  =  0.;
  TRecoVHit* hit=NULL;
  std::string hname;	
  for (Int_t i=0; i<fNhits; ++i){
    hit = fhitEvent->Hit(i);
    Int_t ix = hit->GetChannelId()/100;
    Int_t iy = hit->GetChannelId()%100;
    Double_t energy = hit->GetEnergy();
    eTot += energy;
    if (energy > eMax) {iLead = i; eMax=energy;}
    Double_t time = hit->GetTime();

    hname = "ECal_HitEnergy";
    hSvc->FillHisto(hname,energy,1.);
    hname = "ECal_HitTime";
    hSvc->FillHisto(hname,time,  1.);
    hname = "ECal_HitMap";
    hSvc->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);
    hname = "ECal_EneMap";
    hSvc->FillHisto2(hname, (Double_t)ix, (Double_t)iy, energy);

    // ntuple variables 
    hSvc->myEvt.NTECluster[i]=hit->GetEnergy();
    hSvc->myEvt.NTTCluster[i]=hit->GetTime();
    hSvc->myEvt.NTXCluster[i]=hit->GetChannelId()/100;
    hSvc->myEvt.NTYCluster[i]=hit->GetChannelId()%100;
    
   }

   hname = "ECal_Etot";
   hSvc->FillHisto(hname,eTot,1.);

   TRecoVHit* lHit = fhitEvent->Hit(iLead);
   Int_t lix = lHit->GetChannelId()/100;
   Int_t liy = lHit->GetChannelId()%100;
   Double_t lenergy = lHit->GetEnergy();

   hname = "ECal_LeadingHitTime";
   hSvc->FillHisto(hname,lHit->GetTime(),1.);
   hname = "ECal_LeadingHitEnergy";
   hSvc->FillHisto(hname,lenergy,1.);
   hname = "ECal_LeadingHitMap";
   hSvc->FillHisto2(hname, lix, liy, 1.);
   hname = "ECal_LeadingEneMap";
   hSvc->FillHisto2(hname, lix, liy, lenergy);
  
   return retCode;
}
