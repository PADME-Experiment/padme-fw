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

  fETagECalAss.clear();
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

      HitToClusterMapLeft[j*100 + 900 + i]       = 9+j; //top slabs, 9-14, left sipm
      HitToClusterMapRight[j*100 + 900 + 10 + i] = 9+j; //top slabs, 9-14, right sipm
    }
  }

  for (int j=0; j<3; j++){ 
    for (int i=0; i<4; i++){
      HitToClusterMapLeft[j*100 + 600 + i]       = (6+j)*10; //middle (short) slabs, 6-8, left sipm only
      HitToClusterMapRight[j*100 + 600 + 10 + i] = (6+j)*10 + 1; //middle (short) slabs, 6-8, right sipm only
    }
  }

//  for (int i=0; i<1500; i++){
//    if (HitToClusterMapLeft[i] != -1) std::cout << " Left["<<i<<"] = " << HitToClusterMapLeft[i];
//  }
//  for (int i=0; i<1500; i++){
//    if (HitToClusterMapRight[i] != -1) std::cout << " Righ["<<i<<"] = " << HitToClusterMapRight[i];
//  }

// init array of etag-ecal associations

  InitHistos();

  return true;
}


Bool_t ETagAn::Process()
{

  ETagMatch();
  return true;
}

Int_t ETagAn::ETagSingleMatch(){
  // 
  // given single clusters compatible with the beam e+
  // try to match ETag activity to the ECalCluster -> fraction of matches

  for (int i=0; i< fECalSel->getNECalEvents(); i++){ // loop over ecal events found
    if (fECalSel->getECalEvent(i).flagEv != ev_single) continue; // only use single-cluster events here

    ETagECalAss ass[4]; // cluster 0 standard, 0 with time offset, 0 with 2xtime offsets, ...
    int assCode[4];

    for (int q=0; q<4; q++){ // standard/timeOffsets
      int ecalIndex  = fECalSel->getECalEvent(i).indexECal[0];
      ass[q] = AssociateECalCluster(ecalIndex,q);
      assCode[q] = ETagConditions(ass[q]);
    }

    // Denominator 

    fhSvcVal->FillHisto2List("ETagAn","ECalSingle_ETag_Deno",ass[0].nETagBar,ass[0].closestSide,1.);

    // Match condition

    for (int q = 0; q<4; q++) { // loop over time windows
      if (assCode[q] & (1 << kSingleSideMatch)) { // only match with loosest algorithm
	fhSvcVal->FillHisto2List("ETagAn",Form("ECalSingle_ETag_Nume_Off%d",q),ass[0].nETagBar,ass[0].closestSide,1.);
      }
    } 
  } // loop over ECal events    
  return 0;
}


