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
  
  hSvcVal->BookHisto(this->GetName()+"_SAC_NHits",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_SAC_chs", 100, 0.0, 100.0);
  
  hSvcVal->BookHisto(this->GetName()+"_SAC22_vs_PVETOchs", 100, -300.0, 300.0);

  for (int ch = 1; ch <91; ch++) {  
    hSvcVal->BookHisto(this->GetName()+"_SAC22_vs_PVETO_"+std::to_string(ch), 3000, -300.0, 300.0);
  }
  /*
  for (int ch = 1; ch <91; ch++) {  
    hSvcVal->BookHisto(this->GetName()+"_SACE_vs_Pch"+std::to_string(ch), 300, 0.0, 600.0);
  }
  */

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

      
    hSvc->FillHisto(this->GetName()+"_SAC_NHits",evt->SACRecoEvent->GetNHits());

    for (int ih = 0; ih < evt->SACRecoEvent->GetNHits(); ih++) {
      //if (evt->SACRecoEvent->Hit(ih)->GetChannelId() == 22) {
      hSvc->FillHisto(this->GetName()+"_SAC_chs", evt->SACRecoEvent->Hit(ih)->GetChannelId());
        //hSvc->FillHisto(this->GetName()+"_SAC_chs", evt->SACRecoEvent->Hit(ih)->GetTime());
      //}
    }
      

    // callibration PVeto channels vs single SAC channel -> 5ns peak near the mean value
    //std::cout << "channels?" << evt->SACRecoEvent->GetNHits() << std::endl;
    for(int ih = 0;ih<evt->PVetoRecoEvent->GetNHits(); ih++) {
      for (int ihs = 0; ihs < evt->SACRecoEvent->GetNHits(); ihs++) {
        //if (evt->SACRecoEvent->Hit(ihs)->GetChannelId() == 22 && evt->PVetoRecoEvent->Hit(ih)->GetChannelId() == 15) {
        if (evt->SACRecoEvent->Hit(ihs)->GetChannelId() == 22) {//33
          //hSvc->FillHisto(this->GetName()+"_SAC_chs", evt->SACRecoEvent->Hit(ih)->GetChannelId());
          for (int pch = 1; pch<91; pch++) {
            if (pch == evt->PVetoRecoEvent->Hit(ih)->GetChannelId()) {  
              hSvc->FillHisto(this->GetName()+"_SAC22_vs_PVETO_"+std::to_string(pch), evt->SACRecoEvent->Hit(ihs)->GetTime() - evt->PVetoRecoEvent->Hit(ih)->GetTime());
            }
          }
        }
      }
    }

    /*
    for (int ihs = 0; ihs < evt->SACRecoEvent->GetNHits(); ihs++) {
        if (evt->SACRecoEvent->Hit(ihs)->GetChannelId() == 22) {//33
      for (int ip=0; ip<evt->PVetoRecoEvent->GetNHits(); ip++) {
        for (int pch =1; pch<91; pch++) {
          if (pch == evt->PVetoRecoEvent->Hit(ip)->GetChannelId()) {  
            hSvc->FillHisto(this->GetName()+"_SACE_vs_Pch"+std::to_string(pch), evt->SACRecoEvent->Hit(ihs)->GetEnergy());
            //hSvc->FillHisto(this->GetName()+"_SACE_vs_Pch"+std::to_string(pch), evt->SACRecoEvent->Hit(ihs)->GetEnergy() + evt->PVetoRecoEvent->Hit(ip)->GetEnergy());
          }
        }
      }
    }
    }
    */

  }

  fResult = true;
  return fResult;
}
