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
  fCfgFile = cfgFile;
}

ECalCalib::~ECalCalib(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t ECalCalib::Init(){
  if (fVerbose) printf("---> Initializing ECalCalib\n");

  fHS = HistoSvc::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)fCfgFile.Data());
  fGeneralInfo = GeneralInfo::GetInstance();
  fisMC=false;

  fNPairs = 0;
  fPairIndex[0] = -1; // index of first cluster of a good pair 
  fPairIndex[1] = -1; // index of second cluster

  InitHistos(); 
  return true;
}

Bool_t ECalCalib::InitHistos(){
  // ECalCalib directory will contain all histograms related to this analysis
  fHS->CreateList("ECalCalib");
  const Double_t hEMax=800;
  const Int_t hEBins=800;

  fHS->BookHistoList("ECalCalib","ETot2g"  ,hEBins,0.,hEMax);
  fHS->BookHistoList("ECalCalib","ETot2gCal"  ,hEBins,0.,hEMax);

  fHS->BookHisto2List("ECalCalib","ETot2gVsTrelBefore" ,100,-200.,700.,hEBins/2,0.,hEMax);
  fHS->BookHisto2List("ECalCalib","ETot2gVsTrelAfter"  ,100,-200.,700.,hEBins/2,0.,hEMax);
  return true;
}

Bool_t ECalCalib::Process(PadmeAnalysisEvent* event){
  fEvent = event;
  if (fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) fisMC=true;
  //  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  Int_t NEvent = fEvent->RecoEvent->GetEventNumber();
  if(!fisMC && NEvent%10000==0) cout<<"ECalCalib NEvent "<<NEvent<<endl;

  fNPairs = NClusterPairSimpleSelection();
  CorrectETimeSlope(1); //fill plots before and after time-energy slope correction
  CorrectEScale(1); // fill plots before and after energy scale correction      

  return true;
}


//
// Correct energy for a time-dependent energy slope 
//
Int_t ECalCalib::CorrectETimeSlope(Int_t corrLevel){
  if (fisMC) return -1; // At the moment we don't have the information to correct the MC

  // retrieve time-energy slope

  Double_t ESlope; // Mauro was using  5E-5 if no RUN dependent value is found, but here I'm changing parametrisation
  if (fGeneralInfo->IsCalibTimeEnergyAvailable()){
    ESlope = fGeneralInfo->GetCalibTimeEnergyFactor(); 
  } else {
    ESlope = fGeneralInfo->GetGlobalTimeESlope(); 
  }

  // retrieve start time and duration of bunch

  Double_t TStart=0;
  Double_t TWidth=0;
  if (fGeneralInfo->IsBunchLengthAvailable()){
    TStart = fGeneralInfo->GetBeamStart(); 
    TWidth = fGeneralInfo->GetBunchLength(); 
  } else {
    TStart = fGeneralInfo->GetGlobalBunchTimeStart();
    TWidth = fGeneralInfo->GetGlobalBunchTimeLength();
  }

  // evaluate average time of cluster pairs, if any, and fill "uncorrected" histogram

  double TAvg = 0;
  if (fNPairs == 1) {
    TAvg = 0.5*(
		fEvent->ECalRecoCl->Element(fPairIndex[0])->GetTime()+
		fEvent->ECalRecoCl->Element(fPairIndex[1])->GetTime());
    Double_t ETot = 
      fEvent->ECalRecoCl->Element(fPairIndex[0])->GetEnergy() +
      fEvent->ECalRecoCl->Element(fPairIndex[1])->GetEnergy();
    fHS->FillHistoList("ECalCalib","ETot2gVsTrelBefore",(TAvg-TStart),ETot);
  }

  // apply time-energy correction

  for(int ical = 0;ical < fEvent->ECalRecoCl->GetNElements(); ical++) {
    double eECal =  fEvent->ECalRecoCl->Element(ical)->GetEnergy();
    double FracE = ESlope*(fEvent->ECalRecoCl->Element(ical)->GetTime() - TStart);

    if(corrLevel) {
      eECal -= FracE;    //modified
      fEvent->ECalRecoCl->Element(ical)->SetEnergy(eECal);
    }
  }

  // Fill Etot vs T AFTER correction
  if (fNPairs == 1) {
    Double_t ETot = 
      fEvent->ECalRecoCl->Element(fPairIndex[0])->GetEnergy() +
      fEvent->ECalRecoCl->Element(fPairIndex[1])->GetEnergy();
    fHS->FillHistoList("ECalCalib","ETot2gVsTrelAfter",(TAvg-TStart),ETot);
  }

  return 1;
}

