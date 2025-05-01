#include "SPA_ECalEfficiency.hh"
#include "HistoSvc.hh"
#include <iostream>

static SPA_ECalEfficiency ThisAnalyser("SPA_ECalEfficiency");

SPA_ECalEfficiency::SPA_ECalEfficiency(const char *s,int valid, int verb)
  : PadmeVAnalyser(s,valid,verb)
{;}

SPA_ECalEfficiency::SPA_ECalEfficiency(const char *s):
  PadmeVAnalyser(s)
{;}

Bool_t SPA_ECalEfficiency::InitHistos(){

  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  hSvcVal->makeFileDir(GetName());
  hSvcVal->BookHisto(this->GetName()+"_PVeto_NHits",100,0.0,100.0);

  
  return true;
}


Bool_t SPA_ECalEfficiency::Process(){
  HistoSvc* hSvc =  HistoSvc::GetInstance();

  if(evt->PVetoRecoEvent) 
    hSvc->FillHisto(this->GetName()+"_PVeto_NHits",evt->PVetoRecoEvent->GetNHits());
  
  fResult = true;
  return fResult;
}
