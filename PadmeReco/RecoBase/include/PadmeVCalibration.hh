#ifndef PADMEVCALIBRATION_H
#define PADMEVCALIBRATION_H

#include "TRecoVHit.hh"
#include "PadmeVRecoConfig.hh"



class PadmeVCalibration {
public:
  PadmeVCalibration();
  virtual  ~PadmeVCalibration();
  
  virtual void Init(PadmeVRecoConfig *cfg);
  double GetChannelT0(int ich){return (ich>=0  && ich<fNChannels)?fTimeCalib[ich]:-1e23;};
  double GetChannelEnergyResponse(int ich){return (ich>=0  && ich<fNChannels)?fEnergyCalib[ich]:-1e23;};
  void PerformCalibration(std::vector<TRecoVHit *> &hitArray);
  
  
private:
  int fNChannels;
  // map<int,double> fTimeCalib;
  // map<int,double> fEnergyCalib;
  
  double *fTimeCalib;
  double *fEnergyCalib;
  
};

#endif
