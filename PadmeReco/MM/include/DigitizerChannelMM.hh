#include "ChannelVReco.hh"
#include "GlobalRecoConfigOptions.hh"
#include "utlConfigParser.hh"
#include "TMMChannel.hh"
#include "TMMBoard.hh"

typedef  GlobalRecoConfigOptions LocalRecoConfigOptions;

class DigitizerChannelMM : public ChannelVReco {
public:
  DigitizerChannelMM(){;};
  virtual ~DigitizerChannelMM();

  virtual void SetDigis(UShort_t n,Short_t* arr){fNSamples = n;fSamples = arr; };
  void Reconstruct(std::vector<TRecoVHit *> &hitArray, TMMBoard* bd, TMMChannel* ch);
  virtual void Reconstruct(std::vector<TRecoVHit *> &hitArray){;}
  virtual void Init(PadmeVRecoConfig *cfg){return ;}
  virtual void Init(GlobalRecoConfigOptions *gOptions, PadmeVRecoConfig *cfg);
  void PrintConfig();

  
private:
  UShort_t fNSamples;
  Short_t *fSamples;

  //Configuration variables
  double fADCUnitToCharge ;
  double fADCTimeBin      ;
  double fThresholdTruncatedMean;
  //mode variables
  GlobalRecoConfigOptions* fGlobalMode;
  LocalRecoConfigOptions*  fLocalMode;
  
};

