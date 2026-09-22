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
Double_t MMFindBestTrack::PurityFunc(Double_t x, Double_t y, Double_t *p)
{

  double sigY = TMath::Exp(-0.5 * (y - p[2]) * (y - p[2]) / (p[3] * p[3]));
  double bkgY = TMath::Exp(-0.5 * (y - p[4]) * (y - p[4]) / (p[5] * p[5]));

  double sigX = TMath::Exp(p[6] * x);
  double bkgX = TMath::Exp(-p[7] * x);

  return p[0] * sigX * sigY / (p[0] * sigX * sigY + p[1] * bkgX * bkgY);
}

Double_t MMFindBestTrack::PurityFunc_4HitL1(Double_t x, Double_t *p) {
  double sig = TMath::Exp(-0.5*(x-p[1])*(x-p[1])/(p[2]*p[2]));
  double bkg = TMath::Exp(-0.5*(x-p[4])*(x-p[4])/(p[5]*p[5]));

  return p[0]*sig/(p[0]*sig + p[1]*bkg);
}

void MMFindBestTrack::AssignPurity(MMBestTrack* track, TVector3 cluPos,Double_t extrZ){
  TVector3 extPos = track->BestTrackExtrapolationAtZ(extrZ);
  double dR = (extPos[1 - track->view] - cluPos[1 - track->view]);
  // double dV = -999;
  // double dTheta = -999;
  // if (track->level == 1){
  // dV = track->inter_lvl0[0] - track->inter_lvl0[1];
  // dTheta = TMath::ATan(track->slope_lvl0[0]) - TMath::ATan(track->slope_lvl0[1]);
  // }
  double purity = -999;
  if (track->level == 0)
  {
    if (track->pchi2 < 0.6)
    return;
    else
      {
	if (track->nhit == 3)
	  {
	    purity = PurityFunc(track->pchi2, dR, pars_3h_lvl0);
	  }
	if (track->nhit == 4)
	  {
	    purity = PurityFunc(track->pchi2, dR, pars_4h_lvl0);
	  }
      }
  }
  else
  { // track->level == 1
    if(track->pchi2 < 0.1) return;
    else
      {
	if(track->nhit == 4) {
	  purity = PurityFunc_4HitL1(dR, pars_4h_lvl1);
	}
      }
    else if (track->pchi2 < 0.6)
      return;
    else {
      
      if (track->nhit == 5)
	{
	  purity = PurityFunc(track->pchi2, dR, pars_5h_lvl1);
	}
      if (track->nhit > 5)
	{
	  purity = PurityFunc(track->pchi2, dR, pars_6ph_lvl1);
	}
    }
  }
  track->purity= purity;
  return;
}

MMBestTrack3D* MMFindBestTrack::GetBestTrackFromCluPos(TVector3 cluPos,Double_t extrZ){
  
  vector<MMBestTrack *> trackvect = GetVectorTracks();
  
  int quad = -1;
  if (cluPos.X() < 0 && cluPos.Y() < 0)
    quad = 0;
  else if (cluPos.X() < 0 && cluPos.Y() > 0)
    quad = 1;
  else if (cluPos.X() > 0 && cluPos.Y() > 0)
    quad = 2;
  else
    quad = 3;
  
  int purity[2] = {-999, -999};
  MMBestTrack* besttrack[2] = {nullptr,nullptr};
  for (auto it = begin(trackvect); it != end(trackvect); ++it)
      {
        MMBestTrack *track = *it;
        if (track->quad != quad)
          continue;
        AssignPurity(track, cluPos, extrZ);
        if(track->purity > purity[track->view]){
          purity[track->view] = track->purity;
          besttrack[track->view] = track;
        }  
  } //best track found, now combine
  MMBestTrack3D* besttrack3D = new MMBestTrack3D() ;

  if(besttrack[0] == nullptr && besttrack[1] == nullptr) return besttrack3D; // no track found
  //else if(besttrack[0] ==nullptr || besttrack[1]== nullptr){std::cout << "ao ao ao uno è rotto"<<std::endl;}
  besttrack3D->tracks[0] = besttrack[0] ? besttrack[0]: nullptr;
  besttrack3D->tracks[1] =  besttrack[1] ? besttrack[1]: nullptr;
  Double_t combined_purity = -999;
  if(besttrack[0] == nullptr) combined_purity= besttrack[1]->purity;
  else if(besttrack[1] == nullptr) combined_purity= besttrack[0]->purity;
  else combined_purity = 1-((1-besttrack[0]->purity)*(1-besttrack[1]->purity)); 
  besttrack3D->purity = combined_purity;
  return besttrack3D;
}
 




Bool_t MMFindBestTrack::Finalize(){
  if(fGeneralInfo->isMC()){
    std::cout<<"This run is MC, MMFindBestTrack checks do not apply"<<std::endl;
    return false;
  } 

  if (fVerbose) printf("---> Finalizing MMFindBestTrack\n");
  
  return true;
}