// 
// Correct energy for a run-dependent slope
//
Int_t ECalCalib::CorrectEScale(Int_t corrLevel){

  // fill total-energy uncorrected plot
  if (fNPairs == 1) {
    fHS->FillHistoList("ECalCalib","ETot2g",
		       fEvent->ECalRecoCl->Element(fPairIndex[0])->GetEnergy() +
		       fEvent->ECalRecoCl->Element(fPairIndex[1])->GetEnergy());
  }


  if(fisMC) return -1; // at the moment, it's unclear what mis-calibration is in the mc


  // retrieve energy scale correction

  Double_t EScale; // Mauro was using 1.11398 if no RUN dependent value is found, I'm using 1  
  if(fGeneralInfo->IsCalibEnergyAvailable()) EScale = fGeneralInfo->GetCalibEnergyFactor();
  else EScale = fGeneralInfo->GetGlobalESlope();
  
  // apply energy scale correction

  for(int ical = 0;ical < fEvent->ECalRecoCl->GetNElements(); ical++) {
    double eECal    =  fEvent->ECalRecoCl->Element(ical)->GetEnergy();
    if(corrLevel) {
      if (EScale > 0) eECal /=  EScale;  //Data ECal energy Need the reco to be calibrated
      fEvent->ECalRecoCl->Element(ical)->SetEnergy(eECal);
    }
  }

  // fill histogram with corrected total energy
  if (fNPairs == 1) {
    fHS->FillHistoList("ECalCalib","ETot2gCal",      
		       fEvent->ECalRecoCl->Element(fPairIndex[0])->GetEnergy() +
		       fEvent->ECalRecoCl->Element(fPairIndex[1])->GetEnergy());
  }

  return 1;
}

Bool_t ECalCalib::Finalize(){ // should evaluate the corrections in a simil-histo mode?
  if (fVerbose) printf("---> Finalizing ECalCalib\n");
  return true;
}

// selection of cluster pairs with simple cuts, not energy-dependent
Int_t ECalCalib::NClusterPairSimpleSelection(){
  // should check if these values are OK for every beam momentum
  const Double_t  MinECluster=80.;  //MeV
  const Double_t  MaxECluster=250.; //MeV
  const Double_t ClRadMin= 125.; //mm
  const Double_t ClRadMax= 270.; //mm
  const Double_t MinClClDistance = 250.; //mm
  const Double_t MaxClClDT = 3.; // ns
  int NClusters = fEvent->ECalRecoCl->GetNElements();

  // loop on cluster pairs, apply radius and energy cuts, apply distance in time and space  

  int nPairs = 0;
  fPairIndex[0] = -1;
  fPairIndex[1] = -1;
  
  for (Int_t ii=0; ii<NClusters;ii++){
    double energy_i    =  fEvent->ECalRecoCl->Element(ii)->GetEnergy();
    if(energy_i < MinECluster || energy_i > MaxECluster) continue; 

    TVector3 pos_i    =  fEvent->ECalRecoCl->Element(ii)->GetPosition();
    double radius_i = sqrt(pos_i.X()*pos_i.X() + pos_i.Y()*pos_i.Y());
    if(radius_i < ClRadMin || radius_i > ClRadMax) continue;

    for (Int_t jj=ii+1; jj<NClusters;jj++){
      double energy_j    =  fEvent->ECalRecoCl->Element(jj)->GetEnergy();
      if(energy_j < MinECluster || energy_j > MaxECluster) continue; 

      TVector3 pos_j    =  fEvent->ECalRecoCl->Element(jj)->GetPosition();
      double radius_j = sqrt(pos_j.X()*pos_j.X() + pos_j.Y()*pos_j.Y());
      if(radius_j < ClRadMin || radius_j > ClRadMax) continue;

      double dist = sqrt(
			 (pos_i.X()-pos_j.X())*(pos_i.X()-pos_j.X()) + 
			 (pos_i.Y()-pos_j.Y())*(pos_i.Y()-pos_j.Y()) );
      if(dist<MinClClDistance) continue;

      double dt = 
	fEvent->ECalRecoCl->Element(ii)->GetTime() - 
	fEvent->ECalRecoCl->Element(jj)->GetTime();
      if (fabs(dt) > MaxClClDT) continue;
      
      nPairs++;
      fPairIndex[0] = ii;
      fPairIndex[1] = jj;
    }
  }
  return nPairs;
}
