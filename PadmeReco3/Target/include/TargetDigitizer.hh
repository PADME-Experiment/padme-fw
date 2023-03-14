#ifndef TargetDigitizer_h
#define TargetDigitizer_h 1

#include <map>
#include <Rtypes.h>

class PadmeVRecoConfig;
class TargetHit;
//class TargetChannelDigitizer;

class TargetDigitizer
{

public:

  TargetDigitizer(PadmeVRecoConfig*);
  ~TargetDigitizer();

  Bool_t Init();

  Int_t GetVerbose() { return fVerbose; }
  void  SetVerbose(Int_t v) { fVerbose = v; }

  Bool_t BuildHits(TRawEvent*,vector<TargetHit*>&);

private:

  Bool_t CreateChannelMap();

  Int_t fVerbose;

  PadmeVRecoConfig* fTargetConfig;

  Int_t fChannelMap[32][32]; // Map for individual SiPMs

  std::map<Int_t,std::pair<UChar_t,UChar_t>> fMapChannel;

  //TargetChannelDigitizer* fTargetChannelDigitizer;

};
#endif // TargetDigitizer_h