Int_t ETagAn::ETagMatch(){
  
  // given pairs of clusters compatible with the e+e- --> gamma-gamma/e+e- transition
  // try to match ETag activity to each ECalCluster -> plots number of matches
  // make first-order evaluation of the ETag efficiency using tag & probe method

  int tagBits[2] = {kSingleSideMatch,kDoubleSideMatch};

  fETagECalAss.clear();  

  for (int i=0; i< fECalSel->getNECalEvents(); i++){ // loop over ecal events found
    if (fECalSel->getECalEvent(i).flagEv != ev_gg && fECalSel->getECalEvent(i).flagEv != ev_ee) continue; // only use 2-cluster events here

    ETagECalAss ass[8]; // cluster 0 standard, 1 standard, 0 with time offset, 1 with time offset, 0 with 2xtime offsets, ...
    for (int q=0; q<4; q++){ // standard/timeOffset
      for (int j=0; j<2; j++){
	int ecalIndex  = fECalSel->getECalEvent(i).indexECal[j];
	ass[j+2*q] = AssociateECalCluster(ecalIndex,q);
      }
    }

    fhSvcVal->FillHisto2List("ETagAn","ECal_SC_ETagHitsMatches",
			     (double)(ass[0].nAss[0] + maxETagAssPerSide*ass[0].nAss[1]),
			     (double)(ass[1].nAss[0] + maxETagAssPerSide*ass[1].nAss[1])
			     );


    // Study of energy release
    for (int j=0; j<2; j++){ // pair combination {0,1}, {1,0}
      int closestSide = ass[j].closestSide;
      fhSvcVal->FillHisto2List("ETagAn",Form("ECalDouble_ETag_EvsR_Side%s_Bar%d",fLabel[closestSide].Data(),ass[j].nETagBar),
			       ass[j].sipmDist[closestSide],ass[j].EAss[closestSide],1.);
    }


    // tag and probe efficiency

    for (int j=0; j<2; j++){ // pair combination {0,1}, {1,0}
      int iass = j; // tag clus
      int jass = 1-j; // probe clus

      // tag with L_OR_R PROBE
      int iassCode = ETagConditions(ass[iass]);
      int jassCode[4];
      for (int q = 0; q<4; q++) jassCode[q] = ETagConditions(ass[jass + 2*q]);

      for (int k=0; k<2; k++){	 // loop over probe algorithms
	if (!(iassCode & (1<< tagBits[k]))) continue; // no tag, pass to the next combination

	fhSvcVal->FillHisto2List("ETagAn",Form("ECal_ETag_%dSlab_ProbeAlgo%d",ass[jass].nETagBar,k),
				 (double)(ass[jass].nAss[0]), (double)(ass[jass].nAss[1]),1.);

	// Denominator Probe

	fhSvcVal->FillHisto2List("ETagAn",Form("ECal_ETag_ProbeAlgo%d_Deno",k),ass[jass].nETagBar,ass[jass].closestSide,1.);

	// Match condition

	for (int q = 0; q<4; q++) { // loop over time windows
	  if (jassCode[q] & (1 << kSingleSideMatch)) { // only match with loosest algorithm
	    fhSvcVal->FillHisto2List("ETagAn",Form("ECal_ETag_ProbeAlgo%d_Nume_Off%d",k,q),ass[jass].nETagBar,ass[jass].closestSide,1.);
	  }
	}
      } // loop over algorithms for tag
    } // loop over component of the pair (should we use each pair twice?)
  } // loop over cluster pairs

  return 0;
}


Int_t ETagAn::ETagConditions(ETagECalAss ass){ 
  // return codes: 
  //    bit 0 -> one-side (closest side): NSiPM > 2 && RMS time < max
  //    bit 1 -> two-side (if available, otherwise it's like bit 0): N > 3 per side && RMS time < max 
  //    to do: add energy cut parametrized per side
  const double rmsMax = 0.8;
  int probeCode = 0;

  if (ass.nAss[ass.closestSide]>2 && ass.rmsTime[ass.closestSide] < rmsMax) {
    probeCode |= (1 << kSingleSideMatch); // single-side tag
  }

  if (ass.nAss[ass.closestSide]>2 && ass.rmsTime[ass.closestSide] < rmsMax) {
    if (ass.nSides == 2) {
      if (ass.nAss[1-ass.closestSide]>2 && ass.rmsTime[1-ass.closestSide] < rmsMax) probeCode |= (1 << kDoubleSideMatch); // left AND right tag
    }
    else {
      probeCode |= (1 << kDoubleSideMatch); // cant ask left and right, since it's one-sided bar
    }
  }

  return probeCode;
}




