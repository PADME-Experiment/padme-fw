// Written by M. Raggi 1/04/2022
// Aligned with offline db and event-level info by T. Spadaro 17/9/2023
#include "ECalCalib.hh"

//Added the Finstance to allow to use the information in all the enalyzers
ECalCalib* ECalCalib::fInstance = 0;
ECalCalib* ECalCalib::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new ECalCalib(); }
  return fInstance;
}

ECalCalib::ECalCalib(TString cfgFile, Int_t verbose)
{
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating ECalCalib\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  fHS = HistoSvc::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
  fOfflineServer = OfflineServer::GetInstance();
  fGeneralInfo = GeneralInfo::GetInstance();

}

ECalCalib::~ECalCalib(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t ECalCalib::Init(){
  if (fVerbose) printf("---> Initializing ECalCalib\n");
  InitHistos(); 
  return true;
}

Bool_t ECalCalib::InitHistos(){
  // ECalCalib directory will contain all histograms related to this analysis
  fHS->CreateList("ECalCalib");
  Double_t hEMax=800;
  Double_t hEBins=800;
  fHS->BookHistoList("ECalCalib","ETot2g"  ,hEBins,0.,hEMax);
  fHS->BookHistoList("ECalCalib","ETot2gCal"  ,hEBins,0.,hEMax);
  return true;
}

Bool_t ECalCalib::Process(PadmeAnalysisEvent* event){
  fEvent = event;
  if (fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) fisMC=true;
  //  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();

  return true;
}


//
// Correct energy for a time-dependent energy slope 
//
Int_t ECalCalib::CorrectESlope(){
  if(fisMC) return -1;

  Double_t ESlope=0;
  if (fGeneralInfo->IsEnergyTimeSlopeAvailable()){
    ESlope = fGeneralInfo->GetEnergyTimeSlope(); 
  } else {
    ESlope = fGeneralInfo->GetGlobalTimeESlope(); // it might be the average value or an interpolation
  }

  Double_t TStart=0;
  Double_t TWidth=0;
  if (fOfflineServer->isTimeAvailable(fEvent->RecoEvent->GetRunNumber())){
    TStart = fOfflineServer->getBeamStart(fEvent->RecoEvent->GetRunNumber()); 
    TWidth = fOfflineServer->getBunchLength(fEvent->RecoEvent->GetRunNumber()); 
  } else {
    TStart = fGeneralInfo->GetGlobalTimeStart();
    TWidth = fGeneralInfo->GetGlobalTimeWidth();
  }


  Int_t NClusters =fEvent->ECalRecoCl->GetNElements();

  for(int ical = 0;ical < NClusters; ical++) {
    double eECal    =  fEvent->ECalRecoCl->Element(ical)->GetEnergy();
    double FracE = ESlope*(fEvent->ECalRecoCl->Element(ical)->GetTime() - TStart)/TWidth;

    eECal -= FracE*eECal;    
    fEvent->ECalRecoCl->Element(ical)->SetEnergy(eECal);
  }
  return 1;
}

// 
// Correct energy for a run-dependent slope
//
Double_t ECalCalib::SetEScale(){
  if(fisMC) return -1;
  Int_t NEvent = fEvent->RecoEvent->GetEventNumber(); 

  Double_t EScale;
  
  if(fOfflineServer->isCalibEnergyAvailable(fEvent->RecoEvent->GetRunNumber())) {
    EScale = fOfflineServer->getCalibEnergyFactor(fEvent->RecoEvent->GetRunNumber());
  }  
  else {
    EScale = fGeneralInfo->GetGlobalESlope();
  }

  for(int ical = 0;ical < fEvent->ECalRecoCl->GetNElements(); ical++) {
    double eECal    =  fEvent->ECalRecoCl->Element(ical)->GetEnergy();
    eECal*=  EScale;  //Data ECal energy Need the reco to be calibrated
    fEvent->ECalRecoCl->Element(ical)->SetEnergy(eECal);
  }
  return 1;
}

Bool_t ECalCalib::Finalize()
{
  if (fVerbose) printf("---> Finalizing ECalCalib\n");
  return true;
}
