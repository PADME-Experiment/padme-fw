#include "ChannelVReco.hh"
#include "GlobalRecoConfigOptions.hh"
#include "utlConfigParser.hh"
#include "TMMChannel.hh"
#include "TMMBoard.hh"
#include "TF1.h"
#include "TGraphErrors.h"

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
  Short_t fNSamples;
  Short_t *fSamples;

  //Configuration variables
  double fADCUnitToCharge ;
  double fAPVTimeBin      ;
  double fThresholdTruncatedMean;
  double fTimeTau; // tau value of the signal A t/tau exp(-t/tau)
  Short_t fTimeWindowSamplesLow; // extent of the window in which the signal is fit
  Short_t fTimeWindowSamplesHigh; // extent of the window in which the signal is fit
  Short_t fHitChargeThreshold; // minimum charge max in ADC counts for hit reconstruction
  Short_t fHitChargeSmoothnessThreshold; // maximum charge difference between max sample and adjacent sample(s) in ADC counts for hit reconstruction
  double fHitTimeLow  ;
  double fHitTimeHigh ;
  double fHitTimeMinimum ;
  double fHitChargeMaxForPChi2Cut ;
  double fHitPChi2Cut ;
  
  int fCounters;
  //mode variables
  GlobalRecoConfigOptions* fGlobalMode;
  LocalRecoConfigOptions*  fLocalMode;
  // private methods
  bool evaluateSig(Short_t sampleMaxId, Short_t maxQ, double* sigAmplitude, double* sigTimePeak); // id of the max sample, max sample value
  TF1* fSignalShape;
  static Double_t fitSignalShape(Double_t *x, Double_t *par);
};

