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
  fIPPhaseAngle = -9999;
  fDvAtMeshPlane = -9999;
  fEcalClusIndex = -999;
}

MMCluster::~MMCluster() {
  fMMHitsInClu.clear();
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


bool MMCluster::MergeAcrossPlanes(MMCluster* inputclus){
  MMchInfo chinfoThis = fMMHitsInClu.at(0)->GetMMchInfo();
  MMchInfo chinfoThat = inputclus->GetHit(0)->GetMMchInfo();
  // want to merge clusters of the same view and quadrant, across different planes
  if (chinfoThis.plane == chinfoThat.plane) return kFALSE;
  if (chinfoThis.view  != chinfoThat.view) return kFALSE;
  if (chinfoThis.quad  != chinfoThat.quad) return kFALSE;


  const double dvAtMeshMax = 5; // mm
  // check the dv at the mesh between the two clusters
  double dv = fTracos.inter - inputclus->GetTracklet().inter;
  
  if (TMath::Abs(dv) > dvAtMeshMax) return kFALSE;

  double x_init_P0=0., y_init_P0=0., x_init_P1=0., y_init_P1=0.;
  if(chinfoThis.plane == 0) {
    x_init_P0 = fMMHitsInClu.at(0)->GetPosition().X();
    y_init_P0 = fMMHitsInClu.at(0)->GetPosition().Y();

    x_init_P1 = inputclus->GetHit(0)->GetPosition().X();
    y_init_P1 = inputclus->GetHit(0)->GetPosition().Y();
  }
  else if(chinfoThis.plane == 1) {
    x_init_P1 = fMMHitsInClu.at(0)->GetPosition().X();
    y_init_P1 = fMMHitsInClu.at(0)->GetPosition().Y();

    x_init_P0 = inputclus->GetHit(0)->GetPosition().X();
    y_init_P0 = inputclus->GetHit(0)->GetPosition().Y();
  }
  else {
    std::cerr<<"[MergeAcrossPlanes] Planes in MMchInfo NOT RECOGNIZED!!!"<<std::endl;
    return kFALSE;
  }

  
  fFitter.SetFCN(5, fMMTrackFcn); // 5 parameters
  TString parnames[5] = {"RefPlane0_X","RefPlane0_Y","RefPlane1_X","RefPlane1_Y","DZOffset"};
  double parinput[5] = {x_init_P0,y_init_P0,x_init_P1,y_init_P1,0.};
  double parstep[5] = {0.1,0.1,0.1,0.1,0.1};
  for (int ip=0; ip < 5; ip++) {
    fFitter.Config().ParSettings(ip) = ROOT::Fit::ParameterSettings(parnames[ip].Data(),parinput[ip],parstep[ip]);
  }
  // now decide if fit is XZ view, YZ view or 3d: 0-> fit y positions, 1-> fit x positions, 2->fit x and y positions
  // need to add 10 if dZ to be fitted
  int fitmode = 10 + chinfoThis.view; // 11 for fitting XZ view and DZ; 10 for fitting YZ and DZ 
  for (int i=0; i<2; i++) fFitter.Config().ParSettings(chinfoThis.view+2*i).Fix(); // for Y view, fix parameters 0,2; for X view fix 1,3
  
  fMMTrackFcn.setFitMode(fitmode);
  vector<MMSoftHit*> hitArray;
  for (Int_t i= 0; i<(int)fMMHitsInClu.size(); i++) hitArray.push_back(fMMHitsInClu.at(i));
  for (Int_t i= 0; i<(int)inputclus->GetHitsVectorSize(); i++) hitArray.push_back(inputclus->GetHit(i));

  bool calibration = kTRUE; //TO BE DEFINED IN GENERAL SETTING
  if (fIpmode) InitFit(hitArray); // if IP is not used
  else {
    if(!calibration) InitFit(hitArray,GeneralInfo::GetInstance()->GetTargetPos().X(),GeneralInfo::GetInstance()->GetTargetPos().Y(),GeneralInfo::GetInstance()->GetTargetPos().Z());
    else InitFit(hitArray);
  }
  
  bool okfit = fFitter.FitFCN();
  const ROOT::Fit::FitResult & result = fFitter.Result(); 

  std::cout<<"[MERGE PLANES] dv: "<<dv<<" okfit:"<<okfit<<" chi2: "<<result.MinFcnValue()<<std::endl;
  if (!okfit) {    
    //    result.Print(std::cout);
    return kFALSE;
  }

  // success: update the cluster
  for (Int_t i= 0; i<(int)inputclus->GetHitsVectorSize(); i++) {
    fMMHitsInClu.push_back(inputclus->GetHit(i));
  }
  int otherplane = inputclus->GetHit(0)->GetMMchInfo().plane;
  fNHitsPerPlane[otherplane] = inputclus->GetNHitsPerPlane(otherplane);

  fTracos.chi2 = result.MinFcnValue();
  for (int i=0; i<5; i++) fTracos.pars[i] = result.GetParams()[i];

  fTracos.slope = (fTracos.pars[3-chinfoThis.view]- fTracos.pars[1-chinfoThis.view])/(GeneralInfo::GetInstance()->GetMMPosPlaneZ(1)-GeneralInfo::GetInstance()->GetMMPosPlaneZ(0));
  fTracos.inter = fTracos.pars[1-chinfoThis.view] + fTracos.slope*(GeneralInfo::GetInstance()->GetMMPosPlaneZ(2)-GeneralInfo::GetInstance()->GetMMPosPlaneZ(0));

  double cosv = fTracos.slope/TMath::Sqrt(1+fTracos.slope*fTracos.slope);
  double cosz = 1./TMath::Sqrt(1+fTracos.slope*fTracos.slope);
  
  fTracos.lambda[1-chinfoThis.view] = cosv; 
  fTracos.lambda[chinfoThis.view] = 0.;     
  fTracos.lambda.SetZ(cosz);

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
    ers[1-view] = 1.2/TMath::Sqrt(12.); // WILL USE VALUE FROM CONFIG
    ers[2] = 5.*0.105;// z = hit->GetTime()*0.105, here consider deltaT = 20 ns
    TVector3 errors;
    errors.SetXYZ(ers[0],ers[1],ers[2]);
    fErrors.push_back(errors);
    TVector3 positions;
    //positions.SetXYZ(hit->GetPosition().X(),hit->GetPosition().Y(),hit->GetPosition().Z());
    positions.SetXYZ(hit->GetPosition().X(),hit->GetPosition().Y(),hit->GetZfromTime());
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
    zhits.push_back(fMMHitsInClu.at(i)->GetZfromTime());
    vhits.push_back(fMMHitsInClu.at(i)->GetPosition()[1-fMMHitsInClu.at(i)->GetMMchInfo().view]); // view == 1 corresponds to Xview
    //std::cout<<"pippo"<<std::endl;
  }
  //zhits.push_back(softhit->GetPosition().Z());
  zhits.push_back(softhit->GetZfromTime());
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
    
    if (TMath::Abs(fIPPhaseAngle) > IPSINCUT*4) return kFALSE;
  }
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
    std::cout<<"z_rms2 = "<<z_rms2<<std::endl;
    z_rms2 = very_small;
  }
  
  double mt_avg = (zv_avg - z_avg*v_avg)/(z_rms2);
  double ct_tmp = v_avg - mt_avg*z_avg; // intercept at 0 PADME reference frame
  double chi2 = 0;
  for (int i=0; i<npts; i++){
    double residual = (vhits.at(i)-(mt_avg*zhits.at(i) + ct_tmp));
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
