#include "MMTracker.hh"
#include "GeneralInfo.hh"


//MMTracker* MMTracker::fInstance = 0;

//MMTracker* MMTracker::GetInstance(){
//  if ( fInstance == 0 ) { fInstance = new MMTracker(); }
//  return fInstance;
//}

MMTracker::MMTracker(int fitMode){
  fFitMode = fitMode; //fitMode = 0,1 -> X/Y view 2-> 3d view
}

MMTracker::~MMTracker(){
  if (fParRes) delete fParRes;
}


void MMTracker::InitFitter(){  
  fParRes = new double[4];
  fFitter.SetFCN(4, fMyFcn); // 4 parameters
  TString parnames[4] = {"RefPlane0_X","RefPlane0_Y","RefPlane1_X","RefPlane1_Y"};
  double parinput[4] = {0.,0.,0.,0.};
  double parstep[4] = {0.1,0.1,0.1,0.1};
  for (int ip=0; ip < 4; ip++) {
    fFitter.Config().ParSettings(ip) = ROOT::Fit::ParameterSettings(parnames[ip].Data(),parinput[ip],parstep[ip]);
  }
  // pass the Z reference setting
  
  fMyFcn.setReferenceZPlanes(
			     GeneralInfo::GetInstance()->GetMMPosPlaneZ(0),
			     GeneralInfo::GetInstance()->GetMMPosPlaneZ(1)
			     );  
  // RELEASE DOESNT WORK
  //  for (int i=0; i<4; i++) fFitter.Config().ParSettings(i).Release(); // for fitMode = 0 -> release par1,3 ; for fitMode = 0, release par1,3
  
  if (fFitMode == 0){
    for (int i=0; i<2; i++) fFitter.Config().ParSettings(2*i).Fix(); 
  } else if (fFitMode == 1){
    for (int i=0; i<2; i++) fFitter.Config().ParSettings(1+2*i).Fix(); 
  }	
  fMyFcn.setFitMode(fFitMode);
}

void MMTracker::InitFit(){
  setPoints(); // evaluate the errors per point
  fMyFcn.setPositions(fPositions);
  fMyFcn.setErrors(fErrors);

  // evaluate the reference point as a weighted average
  TVector3 refp(0,0,0);
  TVector3 refe(0,0,0);
  for (Int_t i= 0; i<hitArray.size(); i++) {
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

bool MMTracker::MakeFit(){
  if (hitArray.size() == 1 || hitArray.size() == 2) {
    for (int i=0; i<4; i++) fParRes[i] = fFitter.Config().ParamsValues()[i];    
    fMinFCN = -1;
    return kTRUE;
  }

  bool okfit = fFitter.FitFCN();
  if (!okfit) return kFALSE;
  const ROOT::Fit::FitResult & result = fFitter.Result(); 
  //  result.Print(std::cout);
  fMinFCN = result.MinFcnValue();
  for (int i=0; i<4; i++) fParRes[i] = result.GetParams()[i];
  return kTRUE;
}


void MMTracker::setPoints(){
  fPositions.clear();
  fErrors.clear();
  for (Int_t i= 0; i<hitArray.size(); i++) {
    TRecoVHit* hit = hitArray.at(i);
    int chId = hit->GetChannelId();
    int bdid = (chId & 0xF00 ) >> 8; // board SN 0-15
    int layer = bdid/2;  // layer 0-7
    int plane = layer/4;  // plane 0-1
    int side = bdid%2; // left/right (X view), bottom/top (Y view)
    int strip = (chId & 0x0FF); // strip 0-255
    int view = (layer/2)%2; // 0 means Y view, 1 means X view    
    int otherview[8] = {0,1,1,0,0,1,1,0}; // 0 means the half-strip left (bottom) depending on the view
    
    double ers[3];
    ers[view] = 2.*TMath::Abs(hit->GetPosition()[view])/TMath::Sqrt(12.); // WILL USE VALUE FROM CONFIG
    ers[1-view] = 1.2/TMath::Sqrt(12.); // WILL USE VALUE FROM CONFIG
    ers[2] = 10.*0.105;// z = hit->GetTime()*0.105, here consider deltaT = 20 ns
    TVector3 errors;
    errors.SetXYZ(ers[0],ers[1],ers[2]);
    fErrors.push_back(errors);
    TVector3 positions;
    positions.SetXYZ(hit->GetPosition().X(),hit->GetPosition().Y(),hit->GetPosition().Z());
    fPositions.push_back(positions);
  }
}
