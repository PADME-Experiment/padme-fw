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
  hSvcVal->BookHisto(this->GetName()+"_MyHisto",10,0.0,10.0);

  
  return true;
}


Bool_t UserTemplateAnalyser::Process(){

  fResult = true;
  return fResult;
}
