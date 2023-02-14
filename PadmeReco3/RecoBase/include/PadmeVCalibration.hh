#ifndef PADMEVCALIBRATION_H
#define PADMEVCALIBRATION_H

//#include "TRecoVHit.hh"
#include "PadmeVRecoConfig.hh"
//#include <map>
//#include <vector>

class RecoVChannelID;
class TRecoVHit;
class TRecoVCluster;

class PadmeVCalibration {
public:
  PadmeVCalibration();
  virtual  ~PadmeVCalibration();
  
  virtual void Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr);
  virtual void InitCommonT0(PadmeVRecoConfig *cfg)         ;
  virtual void InitCommonEnergyScale(PadmeVRecoConfig *cfg);
  double GetCommonT0(){return fCommonT0;}
  double GetCommonT0MC(){return fCommonT0MC;}
  double GetCommonEnergyScale(){return fCommonEnergyScale;}
  void SetCommonT0(double x)   {fCommonT0=x;}
  void SetCommonT0MC(double x) {fCommonT0MC=x;}
  void SetCommonEnergyScale(double x){fCommonEnergyScale=x;}


  double GetRelChannelT0(int ich){
    if ( fTimeCalibMap->find(ich) != fTimeCalibMap->end() )
      return fTimeCalibMap->find(ich)->second;
    else
      return -1e23;
  }

  double GetRelChannelT0MC(int ich){
    if ( fTimeCalibMapMC->find(ich) != fTimeCalibMapMC->end() )
      return fTimeCalibMapMC->find(ich)->second;
    else
      return -1e23;
  }
  double GetRelChannelEnergyResponse(int ich){
    if ( fEnergyCalibMap->find(ich) != fEnergyCalibMap->end() )
      return fEnergyCalibMap->find(ich)->second;
    else
      return -1e23;
  }
  double GetChannelT0(int ich)            { return GetRelChannelT0(ich) + GetCommonT0(); }
  double GetChannelEnergyResponse(int ich){ return GetRelChannelEnergyResponse(ich)*GetCommonEnergyScale(); }
  double GetChannelT0MC(int ich)          { return GetRelChannelT0MC(ich)+ GetCommonT0MC() ; }

  virtual void PerformCalibration(std::vector<TRecoVHit *> &hitArray);
  virtual void PerformCalibration(std::vector<TRecoVCluster*> &clusArray){;}
  virtual void PerformMCCalibration(std::vector<TRecoVHit *> &hitArray);
  virtual void PerformMCCalibration(std::vector<TRecoVCluster*> &clusArray){;}
  
  
protected:
  int fNChannels;
  double fCommonT0;
  double fCommonT0MC;
  double fCommonEnergyScale;
  std::map<int,double> *fTimeCalibMap;
  std::map<int,double> *fEnergyCalibMap;
  std::map<int,double> *fTimeCalibMapMC;
  
};

#endif