ETagECalAss ETagAn::AssociateECalCluster(int indexCl, int timeOff){
  const double timeOffset = -20.;

  const double dtMin = 15; // ns matching window 
  const double dtMax = 24; // ns matching window
  const double fETagBarSizeX[2] = {660, 265};//*mm; long/short
  const double fETagBarSizeY = 44;//*mm;   long/short

  TVector3 ecalpos = fECal_clEvent->Element(indexCl)->GetPosition();
  double ecalTimes = fECal_clEvent->Element(indexCl)->GetTime();

  int nSiPMMatched[2] = {0,0}; // number of left,right SiPM matched
  int iSiPMMatched[2][maxETagAssPerSide]; // index of left,right SiPM matched
  int dChannel[2][maxETagAssPerSide]; // index of left,right SiPM matched
  double dtimeSiPMMatchAv[2] = {0.,0.}; //average of left,right times of SiPM matched
  double dtimeSiPMMatchRms[2] = {0.,0.};//rms of left, right times of SiPM matched

  // evaluate position of the ecal cluster wrt the ETag frame

  double sipmDist[2] = {-1,-1}; // distance from left, right sipm (-1 if not instrumented)

  int bary = (ecalpos.Y()+fETagBarSizeY*0.5)/fETagBarSizeY + 7;
  if (bary < 0 || bary > 14) {
    std::cout << "ETag association: inconsistent value " << bary << " " << ecalpos.Y() << std::endl;
  }

  if (bary < 6 || bary > 8) { // two-sided bar
    sipmDist[0] = fETagBarSizeX[0]*0.5 + ecalpos.X();
    sipmDist[1] = fETagBarSizeX[0]*0.5 - ecalpos.X();
  }
  else if (ecalpos.X() < 0) {
    sipmDist[0] =  fETagBarSizeX[1] + 1.5*fETagBarSizeY + ecalpos.X(); // it should be equal to fETagBarSizeX[0]	
    sipmDist[1] = -1;
  } else {
    sipmDist[0] = -1;
    sipmDist[1] = fETagBarSizeX[1] + 1.5*fETagBarSizeY - ecalpos.X();
  }

  // evaluate if the ecal cluster is located in the lower or upper half of the bar

  double downEdgeDist = ecalpos.Y()+fETagBarSizeY*0.5 - fETagBarSizeY*(bary-7);

  int dchannelCut = 1;                                   // will match in the same bar and in the bar above
  if (downEdgeDist < fETagBarSizeY*0.5) dchannelCut = -1; // will match in the same bar and in the bar below


  for (int j=0; j<2; j++){ //left, right
    if (sipmDist[j] > 0) fhSvcVal->FillHisto2List("ETagAn","ECal_SC_ECal_LRvsYBars",bary,j);
  }
  
  // match cluster with ETAG hits

  for (int h1 = 0; h1< fETag_hitEvent->GetNHits(); h1++) {

    TRecoVHit* hitto = fETag_hitEvent->Hit(h1);
    int ich = -1;
    int lr = -1;
    int sipm = -1;

    // retrieve slab, lr flag and sipm id

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
      continue;
    }
    if (ich > 14) ich = TMath::Abs(ich/10); // 6, 7, 8

    // check cluster matching

    double dt = hitto->GetTime() - ecalTimes;

    if (sipmDist[lr] > 0) { // hit of the correct side

      if (timeOff == 0) { // no offset -> fill histograms
	fhSvcVal->FillHisto2List("ETagAn",Form("ECal_SC_ETagHits%sCh_%d_SiPM_%d_dtvsdch",fLabel[lr].Data(),ich,sipm), ich-bary, dt);
	if ((ich == bary) || (ich == bary + dchannelCut)) {
	  fhSvcVal->FillHisto2List("ETagAn",Form("ECal_SC_ETagHits%sCh_%d_SiPM_%d_dtvsr",fLabel[lr].Data(),ich,sipm), sipmDist[lr], dt);
	  fhSvcVal->FillHisto2List("ETagAn",Form("ECal_SC_ETagHits%sCh_%d_SiPM_%d_dtvsE",fLabel[lr].Data(),ich,sipm), hitto->GetEnergy(), dt);
	  fhSvcVal->FillHisto2List("ETagAn",Form("ECal_SC_ETagHits%sCh_%d_dtvsr",fLabel[lr].Data(),ich), sipmDist[lr], dt);
	}
      }
	
      if (((ich==bary) || (ich == bary + dchannelCut)) && dt > dtMin+timeOff*timeOffset && dt < dtMax+timeOff*timeOffset) {
	if (timeOff == 0) { // no offset
	  fhSvcVal->FillHisto2List("ETagAn",Form("ECal_ETagHits_%sCh_%d_Evsr",fLabel[lr].Data(),ich), sipmDist[lr], hitto->GetEnergy());
	}
	else {
	  fhSvcVal->FillHisto2List("ETagAn",Form("ECal_ETagHits_%sCh_%d_EvsrOff",fLabel[lr].Data(),ich), sipmDist[lr], hitto->GetEnergy());
	}
	if (nSiPMMatched[lr] < maxETagAssPerSide) {
	  iSiPMMatched[lr][nSiPMMatched[lr]] = h1;
	  dChannel[lr][nSiPMMatched[lr]] = bary-ich;
	}
	nSiPMMatched[lr]++;
	dtimeSiPMMatchAv[lr] += dt;
	dtimeSiPMMatchRms[lr] += dt*dt;
      } 
    }
  } // loop over ETag hits

  // fill entry for array of ETagECal associations 
  
  ETagECalAss ass;
  ass.indexECal = indexCl;
  ass.nETagBar = bary; // index of ETagBar identified from the ECal cluster

  for (int j=0; j<2; j++){ //left, right  
    ass.nAss[j] = TMath::Min(nSiPMMatched[j],maxETagAssPerSide); // number of Left,Right SiPM associations
    ass.EAss[j] = 0.;
    for (int i=0; i<ass.nAss[j]; i++) {
      ass.iAss[j][i] = iSiPMMatched[j][i]; //indices of the Left,Right SiPM hits associated 
      ass.dCh[j][i] = dChannel[j][i];
      ass.EAss[j] += fETag_hitEvent->Hit(ass.iAss[j][i])->GetEnergy();
    }

    // distance to the SiPMs of that side

    ass.sipmDist[j] = sipmDist[j];

    // average time per side

    if (nSiPMMatched[j]) {
      ass.avgTime[j] = dtimeSiPMMatchAv[j] / nSiPMMatched[j];
    }
    else {
      ass.avgTime[j] = -999;
    }

    // average rms of the times per side

    if (nSiPMMatched[j] > 1){
      ass.rmsTime[j] = TMath::Sqrt(
				   (dtimeSiPMMatchRms[j]/nSiPMMatched[j] - ass.avgTime[j]*ass.avgTime[j])/
				   (nSiPMMatched[j]-1) );
    }
    else {
      ass.rmsTime[j] = -999;
    }

    fhSvcVal->FillHisto2List("ETagAn",Form("ECal_SC_ETagHitsMatch%sTimevsN",fLabel[j].Data()),nSiPMMatched[j]*1.,ass.avgTime[j]-timeOff*timeOffset);	
    fhSvcVal->FillHisto2List("ETagAn",Form("ECal_SC_ETagHitsMatch%sTimeRMSvsN",fLabel[j].Data()),nSiPMMatched[j]*1.,ass.rmsTime[j]);	
  }

  int closestSide = 0; //left by default
  int nSides = 2;
  if (ass.sipmDist[0]>0 && ass.sipmDist[1]>0){
    if (ass.sipmDist[1] < ass.sipmDist[0]) closestSide = 1;
  }
  else if (ass.sipmDist[1] > 0) {
    closestSide = 1;
    nSides = 1;
  }
  else if (ass.sipmDist[0] > 0) {
    closestSide = 0;
    nSides = 1;
  }
  else {
    std::cout << "ETagAn ETagAssociation: inconsistent side configuration " << std::endl;
  }

  ass.closestSide = closestSide;
  ass.nSides = nSides;

