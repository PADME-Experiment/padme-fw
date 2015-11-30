#ifndef ECALAnal_h
#define ECALAnal_h

#include "ECALHisto.hh"
#include "TRawEvent.hh"

class ECALAnal {

public:

  ECALAnal();
  ~ECALAnal();

  void SetEvent(TRawEvent* event) { fRawEvent = event; }

  void AnalyzeCharge();
  void AnalyzePosition();

private:

  ECALHisto* fECALHisto;

  TRawEvent* fRawEvent;

  Int_t fPlotEvent;

  Float_t fSample[TADCCHANNEL_NSAMPLES];
  Float_t fSampleReco[TADCCHANNEL_NSAMPLES];

  Float_t fQTotal1[TRAWEVENT_MAXADCBOARDS];
  Float_t fQTotal2[TRAWEVENT_MAXADCBOARDS];
  Float_t fQChannel[TRAWEVENT_MAXADCBOARDS][TADCBOARD_NCHANNELS];

};
#endif
