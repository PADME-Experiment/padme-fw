#include "MMTrackDevel.hh"
#include "TGraph.h"
#include "TString.h"
#include "TF1.h"
#include "TMath.h"
#include "GeneralInfo.hh"
#include "ECalSel.hh"

MMTrackDevel* MMTrackDevel::fInstance = 0;

MMTrackDevel* MMTrackDevel::GetInstance(){
  if ( fInstance == 0 ) { fInstance = new MMTrackDevel(); }
  return fInstance;
}

MMTrackDevel::~MMTrackDevel(){
  delete fCfgParser;
}

Bool_t MMTrackDevel::Init(PadmeAnalysisEvent* event,  Bool_t fHistoModeVal, TString InputHistofileVal){

  if (fVerbose) printf("---> Initializing MMTrackDevel\n");
  TString cfgFile = "config/UserAnalysis.conf";
  fHS = HistoSvc::GetInstance();
  fGeneralInfo = GeneralInfo::GetInstance();
  fMMClusteringInstance = MMClustering::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());

  fEvent = event;
  fNRun = fGeneralInfo->GetRunNumberFromDB(); //30000 vale solo per il 2022
  fHistoMode = fHistoModeVal;
  InputHistofile = InputHistofileVal;
  fEventCounter = 0;
  InitHistos(fNRun);

  return true;
}

Bool_t MMTrackDevel::InitHistos(Int_t nRun){
  // MMTrackDevel directory will contain all histograms related to this analysis

  fHS->CreateList("MMTrackDevel");
  cout<<" Creating MMTrackDevel Hystograms for Run "<<nRun<<" "<<endl;
  //  fHS->BookHisto2List("MMTrackDevel","cluDistance",100,0,100,200,-100,100);
  fHS->BookHisto2List("MMTrackDevel","MM_Nclus_vs_NHits",100,0,3000.,500,0,500); 
  
  int ipmodeMax[2] = {2,1};
  for (int clumode=0; clumode<2; clumode++){
    for (int ipmode=0; ipmode<ipmodeMax[clumode]; ipmode++){
      fHS->BookHistoList("MMTrackDevel",Form("MM_NclusIPMode%dCluMode%d",ipmode,clumode),1000,0,1000);
    }
  }
  fHS->BookHisto2List("MMTrackDevel","MM_chi2_vs_dz",100,-50,50,1000,0,50);
  fHS->BookHisto2List("MMTrackDevel","ECAL_dt_vs_MM_dz",100,-50,50,1000,-500,500);

  for (int view = 0; view<2; view++){
    TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(view);    
    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_dz_Q%d_clu1",viewlabel.Data(),quad),100,-200,200,1000,-100,100);
    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_dz_Q%d_clu1_recenter",viewlabel.Data(),quad),100,-20,20,100,-20,20);
    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_%s_vs_IPtheta_Q%d_clu1",viewlabel.Data(),quad),100,-0.01,0.01,1200,-600,600);
    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_Nhit_Q%d_clu0",viewlabel.Data(),quad),10,0,10,1000,-100,100);
    for (int quad = 0; quad<4; quad++) fHS->BookHistoList("MMTrackDevel",Form("MM_ECAL_%sAtTarget_Q%d_clu1",viewlabel.Data(),quad),400,-200.,200.);
    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_XEcal_vs_%sAtTarget_Q%d_clu1",viewlabel.Data(),quad),400,-200,200,100,-300,300.);
    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_YEcal_vs_%sAtTarget_Q%d_clu1",viewlabel.Data(),quad),400,-200,200,100,-300,300.);
    for (int quad = 0; quad<4; quad++) fHS->BookHistoList("MMTrackDevel",Form("MM_ECAL_%sAtTarget_Q%d_clu1_ReFit",viewlabel.Data(),quad),400,-200.,200.);
    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_chi2_vs_Nhit_clu1_Q%dV%s",quad,viewlabel.Data()),50,0,50,1000,0,100);
  }
  for (int view = 0; view<2; view++){
    TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(view);    
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_dz_Qall_clu1_vsYEcal",viewlabel.Data()),100,-300,300,100,-200,200);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_dz_Qall_clu1_vsXEcal",viewlabel.Data()),100,-300,300,100,-200,200);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECALMatch_d%s_vs_dz_Qall_clu1_Chi2vsAngleLevelZero",viewlabel.Data()),100,-0.1,0.1,100,0,100);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECALMatch_d%s_vs_dz_Qall_clu1_Chi2vsDZ",viewlabel.Data()),100,-80,80,100,0,100);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECALMatch_d%s_vs_dz_Qall_clu1_DdzvsDZ",viewlabel.Data()),100,-80,80,100,-200,200);
  }
  for (int bdid = 0; bdid < 16; bdid++) {
    fHS->BookHisto2List("MMTrackDevel",Form("MM_Time_vs_channel_bdid%d",bdid),256,-0.5,255.5,900,-150,750);
  }
  for(int quad=0; quad<4; quad++) fHS->BookHistoList("MMTrackDevel",Form("MM_Z_first_last_hit_clu1_matched_Q%d",quad),100,fGeneralInfo->GetMMPosPlaneZ(0)-50,fGeneralInfo->GetMMPosPlaneZ(1)+50);
  fHS->BookHisto2List("MMTrackDevel",Form("MM_QuadrantID_vs_TrackMatchedCode"),8,0,8,4,0,4);
  fHS->BookHisto2List("MMTrackDevel",Form("MM_TrackMatchedCode2vs1"),4,0,4,4,0,4);

  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dX1_vs_dX2_clu0",100,-50,50,100,-50,50);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dY1_vs_dY2_clu0",100,-50,50,100,-50,50);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dY1_vs_dX2_clu0",100,-50,50,100,-50,50);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dX3_vs_dX4_clu0",100,-50,50,100,-50,50);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dY3_vs_dY4_clu0",100,-50,50,100,-50,50);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dY3_vs_dX4_clu0",100,-50,50,100,-50,50);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dX_vs_dX_4p_clu0",100,-50,50,100,-50,50);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dY_vs_dY_4p_clu0",100,-50,50,100,-50,50);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dY_vs_dX_4p_clu0",100,-50,50,100,-50,50);

  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_wrong_dX1_vs_dX2_clu0",100,-50,50,100,-50,50);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_wrong_dY1_vs_dY2_clu0",100,-50,50,100,-50,50);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_wrong_dY1_vs_dX2_clu0",100,-50,50,100,-50,50);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_wrong_dX3_vs_dX4_clu0",100,-50,50,100,-50,50);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_wrong_dY3_vs_dY4_clu0",100,-50,50,100,-50,50);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_wrong_dY3_vs_dX4_clu0",100,-50,50,100,-50,50);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_wrong_dX_vs_dX_4p_clu0",100,-50,50,100,-50,50);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_wrong_dY_vs_dY_4p_clu0",100,-50,50,100,-50,50);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_wrong_dY_vs_dX_4p_clu0",100,-50,50,100,-50,50);
 
  return true;
}

