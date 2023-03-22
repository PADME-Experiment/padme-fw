#ifndef ECalDigitizer_h
#define ECalDigitizer_h 1

#include <map>
#include <Rtypes.h>

class PadmeVRecoConfig;
class ECalHit;
class HistoSvc;
class ECalChannelDigitizer;

class ECalDigitizer
{

public:

  ECalDigitizer(PadmeVRecoConfig*);
  ~ECalDigitizer();

  Bool_t Init();

  Int_t GetVerbose() { return fVerbose; }
  void  SetVerbose(Int_t v) { fVerbose = v; }

  Bool_t BuildHits(TRawEvent*,vector<ECalHit*>&);

private:

  Bool_t CreateChannelMap();

  Int_t fVerbose;

  PadmeVRecoConfig* fECalConfig;

  HistoSvc* fHistoSvc;

  Int_t fChannelMap[32][32]; // Map for individual SiPMs
  Int_t fCardMap[32][8];    // Map for SiPM cards (4 SiPMs each)

  std::map<Int_t,std::pair<UChar_t,UChar_t>> fMapChannel;
  std::map<Int_t,std::array<std::pair<UChar_t,UChar_t>,4>> fMapCard;

  ECalChannelDigitizer* fECalChannelDigitizer;

};
#endif // ECalDigitizer_h
