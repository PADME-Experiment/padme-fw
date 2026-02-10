//
// Management of Event-level information
//
#include "GeneralInfo.hh"
#include "TMath.h"
#include "OfflineServer.hh"

GeneralInfo* GeneralInfo::fInstance = 0;

GeneralInfo* GeneralInfo::GetInstance(){
  if ( fInstance == 0 ) { fInstance = new GeneralInfo(); }

  return fInstance;
}


GeneralInfo::GeneralInfo(){
  fRecoEvent   = NULL;
  fDBRunNumber =0;
  fMe = 0.511;
}

GeneralInfo::~GeneralInfo(){}

Bool_t GeneralInfo::Init(PadmeAnalysisEvent* event, Int_t DBRunNumber){
  fRecoEvent = event->RecoEvent;
  fDBRunNumber = DBRunNumber;

  Int_t trueRunNumber=0;
  fDBRunNumber==0 ? trueRunNumber= fRecoEvent->GetRunNumber(): trueRunNumber=fDBRunNumber;
  std::cout<<"General Info init: DBRunNumber: "<<fDBRunNumber<<" trueRunNumber:"<<trueRunNumber<<std::endl;
  fOfflineServerDB = OfflineServer::GetInstance();
  
  fRunOld = -1;
  // set global values for each run period

  if (trueRunNumber < 50151) {
    fPeriodStartTime = 1600256773;// sec, first good run of 2020 run, 30339
    fBeamMomentum = 428.48;   // MeV, DHSTB02 energy for 30339
    fZECal = 2508.31;       // mm,  = 2550.51 - 230./2. + 6.5*X0, X0=11.2 mm: should override what's in the reco
    fZTarg = -1028;
   }
  else if(trueRunNumber < 60000){
    fPeriodStartTime = 1664807042;    // sec, first good run of 2022 run, 50151 
    fBeamMomentum = 268.94;       // MeV, DHSTB01 energy for 50381
    //fZECal = 2508.31 + 175.650 ; // mm, relative offset from 2022 survey: average of 176.9 and 174.4
    fZECal = 2612.4 ; // front face of ECAL (crystal front) from MC Detector setup 40, no shower max, ok for positrons
    fZTarg = -1028; 
   }
  else if(trueRunNumber > 80000){
    fPeriodStartTime = 174000000;
    fBeamMomentum = 268.94;
    fZTarg = -732.47;
    fZECal = 2577.77; //2326.5 Carbon fiber window + 20 cm estimate of chamber+ecal cup	
   }

  // default start and stop time of runs
  fRunStartTime = 0;
  fRunStopTime = 1;

  fXTarg = 0.;    // default
  fYTarg = 0.;    // default
  //fZTarg = -1028; // was -1030 originally but set at -1028 after the 2020 survey

  fXCOG = 0.;    // default
  fYCOG = 0.;    // default

  fGlobalESlope = 1.; // default energy fractional correction
  fGlobalTimeESlope = 0.; // default energy fractional correction as a function of the time within the burst
  fGlobalBunchTimeStart = -100.; // to be cross-checked
  fGlobalBunchTimeLength = 300.; // to be cross-checked

  fQuadrantTemperature[0] = 37.1; // degrees
  fQuadrantTemperature[1] = 37.1; // degrees
  fQuadrantTemperature[2] = 37.1; // degrees
  fQuadrantTemperature[3] = 37.1; // degrees
  fQuadrantTempCorr[0] = 0.0; // correction
  fQuadrantTempCorr[1] = 0.0; // correction
  fQuadrantTempCorr[2] = 0.0; // correction
  fQuadrantTempCorr[3] = 0.0; // correction


  fMMStripPitch      = 1.2; // mm
  fMMOffsetPlaneX[0] = 0; // mm 
  fMMOffsetPlaneY[0] = 0; // mm
  fMMPosPlaneZ[0]	   = 2357.32;// mm
  fMMOffsetPlaneX[1] = 0; // mm
  fMMOffsetPlaneY[1] = 0;  // mm
  fMMPosPlaneZ[1]    = 2458.32; // mm
  fMMOffsetCenterX   = 9.995;  // mm
  fMMOffsetCenterY   = 20.1975;  // mm
  fMMPosPlaneZ[2]    = 0.5*(fMMPosPlaneZ[0]+fMMPosPlaneZ[1]); //mm


  
  fIsEnergyAvailable = kFALSE;
  fIsTargetAvgAvailable = kFALSE;
  fIsCOGAvailable = kFALSE;
  fIsBunchLengthAvailable = kFALSE;
  fIsCalibEnergyAvailable = kFALSE;
  fIsCalibTimeEnergyAvailable = kFALSE; 
  fIsTemperatureAvailable = kFALSE;
  RetrieveDBInfo(trueRunNumber);
  
  EvalBeamProperties();

  return true;
}

