// Written by M. Raggi 1/04/2022
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
  fNRun=0;	     
  fCurrentRun=-1.;     
  fCurrentRunIndex=-1.;
}

ECalCalib::~ECalCalib(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t ECalCalib::Init(){
  if (fVerbose) printf("---> Initializing ECalCalib\n");
  InitHistos(); 
  int NRun;
  double EBeam;
  double EAvgRun;
  double SlopeRun;
  double E0;
  double COGX;
  double COGY;
  double NPoT;
  double NPoTRes;

  // read calibration file data
  ifstream InFile("ParamECal.txt");
  if(!InFile.is_open()) {
    cout<<"Cannot open ECal calibration File!!!";
  }else{
    while(!InFile.eof()){
      InFile>>NRun>>EBeam>>EAvgRun>>E0>>SlopeRun>>COGX>>COGY>>NPoT>>NPoTRes;
      vNRun.push_back(NRun);
      vEBeam.push_back(EBeam);
      vEAvgRun.push_back(EAvgRun);
      vE0Run.push_back(E0);
      vSlopeRun.push_back(SlopeRun);
      vCOGX.push_back(COGX);
      vCOGY.push_back(COGY);
      cout<<"NRun "<<NRun<<" EBeam "<<EBeam<<" EAvgRun "<<EAvgRun<<" SlopeRun "<<SlopeRun<<" "<<COGX<<" "<<COGY<<" "<<NPoT<<" "<<NPoTRes<<endl;
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

Bool_t ECalCalib::Process(PadmeAnalysisEvent* event){
  fEvent = event;
  if (fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) fisMC=true;
  //  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  Int_t NEvent = fEvent->RecoEvent->GetEventNumber(); 
  fNRun      = fEvent->RecoEvent->GetRunNumber(); //Occhio che 30000 vale solo per il 2022
  fNClusters = fEvent->ECalRecoCl->GetNElements();

  // Retrieve Run index   
  if(fNRun!=fCurrentRun){
    for(Int_t ll=0;ll<vNRun.size();ll++){
      //      cout<<ll<< "Process NRUN "<<fNRun<<" CurrentRun "<<fCurrentRun<<endl;
      if(vNRun[ll]==fNRun){ 
	fCurrentRunIndex=ll; //retrieve the current run index
	fBeamEnergy = vEBeam[ll];
	fCOGX       = vCOGX[ll];
        fCOGY       = vCOGY[ll];
	cout<<ll<< "Process NRUN "<<fNRun<<" CurrentRun "<<fCurrentRun<<" "<<fCOGX<<" "<<fCOGY<<endl;
	break;
      }
    }
  }
  fCurrentRun=fNRun;  
  //  cout<<"Process NClus "<<fNClusters<<" NRUN "<<fNRun<<" RunIndex "<<fCurrentRunIndex<<endl;
  return true;
}

// does this fix what Geometrical of COG postion?
Double_t ECalCalib::FixPosition(){
  if(fisMC) return -1;
  Double_t X,X_CORR;
  Double_t Y,Y_CORR;
  
  if(fCurrentRunIndex!=-1) X_CORR = vCOGX[fCurrentRunIndex];
  if(fCurrentRunIndex!=-1) Y_CORR = vCOGY[fCurrentRunIndex];
  if(fCurrentRunIndex==-1) X_CORR = 0;
  if(fCurrentRunIndex==-1) Y_CORR = 0;

  Int_t NClusters =fEvent->ECalRecoCl->GetNElements();
  //  cout<<"NClus "<<NClusters<<endl;
  for(int ical = 0;ical < NClusters; ical++) {
    TVector3 pos = fEvent->ECalRecoCl->Element(ical)->GetPosition();
    //    cout<<"COGX Before "<<pos.X()<<" COGY Before "<<pos.Y()<<endl;
    double X =  pos.X()-X_CORR;
    double Y =  pos.Y()-Y_CORR;
    //    cout<<"COGX "<<X<<" COGY "<<Y<<endl;
    //    fEvent->ECalRecoCl->Element(ical)->SetPosition.X(COGX);
    //    fEvent->ECalRecoCl->Element(ical)->SetPosition.Y(COGY);
  }
  return 1.;
}


//Correct for the energy slope vs time in the bunch
Double_t ECalCalib::CorrectESlope(){
  if(fisMC) return -1;
  Double_t ESlope=0;
  if(fCurrentRunIndex!=-1) ESlope = vSlopeRun[fCurrentRunIndex];
  if(fCurrentRunIndex==-1) ESlope = fGlobalESlope; //if nothing available for that run
  
  Int_t NClusters =fEvent->ECalRecoCl->GetNElements();
  //  cout<<"NClus "<<NClusters<<endl;
  for(int ical = 0;ical < NClusters; ical++) {
    double eECal    =  fEvent->ECalRecoCl->Element(ical)->GetEnergy();
    double FracE = ESlope*fEvent->ECalRecoCl->Element(ical)->GetTime();
    //    cout<<" eECal Before "<<eECal<<" Time "<<fEvent->ECalRecoCl->Element(ical)->GetTime()<<endl;
    eECal -= FracE*eECal;
    //    cout<<" eECal after "<<eECal<<" FracE "<<FracE<<endl;
    fEvent->ECalRecoCl->Element(ical)->SetEnergy(eECal);
  }
  return 1;
}

// SET RUN DEPENDENT ENERGY SCALE FOR ECAL

Double_t ECalCalib::SetEScale(){
  if(fisMC) return -1;
  Int_t NEvent = fEvent->RecoEvent->GetEventNumber(); 
  Double_t EScale;
  
  if(fCurrentRunIndex!=-1) EScale = vEBeam[fCurrentRunIndex]/vEAvgRun[fCurrentRunIndex];
  if(fCurrentRunIndex==-1) EScale = fGlobalEScale; //if nothing available for that run;

  if(NEvent==5) {
    for(int ical = 0;ical < fNClusters; ical++) {
      cout<<"Non calib Energy "<<fEvent->ECalRecoCl->Element(ical)->GetEnergy()<<endl;
    }
  }
  if(fNClusters>1 && fNClusters<4 && 
     fabs(fEvent->ECalRecoCl->Element(0)->GetTime()-fEvent->ECalRecoCl->Element(1)->GetTime())<3.){
    double E0 = fEvent->ECalRecoCl->Element(0)->GetEnergy();
    double E1 = fEvent->ECalRecoCl->Element(1)->GetEnergy();
    fHS->FillHistoList("ECalCalib","ETot2g",E0+E1);
  }
  
  for(int ical = 0;ical < fNClusters; ical++) {
    double eECal    =  fEvent->ECalRecoCl->Element(ical)->GetEnergy();
    eECal*=  EScale;  //Data ECal energy Need the reco to be calibrated
    fEvent->ECalRecoCl->Element(ical)->SetEnergy(eECal);
    if(NEvent==5) cout<<"calib Energy "<<fEvent->ECalRecoCl->Element(ical)->GetEnergy()<<" EScale "<<EScale<<" "<<endl;
  }
  
  if(fNClusters>1 && fNClusters<4 &&
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
