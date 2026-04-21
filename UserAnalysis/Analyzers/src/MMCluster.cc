#include "MMCluster.hh"
#include "GeneralInfo.hh"

//INIZIALIZZARE LE VARIABILI PRIVATE
MMCluster::MMCluster(Int_t ipmode, Int_t clumode) {
  fIpmode = ipmode;
  fClumode = clumode;

  fMMHitsInClu.clear();
  
  fSeedSlope = -999;
  for(Int_t h=0; h<2; h++) {
    fNHitsPerPlane[h] = 0;
  }
  fIsolationFlag = -999;

  fTracos.slope = -9999;
  fTracos.inter = -9999;
  fTracos.chi2 = -9999;
  for(Int_t p=0; p<5; p++) {
    fTracos.pars[p] = -9999;
  }
  fTracos.lambda = {-9999,-9999,-9999};
  fTracos.vres.clear();
    
  fIPPhaseAngle = -9999;
  fDvAtMeshPlane = -9999;
  fEcalClusIndex = -999;
}

MMCluster::~MMCluster() {
  fMMHitsInClu.clear();
  fTracos.vres.clear();
}

void MMCluster::Print() {
  std::cout<<"\t Cluster of IPmode: "<<fIpmode<<" Clumode: "<<fClumode<<std::endl;
  std::cout<<"\t Number of hits -> Plane 0: "<<fNHitsPerPlane[0]<<" | Plane 1: "<<fNHitsPerPlane[1]<<std::endl;
  std::cout<<"\t Hits belonging to cluster: "<<std::endl;
  for(Int_t h=0; h<(int) fMMHitsInClu.size(); h++) {
    std::cout<<"\t\t";
    fMMHitsInClu.at(h)->Print();
  }
  std::cout<<"\t Slope of the first doublet (cluster seed): "<<fSeedSlope<<std::endl; 
  std::cout<<"\t IP phase angle: "<<fIPPhaseAngle<<std::endl;

  std::cout<<"\t Cluster parameters: "<<std::endl;
  std::cout<<"\t\t [m,c] = ["<<fTracos.slope<<" , "<<fTracos.inter<<"]"<<std::endl;
  std::cout<<"\t\t chi2: "<<fTracos.chi2<<std::endl;
  std::cout<<"\t\t [x0,y0] = ["<<fTracos.pars[0]<<" , "<<fTracos.pars[1]<<"]"<<std::endl;
  std::cout<<"\t\t [x1,y1] = ["<<fTracos.pars[2]<<" , "<<fTracos.pars[3]<<"]"<<std::endl;
  std::cout<<"\t\t dt: "<<fTracos.pars[4]<<std::endl;
  std::cout<<"\t\t lambda = ["<<fTracos.lambda.X()<<" , "<<fTracos.lambda.Y()<<" , "<<fTracos.lambda.Z()<<"]"<<std::endl; 
  
  std::cout<<"\t Dv at mesh plane: "<<fDvAtMeshPlane<<" (Clumode >=1 only)"<<std::endl;
  std::cout<<"\t Cluster connected to ECAL clu: "<<fEcalClusIndex<<std::endl;
  std::cout<<"\t Isolation flag "<<fIsolationFlag<<std::endl;

  std::cout<<std::endl;
  std::cout<<std::endl;
}

void MMCluster::Import(MMCluster* oldclu){
  //  copy cluster content from input cluster
  //  fClumode and fIpmode are kept as in the cluster object constructor

  fMMHitsInClu.clear();
  for (int i=0; i < (int) oldclu->GetHitsVectorSize(); i++) fMMHitsInClu.push_back(oldclu->GetHit(i));    
  
  fSeedSlope = oldclu->GetSeedSlope();
  for(Int_t h=0; h<2; h++) fNHitsPerPlane[h] = oldclu->GetNHitsPerPlane(h);
  
  fIsolationFlag = oldclu->GetIsolationFlag();
  fTracos = oldclu->GetTracklet();
  fEcalClusIndex = oldclu->GetEcalClusIndex();
  fIPPhaseAngle = oldclu->GetIPPhaseAngle();
  fDvAtMeshPlane = oldclu->GetDvAtMeshPlane();
}

