#include "ECalSel.hh"

#include "TRecoEvent.hh"
#include "TRecoVObject.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "TVector3.h"
#include "TProfile.h"
#include "TString.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TLorentzVector.h"
#include <iostream>

ECalSel* ECalSel::fInstance = 0;
ECalSel* ECalSel::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new ECalSel(); }
  return fInstance;
}


ECalSel::ECalSel()
{
  fRecoEvent   = NULL;
  fECal_hitEvent=0;  
  fVerbose        = 0;
  fECalEvents.clear();
}
ECalSel::~ECalSel()
{
}

Bool_t ECalSel::Init(PadmeAnalysisEvent* event){
  fRecoEvent = event->RecoEvent;
  fECal_hitEvent = event->ECalRecoEvent;
  fECal_clEvent = event->ECalRecoCl; 
  fOfflineServerDB = OfflineServer::GetInstance();
  fhSvcVal =  HistoSvc::GetInstance(); 

  fRunOld = -1;

  InitHistos();
  fdistanceTarget=3470;
  
  return true;
}


Bool_t ECalSel::Process(){
  std::string hname;
  
  fECalEvents.clear();
  TwoClusSel();

  return true;
}



Int_t ECalSel::TwoClusSel(){

  const double me = 0.511;

  // run level properties

  int runID = fRecoEvent->GetRunNumber();
  double beamEnergy = 268.94;// default
  double sqrts = sqrt(2.*me*me + 2.*beamEnergy*me);
  double bg = beamEnergy/sqrts; // beta gamma
  double gam = sqrt(bg*bg+1.);
  double beta = bg/gam;

  if (runID != fRunOld) { // need to update run-level features: TO BE DONE, beam direction [cog, target pos]
    beamEnergy = fOfflineServerDB->getDHSTB01Energy(runID);
    sqrts = sqrt(2.*me*me + 2.*beamEnergy*me);
    bg = beamEnergy/sqrts; // beta gamma
    gam = sqrt(bg*bg+1.);
    beta = bg/gam;
  }

  double xTarg = 1.;  // -> should be from db
  const double yTarg = 1.; // -> should be from db
  const double zTarg = -1028; // was -1030 default but set at -1028 from the 2020 survey, new value??
  TVector3 rTarg(xTarg,yTarg,zTarg);
  const double zOff = 2508.31; //[mm] = 2550.51 - 230./2. + 6.5*X0, X0=11.2 mm. zClu is 2555 now and not 2550.51 which should be from the survey]. 
  TVector3 cogAtECal(-16.,1.,zOff);
  TVector3 boostMom(cogAtECal.X(),cogAtECal.Y(),cogAtECal.Z());
  //  TVector3 boostMom(-9.5,-2.5,zOff);
  boostMom -= rTarg;
  boostMom *= (beta/boostMom.Mag());

  // selects pairs of clusters compatible with the e+e- --> gamma-gamma/e+e- transition
  // returns the number of selected pairs
  // pair indices are in the vector of pairs fIndPair

  // kinematic conditions might be run-dependent

  const double energyMin = 90; //MeV
  const double energyMax = 190; //MeV
  const double radiusMin = 140; //mm
  const double radiusMax = 300; //mm
  
  const double timeSafeMin = -1E10;//-110; // ns should do a time-dependent study
  const double maxTimeDistance = 5; // ns, sigma = 1.6 ns
  const double minGGDistance = 60; // mm
  
  //  std::cout << "beam pars " << fTargetRecoBeam->getXCfit() << " " << fTargetRecoBeam->getYCfit() << std::endl; //HOW TO USE THEM??

  const double deCutCenterX = 0; // MeV
  const double deCutRadiusX = 15.; // MeV
  const double deCutCenterY = 0; // MeV
  const double deCutRadiusY = 15; // MeV

  const double sigmadphi = 0.1; // rad (possible double peak present)
  const double pi=3.14159265358979;


  TRecoVCluster* tempClu[2];
  TVector3 cluPos[2];
  TVector3 cluPosRel[2];
  double cluTime[2];
  double cluEnergy[2];

  // fill general occupancy plot of ECal

  for (int h1=0; h1< fECal_clEvent->GetNElements(); ++h1) {
    tempClu[0] = fECal_clEvent->Element((int)h1);
    fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_yvsx_EweightAll"), tempClu[0]->GetPosition().X(), tempClu[0]->GetPosition().Y(), tempClu[0]->GetEnergy());
    fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_yvsxAll"), tempClu[0]->GetPosition().X(), tempClu[0]->GetPosition().Y(), 1.);
    if (tempClu[0]->GetEnergy() < energyMin) continue;
    if (tempClu[0]->GetEnergy() > energyMax) continue;
    fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_yvsx_Eweight"), tempClu[0]->GetPosition().X(), tempClu[0]->GetPosition().Y(), tempClu[0]->GetEnergy());
    fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_yvsx"), tempClu[0]->GetPosition().X(), tempClu[0]->GetPosition().Y(), 1.);
  }


  // loop on cluster pairs

  for (int h1=0; h1< fECal_clEvent->GetNElements(); ++h1) {
    tempClu[0] = fECal_clEvent->Element((int)h1);

    cluEnergy[0] = tempClu[0]->GetEnergy();
    if (cluEnergy[0] < energyMin) continue; // require a minimum energy
    if (cluEnergy[0] > energyMax) continue;
    if (tempClu[0]->GetPosition().Perp() < radiusMin) continue;

    cluTime[0] = tempClu[0]->GetTime();
    if (cluTime[0] < timeSafeMin) continue; // require time in the safe region [TBchecked]

    cluPos[0].SetXYZ(tempClu[0]->GetPosition().X(),tempClu[0]->GetPosition().Y(),zOff);
    cluPosRel[0] = cluPos[0]-cogAtECal;

    int isPaired = -1; // look for a gamma-gamma event, with loose conditions

    for (int h2=h1+1; h2< fECal_clEvent->GetNElements(); ++h2) {
      tempClu[1] = fECal_clEvent->Element((int)h2);
      cluEnergy[1] = tempClu[1]->GetEnergy();
      cluTime[1] = tempClu[1]->GetTime();
      if (cluTime[1] < timeSafeMin) continue;
      cluPos[1].SetXYZ(tempClu[1]->GetPosition().X(),tempClu[1]->GetPosition().Y(),zOff);

      double dt = cluTime[0]-cluTime[1];
      double dr = (cluPos[0]-cluPos[1]).Mag();
      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_DrVsDtAll"), dt, dr, 1.);

      if (cluEnergy[1] < energyMin) continue; // require a minimum energy
      if (cluEnergy[1] > energyMax) continue;
      if (tempClu[1]->GetPosition().Perp() < radiusMin) continue;

      fhSvcVal->FillHisto2List("ECalSel","ECal_SC_DrVsDt", dt, dr, 1.);

      if (fabs(dt) < maxTimeDistance && dr > minGGDistance) {
	if (isPaired == -1) isPaired = h2;
	else { // more than 1 cluster found connected -> reject the event
	  isPaired = -1;
	  break;
	}
      }
    } // inner cluster loop
    
    if (isPaired != -1) { // fill plots if only one pair found
      cluEnergy[0] = tempClu[0]->GetEnergy();//*tempCorr;
      tempClu[1] = fECal_clEvent->Element((int)isPaired);
      cluEnergy[1] = tempClu[1]->GetEnergy();//*tempCorr;
      cluTime[1] = tempClu[1]->GetTime();
      cluPos[1].SetXYZ(tempClu[1]->GetPosition().X(),tempClu[1]->GetPosition().Y(),zOff); 
      cluPosRel[1] = cluPos[1]-cogAtECal;

      double pg[2];
      for (int i = 0; i<2; i++){
	TVector3 rPos = cluPos[i]-rTarg;
	//	double cluDist = (cluPos[i]-rTarg).Mag();
	//	double cosq = (cluPos[i].Z() - rTarg.Z())/cluDist; //will have to correct Z of each cluster for its average cluster depth
	double cosq = rPos.Dot(boostMom)/(rPos.Mag()*boostMom.Mag());
	pg[i] = 0.5*sqrts/sqrt(1.-cosq*cosq + gam*gam*cosq*cosq - 2.*bg*gam*cosq + bg*bg);
      }      
      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_DE1VsDE2"), cluEnergy[0]-pg[0], cluEnergy[1]-pg[1], 1.);
      

      double elli = 
	(cluEnergy[0]-pg[0]-deCutCenterX)*(cluEnergy[0]-pg[0]-deCutCenterX)/(deCutRadiusX*deCutRadiusX) +
	(cluEnergy[1]-pg[1]-deCutCenterY)*(cluEnergy[1]-pg[1]-deCutCenterY)/(deCutRadiusY*deCutRadiusY);

      // plot elli vs dphi to better identify the signal
      
      double dphi = TMath::ASin((cluPosRel[1].Y()*cluPosRel[0].X()-cluPosRel[1].X()*cluPosRel[0].Y())/(cluPosRel[0].Perp()*cluPosRel[1].Perp()));

      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_dphiElli"), elli, dphi, 1.);

      if (elli < 4 && TMath::Abs(dphi/sigmadphi) < 3) {
	
	fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_EVsT"), 0.5*(cluTime[0]+cluTime[1]), cluEnergy[0]+cluEnergy[1], 1.);

	TVector2 cog(
		     (cluEnergy[0]*cluPos[0]+cluEnergy[1]*cluPos[1]).X()/(cluEnergy[0]+cluEnergy[1]),
		     (cluEnergy[0]*cluPos[0]+cluEnergy[1]*cluPos[1]).Y()/(cluEnergy[0]+cluEnergy[1])
		     );

	fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_COGYX"), cog.X(),cog.Y(),1.);		     
	fhSvcVal->FillHisto2List("ECalSel","ECal_SC_YX",cluPos[0].X(),cluPos[0].Y(),1.);
	fhSvcVal->FillHisto2List("ECalSel","ECal_SC_YX",cluPos[1].X(),cluPos[1].Y(),1.);

	ECalSelEvent selev;
	selev.flagEv = ev_gg;
	selev.indexECal[0] = h1;
	selev.indexECal[1] = isPaired;
	selev.indexECal[2] = -1;
	selev.totalE = cluEnergy[0] + cluEnergy[1];
	selev.avgT = 0.5*(cluTime[0]+cluTime[1]);
	selev.cog.Set(cog.X(),cog.Y());
	fECalEvents.push_back(selev);
      } 

      if (elli < 2) { // looser cut because of the temperature effect
	for (int i=0; i<2; i++) {
	  fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_EExpVsE"), cluEnergy[i], pg[i],1.);
	  double phi = cluPos[i].Phi();
	  if (phi < 0) phi += 2*pi;
	  int q = (phi*3./pi+0.5);
	  if (q > 5) q = 0;
	  fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_EExpVsE_q%d",q), cluEnergy[i], pg[i],1.);
	}
      }
    } // isPaired
  } // cluster loop

  fhSvcVal->FillHistoList("ECalSel","NumberOfECalCluPairs",fECalEvents.size());
  return fECalEvents.size();
}






