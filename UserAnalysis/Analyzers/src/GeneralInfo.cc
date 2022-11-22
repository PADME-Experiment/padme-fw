#include "GeneralInfo.hh"

GeneralInfo* GeneralInfo::fInstance = 0;

GeneralInfo* GeneralInfo::GetInstance(){
  if ( fInstance == 0 ) { fInstance = new GeneralInfo(); }
  return fInstance;
}


GeneralInfo::GeneralInfo(){
  fRecoEvent   = NULL;
  fMe = 0.511;
}

GeneralInfo::~GeneralInfo(){}

Bool_t GeneralInfo::Init(PadmeAnalysisEvent* event){
  fRecoEvent = event->RecoEvent;
  fOfflineServerDB = OfflineServer::GetInstance();

  fRunOld = -1;

  // set period values and defaults

  if (fRecoEvent->GetRunNumber() < 50151) {
    fStartTime = 1600256773;// sec, first good run of 2020 run, 30339
    fBeamMomentum = 428.48;   // MeV, DHSTB02 energy for 30339
    fZECal = 2508.31;       // mm,  = 2550.51 - 230./2. + 6.5*X0, X0=11.2 mm: should override what's in the reco
  }
  else {
    fStartTime = 1664807042;    // sec, first good run of 2022 run, 50151 
    fBeamMomentum = 268.94;       // MeV, DHSTB01 energy for 50381
    fZECal = 2508.31 + 175.650; // mm, relative offset from 2022 survey: average of 176.9 and 174.4
  }

  fXTarg = 0.;    // default
  fYTarg = 0.;    // default
  fZTarg = -1028; // was -1030 originally but set at -1028 after the 2020 survey

  fXCOG = 0.;    // default
  fYCOG = 0.;    // default

  EvalBeamProperties();

  return true;
}

void GeneralInfo::EvalBeamProperties(){
  fRTarg.SetXYZ(fXTarg,fYTarg,fZTarg);
  fCOGAtECal.SetXYZ(fXCOG,fYCOG,fZECal);

  fSqrts = sqrt(2.*fMe*fMe + 2.*fBeamMomentum*fMe);
  fBG = fBeamMomentum/fSqrts; // beta gamma
  fGam = sqrt(fBG*fBG+1.);
  fBeta = fBG/fGam;

  fBoostMom.SetXYZ(fCOGAtECal.X()-fRTarg.X(),fCOGAtECal.Y()-fRTarg.Y(),fCOGAtECal.Z()-fRTarg.Z());
  fBoostMom *= (fBeta/fBoostMom.Mag());

  fRadiusMax = 304.5; // in the past, we used 300 mm

  double tanQMax = fRadiusMax/(fZECal-fZTarg);
  double tanQMin = 1./(fGam*fGam*tanQMax);

  fRadiusMin = (fZECal-fZTarg)*tanQMin; // 171 vs 140 used for for sqrt(s) = 16.6 
  fEnergyMin = 0.5*fSqrts*(fGam + fBG*TMath::Cos(TMath::ATan(tanQMax))); // 97 vs 90 MeV used for sqrt(s) = 16.6
  fEnergyMax = 0.5*fSqrts*(fGam + fBG*TMath::Cos(TMath::ATan(tanQMin))); // 172 vs 190 MeV used for sqrt(s) = 16.6
    
}

// To pass from a run-level evaluation to a time-based evaluation, need to adapt the Process method.

Bool_t GeneralInfo::Process(){ 

  int runID = fRecoEvent->GetRunNumber();

  if (runID != fRunOld) { // need to update run-level features
    fBeamMomentum = fOfflineServerDB->getDHSTB01Energy(runID);
    fStartTime =  fOfflineServerDB->getRunStartTime(runID);

    if (fRunOld < 50151) {
      fZECal = 2508.31;       // mm,  = 2550.51 - 230./2. + 6.5*X0, X0=11.2 mm: should override what's in the reco
    }
    else {
      fZECal = 2508.31 + 175.650; // mm, relative offset from 2022 survey: average of 176.9 and 174.4
    }

    fXTarg = fOfflineServerDB->getTargetXAvg(runID);
    fYTarg = fOfflineServerDB->getTargetYAvg(runID);
    fXCOG = fOfflineServerDB->getCOGXAvg(runID);
    fYCOG = fOfflineServerDB->getCOGYAvg(runID);
  
    std::cout << "GeneralInfo updating run-level info for run " << runID << " Pbeam = " << fBeamMomentum << " StartT = " << static_cast<long long int>(fStartTime) << 
      " target = { "<< fXTarg<< " , "<< fYTarg << " }; COG = { " << fXCOG << " , " << fYCOG << " }" << std::endl;

    EvalBeamProperties();
    fRunOld = runID;
    return 1;
  }

  return 0;
}

