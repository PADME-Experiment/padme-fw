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
  UShort_t fNPedSamples;

  UShort_t fEventCounter;
  UShort_t fPrintedSignalCounter;

  //Configuration variables
  Int_t fSignalWidth;
  // Int_t fPreSamples;
  // Int_t fPostSamples;
  Int_t fIntBegin;
  Int_t fIntEnd;
  Int_t fPedOffset; 
  Int_t fPedMaxNSamples;
  UShort_t fNEventsToPrint;
  Double_t fTimeBin;
  Double_t fVoltageBin;
  Double_t fImpedance;
  Int_t fSaturationAmpSample;

  // Histograms
  TH1D* hTargetNEventsToPrint;
  TH1D* hTargetDigitizer;
  TH1D* hXMaxChannels;
  TH1D* hYMaxChannels;
  TH1D* hXMaxAmplitudes;
  TH1D* hYMaxAmplitudes;
  TH1D* hXNonSatMaxChannels;
  TH1D* hYNonSatMaxChannels;
  TH1D* hXNonSatMaxAmplitudes;
  TH1D* hYNonSatMaxAmplitudes;
  TH1D* hCh2Ch3ChargeIntegral;
  TH1D* hCh24Ch23ChargeIntegral;
  TH1D* hCh30Ch24ChargeIntegral;
  TH1D* hCh30Ch24Ch31ChargeIntegral;
  TH2D* hCh24vsCh30ChargeIntegral;
  std::vector<TH1D*> hTargetSignals;
  std::vector<TH1D*> hChargeIntegrals;
  std::vector<TH1D*> hNoBeamIntegrals;
  std::vector<TH1D*> hChargePerEvent;

  //Define X strip channels and Y strip channels
  Double_t fXChannels[16] = {8,7,9,6,10,5,11,4,12,3,13,2,14,1,15,0};
  Double_t fYChannels[16] = {27,28,26,29,25,30,24,31,23,16,22,17,21,18,20,19};

};
#endif // TargetDigitizer_h
