#include "HEPVetoAnalysis.hh"

#include "THEPVetoRecoEvent.hh"
#include "TRecoVHit.hh"
#include "HistoSvc.hh"
#include <iostream>

HEPVetoAnalysis::HEPVetoAnalysis()
{
  fhitEvent=NULL;
  fClColl=NULL;
  fVerbose = 0;
  fValidation = 0;
  InitHistos();
}
HEPVetoAnalysis::HEPVetoAnalysis(Int_t valid, Int_t verb)
{
  fhitEvent=NULL;
  fClColl=NULL;
  fVerbose = verb;
  fValidation = valid;
  InitHistos();
}
HEPVetoAnalysis::~HEPVetoAnalysis()
{
}
Bool_t HEPVetoAnalysis::InitHistos()
{
  if (fValidation)
    {
      InitHistosValidation("HEPVeto");
      return true;
    }
 
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
Bool_t HEPVetoAnalysis::Init(THEPVetoRecoEvent* ev, TRecoVClusCollection* cl)
{
  Bool_t retCode = 0;
  fhitEvent = ev;
  fClColl   = cl;
  fTmin = -100.;
  fTmax =  180.;

  //InitHistos();
  
  return retCode;
}
Bool_t HEPVetoAnalysis::Process()
{
  Bool_t retCode = 0;
  if (fValidation)
    {
      ProcessValidation("HEPVeto");
      return true;
    }


  HistoSvc* hSvc =  HistoSvc::GetInstance();

  Double_t eMax  =  0.;
  Int_t    iLead = -1;
  TRecoVHit* hit=NULL;
  TRecoVHit* hitn=NULL;
  std::string hname;

  Int_t      chId;
  Double_t energy;
  Double_t   time;
  Int_t      chIdn;
  Double_t energyn;
  Double_t   timen;

  
  Int_t fNhits = fhitEvent->GetNHits();
  for (Int_t i=0; i<fNhits; ++i){
    hit    = fhitEvent->Hit(i);
    chId   = hit->GetChannelId();
    energy = hit->GetEnergy();
    time   = hit->GetTime();


    hname = "HEPVeto_Hit_TimeVSch";
    hSvc->FillHisto2(hname,time, (float)chId,  1.);
    
    if (time<fTmin || time>fTmax) continue;
    for (Int_t j=0; j<fNhits; ++j)
      {
	if (i==j) continue;
	hitn = fhitEvent->Hit(j);
	chIdn   = hitn->GetChannelId();
	energyn = hitn->GetEnergy();
	timen   = hitn->GetTime();

	if (chIdn==chId){
	  hname = "HEPVeto_Hit_DTime_DCh0";
	  hSvc->FillHisto(hname,time-timen,  1.);
	}
	else if (abs(chIdn-chId)<2){
	  hname = "HEPVeto_Hit_DTime_DCh1";
	  hSvc->FillHisto(hname,time-timen,  1.);
	  hname = "HEPVeto_Hit_DTimeVSch_DCh1";
	  hSvc->FillHisto2(hname,time-timen,(float)chId,  1.);
	}
	else if (abs(chIdn-chId)<3){
	  hname = "HEPVeto_Hit_DTime_DCh2";
	  hSvc->FillHisto(hname,time-timen,  1.);
	}
	else{
	  hname = "HEPVeto_Hit_DTime_DChLarge";
	  hSvc->FillHisto(hname,time-timen,  1.);
	}
	
      }

    if (energy > eMax) {iLead = i; eMax=energy;}

    hname = "HEPVeto_HitEnergy";
    hSvc->FillHisto(hname,energy,1.);
    
    hname = "HEPVeto_HitMap";
    hSvc->FillHisto(hname, (Double_t)chId, 1.);

    /*
    // ntuple variables
    if (i<1000)
      {
	hSvc->myEvt.NTHEPVetoTrkFinger[i]=hit->GetChannelId();
	hSvc->myEvt.NTHEPVetoTrkTime[i]  =hit->GetTime();
      }
    */
   }
   hSvc->myEvt.NTNHEPVetoTracks=fNhits;

  
   return retCode;
}
