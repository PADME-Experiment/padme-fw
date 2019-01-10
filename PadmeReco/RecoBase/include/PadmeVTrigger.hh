#ifndef PADMEVTRIGGER_H
#define PADMEVTRIGGER_H

#include "TRecoVHit.hh"
#include "PadmeVRecoConfig.hh"
#include "TADCTrigger.hh"
#include <map>
#include <utility>


class PadmeVTrigger {
public:
  PadmeVTrigger();
  virtual  ~PadmeVTrigger();
  
  virtual void Init(PadmeVRecoConfig *cfg);
  double GetTriggerTime(int Board,int ich);
  double GetChannelTriggerTime(int Board,int ich);
  double GetFrequency(int Board,int ich);

  //  int GetTriggerType(int Board,int ich);
  //  int GetTimeStamp(){};
  void ProcessTrigger(int bID, TADCTrigger* tr);
  double CalcTime(Short_t * sample);
  void Clear(){ fTriggerTime.clear();fTriggerLength.clear();};
  double GetTimeBin(int ib,int ig);

  void SetTrigMask(UInt_t val) {fTrigMask = val;};
  int GetTrigMask() {return fTrigMask;};

private:
  int fTriggerNumber;
  int fFrequency;
  UInt_t fTrigMask;

  //   BoardID,GroupID -> Trigger Time
  //                   bID,gID
  std::map < std::pair<int,int>,double> fTriggerTime;
  std::map < std::pair<int,int>,double>   fSamplingFrequency;
  std::map < std::pair<int,int>,double>    fTriggerLength;


  // map<int,double> fTimeCalib;
  // map<int,double> fEnergyCalib;
  
};

#endif
