#ifndef CHANNEL_VRECO_H
#define CHANNEL_VRECO_H

#include "TRecoVHit.hh"
#include <vector>
#include "PadmeVRecoConfig.hh"

class ChannelVReco {

public:
  ChannelVReco(){;};
  virtual  ~ChannelVReco(){;};
  
  virtual void SetDigis(UShort_t n,Short_t* arr) = 0;  
  virtual void Reconstruct(std::vector<TRecoVHit *> &hitArray)=0;
  virtual void Init(PadmeVRecoConfig *cfg) = 0;

  Bool_t fIsPed;
  Bool_t fIsReco;
  Bool_t fIsCosmic;
  Bool_t fIsMonitor;
  Int_t  fIsGlobalDebug;
  Int_t  fIsLocalDebug;

  void  SetIsPedOn()     { fIsPed = true; }
  void  SetIsPedOff()    { fIsPed = false; }
  bool  GetIsPed()       { return fIsPed;}
  
  void  SetIsRecoOn()    { fIsReco = true; }
  void  SetIsRecoOff()   { fIsReco = false; }
  bool  GetIsReco()      { return fIsReco;}
  
  void  SetIsCosmicOn()  { fIsCosmic = true; }
  void  SetIsCosmicOff() { fIsCosmic = false; }
  bool  GetIsCosmic()    { return fIsCosmic;}
  
  void  SetIsMonitorOn()  { fIsMonitor = true; }
  void  SetIsMonitorOff() { fIsMonitor = false; }
  bool  GetIsMonitor()    { return fIsMonitor;}
  
  void  SetIsGlobalDebug(Int_t value) { fIsGlobalDebug = value; }
  void  SetIsLocalDebug(Int_t value)  { fIsLocalDebug = value; }
  Int_t GetIsGlobalDebug()  { return fIsGlobalDebug; }

};
#endif
