#ifndef ETagChannelDigitizer_h
#define ETagChannelDigitizer_h 1

#include <Rtypes.h>

class PadmeVRecoConfig;
class ETagHit;

class ETagChannelDigitizer
{

public:

  ETagChannelDigitizer(PadmeVRecoConfig*);
  ~ETagChannelDigitizer();

  void Init();

  Int_t GetVerbose() { return fVerbose; }
  void  SetVerbose(Int_t v) { fVerbose = v; }

  Bool_t DigitizeChannel(UChar_t,UChar_t,Int_t,Short_t*,vector<ETagHit*>&);
  Bool_t DigitizeCard(Int_t,Short_t*,vector<ETagHit*>&);

private:

  Int_t fVerbose;

  PadmeVRecoConfig* fETagConfig;

};
#endif // ETagChannelDigitizer_h
