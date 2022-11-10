#include "ETagAn.hh"

#include "TRecoEvent.hh"
#include "TRecoVObject.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "HistoSvc.hh"
#include "TVector3.h"
#include "TProfile.h"
#include "TString.h"
#include "TLorentzVector.h"
#include <iostream>

ETagAn* ETagAn::fInstance = 0;
ETagAn* ETagAn::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new ETagAn(); }
  return fInstance;
}

ETagAn::ETagAn()
{
  
  fRecoEvent   = NULL;

  fECal_hitEvent=0;
  fETag_hitEvent=0;
  
  fVerbose        = 0;
  fhSvcVal = HistoSvc::GetInstance();
  fECalSel = ECalSel::GetInstance(); 

//  std::map<int, int> HitToClusterMapLeft = {
//    {0, 0}, {1, 0}, {2, 0}, {3, 0},
//    {100, 1}, {101, 1}, {102, 1}, {103, 1},
//    {200, 2}, {201, 2}, {202, 2}, {203, 2},
//    {300, 3}, {301, 3}, {302, 3}, {303, 3},
//    {400, 4}, {401, 4}, {402, 4}, {403, 4},
//    {500, 5}, {501, 5}, {502, 5}, {503, 5},
//    {600, 60},{601, 60},{602, 60},{603, 60},
//    {700, 70}, {701, 70}, {702, 70}, {703, 70},
//    {800, 80}, {801, 80}, {802, 80}, {803, 80},
//    {900, 9}, {901, 9}, {902, 9}, {903, 9},
//    {1000, 10}, {1001, 10}, {1002, 10}, {1003, 10},
//    {1100, 11}, {1101, 11}, {1102, 11}, {1103, 11},
//    {1200, 12}, {1201, 12}, {1202, 12}, {1203, 12},
//    {1300, 13}, {1301, 13}, {1302, 13}, {1303, 13},
//    {1400, 14}, {1401, 14}, {1402, 14}, {1403, 14},


//  std::map<int, int> HitToClusterMapRight = {
//    {10, 0}, {11, 0}, {12, 0}, {13, 0},
//    {110, 1}, {111, 1}, {112, 1}, {113, 1},
//    {210, 2}, {211, 2}, {212, 2}, {213, 2},
//    {310, 3}, {311, 3}, {312, 3}, {313, 3},
//    {410, 4}, {411, 4}, {412, 4}, {413, 4},
//    {510, 5}, {511, 5}, {512, 5}, {513, 5},
//    {610, 61}, {611, 61}, {612, 61}, {613, 61},
//    {710, 71}, {711, 71}, {712, 71}, {713, 71},
//    {810, 81}, {811, 81}, {812, 81}, {813, 81},
//    {910, 9}, {911, 9}, {912, 9}, {913, 9},
//    {1010, 10}, {1011, 10}, {1012, 10}, {1013, 10},
//    {1110, 11}, {1111, 11}, {1112, 11}, {1113, 11},
//    {1210, 12}, {1211, 12}, {1212, 12}, {1213, 12},
//    {1310, 13}, {1311, 13}, {1312, 13}, {1313, 13},
//    {1410, 14}, {1411, 14}, {1412, 14}, {1413, 14},
//  };
}
ETagAn::~ETagAn()
{
}

Bool_t ETagAn::Init(PadmeAnalysisEvent* event){
  fRecoEvent = event->RecoEvent;
  fECal_hitEvent = event->ECalRecoEvent;
  fECal_clEvent = event->ECalRecoCl; 
  fETag_hitEvent = event->ETagRecoEvent; 
  fETag_clEvent = event->ETagRecoCl;

  // init the left/right ETag maps

  for (int i=0; i<1500; i++){
    HitToClusterMapLeft[i] = -1;
    HitToClusterMapRight[i] = -1;
  }

  for (int j=0; j<6; j++){ 
    for (int i=0; i<4; i++){
      HitToClusterMapLeft[j*100 + i]       = j; //bottom slabs, 0-5, left sipm
      HitToClusterMapRight[j*100 + 10 + i] = j; //bottom slabs, 0-5, right sipm

      HitToClusterMapLeft[j*100 + 900 + i]       = 9+j; //top slabs, 0-5, left sipm
      HitToClusterMapRight[j*100 + 900 + 10 + i] = 9+j; //top slabs, 0-5, right sipm
    }
  }

  for (int j=0; j<3; j++){ 
    for (int i=0; i<4; i++){
      HitToClusterMapLeft[j*100 + 600 + i]       = (6+j)*10; //middle (short) slabs, 6-8, left sipm only
      HitToClusterMapRight[j*100 + 600 + 10 + i] = (6+j)*10 + 1; //middle (short) slabs, 6-8, right sipm only
    }
  }

  for (int i=0; i<1500; i++){
    if (HitToClusterMapLeft[i] != -1) std::cout << " Left["<<i<<"] = " << HitToClusterMapLeft[i];
  }
  for (int i=0; i<1500; i++){
    if (HitToClusterMapRight[i] != -1) std::cout << " Righ["<<i<<"] = " << HitToClusterMapRight[i];
  }
  
  InitHistos();

  return true;
}


