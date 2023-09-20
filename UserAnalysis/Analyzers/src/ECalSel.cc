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

  // single cluster conditions
  fTimeSafeMin = -1E10;// ns 
  fMaxTimeDistance = 5; // ns, sigma = 1.6 ns
  fMinGGDistance = 60; // mm
  // kinematic conditions are run-dependent. 
  fSafeEnergyFactor = 0.9; // Safety factor used for the energy min and max cuts
  fSafeSpaceMargin = 21; // mm, safety margin used for the radius min cut    
}

ECalSel::~ECalSel(){}

Bool_t ECalSel::Init(PadmeAnalysisEvent* event){
  fRecoEvent = event->RecoEvent;
  fECal_hitEvent = event->ECalRecoEvent;
  fECal_clEvent = event->ECalRecoCl; 
  fGeneralInfo = GeneralInfo::GetInstance();
  fhSvcVal =  HistoSvc::GetInstance(); 

  // binning of theta vs phi to retrieve independently the beam direction

  fNThetaBins = 70;//20;
  //  fThetaWid = 0.003/fNThetaBins; // rad
  fThetaWid = 0.010/fNThetaBins; // rad
  fNPhiDirBins = 50;//was 25 50;
  //  const double pi = 3.1415927;
  fPhiDirWid = 2.*TMath::Pi()/fNPhiDirBins; // rad

// x and y slices
  fXMin = -21.*(14+0.5);
  fXMax = 21.*(14+0.5);
  fXW = 21; // mm
  fNXBins = (fXMax-fXMin)/fXW;
  fYMin = -21.*(14+0.5);
  fYMax = 21.*(14+0.5);
  fYW = 21; // mm
  fNYBins = (fYMax-fYMin)/fYW;

  InitHistos();

  return true;
}


Bool_t ECalSel::Process(){

  fECalEvents.clear();
  TwoClusSel();
  OneClusSel();

  return true;
}