bool MMCluster::FitWithClusterTime(double xEcal, double yEcal, double tEcal){
  // build complete hit vector
  vector<MMSoftHit*> hitArray;
  for (Int_t i= 0; i<(int)fMMHitsInClu.size(); i++) hitArray.push_back(fMMHitsInClu.at(i));

  // init fit
  double x_init_P0=0., y_init_P0=0., x_init_P1=0., y_init_P1=0.;  
  fFitter.SetFCN(5, fMMTrackFcn); // 5 parameters
  TString parnames[5] = {"RefPlane0_X","RefPlane0_Y","RefPlane1_X","RefPlane1_Y","DZOffset"};
  double parinput[5] = {x_init_P0,y_init_P0,x_init_P1,y_init_P1,0.};
  double parstep[5] = {0.1,0.1,0.1,0.1,0.1};
  for (int ip=0; ip < 5; ip++)  fFitter.Config().ParSettings(ip) = ROOT::Fit::ParameterSettings(parnames[ip].Data(),parinput[ip],parstep[ip]);
  
  // now decide if fit is XZ view, YZ view or 3d: 0-> fit y positions, 1-> fit x positions, 2->fit x and y positions
  MMchInfo chinfoThis =  fMMHitsInClu.at(0)->GetMMchInfo(); // IN FUTURE MIGHT CHECK THE VIEW FOR ALL HITS AND IF X and Y ARE FOUND -> FIT 3d

  int fitmode = 10+chinfoThis.view; // 1 for fitting XZ view; 0 for fitting YZ 
  for (int i=0; i<2; i++) fFitter.Config().ParSettings(chinfoThis.view+2*i).Fix(); // for Y view, fix parameters 0,2; for X view fix 1,3
  // FIX PAR 4 (DZ) to the input value

  double refTimeZ = GeneralInfo::GetInstance()->GetMMECALdz(chinfoThis.view, xEcal, yEcal, tEcal);
  fFitter.Config().ParSettings(4).SetValue(refTimeZ);
  if(!CALIBRATION) fFitter.Config().ParSettings(4).Fix(); // Fix DZ

  fMMTrackFcn.setFitMode(fitmode);
  
 
  InitFit(hitArray); 
  bool okfit = fFitter.FitFCN();
  const ROOT::Fit::FitResult & result_temp = fFitter.Result(); 

  //----------------------------------------------- HIT REJECTION
  int Nhit_P0 = fNHitsPerPlane[0];
  int Nhit_P1 = fNHitsPerPlane[1];
  int Nhit_tot = fMMHitsInClu.size();
  
  for(int hr=0; hr<10; hr++) {

    fMMTrackFcn.ComputeResiduals(result_temp.GetParams());
    double chi2 = result_temp.MinFcnValue();
    double ndf = result_temp.Ndf();
    double pchi2 = ROOT::Math::chisquared_cdf_c(chi2, (int)hitArray.size()-2);
    
    /*if (!okfit) {    
    //    result.Print(std::cout);
    return kFALSE;
    }*/
    int res_size = fMMTrackFcn.GetResVectorLenght();
    double res_m2_max = -1;
    int i_max = 0;
    for(int i=0; i<res_size; i++) {
      double res_m2 = fMMTrackFcn.GetResidual(i).Mag2();
      if(res_m2 > res_m2_max) {
	res_m2_max = res_m2;
	i_max = i;
      }
    }

    if(i_max - Nhit_P0 >= 0) Nhit_P1--;
    else Nhit_P0--;
    
    if(pchi2 < 0.05  && hitArray.size() > 4){ // && Nhit_P0>1 && Nhit_P1>1) {
      hitArray.erase(hitArray.begin()+i_max);
      InitFit(hitArray);
      bool okfit_new = fFitter.FitFCN();
      
      double chi2_new = result_temp.MinFcnValue();
      double ndf_new = result_temp.Ndf();
      double pchi2_new = ROOT::Math::chisquared_cdf_c(chi2_new, (int)hitArray.size()-2);
      
      if(pchi2_new > 0.05) {
	okfit = okfit_new;
      }
    }
  }
  //-------------------------------------------------
  
  const ROOT::Fit::FitResult & result = fFitter.Result(); 
  fMMTrackFcn.ComputeResiduals(result.GetParams());
  
  if (!okfit) {    
    //    result.Print(std::cout);
    return kFALSE;
  }

  // success
  
  fTracos.chi2 = result.MinFcnValue();
  for (int i=0; i<5; i++) fTracos.pars[i] = result.GetParams()[i];

  fTracos.slope = (fTracos.pars[3-chinfoThis.view]- fTracos.pars[1-chinfoThis.view])/(GeneralInfo::GetInstance()->GetMMPosPlaneZ(1)-GeneralInfo::GetInstance()->GetMMPosPlaneZ(0));
  fTracos.inter = fTracos.pars[1-chinfoThis.view] + fTracos.slope*(GeneralInfo::GetInstance()->GetMMPosPlaneZ(2)-GeneralInfo::GetInstance()->GetMMPosPlaneZ(0));

  double cosv = fTracos.slope/TMath::Sqrt(1+fTracos.slope*fTracos.slope);
  double cosz = 1./TMath::Sqrt(1+fTracos.slope*fTracos.slope);
  
  fTracos.lambda[1-chinfoThis.view] = cosv; 
  fTracos.lambda[chinfoThis.view] = 0.;     
  fTracos.lambda.SetZ(cosz);

  fTracos.vres.clear();
  int vres_size = fMMTrackFcn.GetResVectorLenght();
  //std::cout<<"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA Nhit:"<<vres_size<<std::endl;
  for(int i=0; i<vres_size; i++) {
    TVector3 res = fMMTrackFcn.GetResidual(i);
    fTracos.vres.push_back(res);
  }
  //std::cout<<"CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC Nhit:"<<fTracos.vres.size()<<std::endl;
  
  //IP phase angle in Clu Mode 1
  double v_target = GeneralInfo::GetInstance()->GetTargetPos()[1-fMMHitsInClu.at(0)->GetMMchInfo().view];
  double z_target = GeneralInfo::GetInstance()->GetTargetPos().Z();
  
  double ravg_minus_target[2] = {fTracos.inter-v_target, GeneralInfo::GetInstance()->GetMMPosPlaneZ(2)-z_target};
  double lambda_hits[2] = {cosv,cosz};
  double ravg_minus_target_norm = TMath::Sqrt(ravg_minus_target[0]*ravg_minus_target[0]+ravg_minus_target[1]*ravg_minus_target[1]);
  for (int q=0; q<2; q++) ravg_minus_target[q] /= ravg_minus_target_norm;
  fIPPhaseAngle = lambda_hits[0]*ravg_minus_target[1]-lambda_hits[1]*ravg_minus_target[0];  
  
  return kTRUE;
}


