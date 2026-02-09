#include "MMCluster.hh"
#include "GeneralInfo.hh"

//INIZIALIZZARE LE VARIABILI PRIVATE
MMCluster::MMCluster(Int_t ipmode, Int_t clumode) {
  fIpmode = ipmode;
  fClumode = clumode;

  fMMHitsInClu.clear();
  
  fSeedSlope = -999;
  for(Int_t h=0; h<2; h++) {
    fNHitPerPlane[h] = 0;
  }
  fIsolationflag = -999;

  fTracos.slope = -9999;
  fTracos.inter = -9999;
  fTracos.chi2 = -9999;
  for(Int_t p=0; p<4; p++) {
    pars[p] = -9999;
  }
  fTracos.lambda = {-9999,-9999,-9999};

  fEcalClusIndex = -999;
}

MMCluster::~MMCluster() {
  fMMHitsInClu.clear();
};


bool MMCluster::AddHit(MMSoftHit* softhit) {
  // caso 1a hit -> aggiunge e basta [no aggiornare tracklet];
  // caso 2a hit [senza IP] -> fit scemo [aggiornare tracklet];
  // caso 2a hit [IP] -> fit vero con IP [aggiornare tracklet se passa controllo];
  // caso 3a+ hit -> fit vero (con o senza IP) [aggiornare tracklet se passa controllo];
  if(fMMHitsInClu.size() == 0) {
    fMMHitsInClu.push_back(softhit);
    fNHitPerPlane[softhit->GetMMchInfo().plane]++;
    return kTRUE;
  }

  // straight line between past hits and present hit
  vector<double> zhits, vhits;
  
  for (int i=0; i<fMMHitsInClu.size(); i++){
    zhits.push_back(fMMHitsInClu.at(i)->GetPosition().Z());
    vhits.push_back(fMMHitsInClu.at(i)->GetMMchInfo().view == Xview ? fMMHitsInClu.at(i)->GetPosition().X() : fMMHitsInClu.at(i)->GetPosition().Y());
  }
  zhits.push_back(softhit->GetPosition().Z());
  vhits.push_back(softhit->GetMMchInfo().view == Xview ? sofhit->GetPosition().X() : softhit->GetPosition().Y());
  double v_avg,z_avg,mt_avg,ct_avg,cosv,cosz;
  evaluateStraightLineTwoD(vhits,zhits, &v_avg, &z_avg, &mt_avg, &ct_avg, &cosv, &cosz);
    
//  double z_avg = 0.5*(z_now+z_pre); //TODO semmai aggiungere gli errori
//  double v_avg = 0.5*(v_now+v_pre);
//  double zv_avg = 0.5*(z_now*v_now+z_pre*v_pre);
//  double z2_avg = 0.5*(z_now*z_now+z_pre*z_pre);
//  // dv/dz slope
//  double mt_avg = (zv_avg - z_avg*v_avg)/(z2_avg-z_avg*z_avg);
//
//  // intercept at the mesh plane
//  double ct_avg = v_avg - mt_avg*(z_avg - GeneralInfo::GetInstance()->GetMMPosPlaneZ(3));

  // if ip is to be used, evaluate sin of angle between target position wrt magnet center and average position of the two chamber hits wrt magnet center      
  if(fIpmode == 0) {
    double v_target = fMMHitsInClu.at(0)->GetMMchInfo().view == Xview ? GeneralInfo::GetInstance()->GetTargetPos().X() : GeneralInfo::GetInstance()->GetTargetPos().Y();
    double z_target = GeneralInfo::GetInstance()->GetTargetPos().Z();
    //      double sint = (v_avg*z_target - z_avg*v_target)/(sqrt(v_avg*v_avg + z_avg*z_avg)*sqrt(v_target*v_target+z_target*z_target));
    //      if(sint > IPSINCUT) return kFALSE;

    double ravg_minus_target[2] = {v_avg-v_target, z_avg-z_target};
    double lambda_hits[2] = {cosv,cosz};
    double ravg_minus_target_norm = 0;
    for (int q=0; q<2; q++) ravg_minus_target_norm += ravg_minus_target[q]*ravg_minus_target[q];
    ravg_minus_target_norm = TMath::Sqrt(ravg_minus_target_norm);
    for (int q=0; q<2; q++){
      ravg_minus_target[q] /= ravg_minus_target_norm;
      lambda_hits[q] /= lambda_hits_norm;
    }
    double phasen = lambda_hits[0]*ravg_minus_target[1]-lambda_hits[1]*ravg_minus_target[0];
    if (TMath::Abs(phasen) > IPSINCUT) return kFALSE;
  }
  // add present hit to the cluster    
  fMMHitsInClu.push_back(softhit);
  fSeedSlope = mt_avg;
  fNHitPerPlane[softhit->GetMMchInfo().plane]++;
    
  fTracos.slope = mt_avg;
  fTracos.inter = ct_avg;
  // evaluate intercepts at the two planes
  for(Int_t pl = 0; pl<2; pl++) {
    double v_pl = mt_avg*(GeneralInfo::GetInstance()->GetMMPosPlaneZ(pl) - GeneralInfo::GetInstance()->GetMMPosPlaneZ(3)) + ct_avg;       
    fTracos.par[2*pl] = fMMHitsInClu.at(0)->GetMMchInfo().view == Xview ?  v_pl : fMMHitsInClu.at(0)->GetPosition().X();
    fTracos.par[2*pl+1] = fMMHitsInClu.at(0)->GetMMchInfo().view == Yview ?  v_pl : fMMHitsInClu.at(0)->GetPosition().Y();
  }
    
  fTracos.lambda.X() = fMMHitsInClu.at(0)->GetMMchInfo().view == Xview ?  cosv : 0.;
  fTracos.lambda.Y() = fMMHitsInClu.at(0)->GetMMchInfo().view == Yview ?  cosv : 0.;
  fTracos.lambda.Z() = cosz;
  return kTRUE;
  
}


void MMCluster::evaluateStraightLineTwoD(vector<double>vhits,vector<double>zhits, double* v_avgout, double* z_avgout, double* mt_avgout, double* ct_avgout,double* cosvout,double* coszout){
  double z_avg, v_avg, zv_avg, z2_avg = 0;
  for (int i=0; i<vhits.size(); i++){
    z_avg += zhits.at(i);
    v_avg += vhits.at(i);
    zv_avg += vhits.at(i)*zhits.at(i);
    z2_avg += zhits.at(i)*zhits.at(i);
  }
  z_avg  /= vhits.size();
  v_avg  /= vhits.size();
  zv_avg /= vhits.size();
  z2_avg /= vhits.size();

  double mt_avg = (zv_avg - z_avg*v_avg)/(z2_avg-z_avg*z_avg);
  double ct_avg = v_avg - mt_avg*(z_avg - GeneralInfo::GetInstance()->GetMMPosPlaneZ(3));
  double normo = 1./TMath::Sqrt(1+mt_avg*mt_avg);

  *v_avgout = v_avg;
  *z_avgout = z_avg;
  *mt_avgout = mt_avg;
  *ct_avgout = ct_avg;
  *cosvout = mt_avg*normo;
  *coszout = normo;
}