Bool_t ETagAn::Process(Bool_t isMC)
{

  ETagMatch();
  return true;
}

Int_t ETagAn::ETagMatch(){
  // 
  // given pairs of clusters compatible with the e+e- --> gamma-gamma/e+e- transition
  // try to match ETag activity to each ECalCluster -> plots number of matches
  // will make more sophisticate reconstruction of the ETag position using the time and the ECal position

  const double dtMin = 15; // ns matching window 
  const double dtMax = 24; // ns matching window

  TRecoVCluster* tempClu;

  // match cluster pairs with ETAG hits

  const double fETagBarSizeX[2] = {660, 265};//*mm; long/short
  const double fETagBarSizeY = 44;//*mm;   long/short

  for (int i=0; i< fECalSel->getNECalEvents(); i++){ // loop over cluster pairs found
    if (fECalSel->getECalEvent(i).flagEv != ev_gg && fECalSel->getECalEvent(i).flagEv != ev_ee) continue;

    int ecalIndex[2]    = {fECalSel->getECalEvent(i).indexECal[0],fECalSel->getECalEvent(i).indexECal[1]};
    TVector3 ecalpos[2] = {fECal_clEvent->Element(ecalIndex[0])->GetPosition(), fECal_clEvent->Element(ecalIndex[1])->GetPosition()};
    double ecalTimes[2] = {fECal_clEvent->Element(ecalIndex[0])->GetTime(), fECal_clEvent->Element(ecalIndex[1])->GetTime()};

    int nLeftSiPMMatched[2] = {0,0}; // number of left SiPM matched
    int nRightSiPMMatched[2] = {0,0}; // number of right SiPM matched
    double dtimeLeftSiPMMatchAv[2] = {0.,0.};
    double dtimeRightSiPMMatchAv[2] = {0.,0.};
    double dtimeLeftSiPMMatchRms[2] = {0.,0.};
    double dtimeRightSiPMMatchRms[2] = {0.,0.};

    // evaluate position of the ecal clusters in the ETag frame
    int bary[2];
    double sipmDistLeft[2] = {-1,-1}; // -1 if not instrumented
    double sipmDistRight[2] = {-1,-1};// -1 if not instrumented

    for (int j=0; j<2; j++){
      bary[j] = (ecalpos[j].Y()+fETagBarSizeY*0.5)/fETagBarSizeY + 7;
      if (bary[j] < 0 || bary[j] > 14) {
	std::cout << "strange value " << bary[j] << " " << ecalpos[j].Y() << " " << j << std::endl;
      }
      if (bary[j] < 6 || bary[j] > 8) { // two-sided bar
	sipmDistLeft[j] =  fETagBarSizeX[0]*0.5 + ecalpos[j].X();
	sipmDistRight[j] = fETagBarSizeX[0]*0.5 - ecalpos[j].X();
      }
      else if (ecalpos[j].X() < 0) {
	sipmDistLeft[j] =  fETagBarSizeX[1] + 1.5*fETagBarSizeY + ecalpos[j].X(); // it should be equal to fETagBarSizeX[0]	
	sipmDistRight[j] = -1;
      } else {
	sipmDistLeft[j] = -1;
	sipmDistRight[j] = fETagBarSizeX[1] + 1.5*fETagBarSizeY - ecalpos[j].X();
      }

      //      std::cout <<" ecal cluster " << j << " (x,y) = " << ecalpos[j].X() << " , " << ecalpos[j].Y() << " YBar Tag = " << bary[j] << " distL, distR = " << sipmDistLeft[j] << " , " << sipmDistRight[j] << " t = " << ecalTimes[j] << std::endl;
      if (sipmDistRight[j]>0) fhSvcVal->FillHisto2List("ETagAn","ECal_SC_ECal_LRvsYBars",bary[j],1);
      if (sipmDistLeft[j]>0)  fhSvcVal->FillHisto2List("ETagAn","ECal_SC_ECal_LRvsYBars",bary[j],0);
    }

  
    // match cluster pairs with ETAG hits

    for (int h1 = 0; h1< fETag_hitEvent->GetNHits(); h1++) {
      TRecoVHit* hitto = fETag_hitEvent->Hit(h1);
      int ich = -1;
      int lr = -1;
      int sipm = -1;
      if (HitToClusterMapLeft[hitto->GetChannelId()] != -1) { //left channel
	ich = HitToClusterMapLeft[hitto->GetChannelId()];
	lr = 0; //left SiPM hit
	sipm = hitto->GetChannelId()%100;
      }
      else if (HitToClusterMapRight[hitto->GetChannelId()] != -1) { //right channel
	ich = HitToClusterMapRight[hitto->GetChannelId()];
	lr = 1; //right SiPM hit
	sipm = (hitto->GetChannelId()-10)%100;
      }
      else {
	std::cout << "Strange ETag hit " << h1 << " chID = " << hitto->GetChannelId() << " time = " << hitto->GetTime() << std::endl;
      }
      if (ich > 14) ich = TMath::Abs(ich/10); // 6, 7, 8
    //    std::cout << "ETag Hit " << j << " / " << fETag_hitEvent->GetNHits() << " chId = " << hitto->GetChannelId() << " t = " << hitto->GetTime() << " chbar = " << ich << " lr = " << lr << std::endl;

      for (int j=0; j<2; j++){ // ecal clusters
	double dt = hitto->GetTime() - ecalTimes[j];
	if (lr == 1 && sipmDistRight[j] > 0) { //right hit
	  if (TMath::Abs(ich-bary[j]) < 2) {
	    fhSvcVal->FillHisto2List("ETagAn",Form("ECal_SC_ETagHitsRightCh_%d_SiPM_%d_dtvsr",ich,sipm), sipmDistRight[j], dt);
	    fhSvcVal->FillHisto2List("ETagAn",Form("ECal_SC_ETagHitsRightCh_%d_dtvsr",ich), sipmDistRight[j], dt);
	  }
	  fhSvcVal->FillHisto2List("ETagAn",Form("ECal_SC_ETagHitsRightCh_%d_SiPM_%d_dtvsdch",ich,sipm), ich-bary[j], dt);

	  if (TMath::Abs(ich-bary[j]) < 2 && dt > dtMin && dt < dtMax) {
	    nRightSiPMMatched[j]++;
	    dtimeRightSiPMMatchAv[j] += dt;
	    dtimeRightSiPMMatchRms[j] += dt*dt;
	  } 

	}
	else if (lr == 0 && sipmDistLeft[j] > 0) { // left hit
	  if (TMath::Abs(ich-bary[j]) < 2) {
	    fhSvcVal->FillHisto2List("ETagAn",Form("ECal_SC_ETagHitsLeftCh_%d_SiPM_%d_dtvsr",ich,sipm) , sipmDistLeft[j], dt);
	    fhSvcVal->FillHisto2List("ETagAn",Form("ECal_SC_ETagHitsLeftCh_%d_dtvsr",ich), sipmDistLeft[j], dt);
	  }
	  fhSvcVal->FillHisto2List("ETagAn",Form("ECal_SC_ETagHitsLeftCh_%d_SiPM_%d_dtvsdch",ich,sipm), ich-bary[j], dt);

	  if (TMath::Abs(ich-bary[j]) < 2 && dt > dtMin && dt < dtMax) {
	    nLeftSiPMMatched[j]++;
	    dtimeLeftSiPMMatchAv[j] += dt;
	    dtimeLeftSiPMMatchRms[j] += dt*dt;
	  } 


	}
      }
    } // loop over ETag hits


    // study the matching 
    for (int j=0; j<2; j++){

      if (nLeftSiPMMatched[j]) {
	dtimeLeftSiPMMatchAv[j] /= nLeftSiPMMatched[j];
	if (nLeftSiPMMatched[j] > 1) 
	  dtimeLeftSiPMMatchRms[j] = TMath::Sqrt(
						 (dtimeLeftSiPMMatchRms[j]/nLeftSiPMMatched[j] - dtimeLeftSiPMMatchAv[j]*dtimeLeftSiPMMatchAv[j])/
						 (nLeftSiPMMatched[j]-1)
						 );
	fhSvcVal->FillHisto2List("ETagAn","ECal_SC_ETagHitsMatchLeftTimevsN",nLeftSiPMMatched[j]*1.,dtimeLeftSiPMMatchAv[j]);	
      }

      if (nRightSiPMMatched[j]) {
	dtimeRightSiPMMatchAv[j] /= nRightSiPMMatched[j];
	if (nRightSiPMMatched[j] > 1) 
	  dtimeRightSiPMMatchRms[j] = TMath::Sqrt(
						  (dtimeRightSiPMMatchRms[j]/nRightSiPMMatched[j] - dtimeRightSiPMMatchAv[j]*dtimeRightSiPMMatchAv[j])/
						  (nRightSiPMMatched[j]-1)
						  );
	fhSvcVal->FillHisto2List("ETagAn","ECal_SC_ETagHitsMatchRightTimevsN",nRightSiPMMatched[j]*1.,dtimeRightSiPMMatchAv[j]);
      }

      fhSvcVal->FillHisto2List("ETagAn","ECal_SC_ETagHitsMatchRightVsLeft",nLeftSiPMMatched[j]*1.,nRightSiPMMatched[j]*1.);

    }
    fhSvcVal->FillHisto2List("ETagAn","ECal_SC_ETagHitsMatches",
			(double)(TMath::Min(nLeftSiPMMatched[0],9) + 10*TMath::Min(nRightSiPMMatched[0],9)),
			(double)(TMath::Min(nLeftSiPMMatched[1],9) + 10*TMath::Min(nRightSiPMMatched[1],9))
			);



    // match cluster pairs with ETAG clusters 

    for (int h1=0; h1< fETag_clEvent->GetNElements(); ++h1) {
      tempClu = fETag_clEvent->Element((int)h1);
      Int_t nhits = tempClu->GetNHitsInClus();
      Int_t channel = tempClu->GetChannelId();

      Int_t isRight = 1; // right SiPM
      if (channel < 0) isRight = 0; // left SiPM
      Int_t baryETag = TMath::Abs(channel);
      if (baryETag > 14) baryETag = TMath::Abs(baryETag/10); // 60/61 -> 6, etc 7, 8

      //      std::cout << "ETag channel " << h1 << " / " << fETag_clEvent->GetNElements() << " " << channel << " bary =  " << baryETag << " t = " << tempClu->GetTime() << endl;

      for (int j=0; j<2; j++){ // ecal clusters
	if (bary[j] != baryETag) continue; // require same Y bar
	double dt = tempClu->GetTime() - ecalTimes[j];
	if (isRight) fhSvcVal->FillHisto2List("ETagAn",Form("ECal_SC_RightCh_%d_dtvsr",baryETag), sipmDistRight[j], dt);
	else         fhSvcVal->FillHisto2List("ETagAn",Form("ECal_SC_LeftCh_%d_dtvsr",baryETag), sipmDistLeft[j], dt);
      }
    }// loop over ETag clusters	

//    std::vector<Int_t> ihits = tempClu->GetHitVecInClus();
//    std::cout << "ETag Cluster " << h1 << " has " << nhits << " hits and chId = " << tempClu->GetChannelId() << std::endl;
//    for (int j = 0; j< nhits; j++) {
//      TRecoVHit* hitn = fETag_hitEvent->Hit(ihits[j]);
//      if (hitn) {
//	std::cout << " hit # " << j << " / " << nhits << " is channel " << hitn->GetChannelId() << endl;
//      }
//      else {
//	std::cout << " hit # " << j << " / " << nhits << " is ??? " << endl;
//      }
//    }
  } // loop over cluster pairs

  return 0;
}


