#ifndef ECalChannelDigitizer_h
#define ECalChannelDigitizer_h 1

#define ECALCHANNELDIGITIZER_TEMPLATE_MAXBINS 1024

#include <Rtypes.h>

class TF1;
class TSpline5;

class PadmeVRecoConfig;
class TrigTimeSvc;
class HistoSvc;
class ECalHit;

class ECalChannelDigitizer
{

public:

  ECalChannelDigitizer(PadmeVRecoConfig*);
  ~ECalChannelDigitizer();

  Bool_t Init();

  Int_t GetVerbose() { return fVerbose; }
  void  SetVerbose(Int_t v) { fVerbose = v; }

  void SetRunNumber(Int_t r) { fRunNumber = r; }
  void SetEventNumber(Int_t e) { fEventNumber = e; }

  // (boardId,channelId,ECalchannel,StartIndexCell,samples,hits)
  Bool_t DigitizeChannel(UChar_t,UChar_t,Int_t,UShort_t,Short_t*,vector<ECalHit*>&);

private:

  Int_t fVerbose;

  PadmeVRecoConfig* fECalConfig;

  TrigTimeSvc* fTrigTimeSvc;
  HistoSvc* fHistoSvc;

  Int_t fGoodSamples;
  Int_t fPedestalSamples;
  Double_t fPedestalRMSThreshold;
  Double_t fZeroSupRMSThreshold;

  Int_t fRunNumber;
  Int_t fEventNumber;

  TString fTemplateFileName;
  Int_t fTemplateNBins;
  Int_t fTemplateMaxBin;
  Double_t fTemplateMaxVal;
  Double_t fTemplate[ECALCHANNELDIGITIZER_TEMPLATE_MAXBINS];
  Double_t fTemplateCFTime;
  Double_t fTemplateCFShift;

  TSpline5* fTemplateSpline;
  TF1* fTemplateFunction;
  TF1* fTempFitFunction;

  Double_t fCFRatio;
  Int_t fCFShift;
  Double_t fCFThreshold;

};
#endif // ECalChannelDigitizer_h
