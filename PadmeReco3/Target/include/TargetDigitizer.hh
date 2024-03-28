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
  void SetAbsSignals();
  Double_t CalcPedestal();

  //TargetChannelDigitizer* fTargetChannelDigitizer;

  UShort_t fNSamples;
  Short_t *fSamples;
  Double_t fPed;
  UShort_t fNPedSamples;

  UShort_t fEventCounter;

  //Configuration variables
  Int_t fSignalWidth;
  Int_t fPreSamples;
  Int_t fPostSamples;
  Int_t fPedOffset; 
  Int_t fPedMaxNSamples;
  UShort_t fNEventsToPrint;
  Double_t fTimeBin;
  Double_t fVoltageBin;
  Double_t fImpedance;

  // Histograms
  TH1D* hTargetDigitizer;
  TH1D* hTargetSignals[32];


};
#endif // TargetDigitizer_h
