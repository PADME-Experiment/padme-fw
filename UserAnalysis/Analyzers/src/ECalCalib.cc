// Written by M. Raggi 1/04/2022
#include "ECalCalib.hh"

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
}

ECalCalib::~ECalCalib(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t ECalCalib::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing ECalCalib\n");
  fEvent = event;
  InitHistos();
  if (fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) fisMC=true;
  int NRun;
  double EAvgRun;
  double SlopeRun;

  // read calibration file data
  ifstream InFile("ParamECal.txt");
  if(!InFile.is_open()) {
    cout<<"Cannot open ECal calibration File!!!";
  }else{
    while(!InFile.eof()){
      InFile>>NRun>>EAvgRun>>SlopeRun;
      //      cout<<"NRun "<<NRun<<" EAvgRun "<<EAvgRun<<" SlopeRun "<<SlopeRun<<endl;
      vNRun.push_back(NRun);
      vEAvgRun.push_back(EAvgRun);
      vSlopeRun.push_back(SlopeRun);
    }
  }
  InFile.close();
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

//Correct for the energy slope vs time in the bunch
Double_t ECalCalib::CorrectESlope(){
  if(fisMC) return -1;
  Double_t ESlope=0;
  for(Int_t ll=0;ll<vNRun.size();ll++){
    if(vNRun[ll]==fNRun){ 
      ESlope = vSlopeRun[ll];
      //      cout<<"Slope "<<ESlope<<"Run "<<vNRun[ll]<<endl;
    }
  }

  Int_t NClusters =fEvent->ECalRecoCl->GetNElements();
  //  cout<<"NClus "<<NClusters<<endl;
  for(int ical = 0;ical < NClusters; ical++) {
    double eECal    =  fEvent->ECalRecoCl->Element(ical)->GetEnergy();
    if(!fisMC) eECal+=  ESlope*fEvent->ECalRecoCl->Element(ical)->GetTime();
    fEvent->ECalRecoCl->Element(ical)->SetEnergy(eECal);
  }
  return 1;
}


Bool_t ECalCalib::Process(){
  //  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  return true;
}

// SET RUN DEPENDENT ENERGY SCALE FOR ECAL

Double_t ECalCalib::SetEScale(){
  if(fisMC) return -1;
  Double_t EScale;
  EScale=fGlobalEScale; //if nothing available
  
  fNRun   = fEvent->RecoEvent->GetRunNumber()-30000.;
  //  Int_t NEvent = fEvent->RecoEvent->GetEventNumber(); 
  
  // Retrieve Run dependent energy scale and Energy vs time slope
  
  for(Int_t ll=0;ll<vNRun.size();ll++){
    if(vNRun[ll]==fNRun){ 
      EScale = fGlobalEScale * 430/vEAvgRun[ll];
    }
  }
  
  Int_t NClusters =fEvent->ECalRecoCl->GetNElements();
  //  cout<<"NClus "<<NClusters<<endl;
      
  if(NClusters>1 && NClusters<4 && 
     fabs(fEvent->ECalRecoCl->Element(0)->GetTime()-fEvent->ECalRecoCl->Element(1)->GetTime())<3.){
    double E0 = fEvent->ECalRecoCl->Element(0)->GetEnergy();
    double E1 = fEvent->ECalRecoCl->Element(1)->GetEnergy();
    fHS->FillHistoList("ECalCalib","ETot2g",E0+E1);
  }
  
  for(int ical = 0;ical < NClusters; ical++) {
    double eECal    =  fEvent->ECalRecoCl->Element(ical)->GetEnergy();
    eECal*=  EScale;  //Data ECal energy Need the reco to be calibrated
    fEvent->ECalRecoCl->Element(ical)->SetEnergy(eECal);
  }
  
  if(NClusters>1 && NClusters<4 &&
     fabs(fEvent->ECalRecoCl->Element(0)->GetTime()-fEvent->ECalRecoCl->Element(1)->GetTime())<3.){
    double E0 = fEvent->ECalRecoCl->Element(0)->GetEnergy();
    double E1 = fEvent->ECalRecoCl->Element(1)->GetEnergy();
    fHS->FillHistoList("ECalCalib","ETot2gCal",E0+E1);
  }
  return 1;
}

Bool_t ECalCalib::Finalize()
{
  if (fVerbose) printf("---> Finalizing ECalCalib\n");
  return true;
}