Bool_t ETagAn::InitHistos()
{
// x and y slices
//  Double_t fXMin = -21.*(14+0.5);
//  Double_t fXMax = 21.*(14+0.5);
//  Double_t fXW = 21; // mm
//  Int_t fNXBins = (fXMax-fXMin)/fXW;
//  Double_t fYMin = -21.*(14+0.5);
//  Double_t fYMax = 21.*(14+0.5);
//  Double_t fYW = 21; // mm
//  Int_t fNYBins = (fYMax-fYMin)/fYW;

  HistoSvc* fhSvcVal =  HistoSvc::GetInstance();

  for (int q=0; q<15; q++){
    for (int aa = 0; aa < 4; aa++) { // SiPM
      fhSvcVal->BookHisto2List("ETagAn",Form("ECal_SC_ETagHitsRightCh_%d_SiPM_%d_dtvsdch",q,aa),61,-30.5,30.5,100,-50,50); 
      fhSvcVal->BookHisto2List("ETagAn",Form("ECal_SC_ETagHitsLeftCh_%d_SiPM_%d_dtvsdch",q,aa),61,-30.5,30.5,100,-50,50); 
      fhSvcVal->BookHisto2List("ETagAn",Form("ECal_SC_ETagHitsRightCh_%d_SiPM_%d_dtvsr",q,aa),29,0,609,200,-50,50); 
      fhSvcVal->BookHisto2List("ETagAn",Form("ECal_SC_ETagHitsLeftCh_%d_SiPM_%d_dtvsr",q,aa),29,0,609,200,-50,50); 
    }
    fhSvcVal->BookHisto2List("ETagAn",Form("ECal_SC_ETagHitsRightCh_%d_dtvsr",q),21,0,609,200,-50,50); 
    fhSvcVal->BookHisto2List("ETagAn",Form("ECal_SC_ETagHitsLeftCh_%d_dtvsr",q),21,0,609,200,-50,50); 
    fhSvcVal->BookHisto2List("ETagAn",Form("ECal_SC_RightCh_%d_dtvsr",q),21,0,609,100,-50,50); 
    fhSvcVal->BookHisto2List("ETagAn",Form("ECal_SC_LeftCh_%d_dtvsr",q),21,0,609,100,-50,50); 
  }

  fhSvcVal->BookHisto2List("ETagAn","ECal_SC_ECal_LRvsYBars",15,-0.5,14.5,2,-0.5,1.5); 
  fhSvcVal->BookHisto2List("ETagAn","ECal_SC_ETagHitsMatchLeftTimevsN",20,0,20,20,15.,25.);
  fhSvcVal->BookHisto2List("ETagAn","ECal_SC_ETagHitsMatchRightTimevsN",20,0,20,20,15.,25.);
  fhSvcVal->BookHisto2List("ETagAn","ECal_SC_ETagHitsMatchRightVsLeft",20,0,20,20,0,20);
  fhSvcVal->BookHisto2List("ETagAn","ECal_SC_ETagHitsMatches",100,0,100,100,0,100);

  return true;
}