Int_t ECalSel::OneClusSel(){

  //  int runID = fRecoEvent->GetRunNumber();

  // selects isolated single clusters compatible with the e+e- --> e+e- or gamma-gamma, to be used for tag & probe
  // returns the number of selected clusters
  // indices are in the vector of pairs fIndPair

  TTimeStamp evt = fRecoEvent->GetEventTime();
  long long int eventTime = static_cast<long long int>(evt.GetSec());
  long long int  deventTime = eventTime - fGeneralInfo->GetPeriodStartTime(); // using the period start time and not the run start time <-- to be checked

  const double energyMax = 200; //MeV
  const double radiusMin = 140; //mm
  //  const double radiusMax = 300; //mm
  
  const double fTimeSafeMin = -1E10;//-110; // ns should do a time-dependent study
  const double fMaxTimeDistance = 5; // ns, sigma = 1.6 ns
  const double minDistance = 125; // mm
  const double maxDistance = 145; // mm
  


  TRecoVCluster* tempClu[2];
  TVector3 cluPos[2];
  double cluTime[2];
  double cluEnergy[2];

  // loop on clusters

  int nOld = fECalEvents.size();

  for (int h1=0; h1< fECal_clEvent->GetNElements(); ++h1) {
    tempClu[0] = fECal_clEvent->Element((int)h1);
    cluEnergy[0] = tempClu[0]->GetEnergy();
    if (cluEnergy[0] < energyMax) continue; // cluster should be softer than the 2gamma cluster max energy

    cluPos[0].SetXYZ(tempClu[0]->GetPosition().X(),tempClu[0]->GetPosition().Y(),fGeneralInfo->GetCOG().Z());
    if (cluPos[0].Perp() > radiusMin) continue; // cluster should be above the minimum radius of the 2gamma cluster pair

    cluTime[0] = tempClu[0]->GetTime();
    if (cluTime[0] < fTimeSafeMin) continue; // require time in the safe region [TBchecked]

    fhSvcVal->FillHistoList("ECalSel",Form("ECal_SingleClu_dE"), fGeneralInfo->GetBeamMomentum()-cluEnergy[0], 1.);

    int isPaired = -1; // look for a cluster from the same interaction
    int npaired = 0;
    for (int h2=0; h2< fECal_clEvent->GetNElements(); ++h2) {
      if (h1 == h2) continue;
      tempClu[1] = fECal_clEvent->Element((int)h2);
      cluEnergy[1] = tempClu[1]->GetEnergy();
      cluTime[1] = tempClu[1]->GetTime();
      if (cluTime[1] < fTimeSafeMin) continue;
      cluPos[1].SetXYZ(tempClu[1]->GetPosition().X(),tempClu[1]->GetPosition().Y(),fGeneralInfo->GetCOG().Z());
      //      if (cluPos[1].Perp() > radiusMin) continue;

      double dt = cluTime[0]-cluTime[1];
      double dr = (cluPos[0]-cluPos[1]).Mag();
      TVector3 cog = (cluEnergy[0]/(cluEnergy[0]+cluEnergy[1]))*cluPos[0]+(cluEnergy[1]/(cluEnergy[0]+cluEnergy[1]))*cluPos[1];// cog of the two clusters
      cog -= fGeneralInfo->GetCOG();
      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SingleClu_DrVsDtAll"), dt, dr, 1.);
      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SingleClu_E1F_vs_E2F"), (cluEnergy[0]+cluEnergy[1])/fGeneralInfo->GetBeamMomentum(), cog.Perp(), 1.);
      if (fabs(dt) < fMaxTimeDistance && dr > minDistance && dr < maxDistance) {
	isPaired = h2;
	fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SingleClu_E1F_vs_E2FCut"), (cluEnergy[0]+cluEnergy[1])/fGeneralInfo->GetBeamMomentum(), cog.Perp(), 1.);

	npaired++;
      }
    } // inner cluster loop    

    if (npaired == 1){
      tempClu[1] = fECal_clEvent->Element((int)isPaired);
      ECalSelEvent selev;
      selev.flagEv = ev_single;
      selev.indexECal[0] = h1;
      selev.indexECal[1] = isPaired;
      selev.indexECal[2] = -1;
      selev.totalE = cluEnergy[0] + tempClu[1]->GetEnergy();
      selev.avgT = cluTime[0];
      selev.cog.Set(cluPos[0].X(),cluPos[0].Y());
      selev.indexETagAss[0] = -1;
      selev.indexETagAss[1] = -1;
      selev.indexETagAss[2] = -1;
      fECalEvents.push_back(selev);
    }

  } // cluster loop



  fhSvcVal->FillHistoList("ECalSel","NumberOfSingleClus",fECalEvents.size()-nOld);
  return fECalEvents.size()-nOld;
}




