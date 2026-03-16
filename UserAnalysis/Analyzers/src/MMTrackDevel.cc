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

  fHS->BookHisto2List("MMTrackDevel",Form("MM_NHitsPerAPV_vs_APVID"),32,0,32,129,-0.5,128.5);
  fHS->BookHistoList("MMTrackDevel",Form("MM_ZeroHit"),32,0,32);
  
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
    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_pchi2_vs_Nhit_clu1_Q%dV%s",quad,viewlabel.Data()),50,0,50,100,0,1);
    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_pchi2_vs_Nhit_clu1_refit_Q%dV%s",quad,viewlabel.Data()),50,0,50,100,0,1);
    for (int quad = 0; quad<4; quad++) fHS->BookHistoList("MMTrackDevel",Form("MM_Nhit_clu1_Q%dV%s",quad,viewlabel.Data()),50,0,50);
    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_Zres_vs_%sres_clu1_Q%d",viewlabel.Data(),quad),100,-2,2,100,-2,2);
    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_Zres_vs_%sres_clu1_refit_Q%d",viewlabel.Data(),quad),100,-2,2,100,-2,2);
  }
  fHS->BookHisto2List("MMTrackDevel",Form("MM_NHitsPerAPV_vs_TrackMatchedCode"),2,-0.5,1.5,129,-0.5,128.5);

  
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

  for(int quad=0; quad<4; quad++) {
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_dX2_vs_dX1_clu0_Q%d",quad),100,-100,100,100,-100,100);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_dY2_vs_dY1_clu0_Q%d",quad),100,-100,100,100,-100,100);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_dX2_vs_dX1_cut_clu0_Q%d",quad),100,-100,100,100,-100,100);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_dY2_vs_dY1_cut_clu0_Q%d",quad),100,-100,100,100,-100,100);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_dX_vs_dY_clu0_Q%d",quad),100,-100,100,100,-100,100);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_wrong_dX2_vs_dX1_clu0_Q%d",quad),100,-100,100,100,-100,100);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_wrong_dY2_vs_dY1_clu0_Q%d",quad),100,-100,100,100,-100,100);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_wrong_dX2_vs_dX1_cut_clu0_Q%d",quad),100,-100,100,100,-100,100);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_wrong_dY2_vs_dY1_cut_clu0_Q%d",quad),100,-100,100,100,-100,100);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_wrong_dX_vs_dY_clu0_Q%d",quad),100,-100,100,100,-100,100);
    fHS->BookHistoList("MMTrackDevel",Form("MM_ECAL_chi2_clu0_Q%d",quad),100,0,100);
    fHS->BookHistoList("MMTrackDevel",Form("MM_ECAL_wrong_chi2_clu0_Q%d",quad),100,0,100);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_DdX12_vs_Dslope12_clu0_Q%d",quad),100,-0.1,0.1,200,-200,200);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_DdY12_vs_Dslope12_clu0_Q%d",quad),100,-0.1,0.1,200,-200,200);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_wrong_DdX12_vs_Dslope12_clu0_Q%d",quad),100,-0.1,0.1,200,-200,200);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_wrong_DdY12_vs_Dslope12_clu0_Q%d",quad),100,-0.1,0.1,200,-200,200);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_DinterX12_vs_Dslope12_clu0_Q%d",quad),100,-0.1,0.1,200,-200,200);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_DinterY12_vs_Dslope12_clu0_Q%d",quad),100,-0.1,0.1,200,-200,200);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_DinterX12_vs_Dslope12_cut_clu0_Q%d",quad),100,-0.1,0.1,200,-200,200);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_DinterY12_vs_Dslope12_cut_clu0_Q%d",quad),100,-0.1,0.1,200,-200,200);
    fHS->BookHistoList("MMTrackDevel",Form("MM_ECAL_tEcal_clu0_Q%d",quad),100,-200,200);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_tEcal_vs_dinterMM_clu0_Q%d",quad),20,-5,5,20,-200,200);
    fHS->BookHistoList("MMTrackDevel",Form("MM_ECAL_dz_clu0_Q%d",quad),100,-200,200);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_tEcal_vs_dtMM_clu0_Q%d",quad),100,-700,700,100,-200,200);
    
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_dZ_vs_dV_FIRST_NOCUT_clu0_Q%d",quad),100,-10,10,200,-20,20);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_dZ_vs_dV_SECOND_NOCUT_clu0_Q%d",quad),100,-10,10,200,-20,20);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_dZ_vs_dV_FIRST_CUT_clu0_Q%d",quad),100,-10,10,200,-20,20);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_dZ_vs_dV_SECOND_CUT_clu0_Q%d",quad),100,-10,10,200,-20,20);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_dZ_vs_Nhit_FIRST_NOCUT_clu0_Q%d",quad),10,0,10,200,-20,20);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_dZ_vs_Nhit_SECOND_NOCUT_clu0_Q%d",quad),10,0,10,200,-20,20);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_dZ_vs_Nhit_FIRST_CUT_clu0_Q%d",quad),10,0,10,200,-20,20);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_dZ_vs_Nhit_SECOND_CUT_clu0_Q%d",quad),10,0,10,200,-20,20);
  }

  fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_Ddz_vs_YECAL_clu1_Xn"),40,-400,400,100,-200,200);
  fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_Ddz_vs_YECAL_clu1_Xp"),40,-400,400,100,-200,200);
  fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_Ddz_vs_XECAL_clu1_Yn"),40,-400,400,100,-200,200);
  fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_Ddz_vs_XECAL_clu1_Yp"),40,-400,400,100,-200,200);

  fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_dY_vs_YECAL_clu1_Xn"),40,-400,400,100,-200,200);
  fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_dY_vs_YECAL_clu1_Xp"),40,-400,400,100,-200,200);
  fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_dX_vs_XECAL_clu1_Yn"),40,-400,400,100,-200,200);
  fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_dX_vs_XECAL_clu1_Yp"),40,-400,400,100,-200,200);
  for (int view = 0; view<2; view++){
    TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(view);    
    fHS->BookHistoList("MMTrackDevel",Form("MM_ECAL_d%s",viewlabel.Data()),100,-100,100);
  }
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
  int zero_hit=0;
  for (int apvid = 0; apvid<2; apvid++){
    for (int bdid = 0; bdid<16; bdid++){
      int NHitsPerAPV = fMMClusteringInstance->GetNHitsPerAPV(apvid,bdid);
      if(NHitsPerAPV == 0) zero_hit++;
      fHS->FillHisto2List("MMTrackDevel",Form("MM_NHitsPerAPV_vs_APVID"),apvid+2*bdid, NHitsPerAPV,1.);
    }
  }

  fHS->FillHistoList("MMTrackDevel",Form("MM_ZeroHit"),zero_hit,1.);

  
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
    -5, -10., -5., -5.  };  //updated were {0.,-10.,-5.,0.}
  double dxy_MMEcalMax[2][4] = {
    20,  20., 20., 20.,
    20,  20., 20., 20.};
  double  offsetdDZvsXY[2][4] = {
    -7.2, 0., 14.7, -2.5,
    10., 2.8, 10., 10.};
  //old 
  /* double offsetdDZvsXY[2][4] = {
    12.3,  -1.1, 15., 8. ,
    -10,  29.,  18.2, 3. };
  */
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
    //fHS->FillHisto2List("MMTrackDevel","MM_chi2_vs_angleDiffLevel0",dz,chi2,1.);
    
    fHS->FillHistoList("MMTrackDevel",Form("MM_Nhit_clu1_Q%dV%s",quad,viewlabel.Data()),Nhit,1.);
    vector<TVector3> residuals = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetTracklet().vres;
    //std::cout<<"residual size: "<<residuals.size()<<" hit size: "<<Nhit<<std::endl;
    for(int i=0; i<(int) residuals.size(); i++) {
      fHS->FillHisto2List("MMTrackDevel",Form("MM_Zres_vs_%sres_clu1_Q%d",viewlabel.Data(),quad),residuals.at(i)[1-view],residuals.at(i).Z(),1.);
    }
    int Nhit_true = (int) residuals.size();
    double pchi2 = ROOT::Math::chisquared_cdf_c(chi2, Nhit_true-3);
    fHS->FillHisto2List("MMTrackDevel",Form("MM_pchi2_vs_Nhit_clu1_Q%dV%s",quad,viewlabel.Data()),Nhit_true,pchi2,1.);
    

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

      bool tra0_otherview_matched = kFALSE;
      for(int itra0=0; itra0<(int) fMMClusteringInstance->GetMMClusterLength(0,0); itra0++) { 
	int view0 = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetHit(0)->GetMMchInfo().view;
	int quad0 = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetHit(0)->GetMMchInfo().quad;    
	if(view0 == view && quad0 != quad) continue;
	TVector3 MMposAtEcal_tra0 = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetTracklet().ExtrapolationAtZ(z_Ecal);
	double dv0_MMEcal = MMposAtEcal_tra0[view] - tempClu->GetPosition()[view];
	if(fabs(dv0_MMEcal) < 10) tra0_otherview_matched = kTRUE;
      }
      
      if(x_Ecal*signsQuadX[quad] > 0 && y_Ecal*signsQuadY[quad] > 0 && pchi2 > 0.5) {
	fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_dz_Q%d_clu1",viewlabel.Data(),quad),Ddz,dv_MMEcal,1.);
	if(tra0_otherview_matched && fabs(dz)<20) {//fabs(dv_MMEcal)<10) { //20) {

	  if(fabs(tempClu->GetPosition()[1-view])>100) fHS->FillHistoList("MMTrackDevel",Form("MM_ECAL_d%s",viewlabel.Data()),dv_MMEcal,1.);
	    
	  if(view == 1) { //X view
	    if(y_Ecal > 0) {
	      fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_XECAL_clu1_Yp",viewlabel.Data()),x_Ecal,dv_MMEcal,1.);
	      if(fabs(dv_MMEcal-offsetXY[1-view][quad])< dxy_MMEcalMax[1-view][quad]*0.5) fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_Ddz_vs_XECAL_clu1_Yp"),x_Ecal,Ddz,1.);
	    }
	    if(y_Ecal < 0) {
	      fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_XECAL_clu1_Yn",viewlabel.Data()),x_Ecal,dv_MMEcal,1.);
	      if(fabs(dv_MMEcal-offsetXY[1-view][quad])< dxy_MMEcalMax[1-view][quad]*0.5) fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_Ddz_vs_XECAL_clu1_Yn"),x_Ecal,Ddz,1.);
	    }
	  }
	  if(view == 0) { //Y view
	    if(x_Ecal > 0) {
	      fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_YECAL_clu1_Xp",viewlabel.Data()),y_Ecal,dv_MMEcal,1.);
	      if(fabs(dv_MMEcal-offsetXY[1-view][quad])< dxy_MMEcalMax[1-view][quad]*0.5) fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_Ddz_vs_YECAL_clu1_Xp"),y_Ecal,Ddz,1.);
	    }
	    if(x_Ecal < 0) {
	      fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_YECAL_clu1_Xn",viewlabel.Data()),y_Ecal,dv_MMEcal,1.);
	      if(fabs(dv_MMEcal-offsetXY[1-view][quad])< dxy_MMEcalMax[1-view][quad]*0.5) fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_Ddz_vs_YECAL_clu1_Xn"),y_Ecal,Ddz,1.);
	    }
	  }
	}
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
	    vector<TVector3> residuals_refit = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetTracklet().vres;
	    //std::cout<<"residual size: "<<residuals.size()<<" hit size: "<<Nhit<<std::endl;
	    for(int i=0; i<(int) residuals_refit.size(); i++) {
	      fHS->FillHisto2List("MMTrackDevel",Form("MM_Zres_vs_%sres_clu1_refit_Q%d",viewlabel.Data(),quad),residuals_refit.at(i)[1-view],residuals_refit.at(i).Z(),1.);
	    }
	    int Nhit_true_refit = (int) residuals_refit.size();
	    double chi2_refit = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetTracklet().chi2;
	    double pchi2_refit = ROOT::Math::chisquared_cdf_c(chi2_refit, Nhit_true_refit-3);
	    fHS->FillHisto2List("MMTrackDevel",Form("MM_pchi2_vs_Nhit_clu1_refit_Q%dV%s",quad,viewlabel.Data()),Nhit_true_refit,pchi2_refit,1.);

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
    int apvid = 0;
    int bdid = 0;
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
	fHS->FillHisto2List("MMTrackDevel",Form("MM_NHitsPerAPV_vs_TrackMatchedCode"),(responseCode.at(iidx) & (1<<vw)) >0, NHitsPerAPV);
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


  vector<pair<double,int>> distance[4], wrong_distance[4];
  double dx0=10000, dx1=10000, dy0=10000, dy1=10000;
  double wdx0=10000, wdx1=10000, wdy0=10000, wdy1=10000;

  int wrong_quadY[4] = {3,2,1,0};
  int wrong_quadX[4] = {1,0,3,2};
  
  for(int iidx=0; iidx<(int) cluIndices.size(); iidx++) {
    TRecoVCluster* tempClu = ECal_clEvent->Element(cluIndices.at(iidx));
    double z_Ecal = GeneralInfo::GetInstance()->GetCOG().Z()+6.5*11.;
    for(int q=0; q<4; q++) {
      distance[q].clear();
      wrong_distance[q].clear();
    }

    double x_Ecal = tempClu->GetPosition().X();
    double y_Ecal = tempClu->GetPosition().Y();
    int Ecal_quad;
    if (x_Ecal < 0 && y_Ecal < 0) Ecal_quad = 0;
    else if (x_Ecal < 0 && y_Ecal > 0) Ecal_quad = 1;
    else if (x_Ecal > 0 && y_Ecal > 0) Ecal_quad = 2;
    else Ecal_quad = 3;

    double t_Ecal = tempClu->GetTime()+440;
    fHS->FillHistoList("MMTrackDevel",Form("MM_ECAL_tEcal_clu0_Q%d",Ecal_quad),t_Ecal,1.);
    
    for(int itra=0; itra<(int) fMMClusteringInstance->GetMMClusterLength(0,0); itra++) {
      int Nhit = (int) fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHitsVectorSize();
      int quad = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(0)->GetMMchInfo().quad;
      int view = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(0)->GetMMchInfo().view;
      int plane = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(0)->GetMMchInfo().plane;
      int other_view = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(0)->GetMMchInfo().otherview;
      TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(view);
      
      TVector3 MMposAtEcal = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetTracklet().ExtrapolationAtZ(z_Ecal);
      double dv_MMEcal = MMposAtEcal[1-view] - tempClu->GetPosition()[1-view];

      if(quad == wrong_quadX[Ecal_quad]) {
	if(view == 1 && plane == 0 && fabs(dv_MMEcal) < wdx0) wdx0 = fabs(dv_MMEcal);
	if(view == 1 && plane == 1 && fabs(dv_MMEcal) < wdx1) wdx1 = fabs(dv_MMEcal);
	if(view == 1) wrong_distance[Ecal_quad].emplace_back(dv_MMEcal,itra);
      }
      if(quad == wrong_quadY[Ecal_quad]) {
	if(view == 0 && plane == 0 && fabs(dv_MMEcal) < wdy0) wdy0 = fabs(dv_MMEcal);
	if(view == 0 && plane == 1 && fabs(dv_MMEcal) < wdy1) wdy1 = fabs(dv_MMEcal);
	if(view == 0) wrong_distance[Ecal_quad].emplace_back(dv_MMEcal,itra);
      }
      
      if(Ecal_quad != quad) continue;
      distance[quad].emplace_back(dv_MMEcal, itra);

      if(view == 1 && plane == 0 && fabs(dv_MMEcal) < dx0) dx0 = fabs(dv_MMEcal);
      if(view == 1 && plane == 1 && fabs(dv_MMEcal) < dx1) dx1 = fabs(dv_MMEcal);
      if(view == 0 && plane == 0 && fabs(dv_MMEcal) < dy0) dy0 = fabs(dv_MMEcal);
      if(view == 0 && plane == 1 && fabs(dv_MMEcal) < dy1) dy1 = fabs(dv_MMEcal);
    }

    double chi2 = dx0*dx0 + dx1*dx1 + dy0*dy0 + dy1*dy1;
    double wchi2 = wdx0*wdx0 + wdx1*wdx1 + wdy0*wdy0 + wdy1*wdy1; 
    fHS->FillHistoList("MMTrackDevel",Form("MM_ECAL_chi2_clu0_Q%d",Ecal_quad),chi2/100.,1.);
    fHS->FillHistoList("MMTrackDevel",Form("MM_ECAL_wrong_chi2_clu0_Q%d",Ecal_quad),wchi2/100.,1.); 

    int q = Ecal_quad;
    
    for(int idist=0; idist<(int) distance[q].size(); idist++) {
      int itra = distance[q].at(idist).second;
      int iview = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(0)->GetMMchInfo().view;
      int iplane = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(0)->GetMMchInfo().plane;
      TString iviewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(iview);
      
      int iNhit = (int) fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHitsVectorSize();
      double islope = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetTracklet().slope;
      double iinter = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetTracklet().inter;
      
      double idv_MMEcal = distance[q].at(idist).first;

      
      for(int jdist=idist+1; jdist<(int) distance[q].size(); jdist++) {
	int jtra = distance[q].at(jdist).second;
	int jview = fMMClusteringInstance->GetMMCluster(jtra,0,0)->GetHit(0)->GetMMchInfo().view;
	int jplane = fMMClusteringInstance->GetMMCluster(jtra,0,0)->GetHit(0)->GetMMchInfo().plane;
	TString jviewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(jview);

	int jNhit = (int) fMMClusteringInstance->GetMMCluster(jtra,0,0)->GetHitsVectorSize();
	double jslope = fMMClusteringInstance->GetMMCluster(jtra,0,0)->GetTracklet().slope;
	double jinter = fMMClusteringInstance->GetMMCluster(jtra,0,0)->GetTracklet().inter;
	
	double jdv_MMEcal = distance[q].at(jdist).first;

	
        
	if(iplane != jplane) {
	  if(iview == jview) {
	    double dv_inter;
	    if(iplane == 0) dv_inter = iinter-jinter;
	    else dv_inter = -iinter+jinter;
	    
	    fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_d%s2_vs_d%s1_clu0_Q%d",iviewlabel.Data(),jviewlabel.Data(),q),idv_MMEcal,jdv_MMEcal,1.);
	    //residues
	    for(int ih=0; ih < iNhit; ih++) {
	      MMSoftHit* ihit = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(ih);
	      double v_hit = ihit->GetPosition()[1-iview];
	      double z_hit = ihit->GetZfromTime(1.) - GeneralInfo::GetInstance()->GetMMPosPlaneZ(2);
	      
	      double v_res = v_hit - (islope*z_hit+iinter);
	      double z_res = z_hit - (v_hit-iinter)/islope;

	      fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_dZ_vs_dV_FIRST_NOCUT_clu0_Q%d",q),v_res,z_res,1.);
	      fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_dZ_vs_Nhit_FIRST_NOCUT_clu0_Q%d",q),iNhit,z_res,1.);
	    }
	    for(int jh=0; jh < jNhit; jh++) {
	      MMSoftHit* jhit = fMMClusteringInstance->GetMMCluster(jtra,0,0)->GetHit(jh);
	      double v_hit = jhit->GetPosition()[1-jview];
	      double z_hit = jhit->GetZfromTime(1.) - GeneralInfo::GetInstance()->GetMMPosPlaneZ(2);

	      double v_res = v_hit - (jslope*z_hit+jinter);
	      double z_res = z_hit - (v_hit-jinter)/jslope;

	      fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_dZ_vs_dV_SECOND_NOCUT_clu0_Q%d",q),v_res,z_res,1.);
	      fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_dZ_vs_Nhit_SECOND_NOCUT_clu0_Q%d",q),jNhit,z_res,1.);
	    }

	    
	    if(fabs(islope-jslope)<0.025 && fabs(iinter-jinter)<5) {

	      fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_d%s2_vs_d%s1_cut_clu0_Q%d",iviewlabel.Data(),jviewlabel.Data(),q),idv_MMEcal,jdv_MMEcal,1.);

	      if(fabs(idv_MMEcal)<20 && fabs(jdv_MMEcal)<20) {
		//residues
		for(int ih=0; ih < iNhit; ih++) {
		  MMSoftHit* ihit = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(ih);
		  double v_hit = ihit->GetPosition()[1-iview];
		  double z_hit = ihit->GetZfromTime(1.) - GeneralInfo::GetInstance()->GetMMPosPlaneZ(2);
		  
		  double v_res = v_hit - (islope*z_hit+iinter);
		  double z_res = z_hit - (v_hit-iinter)/islope;
		  
		  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_dZ_vs_dV_FIRST_CUT_clu0_Q%d",q),v_res,z_res,1.);
		  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_dZ_vs_Nhit_FIRST_CUT_clu0_Q%d",q),iNhit,z_res,1.);
		}
		for(int jh=0; jh < jNhit; jh++) {
		  MMSoftHit* jhit = fMMClusteringInstance->GetMMCluster(jtra,0,0)->GetHit(jh);
		  double v_hit = jhit->GetPosition()[1-jview];
		  double z_hit = jhit->GetZfromTime(1.) - GeneralInfo::GetInstance()->GetMMPosPlaneZ(2);
		  
		  double v_res = v_hit - (jslope*z_hit+jinter);
		  double z_res = z_hit - (v_hit-jinter)/jslope;
		  
		  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_dZ_vs_dV_SECOND_CUT_clu0_Q%d",q),v_res,z_res,1.);
		  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_dZ_vs_Nhit_SECOND_CUT_clu0_Q%d",q),jNhit,z_res,1.);
		}
		
		double m_avg = 0.5*(islope+jslope);
		double dz = dv_inter/m_avg;

		fHS->FillHistoList("MMTrackDevel",Form("MM_ECAL_dz_clu0_Q%d",q),dz,1.);
		
		double dt = dz/GeneralInfo::GetInstance()->GetMMDriftVelocity();

		fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_tEcal_vs_dinterMM_clu0_Q%d",q),dv_inter,t_Ecal,1.);
		fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_tEcal_vs_dtMM_clu0_Q%d",q),dt,t_Ecal,1.);
	      }
	    }
	    
	    fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_Dd%s12_vs_Dslope12_clu0_Q%d",iviewlabel.Data(),q),islope-jslope,idv_MMEcal-jdv_MMEcal,1.);
	    fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_Dinter%s12_vs_Dslope12_clu0_Q%d",iviewlabel.Data(),q),islope-jslope,dv_inter,1.);
	    if(fabs(idv_MMEcal)<20 && fabs(jdv_MMEcal)<20) fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_Dinter%s12_vs_Dslope12_cut_clu0_Q%d",iviewlabel.Data(),q),islope-jslope,iinter-jinter,1.);
	  }
	}
	if(iview != jview) fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_dX_vs_dY_clu0_Q%d",q),idv_MMEcal,jdv_MMEcal,1.); 
        
      }
      
    }
    
    for(int iwdist=0; iwdist<(int) wrong_distance[q].size(); iwdist++) {
      int itra = wrong_distance[q].at(iwdist).second;
      int iview = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(0)->GetMMchInfo().view;
      int iplane = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetHit(0)->GetMMchInfo().plane;
      TString iviewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(iview);
      
      double islope = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetTracklet().slope;
      double iinter = fMMClusteringInstance->GetMMCluster(itra,0,0)->GetTracklet().inter;
      double idv_MMEcal = wrong_distance[q].at(iwdist).first;
      
      for(int jwdist=iwdist+1; jwdist<(int) wrong_distance[q].size(); jwdist++) {
	int jtra = wrong_distance[q].at(jwdist).second;
	int jview = fMMClusteringInstance->GetMMCluster(jtra,0,0)->GetHit(0)->GetMMchInfo().view;
	int jplane = fMMClusteringInstance->GetMMCluster(jtra,0,0)->GetHit(0)->GetMMchInfo().plane;
	TString jviewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(jview);

	double jslope = fMMClusteringInstance->GetMMCluster(jtra,0,0)->GetTracklet().slope;
	double jinter = fMMClusteringInstance->GetMMCluster(jtra,0,0)->GetTracklet().inter;
	double jdv_MMEcal = wrong_distance[q].at(jwdist).first;
	
	if(iplane != jplane) {
	  if(iview == jview) {
	    fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_wrong_d%s2_vs_d%s1_clu0_Q%d",iviewlabel.Data(),jviewlabel.Data(),q),idv_MMEcal,jdv_MMEcal,1.);
	    if(fabs(islope-jslope)<0.025 && fabs(iinter-jinter)<5) {
	      
	      fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_wrong_d%s2_vs_d%s1_cut_clu0_Q%d",iviewlabel.Data(),jviewlabel.Data(),q),idv_MMEcal,jdv_MMEcal,1.);
	   
	    }
	    fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_wrong_Dd%s12_vs_Dslope12_clu0_Q%d",iviewlabel.Data(),q),islope-jslope,idv_MMEcal-jdv_MMEcal,1.);
	  }
	  
	}
	if(iview != jview) fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_wrong_dX_vs_dY_clu0_Q%d",q),idv_MMEcal,jdv_MMEcal,1.); 
        
      }
      
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