Bool_t ECalSel::InitHistos()
{
// x and y slices
  Double_t fXMin = -21.*(14+0.5);
  Double_t fXMax = 21.*(14+0.5);
  Double_t fXW = 21; // mm
  Int_t fNXBins = (fXMax-fXMin)/fXW;
  Double_t fYMin = -21.*(14+0.5);
  Double_t fYMax = 21.*(14+0.5);
  Double_t fYW = 21; // mm
  Int_t fNYBins = (fYMax-fYMin)/fYW;

  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_yvsx_Eweight", fNXBins*10,fXMin,fXMax, fNYBins*10,fYMin,fYMax);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_yvsx", fNXBins*10,fXMin,fXMax, fNYBins*10,fYMin,fYMax);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_yvsx_EweightAll", fNXBins*10,fXMin,fXMax, fNYBins*10,fYMin,fYMax);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_yvsxAll", fNXBins*10,fXMin,fXMax, fNYBins*10,fYMin,fYMax);
  
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_DrVsDtAll", 800, -400,400, 200, 0, 600.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_DrVsDt", 800, -400,400, 200, 0, 600.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_EVsT", 800, -400,400, 200, 0, 600.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_DE1VsDE2", 800, -400,400, 800, -400, 400.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_dphiElli", 200,0,20, 100, -0.5*TMath::Pi(), 0.5*TMath::Pi());
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_COGYX",100,-200,200,100,-200,200);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_YX", fNXBins*10,fXMin,fXMax, fNYBins*10,fYMin,fYMax);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_EExpVsE", 800, 0,400, 400, 0, 400.);
  fhSvcVal->BookHistoList("ECalSel","NumberOfECalCluPairs", 5,0.,5.);

  for (int q=0; q<6; q++){
    fhSvcVal->BookHisto2List("ECalSel",Form("ECal_SC_EExpVsE_q%d",q), 800, 0,400, 400, 0, 400.);
  }

  for (int q=0; q<15; q++){
    for (int aa = 0; aa < 4; aa++) { // SiPM
      fhSvcVal->BookHisto2List("ECalSel",Form("ECal_SC_ETagHitsRightCh_%d_SiPM_%d_dtvsdch",q,aa),61,-30.5,30.5,100,-50,50); 
      fhSvcVal->BookHisto2List("ECalSel",Form("ECal_SC_ETagHitsLeftCh_%d_SiPM_%d_dtvsdch",q,aa),61,-30.5,30.5,100,-50,50); 
      fhSvcVal->BookHisto2List("ECalSel",Form("ECal_SC_ETagHitsRightCh_%d_SiPM_%d_dtvsr",q,aa),29,0,609,200,-50,50); 
      fhSvcVal->BookHisto2List("ECalSel",Form("ECal_SC_ETagHitsLeftCh_%d_SiPM_%d_dtvsr",q,aa),29,0,609,200,-50,50); 
    }
    fhSvcVal->BookHisto2List("ECalSel",Form("ECal_SC_ETagHitsRightCh_%d_dtvsr",q),21,0,609,200,-50,50); 
    fhSvcVal->BookHisto2List("ECalSel",Form("ECal_SC_ETagHitsLeftCh_%d_dtvsr",q),21,0,609,200,-50,50); 
    fhSvcVal->BookHisto2List("ECalSel",Form("ECal_SC_RightCh_%d_dtvsr",q),21,0,609,100,-50,50); 
    fhSvcVal->BookHisto2List("ECalSel",Form("ECal_SC_LeftCh_%d_dtvsr",q),21,0,609,100,-50,50); 
  }

  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_ECal_LRvsYBars",15,-0.5,14.5,2,-0.5,1.5); 
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_ETagHitsMatchLeftTimevsN",20,0,20,20,15.,25.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_ETagHitsMatchRightTimevsN",20,0,20,20,15.,25.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_ETagHitsMatchRightVsLeft",20,0,20,20,0,20);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_ETagHitsMatches",100,0,100,100,0,100);

  return true;
}


Bool_t ECalSel::Finalize(){
  return true;
}
