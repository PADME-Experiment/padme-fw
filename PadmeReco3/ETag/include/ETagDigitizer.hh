#ifndef ETagDigitizer_h
#define ETagDigitizer_h 1

#include <map>
#include <Rtypes.h>

class PadmeVRecoConfig;
class ETagHit;
class ETagChannelDigitizer;

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

  Int_t fChannelMap[32][32]; // Map for individual SiPMs
  Int_t fCardMap[32][8];    // Map for SiPM cards (4 SiPMs each)

  std::map<Int_t,std::pair<UChar_t,UChar_t>> fMapChannel;
  std::map<Int_t,std::array<std::pair<UChar_t,UChar_t>,4>> fMapCard;

  ETagChannelDigitizer* fETagChannelDigitizer;

};
#endif // ETagDigitizer_h
