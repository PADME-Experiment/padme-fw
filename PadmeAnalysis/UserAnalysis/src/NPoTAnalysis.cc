#include "NPoTAnalysis.hh"
#include "HistoSvc.hh"
#include <iostream>

static NPoTAnalysis ThisAnalyser("NPoTAnalysis");

NPoTAnalysis::NPoTAnalysis(const char *s,int valid, int verb)
  : PadmeVAnalyser(s,valid,verb)
{;}

NPoTAnalysis::NPoTAnalysis(const char *s):
  PadmeVAnalyser(s)
{;}

Bool_t NPoTAnalysis::InitHistos(){

  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  hSvcVal->makeFileDir(GetName());
  hSvcVal->BookHisto(this->GetName()+"_PVeto_NHits",100,0.0,100.0);

  
  return true;
}


Bool_t NPoTAnalysis::Process(){
  HistoSvc* hSvc =  HistoSvc::GetInstance();

  if(evt->PVetoRecoEvent) 
    hSvc->FillHisto(this->GetName()+"_PVeto_NHits",evt->PVetoRecoEvent->GetNHits());
    Float_t fNPoTLG =0.;
  Float_t BeamEnergy = 267.37;
  if(evt->LeadGlassRecoEvent){
    fNPoTLG   =  402.5*evt->LeadGlassRecoEvent->GetNPoTs()/BeamEnergy;
 
  }
  
  fResult = true;
  return fResult;
}
