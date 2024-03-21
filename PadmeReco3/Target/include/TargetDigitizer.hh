#ifndef TargetDigitizer_h
#define TargetDigitizer_h 1

#include <map>
#include <Rtypes.h>

class TH1D;

class HistoSvc;
class RunConditionSvc;
class RunConfigurationSvc;

class PadmeVRecoConfig;
//class TargetHit;   //commented by Beth 21/3/24: The concept of "hits" in the target doesn't really have any meaning. It would be better to compute the charge per strip
//class TargetChannelDigitizer;
class TargetStripCharge;

class TargetDigitizer
{

public:

  TargetDigitizer(PadmeVRecoConfig*);
  ~TargetDigitizer();

  Bool_t Init();

  Int_t GetVerbose() { return fVerbose; }
  void  SetVerbose(Int_t v) { fVerbose = v; }
  
  //commented by Beth 21/3/24: The concept of "hits" in the target doesn't really have any meaning. It would be better to compute the charge per strip
  //  Bool_t BuildHits(TRawEvent*,vector<TargetHit*>&);
  void ComputeChargePerStrip(TRawEvent*,vector<TargetStripCharge*>&);

private:

  HistoSvc* fHistoSvc;
  RunConditionSvc* fRunConditionSvc;
  RunConfigurationSvc* fRunConfigurationSvc;

  Bool_t CreateChannelMap();

  Int_t fVerbose;

  PadmeVRecoConfig* fTargetConfig;

  Int_t fChannelMap[32][32]; // Map for individual SiPMs

  std::map<Int_t,std::pair<UChar_t,UChar_t>> fMapChannel;

  //TargetChannelDigitizer* fTargetChannelDigitizer;

  // Histograms
  TH1D* hTargetDigitizer;


};
#endif // TargetDigitizer_h