//
// Can potentially yield event-level beam information (should input PadmeAnalysisEvent* ).
// 
Bool_t GeneralInfo::Process(){ 

  //int runID = fRecoEvent->GetRunNumber();

  Int_t runID=0;
  fDBRunNumber==0 ? runID= fRecoEvent->GetRunNumber(): runID=fDBRunNumber;
  //std::cout<<"DBRunNumber: "<<DBRunNumber<<" runID:"<<trueRunNumber<<std::endl;
 
  fisMC = fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED);
  
  if (runID != fRunOld) { // update run-level information
    // fBeamMomentum = fOfflineServerDB->getDHSTB01Energy(runID);
    // fIsEnergyAvailable = fOfflineServerDB->isEnergyAvailable(runID);
    // // might interpolate if info not available

    // fBunchLength = fOfflineServerDB->getBunchLength(runID);
    // fBeamStart =  fOfflineServerDB->getBeamStart(runID);
    // fIsBunchLengthAvailable = fOfflineServerDB->isBunchLengthAvailable(runID);
    // // might interpolate if info not available
    
    // if (runID < 50151) {
    //   fPeriodStartTime = 1600256773;// sec, first good run of 2020 run, 30339
    //   fZECal = 2508.31; // mm,  = 2550.51 - 230./2. + 6.5*X0, X0=11.2 mm: should override reco
    // }
    // else {
    //   fPeriodStartTime = 1664807042;    // sec, first good run of 2022 run, 50151 
    //   fZECal = 2508.31 + 175.650; // mm, from 2022 survey: average of 176.9 and 174.4
    // }

    // fXTarg = fOfflineServerDB->getTargetXAvg(runID);
    // fYTarg = fOfflineServerDB->getTargetYAvg(runID);
    // fIsTargetAvgAvailable = fOfflineServerDB->isTargetAvgAvailable(runID);
    // // might interpolate if info not available

    // fXCOG = fOfflineServerDB->getCOGXAvg(runID);
    // fYCOG = fOfflineServerDB->getCOGYAvg(runID);
    // fIsCOGAvailable = fOfflineServerDB->isCOGAvailable(runID);
    // // might interpolate if info not available

    // fCalibEnergyFactor = fOfflineServerDB->getCalibEnergyFactor(runID);
    // fIsCalibEnergyAvailable = fOfflineServerDB->isCalibEnergyAvailable(runID);
    // // might interpolate if info not available

    // fCalibTimeEnergyFactor = fOfflineServerDB->getCalibTimeEnergyFactor(runID);
    // fIsCalibTimeEnergyAvailable = fOfflineServerDB->isCalibTimeEnergyAvailable(runID);
    // // might interpolate if info not available

    // for (int quad = 0; quad < 4; quad++){
    //   fQuadrantTemperature[quad] = fOfflineServerDB->getQuadrantTemperature(runID, quad);
    //   fQuadrantTempCorr[quad] = fOfflineServerDB->getQuadrantTempCorr(runID, quad);
    // }
    // fIsTemperatureAvailable = fOfflineServerDB->isTemperatureAvailable(runID);

    // EvalBeamProperties();
    // PrintBeamProperties(runID);
    RetrieveDBInfo(runID);
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
  fE = sqrt(fMe*fMe + fBeamMomentum*fBeamMomentum);
  fSqrts = sqrt(2.*fMe*fMe + 2.*fBeamMomentum*fMe);
  fBG = fBeamMomentum/fSqrts; // beta gamma
  fGam = sqrt(fBG*fBG+1.);
  fBeta = fBG/fGam;

  fBoostMom.SetXYZ(fCOGAtECal.X()-fRTarg.X(),fCOGAtECal.Y()-fRTarg.Y(),fCOGAtECal.Z()-fRTarg.Z());
  fBoostMom *= (fBeta/fBoostMom.Mag());
  fRadiusMax = 270.0;
  //fRadiusMax = 270.;//304.5; // in the past, we used 300 mm

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
  std::cout<<"General Info: fEnergyMax : "<<fEnergyMax<<" fEnergyMin "<<fEnergyMin<<std::endl;
}


