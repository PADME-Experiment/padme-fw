#ifndef SPA_acceptance_h
#define SPA_acceptance_h

#include "PadmeVAnalyser.hh"


class SPA_acceptance: public  PadmeVAnalyser {
public:
  SPA_acceptance(){;};
  SPA_acceptance(const char *s,int valid, int verb);
  SPA_acceptance(const char *s);
  ~SPA_acceptance(){;};

  virtual Bool_t InitHistos();
  virtual double ComputeTheta(Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma);
  virtual double ComputeRadius(Double_t PosX1,Double_t PosY1);
  virtual double ComputeMissingMass(double BeamEnergy, Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma);
  virtual Bool_t Process();  
  virtual Bool_t Finalize(){return true;}


private:
  
  
};


#endif
