#ifndef ECalChannelDigitizer_h
#define ECalChannelDigitizer_h 1

#include <Rtypes.h>

class PadmeVRecoConfig;
class TrigTimeSvc;
class HistoSvc;
class ECalHit;

class ECalChannelDigitizer
{

public:

  ECalChannelDigitizer(PadmeVRecoConfig*);
  ~ECalChannelDigitizer();

  void Init();

  Int_t GetVerbose() { return fVerbose; }
  void  SetVerbose(Int_t v) { fVerbose = v; }

  void SetRunNumber(Int_t r) { fRunNumber = r; }
  void SetEventNumber(Int_t e) { fEventNumber = e; }

  Bool_t DigitizeChannel(UChar_t,UChar_t,Int_t,Short_t*,vector<ECalHit*>&);

private:

  Int_t fVerbose;

  PadmeVRecoConfig* fECalConfig;

  TrigTimeSvc* fTrigTimeSvc;
  HistoSvc* fHistoSvc;

  Int_t fGoodSamples;
  Int_t fPedestalSamples;
  Double_t fZeroSupRMSThreshold;

  Int_t fRunNumber;
  Int_t fEventNumber;

};
#endif // ECalChannelDigitizer_h
