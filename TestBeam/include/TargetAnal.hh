#ifndef TargetAnal_h
#define TargetAnal_h

#include "TargetHisto.hh"
#include "TRawEvent.hh"

class TargetAnal {

public:

  TargetAnal();
  ~TargetAnal();

  void SetEvent(TRawEvent* event) { fRawEvent = event; }

  void AnalyzeCharge();
  void AnalyzePosition();

private:

  TargetHisto* fTargetHisto;

  TRawEvent* fRawEvent;

  Int_t fPlotEvent;

  //Float_t fSampleIndex[TADCCHANNEL_NSAMPLES];
  Float_t fSample[TADCCHANNEL_NSAMPLES];
  Float_t fSampleReco[TADCCHANNEL_NSAMPLES];

  Float_t fQTotal[TADCBOARD_NCHANNELS];
  Float_t fQChannel[TRAWEVENT_MAXADCBOARDS][TADCBOARD_NCHANNELS];

};
#endif