Int_t ECalSel::TwoClusSel(){
  const double deCutCenterX = 0; // MeV
  const double deCutRadiusX = 15.; // MeV
  const double deCutCenterY = 0; // MeV
  const double deCutRadiusY = 15; // MeV
  const double maxRCOG = 150.; // mm
  const double sigmadphi = 0.1; // rad (possible double peak present)

  int nOld = fECalEvents.size();

  // selects pairs of clusters compatible with the e+e- --> gamma-gamma/e+e- transition
  // returns the number of selected pairs
  // pair indices are in the vector of pairs fIndPair

  TTimeStamp evt = fRecoEvent->GetEventTime();
  long long int eventTime = static_cast<long long int>(evt.GetSec());
  long long int  deventTime = eventTime - fGeneralInfo->GetPeriodStartTime();// using the period start time and not the run start time <-- to be checked



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
    cluPos[0].SetXYZ(tempClu[0]->GetPosition().X(),tempClu[0]->GetPosition().Y(),fGeneralInfo->GetCOG().Z());
    cluPosRel[0] = cluPos[0]-fGeneralInfo->GetCOG();

    if (tempClu[0]->GetEnergy() > fGeneralInfo->GetEnergyMin()*fSafeEnergyFactor && tempClu[0]->GetEnergy() < fGeneralInfo->GetEnergyMax()/fSafeEnergyFactor) {
      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_yvsx_Eweight"), tempClu[0]->GetPosition().X(), tempClu[0]->GetPosition().Y(), tempClu[0]->GetEnergy());
      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_yvsx"), tempClu[0]->GetPosition().X(), tempClu[0]->GetPosition().Y(), 1.);
    }    
  }


  // loop on cluster pairs

  for (int h1=0; h1< fECal_clEvent->GetNElements(); ++h1) {
    tempClu[0] = fECal_clEvent->Element((int)h1);

    cluTime[0] = tempClu[0]->GetTime();
    if (cluTime[0] < fTimeSafeMin) continue; // require time in the safe region [TBchecked]
    cluEnergy[0] = tempClu[0]->GetEnergy();
    if (cluEnergy[0] < fGeneralInfo->GetEnergyMin()*fSafeEnergyFactor) continue; // require a minimum energy
    if (cluEnergy[0] > fGeneralInfo->GetEnergyMax()/fSafeEnergyFactor) continue; // require a maximum energy

    cluPos[0].SetXYZ(tempClu[0]->GetPosition().X(),tempClu[0]->GetPosition().Y(),fGeneralInfo->GetCOG().Z());
    if (cluPos[0].Perp() < fGeneralInfo->GetRadiusMin() - fSafeSpaceMargin) continue; // require a minimum radius at the ECAL
    if (cluPos[0].Perp() > fGeneralInfo->GetRadiusMax()) continue; // require a maximum radius at the ECAL, should we use a margin here as well?


    int isPaired = -1; // look for a gamma-gamma event, with loose conditions

    for (int h2=h1+1; h2< fECal_clEvent->GetNElements(); ++h2) {
      tempClu[1] = fECal_clEvent->Element((int)h2);

      cluTime[1] = tempClu[1]->GetTime();
      if (cluTime[1] < fTimeSafeMin) continue;
      cluEnergy[1] = tempClu[1]->GetEnergy();
      cluPos[1].SetXYZ(tempClu[1]->GetPosition().X(),tempClu[1]->GetPosition().Y(),fGeneralInfo->GetCOG().Z());

      double dt = cluTime[0]-cluTime[1];
      double dr = (cluPos[0]-cluPos[1]).Mag();

      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_DrVsDtAll"), dt, dr, 1.);

      if (cluEnergy[1] < fGeneralInfo->GetEnergyMin()*fSafeEnergyFactor) continue; // require a minimum energy
      if (cluEnergy[1] > fGeneralInfo->GetEnergyMax()/fSafeEnergyFactor) continue; // require a maximum energy

      cluPos[1].SetXYZ(tempClu[1]->GetPosition().X(),tempClu[1]->GetPosition().Y(),fGeneralInfo->GetCOG().Z());
      if (cluPos[1].Perp() < fGeneralInfo->GetRadiusMin() - fSafeSpaceMargin) continue; // require a minimum radius at the ECAL
      if (cluPos[1].Perp() > fGeneralInfo->GetRadiusMax()) continue; // require a maximum radius at the ECAL, should we use a margin here as well?


      fhSvcVal->FillHisto2List("ECalSel","ECal_SC_DrVsDt", dt, dr, 1.);

      if (fabs(dt) < fMaxTimeDistance && dr > fMinGGDistance) {
	if (isPaired == -1) isPaired = h2;
	else { // more than 1 cluster found connected -> reject the event
	  isPaired = -1;
	  break;
	}
      }
    } // inner cluster loop
    
    if (isPaired != -1) { // fill plots if only one pair found
      cluEnergy[0] = tempClu[0]->GetEnergy();
      cluPosRel[0] = cluPos[0]-fGeneralInfo->GetCOG();

      tempClu[1] = fECal_clEvent->Element((int)isPaired);
      cluEnergy[1] = tempClu[1]->GetEnergy();
      cluTime[1] = tempClu[1]->GetTime();
      cluPos[1].SetXYZ(tempClu[1]->GetPosition().X(),tempClu[1]->GetPosition().Y(),fGeneralInfo->GetCOG().Z()); 
      cluPosRel[1] = cluPos[1] - fGeneralInfo->GetCOG();
      
      // evaluate expected energies in the lab frame
      double pg[2], cosq[2];
      TVector3 rPosCrossBoost[2];
      TLorentzVector labMomenta[2], labMomentaCM[2];
      for (int i = 0; i<2; i++){
	TVector3 rPos = cluPos[i]-fGeneralInfo->GetTargetPos();
	rPos *= (cluEnergy[i]/rPos.Mag());

	// laboratory and cm momenta
	labMomenta[i].SetVectM(rPos,0.); // define a photon-like tlorentzVector
	labMomentaCM[i].SetVectM(labMomenta[i].Vect(),0);
	labMomentaCM[i].Boost(-fGeneralInfo->GetBoost());

	// cosine and sine with respect to the boost
	cosq[i] = rPos.Dot(fGeneralInfo->GetBoost())/(rPos.Mag()*fGeneralInfo->GetBoost().Mag());
	rPosCrossBoost[i] = rPos.Cross(fGeneralInfo->GetBoost());
	rPosCrossBoost[i] *= 1./(rPos.Mag()*fGeneralInfo->GetBoost().Mag());

	// energy expected in the lab
	pg[i] = 0.5*fGeneralInfo->GetSqrts()/sqrt(1.-cosq[i]*cosq[i] + 
						  pow(fGeneralInfo->GetGam()*cosq[i],2) - 
						  2.*fGeneralInfo->GetBG()*fGeneralInfo->GetGam()*cosq[i] + 
						  pow(fGeneralInfo->GetBG(),2) );
      }      
      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_DE1VsDE2"), cluEnergy[0]-pg[0], cluEnergy[1]-pg[1], 1.);
      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_DTHEVsDPHI"), 
			       labMomentaCM[0].Vect().Theta()+labMomentaCM[1].Vect().Theta(), 
			       labMomentaCM[0].Vect().Phi()+labMomentaCM[1].Vect().Phi(), 
			       1.);
      

      
      // define an elliptical signal region
      double elli = 
	(cluEnergy[0]-pg[0]-deCutCenterX)*(cluEnergy[0]-pg[0]-deCutCenterX)/(deCutRadiusX*deCutRadiusX) +
	(cluEnergy[1]-pg[1]-deCutCenterY)*(cluEnergy[1]-pg[1]-deCutCenterY)/(deCutRadiusY*deCutRadiusY);

      // plot  vs dphi to better identify the signal
      
      //      double dphi = TMath::ASin((cluPosRel[1].Y()*cluPosRel[0].X()-cluPosRel[1].X()*cluPosRel[0].Y())/(cluPosRel[0].Perp()*cluPosRel[1].Perp()));
      double signPlane = -1;
      if (rPosCrossBoost[0].Dot(rPosCrossBoost[1]) > 0) signPlane = 1;
      double dphi = (
		     (fGeneralInfo->GetGam()*cosq[0]-fGeneralInfo->GetBG())*
		     (fGeneralInfo->GetGam()*cosq[1]-fGeneralInfo->GetBG()) +
		     signPlane*sqrt(1.-cosq[0]*cosq[0])*sqrt(1-cosq[1]*cosq[1]))/
	((fGeneralInfo->GetGam()-fGeneralInfo->GetBG()*cosq[0])*(fGeneralInfo->GetGam()-fGeneralInfo->GetBG()*cosq[1]));
      if (fabs(dphi) > 1) {
	std::cout << "ECalSel >> Inconsistent two-body kinematics " << dphi << " " << cosq[0] << " " << cosq[1] << " " << signPlane << std::endl;
	break;
      }
      dphi = TMath::ACos(dphi);

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
	  selev.flagAlgo = estars;
	  selev.indexECal[0] = h1;
	  selev.indexECal[1] = isPaired;
	  selev.indexECal[2] = -1;
	  selev.totalE = cluEnergy[0] + cluEnergy[1];
	  selev.avgT = 0.5*(cluTime[0]+cluTime[1]);
	  selev.cog.Set(cog.X(),cog.Y());
	  selev.indexETagAss[0] = -1;
	  selev.indexETagAss[1] = -1;
	  selev.indexETagAss[2] = -1;
	  fECalEvents.push_back(selev);

	  TLorentzVector photonMom[2];
	  for (int j=0; j<2; j++) {
	    TVector3 rPos = cluPos[j]-fGeneralInfo->GetTargetPos();
	    rPos *= (cluEnergy[j]/rPos.Mag());
	    photonMom[j].SetXYZT(rPos.X(),rPos.Y(),rPos.Z(),cluEnergy[j]);
	  }

    // Fill plots to check energy and radius range

	  for (int kk = 0; kk < 2; kk++){
	    fhSvcVal->FillHisto2List("ECalSel",Form("ECal_E_Radius"), cluPosRel[kk].Perp(), tempClu[kk]->GetEnergy());
	    fhSvcVal->FillHistoList("ECalSel",Form("ECal_RadiusECut"),
				    (cluPosRel[kk].Perp()-fGeneralInfo->GetRadiusMin())/(fGeneralInfo->GetRadiusMax()-fGeneralInfo->GetRadiusMin()));
	    fhSvcVal->FillHistoList("ECalSel",Form("ECal_ERCut"),
				    (tempClu[kk]->GetEnergy()-fGeneralInfo->GetEnergyMin())/(fGeneralInfo->GetEnergyMax()-fGeneralInfo->GetEnergyMin()));

	    int xbin = (cluPos[kk].X() - fXMin)/fXW;
	    int ybin = (cluPos[kk].Y() - fYMin)/fYW;
	    if (xbin < 0 || xbin >= fNXBins){
	      std::cout << "X value out of range " << cluPos[kk].X() << " " << cluPos[kk].Y() << std::endl;
	      break;
	    }
	    if (ybin < 0 || ybin >= fNYBins){
	      std::cout << "Y value out of range " << cluPos[kk].X() << " " << cluPos[kk].Y() << std::endl;
	      continue;
	    }
	    
	    TString hname = Form("EExpVsEX_%d_Y_%d",xbin+1,ybin+1);
	    fhSvcVal->FillHisto2List("ECalScale",hname.Data(),pg[kk],cluEnergy[kk]/pg[kk]-1,1.); 
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
    } // isPaired
  } // cluster loop

  fhSvcVal->FillHistoList("ECalSel","NumberOfECalCluPairs",fECalEvents.size()-nOld);
  return fECalEvents.size()-nOld;
}