bool MMCluster::SimpleFitWithClusterTime(double xEcal, double yEcal, double tEcal) {
  MMchInfo chinfoThis =  fMMHitsInClu.at(0)->GetMMchInfo();
  double dz = GeneralInfo::GetInstance()->GetMMECALdz(chinfoThis.view, xEcal, yEcal, tEcal);

  vector<double> zhits, vhits;  
  for (int i=0; i<(int)fMMHitsInClu.size(); i++){
    double z_hit_corr = fMMHitsInClu.at(i)->GetZfromTime(1.);
    if(fMMHitsInClu.at(i)->GetMMchInfo().plane == 0) z_hit_corr -= dz;
    else z_hit_corr += dz;
    zhits.push_back(z_hit_corr);
    vhits.push_back(fMMHitsInClu.at(i)->GetPosition()[1-fMMHitsInClu.at(i)->GetMMchInfo().view]); // view == 1 corresponds to Xview
 
  }
  double v_avg,z_avg,mt_avg,ct_avg,cosv,cosz,chi2;
  evaluateStraightLineTwoD(vhits,zhits, &v_avg, &z_avg, &mt_avg, &ct_avg, &cosv, &cosz, &chi2); // add quality control?

  for(int h=0; h<10; h++) {
    
    double pchi2 = ROOT::Math::chisquared_cdf_c(chi2, (int)zhits.size()-2);

    double res_m2_max = -1;
    int i_max = 0;
    for(int r=0; r<(int)zhits.size(); r++) {
      double z_hit = zhits.at(r) - GeneralInfo::GetInstance()->GetMMPosPlaneZ(2);
      double v_hit = vhits.at(r);
      
      double v_reco = mt_avg * z_hit + ct_avg;
      double v_res2 = (v_hit - v_reco)*(v_hit - v_reco);
      
      if(v_res2 > res_m2_max) {
	res_m2_max = v_res2;
	i_max = r;
      }
    }
    
    if(pchi2 < 0.05 && zhits.size() > 4) {
      zhits.erase(zhits.begin()+i_max);
      vhits.erase(vhits.begin()+i_max);
      
      std::cout<<"it: "<<h<<" ENTRATO: "<<zhits.size()<<std::endl;
      
      evaluateStraightLineTwoD(vhits,zhits, &v_avg, &z_avg, &mt_avg, &ct_avg, &cosv, &cosz, &chi2); // add quality control?
    }
  }  

  int Nhit = zhits.size();
  vector<TVector3> residues;
  for(int r=0; r<Nhit; r++) {
    double z_hit = zhits.at(r) - GeneralInfo::GetInstance()->GetMMPosPlaneZ(2);
    double v_hit = vhits.at(r);
    
    double v_reco = mt_avg * z_hit + ct_avg;
    double z_reco = -999;
    if(fabs(mt_avg)>1e-5) z_reco = (v_hit - ct_avg)/mt_avg;
    else std::cerr<<"[SimpleFitWithClusterTime] AIUTO SLOPE NEGATIVA!!!!"<<std::endl;
    
    TVector3 res;
    res[1-chinfoThis.view] = v_hit - v_reco;
    res[chinfoThis.view] = 0;
    res.SetZ(z_hit - z_reco);
    residues.push_back(res);
  }

  fTracos.vres.clear();
  int vres_size = residues.size();
  for(int i=0; i<vres_size; i++) {
    TVector3 res = residues.at(i);
    fTracos.vres.push_back(res);
  }
  
    
  fTracos.slope = mt_avg;
  fTracos.inter = ct_avg;
  // evaluate intercepts at the two planes
  for(Int_t pl = 0; pl<2; pl++) {
    double v_pl = mt_avg*(GeneralInfo::GetInstance()->GetMMPosPlaneZ(pl) - GeneralInfo::GetInstance()->GetMMPosPlaneZ(2)) + ct_avg;
    fTracos.pars[2*pl+1-fMMHitsInClu.at(0)->GetMMchInfo().view] = v_pl; // fit quantity: for view=0 i.e. Yview, pars 1,3
    fTracos.pars[2*pl+fMMHitsInClu.at(0)->GetMMchInfo().view] = fMMHitsInClu.at(0)->GetPosition()[fMMHitsInClu.at(0)->GetMMchInfo().view]; // for view=0, it's the X of the hit (center point)
  }
  
  // since the hits are not sorted in Z, impose that the track in output is always outgoing from the target (cosz > 0)
  int reverseTrack = 1;
  if (cosz < 0) reverseTrack = -1;
  fTracos.lambda[1-fMMHitsInClu.at(0)->GetMMchInfo().view] = reverseTrack*cosv;
  fTracos.lambda[fMMHitsInClu.at(0)->GetMMchInfo().view] = 0; 
  fTracos.lambda.SetZ(reverseTrack*cosz);
  fTracos.chi2 = chi2;

  return true;
}


