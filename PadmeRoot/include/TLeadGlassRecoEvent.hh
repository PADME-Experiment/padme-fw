#ifndef TLeadGlassRecoEvent_H
#define TLeadGlassRecoEvent_H

#include "TRecoVObject.hh"

class TLeadGlassRecoEvent : public TRecoVObject {

public:

  TLeadGlassRecoEvent(); 
  ~TLeadGlassRecoEvent();

  void Clear();

  void SetPedestal(Double_t c) { fLGPedestal = c; }
  Double_t GetPedestal() { return fLGPedestal; }

  void SetPedestalRMS(Double_t c) { fLGPedestalRMS = c; }
  Double_t GetPedestalRMS() { return fLGPedestalRMS; }

  void SetTotalCharge(Double_t c) { fLGTotalCharge = c; }
  Double_t GetTotalCharge() { return fLGTotalCharge; }

  void SetTotalEnergy(Double_t e) { fLGTotalEnergy = e; }
  Double_t GetTotalEnergy() { return fLGTotalEnergy; }

  void SetNPoTs(Double_t p) { fLGNPoTs = p; }
  Double_t GetNPoTs() { return fLGNPoTs; }

  void SetBunchLength(Double_t b) { fLGBunchLength = b; }
  Double_t GetBunchLength() { return fLGBunchLength; }

  void SetBunchBBQ(Double_t b) { fLGBunchBBQ = b; }
  Double_t GetBunchBBQ() { return fLGBunchBBQ; }

private:
  
  Double_t fLGPedestal;
  Double_t fLGPedestalRMS;

  Double_t fLGTotalCharge;
  Double_t fLGTotalEnergy;
  Double_t fLGNPoTs;
  Double_t fLGBunchLength;
  Double_t fLGBunchBBQ;

  ClassDef(TLeadGlassRecoEvent,1);
};
#endif