void GeneralInfo::PrintBeamProperties(int runID){
  std::cout << "GeneralInfo: run-level info for run " << runID ;
  std::cout << " Pbeam = " << fBeamMomentum << " StartT = " << static_cast<long long int>(fRunStartTime) <<  " StopT = " << static_cast<long long int>(fRunStopTime) <<
    " target = { "<< fRTarg.X()<< " , "<< fRTarg.Y() << " , " << fRTarg.Z() << " }; COG = { " << fCOGAtECal.X() << " , " << fCOGAtECal.Y() << " , "<< fCOGAtECal.Z() << " }" << 
    " sqrt(s) = " << fSqrts << " bg = " << fBG << " beta = " << fBeta <<
    " energyRange = { " << fEnergyMin << " , " << fEnergyMax << " }; radiusRange = { " << fRadiusMin << " , " << fRadiusMax << " }" << 
    " Temp per quad[deg]: " << fQuadrantTemperature[0] << " , " << fQuadrantTemperature[1] << " , " << fQuadrantTemperature[2] << " , " << fQuadrantTemperature[3] <<
    " Period:"<<fPeriod<<std::endl;
}

void GeneralInfo::RetrieveDBInfo(int runID){
  fRunNumberFormDB = runID;
  fRunStartTime = fOfflineServerDB->getRunStartTime(runID);
  fRunStopTime =  fOfflineServerDB->getRunStopTime(runID);

    fBeamMomentum = fOfflineServerDB->getDHSTB01Energy(runID);
    fIsEnergyAvailable = fOfflineServerDB->isEnergyAvailable(runID);
    // might interpolate if info not available

    fBunchLength = fOfflineServerDB->getBunchLength(runID);
    fBeamStart =  fOfflineServerDB->getBeamStart(runID);
    fIsBunchLengthAvailable = fOfflineServerDB->isBunchLengthAvailable(runID);
    // might interpolate if info not available
    
    // if (runID < 50151) {
    //   fPeriodStartTime = 1600256773;// sec, first good run of 2020 run, 30339
    //   fZECal = 2508.31; // mm,  = 2550.51 - 230./2. + 6.5*X0, X0=11.2 mm: should override reco
    // }
    // else {
    //   fPeriodStartTime = 1664807042;    // sec, first good run of 2022 run, 50151 
    //   fZECal = 2508.31 + 175.650; // mm, from 2022 survey: average of 176.9 and 174.4
    // }

    fXTarg = fOfflineServerDB->getTargetXAvg(runID);
    fYTarg = fOfflineServerDB->getTargetYAvg(runID);
    fIsTargetAvgAvailable = fOfflineServerDB->isTargetAvgAvailable(runID);
    // might interpolate if info not available

    fXCOG = fOfflineServerDB->getCOGXAvg(runID);
    fYCOG = fOfflineServerDB->getCOGYAvg(runID);
    fIsCOGAvailable = fOfflineServerDB->isCOGAvailable(runID);
    // might interpolate if info not available

    fCalibEnergyFactor = fOfflineServerDB->getCalibEnergyFactor(runID);
    std::cout<<"Value EScale: "<<fCalibEnergyFactor<<std::endl;
    fIsCalibEnergyAvailable = fOfflineServerDB->isCalibEnergyAvailable(runID);
    // might interpolate if info not available

    fCalibTimeEnergyFactor = fOfflineServerDB->getCalibTimeEnergyFactor(runID);
    fIsCalibTimeEnergyAvailable = fOfflineServerDB->isCalibTimeEnergyAvailable(runID);
    // might interpolate if info not available

    fPeriod = fOfflineServerDB->getPeriod(runID);
    fIsPeriodAvailable = fOfflineServerDB->isPeriodAvailable(runID);

    fLGCorr = fOfflineServerDB->getLGCorr(runID);
    fIsLGCorrAvailable = fOfflineServerDB->isLGCorrAvailable(runID);

    for (int quad = 0; quad < 4; quad++){
      fQuadrantTemperature[quad] = fOfflineServerDB->getQuadrantTemperature(runID, quad);
      fQuadrantTempCorr[quad] = fOfflineServerDB->getQuadrantTempCorr(runID, quad);
    }
    fIsTemperatureAvailable = fOfflineServerDB->isTemperatureAvailable(runID);

    EvalBeamProperties();
    PrintBeamProperties(runID);
    return;
}

