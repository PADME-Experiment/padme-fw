#include "PVetoAnalysis.hh"

#include "TPVetoRecoEvent.hh"
#include "TRecoVHit.hh"
#include "HistoSvc.hh"
#include <iostream>

PVetoAnalysis::PVetoAnalysis()
{
  fNhits=0;
  fhitEvent=NULL;
}
PVetoAnalysis::~PVetoAnalysis()
{
  fNhits=0;
  fhitEvent=NULL;
}
Bool_t PVetoAnalysis::Init(Int_t nh, TPVetoRecoEvent* ev)
{
  Bool_t retCode = 0;
  fNhits    = nh;
  fhitEvent = ev;

  return retCode;
}
Bool_t PVetoAnalysis::Process()
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

    hname = "PVeto_HitEnergy";
    hSvc->FillHisto(hname,energy,1.);
    
    hname = "PVeto_HitTime";
    hSvc->FillHisto(hname,time,  1.);
    hname = "PVeto_HitMap";
    hSvc->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);
    hname = "PVeto_EneMap";
    hSvc->FillHisto2(hname, (Double_t)ix, (Double_t)iy, energy);
     
    //if(time>0)std::cout << "Pveto energy and time "<< energy << " " << time<< std::endl;

    // ntuple variables
    if (i<1000)
      {
	hSvc->myEvt.NTPVetoTrkFinger[i]=hit->GetChannelId();
	hSvc->myEvt.NTPVetoTrkTime[i]  =hit->GetTime();
      }
   }
  hSvc->myEvt.NTNPVetoTracks=fNhits;

   hname = "PVeto_Etot";
   hSvc->FillHisto(hname,eTot,1.);

   TRecoVHit* lHit = fhitEvent->Hit(iLead);
   Int_t lix = lHit->GetChannelId()/100;
   Int_t liy = lHit->GetChannelId()%100;
   Double_t lenergy = lHit->GetEnergy();

   hname = "PVeto_LeadingHitTime";
   hSvc->FillHisto(hname,lHit->GetTime(),1.);
   hname = "PVeto_LeadingHitEnergy";
   hSvc->FillHisto(hname,lenergy,1.);
   hname = "PVeto_LeadingHitMap";
   hSvc->FillHisto2(hname, lix, liy, 1.);
   hname = "PVeto_LeadingEneMap";
   hSvc->FillHisto2(hname, lix, liy, lenergy);
   
   //hSvc->FillNtuple();
  
   return retCode;
}
