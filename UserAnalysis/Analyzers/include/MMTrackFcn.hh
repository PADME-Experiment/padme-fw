#ifndef MMTrackFcn_h
#define MMTrackFcn_h 1
//
// fFitMode = 0/1 -> Y/X view fits; 2 -> 3d fit
//

class MMTrackFcn {
public:
  MMTrackFcn(){fFitMode = 2;}
  void setReferenceZPlanes(double zplane0, double zplane1){
    fRefZPoints[0] = zplane0;
    fRefZPoints[1] = zplane1;
  }
  void setPositions(std::vector<TVector3> val){
    fTrackmmt.clear();
    for (uint i=0; i<val.size(); i++) fTrackmmt.push_back(val.at(i));    
  }
  void setErrors(std::vector<TVector3> val){
    fTrackerr.clear();
    for (uint i=0; i<val.size(); i++) fTrackerr.push_back(val.at(i));    
  }
  void setBoardIds(std::vector<int> val){
    fTrackBdIds.clear();
    for (uint i=0; i<val.size(); i++) fTrackBdIds.push_back(val.at(i));    
  }
  void setDZ(double dz) {
    fDZ = dz;
  }
  void setFitMode(int fitMode){fFitMode = fitMode;}

  double operator()(const double *par) {

    Double_t chisq = 0;
    Double_t delta;

    
    if (fFitMode%10 == 2){ //3D fit
      TVector3 refp[2];
      for (uint i=0; i<2; i++) refp[i].SetXYZ(par[2*i],par[1+2*i],fRefZPoints[i]); // reference points of the straight line
      TVector3 lambda = refp[1];
      lambda -= refp[0];
      lambda *= (1./lambda.Mag()); // direction of the track
      
      // track is \vec{r} = \vec{p0} + \vec{lambda} * t, p0 = barycenter of points
      // point of CDA on the track line: \vec{p0} + ( (\vec{point}_i - \vec{p0}) dot \vec{lambda} ) \vec{lambda}                                                                                                    
      // vector ortogonal to the line at CDA: (\vec{point}_i - \vec{p0}) - ( (\vec{point}_i - \vec{p0}) dot \vec{lambda} ) \vec{lambda}
      
      for (uint i= 0; i<fTrackmmt.size(); i++) {
	TVector3 distance;
	distance.SetX(fTrackmmt.at(i).X());
	distance.SetY(fTrackmmt.at(i).Y());
	distance.SetZ(fTrackmmt.at(i).Z());
	if (fFitMode/10) {// fit DZ
	  if (fTrackBdIds.at(i)>=0 && fTrackBdIds.at(i)<8)       distance.SetZ(distance.Z() - par[4]);
	  else if (fTrackBdIds.at(i)>=8 && fTrackBdIds.at(i)<16) distance.SetZ(distance.Z() + par[4]);
	}
	distance -= refp[0];
	double projection = distance*lambda;
	TVector3 parallelDist = lambda;
	parallelDist *= projection;
      
	distance -= parallelDist ; // ortogonal distance point to line                  
	if (fFitMode%10 != 0){
	  delta  = (distance.X())/fTrackerr.at(i).X();
	  chisq += delta*delta;
	}
	if (fFitMode%10 != 1){
	  delta  = (distance.Y())/fTrackerr.at(i).Y();
	  chisq += delta*delta;
	}
	delta  = (distance.Z())/fTrackerr.at(i).Z();
	chisq += delta*delta;

	//std::cout<<"[FITTER] chi2 step: "<<chisq<<std::endl;
	
      }
    } else { //2D fit
      
      TVector2 refp[2];
      for (uint i=0; i<2; i++) refp[i].Set(par[1-fFitMode%10+2*i],fRefZPoints[i]); // reference points of the straight line
      TVector2 lambda = refp[1];
      lambda -= refp[0];
      lambda *= (1./lambda.Mod()); // direction of the track

//      std::cout<<"[FITTER] initial refp P0: ["<<refp[0].X()<<" , "<<refp[0].Y()<<"]"<<std::endl;
//      std::cout<<"[FITTER] initial refp P1: ["<<refp[1].X()<<" , "<<refp[1].Y()<<"]"<<std::endl;
//      std::cout<<"[FITTER] initial lambda: ["<<lambda.X()<<" , "<<lambda.Y()<<std::endl;
      
      // track is \vec{r} = \vec{p0} + \vec{lambda} * t, p0 = barycenter of points
      // point of CDA on the track line: \vec{p0} + ( (\vec{point}_i - \vec{p0}) dot \vec{lambda} ) \vec{lambda}                                                                                                    
      // vector ortogonal to the line at CDA: (\vec{point}_i - \vec{p0}) - ( (\vec{point}_i - \vec{p0}) dot \vec{lambda} ) \vec{lambda}
      
      for (uint i= 0; i<fTrackmmt.size(); i++) {
	TVector2 point(fTrackmmt.at(i)[1-fFitMode%10],fTrackmmt.at(i)[2]);

	//	std::cout<<"[FITTER] point"<<i<<" : ["<<point.X()<<" , "<<point.Y()<<"]"<<std::endl;

	if (fFitMode/10) {// fit DZ
	  if (fTrackBdIds.at(i)>=0 && fTrackBdIds.at(i)<8)       point.SetY(point.Y() - par[4]);
	  else if (fTrackBdIds.at(i)>=8 && fTrackBdIds.at(i)<16) point.SetY(point.Y() + par[4]);
	}
	
	TVector2 distance = point;
	distance -= refp[0];
	double projection = distance*lambda;
	TVector2 parallelDist = lambda;
	parallelDist *= projection;
	
	//	std::cout << "Prova point " << i << " / " << fTrackmmt.size() << " v = " << fTrackmmt.at(i)[1-fFitMode%10] << " z = " << fTrackmmt.at(i)[2] << " v0 = " << refp[0].X() << " v1 = " << refp[1].X();
	distance -= parallelDist ; // ortogonal distance point to line                  
	delta  = (distance.X())/fTrackerr.at(i)[1-fFitMode%10];
	//	std::cout << " deltav = " << delta;
	chisq += delta*delta;
	delta  = (distance.Y())/fTrackerr.at(i).Z();
	//	std::cout << " deltaz = " << delta << std::endl;
	chisq += delta*delta;
	
	//std::cout<<"[FITTER] chi2 step: "<<chisq<<std::endl;
      }
      //      std::cout << " chi2 = " << chisq;
    }

    if(fDZ != -999) {
      double sigma_dz = 10.;
      chisq += (fDZ-par[4])*(fDZ-par[4])/(sigma_dz*sigma_dz);
    }
    
    return chisq; // Npoints-4 degrees of freedom
  }
  
  
  void ComputeResiduals (const double *par) {

    fTrackres.clear();
    for (uint i= 0; i<fTrackmmt.size(); i++) {
      TVector3 init_res(-999999,-999999,-999999);
      fTrackres.push_back(init_res);
    }
    
    if (fFitMode%10 == 2){ //3D fit
      TVector3 refp[2];
      for (uint i=0; i<2; i++) refp[i].SetXYZ(par[2*i],par[1+2*i],fRefZPoints[i]); // reference points of the straight line
      TVector3 lambda = refp[1];
      lambda -= refp[0];
      lambda *= (1./lambda.Mag()); // direction of the track

      for (uint i= 0; i<fTrackmmt.size(); i++) {
	TVector3 distance;
	distance.SetX(fTrackmmt.at(i).X());
	distance.SetY(fTrackmmt.at(i).Y());
	distance.SetZ(fTrackmmt.at(i).Z());
	if (fFitMode/10) {// fit DZ
	  if (fTrackBdIds.at(i)>=0 && fTrackBdIds.at(i)<8)       distance.SetZ(distance.Z() - par[4]);
	  else if (fTrackBdIds.at(i)>=8 && fTrackBdIds.at(i)<16) distance.SetZ(distance.Z() + par[4]);
	}
	distance -= refp[0];
	double projection = distance*lambda;
	TVector3 parallelDist = lambda;
	parallelDist *= projection;
      
	distance -= parallelDist ; // ortogonal distance point to line                  
	if (fFitMode%10 != 0){ //                                                                      CHE VORDI!!!!
	  fTrackres.at(i).SetX(distance.X());
	}
	if (fFitMode%10 != 1){ //                                                                      CHE VORDI!!!!
	  fTrackres.at(i).SetY(distance.Y());
	}
	fTrackres.at(i).SetZ(distance.Z());
	
	//std::cout<<"[FITTER] chi2 step: "<<chisq<<std::endl;
      }
    }
    else { //2D fit
      TVector2 refp[2];
      for (uint i=0; i<2; i++) refp[i].Set(par[1-fFitMode%10+2*i],fRefZPoints[i]); // reference points of the straight line
      TVector2 lambda = refp[1];
      lambda -= refp[0];
      lambda *= (1./lambda.Mod()); // direction of the track
      
      
      for (uint i= 0; i<fTrackmmt.size(); i++) {
	TVector2 point(fTrackmmt.at(i)[1-fFitMode%10],fTrackmmt.at(i)[2]);
	
	//	std::cout<<"[FITTER] point"<<i<<" : ["<<point.X()<<" , "<<point.Y()<<"]"<<std::endl;

	if (fFitMode/10) {// fit DZ
	  if (fTrackBdIds.at(i)>=0 && fTrackBdIds.at(i)<8)       point.SetY(point.Y() - par[4]);
	  else if (fTrackBdIds.at(i)>=8 && fTrackBdIds.at(i)<16) point.SetY(point.Y() + par[4]);
	}
	
	TVector2 distance = point;
	distance -= refp[0];
	double projection = distance*lambda;
	TVector2 parallelDist = lambda;
	parallelDist *= projection;
	
	distance -= parallelDist ; // ortogonal distance point to line                  

	if(1-fFitMode%10 == 0) { //X view
	  fTrackres.at(i).SetX(distance.X());
	  fTrackres.at(i).SetY(0.);
	}
	else {                   //Y view
	  fTrackres.at(i).SetX(0.);
	  fTrackres.at(i).SetY(distance.X());
	}
	fTrackres.at(i).SetZ(distance.Y());
      }
    }
  }
  
  int GetResVectorLenght() {return (int) fTrackres.size();}
  TVector3 GetResidual(int i) {if(i<0 && i>(int) fTrackres.size()) return {-999999,-999999,-999999}; return fTrackres.at(i);}
  
  
private:
  Double_t fRefZPoints[2];
  Double_t fDZ;
  std::vector<TVector3> fTrackmmt; // track position measurements
  std::vector<TVector3> fTrackerr; // track position expected errors
  std::vector<int> fTrackBdIds; // track Board ID number, -1 for additional point
  std::vector<TVector3> fTrackres;

  int fFitMode;
};
#endif