bool MMCluster::MergeAcrossPlanes(MMCluster* inputclus){
  MMchInfo chinfoThis = fMMHitsInClu.at(0)->GetMMchInfo();
  MMchInfo chinfoThat = inputclus->GetHit(0)->GetMMchInfo();
  // want to merge clusters of the same view and quadrant, across different planes
  if (chinfoThis.plane == chinfoThat.plane) return kFALSE;
  if (chinfoThis.view  != chinfoThat.view) return kFALSE;
  if (chinfoThis.quad  != chinfoThat.quad) return kFALSE;


  const double dvAtMeshMax = 5; // mm
  const double dslopeMAX = 0.040; //old 0.025 
  // check the dv at the mesh between the two clusters
  double dv = fTracos.inter - inputclus->GetTracklet().inter;  
  double dslope = fTracos.slope - inputclus->GetTracklet().slope;  
  if (TMath::Abs(dv) > dvAtMeshMax) return kFALSE;
  if (TMath::Abs(dslope) > dslopeMAX) return kFALSE;

  // success: update the cluster
  for (Int_t i= 0; i<(int)inputclus->GetHitsVectorSize(); i++) {
    fMMHitsInClu.push_back(inputclus->GetHit(i));
  }
  int otherplane = inputclus->GetHit(0)->GetMMchInfo().plane;
  fNHitsPerPlane[otherplane] = inputclus->GetNHitsPerPlane(otherplane);
  
  for (int i=0; i<5; i++) fTracos.pars[i] = 0.5*(fTracos.pars[i] + inputclus->GetTracklet().pars[i]);
  fTracos.slope = 0.5*(fTracos.slope + inputclus->GetTracklet().slope);
  fTracos.inter = 0.5*(fTracos.inter + inputclus->GetTracklet().inter);
  
  double cosv = fTracos.slope/TMath::Sqrt(1+fTracos.slope*fTracos.slope);
  double cosz = 1./TMath::Sqrt(1+fTracos.slope*fTracos.slope);
  
  fTracos.lambda[1-chinfoThis.view] = cosv; 
  fTracos.lambda[chinfoThis.view] = 0.;     
  fTracos.lambda.SetZ(cosz);
  
  //IP phase angle in Clu Mode 1
  double v_target = GeneralInfo::GetInstance()->GetTargetPos()[1-fMMHitsInClu.at(0)->GetMMchInfo().view];
  double z_target = GeneralInfo::GetInstance()->GetTargetPos().Z();
  
  double ravg_minus_target[2] = {fTracos.inter-v_target, GeneralInfo::GetInstance()->GetMMPosPlaneZ(2)-z_target};
  double lambda_hits[2] = {fTracos.lambda[1-chinfoThis.view],fTracos.lambda.Z()};
  double ravg_minus_target_norm = TMath::Sqrt(ravg_minus_target[0]*ravg_minus_target[0]+ravg_minus_target[1]*ravg_minus_target[1]);
  for (int q=0; q<2; q++) ravg_minus_target[q] /= ravg_minus_target_norm;
  fIPPhaseAngle = lambda_hits[0]*ravg_minus_target[1]-lambda_hits[1]*ravg_minus_target[0];    
  
  return kTRUE;
}


void MMCluster::InitFit(vector<MMSoftHit*> hitArray, double x, double y, double z){ // probably will need to pass errors as well
  InitFit(hitArray);
  setAdditionalPoint(x,y,z);
  fMMTrackFcn.setPositions(fPositions);
  fMMTrackFcn.setErrors(fErrors);
  fMMTrackFcn.setBoardIds(fBoardIds);
  fMMTrackFcn.setReferenceZPlanes(GeneralInfo::GetInstance()->GetMMPosPlaneZ(0),GeneralInfo::GetInstance()->GetMMPosPlaneZ(1));
}

