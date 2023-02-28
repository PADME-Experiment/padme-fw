#ifndef ETagDigitizer_h
#define ETagDigitizer_h 1

#include <Rtypes.h>

class PadmeVRecoConfig;
class ETagHit;

class ETagDigitizer
{

public:

  ETagDigitizer(PadmeVRecoConfig*);
  ~ETagDigitizer();

  void Init();

  Int_t GetVerbose() { return fVerbose; }
  void  SetVerbose(Int_t v) { fVerbose = v; }

  Bool_t BuildHits(TRawEvent*,vector<ETagHit*>&);

private:

  Bool_t CreateChannelMap();

  Int_t fVerbose;

  PadmeVRecoConfig* fETagConfig;

  Int_t fChannelMap[32][32];

};
#endif // ETagDigitizer_h
