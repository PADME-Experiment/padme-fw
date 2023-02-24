#ifndef ETagDigitizer_h
#define ETagDigitizer_h 1

#include <Rtypes.h>

class PadmeVRecoConfig;

class ETagDigitizer
{

public:

  ETagDigitizer(PadmeVRecoConfig*);
  ~ETagDigitizer();

  void Init();

  Int_t GetVerbose() { return fVerbose; }
  void  SetVerbose(Int_t v) { fVerbose = v; }

private:

  Int_t fVerbose;

  PadmeVRecoConfig* fETagConfig;

};
#endif // ETagDigitizer_h
