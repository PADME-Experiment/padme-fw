#ifndef SinglePhotonAnalysis_h
#define SinglePhotonAnalysis_h

#include "PadmeVAnalyser.hh"

class GeneralInfo;


class SinglePhotonAnalysis: public  PadmeVAnalyser {
public:
  SinglePhotonAnalysis(){;};
  SinglePhotonAnalysis(const char *s,int valid, int verb);
  SinglePhotonAnalysis(const char *s);
  ~SinglePhotonAnalysis(){;};
  

  virtual Bool_t InitHistos();
  virtual double ComputeInvariantMass(Double_t E1, Double_t E2, Double_t PosX1, Double_t PosX2, Double_t PosY1, Double_t PosY2);
  virtual double ComputeRadius(Double_t PosX1,Double_t PosY1);
  virtual double ComputeDistance(Double_t PosX1,Double_t PosY1, Double_t PosX2, Double_t PosY2);
  virtual double ComputeMissingMass(double BeamEnergy, Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma);
  virtual double ComputeTheta(Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma);
  virtual double ComputePhi(Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma);
  virtual double ComputeDPhi(Double_t PosX1, Double_t PosY1, Double_t PosZ1, Double_t E_gamma1,Double_t PosX2, Double_t PosY2, Double_t PosZ2, Double_t E_gamma2);
  virtual double ComputeCOG_GGG(Double_t E_gamma1, Double_t E_gamma2, Double_t E_gamma3, Double_t Pos1, Double_t Pos2, Double_t Pos3);
  
  
  virtual Bool_t Process();  
  virtual Bool_t Finalize(){return true;}


private:

  
};


#endif
