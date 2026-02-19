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
  fHS->BookHistoList("MMTrackDevel","MM_NclusIPMode0CluMode0",1000,0,1000);
  fHS->BookHistoList("MMTrackDevel","MM_NclusIPMode1CluMode0",1000,0,1000);
  fHS->BookHistoList("MMTrackDevel","MM_NclusIPMode0CluMode1",1000,0,1000);
  fHS->BookHisto2List("MMTrackDevel","MM_chi2_vs_dz",100,-50,50,1000,0,50);

  fHS->BookHisto2List("MMTrackDevel","ECAL_dt_vs_MM_dz",100,-50,50,1000,-500,500);
  
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dY_vs_dz_Q0_clu1",100,-200,200,1000,-100,100);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dY_vs_dz_Q1_clu1",100,-200,200,1000,-100,100);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dY_vs_dz_Q2_clu1",100,-200,200,1000,-100,100);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dY_vs_dz_Q3_clu1",100,-200,200,1000,-100,100);
  
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dX_vs_dz_Q0_clu1",100,-200,200,1000,-100,100);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dX_vs_dz_Q1_clu1",100,-200,200,1000,-100,100);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dX_vs_dz_Q2_clu1",100,-200,200,1000,-100,100);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dX_vs_dz_Q3_clu1",100,-200,200,1000,-100,100);

  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_Y_vs_IPtheta_Q0_clu1",100,-0.01,0.01,1200,-600,600);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_Y_vs_IPtheta_Q1_clu1",100,-0.01,0.01,1200,-600,600);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_Y_vs_IPtheta_Q2_clu1",100,-0.01,0.01,1200,-600,600);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_Y_vs_IPtheta_Q3_clu1",100,-0.01,0.01,1200,-600,600);
  
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_X_vs_IPtheta_Q0_clu1",100,-0.01,0.01,1200,-600,600);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_X_vs_IPtheta_Q1_clu1",100,-0.01,0.01,1200,-600,600);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_X_vs_IPtheta_Q2_clu1",100,-0.01,0.01,1200,-600,600);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_X_vs_IPtheta_Q3_clu1",100,-0.01,0.01,1200,-600,600);

  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dY_vs_Nhit_Q0_clu0",10,0,10,1000,-100,100);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dY_vs_Nhit_Q1_clu0",10,0,10,1000,-100,100);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dY_vs_Nhit_Q2_clu0",10,0,10,1000,-100,100);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dY_vs_Nhit_Q3_clu0",10,0,10,1000,-100,100);
  
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dX_vs_Nhit_Q0_clu0",10,0,10,1000,-100,100);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dX_vs_Nhit_Q1_clu0",10,0,10,1000,-100,100);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dX_vs_Nhit_Q2_clu0",10,0,10,1000,-100,100);
  fHS->BookHisto2List("MMTrackDevel","MM_ECAL_dX_vs_Nhit_Q3_clu0",10,0,10,1000,-100,100);
 

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
  
  fHS->FillHistoList("MMTrackDevel","MM_NclusIPMode0CluMode0",fMMClusteringInstance->GetMMClusterLength(0,0),1.);
  fHS->FillHistoList("MMTrackDevel","MM_NclusIPMode1CluMode0",fMMClusteringInstance->GetMMClusterLength(1,0),1.);
  fHS->FillHistoList("MMTrackDevel","MM_NclusIPMode0CluMode1",fMMClusteringInstance->GetMMClusterLength(0,1),1.);
  
  for(int iclu=0; iclu<(int) fMMClusteringInstance->GetMMClusterLength(0,1); iclu++) {
    int quad = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetHit(0)->GetMMchInfo().quad;
    int view = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetHit(0)->GetMMchInfo().view;
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
      TVector3 MMposAtEcal = fMMClusteringInstance->GetMMCluster(iclu,0,1)->GetTracklet().ExtrapolationAtZ(tempClu->GetPosition().Z());

      double dz_Ecal = (tempClu->GetTime()+440.)*fGeneralInfo->GetMMDriftVelocity();
      double y_Ecal = tempClu->GetPosition().Y();
      double x_Ecal = tempClu->GetPosition().X();

      double Ddz = dz - dz_Ecal;
      
      if(view == YVIEW) {
	double dy_MMEcal = MMposAtEcal.Y() - y_Ecal;	
	
	if(quad == 0 && x_Ecal<0 && y_Ecal<0) {
	  fHS->FillHisto2List("MMTrackDevel","MM_ECAL_dY_vs_dz_Q0_clu1",Ddz,dy_MMEcal,1.);
	  if(fabs(dy_MMEcal)<20. && fabs(Ddz)<50.) fHS->FillHisto2List("MMTrackDevel","MM_ECAL_Y_vs_IPtheta_Q0_clu1",ipphaseangle,MMposAtEcal.Y(),1.);
	}
	if(quad == 1 && x_Ecal<0 && y_Ecal>0) {
	  fHS->FillHisto2List("MMTrackDevel","MM_ECAL_dY_vs_dz_Q1_clu1",Ddz,dy_MMEcal,1.);
	  if(fabs(dy_MMEcal+10)<20. && fabs(Ddz-50)<50.) fHS->FillHisto2List("MMTrackDevel","MM_ECAL_Y_vs_IPtheta_Q1_clu1",ipphaseangle,MMposAtEcal.Y(),1.);
	}
	if(quad == 2 && x_Ecal>0 && y_Ecal>0) {
	  fHS->FillHisto2List("MMTrackDevel","MM_ECAL_dY_vs_dz_Q2_clu1",Ddz,dy_MMEcal,1.);
	  if(fabs(dy_MMEcal+5)<20. && fabs(Ddz)<50.) fHS->FillHisto2List("MMTrackDevel","MM_ECAL_Y_vs_IPtheta_Q2_clu1",ipphaseangle,MMposAtEcal.Y(),1.);
	}
	if(quad == 3 && x_Ecal>0 && y_Ecal<0) {
	  fHS->FillHisto2List("MMTrackDevel","MM_ECAL_dY_vs_dz_Q3_clu1",Ddz,dy_MMEcal,1.);
	  if(fabs(dy_MMEcal)<20. && fabs(Ddz-5)<50.) fHS->FillHisto2List("MMTrackDevel","MM_ECAL_Y_vs_IPtheta_Q3_clu1",ipphaseangle,MMposAtEcal.Y(),1.);
	}
      }
      if(view == XVIEW) {
	double dx_MMEcal = MMposAtEcal.X() - x_Ecal;
	
	if(quad == 0 && y_Ecal<0 && x_Ecal<0) {
	  fHS->FillHisto2List("MMTrackDevel","MM_ECAL_dX_vs_dz_Q0_clu1",Ddz,dx_MMEcal,1.);
	  if(fabs(dx_MMEcal)<20. && fabs(Ddz-5)<50.) fHS->FillHisto2List("MMTrackDevel","MM_ECAL_X_vs_IPtheta_Q0_clu1",ipphaseangle,MMposAtEcal.X(),1.);
	}
	if(quad == 1 && y_Ecal>0 && x_Ecal<0) {
	  fHS->FillHisto2List("MMTrackDevel","MM_ECAL_dX_vs_dz_Q1_clu1",Ddz,dx_MMEcal,1.);
	  if(fabs(dx_MMEcal)<20. && fabs(Ddz-5)<50.) fHS->FillHisto2List("MMTrackDevel","MM_ECAL_X_vs_IPtheta_Q1_clu1",ipphaseangle,MMposAtEcal.X(),1.);
	}
	if(quad == 2 && y_Ecal>0 && x_Ecal>0) {
	  fHS->FillHisto2List("MMTrackDevel","MM_ECAL_dX_vs_dz_Q2_clu1",Ddz,dx_MMEcal,1.);
	  if(fabs(dx_MMEcal+10)<20. && fabs(Ddz-5)<50.) fHS->FillHisto2List("MMTrackDevel","MM_ECAL_X_vs_IPtheta_Q2_clu1",ipphaseangle,MMposAtEcal.X(),1.);
	}
	if(quad == 3 && y_Ecal<0 && x_Ecal>0) {
	  fHS->FillHisto2List("MMTrackDevel","MM_ECAL_dX_vs_dz_Q3_clu1",Ddz,dx_MMEcal,1.);
	  if(fabs(dx_MMEcal+10)<20. && fabs(Ddz-5)<50.) fHS->FillHisto2List("MMTrackDevel","MM_ECAL_X_vs_IPtheta_Q3_clu1",ipphaseangle,MMposAtEcal.X(),1.);
	}
      }   
    }
  }

  for(int iclu=0; iclu<(int) fMMClusteringInstance->GetMMClusterLength(0,0); iclu++) {
    int Nhit = (int) fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetHitsVectorSize();
    int quad = fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetHit(0)->GetMMchInfo().quad;
    int view = fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetHit(0)->GetMMchInfo().view;

    for(int iidx=0; iidx<(int) cluIndices.size(); iidx++) {
      TRecoVCluster* tempClu = ECal_clEvent->Element(cluIndices.at(iidx));
      TVector3 MMposAtEcal = fMMClusteringInstance->GetMMCluster(iclu,0,0)->GetTracklet().ExtrapolationAtZ(tempClu->GetPosition().Z());

      double y_Ecal = tempClu->GetPosition().Y();
      double x_Ecal = tempClu->GetPosition().X();
      
      if(view == YVIEW) {
	double dy_MMEcal = MMposAtEcal.Y() - y_Ecal;	
	
	if(quad == 0 && x_Ecal<0 && y_Ecal<0) fHS->FillHisto2List("MMTrackDevel","MM_ECAL_dY_vs_Nhit_Q0_clu0",Nhit,dy_MMEcal,1.);
	if(quad == 1 && x_Ecal<0 && y_Ecal>0) fHS->FillHisto2List("MMTrackDevel","MM_ECAL_dY_vs_Nhit_Q1_clu0",Nhit,dy_MMEcal,1.);
	if(quad == 2 && x_Ecal>0 && y_Ecal>0) fHS->FillHisto2List("MMTrackDevel","MM_ECAL_dY_vs_Nhit_Q2_clu0",Nhit,dy_MMEcal,1.);
	if(quad == 3 && x_Ecal>0 && y_Ecal<0) fHS->FillHisto2List("MMTrackDevel","MM_ECAL_dY_vs_Nhit_Q3_clu0",Nhit,dy_MMEcal,1.);
      }
      if(view == XVIEW) {
	double dx_MMEcal = MMposAtEcal.X() - x_Ecal;
	
	if(quad == 0 && y_Ecal<0 && x_Ecal<0) fHS->FillHisto2List("MMTrackDevel","MM_ECAL_dX_vs_Nhit_Q0_clu0",Nhit,dx_MMEcal,1.);
	if(quad == 1 && y_Ecal>0 && x_Ecal<0) fHS->FillHisto2List("MMTrackDevel","MM_ECAL_dX_vs_Nhit_Q1_clu0",Nhit,dx_MMEcal,1.);
	if(quad == 2 && y_Ecal>0 && x_Ecal>0) fHS->FillHisto2List("MMTrackDevel","MM_ECAL_dX_vs_Nhit_Q2_clu0",Nhit,dx_MMEcal,1.);
	if(quad == 3 && y_Ecal<0 && x_Ecal>0) fHS->FillHisto2List("MMTrackDevel","MM_ECAL_dX_vs_Nhit_Q3_clu0",Nhit,dx_MMEcal,1.);
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