void MMCluster::InitFit(vector<MMSoftHit*> hitArray){
  setPoints(hitArray); // evaluate the errors per point
  fMMTrackFcn.setPositions(fPositions);
  fMMTrackFcn.setErrors(fErrors);
  fMMTrackFcn.setBoardIds(fBoardIds);
  fMMTrackFcn.setReferenceZPlanes(GeneralInfo::GetInstance()->GetMMPosPlaneZ(0),GeneralInfo::GetInstance()->GetMMPosPlaneZ(1));
  // evaluate the reference point as a weighted average
  TVector3 refp(0,0,0);
  TVector3 refe(0,0,0);
  for (Int_t i= 0; i<(int)hitArray.size(); i++) {
    for (Int_t ix = 0; ix<3; ix++){
      refp[ix] += hitArray.at(i)->GetPosition()[ix]/(fErrors.at(i)[ix]*fErrors.at(i)[ix]);
      refe[ix] += 1./(fErrors.at(i)[ix]*fErrors.at(i)[ix]);
    }
  }
  for (Int_t ix = 0; ix<3; ix++) {
    refp[ix] /= refe[ix];
    refe[ix] /= TMath::Sqrt(refe[ix]);
  }


  // initialise the x,y at Z0, and Z1 [same values used, corresponding to track along Z]

  for (uint i=0; i<2; i++){// x,y
    for (uint j=0; j<2; j++){ // z0, z1
      fFitter.Config().ParSettings(i+2*j).SetValue(refp[i]);
    }
  }

}

void MMCluster::setPoints(vector<MMSoftHit*> hitArray){
  fPositions.clear();
  fErrors.clear();
  fBoardIds.clear();
  for (Int_t i= 0; i<(int)hitArray.size(); i++) {
    MMSoftHit* hit = hitArray.at(i);
    int view = hit->GetMMchInfo().view;
    double ers[3];
    ers[view] = 2.*TMath::Abs(hit->GetPosition()[view])/TMath::Sqrt(12.); // WILL USE VALUE FROM CONFIG
    ers[1-view] = 3*1.2/TMath::Sqrt(12.); // WILL USE VALUE FROM CONFIG
    ers[2] = 3*10*GeneralInfo::GetInstance()->GetMMDriftVelocity();//5.*0.105;// z = hit->GetTime()*0.105, here consider deltaT = 20 ns
    TVector3 errors;
    errors.SetXYZ(ers[0],ers[1],ers[2]);
    fErrors.push_back(errors);
    TVector3 positions;
    //positions.SetXYZ(hit->GetPosition().X(),hit->GetPosition().Y(),hit->GetPosition().Z());
    positions.SetXYZ(hit->GetPosition().X(),hit->GetPosition().Y(),hit->GetZfromTime(1.));
    fPositions.push_back(positions);
    fBoardIds.push_back(hit->GetMMchInfo().bdid);
  }
}

void MMCluster::setAdditionalPoint(double x, double y, double z){ // used to insert the IP as an additional measurement
  TVector3 positions;
  positions.SetXYZ(x,y,z);
  fPositions.push_back(positions);
  TVector3 errors;
  errors.SetXYZ(1.,1.,0.1);
  fErrors.push_back(errors);
  fBoardIds.push_back(-1); // special point, not in any board
}


// caso 1a hit -> aggiunge e basta [no aggiornare tracklet];
// caso 2a hit [senza IP] -> fit scemo [aggiornare tracklet];
// caso 2a hit [IP] -> fit vero con IP [aggiornare tracklet se passa controllo];
// caso 3a+ hit -> fit vero (con o senza IP) [aggiornare tracklet se passa controllo];

