#include "MMTrackDevel.hh"
#include "TGraph.h"
#include "TString.h"
#include "TF1.h"
#include "TMath.h"
#include "GeneralInfo.hh"
#include "ECalSel.hh"

#define BFIELD true

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

  //  fevent=0;
  //ftree = new TTree("ftree","MMTrackDevelNtuple");
  //ftree->Branch("fevent",fevent,"fevent/I");
  return true;
}

Bool_t MMTrackDevel::InitHistos(Int_t nRun){
  // MMTrackDevel directory will contain all histograms related to this analysis

  fHS->CreateList("MMTrackDevel");
  cout<<" Creating MMTrackDevel Hystograms for Run "<<nRun<<" "<<endl;
  //  fHS->BookHisto2List("MMTrackDevel","cluDistance",100,0,100,200,-100,100);
  fHS->BookHisto2List("MMTrackDevel","MM_Nclus0_vs_NHits",100,0,3000.,500,0,500); 
  fHS->BookHisto2List("MMTrackDevel","MM_Nclus1_vs_NHits",100,0,3000.,500,0,500); 

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
        
    // for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_Nhit_Q%d_clu0",viewlabel.Data(),quad),10,0,10,1000,-100,100);
    
    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_pchi2_vs_Nhit_clu1_refit_Q%dV%s",quad,viewlabel.Data()),50,0,50,100,0,1);
    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_pchi2_vs_Nhit_clu1_simplefit_Q%dV%s",quad,viewlabel.Data()),50,0,50,100,0,1);
    
    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_Zres_vs_%sres_clu1_refit_Q%d",viewlabel.Data(),quad),100,-10,10,100,-10,10);
    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_Zres_vs_%sres_clu1_simplefit_Q%d",viewlabel.Data(),quad),100,-10,10,100,-10,10);
    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_clu1_refit_Q%d",viewlabel.Data(),quad),100,0,1,100,-50,50);
    
    fHS->BookHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_4hit_clu1_refit",viewlabel.Data()),20,0,1,20,-50,50);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_5hit_clu1_refit",viewlabel.Data()),20,0,1,20,-50,50);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_6hit_clu1_refit",viewlabel.Data()),20,0,1,20,-50,50);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_7hit_clu1_refit",viewlabel.Data()),20,0,1,20,-50,50);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_8hit_clu1_refit",viewlabel.Data()),20,0,1,20,-50,50);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_910hit_clu1_refit",viewlabel.Data()),20,0,1,20,-50,50);
    
    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_clu1_simplefit_Q%d",viewlabel.Data(),quad),100,0,1,100,-50,50);

    fHS->BookHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_4hit_clu1_simplefit",viewlabel.Data()),20,0,1,20,-50,50);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_5hit_clu1_simplefit",viewlabel.Data()),20,0,1,20,-50,50);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_6hit_clu1_simplefit",viewlabel.Data()),20,0,1,20,-50,50);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_7hit_clu1_simplefit",viewlabel.Data()),20,0,1,20,-50,50);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_8hit_clu1_simplefit",viewlabel.Data()),20,0,1,20,-50,50);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_910hit_clu1_simplefit",viewlabel.Data()),20,0,1,20,-50,50);

    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_refit_vs_d%sMMECAL_simplefit_clu1_Q%d",viewlabel.Data(),viewlabel.Data(),quad),100,-50,50,100,-50,50);
    
  }

  fHS->BookHisto2List("MMTrackDevel",Form("MM_dVMMECAL_vs_Nhit_refit_clu1"),11,4,15,100,-100,100);

  for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_pchi2_vs_Nhit_clu0_Q%d",quad),10,0,10,100,0,1);
  for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_Vres_vs_Nhit_clu0_Q%d",quad),10,0,10,500,-10,10);
  
  fHS->BookHisto2List("MMTrackDevel",Form("MM_Vres_vs_pchi2_3hit_clu0"),100,0,1,500,-10,10);
  fHS->BookHisto2List("MMTrackDevel",Form("MM_Vres_vs_pchi2_4hit_clu0"),100,0,1,500,-10,10);
  fHS->BookHisto2List("MMTrackDevel",Form("MM_Vres_vs_pchi2_5hit_clu0"),100,0,1,500,-10,10);
  fHS->BookHisto2List("MMTrackDevel",Form("MM_Vres_vs_pchi2_67hit_clu0"),100,0,1,500,-10,10);

  fHS->BookHisto2List("MMTrackDevel",Form("MM_NHitsPerAPV_vs_TrackMatchedCode"),2,-0.5,1.5,129,-0.5,128.5);
  fHS->BookHisto2List("MMTrackDevel",Form("MM_dyECAL_vs_dxECAL_clu0"),300,-300,300,300,-300,300);
  fHS->BookHisto2List("MMTrackDevel",Form("MM_dVMMECAL_vs_pchi2_right_singleALL_clu0"),50,0,1,300,-300,300);
  fHS->BookHisto2List("MMTrackDevel",Form("MM_dVMMECAL_vs_pchi2_right_double_clu0"),50,0,1,300,-300,300);

  for(int pl=0; pl<2; pl++) {
    for(int vw=0; vw<2; vw++) {
      TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(vw);    
      fHS->BookHisto2List("MMTrackDevel",Form("MM_dVMMECAL_vs_pchi2_right_3h_P%dV%s_clu0",pl,viewlabel.Data()),50,0,1,300,-300,300);
      fHS->BookHisto2List("MMTrackDevel",Form("MM_dVMMECAL_vs_pchi2_wrong_3h_P%dV%s_clu0",pl,viewlabel.Data()),50,0,1,300,-300,300);
    }
  }

  fHS->BookHisto2List("MMTrackDevel",Form("MM_dVMMECAL_vs_pchi2_right_clu1"),50,0,1,300,-300,300);
  fHS->BookHisto2List("MMTrackDevel",Form("MM_dVMMECAL_vs_pchi2_wrong_clu1"),50,0,1,300,-300,300);

  for(int vw=0; vw<2; vw++) {
   TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(vw);
   fHS->BookHisto2List("MMTrackDevel",Form("MM_dVMMECAL_vs_pchi2_right_V%s_clu1",viewlabel.Data()),50,0,1,300,-300,300);
   fHS->BookHisto2List("MMTrackDevel",Form("MM_dVMMECAL_vs_pchi2_wrong_V%s_clu1",viewlabel.Data()),50,0,1,300,-300,300);
  }
  
  for (int view = 0; view<2; view++){
    TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(view);    
    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_Ddz_vs_%s_Q%d_atMesh_clu1",viewlabel.Data(),quad),700,-350,350,400,-200,200);
    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_z_first_last_hit_vs_%s_Q%d_atMesh_clu1",viewlabel.Data(),quad),700,-350,350,400,-200,200);
    
  }
  
  for (int bdid = 0; bdid < 16; bdid++) {
    fHS->BookHisto2List("MMTrackDevel",Form("MM_Time_vs_channel_bdid%d",bdid),256,-0.5,255.5,900,-150,750);
  }
  //for(int quad=0; quad<4; quad++) fHS->BookHistoList("MMTrackDevel",Form("MM_Z_first_last_hit_clu1_matched_Q%d",quad),100,fGeneralInfo->GetMMPosPlaneZ(0)-50,fGeneralInfo->GetMMPosPlaneZ(1)+50);
  fHS->BookHisto2List("MMTrackDevel",Form("MM_QuadrantID_vs_TrackMatchedCode"),8,0,8,4,0,4);
  fHS->BookHisto2List("MMTrackDevel",Form("MM_TrackMatchedCode2vs1"),4,0,4,4,0,4);

  fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_m_vs_c_clu1"),400,-400,400,200,-0.5,0.5);
  
  /* //CALIBRATION
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
    }*/

  //fHS->BookNtupleList("MMTrackDevel","ftree");
  
  return true;
}

