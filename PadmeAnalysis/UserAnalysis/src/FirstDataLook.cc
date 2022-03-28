#include "FirstDataLook.hh"
#include "HistoSvc.hh"
#include <iostream>

static FirstDataLook ThisAnalyser("FirstDataLook");

FirstDataLook::FirstDataLook(const char *s,int valid, int verb)
  : PadmeVAnalyser(s,valid,verb)
{;}

FirstDataLook::FirstDataLook(const char *s):
  PadmeVAnalyser(s)
{;}

Bool_t FirstDataLook::InitHistos(){

  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  hSvcVal->makeFileDir(GetName());
  
  hSvcVal->BookHisto(this->GetName()+"_PVeto_NHits",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_PVeto_ch_occ",100,0.0,100.0);
  
  return true;
}


Bool_t FirstDataLook::Process(){
  HistoSvc* hSvc =  HistoSvc::GetInstance();

  // std::cout << "========================" << this->GetName() << "   called." << std::endl;
  

  
  if(evt->PVetoRecoEvent) {
    hSvc->FillHisto(this->GetName()+"_PVeto_NHits",evt->PVetoRecoEvent->GetNHits());
    for(int ih = 0;ih<evt->PVetoRecoEvent->GetNHits(); ih++) {
      

      
      hSvc->FillHisto(this->GetName()+"_PVeto_ch_occ",evt->PVetoRecoEvent->Hit(ih)->GetChannelId());
    }
    
  }
  fResult = true;
  return fResult;
}
