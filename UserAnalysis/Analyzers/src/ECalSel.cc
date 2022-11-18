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
ECalSel* ECalSel::GetInstance(){
  if ( fInstance == 0 ) { fInstance = new ECalSel(); }
  return fInstance;
}

ECalSel::ECalSel(){
  fRecoEvent   = NULL;
  fECal_hitEvent=0;  
  fVerbose        = 0;
  fECalEvents.clear();
}

ECalSel::~ECalSel(){}

Bool_t ECalSel::Init(PadmeAnalysisEvent* event){
  fRecoEvent = event->RecoEvent;
  fECal_hitEvent = event->ECalRecoEvent;
  fECal_clEvent = event->ECalRecoCl; 
  fGeneralInfo = GeneralInfo::GetInstance();
  fhSvcVal =  HistoSvc::GetInstance(); 

  // binning of theta vs phi to retrieve independently the beam direction

  fNThetaBins = 40;//20;
  //  fThetaWid = 0.003/fNThetaBins; // rad
  fThetaWid = 0.008/fNThetaBins; // rad
  fNPhiDirBins = 40;//was 25 50;
  //  const double pi = 3.1415927;
  fPhiDirWid = 2.*TMath::Pi()/fNPhiDirBins; // rad

  InitHistos();

  return true;
}


Bool_t ECalSel::Process(){

  fECalEvents.clear();
  TwoClusSel();

  return true;
}



