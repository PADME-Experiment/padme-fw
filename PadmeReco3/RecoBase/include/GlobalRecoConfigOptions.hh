#ifndef GLOBAL_RECOCONFIGOPTIONS_H
#define GLOBAL_RECOCONFIGOPTIONS_H
#include "Rtypes.h"


class GlobalRecoConfigOptions {

public:
  GlobalRecoConfigOptions(){fRunningMode=1;fGlobalDebugMode=0;}
  GlobalRecoConfigOptions(Int_t recoM, Int_t pedM, Int_t cosmicM, Int_t monitM, Int_t gdebugM);
  virtual  ~GlobalRecoConfigOptions(){;};


  void  SetRunningMode(Int_t recoM, Int_t pedM, Int_t cosmicM, Int_t monitM, Int_t gdebugM);
  Int_t GetRunningMode()     {return fRunningMode;}
  Int_t GetGlobalDebugMode() {return fGlobalDebugMode;}

  Int_t GetRecoBit();
  bool  IsRecoMode()      { return (bool)GetRecoBit();}

  Int_t GetPedestalBit();
  bool  IsPedestalMode() { return (bool)GetPedestalBit();}

  Int_t GetCosmicsBit();
  bool  IsCosmicsMode()    { return (bool)GetCosmicsBit();}

  Int_t GetMonitorBit();
  bool  IsMonitorMode()   { return (bool)GetMonitorBit();}

  Int_t GetGlobalDebugBit(){return fGlobalDebugMode;}
 

private: 
   Int_t fRunningMode;
   Int_t fGlobalDebugMode;

};
#endif
