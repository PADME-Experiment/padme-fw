#include "MMFindBestTrack.hh"

MMFindBestTrack* MMFindBestTrack::fInstance = 0;

MMFindBestTrack* MMFindBestTrack::GetInstance(){
  if ( fInstance == 0 ) { fInstance = new MMFindBestTrack(); }

  return fInstance;
}


// MMFindBestTrack::MMFindBestTrack(){
  
// }

MMFindBestTrack::~MMFindBestTrack(){
    fvTracks.clear();
    delete fCfgParser;
}

Bool_t MMFindBestTrack::Init(PadmeAnalysisEvent* event){

  if (fVerbose) printf("---> Initializing MMFindBestTrack\n");
  TString cfgFile = "config/UserAnalysis.conf";
  fGeneralInfo = GeneralInfo::GetInstance();
  fMMClusteringInstance = MMClustering::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
  fEvent = event;
  return true;
}
Bool_t MMFindBestTrack::Process(){
  fvTracks.clear();
  fMMClusteringInstance->Clear();
  fMMClusteringInstance->Init(fEvent->MMRecoEvent);

  
  fMMClusteringInstance->Clusterize();
  //BEST TRACK FINDER
  
  MMTrack best_tra0[4][2];
  MMTrack best_tra1[4][2];
  double pchi2_best0[4][2] = {{-999,-999},{-999,-999},{-999,-999},{-999,-999}};
  double pchi2_best1[4][2] = {{-999,-999},{-999,-999},{-999,-999},{-999,-999}};
  
  for(int itra0=0; itra0<(int) fMMClusteringInstance->GetMMClusterLength(0,0); itra0++) {
    int quad = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetHit(0)->GetMMchInfo().quad;
    int view = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetHit(0)->GetMMchInfo().view;
    MMTracklet tracklet = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetTracklet();
    
    int nhit_tmp = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetHitsVectorSize();
    if (nhit_tmp < 3) continue; //only consider tracks with 3+ hits

    double chi2_tmp = fMMClusteringInstance->GetMMCluster(itra0,0,0)->GetTracklet().chi2;
    double pchi2_tmp = ROOT::Math::chisquared_cdf_c(chi2_tmp,nhit_tmp-2);

    if(pchi2_tmp > pchi2_best0[quad][view]) {
      pchi2_best0[quad][view] = pchi2_tmp;
      best_tra0[quad][view].quad = quad;
      best_tra0[quad][view].view = view;
      best_tra0[quad][view].level = 0;
      best_tra0[quad][view].nhit = nhit_tmp;
      best_tra0[quad][view].tracklet = tracklet;
    }
  }

  for(int itra1=0; itra1<(int) fMMClusteringInstance->GetMMClusterLength(0,1); itra1++) {
    int quad = fMMClusteringInstance->GetMMCluster(itra1,0,1)->GetHit(0)->GetMMchInfo().quad;
    int view = fMMClusteringInstance->GetMMCluster(itra1,0,1)->GetHit(0)->GetMMchInfo().view;
    MMTracklet tracklet = fMMClusteringInstance->GetMMCluster(itra1,0,1)->GetTracklet();
    
    int nhit_tmp = fMMClusteringInstance->GetMMCluster(itra1,0,1)->GetHitsVectorSize();

    double chi2_tmp = fMMClusteringInstance->GetMMCluster(itra1,0,1)->GetTracklet().chi2;
    //double pchi2_tmp = ROOT::Math::chisquared_cdf_c(chi2_tmp,nhit_tmp-3); //chi2 done at the moment with slope
    double pchi2_tmp = fMMClusteringInstance->GetMMCluster(itra1,0,1)->GetTracklet().pchi2;
      
    if(pchi2_tmp > pchi2_best1[quad][view]) {
      pchi2_best1[quad][view] = pchi2_tmp;
      best_tra1[quad][view].quad = quad;
      best_tra1[quad][view].view = view;
      best_tra1[quad][view].level = 1;
      best_tra1[quad][view].nhit = nhit_tmp;
      best_tra1[quad][view].tracklet = tracklet;
    }
  }

  for(int qd=0; qd<4; qd++) {
    for(int vw=0; vw<2; vw++) { ////// TO BE IMPLEMENTED //provare a uscire con più tracce per vista, prendere dZ dal tracklet -->pars[4]
      if(best_tra1[qd][vw].quad > -1) {
        MMBestTrack *bt = new MMBestTrack();
        bt->quad = qd;
        bt->view = vw;
        bt->level = 1;
	bt->nhit = best_tra1[qd][vw].nhit;
	bt->slope = best_tra1[qd][vw].tracklet.slope;
        bt->inter = best_tra1[qd][vw].tracklet.inter;
        bt->chi2 = best_tra1[qd][vw].tracklet.chi2;
        bt->chi2IP = best_tra1[qd][vw].tracklet.chi2IP;
	bt->pchi2 = pchi2_best1[qd][vw];
	bt->slope_lvl0[0] = best_tra1[qd][vw].tracklet.slope_lvl0[0];
	bt->slope_lvl0[1] = best_tra1[qd][vw].tracklet.slope_lvl0[1];
	bt->inter_lvl0[0] = best_tra1[qd][vw].tracklet.inter_lvl0[0];
	bt->inter_lvl0[1] = best_tra1[qd][vw].tracklet.inter_lvl0[1]; 
        for(int ipar=0; ipar<5; ipar++){
          bt->pars[ipar] = best_tra1[qd][vw].tracklet.pars[ipar];
        }
        bt->lambda = best_tra1[qd][vw].tracklet.lambda;
        bt->vres = best_tra1[qd][vw].tracklet.vres;

        

	    fvTracks.push_back(bt);
        //std::cout<<"Track extrapolation LV1: "<<fvTracks.back()->view<<" quad: "<<fvTracks.back()->quad<<"tracklet slope:  "<<fvTracks.back()->tracklet.slope<<"tracklet inter: "<<fvTracks.back()->tracklet.inter<<std::endl;

      }
      else if(best_tra0[qd][vw].quad > -1) { 

	    MMBestTrack *bt = new MMBestTrack();
        bt->quad = qd;
        bt->view = vw;
        bt->level = 0;
	bt->nhit = best_tra0[qd][vw].nhit;
        bt->slope = best_tra0[qd][vw].tracklet.slope;
        bt->inter = best_tra0[qd][vw].tracklet.inter;
        bt->chi2 = best_tra0[qd][vw].tracklet.chi2;
        bt->chi2IP = best_tra0[qd][vw].tracklet.chi2IP;
	bt->pchi2 = pchi2_best0[qd][vw];
        for(int ipar=0; ipar<5; ipar++){
          bt->pars[ipar] = best_tra0[qd][vw].tracklet.pars[ipar];
        }        
        bt->lambda = best_tra0[qd][vw].tracklet.lambda;
        bt->vres = best_tra0[qd][vw].tracklet.vres;

        

	    fvTracks.push_back(bt);
        //std::cout<<"Track extrapolation LV0: "<<fvTracks.back()->view<<" quad: "<<fvTracks.back()->quad<<"tracklet slope:  "<<fvTracks.back()->tracklet.slope<<"tracklet inter: "<<fvTracks.back()->tracklet.inter<<std::endl;

      }
      //else std::cerr<<"[BEST TRACK FINDER] ci sta qualche problema"<<std::endl;
    }
  }  
  return true;
}
Bool_t MMFindBestTrack::Finalize(){
  if(fGeneralInfo->isMC()){
    std::cout<<"This run is MC, MMFindBestTrack checks do not apply"<<std::endl;
    return false;
  } 

  if (fVerbose) printf("---> Finalizing MMFindBestTrack\n");
  
  return true;
}
