// Written by E. Leonardi 
// Last modified by M. Raggi 2/08/2021
#include "ETagAnalysis.hh"

ETagAnalysis::ETagAnalysis(TString cfgFile, Int_t verbose)
{
  Neve=0;
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating ETagAnalysis\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  fHS = HistoSvc::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
}

ETagAnalysis::~ETagAnalysis(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t ETagAnalysis::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing ETagAnalysis\n");
  fEvent = event;
  InitHistos();
  return true;
}

Bool_t ETagAnalysis::InitHistos(){
  // ETagAnalysis directory will contain all histograms related to this analysis
  fHS->CreateList("ETagAnalysis");
  fHS->BookHistoList("ETagAnalysis","NPoT",600,-1000.,59000.);
  fHS->BookHistoList("ETagAnalysis","NPoTNoPhys",600,-1000.,59000.);
  fHS->BookHistoList("ETagAnalysis","NPoTPhys",600,-1000.,59000.);
  fHS->BookHistoList("ETagAnalysis","NPoTPhys5K",600,-1000.,59000.);

  fHS->BookHistoList("ETagAnalysis","XPos",200,-25.,25.);
  fHS->BookHistoList("ETagAnalysis","YPos",200,-25.,25.);

  fHS->BookHisto2List("ETagAnalysis","Occupancy2D", 2, 0., 2., 4*15, 0., 4*15.);

  fHS->BookHistoList("ETagAnalysis","LeftRightTimeDiff",200,-25.,25.);
  fHS->BookHistoList("ETagAnalysis","LeftRightTimeSum",200,-500.,0.);
  fHS->BookHistoList("ETagAnalysis","LeftRightTrueTimeDiff",200,-25.,25.);
  fHS->BookHistoList("ETagAnalysis","LeftRightTrueTimeSum",200,-500.,0.);
  //fHS->BookHistoList("ETagAnalysis","LeftRightTimeSumTrueTime",200,-500.,0.);
  fHS->BookHistoList("ETagAnalysis","LeftECalTimeDiff",200,-50.,50.);
  fHS->BookHistoList("ETagAnalysis","RightECalTimeDiff",200,-50.,50.);
  fHS->BookHistoList("ETagAnalysis","LeftECalTimeDiffECalEnergyCut",200,-50.,50.);
  fHS->BookHistoList("ETagAnalysis","RightECalTimeDiffECalEnergyCut",200,-50.,50.);
  fHS->BookHistoList("ETagAnalysis","LeftETagTime",200,-300.,0.);
  fHS->BookHistoList("ETagAnalysis","RightETagTime",200,-300.,0.);
	
  return true;
}