Int_t ECalSel::TwoClusSel(){

  //  int runID = fRecoEvent->GetRunNumber();

  // selects pairs of clusters compatible with the e+e- --> gamma-gamma/e+e- transition
  // returns the number of selected pairs
  // pair indices are in the vector of pairs fIndPair

  TTimeStamp evt = fRecoEvent->GetEventTime();
  long long int eventTime = static_cast<long long int>(evt.GetSec());
  long long int  deventTime = eventTime - fGeneralInfo->GetStartTime();

  // kinematic conditions might be run-dependent

  const double energyMin = 90; //MeV
  const double energyMax = 190; //MeV
  const double radiusMin = 140; //mm
  const double radiusMax = 300; //mm
  
  const double timeSafeMin = -1E10;//-110; // ns should do a time-dependent study
  const double maxTimeDistance = 5; // ns, sigma = 1.6 ns
  const double minGGDistance = 60; // mm
  
  const double deCutCenterX = 0; // MeV
  const double deCutRadiusX = 15.; // MeV
  const double deCutCenterY = 0; // MeV
  const double deCutRadiusY = 15; // MeV
  const double maxRCOG = 150.; // mm
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

    cluPos[0].SetXYZ(tempClu[0]->GetPosition().X(),tempClu[0]->GetPosition().Y(),fGeneralInfo->GetCOG().Z());
    cluPosRel[0] = cluPos[0]-fGeneralInfo->GetCOG();

    int isPaired = -1; // look for a gamma-gamma event, with loose conditions

    for (int h2=h1+1; h2< fECal_clEvent->GetNElements(); ++h2) {
      tempClu[1] = fECal_clEvent->Element((int)h2);
      cluEnergy[1] = tempClu[1]->GetEnergy();
      cluTime[1] = tempClu[1]->GetTime();
      if (cluTime[1] < timeSafeMin) continue;
      cluPos[1].SetXYZ(tempClu[1]->GetPosition().X(),tempClu[1]->GetPosition().Y(),fGeneralInfo->GetCOG().Z());

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
      cluPos[1].SetXYZ(tempClu[1]->GetPosition().X(),tempClu[1]->GetPosition().Y(),fGeneralInfo->GetCOG().Z()); 
      cluPosRel[1] = cluPos[1] - fGeneralInfo->GetCOG();
      
      double pg[2];
      for (int i = 0; i<2; i++){
	TVector3 rPos = cluPos[i]-fGeneralInfo->GetTargetPos();
	double cosq = rPos.Dot(fGeneralInfo->GetBoost())/(rPos.Mag()*fGeneralInfo->GetBoost().Mag());
	pg[i] = 0.5*fGeneralInfo->GetSqrts()/sqrt(1.-cosq*cosq + 
						  pow(fGeneralInfo->GetGam()*cosq,2) - 
						  2.*fGeneralInfo->GetBG()*fGeneralInfo->GetGam()*cosq + 
						  pow(fGeneralInfo->GetBG(),2) );
      }      
      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_DE1VsDE2"), cluEnergy[0]-pg[0], cluEnergy[1]-pg[1], 1.);
      

      double elli = 
	(cluEnergy[0]-pg[0]-deCutCenterX)*(cluEnergy[0]-pg[0]-deCutCenterX)/(deCutRadiusX*deCutRadiusX) +
	(cluEnergy[1]-pg[1]-deCutCenterY)*(cluEnergy[1]-pg[1]-deCutCenterY)/(deCutRadiusY*deCutRadiusY);

      // plot elli vs dphi to better identify the signal
      
      double dphi = TMath::ASin((cluPosRel[1].Y()*cluPosRel[0].X()-cluPosRel[1].X()*cluPosRel[0].Y())/(cluPosRel[0].Perp()*cluPosRel[1].Perp()));

      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_dphiElli"), elli, dphi, 1.);


      if (elli < 4 && TMath::Abs(dphi/sigmadphi) < 3) {
	
	TVector2 cog(
		     (cluEnergy[0]*cluPos[0]+cluEnergy[1]*cluPos[1]).X()/(cluEnergy[0]+cluEnergy[1]),
		     (cluEnergy[0]*cluPos[0]+cluEnergy[1]*cluPos[1]).Y()/(cluEnergy[0]+cluEnergy[1])
		     );

	fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_EVsT"), 0.5*(cluTime[0]+cluTime[1]), cluEnergy[0]+cluEnergy[1], 1.);
	fhSvcVal->FillHisto2List("ECalSel","ECal_SC_COGYX", cog.X(),cog.Y(),1.);		     
	fhSvcVal->FillHisto2List("ECalSel","ECalX_vs_Time", deventTime, cog.X(),1.);		     
	fhSvcVal->FillHisto2List("ECalSel","ECalY_vs_Time", deventTime, cog.Y(),1.);		     
	//	std::cout << "deventTime " << deventTime << " cog = " << cog.X() << " " << cog.Y() << std::endl;

	fhSvcVal->FillHisto2List("ECalSel","ECal_SC_YX",cluPos[0].X(),cluPos[0].Y(),1.);
	fhSvcVal->FillHisto2List("ECalSel","ECal_SC_YX",cluPos[1].X(),cluPos[1].Y(),1.);

	if (cog.Mod() < maxRCOG){
	  ECalSelEvent selev;
	  selev.flagEv = ev_gg;
	  selev.indexECal[0] = h1;
	  selev.indexECal[1] = isPaired;
	  selev.indexECal[2] = -1;
	  selev.totalE = cluEnergy[0] + cluEnergy[1];
	  selev.avgT = 0.5*(cluTime[0]+cluTime[1]);
	  selev.cog.Set(cog.X(),cog.Y());
	  fECalEvents.push_back(selev);

	  TLorentzVector photonMom[2];
	  for (int j=0; j<2; j++) {
	    TVector3 rPos = cluPos[j]-fGeneralInfo->GetTargetPos();
	    rPos *= (cluEnergy[j]/rPos.Mag());
	    photonMom[j].SetXYZT(rPos.X(),rPos.Y(),rPos.Z(),cluEnergy[j]);
	  }
	  
    // evaluate best boost direction

	  for (int ith = 0; ith<fNThetaBins; ith++){ // loop on theta
	    double thetaRot = fThetaWid*(ith+0.5);
	    for (int iph = 0; iph<fNPhiDirBins; iph++){ // loop on phi
	      double phiRot = fPhiDirWid*(iph+0.5);
	      TVector3 trialBoost(fGeneralInfo->GetBoost().Mag()*TMath::Sin(thetaRot)*TMath::Cos(phiRot),
				  fGeneralInfo->GetBoost().Mag()*TMath::Sin(thetaRot)*TMath::Sin(phiRot),
				  fGeneralInfo->GetBoost().Mag()*TMath::Cos(thetaRot)); 
	      trialBoost *= -1;
	      TLorentzVector pgstar[2];
	      for (int j=0; j<2; j++) {
		pgstar[j].SetXYZT(photonMom[j].X(),photonMom[j].Y(),photonMom[j].Z(),photonMom[j].E());
		pgstar[j].Boost(trialBoost);
	      }

	      double cosRel = pgstar[0].Vect().Dot(pgstar[1].Vect())/(pgstar[0].E()*pgstar[1].E());	

	      fhSvcVal->FillHisto2List("ECalSel","DPhiVsRotVsRunCos",thetaRot,phiRot,1.+cosRel); 
	      if (cosRel < -0.998) {
		fhSvcVal->FillHisto2List("ECalSel","DPhiVsRotVsRun",thetaRot,phiRot,1.); 
	      }
	    }
	  }

	} 
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
  fhSvcVal->CreateList("ECalSel");

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
  fhSvcVal->BookHisto2List("ECalSel","ECalX_vs_Time",48,0,86400.,100,-200,200); //30-minute binning
  fhSvcVal->BookHisto2List("ECalSel","ECalY_vs_Time",48,0,86400.,100,-200,200); //30-minute binning
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_YX", fNXBins*10,fXMin,fXMax, fNYBins*10,fYMin,fYMax);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_EExpVsE", 800, 0,400, 400, 0, 400.);
  fhSvcVal->BookHistoList("ECalSel","NumberOfECalCluPairs", 5,0.,5.);

  fhSvcVal->BookHisto2List("ECalSel","DPhiVsRotVsRunCos",fNThetaBins,0,fNThetaBins*fThetaWid,fNPhiDirBins,0,2*TMath::Pi()); //to be done run-wise: first read runID, then book
  fhSvcVal->BookHisto2List("ECalSel","DPhiVsRotVsRun",fNThetaBins,0,fNThetaBins*fThetaWid,fNPhiDirBins,0,2*TMath::Pi()); //to be done run-wise: first read runID, then book


  for (int q=0; q<6; q++){
    fhSvcVal->BookHisto2List("ECalSel",Form("ECal_SC_EExpVsE_q%d",q), 800, 0,400, 400, 0, 400.);
  }

  return true;
}


Bool_t ECalSel::Finalize(){
  return true;
}