Bool_t ETagAn::Finalize(){
  TString labl[2] = {"Right","Left"};
  

  HistoSvc* fhSvcVal =  HistoSvc::GetInstance();
  fhSvcVal->BookHistoList("ETagAn","ETagTimeOffsets",120,0.,120.);
  int elcount = 0;
  for (int side = 0; side < 2; side++){
    for (int q=0; q<15; q++){
      for (int aa = 0; aa < 4; aa++) { // SiPM
	TH1D* rightpro = (fhSvcVal->GetHisto2List("ETagAn",Form("ECal_SC_ETagHits%sCh_%d_SiPM_%d_dtvsdch",labl[side].Data(),q,aa)))->ProjectionY(Form("%spro",labl[side].Data()));
	double timeMax = rightpro->GetBinCenter(rightpro->GetMaximumBin());
	fhSvcVal->GetHistoList("ETagAn","ETagTimeOffsets")->SetBinContent(elcount,timeMax);
	elcount++;
      }

      fhSvcVal->BookHistoList("ETagAn",Form("DTVsR_Hits%sCh_%d",labl[side].Data(),q),18,0,378.);
      TProfile* profo = (fhSvcVal->GetHisto2List("ETagAn",Form("ECal_SC_ETagHits%sCh_%d_dtvsr",labl[side].Data(),q)))->ProfileX(Form("%sprof%d",labl[side].Data(),q),122,161); //to be done better
      int ibins = (profo->GetXaxis())->GetNbins();
      for (int ib=0; ib < ibins; ib++){
	double xx = profo->GetBinCenter(ib+1);
	double conto = profo->GetBinContent(ib+1);
	double conte = profo->GetBinError(ib+1);
	int ibb = xx/21.+1; //to be done better
	fhSvcVal->GetHistoList("ETagAn",Form("DTVsR_Hits%sCh_%d",labl[side].Data(),q))->SetBinContent(ibb,conto);
	fhSvcVal->GetHistoList("ETagAn",Form("DTVsR_Hits%sCh_%d",labl[side].Data(),q))->SetBinError(ibb,conte);
      }
    }
  }

  return true;
}