Bool_t ECalSel::InitHistos()
{
  fhSvcVal->CreateList("ECalSel");

  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_yvsx_Eweight", fNXBins*10,fXMin,fXMax, fNYBins*10,fYMin,fYMax);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_yvsx", fNXBins*10,fXMin,fXMax, fNYBins*10,fYMin,fYMax);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_yvsx_EweightAll", fNXBins*10,fXMin,fXMax, fNYBins*10,fYMin,fYMax);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_yvsxAll", fNXBins*10,fXMin,fXMax, fNYBins*10,fYMin,fYMax);

  fhSvcVal->BookHisto2List("ECalSel","ECal_E_Radius", 100,0.,TMath::Sqrt(fXMax*fXMax+fYMax*fYMax),100,0,500.);
  fhSvcVal->BookHistoList("ECalSel","ECal_RadiusECut", 100,-1.5,1.5);
  fhSvcVal->BookHistoList("ECalSel","ECal_ERCut", 100,-1.5,1.5);
  
  fhSvcVal->BookHistoList("ECalSel","ECal_SingleClu_dE", 200,-100.,100.);
  double LMax = TMath::Sqrt((fXMax-fXMin)*(fXMax-fXMin)+(fYMax-fYMin)*(fYMax-fYMin));
  fhSvcVal->BookHisto2List("ECalSel","ECal_SingleClu_E1F_vs_E2F"   , 400,0.,4.,100,0.,LMax);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SingleClu_E1F_vs_E2FCut", 400,0.,4.,100,0.,LMax);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SingleClu_DrVsDtAll", 800, -400,400, 200, 0, 600.);
  fhSvcVal->BookHistoList("ECalSel","NumberOfSingleClus", 10,0.,10.);

  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_DrVsDtAll", 800, -400,400, 200, 0, 600.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_DrVsDt", 800, -400,400, 200, 0, 600.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_EVsT", 800, -400,400, 200, 0, 600.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_DE1VsDE2", 800, -400,400, 800, -400, 400.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_DTHEVsDPHI", 800, -4*TMath::Pi(),4*TMath::Pi(), 800, -4*TMath::Pi(), -4*TMath::Pi());

  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_dphiElli", 200,0,20, 100, -0.5*TMath::Pi(), 0.5*TMath::Pi());
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_COGYX",100,-200,200,100,-200,200);
  fhSvcVal->BookHisto2List("ECalSel","ECalX_vs_Time",48,0,86400.,100,-200,200); //30-minute binning
  fhSvcVal->BookHisto2List("ECalSel","ECalY_vs_Time",48,0,86400.,100,-200,200); //30-minute binning
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_YX", fNXBins*10,fXMin,fXMax, fNYBins*10,fYMin,fYMax);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_EExpVsE", 800, 0,400, 400, 0, 400.);
  fhSvcVal->BookHistoList("ECalSel","NumberOfECalCluPairs", 5,0.,5.);

  fhSvcVal->BookHisto2List("ECalSel","DPhiVsRotVsRunCos",fNThetaBins,0,fNThetaBins*fThetaWid,fNPhiDirBins,0,2*TMath::Pi()); //to be done run-wise: first read runID, then book
  fhSvcVal->BookHisto2List("ECalSel","DPhiVsRotVsRun",fNThetaBins,0,fNThetaBins*fThetaWid,fNPhiDirBins,0,2*TMath::Pi()); //to be done run-wise: first read runID, then book


  const float fEExpMin = 90;
  const float fEExpMax = 410;
  const int fNEExpBins = 80;
  fhSvcVal->CreateList("ECalScale");

  for (int it = 0; it<fNXBins; it++){
    double xmin = fXMin + fXW*it;
    double xmax = fXMin + fXW*(it+1);

    for (int iq = 0; iq < fNYBins; iq++){
      double ymin = fYMin + fYW*iq;
      double ymax = fYMin + fYW*(iq+1);

      TString hname = Form("EExpVsEX_%d_Y_%d",it+1,iq+1);
      fhSvcVal->BookHisto2List("ECalScale",hname.Data(),fNEExpBins,fEExpMin,fEExpMax,100,-1,1);
//      fhSvcVal->GetHisto2List("ECalScale",hname.Data())->SetNameTitle
//	(Form("EExp vs E Xrange = (%f4.2,%f4.2)mm YRange = (%f4.2,%f4.2)",(float)xmin,(float)xmax,(float)ymin,(float)ymax));
    }
  }

  return true;
}


Bool_t ECalSel::Finalize(){
  return true;
}