Bool_t MMTrackDevel::Process(){

  // retrieve ecal info
  
  if (ECalSel::GetInstance()->getNECalEvents() == 0) return kFALSE;
  
  TRecoVClusCollection* ECal_clEvent = fEvent->ECalRecoCl;
  std::vector<int> cluIndices;
  std::vector<double> cluTime_avg;
  for (int i=0; i< ECalSel::GetInstance()->getNECalEvents(); i++){
    ECalSelEvent* selEvent = ECalSel::GetInstance()->getECalEvent(i);
    if (selEvent->flagEv != ev_gg) continue;
    cluTime_avg.push_back(0.);
    for (int h1 = 0; h1 < 2; h1++) {
      TRecoVCluster* tempClu = ECal_clEvent->Element(selEvent->indexECal[h1]);
      //      fHS->FillHisto2List("MMTrackDevel",Form("ECalSelClusters_yvsx_ev%d",fEventCounter),tempClu->GetPosition().X(),tempClu->GetPosition().Y(),tempClu->GetEnergy());
      cluTime_avg.at(cluTime_avg.size()-1) += tempClu->GetTime()*0.5;
      cluIndices.push_back(selEvent->indexECal[h1]);
    }
    cluTime_avg.at(cluTime_avg.size()-1) += 440.;
  }
  
  if (cluIndices.size() == 0) return kFALSE; // cluster pairs
  // retrieve trigger info  
  
  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  //long long int fTimeStamp =(long long int) fEvent->RecoEvent->GetEventTime().GetSec();
  if(trigMask & (1 << 0)) {
  }

  int nclus = fEvent->MMRecoCl->GetNElements();
  int nhits = fEvent->MMRecoEvent->GetNHits();
  fHS->FillHisto2List("MMTrackDevel","MM_Nclus_vs_NHits",nhits,nclus,1.);

  fMMClusteringInstance->Clear();
  fMMClusteringInstance->Init(fEvent->MMRecoEvent);
  fMMClusteringInstance->Clusterize();

  // plot NHit vs apv vs boardid Nhit vs (apvid + 2*boardid)
  
  int ipmodeMax[2] = {2,1};
  for (int clumode=0; clumode<2; clumode++){
    for (int ipmode=0; ipmode<ipmodeMax[clumode]; ipmode++){
      fHS->FillHistoList("MMTrackDevel",Form("MM_NclusIPMode%dCluMode%d",ipmode,clumode),fMMClusteringInstance->GetMMClusterLength(ipmode,clumode),1.);
    }
  }

  int signsQuadX[4] = {-1,-1,1,1};
  int signsQuadY[4] = {-1,1,1,-1};
  double offsetXY[2][4]     = {
    0,   0.,-10.,-10. ,
    0, -10., -5., 0.  };
  double dxy_MMEcalMax[2][4] = {
    20,  20., 20., 20.,
    20,  20., 20., 20.};
  double offsetdDZvsXY[2][4] = {
    12.3,  -1.1, 15., 8. ,
    -10,  29.,  18.2, 3. };
  double maxdDZvsXY[2][4]    = {
    30., 22., 16., 16.,
    16, 16., 16., 16.};

//  // dDZ vs Y
//  double offsetY[4]      = {  0, -10., -5., 0. };
//  double dy_MMEcalMax[4] = { 20,  20., 20., 20.};
//  double offsetdDZvsY[4] = {0,  50.,  0., 5. };
//  double maxdDZvsY[4]    = {50, 50., 50., 50.};
//  // dDZ vs X
//  double offsetX[4]      = {  0,   0.,-10.,-10. };
//  double dx_MMEcalMax[4] = { 20,  20., 20., 20.};
//  double offsetdDZvsX[4] = { 5.,  5.,  5., 5. };
//  double maxdDZvsX[4]    = {50., 50., 50., 50.};
  
  // plots to calibrate the T0

  for(int iclu=0; iclu<(int) fMMClusteringInstance->GetMMClusterLength(0,0); iclu++) {
    int Nhit = (int) fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetHitsVectorSize();
    int bdid = fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetHit(0)->GetMMchInfo().bdid;

    //    if (Nhit < 3) continue;
    for(int ih=0; ih< Nhit; ih++) {
      MMSoftHit* hitnow = ((MMSoftHit*) fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetHit(ih));
      double timeHit = hitnow->GetTime();
      double stripid = hitnow->GetChannelId() & 0xFF;
      fHS->FillHisto2List("MMTrackDevel",Form("MM_Time_vs_channel_bdid%d",bdid),stripid,timeHit,1.);
    }
  }

  // Variable for response evaluation. For any given Ecal cluster, provides a response code:
  // 0    -> no track-Ecal matching
  // bit0 -> Yview matching
  // bit1 -> Xview matching
  
  vector<int> responseCode;
  for(int iidx=0; iidx<(int) cluIndices.size(); iidx++) responseCode.push_back(0);

  // plots to evaluate matching of level-1 tracks with the ECal info

  for(int iclu=0; iclu<(int) fMMClusteringInstance->GetMMClusterLength(0,1); iclu++) {
    int quad = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetHit(0)->GetMMchInfo().quad;
    int view = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetHit(0)->GetMMchInfo().view;
    vector<int> level0Merged = fMMClusteringInstance->GetHitComposition(iclu,0,1);
    if (level0Merged.size() != 2) {
      std::cout << "Inconsistente level one merging " << level0Merged.size() << std::endl;
      continue;
    }
    MMCluster* levelZeroClus[2] = {fMMClusteringInstance->GetMMCluster(level0Merged.at(0),0,0),fMMClusteringInstance->GetMMCluster(level0Merged.at(1),0,0)};
    
    double angleLevelZero = levelZeroClus[0]->GetTracklet().lambda.Angle(levelZeroClus[1]->GetTracklet().lambda);
  
    TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(view);
    double dz = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetTracklet().pars[4];
    double chi2 = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetTracklet().chi2;
    double ipphaseangle = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetIPPhaseAngle();
    int Nhit = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetHitsVectorSize();
    fHS->FillHisto2List("MMTrackDevel","MM_chi2_vs_dz",dz,chi2,1.);
    fHS->FillHisto2List("MMTrackDevel","MM_chi2_vs_angleDiffLevel0",dz,chi2,1.);
    fHS->FillHisto2List("MMTrackDevel",Form("MM_chi2_vs_Nhit_clu1_Q%dV%s",quad,viewlabel.Data()),Nhit,chi2,1.);
    
    for(int ipair=0; ipair<(int) cluTime_avg.size(); ipair++) {
      double dt_Ecal = cluTime_avg.at(ipair);
      //double dz_Ecal = dt_Ecal*fGeneralInfo->GetDriftVelocity();
      fHS->FillHisto2List("MMTrackDevel","ECAL_dt_vs_MM_dz",dz,dt_Ecal,1.);
    }

    for(int iidx=0; iidx<(int) cluIndices.size(); iidx++) {
      TRecoVCluster* tempClu = ECal_clEvent->Element(cluIndices.at(iidx));
      double z_Ecal = GeneralInfo::GetInstance()->GetCOG().Z()+6.5*11.;
      TVector3 MMposAtEcal = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetTracklet().ExtrapolationAtZ(z_Ecal);//tempClu->GetPosition().Z());
      TVector3 MMposAtTarg = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetTracklet().ExtrapolationAtZ(GeneralInfo::GetInstance()->GetTargetPos().Z());

      double y_Ecal = tempClu->GetPosition().Y();
      double x_Ecal = tempClu->GetPosition().X();
      double dz_Ecal = (tempClu->GetTime()+440.)*fGeneralInfo->GetMMDriftVelocity();

      double Ddz = dz - dz_Ecal;
      double dv_MMEcal = MMposAtEcal[1-view] - tempClu->GetPosition()[1-view];
      if(x_Ecal*signsQuadX[quad] > 0 && y_Ecal*signsQuadY[quad] > 0 && chi2 < 20) {
	fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_dz_Q%d_clu1",viewlabel.Data(),quad),Ddz,dv_MMEcal,1.);
	fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_dz_Q%d_clu1_recenter",viewlabel.Data(),quad),
			    (Ddz-offsetdDZvsXY[1-view][quad])  / (maxdDZvsXY[1-view][quad]),
			    (dv_MMEcal-offsetXY[1-view][quad]) / (dxy_MMEcalMax[1-view][quad]), 1.);
	fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_XEcal_vs_%sAtTarget_Q%d_clu1",viewlabel.Data(),quad),MMposAtTarg[1-view],x_Ecal,1.);
	fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_YEcal_vs_%sAtTarget_Q%d_clu1",viewlabel.Data(),quad),MMposAtTarg[1-view],y_Ecal,1.);
	
	if(fabs(dv_MMEcal-offsetXY[1-view][quad])< dxy_MMEcalMax[1-view][quad] && fabs(Ddz-offsetdDZvsXY[1-view][quad])< maxdDZvsXY[1-view][quad]) { // calorimeter matching
	  responseCode.at(iidx) |= (1 << view);
	  
	  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECALMatch_d%s_vs_dz_Qall_clu1_Chi2vsAngleLevelZero",viewlabel.Data()),angleLevelZero,chi2,1.);
	  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECALMatch_d%s_vs_dz_Qall_clu1_Chi2vsDZ",viewlabel.Data()),dz,chi2,1.);
	  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECALMatch_d%s_vs_dz_Qall_clu1_DdzvsDZ",viewlabel.Data()),dz,Ddz,1.);

	  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_%s_vs_IPtheta_Q%d_clu1",viewlabel.Data(),quad),ipphaseangle,MMposAtEcal[1-view],1.);
	  // plot of track extrapolation at z target
	  fHS->FillHistoList("MMTrackDevel",Form("MM_ECAL_%sAtTarget_Q%d_clu1",viewlabel.Data(),quad),MMposAtTarg[1-view],1.);
	  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_dz_Qall_clu1_vsXEcal",viewlabel.Data()),x_Ecal,dv_MMEcal,1.);
	  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_dz_Qall_clu1_vsYEcal",viewlabel.Data()),y_Ecal,dv_MMEcal,1.);

	  // refit track with DZ from ECal
	  if (fMMClusteringInstance->GetMMCluster(iclu,0,1)->ReFitWithClusterTime(kTRUE,dz_Ecal+offsetdDZvsXY[1-view][quad])) {
	    TVector3 MMposAtTarg = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetTracklet().ExtrapolationAtZ(GeneralInfo::GetInstance()->GetTargetPos().Z());
	    fHS->FillHistoList("MMTrackDevel",Form("MM_ECAL_%sAtTarget_Q%d_clu1_ReFit",viewlabel.Data(),quad),MMposAtTarg[1-view],1.);	    
	  }

	  double z_min=100000000,z_max=-10000000;
	  for(int h=0; h<Nhit; h++) {
	    double z_hit = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetHit(h)->GetZfromTime(1.);
	    if(z_hit > z_max) z_max = z_hit;
	    if(z_hit < z_min) z_min = z_hit;
	  }
	  z_max += dz;
	  z_min -= dz;
	  fHS->FillHistoList("MMTrackDevel",Form("MM_Z_first_last_hit_clu1_matched_Q%d",quad),z_max,1.);
	  fHS->FillHistoList("MMTrackDevel",Form("MM_Z_first_last_hit_clu1_matched_Q%d",quad),z_min,1.);
	}
      }
    } // cluster loop
  } // MM track loop type 1

  // plot for response evaluation per track

  for(int iidx=0; iidx<(int) cluIndices.size(); iidx++) {
    TRecoVCluster* tempClu = ECal_clEvent->Element(cluIndices.at(iidx));
    double y_Ecal = tempClu->GetPosition().Y();
    double x_Ecal = tempClu->GetPosition().X();
    int quad = 0;
    if (x_Ecal < 0 && y_Ecal < 0) quad = 0;
    else if (x_Ecal < 0 && y_Ecal > 0) quad = 1;
    else if (x_Ecal > 0 && y_Ecal > 0) quad = 2;
    else quad = 3;

    double pitch = GeneralInfo::GetInstance()->GetMMStripPitch();
    int apvid;
    for(int pl=0; pl<2; pl++) {
      for(int vw=0; vw<2; vw++) {
	if(vw == 0) { //YVIEW
	  if(quad == 0) {
	    bdid = 0+8*pl;
	    if(fabs(y_Ecal)>128*pitch) apvid = 0;
	    else  apvid = 1; //fabs(y_Ecal)<128*pitch
	  }
	  if(quad == 1) {
	    bdid = 1+8*pl;
	    if(fabs(y_Ecal)<128*pitch) apvid = 0;
	    else  apvid = 1; // fabs(y_Ecal)>128*pitch
	  }
	  if(quad == 2) {
	    bdid = 3+8*pl;
	    if(fabs(y_Ecal)<128*pitch) apvid = 0;
	    else apvid = 1; // fabs(y_Ecal)>128*pitch
	  }
	  if(quad == 3){
	    bdid = 2+8*pl;
	    if(fabs(y_Ecal)>128*pitch) apvid = 0;
	    else apvid = 1; //fabs(y_Ecal)<128*pitch
	  } 
	}
	else { //XVIEW
	 if(quad == 0) {
	    bdid = 6+8*pl;
	    if(fabs(x_Ecal)>128*pitch) apvid = 0;
	    else  apvid = 1; //fabs(x_Ecal)<128*pitch
	  }
	  if(quad == 1) {
	    bdid = 4+8*pl;
	    if(fabs(x_Ecal)>128*pitch) apvid = 0;
	    else  apvid = 1; // fabs(x_Ecal)<128*pitch
	  }
	  if(quad == 2) {
	    bdid = 5+8*pl;
	    if(fabs(x_Ecal)<128*pitch) apvid = 0;
	    else apvid = 1; // fabs(x_Ecal)>128*pitch
	  }
	  if(quad == 3){
	    bdid = 7+8*pl;
	    if(fabs(x_Ecal)<128*pitch) apvid = 0;
	    else apvid = 1; //fabs(x_Ecal)>128*pitch
	  } 
	}
	
	int NHitsPerAPV = fMMClusteringInstance->GetNHitsPerAPV(apvid,bdid);
	fHS->FillHisto2List("MMTrackDevel",Form("MM_NHitsPerAPV_vs_TrackMatchedCode"),responseCode.at(iidx),NHitsPerAPV);
      }
    }

    //plot Nhits vs apvid+2*bdid ([0.5,128.5] vs [0.5,31.5])
    // we expect that when the responseCode has no bit raised for the view x(y), the number of hits for the apvid,bdid pair corresponding to view x(y) has more often LOW number of hits than when the response code has the bit raised for the view x(y)
    // loop over the view
    //     from the quadrant and from the value of the v for that view, derive the apvid,bdid pair which has that view and which can be used to detect a track in that quadrant
    //     plot IsBitOfThatViewRaised vs NHits(apvid,bdid)
    
        
    fHS->FillHisto2List("MMTrackDevel",Form("MM_QuadrantID_vs_TrackMatchedCode"),responseCode.at(iidx),quad);
    
  }

  
  
  // plot for response evaluation per track pair
  int counter = 0;
  for (int i=0; i< ECalSel::GetInstance()->getNECalEvents(); i++){
    ECalSelEvent* selEvent = ECalSel::GetInstance()->getECalEvent(i);
    if (selEvent->flagEv != ev_gg) continue;

//      TRecoVCluster* tempClu = ECal_clEvent->Element(selEvent->indexECal[h1]);
//      double y_Ecal = tempClu->GetPosition().Y();
//      double x_Ecal = tempClu->GetPosition().X();
//      int quad = 0;
//      if (x_Ecal < 0 && y_Ecal < 0) quad = 0;
//      else if (x_Ecal < 0 && y_Ecal > 0) quad = 1;
//      else if (x_Ecal > 0 && y_Ecal > 0) quad = 2;
//      else quad = 3;
      
    fHS->FillHisto2List("MMTrackDevel",Form("MM_TrackMatchedCode2vs1"),responseCode.at(counter),responseCode.at(counter+1));
    counter+=2;
  }
  
  // check on level-zero clusters

  vector<bool> is_used;
  for(int iclu=0; iclu<(int) fMMClusteringInstance->GetMMClusterLength(0,0); iclu++) {
    is_used.push_back(kFALSE);
  }

  
  
  for(int iclu=0; iclu<(int) fMMClusteringInstance->GetMMClusterLength(0,0); iclu++) {
    int Nhit = (int) fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetHitsVectorSize();
    int quad = fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetHit(0)->GetMMchInfo().quad;
    int view = fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetHit(0)->GetMMchInfo().view;
    int plane = fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetHit(0)->GetMMchInfo().plane;
    TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(view);

    double slope_1 = fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetTracklet().slope;
    
    for(int iidx=0; iidx<(int) cluIndices.size(); iidx++) {
      TRecoVCluster* tempClu = ECal_clEvent->Element(cluIndices.at(iidx));
      double z_Ecal = GeneralInfo::GetInstance()->GetCOG().Z()+6.5*11.;

      TVector3 MMposAtEcal = fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetTracklet().ExtrapolationAtZ(z_Ecal);

      double y_Ecal = tempClu->GetPosition().Y();
      double x_Ecal = tempClu->GetPosition().X();
      
      double dv_MMEcal = MMposAtEcal[1-view] - tempClu->GetPosition()[1-view];
      
      if(x_Ecal*signsQuadX[quad]>0 && y_Ecal*signsQuadY[quad]>0) {
	fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_Nhit_Q%d_clu0",viewlabel.Data(),quad),Nhit,dv_MMEcal,1.);
      }
      
      for(int iclu_2=0; iclu_2<(int) fMMClusteringInstance->GetMMClusterLength(0,0); iclu_2++) {
	int quad_2 = fMMClusteringInstance->GetMMCluster(iclu_2,0,0)->GetHit(0)->GetMMchInfo().quad;
	int view_2 = fMMClusteringInstance->GetMMCluster(iclu_2,0,0)->GetHit(0)->GetMMchInfo().view;
	int plane_2 = fMMClusteringInstance->GetMMCluster(iclu_2,0,0)->GetHit(0)->GetMMchInfo().plane;

	if(is_used.at(iclu_2)) continue;
	if(plane != plane_2 && quad == quad_2 && view == view_2) {
	  double slope_2 = fMMClusteringInstance->GetMMCluster(iclu_2,0,0)->GetTracklet().slope;
	  if(fabs(slope_1 - slope_2)<0.015) { // TO BE REPLACED WITH A VARIABLE !!!
	    is_used.at(iclu) = kTRUE;
	    is_used.at(iclu_2) = kTRUE;
	    
	    TVector3 MMposAtEcal_1 = fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetTracklet().ExtrapolationAtZ(z_Ecal);
	    TVector3 MMposAtEcal_2 = fMMClusteringInstance->GetMMCluster(iclu_2,0,0)->GetTracklet().ExtrapolationAtZ(z_Ecal);

	    double x_MMposAtEcal = 0.5*(MMposAtEcal_1.X()+MMposAtEcal_1.X());
	    double y_MMposAtEcal = 0.5*(MMposAtEcal_1.Y()+MMposAtEcal_1.Y());

	    
	  }
	}
	
      }
          
//      if(view == YVIEW) {
//	double dy_MMEcal = MMposAtEcal.Y() - y_Ecal;
//      }
//      if(view == XVIEW) {
//	double dx_MMEcal = MMposAtEcal.X() - x_Ecal;
//	if(x_Ecal*signsQuadX[quad]>0 && y_Ecal*signsQuadY[quad]>0) fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_dX_vs_Nhit_Q%d_clu0",quad),Nhit,dx_MMEcal,1.);
//      }
      
    }
    
    
  }


  vector<pair<double,int>> distance, wrong_distance;
  
  for(int iidx=0; iidx<(int) cluIndices.size(); iidx++) {
      TRecoVCluster* tempClu = ECal_clEvent->Element(cluIndices.at(iidx));
      double z_Ecal = GeneralInfo::GetInstance()->GetCOG().Z()+6.5*11.;

      distance.clear();
      wrong_distance.clear();
      for(int itra=0; itra<(int) fMMClusteringInstance->GetMMClusterLength(0,0); itra++) {
	int Nhit = (int) fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHitsVectorSize();
	int quad = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(0)->GetMMchInfo().quad;
	int view = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(0)->GetMMchInfo().view;
	int plane = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(0)->GetMMchInfo().plane;
	int other_view = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(0)->GetMMchInfo().otherview;
	TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(view);

	TVector3 MMposAtEcal = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetTracklet().ExtrapolationAtZ(z_Ecal);
	double dv_MMEcal = MMposAtEcal[1-view] - tempClu->GetPosition()[1-view];
	
	TVector3 MMHitpos = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(0)->GetPosition();
	if(view == 1) { //XVIEW
	  double YEcal = tempClu->GetPosition().Y();
	  double YMM = MMHitpos.Y();
	  if((quad == 0) && (YEcal < 0) && (YEcal > 2*YMM)) {
	    distance.emplace_back(fabs(dv_MMEcal), itra);
	  }
	  else if((quad == 1) && (YEcal > 0) && (YEcal < 2*YMM)) {
	    distance.emplace_back(fabs(dv_MMEcal), itra);
	  }
	  else if((quad == 2) && (YEcal > 0) && (YEcal < 2*YMM)) {
	    distance.emplace_back(fabs(dv_MMEcal), itra);
	  }
	  else if((quad == 3) && (YEcal < 0) && (YEcal > 2*YMM)) {
	    distance.emplace_back(fabs(dv_MMEcal), itra);
	  }
	  else {
	    wrong_distance.emplace_back(fabs(dv_MMEcal), itra);
	  }
	}
	if(view == 0) { //YVIEW
	  double XEcal = tempClu->GetPosition().X();
	  double XMM = MMHitpos.X();
	  if((quad == 0) && (XEcal < 0) && (XEcal > 2*XMM)) {
	    distance.emplace_back(fabs(dv_MMEcal), itra);
	  }
	  else if((quad == 1) && (XEcal < 0) && (XEcal > 2*XMM)) {
	    distance.emplace_back(fabs(dv_MMEcal), itra);
	  }
	  else if((quad == 2) && (XEcal > 0) && (XEcal < 2*XMM)) {
	    distance.emplace_back(fabs(dv_MMEcal), itra);
	  }
	  else if((quad == 3) && (XEcal > 0) && (XEcal < 2*XMM)) {
	    distance.emplace_back(fabs(dv_MMEcal), itra);
	  }
	  else {
	    wrong_distance.emplace_back(fabs(dv_MMEcal), itra);
	  }
	}
      }

      
      std::sort(distance.begin(), distance.end());
      std::sort(wrong_distance.begin(), wrong_distance.end());


      int view_before, plane_before;
      TString viewlabel_before;
      double dv_MMEcal_before;
      
      for(int idist=0; idist<(int) distance.size(); idist++) {
	int itra = distance.at(idist).second;
	int quad = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(0)->GetMMchInfo().quad;
	int view = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(0)->GetMMchInfo().view;
	int plane = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(0)->GetMMchInfo().plane;
	TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(view);
	
	TVector3 MMposAtEcal = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetTracklet().ExtrapolationAtZ(z_Ecal);
	double dv_MMEcal = MMposAtEcal[1-view] - tempClu->GetPosition()[1-view];

	if(idist == 1) {
	  if(plane_before != plane) {
	    if(view_before == view)  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_d%s1_vs_d%s2_clu0",viewlabel_before.Data(),viewlabel.Data()),dv_MMEcal_before,dv_MMEcal,1.);
	  }
	  if(view_before != view)  fHS->FillHisto2List("MMTrackDevel","MM_ECAL_dY1_vs_dX2_clu0",dv_MMEcal_before,dv_MMEcal,1.);
	}
	if(idist == 3) {
	  if(plane_before != plane) {
	    if(view_before == view)  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_d%s3_vs_d%s4_clu0",viewlabel_before.Data(),viewlabel.Data()),dv_MMEcal_before,dv_MMEcal,1.);
	  }
	  if(view_before != view)  fHS->FillHisto2List("MMTrackDevel","MM_ECAL_dY3_vs_dX4_clu0",dv_MMEcal_before,dv_MMEcal,1.);
	}
	if(idist > 3 && idist%2!=0) {
	  if(plane_before != plane) {
	    if(view_before == view)  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_d%s_4p_clu0",viewlabel_before.Data(),viewlabel.Data()),dv_MMEcal_before,dv_MMEcal,1.);
	  }
	  if(view_before != view)  fHS->FillHisto2List("MMTrackDevel","MM_ECAL_dY_vs_dX_4p_clu0",dv_MMEcal_before,dv_MMEcal,1.);
	}
	  
	view_before = view;
	plane_before = plane;
	viewlabel_before = GeneralInfo::GetInstance()->GetMMViewLabel(view);
	dv_MMEcal_before = MMposAtEcal[1-view] - tempClu->GetPosition()[1-view];
      }
      
      for(int iwdist=0; iwdist<(int) wrong_distance.size(); iwdist++) {
	int itra = wrong_distance.at(iwdist).second;
	int quad = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(0)->GetMMchInfo().quad;
	int view = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(0)->GetMMchInfo().view;
	int plane = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(0)->GetMMchInfo().plane;
	TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(view);
	
	TVector3 MMposAtEcal = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetTracklet().ExtrapolationAtZ(z_Ecal);
	double dv_MMEcal = MMposAtEcal[1-view] - tempClu->GetPosition()[1-view];

	if(iwdist == 1) {
	  if(plane_before != plane) {
	    if(view_before == view)  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_wrong_d%s1_vs_d%s2_clu0",viewlabel_before.Data(),viewlabel.Data()),dv_MMEcal_before,dv_MMEcal,1.);
	  }
	  if(view_before != view)  fHS->FillHisto2List("MMTrackDevel","MM_ECAL_wrong_dY1_vs_dX2_clu0",dv_MMEcal_before,dv_MMEcal,1.);
	}
	if(iwdist == 3) {
	  if(plane_before != plane) {
	    if(view_before == view)  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_wrong_d%s3_vs_d%s4_clu0",viewlabel_before.Data(),viewlabel.Data()),dv_MMEcal_before,dv_MMEcal,1.);
	  }
	  if(view_before != view)  fHS->FillHisto2List("MMTrackDevel","MM_ECAL_wrong_dY3_vs_dX4_clu0",dv_MMEcal_before,dv_MMEcal,1.);
	}
	if(iwdist > 3 && iwdist%2!=0) {
	  if(plane_before != plane) {
	    if(view_before == view)  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_wrong_d%s_vs_d%s_4p_clu0",viewlabel_before.Data(),viewlabel.Data()),dv_MMEcal_before,dv_MMEcal,1.);
	  }
	  if(view_before != view)  fHS->FillHisto2List("MMTrackDevel","MM_ECAL_wrong_dY_vs_dX_4p_clu0",dv_MMEcal_before,dv_MMEcal,1.);
	}
	  
	view_before = view;
	plane_before = plane;
	viewlabel_before = GeneralInfo::GetInstance()->GetMMViewLabel(view);
	dv_MMEcal_before = MMposAtEcal[1-view] - tempClu->GetPosition()[1-view];
      }

      
      
  } 

  
  
  
  
  fEventCounter++;
  return true;
}

Bool_t MMTrackDevel::Finalize(){
  if(fGeneralInfo->isMC()){
    std::cout<<"This run is MC, MMTrackDevel checks do not apply"<<std::endl;
    return false;
  } 

  if (fVerbose) printf("---> Finalizing MMTrackDevel\n");
  
  std::cout<<"Finishing analyzing "<<fEventCounter<<" events"<<std::endl;
  return true;
}