bool MMCluster::AddHit(MMSoftHit* softhit) { // specific of level-zero clusters, with / without ip constraint
  const int maxholes=2;
  if(fMMHitsInClu.size() == 0) {
    fMMHitsInClu.push_back(softhit);
    fNHitsPerPlane[softhit->GetMMchInfo().plane]++;
    // std::cout<<"First Hit Added!\n\t";
    //softhit->Print();
    return kTRUE;
  }

  if (softhit->GetMMchInfo().bdid != fMMHitsInClu.at(fMMHitsInClu.size()-1)->GetMMchInfo().bdid) return kFALSE;
  if (TMath::Abs(softhit->GetMMchInfo().strip - fMMHitsInClu.at(fMMHitsInClu.size()-1)->GetMMchInfo().strip) > maxholes) return kFALSE;

  // in principle, we might make cases on the basis of fCluMode here: 3d tracks, etc.
  
  // straight line between past hits and present hit: to speed up, zhits and vhits might become private and push_backed when needed / expunge the last entry when needed
  vector<double> zhits, vhits;  
  for (int i=0; i<(int)fMMHitsInClu.size(); i++){
    //    zhits.push_back(fMMHitsInClu.at(i)->GetPosition().Z());
    zhits.push_back(fMMHitsInClu.at(i)->GetZfromTime(1.));
    vhits.push_back(fMMHitsInClu.at(i)->GetPosition()[1-fMMHitsInClu.at(i)->GetMMchInfo().view]); // view == 1 corresponds to Xview
    //std::cout<<"pippo"<<std::endl;
  }
  //zhits.push_back(softhit->GetPosition().Z());
  zhits.push_back(softhit->GetZfromTime(1.));
  vhits.push_back(softhit->GetPosition()[1-softhit->GetMMchInfo().view]); // view == 1 corresponds to Xview
  double v_avg,z_avg,mt_avg,ct_avg,cosv,cosz,chi2;
  evaluateStraightLineTwoD(vhits,zhits, &v_avg, &z_avg, &mt_avg, &ct_avg, &cosv, &cosz, &chi2); // add quality control?

  // if ip is to be used, evaluate sin of angle between target position wrt magnet center and average position of the two chamber hits wrt magnet center      
  if(fIpmode == 0) {
    double v_target = GeneralInfo::GetInstance()->GetTargetPos()[1-fMMHitsInClu.at(0)->GetMMchInfo().view];
    double z_target = GeneralInfo::GetInstance()->GetTargetPos().Z();

    double ravg_minus_target[2] = {v_avg-v_target, z_avg-z_target};
    double lambda_hits[2] = {cosv,cosz};
    double ravg_minus_target_norm = TMath::Sqrt(ravg_minus_target[0]*ravg_minus_target[0]+ravg_minus_target[1]*ravg_minus_target[1]);
    for (int q=0; q<2; q++) ravg_minus_target[q] /= ravg_minus_target_norm;
    fIPPhaseAngle = lambda_hits[0]*ravg_minus_target[1]-lambda_hits[1]*ravg_minus_target[0];
    

//    std::cout<<"\t\t\t\t IP sin: "<<fIPPhaseAngle<<std::endl;
//    std::cout<<"N hit in clu: "<<fMMHitsInClu.size()<<std::endl;
//    std::cout<<"cosv: "<<cosv<<" cosz: "<<cosz<<std::endl;
    
    if (TMath::Abs(fIPPhaseAngle) > IPSINCUT) return kFALSE;
  }

  double v_new_hit = softhit->GetPosition()[1-softhit->GetMMchInfo().view];
  double z_new_hit = softhit->GetZfromTime(1.);
  // bool hit_rejected = HitRejectionAlgorithm(v_new_hit, z_new_hit, mt_avg, ct_avg); //evaluating the residue in z of the new hit: returns true if the hit is rejected, i.e. z_res > Z_RES_CUT (default DZ_CUT = 5mm)
  //if(hit_rejected) return kFALSE;
  
  // add present hit to the cluster    
  fMMHitsInClu.push_back(softhit);
  if (fMMHitsInClu.size()==2) fSeedSlope = mt_avg;
  fNHitsPerPlane[softhit->GetMMchInfo().plane]++;
    
  fTracos.slope = mt_avg;
  fTracos.inter = ct_avg;
  // evaluate intercepts at the two planes
  for(Int_t pl = 0; pl<2; pl++) {
    double v_pl = mt_avg*(GeneralInfo::GetInstance()->GetMMPosPlaneZ(pl) - GeneralInfo::GetInstance()->GetMMPosPlaneZ(2)) + ct_avg;
    fTracos.pars[2*pl+1-fMMHitsInClu.at(0)->GetMMchInfo().view] = v_pl; // fit quantity: for view=0 i.e. Yview, pars 1,3
    fTracos.pars[2*pl+fMMHitsInClu.at(0)->GetMMchInfo().view] = fMMHitsInClu.at(0)->GetPosition()[fMMHitsInClu.at(0)->GetMMchInfo().view]; // for view=0, it's the X of the hit (center point)
  }
  // since the hits are not sorted in Z, impose that the track in output is always outgoing from the target (cosz > 0)
  int reverseTrack = 1;
  if (cosz < 0) reverseTrack = -1;
  fTracos.lambda[1-fMMHitsInClu.at(0)->GetMMchInfo().view] = reverseTrack*cosv;
  fTracos.lambda[fMMHitsInClu.at(0)->GetMMchInfo().view] = 0; 
  fTracos.lambda.SetZ(reverseTrack*cosz);
  fTracos.chi2 = chi2;


  int Nhit = zhits.size();
  vector<TVector3> residues;
  for(int r=0; r<Nhit; r++) {
    double z_hit = zhits.at(r) - GeneralInfo::GetInstance()->GetMMPosPlaneZ(2);
    double v_hit = vhits.at(r);
    
    double v_reco = mt_avg * z_hit + ct_avg;
    double z_reco = -999;
    if(fabs(mt_avg)>1e-5) z_reco = (v_hit - ct_avg)/mt_avg;
    else std::cerr<<"[AddHit] AIUTO SLOPE NULLA!!!!"<<std::endl;
    
    TVector3 res;
    res[1-softhit->GetMMchInfo().view] = v_hit - v_reco;
    res[softhit->GetMMchInfo().view] = 0;
    res.SetZ(z_hit - z_reco);
    residues.push_back(res);
  }
  
  fTracos.vres.clear();
  int vres_size = residues.size();
  for(int i=0; i<vres_size; i++) {
    TVector3 res = residues.at(i);
    fTracos.vres.push_back(res);
  }
  
  //std::cout<<"New Hit Added!\n\t";
  //softhit->Print();

  return kTRUE;
  
}

