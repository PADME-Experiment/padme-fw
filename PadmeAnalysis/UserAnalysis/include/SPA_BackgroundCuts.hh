#ifndef SPA_BackgroundCuts_h
#define SPA_BackgroundCuts_h

#include "PadmeVAnalyser.hh"
#include "TLorentzVector.h"
#include "TVector3.h"
#include <fstream>
#include <string>
class SPA_BackgroundCuts: public  PadmeVAnalyser {

  
private:
  Int_t nGG_scan[41*41*41]={0};
  std::string filecoord;
  std::ofstream fcoordout;

  
public:
  SPA_BackgroundCuts(){;};
  SPA_BackgroundCuts(const char *s,int valid, int verb);
  SPA_BackgroundCuts(const char *s);
  ~SPA_BackgroundCuts(){;};

  virtual Bool_t InitHistos();
  virtual double ComputeRadius(Double_t PosX1,Double_t PosY1);
  virtual double ComputeInvariantMass(Double_t E1, Double_t E2, Double_t PosX1, Double_t PosX2, Double_t PosY1, Double_t PosY2);
  virtual double ComputeMissingMass(double BeamEnergy, Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma);
  virtual double ComputeMissingMomentum(double BeamEnergy, Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma);
  virtual TVector3 ComputeMissingMomentumVector(double BeamEnergy, Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma);
  virtual double ComputeTheta(Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma);
  virtual double ComputePhi(Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma);
  virtual double ComputeCOG(Double_t E_gamma1,Double_t Pos1,Double_t E_gamma2,Double_t Pos2);
  virtual TLorentzVector ComputeCMMomentum(Double_t xt, Double_t yt, Double_t zt,Double_t PosX, Double_t PosY, Double_t PosZ);
  virtual int ComputeSector(Double_t x, Double_t y);
  virtual double CalibrateEnergy(Double_t E_gamma, Double_t x, Double_t y);
  virtual double ComputeExpectedEnergy(Double_t PosX, Double_t PosY);
  virtual Bool_t Process();  
  virtual Bool_t Finalize();


  
};


#endif
