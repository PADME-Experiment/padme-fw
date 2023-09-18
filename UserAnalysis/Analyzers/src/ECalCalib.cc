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
}

ECalCalib::~ECalCalib(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t ECalCalib::Init(){
  if (fVerbose) printf("---> Initializing ECalCalib\n");

  fHS = HistoSvc::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
  fGeneralInfo = GeneralInfo::GetInstance();
  fisMC=false;

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

  Double_t ESlope; // Mauro was using  5E-5 if no RUN dependent value is found, but here I'm changing parametrisation
  if (fGeneralInfo->IsCalibTimeEnergyAvailable()){
    ESlope = fGeneralInfo->GetCalibTimeEnergyFactor(); 
  } else {
    ESlope = fGeneralInfo->GetGlobalTimeESlope(); 
  }

  Double_t TStart=0;
  Double_t TWidth=0;
  if (fGeneralInfo->IsBunchLengthAvailable()){
    TStart = fGeneralInfo->GetBeamStart(); 
    TWidth = fGeneralInfo->GetBunchLength(); 
  } else {
    TStart = fGeneralInfo->GetGlobalBunchTimeStart();
    TWidth = fGeneralInfo->GetGlobalBunchTimeLength();
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

  Double_t EScale; // Mauro was using 1.11398 if no RUN dependent value is found, I'm using 1
  
  if(fGeneralInfo->IsCalibEnergyAvailable()) {
    EScale = fGeneralInfo->GetCalibEnergyFactor();
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