Bool_t MMTrackDevel::Process(){

  // retrieve ecal info

  TRecoVClusCollection* ECal_clEvent = fEvent->ECalRecoCl;
  std::vector<int> cluIndices;
  std::vector<double> cluTime_avg;

  if(!BFIELD) { //Normal RunIV data
    
    if (ECalSel::GetInstance()->getNECalEvents() == 0) return kFALSE;
    
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
    
  }
  else { //SINGLE PARTICLE / B Field on RUNs
    double resCALO = 17; //from Elisa!
    for(int i=0; i<ECal_clEvent->GetNElements(); i++) {
      TRecoVCluster* tempClu = ECal_clEvent->Element(i);
      double EClu = tempClu->GetEnergy();
      if(fabs(EClu - GeneralInfo::GetInstance()->GetBeamEnergy()*GeneralInfo::GetInstance()->GetGlobalESlope())>3*resCALO) continue;
      cluIndices.push_back(i);
      cluTime_avg.push_back(tempClu->GetTime()+440.); //tanto non si usa!!!
    }

  }
  
  if (cluIndices.size() == 0) return kFALSE; // cluster pairs
  // retrieve trigger info  
  
  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  //long long int fTimeStamp =(long long int) fEvent->RecoEvent->GetEventTime().GetSec();
  if(trigMask & (1 << 0)) {
  }

  //int nclus = fEvent->MMRecoCl->GetNElements();
  int nhits = fEvent->MMRecoEvent->GetNHits();
  //fHS->FillHisto2List("MMTrackDevel","MM_Nclus_vs_NHits",nhits,nclus,1.);

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
  
  fHS->FillHisto2List("MMTrackDevel","MM_Nclus0_vs_NHits",nhits,fMMClusteringInstance->GetMMClusterLength(0,0),1.);
  fHS->FillHisto2List("MMTrackDevel","MM_Nclus1_vs_NHits",nhits,fMMClusteringInstance->GetMMClusterLength(0,1),1.);

  int signsQuadX[4] = {-1,-1,1,1};
  int signsQuadY[4] = {-1,1,1,-1};
  double offsetXY[2][4]     = {
    0,   0.,-10.,-10. ,
    -5, -10., -5., -5.  };  //updated were {0.,-10.,-5.,0.}
  double dxy_MMEcalMax[2][4] = {
    20,  20., 20., 20.,
    20,  20., 20., 20.};
  double  offsetdDZvsXY[2][4] = {
    -8.8, 3., 14.3, 1.6,
    10., 9., 10., 10.};
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

    int quad = fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetHit(0)->GetMMchInfo().quad;
    int view = fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetHit(0)->GetMMchInfo().view;
    TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(view);
    
    double chi2 = fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetTracklet().chi2;
    double pchi2 = ROOT::Math::chisquared_cdf_c(chi2, Nhit-2);
    fHS->FillHisto2List("MMTrackDevel",Form("MM_pchi2_vs_Nhit_clu0_Q%d",quad),Nhit,pchi2,1.);

    vector<TVector3> residuals = fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetTracklet().vres;
    //std::cout<<"residual size: "<<residuals.size()<<" hit size: "<<Nhit<<std::endl;
    for(int i=0; i<(int) residuals.size(); i++) {
      fHS->FillHisto2List("MMTrackDevel",Form("MM_Vres_vs_Nhit_clu0_Q%d",quad),Nhit,residuals.at(i)[1-view],1.);

      if(Nhit == 3) fHS->FillHisto2List("MMTrackDevel",Form("MM_Vres_vs_pchi2_3hit_clu0"),pchi2,residuals.at(i)[1-view],1.);
      if(Nhit == 4) fHS->FillHisto2List("MMTrackDevel",Form("MM_Vres_vs_pchi2_4hit_clu0"),pchi2,residuals.at(i)[1-view],1.);
      if(Nhit == 5) fHS->FillHisto2List("MMTrackDevel",Form("MM_Vres_vs_pchi2_5hit_clu0"),pchi2,residuals.at(i)[1-view],1.);
      if(Nhit >= 6) fHS->FillHisto2List("MMTrackDevel",Form("MM_Vres_vs_pchi2_67hit_clu0"),pchi2,residuals.at(i)[1-view],1.);

    }
    
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

  /*for(int iclu=0; iclu<(int) fMMClusteringInstance->GetMMClusterLength(0,1); iclu++) {
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
    
    vector<TVector3> residuals = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetTracklet().vres;
    //std::cout<<"residual size: "<<residuals.size()<<" hit size: "<<Nhit<<std::endl;
    //for(int i=0; i<(int) residuals.size(); i++) {
    //  fHS->FillHisto2List("MMTrackDevel",Form("MM_Zres_vs_%sres_clu1_Q%d",viewlabel.Data(),quad),residuals.at(i)[1-view],residuals.at(i).Z(),1.);
    //}
    int Nhit_true = (int) residuals.size();
    double pchi2 = ROOT::Math::chisquared_cdf_c(chi2, Nhit_true-3);
    

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
      double t_Ecal = tempClu->GetTime();

      double dv_MMEcal = MMposAtEcal[1-view] - tempClu->GetPosition()[1-view];

      if(x_Ecal*signsQuadX[quad] > 0 && y_Ecal*signsQuadY[quad] > 0) { //matching ECal clu position with MM quad 
	
	if(fabs(dv_MMEcal)<50) {
	  double dv_MMEcal_refit=-999;
	  double dv_MMEcal_simple=-999;
	  if(fMMClusteringInstance->GetMMCluster(iclu,0,1)->SimpleFitWithClusterTime(x_Ecal,y_Ecal,t_Ecal)) {
	    vector<TVector3> residuals_refit = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetTracklet().vres;
	    //std::cout<<"residual size: "<<residuals.size()<<" hit size: "<<Nhit<<std::endl;
	    for(int i=0; i<(int) residuals_refit.size(); i++) {
	      fHS->FillHisto2List("MMTrackDevel",Form("MM_Zres_vs_%sres_clu1_simplefit_Q%d",viewlabel.Data(),quad),residuals_refit.at(i)[1-view],residuals_refit.at(i).Z(),1.);
	    }
	    int Nhit_true_refit = (int) residuals_refit.size();
	    double chi2_refit = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetTracklet().chi2;
	    double pchi2_refit = ROOT::Math::chisquared_cdf_c(chi2_refit, Nhit_true_refit-2);
	    fHS->FillHisto2List("MMTrackDevel",Form("MM_pchi2_vs_Nhit_clu1_simplefit_Q%dV%s",quad,viewlabel.Data()),Nhit_true_refit,pchi2_refit,1.);
	    TVector3 MMposAtEcal_simple = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetTracklet().ExtrapolationAtZ(z_Ecal);//tempClu->GetPosition().Z());
	    dv_MMEcal_simple = MMposAtEcal_simple[1-view] - tempClu->GetPosition()[1-view];
	    //fHS->FillHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_%sECAL_clu1_simplefit_Q%d",viewlabel.Data(),viewlabel.Data(),quad),tempClu->GetPosition()[1-view],dv_MMEcal_simple,1.);
	    fHS->FillHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_clu1_simplefit_Q%d",viewlabel.Data(),quad),pchi2_refit,dv_MMEcal_simple,1.);
	    if(Nhit_true_refit == 4) fHS->FillHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_4hit_clu1_simplefit",viewlabel.Data()),pchi2_refit,dv_MMEcal_simple,1.);
	    if(Nhit_true_refit == 5) fHS->FillHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_5hit_clu1_simplefit",viewlabel.Data()),pchi2_refit,dv_MMEcal_simple,1.);
	    if(Nhit_true_refit == 6) fHS->FillHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_6hit_clu1_simplefit",viewlabel.Data()),pchi2_refit,dv_MMEcal_simple,1.);
	    if(Nhit_true_refit == 7) fHS->FillHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_7hit_clu1_simplefit",viewlabel.Data()),pchi2_refit,dv_MMEcal_simple,1.);
	    if(Nhit_true_refit == 8) fHS->FillHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_8hit_clu1_simplefit",viewlabel.Data()),pchi2_refit,dv_MMEcal_simple,1.);
	    if(Nhit_true_refit >= 9) fHS->FillHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_910hit_clu1_simplefit",viewlabel.Data()),pchi2_refit,dv_MMEcal_simple,1.);

	    if(fabs(dv_MMEcal) < 10 && pchi2_refit>0.5) {
	      double slope = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetTracklet().slope;
	      double inter = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetTracklet().inter;
	      fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_m_vs_c_clu1"),inter,slope,1.);
	    }
	  }
	  
	  if(fMMClusteringInstance->GetMMCluster(iclu,0,1)->FitWithClusterTime(x_Ecal,y_Ecal,t_Ecal)) {
	    vector<TVector3> residuals_refit = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetTracklet().vres;
	    //std::cout<<"residual size: "<<residuals.size()<<" hit size: "<<Nhit<<std::endl;
	    for(int i=0; i<(int) residuals_refit.size(); i++) {
	      fHS->FillHisto2List("MMTrackDevel",Form("MM_Zres_vs_%sres_clu1_refit_Q%d",viewlabel.Data(),quad),residuals_refit.at(i)[1-view],residuals_refit.at(i).Z(),1.);
	    }
	    int Nhit_true_refit = (int) residuals_refit.size();
	    //std::cout<<"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB Nhit: "<<Nhit_true_refit<<std::endl;
	    double chi2_refit = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetTracklet().chi2;
	    double pchi2_refit = ROOT::Math::chisquared_cdf_c(chi2_refit, Nhit_true_refit-2);
	    fHS->FillHisto2List("MMTrackDevel",Form("MM_pchi2_vs_Nhit_clu1_refit_Q%dV%s",quad,viewlabel.Data()),Nhit_true_refit,pchi2_refit,1.);
	    TVector3 MMposAtEcal_refit = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetTracklet().ExtrapolationAtZ(z_Ecal);//tempClu->GetPosition().Z());
	    dv_MMEcal_refit = MMposAtEcal_refit[1-view] - tempClu->GetPosition()[1-view];
	    fHS->FillHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_clu1_refit_Q%d",viewlabel.Data(),quad),pchi2_refit,dv_MMEcal_refit,1.);
	    if(Nhit_true_refit == 4) fHS->FillHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_4hit_clu1_refit",viewlabel.Data()),pchi2_refit,dv_MMEcal_refit,1.);
	    if(Nhit_true_refit == 5) fHS->FillHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_5hit_clu1_refit",viewlabel.Data()),pchi2_refit,dv_MMEcal_refit,1.);
	    if(Nhit_true_refit == 6) fHS->FillHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_6hit_clu1_refit",viewlabel.Data()),pchi2_refit,dv_MMEcal_refit,1.);
	    if(Nhit_true_refit == 7) fHS->FillHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_7hit_clu1_refit",viewlabel.Data()),pchi2_refit,dv_MMEcal_refit,1.);
	    if(Nhit_true_refit == 8) fHS->FillHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_8hit_clu1_refit",viewlabel.Data()),pchi2_refit,dv_MMEcal_refit,1.);
	    if(Nhit_true_refit >= 9) fHS->FillHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_vs_pchi2_910hit_clu1_refit",viewlabel.Data()),pchi2_refit,dv_MMEcal_refit,1.);

	    if(pchi2_refit>0.5) fHS->FillHisto2List("MMTrackDevel",Form("MM_dVMMECAL_vs_Nhit_refit_clu1"),Nhit_true_refit,dv_MMEcal,1.);
	  }

	  
	  fHS->FillHisto2List("MMTrackDevel",Form("MM_d%sMMECAL_refit_vs_d%sMMECAL_simplefit_clu1_Q%d",viewlabel.Data(),viewlabel.Data(),quad),dv_MMEcal_simple,dv_MMEcal_refit);
	  //This below must be rewritten after one has discarted the hits in the track clu1 (now the hit rejection is not doing it properly)

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
  */
    // plot for response evaluation per track
  
  for(int iidx=0; iidx<(int) cluIndices.size(); iidx++) {
    //fevent++;
    TRecoVCluster* tempClu = ECal_clEvent->Element(cluIndices.at(iidx));
    double y_Ecal = tempClu->GetPosition().Y();
    double x_Ecal = tempClu->GetPosition().X();
    double t_Ecal = tempClu->GetTime();
    double z_Ecal = GeneralInfo::GetInstance()->GetCOG().Z()+6.5*11.;
    double E_Ecal = tempClu->GetEnergy();
    
    // loop over level-zero tracks, find the best levelzero for each view
    //double pchi2bestPair = -999;
    //double dv_MMEcalbestPair[2] = {-999,-999};
    
    double pchi2best0[2][2] = {{-999, -999},{-999, -999}};
    double dv_MMEcalbest0[2][2] = {{-999, -999},{-999, -999}};
    double pchi2best_wrong0[2][2] = {{-999, -999},{-999, -999}};
    double dv_MMEcalbest_wrong0[2][2] = {{-999, -999},{-999, -999}};
    int itrack0_good[2][2] = {{-999, -999},{-999, -999}}, itrack0_wrong[2][2] = {{-999, -999},{-999, -999}};
    int quad_track0_good[2][2] = {{-999,-999},{-999,-999}}, quad_track0_wrong[2][2] = {{-999,-999},{-999,-999}};
    int Nhit_track0_good[2][2] = {{-999,-999},{-999,-999}}, Nhit_track0_wrong[2][2] = {{-999,-999},{-999,-999}};
    double slope_track0_good[2][2] = {{-999,-999},{-999,-999}}, slope_track0_wrong[2][2] = {{-999,-999},{-999,-999}};
    double inter_track0_good[2][2] = {{-999,-999},{-999,-999}}, inter_track0_wrong[2][2] = {{-999,-999},{-999,-999}};
      
    for(int itra0=0; itra0<(int) fMMClusteringInstance->GetMMClusterLength(0,0); itra0++) {
      int nhit_tmp0 = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetHitsVectorSize();
      if (nhit_tmp0 < 3) continue; //only consider tracks with 3+ hits

      int quad0 = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetHit(0)->GetMMchInfo().quad;
      int view0 = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetHit(0)->GetMMchInfo().view;
      int plane0 = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetHit(0)->GetMMchInfo().plane;
      	
      if(x_Ecal*signsQuadX[quad0] > 0 && y_Ecal*signsQuadY[quad0] > 0) { //matching ECal clu position with MM quad 
	double pchi2_tmp0 = ROOT::Math::chisquared_cdf_c(fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetTracklet().chi2,nhit_tmp0-2);
	TVector3 MMposAtEcal0 = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetTracklet().ExtrapolationAtZ(z_Ecal);//tempClu->GetPosition().Z());
	double dv0 = MMposAtEcal0[1-view0] - tempClu->GetPosition()[1-view0];

	fHS->FillHisto2List("MMTrackDevel",Form("MM_dVMMECAL_vs_pchi2_right_singleALL_clu0"),pchi2_tmp0,dv0,1.);
	
	if(pchi2best0[plane0][view0] < pchi2_tmp0) {
	  pchi2best0[plane0][view0] = pchi2_tmp0;
	  dv_MMEcalbest0[plane0][view0] = dv0;
	  itrack0_good[plane0][view0] = itra0;

	  quad_track0_good[plane0][view0] = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetHit(0)->GetMMchInfo().quad;
	  Nhit_track0_good[plane0][view0] = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetHitsVectorSize();
	  slope_track0_good[plane0][view0] = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetTracklet().slope;
	  inter_track0_good[plane0][view0] = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetTracklet().inter;
	}
      }
      
      if((view0 == 0 && x_Ecal*signsQuadX[quad0] < 0 && y_Ecal*signsQuadY[quad0] > 0)||(view0 == 1 && x_Ecal*signsQuadX[quad0] > 0 && y_Ecal*signsQuadY[quad0] < 0)) { //Y or X view wrong quad
	double pchi2_tmp0 = ROOT::Math::chisquared_cdf_c(fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetTracklet().chi2,nhit_tmp0-2);
	TVector3 MMposAtEcal0 = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetTracklet().ExtrapolationAtZ(z_Ecal);//tempClu->GetPosition().Z());
	double dv0 = MMposAtEcal0[1-view0] - tempClu->GetPosition()[1-view0];

	if(pchi2best_wrong0[plane0][view0] < pchi2_tmp0) {
	  pchi2best_wrong0[plane0][view0] = pchi2_tmp0;
	  dv_MMEcalbest_wrong0[plane0][view0] = dv0;
	  itrack0_wrong[plane0][view0] = itra0;

	  quad_track0_wrong[plane0][view0] = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetHit(0)->GetMMchInfo().quad;
	  Nhit_track0_wrong[plane0][view0] = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetHitsVectorSize();
	  slope_track0_wrong[plane0][view0] = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetTracklet().slope;
	  inter_track0_wrong[plane0][view0] = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetTracklet().inter;
	}
      }

      
	/*
      for(int itra1=itra0+1; itra1<(int) fMMClusteringInstance->GetMMClusterLength(0,0); itra1++) {
	int quad1 = fMMClusteringInstance->GetMMCluster(itra1,0,0)->GetHit(0)->GetMMchInfo().quad;
	int view1 = fMMClusteringInstance->GetMMCluster(itra1,0,0)->GetHit(0)->GetMMchInfo().view;
	if(!(x_Ecal*signsQuadX[quad1] > 0 && y_Ecal*signsQuadY[quad1] > 0)) continue; //matching ECal clu position with MM quad 
	//if(x_Ecal*signsQuadX[quad1] < 0 || y_Ecal*signsQuadY[quad1] < 0) continue; //matching ECal clu position with MM quad 	
	int nhit_tmp1 = fMMClusteringInstance->GetMMCluster(itra1,0,0)->GetHitsVectorSize();
	if (nhit_tmp1 < 3) continue; //only consider tracks with 3+ hits
	double pchi2_tmp1 = ROOT::Math::chisquared_cdf_c(fMMClusteringInstance->GetMMCluster(itra1,0,0)->GetTracklet().chi2,nhit_tmp1-2);

	if (view0 == view1) continue; // only consider pair of level zero tracks with different views
	
	TVector3 MMposAtEcal1 = fMMClusteringInstance->GetMMCluster(itra1,0,0)->GetTracklet().ExtrapolationAtZ(z_Ecal);//tempClu->GetPosition().Z());
	double dv1 = MMposAtEcal0[1-view1] - tempClu->GetPosition()[1-view1];

	if(pchi2_tmp0 > 0.7 && pchi2_tmp1 > 0.7) {
	  fHS->FillHisto2List("MMTrackDevel",Form("MM_dyECAL_vs_dxECAL_clu0"),view0==0?dv1:dv0,view0==0?dv0:dv1,1.);
	  fHS->FillHisto2List("MMTrackDevel",Form("MM_dVMMECAL_vs_pchi2_right_double_clu0"),pchi2_tmp0*pchi2_tmp1,
			      TMath::Sign(TMath::Sqrt(dv0*dv0+dv1*dv1),dv0*dv1),
			      1.);
	}
	
	if(pchi2bestPair < pchi2_tmp0*pchi2_tmp1) {
	  pchi2bestPair = pchi2_tmp0*pchi2_tmp1;
	  dv_MMEcalbest[view0] = MMposAtEcal0[1-view0] - tempClu->GetPosition()[1-view0];
	  dv_MMEcalbest[view1] = MMposAtEcal0[1-view1] - tempClu->GetPosition()[1-view1];
	  
	}
      }*/ // loop over tracks level 0 - second of the pair
    } // loop over tracks level 0 - first of the pair

    for(int pl=0; pl<2; pl++) {
      for(int vw=0; vw<2; vw++) {
	TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(vw);    
	fHS->FillHisto2List("MMTrackDevel",Form("MM_dVMMECAL_vs_pchi2_right_3h_P%dV%s_clu0",pl,viewlabel.Data()),pchi2best0[pl][vw],dv_MMEcalbest0[pl][vw],1.);
	fHS->FillHisto2List("MMTrackDevel",Form("MM_dVMMECAL_vs_pchi2_wrong_3h_P%dV%s_clu0",pl,viewlabel.Data()),pchi2best_wrong0[pl][vw],dv_MMEcalbest_wrong0[pl][vw],1.);
      }
    }
    

    

    // loop over level-one tracks
    
    double dv_MMEcal[2] = {-999, -999}, dv_MMEcal_wrong[2] = {-999,-999};
    double pchi2[2] = {-999, -999}, pchi2_wrong[2] = {-999,-999};
    int itrack_good[2] = {-999, -999}, itrack_wrong[2] = {-999, -999};  
    int quad_track1_good[2] = {-999,-999}, quad_track1_wrong[2] = {-999,-999};
    int Nhit_track1_good[2] = {-999,-999}, Nhit_track1_wrong[2] = {-999,-999};
    double slope_track1_good[2] = {-999,-999}, slope_track1_wrong[2] = {-999,-999};
    double inter_track1_good[2] = {-999,-999}, inter_track1_wrong[2] = {-999,-999};
    
    for(int itra=0; itra<(int) fMMClusteringInstance->GetMMClusterLength(0,1); itra++) {

      int quad = fMMClusteringInstance->GetMMCluster(itra,0,1)->GetHit(0)->GetMMchInfo().quad;
      int view = fMMClusteringInstance->GetMMCluster(itra,0,1)->GetHit(0)->GetMMchInfo().view;
      TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(view);
      
      if(x_Ecal*signsQuadX[quad] > 0 && y_Ecal*signsQuadY[quad] > 0) { //matching ECal clu position with MM quad 	
	
	if(fMMClusteringInstance->GetMMCluster(itra,0,1)->SimpleFitWithClusterTime(x_Ecal,y_Ecal,t_Ecal)) {
	  vector<TVector3> residuals_simple = fMMClusteringInstance->GetMMCluster(itra,0,1)->GetTracklet().vres;
	  //std::cout<<"residual size: "<<residuals.size()<<" hit size: "<<Nhit<<std::endl;
	  
	  int Nhit_true_simple = (int) residuals_simple.size();
	  double chi2_simple = fMMClusteringInstance->GetMMCluster(itra,0,1)->GetTracklet().chi2;
	  
	  TVector3 MMposAtEcal_simple = fMMClusteringInstance->GetMMCluster(itra,0,1)->GetTracklet().ExtrapolationAtZ(z_Ecal);//tempClu->GetPosition().Z());
	  
	  
	  // if(fMMClusteringInstance->GetMMCluster(itra,0,1)->FitWithClusterTime(x_Ecal,y_Ecal,t_Ecal)) {
	  //   vector<TVector3> residuals_refit = fMMClusteringInstance->GetMMCluster(itra,0,1)->GetTracklet().vres;
	  //   int Nhit_true_refit = (int) residuals_refit.size();
	  //   double chi2_refit = fMMClusteringInstance->GetMMCluster(itra,0,1)->GetTracklet().chi2;
	  
	  if(pchi2[view] < ROOT::Math::chisquared_cdf_c(chi2_simple, Nhit_true_simple-2)) {
	    pchi2[view] = ROOT::Math::chisquared_cdf_c(chi2_simple, Nhit_true_simple-2);
	    dv_MMEcal[view] = MMposAtEcal_simple[1-view] - tempClu->GetPosition()[1-view];
	    itrack_good[view] = itra;

	    quad_track1_good[view] = fMMClusteringInstance->GetMMCluster(itra,0,1)->GetHitHR(0)->GetMMchInfo().quad;
	    Nhit_track1_good[view] = fMMClusteringInstance->GetMMCluster(itra,0,1)->GetHitsHRVectorSize();
	    slope_track1_good[view] = fMMClusteringInstance->GetMMCluster(itra,0,1)->GetTracklet().slope;
	    inter_track1_good[view] = fMMClusteringInstance->GetMMCluster(itra,0,1)->GetTracklet().inter;
	  } 
	}
      }
      
      if((view == 0 && x_Ecal*signsQuadX[quad] < 0 && y_Ecal*signsQuadY[quad] > 0)||(view == 1 && x_Ecal*signsQuadX[quad] > 0 && y_Ecal*signsQuadY[quad] < 0)) { //Y or X view wrong quad

	if(fMMClusteringInstance->GetMMCluster(itra,0,1)->SimpleFitWithClusterTime(x_Ecal,y_Ecal,t_Ecal)) {
	  vector<TVector3> residuals_simple = fMMClusteringInstance->GetMMCluster(itra,0,1)->GetTracklet().vres;
	  //std::cout<<"residual size: "<<residuals.size()<<" hit size: "<<Nhit<<std::endl;
	  
	  int Nhit_true_simple = (int) residuals_simple.size();
	  double chi2_simple = fMMClusteringInstance->GetMMCluster(itra,0,1)->GetTracklet().chi2;
	  
	  TVector3 MMposAtEcal_simple = fMMClusteringInstance->GetMMCluster(itra,0,1)->GetTracklet().ExtrapolationAtZ(z_Ecal);//tempClu->GetPosition().Z());
	  
	  
	  if(pchi2_wrong[view] < ROOT::Math::chisquared_cdf_c(chi2_simple, Nhit_true_simple-2)) {
	    pchi2_wrong[view] = ROOT::Math::chisquared_cdf_c(chi2_simple, Nhit_true_simple-2);
	    dv_MMEcal_wrong[view] = MMposAtEcal_simple[1-view] - tempClu->GetPosition()[1-view];

	    itrack_wrong[view] = itra;

	    quad_track1_wrong[view] = fMMClusteringInstance->GetMMCluster(itra,0,1)->GetHitHR(0)->GetMMchInfo().quad;
	    Nhit_track1_wrong[view] = fMMClusteringInstance->GetMMCluster(itra,0,1)->GetHitsHRVectorSize();
	    slope_track1_wrong[view] = fMMClusteringInstance->GetMMCluster(itra,0,1)->GetTracklet().slope;
	    inter_track1_wrong[view] = fMMClusteringInstance->GetMMCluster(itra,0,1)->GetTracklet().inter;
	  } 
	}
      }
      
    }
    for(int vw=0; vw<2; vw++) {
      fHS->FillHisto2List("MMTrackDevel",Form("MM_dVMMECAL_vs_pchi2_right_clu1"),pchi2[vw],dv_MMEcal[vw],1.);
      fHS->FillHisto2List("MMTrackDevel",Form("MM_dVMMECAL_vs_pchi2_wrong_clu1"),pchi2_wrong[vw],dv_MMEcal_wrong[vw],1.);

      TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(vw);    
      fHS->FillHisto2List("MMTrackDevel",Form("MM_dVMMECAL_vs_pchi2_right_V%s_clu1",viewlabel.Data()),pchi2[vw],dv_MMEcal[vw],1.);
      fHS->FillHisto2List("MMTrackDevel",Form("MM_dVMMECAL_vs_pchi2_wrong_V%s_clu1",viewlabel.Data()),pchi2_wrong[vw],dv_MMEcal_wrong[vw],1.);
    }

    
    //------------------------------COUTTONE PER TREE---------------------------
    //                                    clu index                                                          
    std::cout<<"RIGHT "<<fEventCounter<<" "<<iidx<<" "<<x_Ecal<<" "<<y_Ecal<<" "<<z_Ecal<<" "<<t_Ecal<<" "<<E_Ecal<<" ";
    for(int pl=0; pl<2; pl++) {
      for(int vw=0; vw<2; vw++) {
	//             track index           plane             quad                  view
	std::cout<<itrack0_good[pl][vw]<<" "<<pl<<" "<<quad_track0_good[pl][vw]<<" "<<vw<<" "<<pchi2best0[pl][vw]<<" "<<dv_MMEcalbest0[pl][vw]<<" "<<slope_track0_good[pl][vw]<<" "<<inter_track0_good[pl][vw]<<" "<<Nhit_track0_good[pl][vw]<<" ";
      }
    }
    for(int vw=0; vw<2; vw++) {
      //         track index          plane           quad               view
      std::cout<<itrack_good[vw]<<" "<<2<<" "<<quad_track1_good[vw]<<" "<<vw<<" "<<pchi2[vw]<<" "<<dv_MMEcal[vw]<<" "<<slope_track1_good[vw]<<" "<<inter_track1_good[vw]<<" "<<Nhit_track1_good[vw]<<" ";
    }
    std::cout<<std::endl;
    //                                    clu index                                                          
    std::cout<<"WRONG "<<fEventCounter<<" "<<iidx<<" "<<x_Ecal<<" "<<y_Ecal<<" "<<z_Ecal<<" "<<t_Ecal<<" ";
    for(int pl=0; pl<2; pl++) {
      for(int vw=0; vw<2; vw++) {
	//             track index           plane             quad                    view
	std::cout<<itrack0_wrong[pl][vw]<<" "<<pl<<" "<<quad_track0_wrong[pl][vw]<<" "<<vw<<" "<<pchi2best_wrong0[pl][vw]<<" "<<dv_MMEcalbest_wrong0[pl][vw]<<" "<<slope_track0_wrong[pl][vw]<<" "<<inter_track0_wrong[pl][vw]<<" "<<Nhit_track0_wrong[pl][vw]<<" ";
      }
    }
    for(int vw=0; vw<2; vw++) {
      //         track index          plane           quad                 view
      std::cout<<itrack_wrong[vw]<<" "<<2<<" "<<quad_track1_wrong[vw]<<" "<<vw<<" "<<pchi2_wrong[vw]<<" "<<dv_MMEcal_wrong[vw]<<" "<<slope_track1_wrong[vw]<<" "<<inter_track1_wrong[vw]<<" "<<Nhit_track1_wrong[vw]<<" ";
    }
    std::cout<<std::endl;
    //-------------------------------------------------------------------------
    
    for(int vw=0; vw<2; vw++) {
      if(CALIBRATION) {
	if(itrack_good[vw] != -999) {
	  if(fabs(dv_MMEcal[vw]) < 20. && pchi2[vw] > 0.5) {
	    if(fMMClusteringInstance->GetMMCluster(itrack_good[vw],0,1)->FitWithClusterTime(x_Ecal,y_Ecal,t_Ecal)) {
	      int quad = fMMClusteringInstance->GetMMCluster(itrack_good[vw],0,1)->GetHit(0)->GetMMchInfo().quad;
	      int view = fMMClusteringInstance->GetMMCluster(itrack_good[vw],0,1)->GetHit(0)->GetMMchInfo().view;
	      TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(view);
	      double dz = fMMClusteringInstance->GetMMCluster(itrack_good[vw],0,1)->GetTracklet().pars[4];
	      double dz_Ecal = (t_Ecal+440.)*GeneralInfo::GetInstance()->GetMMDriftVelocity();
	      double Ddz = dz - dz_Ecal;
	      TVector3 MMposAtMesh_refit = fMMClusteringInstance->GetMMCluster(itrack_good[vw],0,1)->GetTracklet().ExtrapolationAtZ(GeneralInfo::GetInstance()->GetMMPosPlaneZ(2));
	      fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_Ddz_vs_%s_Q%d_atMesh_clu1",viewlabel.Data(),quad),MMposAtMesh_refit[1-view],Ddz,1.);

	      int Nhit_HR = fMMClusteringInstance->GetMMCluster(itrack_good[vw],0,1)->GetHitsHRVectorSize();
	      double z_first_hit = +999, z_last_hit = -999; 
	      for(int h=0; h<Nhit_HR; h++) {
		MMSoftHit * hit = fMMClusteringInstance->GetMMCluster(itrack_good[vw],0,1)->GetHitHR(h);
		double z_hit = hit->GetZfromTime(1.) - GeneralInfo::GetInstance()->GetMMPosPlaneZ(2);
		if(z_first_hit > z_hit) z_first_hit = z_hit;
		if(z_last_hit  < z_hit) z_last_hit  = z_hit;
		
	      }
	      fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_z_first_last_hit_vs_%s_Q%d_atMesh_clu1",viewlabel.Data(),quad),MMposAtMesh_refit[1-view],z_first_hit,1.);
	      fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_z_first_last_hit_vs_%s_Q%d_atMesh_clu1",viewlabel.Data(),quad),MMposAtMesh_refit[1-view],z_last_hit,1.);
	    }
	  }
	}
      }
    }
    //ftree->Fill();
    //fHS->FillNtupleList("MMTrackDevel","ftree");
    
   
  


    
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