//  if (ass.nAss[0] > 2 && ass.nAss[1] > 2){
//    if (TMath::Abs(timeOff) < 0.5) {
//      std::cout << "---- Standard matching ---- " << std::endl;
//    } else {
//      std::cout << "---- OutOfTime matching ---- " << std::endl;
//    }
//    for (int j=0; j<2; j++) {
//      std::cout << "Bar = " << bary << " side " << j << " ECal cluster has time " << ecalTimes << " sideAvgT = " << ass.avgTime[j] << " rms = " << ass.rmsTime[j] << std::endl;
//      for (int i=0; i<ass.nAss[j]; i++) {
//	TRecoVHit* hitto = fETag_hitEvent->Hit(ass.iAss[j][i]);
//	std::cout << "ETag Hit " << i << " / " << ass.nAss[j] << " has time = " << hitto->GetTime()-ecalTimes << " and Energy = " << hitto->GetEnergy() << endl;
//      }
//    }
//  }

  fhSvcVal->FillHisto2List("ETagAn","ECal_SC_ETagHitsMatchRightVsLeft",nSiPMMatched[0]*1.,nSiPMMatched[1]*1.);
    
  return ass;
}








Int_t ETagAn::ETagMatchSave(){
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

  for (int i=0; i< fECalSel->getNECalEvents(); i++){ // loop over ecal events found
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

      if (sipmDistRight[j]>0) fhSvcVal->FillHisto2List("ETagAn","ECal_SC_ECal_LRvsYBars",bary[j],1);
      if (sipmDistLeft[j]>0)  fhSvcVal->FillHisto2List("ETagAn","ECal_SC_ECal_LRvsYBars",bary[j],0);
    }
  
    // match cluster pairs with ETAG hits

    for (int h1 = 0; h1< fETag_hitEvent->GetNHits(); h1++) {
      TRecoVHit* hitto = fETag_hitEvent->Hit(h1);
      int ich = -1;
      int lr = -1;
      int sipm = -1;

      // retrieve slab, lr flag and sipm id

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

	  if (TMath::Abs(ich-bary[j]) < 2 && dt > dtMin && dt < dtMax ) {
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
  fLabel[0] = "Left";
  fLabel[1] = "Right";

  fOffLabel = "Off";

  fhSvcVal->CreateList("ETagAn");

  for (int q=0; q<15; q++){
    for (int aa = 0; aa < 4; aa++) { // SiPM
      for (int i = 0; i<2; i++){
	fhSvcVal->BookHisto2List("ETagAn",Form("ECal_SC_ETagHits%sCh_%d_SiPM_%d_dtvsdch",fLabel[i].Data(),q,aa),61,-30.5,30.5,100,-50,50); 
	fhSvcVal->BookHisto2List("ETagAn",Form("ECal_SC_ETagHits%sCh_%d_SiPM_%d_dtvsr",fLabel[i].Data(),q,aa),29,0,609,200,-50,50); 
	fhSvcVal->BookHisto2List("ETagAn",Form("ECal_SC_ETagHits%sCh_%d_SiPM_%d_dtvsE",fLabel[i].Data(),q,aa),100,0.,5.,200,-50,50); 
      }
    }
    for (int i = 0; i<2; i++){
      fhSvcVal->BookHisto2List("ETagAn",Form("ECal_SC_ETagHits%sCh_%d_dtvsr",fLabel[i].Data(),q),21,0,609,200,-50,50); 
      fhSvcVal->BookHisto2List("ETagAn",Form("ECal_ETagHits_%sCh_%d_Evsr",fLabel[i].Data(),q),21,0,609,100,0.,5.); 
      fhSvcVal->BookHisto2List("ETagAn",Form("ECal_ETagHits_%sCh_%d_EvsrOff",fLabel[i].Data(),q),21,0,609,100,0.,5.); 
      fhSvcVal->BookHisto2List("ETagAn",Form("ECalDouble_ETag_EvsR_Side%s_Bar%d",fLabel[i].Data(),q),29,0,609,200,0,20); 
    }                        
  }

  fhSvcVal->BookHisto2List("ETagAn","ECal_SC_ECal_LRvsYBars",15,-0.5,14.5,2,-0.5,1.5); 
  for (int i = 0; i<2; i++){
    fhSvcVal->BookHisto2List("ETagAn",Form("ECal_SC_ETagHitsMatch%sTimevsN",fLabel[i].Data()),20,0,20,20,15.,25.);
    fhSvcVal->BookHisto2List("ETagAn",Form("ECal_SC_ETagHitsMatch%sTimeRMSvsN",fLabel[i].Data()),20,0,20,20,0.,5.);
  }
  fhSvcVal->BookHisto2List("ETagAn","ECal_SC_ETagHitsMatchRightVsLeft",20,0,20,20,0,20);
  fhSvcVal->BookHisto2List("ETagAn","ECal_SC_ETagHitsMatches",100,0,100,100,0,100);


  // Efficiency plots (Single cluster)

  fhSvcVal->BookHisto2List("ETagAn","ECalSingle_ETag_Deno",15,0,15,2,-0.5,1.5); 
  for (int q = 0; q<4; q++) { // loop over time windows
    fhSvcVal->BookHisto2List("ETagAn",Form("ECalSingle_ETag_Nume_Off%d",q),15,0,15,2,-0.5,1.5);
  }

  // Efficiency plots (Tag and probe)

  for (int j = 0; j< 15; j++){
    for (int k=0; k<2; k++){
      fhSvcVal->BookHisto2List("ETagAn",Form("ECal_ETag_%dSlab_ProbeAlgo%d",j,k),10,0,10,10,0,10);
    }
  }
  for (int k=0; k<2; k++){
    fhSvcVal->BookHisto2List("ETagAn",Form("ECal_ETag_ProbeAlgo%d_Deno",k),15,0,15,2,-0.5,1.5); // should do variable binning to deal with central slabs

    for (int q = 0; q<4; q++){
      fhSvcVal->BookHisto2List("ETagAn",Form("ECal_ETag_ProbeAlgo%d_Nume_Off%d",k,q),15,0,15,2,-0.5,1.5);
    }
  }

  return true;
}


Bool_t ETagAn::Finalize(){
  TString labl[2] = {"Right","Left"};
  

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