void MMCluster::evaluateStraightLineTwoD(vector<double>vhits,vector<double>zhits, double* v_avgout, double* z_avgout, double* mt_avgout, double* ct_avgout,double* cosvout,double* coszout, double*chi2out){
  double very_small = 1e-15;
  double very_wrong = -99999;
  double z_avg = 0, v_avg = 0, zv_avg = 0, z2_avg = 0;
  int npts = vhits.size();
  if(npts<1) {
    *v_avgout  = very_wrong;
    *z_avgout  = very_wrong;
    *mt_avgout = very_wrong;
    *ct_avgout = very_wrong;
    *cosvout   = very_wrong;
    *coszout   = very_wrong;
    *chi2out   = very_wrong;
  }
  
  for (int i=0; i<npts; i++){
    z_avg  += zhits.at(i);
    v_avg  += vhits.at(i);
    zv_avg += vhits.at(i)*zhits.at(i);
    z2_avg += zhits.at(i)*zhits.at(i);
  }
  z_avg  /= npts;
  v_avg  /= npts;
  zv_avg /= npts;
  z2_avg /= npts;

  double z_rms2 = z2_avg-z_avg*z_avg;
  
  if(fabs(z_rms2)<1e-15) {
    //std::cout<<"z_rms2 = "<<z_rms2<<std::endl;
    z_rms2 = very_small;
  }

  double err_v = 3*GeneralInfo::GetInstance()->GetMMStripPitch()/TMath::Sqrt(12);
  double err_z = 3*10*GeneralInfo::GetInstance()->GetMMDriftVelocity();//5.*0.105;// z = hit->GetTime()*0.105, here consider deltaT = 20 ns
  
  double mt_avg = (zv_avg - z_avg*v_avg)/(z_rms2);
  double ct_tmp = v_avg - mt_avg*z_avg; // intercept at 0 PADME reference frame
  double chi2 = 0;
  for (int i=0; i<npts; i++){
    double residual = (vhits.at(i)-(mt_avg*zhits.at(i) + ct_tmp))/TMath::Sqrt(err_v*err_v + err_z*err_z*mt_avg*mt_avg);
    chi2 += residual*residual;
  }

  double ct_avg = v_avg - mt_avg*(z_avg - GeneralInfo::GetInstance()->GetMMPosPlaneZ(2)); //intercept at central MM mesh (local frame)
  double normo = 1./TMath::Sqrt(1+mt_avg*mt_avg);

  *v_avgout = v_avg;
  *z_avgout = z_avg;
  *mt_avgout = mt_avg;
  *ct_avgout = ct_avg;
  *cosvout = mt_avg*normo;
  *coszout = normo;
  *chi2out = chi2;
}

/*
bool MMCluster::HitRejectionAlgorithm(double v_new_hit, double z_new_hit, double mt_avg, double ct_avg) {
  double Z_RES_CUT = 6*1.2; //mm 

  //putting the z of the hit @ mesh
  double z_new_hit_at_mesh = z_new_hit - GeneralInfo::GetInstance()->GetMMPosPlaneZ(2);

  if(fabs(mt_avg)<1e-15) {
    std::cerr<<"[HitRejectionAlgorithm] Found slope = 0 !!!!"<<std::endl;
    return kTRUE;
  }
  double z_res = z_new_hit_at_mesh - (v_new_hit-ct_avg)/mt_avg;

  if(fabs(z_res)<Z_RES_CUT) return kFALSE;
  else return kTRUE;
}
*/

