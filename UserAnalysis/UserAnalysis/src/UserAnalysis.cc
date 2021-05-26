#include <iostream>

#include "UserAnalysis.hh"
#include "HistoSvc.hh"
#include "TempCorr.hh"

UserAnalysis::UserAnalysis(TString cfgFile, Int_t verbose)
{
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Configuration file %s\n",cfgFile.Data());
    printf("---> Verbose level %d\n",fVerbose);
  }
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
}

UserAnalysis::~UserAnalysis(){;}

Bool_t UserAnalysis::Init(PadmeAnalysisEvent* event){
  fEvent = event;
  InitHistos();
  return true;
}

Bool_t UserAnalysis::InitHistos(){

  HistoSvc* hSvc =  HistoSvc::GetInstance();

  // This will go to the top directory in the output file
  hSvc->BookHistoList("TOP","TestTOP",10,0.,10.);

  // No list defined: this will go the EXTRA directory in the output file
  hSvc->BookHisto("Test",10,0.,10.);

  hSvc->CreateList("MyHistos");
  hSvc->BookHistoList("MyHistos","Trigger Mask",256,0.,256.);
  hSvc->BookHistoList("MyHistos","Triggers",8,0.,8.);
  hSvc->BookHisto2List("MyHistos","Test2D",10,0.,10.,10,0.,10.);

  return true;
}

#define ABS(x)  ((x) > 0 ? (x):(-x))

Bool_t UserAnalysis::Process(){

  HistoSvc* hSvc =  HistoSvc::GetInstance();
  //  std::cout << fEvent->PVetoRecoEvent->GetNHits() << std::endl;
  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  //hSvc->FillHisto("Trigger Mask",trigMask,1.);
  //for (int i=0;i<8;i++) { if (trigMask & (1 << i)) hSvc->FillHisto("Triggers",i,1.); }
  hSvc->FillHistoList("MyHistos","Trigger Mask",trigMask,1.);
  for (int i=0;i<8;i++) { if (trigMask & (1 << i)) hSvc->FillHistoList("MyHistos","Triggers",i,1.); }
  /*
  for(int ipv = 0;ipv <  fEvent->PVetoRecoEvent->GetNHits(); ipv++) {
    double tPv = fEvent->PVetoRecoEvent->Hit(ipv)->GetTime();
    int chPV = fEvent->PVetoRecoEvent->Hit(ipv)->GetChannelId();

    //Correct the PVeto timing:
    // linear inyterpolation 
    //tPv = tPv - (30.9219 + 0.137262*chPV);
    // channel by channel 
    tPv = tPv - timeOffsetPVeto[chPV];
    fEvent->PVetoRecoEvent->Hit(ipv)->SetTime(tPv);
  }
  */

  /*
  //Calo-Calo checks
  for(int ical = 0;ical <  fEvent->ECalRecoCl->GetNElements(); ical++) {
    double eECal =  fEvent->ECalRecoCl->Element(ical)->GetEnergy();
    double tECal =  fEvent->ECalRecoCl->Element(ical)->GetTime();
    TVector3 pos1 =  fEvent->ECalRecoCl->Element(ical)->GetPosition();
    hSvc->FillHisto("ECalClEnergy",eECal,1);
  }
  */

  return true;
}

Bool_t UserAnalysis::Finalize()
{
  return true;
}
