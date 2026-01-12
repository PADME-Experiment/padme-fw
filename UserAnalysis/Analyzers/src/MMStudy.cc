#include "MMStudy.hh"
#include "TGraph.h"
#include "TString.h"
#include "TF1.h"
#include "TMath.h"
#include "GeneralInfo.hh"
#include "ECalSel.hh"

struct tracklet{
  double slope; // dv/dz
  double inter; // v at mesh plane
  double chi2;
  double pars[4];// x0,y0,x1,y1
  TVector3 lambda; // cosines of track directions
  int nstrips;
}; // aggiungere la configurazione usata entro il tracklet

struct mmcluster{
  int ecalclusindex;
  double seedslope;
  vector<int> mmhitsindex;
  vector<bool> isolationflag;
  int nhitPerPlane[2];
  tracklet traco; // tracklet done with the last fit done on the cluster using the ip
  tracklet traconoip; // tracklet done with the last fit done on the cluster without using the ip
};

MMStudy* MMStudy::fInstance = 0;

MMStudy* MMStudy::GetInstance(){
  if ( fInstance == 0 ) { fInstance = new MMStudy(); }
  return fInstance;
}

MMStudy::~MMStudy(){
  delete fCfgParser;
  for (int i=0; i<3; i++) delete fTracker[i];
}

Bool_t MMStudy::Init(PadmeAnalysisEvent* event,  Bool_t fHistoModeVal, TString InputHistofileVal){

  if (fVerbose) printf("---> Initializing MMStudy\n");
  TString cfgFile = "config/UserAnalysis.conf";
  fHS = HistoSvc::GetInstance();
  fGeneralInfo = GeneralInfo::GetInstance();
  fNPoTAnalysis = NPoTAnalysis::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
//  fApplyQualityCheck = true;x
//  if(fCfgParser->HasConfig("GENERAL", "DataQualityLevel")){
//     fDataQualityLevel =TString(fCfgParser->GetSingleArg("GENERAL", "DataQualityLevel")).Atoi();
//  } //handling del DataQualityLevel non implementato

  // deve poter leggere il config e sapere se e' in read mode o flag mode, se e' in flag mode deve leggere il file di testo coi periodi con problemi
  // e determina la flag per quell'evento in base al tempo  
  fEvent = event;
  fNRun = fGeneralInfo->GetRunNumberFromDB(); //30000 vale solo per il 2022
  fHistoMode = fHistoModeVal;
  InputHistofile = InputHistofileVal;
  fEventCounter = 0;
  InitHistos(fNRun);

  for (int i = 0; i<3; i++) { // y view, x view, 3d
    fTracker[i] = new MMTracker(i);
    fTracker[i]->InitFitter();
    fTrackerDZ[i] = new MMTracker(10+i); // we want to fit also the DZ
    fTrackerDZ[i]->InitFitter();
  }


  return true;
}

Bool_t MMStudy::InitHistos(Int_t nRun){
  // MMStudy directory will contain all histograms related to this analysis

  fHS->CreateList("MMStudy");
  cout<<" Creating MMStudy Hystograms for Run "<<nRun<<" "<<endl;
  fHS->BookHisto2List("MMStudy","cluDistance",100,0,100,200,-100,100);
  fHS->BookHistoList("MMStudy","nPreselectedHitsPerECalClus",200,0,200);
  fHS->BookHisto2List("MMStudy","straightDistance",100,0,100,200,-100,100);
  fHS->BookHisto2List("MMStudy","straightDistanceDZ",100,0,100,200,-100,100);
  fHS->BookHisto2List("MMStudy","straightDistanceZMinusZExp",100,0,100,200,-100,100);
  fHS->BookHisto2List("MMStudy","straightDistanceDZDV",100,0,100,1000,-100,100);
  fHS->BookHisto2List("MMStudy","isolPlotPrev",200,-120,120,200,-50,50);
  fHS->BookHisto2List("MMStudy","isolPlotFoll",200,-120,120,200,-50,50);
  fHS->BookHistoList("MMStudy","nMMHitsInMMClusters",200,0,200);
  fHS->BookHisto2List("MMStudy","mmFitChi2",2000,0,1.,10,0,10);
  fHS->BookHisto2List("MMStudy","nMMHitsPerPlaneInMMClusters",20,0,20,20,0,20);
  fHS->BookHisto2List("MMStudy","nMMIsolHitsPerPlaneInMMClusters",20,0,20,20,0,20);
  fHS->BookHistoList("MMStudy","nMMClustersPerECalCluster",200,0,200);
  
  for (int nhit=0; nhit<10; nhit++){
    fHS->BookHisto2List("MMStudy",Form("residual_nhits%d",nhit),100,-5.,5.,400,-20,20);
    fHS->BookHisto2List("MMStudy",Form("residual_noipfit_nhits%d",nhit),100,-5.,5.,400,-20,20);
  }
  
  for (int j=0; j<16; j++){
    fHS->BookHisto2List("MMStudy",Form("pairs_vs_track_board%d",j),1000,-1.,1.,1000,-1.,1.);
    fHS->BookHistoList("MMStudy",Form("diff_pairs_track_board%d",j),1000,-1.,1.);
    fHS->BookHisto2List("MMStudy",Form("diffslopeinter_pairs_track_board%d",j),10,-0.05,0.05,10,-30.,30.);
    fHS->BookHisto2List("MMStudy",Form("diffslopeinter_pairs_target_board%d",j),10,-0.05,0.05,10,-30.,30.);      
  }
  fHS->BookHisto2List("MMStudy",Form("MatchingDoubletAngleVsDeltav_boardAll"),50,-30.,30.,100,-0.05,0.05);      
  for (int j=0; j<8; j++){
    fHS->BookHisto2List("MMStudy",Form("MatchingDoubletAngleVsDeltav_board%d",j),50,-30.,30.,100,-0.05,0.05);      
  }
  fHS->BookHisto2List("MMStudy",Form("MatchingDoubletsTimeClu_vs_DZ_boardAll"),400,-200.,200.,100,-1000.,1000.);
  fHS->BookHisto2List("MMStudy",Form("MatchingDoubletsFitChi2_vs_DZ_boardAll"),400,-200.,200.,100,0.,100.);

  
  fHS->BookHisto2List("MMStudy",Form("DistanceCluHit_z_vs_v"),200,-20.,20.,200,-20.,20.);
  fHS->BookHisto2List("MMStudy",Form("NCloseHits"),30,0,30,30,0,30);
  fHS->BookHisto2List("MMStudy",Form("NCloseDoublets"),30,0,30,30,0,30);
  fHS->BookHisto2List("MMStudy","FitChi2",100,0,100.,500,0,500); 
  
  for (int i=0; i<100; i++){
    fHS->BookHisto2List("MMStudy",Form("ECalSelClusters_yvsx_ev%d",i),100,-300.,300.,100,-300,300);
    for (int j=0; j<16; j++){
      fHS->BookHisto2List("MMStudy",Form("hough_transform_cluster_board%d_ev%d",j,i),1000,-0.2,0.2,1000,-400.,400.);
      fHS->BookHisto2List("MMStudy",Form("hough_transform_board%d_ev%d",j,i),1000,-0.2,0.2,1000,-400.,400.);
      fHS->BookHisto2List("MMStudy",Form("hough_transform_tar_board%d_ev%d",j,i),1000,-0.2,0.2,1000,-400.,400.);
      fHS->BookHisto2List("MMStudy",Form("zcorrvsv_board%d_ev%d",j,i),1200,-600.,600.,200,fGeneralInfo->GetMMPosPlaneZ(0)-50,fGeneralInfo->GetMMPosPlaneZ(1)+200);
      fHS->BookHisto2List("MMStudy",Form("zvsv_board%d_ev%d",j,i),1200,-600.,600.,200,fGeneralInfo->GetMMPosPlaneZ(0)-50,fGeneralInfo->GetMMPosPlaneZ(1)+200);
      fHS->BookHisto2List("MMStudy",Form("zvsv_selecthit_board%d_ev%d",j,i),1200,-600.,600.,200,fGeneralInfo->GetMMPosPlaneZ(0)-50,fGeneralInfo->GetMMPosPlaneZ(1)+200);
      fHS->BookHisto2List("MMStudy",Form("zvsv_hitforclus_board%d_ev%d",j,i),1200,-600.,600.,200,fGeneralInfo->GetMMPosPlaneZ(0)-50,fGeneralInfo->GetMMPosPlaneZ(1)+200);
      fHS->BookHisto2List("MMStudy",Form("zvsv_isolhitforclus_board%d_ev%d",j,i),1200,-600.,600.,200,fGeneralInfo->GetMMPosPlaneZ(0)-50,fGeneralInfo->GetMMPosPlaneZ(1)+200);
      fHS->BookHisto2List("MMStudy",Form("zvsv_selecthit_tar_board%d_ev%d",j,i),1200,-600.,600.,200,fGeneralInfo->GetMMPosPlaneZ(0)-50,fGeneralInfo->GetMMPosPlaneZ(1)+200);
      fHS->BookHisto2List("MMStudy",Form("zvsv_board%d_ev%d_clus",j,i),1200,-600.,600.,200,fGeneralInfo->GetMMPosPlaneZ(0)-50,fGeneralInfo->GetMMPosPlaneZ(1)+200);
      fHS->BookHisto2List("MMStudy",Form("zvsv_board%d_ev%d_fit",j,i),1200,-600.,600.,200,fGeneralInfo->GetMMPosPlaneZ(0)-50,fGeneralInfo->GetMMPosPlaneZ(1)+200);
    }
    fHS->BookHistoList("MMStudy",Form("seedslope_ev%d",i),4000,-0.2,0.2);// dv/dz. dv ~ 300, dz ~ 2400
  }

  
  fHS->BookHisto2List("MMStudy","MM_Nclus_vs_NHits",100,0,3000.,500,0,500); 
  fHS->BookHistoList("MMStudy","NHitsPerCluster",100,0,100.);
  fHS->BookHistoList("MMStudy","NJumpsPerCluster",100,0,100.);
  fHS->BookHisto2List("MMStudy","NClustersPerBoard",16,-0.5,15.5,100,0,100.);
  fHS->BookHistoList("MMStudy","HitCharge",500,-10e3,800e3);
  fHS->BookHistoList("MMStudy","ClusterCharge",500,-10e3,10e6);
  for (int view = 0; view < 2; view++) {
    fHS->BookHistoList("MMStudy",Form("PositionAtECal_view%d",view)  ,500,-500,500.);
    fHS->BookHistoList("MMStudy",Form("PositionAtTarget_view%d",view),500,-500,500.);
    fHS->BookHisto2List("MMStudy",Form("PositionAtECalVsAtTarget_view%d",view),500,-500,500.,500,-500,500.);
  }

  for (int i = 0; i< 4; i++) { // strip number bins: 1-2, 3, 4, 5+
    fHS->BookHisto2List("MMStudy",Form("MatchingPlotSlope_stripBin%d",i),1000,-1.,1.,1000,-1.,1.);
  }

  
  for (int side = 0; side < 2; side++) {
    for (int view = 0; view < 2; view++) {
      for (int othview = 0; othview < 2; othview++) {
	fHS->BookHisto2List("MMStudy",Form("MatchingPlotNStrips_side%d_view%d_otherview%d",side,view,othview),30,0.,30.,30,0,30);
	for (int i = 0; i< 4; i++) { // strip number bins: 1-2, 3, 4, 5+
	  fHS->BookHisto2List("MMStudy",Form("MatchingPlotSlope_side%d_view%d_otherview%d_stripBin%d",side,view,othview,i),1000,-1.,1.,1000,-1.,1.);
	  fHS->BookHisto2List("MMStudy",Form("MatchingPlot_side%d_view%d_otherview%d_stripBin%d",side,view,othview,i),5000,-500.,500.,1000,-5000,5000);
	  fHS->BookHisto2List("MMStudy",Form("MatchingPlotDZ_side%d_view%d_otherview%d_stripBin%d",side,view,othview,i),5000,-500.,500.,1000,-5000,5000);
	}
      }
    }
  }
  fHS->BookHisto2List("MMStudy",Form("Chi2OverNdfVsNdf"),30,0.,30.,100,0,100);
  fHS->BookHisto2List("MMStudy",Form("ResVsNdf"),30,0.,30.,100,-100,100);
  return true;
}

