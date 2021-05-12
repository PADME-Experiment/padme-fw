#include "UserAnalysis.hh"
#include "HistoSvc.hh"
#include <iostream>


UserAnalysis::UserAnalysis(){
  ;
}

UserAnalysis::~UserAnalysis(){
  ;
}
Bool_t UserAnalysis::Init(PadmeAnalysisEvent *event){
  evt = event;
  if (fMode < 2) InitHistos();//active on standard analysis and validation mode 
  return true;
}

Bool_t UserAnalysis::InitHistos(){

  HistoSvc* hSvcVal =  HistoSvc::GetInstance();

  //  hSvcVal->BookHisto(hname, nBin, min, max);
  
  hSvcVal->BookHisto("UserAnalysis", 256, 0., 256.);

  //hSvcVal->BookHisto2("PVetoPos_vs_ECalE", 100,0.0,100,400,0.0, 400);

  return true;
}

#define ABS(x)  ((x) > 0 ? (x):(-x))

Bool_t UserAnalysis::Process(){

  HistoSvc* hSvc =  HistoSvc::GetInstance();
  //  std::cout << evt->PVetoRecoEvent->GetNHits() << std::endl;
  hSvc->FillHisto("UserAnalysis",  evt->RecoEvent->GetTriggerMask() ,1.);

  /*
  for(int ipv = 0;ipv <  evt->PVetoRecoEvent->GetNHits(); ipv++) {
    double tPv = evt->PVetoRecoEvent->Hit(ipv)->GetTime();
    int chPV = evt->PVetoRecoEvent->Hit(ipv)->GetChannelId();

    //Correct the PVeto timing:
    // linear inyterpolation 
    //tPv = tPv - (30.9219 + 0.137262*chPV);
    // channel by channel 
    tPv = tPv - timeOffsetPVeto[chPV];
    evt->PVetoRecoEvent->Hit(ipv)->SetTime(tPv);
  }
  */

  /*
  //Calo-Calo checks
  for(int ical = 0;ical <  evt->ECalRecoCl->GetNElements(); ical++) {
    double eECal =  evt->ECalRecoCl->Element(ical)->GetEnergy();
    double tECal =  evt->ECalRecoCl->Element(ical)->GetTime();
    TVector3 pos1 =  evt->ECalRecoCl->Element(ical)->GetPosition();
    hSvc->FillHisto("ECalClEnergy",eECal,1);
  }
  */

  return true;
}


