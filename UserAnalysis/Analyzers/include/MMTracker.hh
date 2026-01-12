#ifndef MMTracker_h
#define MMTracker_h 1

#include "TRecoVHit.hh"
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TFitter.h"
#include <Fit/Fitter.h>
//
// fFitMode = 0/1 -> Y/X view fits; 2 -> 3d fit
//

class MyFcn {
public:
  MyFcn(){fFitMode = 2;}
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
  void setFitMode(int fitMode){fFitMode = fitMode;}

  double operator()(const double *par) {

    Double_t chisq = 0;
    Double_t delta;

    
    if (fFitMode%10 == 2){
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
      }
    } else {

      TVector2 refp[2];
      for (uint i=0; i<2; i++) refp[i].Set(par[1-fFitMode%10+2*i],fRefZPoints[i]); // reference points of the straight line
      TVector2 lambda = refp[1];
      lambda -= refp[0];
      lambda *= (1./lambda.Mod()); // direction of the track

    // track is \vec{r} = \vec{p0} + \vec{lambda} * t, p0 = barycenter of points
    // point of CDA on the track line: \vec{p0} + ( (\vec{point}_i - \vec{p0}) dot \vec{lambda} ) \vec{lambda}                                                                                                    
    // vector ortogonal to the line at CDA: (\vec{point}_i - \vec{p0}) - ( (\vec{point}_i - \vec{p0}) dot \vec{lambda} ) \vec{lambda}

      for (uint i= 0; i<fTrackmmt.size(); i++) {
	TVector2 point(fTrackmmt.at(i)[1-fFitMode%10],fTrackmmt.at(i)[2]);
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
      }
      //      std::cout << " chi2 = " << chisq;
    }

    return chisq; // Npoints-4 degrees of freedom
  }
private:
  Double_t fRefZPoints[2];
  std::vector<TVector3> fTrackmmt; // track position measurements
  std::vector<TVector3> fTrackerr; // track position expected errors
  std::vector<int> fTrackBdIds; // track Board ID number, -1 for additional point
  int fFitMode;
};


class MMTracker {


public:
  MMTracker(int fitmode);// pass the fit mode at the constructor level: 0, 1, 2 for Y view, X view, 3d view fits, fitmode+=10 for the DZ fit
  ~MMTracker();
  //  static MMTracker* GetInstance();

  void AddHit(TRecoVHit* hit){hitArray.push_back(hit);} // Append given hit to hits array
  TRecoVHit* Hit(Int_t id){return hitArray.at(id);}   // Return pointer to hit at given index in hits array
  TRecoVHit* LastHit(){return hitArray.back();}    // Return pointer to last hit in hits array
  //  void RemoveHit(Int_t id){hitArray.erase(id);}      // Remove hit at given index and compress hits array
  void Clear(){hitArray.clear(); fPositions.clear(); fErrors.clear(); fBoardIds.clear();} // Clear hit array 
  Int_t GetNHits() { return hitArray.size(); };

  void InitFitter();
  void InitFit(double x, double y, double z);// beyond the hits, also add additional point to the fit
  void InitFit();
  bool MakeFit();

  // fit
  double GetFitResults(int id) {return fParRes[id];}
  double GetFitChi2() {return fMinFCN;}

  int GetFitMode(){return fFitMode;}

  // getreferencepoint
  // getfitresults errors
  // getfitresults chi2

  
private:
  static MMTracker* fInstance;
  void setPoints();
  void setAdditionalPoint(double x, double y, double z); // used to insert the IP as an additional measurement
  std::vector<TRecoVHit*> hitArray;
  std::vector<TVector3> fPositions;
  std::vector<TVector3> fErrors;
  std::vector<int> fBoardIds;
  MyFcn fMyFcn;
  ROOT::Fit::Fitter fFitter;  
  double fMinFCN;
  double* fParRes;
  int fFitMode;
};



#endif
