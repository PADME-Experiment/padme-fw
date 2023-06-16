#include "SinglePhotonAnalysis.hh"
#include "HistoSvc.hh"
#include <iostream>

static SinglePhotonAnalysis ThisAnalyser("SinglePhotonAnalysis");

SinglePhotonAnalysis::SinglePhotonAnalysis(const char *s,int valid, int verb)
  : PadmeVAnalyser(s,valid,verb)
{;}

SinglePhotonAnalysis::SinglePhotonAnalysis(const char *s):
  PadmeVAnalyser(s)
{;}

Bool_t SinglePhotonAnalysis::InitHistos(){

  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  hSvcVal->makeFileDir(GetName());
  hSvcVal->BookHisto(this->GetName()+"_PVeto_NHits",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_NClusters",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_PVeto_NClusters",100,0.0,100.0);
 

  
  return true;
}


Bool_t SinglePhotonAnalysis::Process(){
  HistoSvc* hSvc =  HistoSvc::GetInstance();

  if(evt->PVetoRecoEvent) 
    hSvc->FillHisto(this->GetName()+"_PVeto_NHits",evt->PVetoRecoEvent->GetNHits());

  TRecoVCluster* cluECal=NULL;
  TRecoVCluster* cluPVeto=NULL;

  Int_t NclusECal = evt->ECalRecoCl->GetNElements();
  Int_t NclusPVeto = evt->PVetoRecoCl->GetNElements();

  hSvc->FillHisto(this->GetName()+"_ECal_NClusters",NclusECal);
  hSvc->FillHisto(this->GetName()+"_PVeto_NClusters",NclusPVeto);
  
  fResult = true;
  return fResult;
}
