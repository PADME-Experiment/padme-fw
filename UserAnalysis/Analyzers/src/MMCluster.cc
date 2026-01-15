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
  if(fMMHitsInClu.size() == 1) {
    double z_now = softhit->GetPosition().Z();
    double z_pre = fMMHitsInClu.at(0)->GetPosition().Z();

    double v_now = softhit->GetMMchInfo().view == Xview ? sofhit->GetPosition().X() : softhit->GetPosition().Y();
    double v_pre = fMMHitsInClu.at(0)->GetMMchInfo().view == Xview ? fMMHitsInClu.at(0)->GetPosition().X() : fMMHitsInClu.at(0)->GetPosition().Y();
    
    double z_avg = 0.5*(z_now+z_pre); //TODO semmai aggiungere gli errori
    double v_avg = 0.5*(v_now+v_pre);
    double zv_avg = 0.5*(z_now*v_now+z_pre*v_pre);
    double z2_avg = 0.5*(z_now*z_now+z_pre*z_pre);
    
    double mt_avg = (zv_avg - z_avg*v_avg)/(z2_avg-z_avg*z_avg);
    double ct_avg = v_avg - mt_avg*(z_avg - GeneralInfo::GetInstace()->GetMMPosPlaneZ(3));

    if(fIpmode == 0) {
      double v_target = fMMHitsInClu.at(0)->GetMMchInfo().view == Xview ? GeneralInfo::GetInstance()->GetTargetPos().X() : GeneralInfo::GetInstance()->GetTargetPos().Y();
      double z_target = GeneralInfo::GetInstance()->GetTargetPos().Z();
      
      double sint = (v_avg*z_target - z_avg*v_target)/(sqrt(v_avg*v_avg + z_avg*z_avg)*sqrt(v_target*v_target+z_target*z_target));

      if(sint > IPSINCUT) return kFALSE;
    }
    
    fMMHitsInClu.push_back(softhit);
    fSeedSlope = mt_avg;
    fNHitPerPlane[softhit->GetMMchInfo().plane]++;
    
    fTracos.slope = mt_avg;
    fTracos.inter = ct_avg;
    for(Int_t pl = 0; pl<2; pl++) {

      double v_pl = mt_avg*(GeneralInfo::GetInstance()->GetMMPosPlaneZ(pl) - GeneralInfo::GetInstance()->GetMMPosPlaneZ(3)) + ct_avg; 
      
      fTracos.par[2*pl] = fMMHitsInClu.at(0)->GetMMchInfo().view == Xview ?  v_pl : fMMHitsInClu.at(0)->GetPosition().X();
      fTracos.par[2*pl+1] = fMMHitsInClu.at(0)->GetMMchInfo().view == Yview ?  v_pl : fMMHitsInClu.at(0)->GetPosition().Y();
    }
    
    fTracos.lambda.X() = fMMHitsInClu.at(0)->GetMMchInfo().view == Xview ?  mt_avg/sqrt(1+mt_avg*mt_avg) : 0.;
    fTracos.lambda.Y() = fMMHitsInClu.at(0)->GetMMchInfo().view == Yview ?  mt_avg/sqrt(1+mt_avg*mt_avg) : 0.;
    fTracos.lambda.X() = 1/sqrt(1+mt_avg*mt_avg);
    return kTRUE;
  }

  if(fMMHitsInClu.size() > 1) {}
  
}