Bool_t ETagAnalysis::Process(){

	auto nHits = fEvent->ETagRecoEvent->GetNHits();
	for (auto i = 0; i < nHits; i++) { 
		auto ID = fEvent->ETagRecoEvent->Hit(i)->GetChannelId();
		fHS->FillHisto2List("ETagAnalysis", "Occupancy2D", PosMap[ID].first, PosMap[ID].second);
	}

  fNPoT = fEvent->TargetRecoBeam->getnPOT();
  fXPos = fEvent->TargetRecoBeam->getX();
  fYPos = fEvent->TargetRecoBeam->getY();
  TotPoT+=fNPoT;

  fHS->FillHistoList("ETagAnalysis","NPoT",fNPoT);

  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  if( !(trigMask & (1 << 0)) ){
    fHS->FillHistoList("ETagAnalysis","NPoTNoPhys",fNPoT);
    TotPoTNoPhys += fNPoT;
  }
  
  if( (trigMask & (1 << 0)) ) {

    std::vector<float> leftClusterTimes, rightClusterTimes;
    std::vector<float> leftClusterTrueTimes, rightClusterTrueTimes;
    std::vector<float> leftClusterChns, rightClusterChns;
	std::vector<float> leftClusterMatched, rightClusterMatched;
    Int_t NClustersETag =fEvent->ETagRecoCl->GetNElements();

    Int_t NClustersECAL = fEvent->ECalRecoCl->GetNElements();
    float totE = 0.;
    std::vector<float> ECALtimes;
    for (int icl = 0; icl < NClustersECAL; icl++) {
      ECALtimes.push_back(fEvent->ECalRecoCl->Element(icl)->GetTime());
      totE += fEvent->ECalRecoCl->Element(icl)->GetEnergy();
    }
	float x_ecal = -1000.0;
	float y_ecal = -1000.0;
	if (NClustersECAL == 1) {
		TVector3 pos1 =  fEvent->ECalRecoCl->Element(0)->GetPosition();
		x_ecal = pos1.X(); // mm
		y_ecal = pos1.Y(); // mm
	}
    for (int iecal = 0; iecal < NClustersECAL; iecal++) {
		TVector3 pos1 =  fEvent->ECalRecoCl->Element(iecal)->GetPosition();
		x_ecal = pos1.X(); // mm
		y_ecal = pos1.Y(); // mm
	    for(int icl = 0; icl < NClustersETag; icl++) {
		float y = -300 + 40/2 + 40 * abs(fEvent->ETagRecoCl->Element(icl)->GetChannelId());
		bool matched = false;
		if (abs(y - y_ecal) < 20) 
			matched = true;
	      if (fEvent->ETagRecoCl->Element(icl)->GetChannelId() < 0) {
		//if (NClustersECAL == 1 && abs(totE) > 250 && abs(totE) < 330) {
			float tLeft = fEvent->ETagRecoCl->Element(icl)->GetTime() - 20.0;
			float conversionFactor = pow(10, 9) / 1000; // m/s --> mm/ns
			float tTrueLeft = tLeft - (-300 - x_ecal) * 1.5 / (3 * pow(10, 8) / conversionFactor); // ns
			leftClusterTimes.push_back(tLeft);
			leftClusterTrueTimes.push_back(tTrueLeft);
			leftClusterMatched.push_back(matched);
			leftClusterChns.push_back(fEvent->ETagRecoCl->Element(icl)->GetChannelId());
		//}
	      }
	      else {
			float tRight = fEvent->ETagRecoCl->Element(icl)->GetTime() - 20.0;
			float conversionFactor = pow(10, 9) / 1000; // m/s --> mm/ns
			float tTrueRight = tRight - (300 - x_ecal) * 1.5 / (3 * pow(10, 8) / conversionFactor); // ns
			rightClusterTimes.push_back(tRight);
			rightClusterTrueTimes.push_back(tTrueRight);
			rightClusterMatched.push_back(matched);
			rightClusterChns.push_back(fEvent->ETagRecoCl->Element(icl)->GetChannelId());
	      }
	    }
	}
    for (auto i = 0; i < leftClusterTimes.size(); i++) {
	auto timeL = leftClusterTimes[i];
	fHS->FillHistoList("ETagAnalysis", "LeftETagTime", timeL);
      for (auto timeE : ECALtimes) {
			if (leftClusterMatched[i]) {
		      fHS->FillHistoList("ETagAnalysis", "LeftECalTimeDiff", timeL - timeE);
			      fHS->FillHistoList("ETagAnalysis", "LeftECalTimeDiffECalEnergyCut", timeL - timeE);
			}
      }
    }
    for (auto i = 0; i < rightClusterTimes.size(); i++) {
	auto timeR = rightClusterTimes[i];
	fHS->FillHistoList("ETagAnalysis", "RightETagTime", timeR);
      for (auto timeE : ECALtimes) {
        fHS->FillHistoList("ETagAnalysis", "RightECalTimeDiff", timeR - timeE);
          fHS->FillHistoList("ETagAnalysis", "RightECalTimeDiffECalEnergyCut", timeR - timeE);
      }
    }
    // if (NClusters == 2 && leftClusterTimes.size() == rightClusterTimes.size()) {
     for (auto i = 0; i < leftClusterTimes.size(); i++) {
	for (auto j = 0; j < rightClusterTimes.size(); j++) {
	     //if (NClusters == 2 && leftClusterTimes.size() == rightClusterTimes.size()) {
	     //if (NClustersECAL == 1) {
		if (abs(leftClusterChns[i]) < 50 && abs(rightClusterChns[j]) < 50)  {
			if (abs(leftClusterChns[i]) == abs(rightClusterChns[j])) {
				fHS->FillHistoList("ETagAnalysis", "LeftRightTimeDiff", leftClusterTimes[i] - rightClusterTimes[j]);
				fHS->FillHistoList("ETagAnalysis", "LeftRightTimeSum", leftClusterTimes[i] + rightClusterTimes[j]);
				if (leftClusterMatched[i]) {
					fHS->FillHistoList("ETagAnalysis", "LeftRightTrueTimeDiff", leftClusterTrueTimes[i] - rightClusterTrueTimes[j]);
					fHS->FillHistoList("ETagAnalysis", "LeftRightTrueTimeSum", leftClusterTrueTimes[i] + rightClusterTrueTimes[j]);
				}
				//fHS->FillHistoList("ETagAnalysis", "LeftRightTimeSumTrueTime", (leftClusterTimes[i] + rightClusterTimes[j] - 3.0)/2.0);
			}
		}
	     //}
        }
      }
    // }
    //for (auto time : leftClusterTimes) {
    //  std::cout << "left cluster time: " << time << std::endl;
    //}
    //for (auto time : rightClusterTimes) {
    //  std::cout << "right cluster time: " << time << std::endl;
    //}
    // for (size_t i = 0; i < leftClusters.size(); i++) {
    //   if (i == 1) break;
    //   // icl : leftClusters) {
    //   std::cout << "left cluster time: " << fEvent->ECalRecoCl->Element(leftClusters[i])->GetChannelId() << std::endl;
    // }
    // for (size_t i = 0; i < rightClusters.size(); i++) {
    //   if (i == 1) break;
    //   std::cout << "right cluster time: " << fEvent->ECalRecoCl->Element(rightClusters[i])->GetChannelId() << std::endl;
    // }


    fHS->FillHistoList("ETagAnalysis","NPoTPhys",fNPoT);
    TotPoTPhys += fNPoT;
    if(fNPoT>5E3) {
      fHS->FillHistoList("ETagAnalysis","NPoTPhys5K",fNPoT);
      TotPoTOver5K += fNPoT;
      fHS->FillHistoList("ETagAnalysis","XPos",fXPos);
      fHS->FillHistoList("ETagAnalysis","YPos",fYPos);
    }
    if(fNPoT>30E3) TotPoTOver30K += fNPoT;
  }  
//  vNPoT.push_back(fNPoT);
//  vNEvt.push_back(Neve);
  Neve++;
  return true;
}

Bool_t ETagAnalysis::Finalize()
{
 // TGraph* nPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vNPoT[0]);
 // fHS->SaveTGraphList("MyHistos","NPotVsTime",nPotVsTime);
  std::cout<<"TotPot        = "<<TotPoT<<std::endl;
  std::cout<<"TotPotPhys    = "<<TotPoTPhys<<std::endl;
  std::cout<<"TotPotNoPhys  = "<<TotPoTNoPhys<<std::endl;
  std::cout<<"TotPot Over30 = "<<TotPoTOver30K<<std::endl;
  std::cout<<"TotPot Over5  = "<<TotPoTOver5K<<std::endl;

  if (fVerbose) printf("---> Finalizing ETagAnalysis\n");
  return true;
}