// boardSN plane Layer side  view otherview hole offset stripid_orig quad
// 0       0     0     0     0    0         6    0      1-256        0
// 1       0     0     1     0    0         6    256    257-512      1
// 2       0     1     0     0    1         6    0      0-255        3
// 3       0     1     1     0    1         6    256    256-511      2
// 4       0     2     0     1    1         1    0      0-255        1
// 5       0     2     1     1    1         1    256    256-511      2
// 6       0     3     0     1    0         1    0      0-255        0
// 7       0     3     1     1    0         1    256    256-511      3
// 8       1     4     0     0    0         1    0      0-255        0
// 9       1     4     1     0    0         1    256    256-511      1
// 10      1     5     0     0    1         1    0      0-255        3
// 11      1     5     1     0    1         1    256    256-511      2
// 12      1     6     0     1    1         6    0      0-255        1
// 13      1     6     1     1    1         6    256    256-511      2
// 14      1     7     0     1    0         6    0      0-255        0
// 15      1     7     1     1    0         6    256    256-511      3

MMchInfo GeneralInfo::DecodeMMChannel(int chId){
  int otherview[8] = {0,1,1,0,0,1,1,0}; // 0 means the half-strip left (bottom) depending on the view
  
  MMchInfo mmi;
  mmi.bdid = (chId & 0xF00 ) >> 8; // board SN 0-15
  mmi.layer = (mmi.bdid)/2;  // layer 0-7
  if      (mmi.bdid == 0 || mmi.bdid == 6 || mmi.bdid == 8 || mmi.bdid == 14) mmi.quad = 0;
  else if (mmi.bdid == 1 || mmi.bdid == 4 || mmi.bdid == 9 || mmi.bdid == 12) mmi.quad = 1;
  else if (mmi.bdid == 3 || mmi.bdid == 5 || mmi.bdid == 11 || mmi.bdid == 13) mmi.quad = 2;
  else if (mmi.bdid == 2 || mmi.bdid == 7 || mmi.bdid == 10 || mmi.bdid == 15) mmi.quad = 3;
  else mmi.quad = -999;
  mmi.plane = (mmi.layer)/4;  // plane 0-1
  mmi.verse = ((mmi.plane )== 0)? 1 : -1;
  mmi.side = mmi.bdid%2; // left/right (X view), bottom/top (Y view)
  mmi.strip = (chId & 0x0FF); // strip 0-255
  mmi.view = (mmi.layer/2)%2; // 0 means Y view, 1 means X view    
  mmi.otherview = otherview[mmi.layer]; // 0 means the half-strip left (bottom) depending on the view
  int packed = (mmi.side << 2);
  packed |= (mmi.view<<1);
  packed |= (mmi.otherview);
  mmi.packed = packed;
  return mmi;
}
