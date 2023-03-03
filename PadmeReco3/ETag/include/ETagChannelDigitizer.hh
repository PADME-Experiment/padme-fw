#ifndef ETagChannelDigitizer_h
#define ETagChannelDigitizer_h 1

#include <Rtypes.h>

class PadmeVRecoConfig;
class TrigTimeSvc;
class HistoSvc;
class ETagHit;

class ETagChannelDigitizer
{

public:

  ETagChannelDigitizer(PadmeVRecoConfig*);
  ~ETagChannelDigitizer();

  void Init();

  Int_t GetVerbose() { return fVerbose; }
  void  SetVerbose(Int_t v) { fVerbose = v; }

  void SetRunNumber(Int_t r) { fRunNumber = r; }
  void SetEventNumber(Int_t e) { fEventNumber = e; }

  Bool_t DigitizeChannel(UChar_t,UChar_t,Int_t,Short_t*,vector<ETagHit*>&);
  Bool_t DigitizeCard(Int_t,Short_t*,vector<ETagHit*>&);

private:

  Int_t fVerbose;

  PadmeVRecoConfig* fETagConfig;

  TrigTimeSvc* fTrigTimeSvc;
  HistoSvc* fHistoSvc;

  Int_t fGoodSamples;
  Int_t fPedestalSamples;
  Double_t fZeroSupRMSThreshold;
  Double_t fHitThreshold;

  Int_t fRunNumber;
  Int_t fEventNumber;

};
#endif // ETagChannelDigitizer_h
