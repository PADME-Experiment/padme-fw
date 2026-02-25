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
    for (int quad = 0; quad<4; quad++) fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_quality_vs_%sAtTarget_Q%d_clu1",viewlabel.Data(),quad),400,-200,200,100,0,120.);
  }
  for (int view = 0; view<2; view++){
    TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(view);    
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_dz_Qall_clu1_vsYEcal",viewlabel.Data()),100,-300,300,100,-200,200);
    fHS->BookHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_dz_Qall_clu1_vsXEcal",viewlabel.Data()),100,-300,300,100,-200,200);
  }
  for (int bdid = 0; bdid < 16; bdid++) fHS->BookHisto2List("MMTrackDevel",Form("MM_Time_vs_channel_bdid%d",bdid),256,-0.5,255.5,900,-150,750);
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
    5.,  5.,  5., 5. ,
    0,  50.,  0., 5. };
  double maxdDZvsXY[2][4]    = {
    50., 50., 50., 50.,
    50, 50., 50., 50.};

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
  

  // plots to evaluate matching of level-1 tracks with the ECal info

  for(int iclu=0; iclu<(int) fMMClusteringInstance->GetMMClusterLength(0,1); iclu++) {
    int quad = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetHit(0)->GetMMchInfo().quad;
    int view = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetHit(0)->GetMMchInfo().view;
    TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(view);
    double dz = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetTracklet().pars[4];
    double chi2 = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetTracklet().chi2;
    double ipphaseangle = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetIPPhaseAngle();
    fHS->FillHisto2List("MMTrackDevel","MM_chi2_vs_dz",dz,chi2,1.);

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
      if(x_Ecal*signsQuadX[quad] > 0 && y_Ecal*signsQuadY[quad] > 0) {
	fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_dz_Q%d_clu1",viewlabel.Data(),quad),Ddz,dv_MMEcal,1.);
	fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_dz_Q%d_clu1_recenter",viewlabel.Data(),quad),
			    (Ddz-offsetdDZvsXY[1-view][quad])  / (maxdDZvsXY[1-view][quad]),
			    (dv_MMEcal-offsetXY[1-view][quad]) / (dxy_MMEcalMax[1-view][quad]), 1.);
	double radius = TMath::Sqrt(
				    TMath::Power((dv_MMEcal-offsetXY[1-view][quad]) / (dxy_MMEcalMax[1-view][quad]),2) +
				    TMath::Power((Ddz-offsetdDZvsXY[1-view][quad])  / (maxdDZvsXY[1-view][quad]),2)
				    );
	fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_quality_vs_%sAtTarget_Q%d_clu1",viewlabel.Data(),quad),MMposAtTarg[1-view],radius,1.);
	
	if(fabs(dv_MMEcal-offsetXY[1-view][quad])< dxy_MMEcalMax[1-view][quad] && fabs(Ddz-offsetdDZvsXY[1-view][quad])< maxdDZvsXY[1-view][quad]) { // calorimeter matching
	  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_%s_vs_IPtheta_Q%d_clu1",viewlabel.Data(),quad),ipphaseangle,MMposAtEcal[1-view],1.);
	  // plot of track extrapolation at z target
	  fHS->FillHistoList("MMTrackDevel",Form("MM_ECAL_%sAtTarget_Q%d_clu1",viewlabel.Data(),quad),MMposAtTarg[1-view],1.);
	  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_dz_Qall_clu1_vsXEcal",viewlabel.Data()),x_Ecal,dv_MMEcal,1.);
	  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_dz_Qall_clu1_vsYEcal",viewlabel.Data()),y_Ecal,dv_MMEcal,1.);
	}
      }
//      if(view == YVIEW) {
//	double dy_MMEcal = MMposAtEcal.Y() - y_Ecal;	
//	
//	if(x_Ecal*signsQuadX[quad] > 0 && y_Ecal*signsQuadY[quad] > 0) {
//	  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_dY_vs_dz_Q%d_clu1",quad),Ddz,dy_MMEcal,1.);
//	  if(fabs(dy_MMEcal-offsetY[quad])< dy_MMEcalMax[quad] && fabs(Ddz-offsetdDZvsY[quad])< maxdDZvsY[quad]) { // calorimeter matching
//	    fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_Y_vs_IPtheta_Q%d_clu1",quad),ipphaseangle,MMposAtEcal.Y(),1.);
//	    // plot of track extrapolation at z target
//	    fHS->FillHistoList("MMTrackDevel",Form("MM_ECAL_YAtTarget_Q%d_clu1",quad),MMposAtTarg.Y(),1.);
//	    
//	  }
//	}
//      }
//      if(view == XVIEW) {
//	double dx_MMEcal = MMposAtEcal.X() - x_Ecal;
//	
//	if(y_Ecal*signsQuadY[quad] > 0 && x_Ecal*signsQuadX[quad] > 0) {
//	  fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_dX_vs_dz_Q%d_clu1",quad),Ddz,dx_MMEcal,1.);
//	  if(fabs(dx_MMEcal-offsetX[quad])< dx_MMEcalMax[quad] && fabs(Ddz-offsetdDZvsX[quad])<maxdDZvsX[quad]){
//	    fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_X_vs_IPtheta_Q%d_clu1",quad),ipphaseangle,MMposAtEcal.X(),1.);
//	    // plot of track extrapolation at z target
//	    fHS->FillHistoList("MMTrackDevel",Form("MM_ECAL_XAtTarget_Q%d_clu1",quad),MMposAtTarg.X(),1.);
//	  }
//	}
//      }   
    } // cluster loop
  } // MM track loop type 1

  for(int iclu=0; iclu<(int) fMMClusteringInstance->GetMMClusterLength(0,0); iclu++) {
    int Nhit = (int) fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetHitsVectorSize();
    int quad = fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetHit(0)->GetMMchInfo().quad;
    int view = fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetHit(0)->GetMMchInfo().view;
    TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(view);

    for(int iidx=0; iidx<(int) cluIndices.size(); iidx++) {
      TRecoVCluster* tempClu = ECal_clEvent->Element(cluIndices.at(iidx));
      double z_Ecal = GeneralInfo::GetInstance()->GetCOG().Z()+6.5*11.;

      TVector3 MMposAtEcal = fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetTracklet().ExtrapolationAtZ(z_Ecal);

      double y_Ecal = tempClu->GetPosition().Y();
      double x_Ecal = tempClu->GetPosition().X();
      double dv_MMEcal = MMposAtEcal[1-view] - tempClu->GetPosition()[1-view];
      if(x_Ecal*signsQuadX[quad]>0 && y_Ecal*signsQuadY[quad]>0) fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_d%s_vs_Nhit_Q%d_clu0",viewlabel.Data(),quad),Nhit,dv_MMEcal,1.);
      
//      if(view == YVIEW) {
//	double dy_MMEcal = MMposAtEcal.Y() - y_Ecal;
//      }
//      if(view == XVIEW) {
//	double dx_MMEcal = MMposAtEcal.X() - x_Ecal;
//	if(x_Ecal*signsQuadX[quad]>0 && y_Ecal*signsQuadY[quad]>0) fHS->FillHisto2List("MMTrackDevel",Form("MM_ECAL_dX_vs_Nhit_Q%d_clu0",quad),Nhit,dx_MMEcal,1.);
//      }
      
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



