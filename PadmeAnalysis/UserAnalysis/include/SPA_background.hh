#ifndef SPA_background_h
#define SPA_background_h

#include "PadmeVAnalyser.hh"

class GeneralInfo;

class SPA_background: public  PadmeVAnalyser {
public:
  SPA_background(){;};
  SPA_background(const char *s,int valid, int verb);
  SPA_background(const char *s);
  ~SPA_background(){;};

  virtual Bool_t InitHistos();

  virtual double ComputeInvariantMass(Double_t E1, Double_t E2, Double_t PosX1, Double_t PosX2, Double_t PosY1, Double_t PosY2);
  virtual double ComputeRadius(Double_t PosX1,Double_t PosY1);
  virtual double ComputeDistance(Double_t PosX1,Double_t PosY1, Double_t PosX2, Double_t PosY2);
  virtual double ComputeCOG(Double_t E_gamma1, Double_t E_gamma2, Double_t Pos1, Double_t Pos2);
  virtual double ComputeMissingMass(double BeamEnergy, Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma);
  virtual double ComputeZfor0MissingMass(double BeamEnergy, Double_t PosX1, Double_t PosX2, Double_t PosY1, Double_t PosY2, Double_t E1, Double_t E2);
  virtual double ComputeTheta(Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma);
  virtual double ComputePhi(Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma);
  virtual Bool_t IsGG(TRecoVCluster* clu, TRecoVCluster* clu2);
  virtual Bool_t IsGGG(TRecoVCluster* clu, TRecoVCluster* clu2, TRecoVCluster* clu3);
  virtual Bool_t IsBrems(TRecoVCluster* clu, TRecoVCluster* clu2);
  virtual Bool_t Process();  
  virtual Bool_t Finalize(){return true;}


private:
  
  
};


#endif
