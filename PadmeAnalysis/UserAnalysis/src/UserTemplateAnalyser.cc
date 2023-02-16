#include "UserTemplateAnalyser.hh"
#include "HistoSvc.hh"
#include <iostream>

static UserTemplateAnalyser ThisAnalyser("UserTemplateAnalyser");

UserTemplateAnalyser::UserTemplateAnalyser(const char *s,int valid, int verb)
  : PadmeVAnalyser(s,valid,verb)
{;}

UserTemplateAnalyser::UserTemplateAnalyser(const char *s):
  PadmeVAnalyser(s)
{;}

Bool_t UserTemplateAnalyser::InitHistos(){

  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  hSvcVal->makeFileDir(GetName());
  hSvcVal->BookHisto(this->GetName()+"_PVeto_NHits",100,0.0,100.0);

  
  return true;
}


Bool_t UserTemplateAnalyser::Process(){
  HistoSvc* hSvc =  HistoSvc::GetInstance();

  if(evt->PVetoRecoEvent) 
    hSvc->FillHisto(this->GetName()+"_PVeto_NHits",evt->PVetoRecoEvent->GetNHits());
  
  fResult = true;
  return fResult;
}
