//
// Management of Event-level information
//
#include "GeneralInfo.hh"
#include "TMath.h"

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

  // set global values for each run period

  if (fRecoEvent->GetRunNumber() < 50151) {
    fBeamMomentum = 428.48;   // MeV, DHSTB02 energy for 30339
    fZECal = 2508.31;       // mm,  = 2550.51 - 230./2. + 6.5*X0, X0=11.2 mm: should override what's in the reco
  }
  else {
    fBeamMomentum = 268.94;       // MeV, DHSTB01 energy for 50381
    fZECal = 2508.31 + 175.650; // mm, relative offset from 2022 survey: average of 176.9 and 174.4
  }

  fXTarg = 0.;    // default
  fYTarg = 0.;    // default
  fZTarg = -1028; // was -1030 originally but set at -1028 after the 2020 survey

  fXCOG = 0.;    // default
  fYCOG = 0.;    // default

  fGlobalESlope = 1.; // default energy fractional correction
  fGlobalTimeESlope = 0.; // default energy fractional correction as a function of the time within the burst
  fGlobalTimeStart = -100.; // to be cross-checked
  fGlobalTimeWidth = 150.; // to be cross-checked

  fIsEnergyAvailable = kFALSE;
  fIsTargetAvgAvailable = kFALSE;
  fIsCOGAvailable = kFALSE;
  fIsBunchLengthAvailable = kFALSE;
  fIsCalibEnergyAvailable = kFALSE;
  fIsCalibTimeEnergyAvailable = kFALSE; 

  
  EvalBeamProperties();

  return true;
}

//
// Can potentially yield event-level beam information.
//
Bool_t GeneralInfo::Process(){ 

  int runID = fRecoEvent->GetRunNumber();

  if (runID != fRunOld) { // update run-level information
    fBeamMomentum = fOfflineServerDB->getDHSTB01Energy(runID);
    fIsEnergyAvailable = fOfflineServerDB->isEnergyAvailable(runID)
    // might interpolate if info not available

    fBunchLength =  fOfflineServerDB->getBunchLength(runID);
    fBeamStart =  fOfflineServerDB->getBeamStart(runID);
    fIsBunchLengthAvailable = fOfflineServerDB->isBunchLengthAvailable(runID)
    // might interpolate if info not available
    
    if (runID < 50151) {
      fZECal = 2508.31; // mm,  = 2550.51 - 230./2. + 6.5*X0, X0=11.2 mm: should override reco
    }
    else {
      fZECal = 2508.31 + 175.650; // mm, from 2022 survey: average of 176.9 and 174.4
    }

    fXTarg = fOfflineServerDB->getTargetXAvg(runID);
    fYTarg = fOfflineServerDB->getTargetYAvg(runID);
    fIsTargetAvgAvailable = fOfflineServerDB->isTargetAvgAvailable(runID);
    // might interpolate if info not available

    fXCOG = fOfflineServerDB->getCOGXAvg(runID);
    fYCOG = fOfflineServerDB->getCOGYAvg(runID);
    fIsCOGAvailable = fOfflineServerDB->isCOGAvailable(runID);
    // might interpolate if info not available

    fCalibEnergyFactor = fOfflineServerDB->getCalibEnergyFactor(runID);
    fIsCalibEnergyAvailable = fOfflineServerDB->isCalibEnergyAvailable(runID);
    // might interpolate if info not available

    fCalibTimeEnergyFactor = fOfflineServerDB->getCalibTimeEnergyFactor(runID);
    fIsCalibTimeEnergyAvailable = fOfflineServerDB->isCalibTimeEnergyAvailable(runID);
    // might interpolate if info not available

    EvalBeamProperties();
    PrintBeamProperties(runID);
    fRunOld = runID;
    return 1;
  }

  return 0;
}

//
// Evaluate derived beam properties
//
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

  // if K = RMax/D is the max tangent in the lab, pi/2 - t < q*/2 < t, where t = atan(gamma RMax/D) must be > pi/4
  // t = pi/4 if gam = 1/K, i.e. at ~ 150 MeV

  double tanQMax = fRadiusMax/(fCOGAtECal.Z()-fRTarg.Z());
  double tLim = TMath::ATan(fGam*tanQMax);
  if (tLim < TMath::Pi()*0.25) {
    std::cout << "No solution? " << tLim << " " << fGam << std::endl;
  }
  
  //  double tanQMin = 1./(fGam*fGam*tanQMax);
  
  fRadiusMin = (fCOGAtECal.Z()-fRTarg.Z())*TMath::Tan(0.5*TMath::Pi()-tLim)/fGam ;// (fCOGAtECal.Z()-fRTarg.Z())*tanQMin; 
  fEnergyMax = fSqrts*fGam*0.5*(1.-TMath::Cos(2*tLim)); 
  fEnergyMin = fSqrts*fGam*0.5*(1.+TMath::Cos(2*tLim)); 
}


void GeneralInfo::PrintBeamProperties(int runID){
  std::cout << "GeneralInfo: run-level info for run " << runID << std::endl;
  std::cout << " Pbeam = " << fBeamMomentum << " StartT = " << static_cast<long long int>(fStartTime) << 
    " target = { "<< fRTarg.X()<< " , "<< fRTarg.Y() << " , " << fRTarg.Z() << " }; COG = { " << fCOGAtECal.X() << " , " << fCOGAtECal.Y() << " , "<< fCOGAtECal.Z() << " }" << 
    " sqrt(s) = " << fSqrts << " bg = " << fBG << " beta = " << fBeta <<
    " energyRange = { " << fEnergyMin << " , " << fEnergyMax << " }; radiusRange = { " << fRadiusMin << " , " << fRadiusMax << " }" << std::endl;
}