Bool_t MMStudy::Process(){
//
//
//
//
//
//
// Geometrical afferent positions are defined by the same values of side, view, otherview but different planes
// 
  
// boardSN plane Layer side  view otherview hole offset stripid_orig
// 0       0     0     0     0    0         6    0      1-256
// 1       0     0     1     0    0         6    256    257-512
// 2       0     1     0     0    1         6    0      0-255
// 3       0     1     1     0    1         6    256    256-511
// 4       0     2     0     1    1         1    0      0-255
// 5       0     2     1     1    1         1    256    256-511
// 6       0     3     0     1    0         1    0      0-255
// 7       0     3     1     1    0         1    256    256-511
// 8       1     4     0     0    0         1    0      0-255
// 9       1     4     1     0    0         1    256    256-511
// 10      1     5     0     0    1         1    0      0-255
// 11      1     5     1     0    1         1    256    256-511
// 12      1     6     0     1    1         6    0      0-255
// 13      1     6     1     1    1         6    256    256-511
// 14      1     7     0     1    0         6    0      0-255
// 15      1     7     1     1    0         6    256    256-511

// corresponding boards
// BoardSN: 0 <--> 8
//          1 <--> 9
//          2 <--> 10
//....


  
// layer 0,1 -> y layer/2 = 0 
// layer 2,3 -> x layer/2 = 1
// layer 4,5 -> y layer/2 = 2
// layer 6,7 -> x layer/2 = 3
// layer 0,1 large hole
// layer 2,3 little hole
// layer 4,5 little hole
// layer 6,7 large hole

  if (ECalSel::GetInstance()->getNECalEvents() == 0) return kFALSE;

  
  TRecoVClusCollection* ECal_clEvent = fEvent->ECalRecoCl;
  std::vector<int> cluIndices;
  for (int i=0; i< ECalSel::GetInstance()->getNECalEvents(); i++){
    ECalSelEvent* selEvent = ECalSel::GetInstance()->getECalEvent(i);
    if (selEvent->flagEv != ev_gg) continue;
    for (int h1 = 0; h1 < 2; h1++) {
      TRecoVCluster* tempClu = ECal_clEvent->Element(selEvent->indexECal[h1]);
      fHS->FillHisto2List("MMStudy",Form("ECalSelClusters_yvsx_ev%d",fEventCounter),tempClu->GetPosition().X(),tempClu->GetPosition().Y(),tempClu->GetEnergy());
      cluIndices.push_back(selEvent->indexECal[h1]);
    }
  }

  if (cluIndices.size() == 0) return kFALSE; // cluster pairs

  
  vector<tracklet> tracklets[2][8]; // plane wise, side/view/otherview packed
  for (int i=0; i<2; i++){
    for (int j=0; j<8; j++){
      tracklets[i][j].clear();
    }
  }
  
  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  //long long int fTimeStamp =(long long int) fEvent->RecoEvent->GetEventTime().GetSec();
  if(trigMask & (1 << 0)) {
  }


  int nclus = fEvent->MMRecoCl->GetNElements();
  int nhits = fEvent->MMRecoEvent->GetNHits();
  fHS->FillHisto2List("MMStudy","MM_Nclus_vs_NHits",nhits,nclus,1.);

  // 
  double dzclu = (GeneralInfo::GetInstance()->GetCOG().Z()-GeneralInfo::GetInstance()->GetTargetPos().Z()); // distance cluster target in z
  double z_cluster = GeneralInfo::GetInstance()->GetCOG().Z();
  double z_target = GeneralInfo::GetInstance()->GetTargetPos().Z();
  TVector2 refpoint[2];// one per view
  for (int i=0; i<2; i++) refpoint[i].Set(GeneralInfo::GetInstance()->GetTargetPos()[1-i], GeneralInfo::GetInstance()->GetTargetPos().Z()); // reference points on the target {y,z} or {x,y} if i = 0 or 1


  // preselect hits using clusters in the ECal
  vector<vector<int>> preselectedHits; // one list for each ecal cluster

  // calo cluster loop 
  
  for (uint q = 0; q < cluIndices.size(); q++){
    TRecoVCluster* tempClu = ECal_clEvent->Element((int)cluIndices.at(q));
    //    double t0chamber = (tempClu->GetTime()+440.+31.); // manual fit
    double t0chamber = (tempClu->GetTime()+440.+60.); // manual fit

    // evaluate directions in y vs z and x vs z from ip to ECal cluster
    double dvdzclu[2] = {
      (tempClu->GetPosition()[1]-GeneralInfo::GetInstance()->GetTargetPos()[1])/dzclu, 
      (tempClu->GetPosition()[0]-GeneralInfo::GetInstance()->GetTargetPos()[0])/dzclu}; // dy/dz, or dx/dz
    TVector2 lambda[2];// one vector per view
    for (int i=0; i<2; i++){
      lambda[i].Set(dvdzclu[i],1.);
      lambda[i] *= (1./lambda[i].Mod()); // direction of the cluster {dvdz/sqrt(1+dvdz^2), 1/sqrt(1+dv/dz^2)}
    }

    // evaluate possible boards
    int admittedBoards[2] = {-1,-1};
    if (tempClu->GetPosition().X() > 0 && tempClu->GetPosition().Y() > 0){
      admittedBoards[0] = 3;
      admittedBoards[1] = 5;
    }
    else if (tempClu->GetPosition().X() > 0 && tempClu->GetPosition().Y() < 0){
      admittedBoards[0] = 2;
      admittedBoards[1] = 7;
    }
    else if (tempClu->GetPosition().X() < 0 && tempClu->GetPosition().Y() > 0){
      admittedBoards[0] = 1;
      admittedBoards[1] = 4;
    }
    else {
      admittedBoards[0] = 0;
      admittedBoards[1] = 6;
    }
    // hit loop
    vector<int> preselectedHitsPerClus;
    
    for (uint i = 0; i < nhits; i++){
      TRecoVHit* hit = fEvent->MMRecoEvent->Hit(i);
      MMchInfo mmi = fGeneralInfo->DecodeMMChannel(hit->GetChannelId());

      if (mmi.bdid != admittedBoards[mmi.view] && mmi.bdid != (admittedBoards[mmi.view]+8)) continue; // only consider possible boards on both planes
      
      double z = mmi.verse*(hit->GetTime()-t0chamber)*0.105 + fGeneralInfo->GetMMPosPlaneZ(mmi.bdid/8) ; // vd = 0.105 maybe in future could calibrate it
      double v = (mmi.view == 0? hit->GetPosition().Y() : hit->GetPosition().X());
      TVector2 point(v,z);
      TVector2 distance = point;
      distance -= refpoint[mmi.view]; // make direction of hit wrt target
      double projection = distance*lambda[mmi.view]; // project direction of hit wrt target onto the line joining cluster and target position
      TVector2 parallelDist = lambda[mmi.view];
      parallelDist *= projection;
      distance -= parallelDist ; // ortogonal distance point to line between hit and line joining cluster and target                  
      fHS->FillHisto2List("MMStudy",Form("cluDistance"),distance.Mod(),v-tempClu->GetPosition()[1-mmi.view],1.);
      if(distance.Mod() < 30) {
	preselectedHitsPerClus.push_back(i);
	//	std::cout << "Presel hit " << preselectedHitsPerClus.at(preselectedHitsPerClus.size()-1) << " dist = " << distance.Mod() << " v = " << v << " clus_v = " << tempClu->GetPosition()[1-mmi.view] << " dv = " << v-tempClu->GetPosition()[1-mmi.view] << " cluster " << q << endl;
      }
    }
    preselectedHits.push_back(preselectedHitsPerClus);
    //    std::cout << "Number of preselected hits, cluster " << q << " / " << cluIndices.size() << " = " << preselectedHitsPerClus.size() << " / " << nhits << " i.e." << preselectedHits.at(q).size() << std::endl;
    fHS->FillHistoList("MMStudy",Form("nPreselectedHitsPerECalClus"),preselectedHitsPerClus.size(),1.);

  }
  
  
  // loop on preselected hits and create clusters


  const int maxNumberOfHitsPerEvent = 4096; // it might be changed in case of multihit reco
  int isUsed[maxNumberOfHitsPerEvent];
  for (int i=0; i<maxNumberOfHitsPerEvent; i++) isUsed[i] = -1; // index of the cluster in which the hit is present

  const double cluHitRadius = 3.6; // 3*1.2 mm which means 3 strips
  vector<mmcluster> mmclusters;

  const double dzmin_isol = 1;// [mm], only remove same z "strips"
  for (uint q = 0; q < cluIndices.size(); q++){
    if (preselectedHits.at(q).size() == 0) continue; 
    TRecoVCluster* tempClu = ECal_clEvent->Element((int)cluIndices.at(q));
    int admittedBoards[2] = {-1,-1};
    if (tempClu->GetPosition().X() > 0 && tempClu->GetPosition().Y() > 0){
      admittedBoards[0] = 3; // view 0
      admittedBoards[1] = 5; // view 1
    }
    else if (tempClu->GetPosition().X() > 0 && tempClu->GetPosition().Y() < 0){
      admittedBoards[0] = 2; // view 0
      admittedBoards[1] = 7; // view 1
    }
    else if (tempClu->GetPosition().X() < 0 && tempClu->GetPosition().Y() > 0){
      admittedBoards[0] = 1;
      admittedBoards[1] = 4;
    }
    else {
      admittedBoards[0] = 0;
      admittedBoards[1] = 6;
    }
    double t0chamber = (tempClu->GetTime()+440.+60.); // manual fit
    int firstHitPerPlane[2] = {-1,-1};
    

    
    for (uint ii = 0; ii < (int) preselectedHits.at(q).size(); ii++){ // seed loop
      int i = preselectedHits.at(q).at(ii);
      if (i >= maxNumberOfHitsPerEvent) {
	std::cout << "Max number of hits reached " << i << " > " << maxNumberOfHitsPerEvent << std::endl;
	continue;
      }
      if (isUsed[i] >=0) continue;

      
      TRecoVHit* hit_i = fEvent->MMRecoEvent->Hit(i);
      MMchInfo mmi = fGeneralInfo->DecodeMMChannel(hit_i->GetChannelId());

      if (mmi.bdid != admittedBoards[mmi.view] && mmi.bdid != (admittedBoards[mmi.view]+8)) continue; // only consider possible boards on both planes

      double z = mmi.verse*(hit_i->GetTime()-t0chamber)*0.105 + fGeneralInfo->GetMMPosPlaneZ(mmi.bdid/8) ; // vd = 0.105 maybe in future could calibrate it
      double v = (mmi.view == 0? hit_i->GetPosition().Y() : hit_i->GetPosition().X());

      // check isolation of the seed
            
      bool isol = kTRUE;
      if (i-1 >= 0) { // the previous hit exists
	TRecoVHit* hit_prev = fEvent->MMRecoEvent->Hit(i-1);
	MMchInfo mmi_prev = fGeneralInfo->DecodeMMChannel(hit_prev->GetChannelId());
	if (mmi_prev.bdid == mmi.bdid) {
	  double v_prev = (mmi_prev.view == 0? hit_prev->GetPosition().Y() : hit_prev->GetPosition().X());
	  double z_prev = mmi_prev.verse*(hit_prev->GetTime()-t0chamber)*0.105 + fGeneralInfo->GetMMPosPlaneZ(mmi_prev.bdid/8) ; // vd = 0.105 maybe in future could calibrate it	  
	  fHS->FillHisto2List("MMStudy",Form("isolPlotPrev"),v_prev-v,z_prev-z,1.);
	  if (TMath::Abs(v_prev-v) < 1.5 && TMath::Abs(z_prev-z) < dzmin_isol) isol = kFALSE;
	}
      }
      if (i+1 < fEvent->MMRecoEvent->GetNHits()) { // the previous hit exists
	TRecoVHit* hit_foll = fEvent->MMRecoEvent->Hit(i+1);
	MMchInfo mmi_foll = fGeneralInfo->DecodeMMChannel(hit_foll->GetChannelId());
	if (mmi_foll.bdid == mmi.bdid) {
	  double v_foll = (mmi_foll.view == 0? hit_foll->GetPosition().Y() : hit_foll->GetPosition().X());
	  double z_foll = mmi_foll.verse*(hit_foll->GetTime()-t0chamber)*0.105 + fGeneralInfo->GetMMPosPlaneZ(mmi_foll.bdid/8) ; // vd = 0.105 maybe in future could calibrate it	  
	  fHS->FillHisto2List("MMStudy",Form("isolPlotFoll"),v_foll-v,z_foll-z,1.);
	  if (TMath::Abs(v_foll-v) < 1.5 && TMath::Abs(z_foll-z) < dzmin_isol) isol = kFALSE;
	}
      }
      if (!isol) continue; // isolation condition
      
      TVector2 point_i(v,z);
      TVector2 lambdaIP = point_i;
      lambdaIP -= refpoint[mmi.view]; // make direction of hit wrt target
      lambdaIP *= (1./lambdaIP.Mod()); // direction of the hit {dvdz/sqrt(1+dvdz^2), 1/sqrt(1+dv/dz^2)}

      
      mmcluster mmclus;
      mmclus.ecalclusindex = cluIndices.at(q);
      mmclus.mmhitsindex.push_back(i);
      mmclus.isolationflag.push_back(kTRUE);
      mmclus.nhitPerPlane[mmi.bdid/8] = 1;
      mmclus.nhitPerPlane[1-mmi.bdid/8] = 0;
      mmclus.seedslope = (point_i.X()-refpoint[mmi.view].X())/(point_i.Y()-refpoint[mmi.view].Y());


      isUsed[i] = q;
      double zold = z;
      double vold = v;
      for (uint jj = 0; jj < (int) preselectedHits.at(q).size(); jj++){
	int j = preselectedHits.at(q).at(jj);
	if (i==j) continue;
	if (j >= maxNumberOfHitsPerEvent) {
	  std::cout << "Max number of j hits reached " << j << " > " << maxNumberOfHitsPerEvent << std::endl;
	  continue;
	}
	if (isUsed[j] >=0) continue;

	TRecoVHit* hit_j = fEvent->MMRecoEvent->Hit(j);
	MMchInfo mmj = fGeneralInfo->DecodeMMChannel(hit_j->GetChannelId());
	if (mmj.view != mmi.view) continue;
	if (mmj.bdid != admittedBoards[mmi.view] && mmj.bdid != (admittedBoards[mmi.view]+8)) continue; // only consider possible boards on both planes

//	int deltaboard = TMath::Abs(mmi.bdid-mmj.bdid);
//	std::cout << "Analysis of hit " << j << " wrt seed " << i << " / " << preselectedHits.at(q).size() << " cluster " << q  << " deltaBoard = " << deltaboard << " " << mmi.bdid << " vs " << mmj.bdid << " " << mmi.view << " " << mmj.view << std::endl;
//	if (deltaboard != 0 && deltaboard != 8) continue; // ensures that the view is the same
	
	double z_j = mmj.verse*(hit_j->GetTime()-t0chamber)*0.105 + fGeneralInfo->GetMMPosPlaneZ(mmj.bdid/8) ; // vd = 0.105 maybe in future could calibrate it
	double v_j = (mmj.view == 0? hit_j->GetPosition().Y() : hit_j->GetPosition().X());
	TVector2 point_j(v_j,z_j);
	point_j -= refpoint[mmj.view]; // make direction of hit wrt target

	// point-straight line distance: P
	double straightDistance = (point_j - (point_j*lambdaIP)*lambdaIP).Mod();

	// evaluate the expected z of the present point: zexp = refpoint_z + dz/dv*(v_j-v_i), where dz/dv = (point_i.Y()-refpoint[mmi.view].Y()) / (point_i.X()-refpoint[mmi.view].X()) 
	double zexp = z + (point_i.Y()-refpoint[mmi.view].Y())/(point_i.X()-refpoint[mmi.view].X())*(v_j-v);
	fHS->FillHisto2List("MMStudy",Form("straightDistance"),straightDistance,v_j-v,1.);
	fHS->FillHisto2List("MMStudy",Form("straightDistanceDZ"),straightDistance,(z_j-zold),1.);
	fHS->FillHisto2List("MMStudy",Form("straightDistanceZMinusZExp"),straightDistance,(z_j-zexp),1.);
	fHS->FillHisto2List("MMStudy",Form("straightDistanceDZDV"),straightDistance,(z_j-zold)/(v_j-vold),1.);

	//	std::cout << "Clus hit " << j << " vs " << i << " " << preselectedHits.at(q).size() << " dist = " << straightDistance << " v_j = " << v_j << " v_i = " << v << " dv = " << v_j-v << " cluster " << q << endl;
	if (straightDistance > cluHitRadius || TMath::Abs(z_j-zexp)>20) continue; // hit j enters in the cluster with seed i


	bool isol = kTRUE;
	if (j-1 >= 0) { // the previous hit exists
	  TRecoVHit* hit_prev = fEvent->MMRecoEvent->Hit(j-1);
	  MMchInfo mmj_prev = fGeneralInfo->DecodeMMChannel(hit_prev->GetChannelId());
	  if (mmj_prev.bdid == mmj.bdid) {
	    double v_prev = (mmj_prev.view == 0? hit_prev->GetPosition().Y() : hit_prev->GetPosition().X());
	    double z_prev = mmj_prev.verse*(hit_prev->GetTime()-t0chamber)*0.105 + fGeneralInfo->GetMMPosPlaneZ(mmj_prev.bdid/8) ; // vd = 0.105 maybe in future could calibrate it	  
	    fHS->FillHisto2List("MMStudy",Form("isolPlotPrev"),v_prev-v_j,z_prev-z_j,1.);
	    if (TMath::Abs(v_prev-v_j) < 1.5 && TMath::Abs(z_prev-z_j) < dzmin_isol) isol = kFALSE;
	  }
	}
	if (j+1 < fEvent->MMRecoEvent->GetNHits()) { // the previous hit exists
	  TRecoVHit* hit_foll = fEvent->MMRecoEvent->Hit(j+1);
	  MMchInfo mmj_foll = fGeneralInfo->DecodeMMChannel(hit_foll->GetChannelId());
	  if (mmj_foll.bdid == mmj.bdid) {
	    double v_foll = (mmj_foll.view == 0? hit_foll->GetPosition().Y() : hit_foll->GetPosition().X());
	    double z_foll = mmj_foll.verse*(hit_foll->GetTime()-t0chamber)*0.105 + fGeneralInfo->GetMMPosPlaneZ(mmj_foll.bdid/8) ; // vd = 0.105 maybe in future could calibrate it	  
	    fHS->FillHisto2List("MMStudy",Form("isolPlotFoll"),v_foll-v_j,z_foll-z_j,1.);
	    if (TMath::Abs(v_foll-v_j) < 1.5 && TMath::Abs(z_foll-z_j) < dzmin_isol) isol = kFALSE;
	  }
	}

	if (!isol) continue;

	// clear tracker hits
	fTracker[mmi.view]->Clear();
	// add all hits in the clusters to the tracker fit
	for (int k= 0; k < mmclus.mmhitsindex.size(); k++){
	  TRecoVHit* hit_k = fEvent->MMRecoEvent->Hit(mmclus.mmhitsindex.at(k));
	  MMchInfo mmi_k = fGeneralInfo->DecodeMMChannel(hit_k->GetChannelId());
	  double z_k = mmi_k.verse*(hit_k->GetTime()-t0chamber)*0.105 + fGeneralInfo->GetMMPosPlaneZ(mmi_k.bdid/8) ; // vd = 0.105 maybe in future could calibrate it	  
	  
	  TVector3 newpos(hit_k->GetPosition().X(),hit_k->GetPosition().Y(),z_k);
	  hit_k->SetPosition(newpos);
	  fTracker[mmi.view]->AddHit(hit_k);
	}
	// add the candidate hit to the tracker fit
	TVector3 newpos(hit_j->GetPosition().X(),hit_j->GetPosition().Y(),z_j);
	hit_j->SetPosition(newpos);
	fTracker[mmi.view]->AddHit(hit_j);
	
	// initialize the fit with the IP information
	fTracker[mmi.view]->InitFit(GeneralInfo::GetInstance()->GetTargetPos().X(),GeneralInfo::GetInstance()->GetTargetPos().Y(),GeneralInfo::GetInstance()->GetTargetPos().Z());
	bool goodfit = fTracker[mmi.view]->MakeFit();
	if (goodfit && TMath::Abs(fTracker[mmi.view]->GetFitResults(1-mmi.view)) < 350. && TMath::Abs(fTracker[mmi.view]->GetFitResults(3-mmi.view)) < 350.) {
	  double chi2p = TMath::Prob(fTracker[mmi.view]->GetFitChi2(),mmclus.mmhitsindex.size());// IP counts as 1, so it's IP + old hits + present hit -2 dof.

	  //	  std::cout << "Hits in cluster " << mmclus.mmhitsindex.size() << " result = " << fTracker[mmi.view]->GetFitResults(1-mmi.view) << " , " << fTracker[mmi.view]->GetFitResults(2-mmi.view) << " chi2 = " << fTracker[mmi.view]->GetFitChi2() << " p= " << chi2p << endl;
	  fHS->FillHistoList("MMStudy",Form("mmFitChi2"),chi2p,mmclus.mmhitsindex.size());
	  if (chi2p > 0.2) {
	    mmclus.mmhitsindex.push_back(j);
	    mmclus.isolationflag.push_back(isol);
	    mmclus.nhitPerPlane[mmj.bdid/8]++;
	    mmclus.traco.chi2 = fTracker[mmi.view]->GetFitChi2();
	    for (int q = 0; q<4; q++) mmclus.traco.pars[q] = fTracker[mmi.view]->GetFitResults(q);
	    
	    double slopevsz = (fTracker[mmi.view]->GetFitResults(3-mmi.view) - fTracker[mmi.view]->GetFitResults(1-mmi.view))/(fGeneralInfo->GetMMPosPlaneZ(1)-fGeneralInfo->GetMMPosPlaneZ(0));// dv/dz      
	    double interatzmid = fTracker[mmi.view]->GetFitResults(1-mmi.view) + slopevsz*0.5*(fGeneralInfo->GetMMPosPlaneZ(1)-fGeneralInfo->GetMMPosPlaneZ(0)); // v0 + dv/dz*(z1-z0)/2
	    mmclus.traco.slope = slopevsz;
	    mmclus.traco.inter = interatzmid;
	    mmclus.traco.nstrips = mmclus.mmhitsindex.size();
	    
	    isUsed[j] = q;
	    zold = z_j;
	    vold = v_j;
	  }
	}
      }

      mmclusters.push_back(mmclus);
    }

  }

  // analysis of the clusters found
  
  for (uint q = 0; q < cluIndices.size(); q++){
    TRecoVCluster* tempClu = ECal_clEvent->Element((int)cluIndices.at(q));
    double t0chamber = (tempClu->GetTime()+440.+60.); // manual fit
    int nclusPerECalClus = 0;
    for (uint j = 0; j < mmclusters.size(); j++){
      if (mmclusters.at(j).ecalclusindex != cluIndices.at(q)) continue;
      nclusPerECalClus++;
      // control plots
      fHS->FillHistoList("MMStudy",Form("nMMHitsInMMClusters"),mmclusters.at(j).mmhitsindex.size(),1.);
      fHS->FillHisto2List("MMStudy",Form("nMMHitsPerPlaneInMMClusters"),mmclusters.at(j).nhitPerPlane[0],mmclusters.at(j).nhitPerPlane[1],1.);

      int nisol[2] = {0,0};
      for (int i = 0; i<mmclusters.at(j).mmhitsindex.size(); i++) {
	TRecoVHit* hit_i = fEvent->MMRecoEvent->Hit(mmclusters.at(j).mmhitsindex.at(i));
	MMchInfo mmi = fGeneralInfo->DecodeMMChannel(hit_i->GetChannelId());
	if (mmclusters.at(j).isolationflag.at(i)) nisol[mmi.bdid/8]++;
      }
      fHS->FillHisto2List("MMStudy",Form("nMMIsolHitsPerPlaneInMMClusters"),nisol[0],nisol[1],1.);
      
      if (mmclusters.at(j).mmhitsindex.size() < 4) continue; // at least 3 hits

      for (int i = 0; i<mmclusters.at(j).mmhitsindex.size(); i++) {
	TRecoVHit* hit_i = fEvent->MMRecoEvent->Hit(mmclusters.at(j).mmhitsindex.at(i));
	MMchInfo mmi = fGeneralInfo->DecodeMMChannel(hit_i->GetChannelId());
	double z = mmi.verse*(hit_i->GetTime()-t0chamber)*0.105 + fGeneralInfo->GetMMPosPlaneZ(mmi.bdid/8) ; // vd = 0.105 maybe in future could calibrate it
	double v = (mmi.view == 0? hit_i->GetPosition().Y() : hit_i->GetPosition().X());
	fHS->FillHisto2List("MMStudy",Form("zvsv_hitforclus_board%d_ev%d",mmi.bdid,fEventCounter),v,z,(j+1)*1000.);
      }

      if (nisol[0]+nisol[1] >=3 ) {	

	
	int viewloc = 0;
	for (int i = 0; i<mmclusters.at(j).mmhitsindex.size(); i++) {
	  if (!mmclusters.at(j).isolationflag.at(i)) continue;
	  TRecoVHit* hit_i = fEvent->MMRecoEvent->Hit(mmclusters.at(j).mmhitsindex.at(i));
	  MMchInfo mmi = fGeneralInfo->DecodeMMChannel(hit_i->GetChannelId());
	  double z = mmi.verse*(hit_i->GetTime()-t0chamber)*0.105 + fGeneralInfo->GetMMPosPlaneZ(mmi.bdid/8) ; // vd = 0.105 maybe in future could calibrate it
	  double v = (mmi.view == 0? hit_i->GetPosition().Y() : hit_i->GetPosition().X());

	  // clear tracker hits
	  if (i==0) {
	    fTracker[mmi.view]->Clear();
	    viewloc = mmi.view;
	  }
	  // add hits to the tracker
	  TVector3 newpos(hit_i->GetPosition().X(),hit_i->GetPosition().Y(),z);
	  hit_i->SetPosition(newpos);
	  fTracker[mmi.view]->AddHit(hit_i);


	  fHS->FillHisto2List("MMStudy",Form("zvsv_isolhitforclus_board%d_ev%d",mmi.bdid,fEventCounter),v,z,(j+1)*1000.);
	  double vexp = mmclusters.at(j).traco.pars[1-mmi.view] + mmclusters.at(j).traco.slope*(z-fGeneralInfo->GetMMPosPlaneZ(0)); // v_hit on plane0 + dv/dz_fit (z_hit-z0)
	  double vres = v - vexp;
	  double zexp = fGeneralInfo->GetMMPosPlaneZ(0) + 1/mmclusters.at(j).traco.slope*(v-mmclusters.at(j).traco.pars[1-mmi.view]); // z0 + 1/(dv/dz) * (v_hit - v0)
	  double zres = z - zexp;
	  fHS->FillHisto2List("MMStudy",Form("residual_nhits%d",TMath::Min(10,(int)mmclusters.at(j).mmhitsindex.size())),vres,zres);	  
	}

	fTracker[viewloc]->InitFit(); // init fit without ip constraint	
	bool goodfit = fTracker[viewloc]->MakeFit();
	for (int q = 0; q<4; q++) mmclusters.at(j).traconoip.pars[q] = fTracker[viewloc]->GetFitResults(q);
	double slopevsz = (fTracker[viewloc]->GetFitResults(3-viewloc) - fTracker[viewloc]->GetFitResults(1-viewloc))/(fGeneralInfo->GetMMPosPlaneZ(1)-fGeneralInfo->GetMMPosPlaneZ(0));// dv/dz      
	double interatzmid = fTracker[viewloc]->GetFitResults(1-viewloc) + slopevsz*0.5*(fGeneralInfo->GetMMPosPlaneZ(1)-fGeneralInfo->GetMMPosPlaneZ(0)); // v0 + dv/dz*(z1-z0)/2
	mmclusters.at(j).traconoip.chi2 = fTracker[viewloc]->GetFitChi2();
	mmclusters.at(j).traconoip.slope = slopevsz;
	mmclusters.at(j).traconoip.inter = interatzmid;
	mmclusters.at(j).traconoip.nstrips = mmclusters.at(j).mmhitsindex.size();
	
	fHS->FillHistoList("MMStudy",Form("seedslope_ev%d",fEventCounter),mmclusters.at(j).seedslope,(j+1)*1000.);	
      }


// plot of residuals for the noip fit
      
      for (int i = 0; i<mmclusters.at(j).mmhitsindex.size(); i++) {
	if (!mmclusters.at(j).isolationflag.at(i)) continue;
	TRecoVHit* hit_i = fEvent->MMRecoEvent->Hit(mmclusters.at(j).mmhitsindex.at(i));
	MMchInfo mmi = fGeneralInfo->DecodeMMChannel(hit_i->GetChannelId());
	double z = mmi.verse*(hit_i->GetTime()-t0chamber)*0.105 + fGeneralInfo->GetMMPosPlaneZ(mmi.bdid/8) ; // vd = 0.105 maybe in future could calibrate it
	double v = (mmi.view == 0? hit_i->GetPosition().Y() : hit_i->GetPosition().X());
	double vexp = mmclusters.at(j).traconoip.pars[1-mmi.view] + mmclusters.at(j).traconoip.slope*(z-fGeneralInfo->GetMMPosPlaneZ(0)); // v_hit on plane0 + dv/dz_fit (z_hit-z0)
	double vres = v - vexp;
	double zexp = fGeneralInfo->GetMMPosPlaneZ(0) + 1/mmclusters.at(j).traconoip.slope*(v-mmclusters.at(j).traconoip.pars[1-mmi.view]); // z0 + 1/(dv/dz) * (v_hit - v0)
	double zres = z - zexp;
	fHS->FillHisto2List("MMStudy",Form("residual_noipfit_nhits%d",TMath::Min(10,(int)mmclusters.at(j).mmhitsindex.size())),vres,zres);	  
      }
      
    }
    // control plots
    fHS->FillHistoList("MMStudy",Form("nMMClustersPerECalCluster"),nclusPerECalClus,1.);
  }
  
  
  // calo cluster loop 
  for (uint q = 0; q < cluIndices.size(); q++){
    TRecoVCluster* tempClu = ECal_clEvent->Element((int)cluIndices.at(q));
    //    double t0chamber = (tempClu->GetTime()+440.+31.); // manual fit
    double t0chamber = (tempClu->GetTime()+440.+60.); // manual fit

    double dvdzclu[2] = {
      (tempClu->GetPosition()[1]-GeneralInfo::GetInstance()->GetTargetPos()[1])/dzclu, 
      (tempClu->GetPosition()[0]-GeneralInfo::GetInstance()->GetTargetPos()[0])/dzclu}; // dy/dz, or dx/dz
    TVector2 lambda[2];// one vector per view
    for (int i=0; i<2; i++){
      lambda[i].Set(dvdzclu[i],1.);
      lambda[i] *= (1./lambda[i].Mod()); // direction of the cluster {dvdz/sqrt(1+dvdz^2), 1/sqrt(1+dv/dz^2)}
    }

    TVector2 lambda_pair[2];// one vector per view
    TVector2 lambda_tar[2];// one vector per view                                                                                                 
    int ncloseHits[16] = {0};

    vector<TRecoVHit*> hit_for_cluster;
    int next_hit_counter=0.;
    int hiterator=0.;
    double slope_good_pair=0., inter_good_pair=0.;

    vector<int> doublets[16]; // every board has a vector of hit indices, the pair is formed with i and i+1
    
    // hit loop
    for (uint i = 0; i < nhits; i++){
      TRecoVHit* hit = fEvent->MMRecoEvent->Hit(i);
      MMchInfo mmi = fGeneralInfo->DecodeMMChannel(hit->GetChannelId());
      double z = mmi.verse*(hit->GetTime()-t0chamber)*0.105 + fGeneralInfo->GetMMPosPlaneZ(mmi.bdid/8) ; // vd = 0.105 maybe in future could calibrate it
      double v = (mmi.view == 0? hit->GetPosition().Y() : hit->GetPosition().X());
      TVector2 point(v,z);
      TVector2 distance = point;
      distance -= refpoint[mmi.view]; // make direction of hit wrt target
      double projection = distance*lambda[mmi.view]; // project direction of hit wrt target onto the line joining cluster and target position
      TVector2 parallelDist = lambda[mmi.view];
      parallelDist *= projection;
      distance -= parallelDist ; // ortogonal distance point to line between hit and line joining cluster and target                  

      fHS->FillHisto2List("MMStudy",Form("DistanceCluHit_z_vs_v"),distance.X(),distance.Y(),1.);
      fHS->FillHisto2List("MMStudy",Form("hough_transform_cluster_board%d_ev%d",mmi.bdid,fEventCounter),lambda[1-mmi.view].X(),tempClu->GetPosition()[1-mmi.view],1.);

      if (distance.Mod() < 5) ncloseHits[mmi.bdid]++;

      //compute slope between pair of consecutive hits
      if(i<nhits-1 && distance.Mod() < 30){
	TRecoVHit* hit_1 = fEvent->MMRecoEvent->Hit(i+1);
	MMchInfo mmi_1 = fGeneralInfo->DecodeMMChannel(hit_1->GetChannelId());
	double z_1 = mmi_1.verse*(hit_1->GetTime()-t0chamber)*0.105 + fGeneralInfo->GetMMPosPlaneZ(mmi_1.bdid/8) ;
	double v_1 = (mmi_1.view == 0? hit_1->GetPosition().Y() : hit_1->GetPosition().X());
	double dvdzpair;
	if(mmi_1.view == mmi.view){
	  dvdzpair = (v_1-v)/(z_1-z);
	}
	else continue;

	lambda_pair[mmi.view].Set(dvdzpair,1.);
	lambda_pair[mmi.view] *= (1./lambda_pair[mmi.view].Mod()); // direction of the track between hits in a pair

	double diff_pairs_track = lambda_pair[mmi.view].X()-lambda[mmi.view].X();
	double q0 = v - dvdzpair*z; //constant term
	double v_ref = dvdzpair*(z_cluster) + q0;
	double diff_clus_point = tempClu->GetPosition()[1-mmi.view] - v_ref;

	///Pointing to the target from the hit
	double dvdztar = (v-GeneralInfo::GetInstance()->GetTargetPos()[1-mmi.view])/(z-z_target);

	lambda_tar[mmi.view].Set(dvdztar,1.);
	lambda_tar[mmi.view] *= (1./lambda_tar[mmi.view].Mod()); // direction of the track

	double diff_pairs_target = lambda_pair[mmi.view].X()-lambda_tar[mmi.view].X();//slope pair - slope  point-target
	double q_tar     = v-dvdztar*z; //constant term point-target
	double v_ref_tar =   dvdztar*(z_cluster)+ q_tar; // ref point o point-target line @ calo
	double diff_clus_point_tar = v_ref - v_ref_tar; //intercetta @ calo of pair - point-target

	if(fabs(diff_pairs_track)<0.05 && fabs(diff_clus_point)<30.){
	  doublets[mmi.bdid].push_back(i);

	  fSaveEvent = true;
	  //WARNING: the second hit is filled twice if it pass the next step!!!
	  fHS->FillHisto2List("MMStudy",Form("zvsv_selecthit_board%d_ev%d",mmi.bdid,fEventCounter),v,z,hit->GetEnergy());
	  fHS->FillHisto2List("MMStudy",Form("zvsv_selecthit_board%d_ev%d",mmi.bdid,fEventCounter),v_1,z_1,hit_1->GetEnergy());
	  fHS->FillHisto2List("MMStudy",Form("hough_transform_board%d_ev%d",mmi.bdid,fEventCounter),lambda_pair[mmi.view].X(),v_ref,1.);
	  fHS->FillHisto2List("MMStudy",Form("diffslopeinter_pairs_track_board%d",mmi.bdid),diff_pairs_track,diff_clus_point,1.);

	  ////////ATTEMPT TO ALIGN HITS///////////
	  // if(hiterator==0){	  
	  //   slope_good_pair = lambda_pair[mmi.view].X();
	  //   inter_good_pair = v_ref;
	  //   hit_for_cluster.push_back(hit);
	  //   hit_for_cluster.push_back(hit_1);
	  //   next_hit_counter++;
	  //   hiterator++; 
	  //   continue;
	  // }
	  // else{
	  //   if(fabs(lambda_pair[mmi.view].X()-slope_good_pair)<0.05 && fabs(v_ref-inter_good_pair)<30.){
	  //     if((hiterator-next_hit_counter)<1){
	  // 	hit_for_cluster.push_back(hit_1);
	  // 	slope_good_pair = lambda_pair[mmi.view].X();
	  // 	inter_good_pair = v_ref;
	  //     }
	  //     else{
	  // 	hit_for_cluster.push_back(hit);
	  // 	hit_for_cluster.push_back(hit_1);
	  // 	slope_good_pair = lambda_pair[mmi.view].X();
	  // 	inter_good_pair = v_ref;
	  //     }
	  //   }
	  //}
	  //hiterator++; 
	}
	if(fabs(diff_pairs_target)<0.05 && fabs(diff_clus_point_tar)<30.){
	  fHS->FillHisto2List("MMStudy",Form("zvsv_selecthit_tar_board%d_ev%d",mmi.bdid,fEventCounter),v,z,hit->GetEnergy());
	  fHS->FillHisto2List("MMStudy",Form("zvsv_selecthit_tar_board%d_ev%d",mmi.bdid,fEventCounter),v_1,z_1,hit_1->GetEnergy());
	  fHS->FillHisto2List("MMStudy",Form("hough_transform_tar_board%d_ev%d",mmi.bdid,fEventCounter),lambda_pair[mmi.view].X(),v_ref,1.);
	  fHS->FillHisto2List("MMStudy",Form("diffslopeinter_pairs_target_board%d",mmi.bdid),diff_pairs_target,diff_clus_point_tar,1.);      
	}
	fHS->FillHisto2List("MMStudy",Form("pairs_vs_track_board%d",mmi.bdid),lambda_pair[mmi.view].X(),lambda[mmi.view].X(),1.);
	fHS->FillHistoList("MMStudy",Form("diff_pairs_track_board%d",mmi.bdid),lambda_pair[mmi.view].X()-lambda[mmi.view].X(),1.);      
      }
    }//close hit loop
    
    // for (int j=0; j<hit_for_cluster.size(); j++){
    //   TRecoVHit* hit = hit_for_cluster.at(j);
    //   MMchInfo mmi = fGeneralInfo->DecodeMMChannel(hit->GetChannelId());
      
    //   double z = mmi.verse*hit->GetTime()*0.105 + hit->GetPosition().Z() ;
    //   double v = (mmi.view == 0? hit->GetPosition().Y() : hit->GetPosition().X());
    //   if (fEventCounter  < 100 && fSaveEvent) {
    // 	fHS->FillHisto2List("MMStudy",Form("zvsv_hitforclus_board%d_ev%d",mmi.bdid,fEventCounter),v,z,hit->GetEnergy());
    //   }	
    for (int j=0; j<8; j++){
      fHS->FillHisto2List("MMStudy",Form("NCloseHits"),ncloseHits[j],ncloseHits[8+j],1.);
      fHS->FillHisto2List("MMStudy",Form("NCloseDoublets"),doublets[j].size(),doublets[8+j].size(),1.);

      if (doublets[j].size() == 0 || doublets[j+8].size() == 0) continue;

      // try to match the two doublets
      
      for (uint p0=0; p0<doublets[j].size(); p0++) { // doublets on the plane 0
	int i0 = doublets[j].at(p0);
	int i1 = i0+1;

	TRecoVHit* hit0 = fEvent->MMRecoEvent->Hit(i0);
	MMchInfo mmi = fGeneralInfo->DecodeMMChannel(hit0->GetChannelId());
	double z0 = mmi.verse*(hit0->GetTime()-t0chamber)*0.105 + fGeneralInfo->GetMMPosPlaneZ(mmi.bdid/8) ; // vd = 0.105 maybe in future could calibrate it       
	double v0 = (mmi.view == 0? hit0->GetPosition().Y() : hit0->GetPosition().X());

	TRecoVHit* hit0_1 = fEvent->MMRecoEvent->Hit(i1);
	MMchInfo mmi_1 = fGeneralInfo->DecodeMMChannel(hit0_1->GetChannelId());
	double z0_1 = mmi_1.verse*(hit0_1->GetTime()-t0chamber)*0.105 + fGeneralInfo->GetMMPosPlaneZ(mmi_1.bdid/8) ;
	double v0_1 = (mmi_1.view == 0? hit0_1->GetPosition().Y() : hit0_1->GetPosition().X());
	double dvdzpair1 = (v0_1-v0)/(z0_1-z0);

	TVector2 lambda_pair1(dvdzpair1,1.);
	lambda_pair1 *= (1./lambda_pair1.Mod()); // direction of the track between hits in a pair

	double q0      = v0 - dvdzpair1*z0; //constant term v = q + dv/dz * z 
	double v_ref1 =     dvdzpair1*(z_cluster) + q0;

	for (uint p1=0; p1<doublets[j+8].size(); p1++) { // doublets on the plane 1
	  int j0 = doublets[j+8].at(p1);
	  int j1 = j0+1;
	  
	  TRecoVHit* hit1 = fEvent->MMRecoEvent->Hit(j0);
	  MMchInfo mmi = fGeneralInfo->DecodeMMChannel(hit1->GetChannelId());
	  double z1 = mmi.verse*(hit1->GetTime()-t0chamber)*0.105 + fGeneralInfo->GetMMPosPlaneZ(mmi.bdid/8) ; // vd = 0.105 maybe in future could calibrate it
	  double v1 = (mmi.view == 0? hit1->GetPosition().Y() : hit1->GetPosition().X());

	  TRecoVHit* hit1_1 = fEvent->MMRecoEvent->Hit(j1);
	  MMchInfo mmi_1 = fGeneralInfo->DecodeMMChannel(hit1_1->GetChannelId());
	  double z1_1 = mmi_1.verse*(hit1_1->GetTime()-t0chamber)*0.105 + fGeneralInfo->GetMMPosPlaneZ(mmi_1.bdid/8) ;
	  double v1_1 = (mmi_1.view == 0? hit1_1->GetPosition().Y() : hit1_1->GetPosition().X());
	  double dvdzpair2 = (v1_1-v1)/(z1_1-z1);

	  TVector2 lambda_pair2(dvdzpair2,1.);
	  lambda_pair2 *= (1./lambda_pair2.Mod()); // direction of the track between hits in a pair

	  double q1      = v1 - dvdzpair2*z1; //constant term
	  double v_ref2 =     dvdzpair2*(z_cluster) + q1;

	  fHS->FillHisto2List("MMStudy",Form("MatchingDoubletAngleVsDeltav_boardAll"),v_ref1-v_ref2,TMath::ASin(lambda_pair1.X()*lambda_pair2.Y()-lambda_pair1.Y()*lambda_pair2.X()));
	  fHS->FillHisto2List("MMStudy",Form("MatchingDoubletAngleVsDeltav_board%d",j),v_ref1-v_ref2,TMath::ASin(lambda_pair1.X()*lambda_pair2.Y()-lambda_pair1.Y()*lambda_pair2.X()));

	  // make a fit to the double doublet, also with the reference point of the IP at the target, fit also the DZ
	  fTrackerDZ[mmi.view]->Clear();
	  TVector3 newpos;

	  newpos.SetXYZ(hit0->GetPosition().X(),hit0->GetPosition().Y(),z0);
	  hit0->SetPosition(newpos);
	  fTrackerDZ[mmi.view]->AddHit(hit0);
	  
	  newpos.SetXYZ(hit0_1->GetPosition().X(),hit0_1->GetPosition().Y(),z0_1);
	  hit0_1->SetPosition(newpos);
	  fTrackerDZ[mmi.view]->AddHit(hit0_1);

	  newpos.SetXYZ(hit1->GetPosition().X(),hit1->GetPosition().Y(),z1);
	  hit1->SetPosition(newpos);
	  fTrackerDZ[mmi.view]->AddHit(hit1);

	  newpos.SetXYZ(hit1_1->GetPosition().X(),hit1_1->GetPosition().Y(),z1_1);
	  hit1_1->SetPosition(newpos);
	  fTrackerDZ[mmi.view]->AddHit(hit1_1);

	  fTrackerDZ[mmi.view]->InitFit(GeneralInfo::GetInstance()->GetTargetPos().X(),GeneralInfo::GetInstance()->GetTargetPos().Y(),GeneralInfo::GetInstance()->GetTargetPos().Z()); 
	  bool goodfit = fTrackerDZ[mmi.view]->MakeFit();
	  if (goodfit) { 
//	    std::cout << "********      Ev = " << fEvent->RecoEvent->GetEventNumber() << " fit done " << i0 << " " << i1 << " " << j0 << " " << j1 << "clus = " << q << " time = " <<
//	      tempClu->GetTime() << " dz = " << fTrackerDZ[mmi.view]->GetFitResults(4) << " fit params = " << fTrackerDZ[mmi.view]->GetFitResults(1-mmi.view)
//		      << " " << fTrackerDZ[mmi.view]->GetFitResults(1-mmi.view+2) << std::endl;
	    
	    fHS->FillHisto2List("MMStudy",Form("MatchingDoubletsTimeClu_vs_DZ_boardAll"),fTrackerDZ[mmi.view]->GetFitResults(4),tempClu->GetTime());
	    fHS->FillHisto2List("MMStudy",Form("MatchingDoubletsFitChi2_vs_DZ_boardAll"),fTrackerDZ[mmi.view]->GetFitResults(4),fTrackerDZ[mmi.view]->GetFitChi2());
	  }
	}
      }
    }    
  }  // cluster loop
  //}

  
  //  std::cout << "********      Ev = " << fEvent->RecoEvent->GetEventNumber() << " Nclus " << nclus << "  ********** NHits " << nhits << std::endl;
  TRecoVCluster* tempClu = ECal_clEvent->Element((int)cluIndices.at(0));
  double t0chamber2 = (tempClu->GetTime()+440.+60.); // manual fit

  // loop micromegas cluster
  for (int i=0; i<nclus; i++){
    TRecoVCluster* clus = fEvent->MMRecoCl->Element(i);
    int nhitsPerClus = clus->GetNHitsInClus();
    
    //    if (nhitsPerClus < 5) continue;

    MMchInfo mmi;
    for (int j=0; j<nhitsPerClus; j++){
      TRecoVHit* hit = fEvent->MMRecoEvent->Hit(clus->GetHitVecInClus().at(j));
      mmi = fGeneralInfo->DecodeMMChannel(hit->GetChannelId());

      if (j==0) fTracker[mmi.view]->Clear();
      
      double z = mmi.verse*hit->GetTime()*0.105 + fGeneralInfo->GetMMPosPlaneZ(mmi.bdid/8) ;
      double v = (mmi.view == 0? hit->GetPosition().Y() : hit->GetPosition().X());
      TVector3 newpos;
      newpos.SetXYZ(hit->GetPosition().X(),hit->GetPosition().Y(),z);
      hit->SetPosition(newpos);
      fTracker[mmi.view]->AddHit(hit);
      if (fEventCounter  < 100 && fSaveEvent) {
	fHS->FillHisto2List("MMStudy",Form("zvsv_board%d_ev%d",mmi.bdid,fEventCounter),v,hit->GetPosition().Z(),hit->GetEnergy());
	fHS->FillHisto2List("MMStudy",Form("zcorrvsv_board%d_ev%d",mmi.bdid,fEventCounter),v,mmi.verse*(hit->GetTime()-t0chamber2)*0.105 + fGeneralInfo->GetMMPosPlaneZ(mmi.bdid/8),hit->GetEnergy());
	for (uint q = 0; q < cluIndices.size(); q++){
	  TRecoVCluster* tempClu = ECal_clEvent->Element((int)cluIndices.at(q));
	  double dvdzclu = (tempClu->GetPosition()[1-mmi.view]-GeneralInfo::GetInstance()->GetTargetPos()[1-mmi.view])/dzclu;
	  for (int ip = 0; ip < 1000; ip++) {
	    double z = GeneralInfo::GetInstance()->GetTargetPos().Z() + dzclu/1000.*ip;	      
	    double v = GeneralInfo::GetInstance()->GetTargetPos()[1-mmi.view] + dvdzclu*(z-GeneralInfo::GetInstance()->GetTargetPos().Z());
	    fHS->FillHisto2List("MMStudy",Form("zvsv_board%d_ev%d_clus",mmi.bdid,fEventCounter),v,z,1.);
	    //				tempClu->GetPosition()[1-mmi.view],GeneralInfo::GetInstance()->GetCOG().Z(),1.);
	  }
	}
      }
    }
    fTracker[mmi.view]->InitFit(); 
    bool goodfit = fTracker[mmi.view]->MakeFit();

    if (goodfit && TMath::Abs(fTracker[mmi.view]->GetFitResults(1-mmi.view)) < 350. && TMath::Abs(fTracker[mmi.view]->GetFitResults(3-mmi.view)) < 350.) { 
      tracklet traco;
      traco.chi2 = fTracker[mmi.view]->GetFitChi2();
      for (int q = 0; q<4; q++) traco.pars[q] = fTracker[mmi.view]->GetFitResults(q);

      double slopevsz = (fTracker[mmi.view]->GetFitResults(3-mmi.view) - fTracker[mmi.view]->GetFitResults(1-mmi.view))/(fGeneralInfo->GetMMPosPlaneZ(1)-fGeneralInfo->GetMMPosPlaneZ(0));// dv/dz      
      double interatzmid = fTracker[mmi.view]->GetFitResults(1-mmi.view) + slopevsz*0.5*(fGeneralInfo->GetMMPosPlaneZ(1)-fGeneralInfo->GetMMPosPlaneZ(0)); // v0 + dv/dz*(z1-z0)/2
      traco.slope = slopevsz;
      traco.inter = interatzmid;
      traco.nstrips = nhitsPerClus;
      tracklets[mmi.plane][mmi.packed].push_back(traco);
    
      fHS->FillHisto2List("MMStudy","FitChi2",nhitsPerClus,fTracker[mmi.view]->GetFitChi2(),1.);

      TVector3 fitpoints[2];
      for (uint i=0; i<2; i++) fitpoints[i].SetXYZ(traco.pars[2*i],traco.pars[1+2*i],fGeneralInfo->GetMMPosPlaneZ(i)); // points at Z0, Z1
	
      TVector3 lambda(fitpoints[1].X()-fitpoints[0].X(),fitpoints[1].Y()-fitpoints[0].Y(),fitpoints[1].Z()-fitpoints[0].Z());
      double dist = lambda.Mag();
      lambda *= (1./dist);      
      traco.lambda = lambda; // (P1-P0)/|P1-P0|
      
      if (fEventCounter  < 100 && fSaveEvent) {
	// r0 + lambda t, r0 = par0,1,2; lambda(theta,phi)=par3,4
	// z = z0 + dz/dy (y-y0)

	for (int tt = 0; tt < 100; tt++){
	  TVector3 errenow(fitpoints[0].X(),fitpoints[0].Y(),fitpoints[0].Z());
	  TVector3 delta;
	  delta.SetXYZ(lambda.X(),lambda.Y(),lambda.Z());
	  double t = tt*dist/100.;	  
	  delta *= t;
	  errenow += delta;
	  fHS->FillHisto2List("MMStudy",Form("zvsv_board%d_ev%d_fit",mmi.bdid,fEventCounter),errenow[1-mmi.view],errenow.Z(),1.);
	}
      }
    }    
  }

  // tracklet pairs whatsoever, between planes
  for (int i=0; i<8; i++){
    int view = (i&0x2)>>1;
    for (uint i0 = 0; i0<tracklets[0][i].size(); i0++){
      tracklet traco0 = tracklets[0][i].at(i0);
      int istripBin0 = 0;
      if (traco0.nstrips < 3) istripBin0 = 0; // 1,2
      else if (traco0.nstrips == 3) istripBin0 = 1; // 3
      else if (traco0.nstrips == 4) istripBin0 = 2; // 4
      else                          istripBin0 = 3; // 5+

      for (int j=0; j<8; j++){
	for (uint i1 = 0; i1<tracklets[1][j].size(); i1++){
	  tracklet traco1 = tracklets[1][j].at(i1);
	  
	  int istripBin1 = 0;
	  if (traco1.nstrips < 3) istripBin1 = 0; // 1,2
	  else if (traco1.nstrips == 3) istripBin1 = 1; // 3
	  else if (traco1.nstrips == 4) istripBin1 = 2; // 4
	  else                          istripBin1 = 3; // 5+

	  int istripBin = TMath::Min(istripBin0,istripBin1);

	  fHS->FillHisto2List("MMStudy",Form("MatchingPlotSlope_stripBin%d",istripBin),traco0.slope,traco1.slope,1.);
	  
	}
      }
    }
  }


  
  for (int i=0; i<8; i++){
    int view = (i&0x2)>>1;
    for (uint i0 = 0; i0<tracklets[0][i].size(); i0++){
      tracklet traco0 = tracklets[0][i].at(i0);
      int istripBin0 = 0;
      if (traco0.nstrips < 3) istripBin0 = 0; // 1,2
      else if (traco0.nstrips == 3) istripBin0 = 1; // 3
      else if (traco0.nstrips == 4) istripBin0 = 2; // 4
      else                          istripBin0 = 3; // 5+

      
      for (uint i1 = 0; i1<tracklets[1][i].size(); i1++){
	tracklet traco1 = tracklets[1][i].at(i1);

	int istripBin1 = 0;
	if (traco1.nstrips < 3) istripBin1 = 0; // 1,2
	else if (traco1.nstrips == 3) istripBin1 = 1; // 3
	else if (traco1.nstrips == 4) istripBin1 = 2; // 4
	else                          istripBin1 = 3; // 5+

	int istripBin = TMath::Min(istripBin0,istripBin1);
	
	fHS->FillHisto2List("MMStudy",Form("MatchingPlotNStrips_side%d_view%d_otherview%d",((i&0x4)>>2),((i&0x2)>>1),(i&0x1)),traco0.nstrips,traco1.nstrips,1.);

	double vmid = 0.5*(traco0.inter + traco1.inter); // half position of the two intercepts at the zmid plane (mesh plane)
	// z = z0 + dz/dv x Dv

	double zp0 = fGeneralInfo->GetMMPosPlaneZ(0) + (vmid-traco0.pars[1-view])/traco0.slope;
	double zp1 = fGeneralInfo->GetMMPosPlaneZ(0) + (vmid-traco1.pars[1-view])/traco1.slope;
	
	fHS->FillHisto2List("MMStudy",Form("MatchingPlotSlope_side%d_view%d_otherview%d_stripBin%d",((i&0x4)>>2),((i&0x2)>>1),(i&0x1),istripBin),traco0.slope,traco1.slope,1.);

	fHS->FillHisto2List("MMStudy",Form("MatchingPlot_side%d_view%d_otherview%d_stripBin%d",((i&0x4)>>2),((i&0x2)>>1),(i&0x1),istripBin),traco0.slope-traco1.slope,traco0.inter-traco1.inter,1.);
	fHS->FillHisto2List("MMStudy",Form("MatchingPlotDZ_side%d_view%d_otherview%d_stripBin%d",((i&0x4)>>2),((i&0x2)>>1),(i&0x1),istripBin),traco0.slope-traco1.slope,zp0-zp1,1.);
      }
    }
  }

  /*  
  int nclusPerBoard[16]={0};
  for (int i=0; i<nclus; i++){
    TRecoVCluster* clus = fEvent->MMRecoCl->Element(i);
    int nhitsPerClus = clus->GetNHitsInClus();
    TRecoVHit* hit = fEvent->MMRecoEvent->Hit(clus->GetHitVecInClus().at(0));
    int chId = hit->GetChannelId();
    int bdid = (chId & 0xF00 ) >> 8; // board SN 0-15
    nclusPerBoard[bdid]++;

    int layer = bdid/2;  // layer 0-7
    int plane = layer/4;  // plane 0-1
    int side = bdid%2; // left/right (X view), bottom/top (Y view)
    int strip = (chId & 0x0FF); // strip 0-255
    int view = (layer/2)%2; // 0 means Y view, 1 means X view    
    int otherview = layer%2; // 0 means the half-strip left (bottom) depending on the view

    // nhits per clus
    fHS->FillHistoList("MMStudy","NHitsPerCluster",nhitsPerClus,1.);
    int njumps = 0;
    for (int j=1; j<nhitsPerClus; j++){
      TRecoVHit* hitR = fEvent->MMRecoEvent->Hit(clus->GetHitVecInClus().at(j));
      int stripR = hitR->GetChannelId() & 0x0FF;
      TRecoVHit* hitL = fEvent->MMRecoEvent->Hit(clus->GetHitVecInClus().at(j-1));
      int stripL = hitL->GetChannelId() & 0x0FF;
      njumps += stripR-stripL-1;
    }
    fHS->FillHistoList("MMStudy","NJumpsPerCluster",njumps,1.);

    // perform evaluation of the tracklet parameters

    // v = a z + b, where v = x or y depending on the view
    // slope (<vz> - <v><z>)/(<z^2> - <z>^2)
    // intercept = - <z> x slope + <v>
    if (nhitsPerClus > 2) {

      double z2av = 0;
      double vzav = 0;
      double vav = 0;
      double zav = 0;
      double clucharge = 0;

      for (int j=0; j<nhitsPerClus; j++){
	TRecoVHit* hit = fEvent->MMRecoEvent->Hit(clus->GetHitVecInClus().at(j));
	double v = (view == 0)? hit->GetPosition().Y() : hit->GetPosition().X();
	int verse = (plane == 0)? 1 : -1;
	double z = verse*hit->GetTime()*0.105 + hit->GetPosition().Z() ;

	fHS->FillHistoList("MMStudy",Form("HitCharge"),hit->GetEnergy(),1.);
	clucharge += hit->GetEnergy();
	//	std::cout << "Clus " << i << " hit " << j << " / " << nhitsPerClus << " v,z = " << v << " , " << z << " time = " << hit->GetTime() << " bd = " << bdid << " lay = " << layer << " side = " << side << " view = " << view << " otherview = " << otherview << " strip = " << strip << std::endl;

	z2av += z*z; // now without error weights
	vzav += z*v;
	vav  += v;
	zav  += z;
      }    
      fHS->FillHistoList("MMStudy",Form("ClusterCharge"),clucharge,1.);

      z2av /= nhitsPerClus;
      vzav /= nhitsPerClus;
      vav /= nhitsPerClus;
      zav /= nhitsPerClus;
      double slope = (vzav - vav*zav)/(z2av - zav*zav);
      double inter = -slope*zav + vav;

      double chi2 = 0;
      for (int j=0; j<nhitsPerClus; j++) {
	TRecoVHit* hit = fEvent->MMRecoEvent->Hit(clus->GetHitVecInClus().at(j));
	double v = (view == 0)? hit->GetPosition().Y() : hit->GetPosition().X();
	int verse = (plane == 0)? 1 : -1;
	double z = verse*hit->GetTime()*0.105 + hit->GetPosition().Z() ;
	double res = v - (slope*z + inter);
	fHS->FillHisto2List("MMStudy",Form("ResVsNdf"),nhitsPerClus-2,res,1.);
	chi2 += res*res;
      }

      fHS->FillHisto2List("MMStudy",Form("Chi2OverNdfVsNdf"),nhitsPerClus-2,chi2/(nhitsPerClus-2),1.);


      int packed = (side << 2);
      packed |= (view<<1);
      packed |= (otherview);

      tracklet traco;
      traco.slope = slope;
      traco.inter = inter;
      tracklets[plane][packed].push_back(traco);
      
      fHS->FillHistoList("MMStudy",Form("PositionAtECal_view%d",view),(slope*fGeneralInfo->GetCOG().Z() + inter),1.);
      fHS->FillHistoList("MMStudy",Form("PositionAtTarget_view%d",view),(slope*fGeneralInfo->GetTargetPos().Z() + inter),1.);
      fHS->FillHisto2List("MMStudy",Form("PositionAtECalVsAtTarget_view%d",view),(slope*fGeneralInfo->GetTargetPos().Z() + inter),(slope*fGeneralInfo->GetCOG().Z() + inter),1.);

    }

  }

  for (int i=0; i<8; i++){
    for (uint i0 = 0; i0<tracklets[0][i].size(); i0++){
      tracklet traco0 = tracklets[0][i].at(i0);
      for (uint i1 = 0; i1<tracklets[1][i].size(); i1++){
	tracklet traco1 = tracklets[1][i].at(i1);

	fHS->FillHisto2List("MMStudy",Form("MatchingPlot"),traco0.slope-traco1.slope,traco0.inter-traco1.inter,1.);
	
      }
    }
  }
      

  
  // tracklets per plane
  // correlate slopes for the same view in the different planes
  // find cluster with slopes similar for the two planes
  // determine the time offset between the planes: DeltaZ/v_drift, where DeltaZ = Zplane1-Zplane0 at a specific v
  // correlate with calorimeter clusters: TimeECal-(TimeOffsetPlanes) vs posECal - posTrack

  // plot of nclus per board
  for (int bdid = 0; bdid<16; bdid++) fHS->FillHisto2List("MMStudy","NClustersPerBoard",bdid,nclusPerBoard[bdid],1.);
  */
  
  fEventCounter++;
  return true;
}

Bool_t MMStudy::Finalize(){
  if(fGeneralInfo->isMC()){
    std::cout<<"This run is MC, MMStudy checks do not apply"<<std::endl;
    return false;
  } 

  if (fVerbose) printf("---> Finalizing MMStudy\n");
  return true;
}