bool MMCluster::HitRejectionAlgorithm() {
  bool hit_rejected_flag = kFALSE;

  double err_v = 3*GeneralInfo::GetInstance()->GetMMStripPitch()/TMath::Sqrt(12);
  double err_z = 3*10*GeneralInfo::GetInstance()->GetMMDriftVelocity();//5.*0.105;// z = hit->GetTime()*0.105, here consider deltaT = 20 ns

  int Nhit = fMMHitsInClu.size();
  int plane = fMMHitsInClu.at(0)->GetMMchInfo().plane;
  int view = fMMHitsInClu.at(0)->GetMMchInfo().view;

  double v_target = GeneralInfo::GetInstance()->GetTargetPos()[1-view];
  double z_target = GeneralInfo::GetInstance()->GetTargetPos().Z();
  
  double v_avg,z_avg,mt_avg,ct_avg,cosv,cosz,chi2;
  vector<double> zhits, vhits;
  vector<MMSoftHit*> hits_temp;
  for(int i=0; i<Nhit; i++) {
    zhits.push_back(fMMHitsInClu.at(i)->GetZfromTime(1.));
    vhits.push_back(fMMHitsInClu.at(i)->GetPosition()[1-view]); // view == 1 corresponds to Xview
    hits_temp.push_back(fMMHitsInClu.at(i));
  }
  fMMHitsInClu.clear();
  
  
  chi2 = fTracos.chi2;
  mt_avg = fTracos.slope;
  ct_avg = fTracos.inter;

  int Niter = 10;
  for(int h=0; h<Niter; h++) {

    double pchi2 = ROOT::Math::chisquared_cdf_c(chi2, Nhit-2);

    if(pchi2 < 0.15 && Nhit > 2) {
      
      double res_m2_max = -1;
      int i_res_max = 0;
      double v_hit_res_max = -999, z_hit_res_max=-999;
      for(int i=0; i<Nhit; i++){
	double z_hit = zhits.at(i) - GeneralInfo::GetInstance()->GetMMPosPlaneZ(2);
	double v_hit = vhits.at(i);
	double v_reco = mt_avg * z_hit + ct_avg;
	
	double residual = (v_hit - v_reco)/TMath::Sqrt(err_v*err_v + err_z*err_z*mt_avg*mt_avg);
	
	double res_m2 = residual*residual;
	
	if(res_m2 > res_m2_max) {
	  res_m2_max = res_m2;
	  i_res_max = i;
	  v_hit_res_max = v_hit;
	  z_hit_res_max = z_hit;
	}
      }

      vector<double> zhits_pair, vhits_pair;
      int not_ip_counter=0;
      for(int i=0; i<Nhit; i++){ //angolo da ip  con doppietti hit res max 
	zhits_pair.clear();
	vhits_pair.clear();
	
	if(i == i_res_max) continue;
  
	double z_hit = zhits.at(i);
	double v_hit = vhits.at(i);

	zhits_pair.push_back(z_hit);
	vhits_pair.push_back(v_hit);
	zhits_pair.push_back(z_hit_res_max + GeneralInfo::GetInstance()->GetMMPosPlaneZ(2));
	vhits_pair.push_back(v_hit_res_max);
		
	double v_avg_pair,z_avg_pair,mt_avg_pair,ct_avg_pair,cosv_pair,cosz_pair,chi2_pair;	
	evaluateStraightLineTwoD(vhits_pair,zhits_pair, &v_avg_pair, &z_avg_pair, &mt_avg_pair, &ct_avg_pair, &cosv_pair, &cosz_pair, &chi2_pair); // add quality control?

	double ravg_minus_target[2] = {v_avg_pair-v_target, z_avg_pair-z_target};
	double lambda_hits[2] = {cosv_pair,cosz_pair};
	double ravg_minus_target_norm = TMath::Sqrt(ravg_minus_target[0]*ravg_minus_target[0]+ravg_minus_target[1]*ravg_minus_target[1]);
	for (int q=0; q<2; q++) ravg_minus_target[q] /= ravg_minus_target_norm;
	double IPPhaseAngle = lambda_hits[0]*ravg_minus_target[1]-lambda_hits[1]*ravg_minus_target[0];

	if (TMath::Abs(fIPPhaseAngle) > IPSINCUT) not_ip_counter++;
      }
      
      if(Nhit >= 3 && (float)not_ip_counter/Nhit > 0.5) {  
	hit_rejected_flag = kTRUE;
	zhits.erase(zhits.begin()+i_res_max);
	vhits.erase(vhits.begin()+i_res_max);
	hits_temp.erase(hits_temp.begin()+i_res_max);	
	fNHitsPerPlane[plane]--;
	Nhit--;
	
	evaluateStraightLineTwoD(vhits,zhits, &v_avg, &z_avg, &mt_avg, &ct_avg, &cosv, &cosz, &chi2); // add quality control?
      }
    }
  }

  for(int i=0; i<(int) hits_temp.size(); i++) fMMHitsInClu.push_back(hits_temp.at(i));
  
  if(hit_rejected_flag) {
    fTracos.slope = mt_avg;
    fTracos.inter = ct_avg;
    // evaluate intercepts at the two planes
    for(Int_t pl = 0; pl<2; pl++) {
      double v_pl = mt_avg*(GeneralInfo::GetInstance()->GetMMPosPlaneZ(pl) - GeneralInfo::GetInstance()->GetMMPosPlaneZ(2)) + ct_avg;
      fTracos.pars[2*pl+1-view] = v_pl; // fit quantity: for view=0 i.e. Yview, pars 1,3
      fTracos.pars[2*pl+view] = fMMHitsInClu.at(0)->GetPosition()[view]; // for view=0, it's the X of the hit (center point)
    }
    
    // since the hits are not sorted in Z, impose that the track in output is always outgoing from the target (cosz > 0)
    int reverseTrack = 1;
    if (cosz < 0) reverseTrack = -1;
    fTracos.lambda[1-view] = reverseTrack*cosv;
    fTracos.lambda[view] = 0; 
    fTracos.lambda.SetZ(reverseTrack*cosz);
    fTracos.chi2 = chi2;
    
    vector<TVector3> residues;
    for(int r=0; r<Nhit; r++) {
      double z_hit = zhits.at(r) - GeneralInfo::GetInstance()->GetMMPosPlaneZ(2);
      double v_hit = vhits.at(r);
      
      double v_reco = mt_avg * z_hit + ct_avg;
      double z_reco = -999;
      if(fabs(mt_avg)>1e-5) z_reco = (v_hit - ct_avg)/mt_avg;
      else std::cerr<<"[HitRejectionAlgorithm] AIUTO SLOPE NULLA!!!!"<<std::endl;
      
      TVector3 res;
      res[1-view] = v_hit - v_reco;
      res[view] = 0;
      res.SetZ(z_hit - z_reco);
      residues.push_back(res);
    }
    
    fTracos.vres.clear();
    int vres_size = residues.size();
    for(int i=0; i<vres_size; i++) {
      TVector3 res = residues.at(i);
      fTracos.vres.push_back(res);
    } 
  }
  
  return kTRUE;
}
