#ifndef TLeadGlassRecoEvent_H
#define TLeadGlassRecoEvent_H

#include "TRecoVObject.hh"

#define N_LEADGLASS 2

class TLeadGlassRecoEvent : public TRecoVObject {

public:

  TLeadGlassRecoEvent(); 
  ~TLeadGlassRecoEvent();

  void Clear();

  void SetPedestal(UChar_t lg, Double_t c) { fLGPedestal[lg] = c; }
  Double_t GetPedestal(UChar_t lg) { return fLGPedestal[lg]; }

  void SetPedestalRMS(UChar_t lg, Double_t c) { fLGPedestalRMS[lg] = c; }
  Double_t GetPedestalRMS(UChar_t lg) { return fLGPedestalRMS[lg]; }

  void SetTotalCharge(UChar_t lg, Double_t c) { fLGTotalCharge[lg] = c; }
  Double_t GetTotalCharge(UChar_t lg) { return fLGTotalCharge[lg]; }

  void SetTotalEnergy(UChar_t lg, Double_t e) { fLGTotalEnergy[lg] = e; }
  Double_t GetTotalEnergy(UChar_t lg) { return fLGTotalEnergy[lg]; }

  void SetNPoTs(Double_t p) { fLGNPoTs = p; }
  Double_t GetNPoTs() { return fLGNPoTs; }

  void SetBunchLength(Double_t b) { fLGBunchLength = b; }
  Double_t GetBunchLength() { return fLGBunchLength; }

  void SetBunchBBQ(Double_t b) { fLGBunchBBQ = b; }
  Double_t GetBunchBBQ() { return fLGBunchBBQ; }

private:
  
  Double_t fLGPedestal[N_LEADGLASS];
  Double_t fLGPedestalRMS[N_LEADGLASS];

  Double_t fLGTotalCharge[N_LEADGLASS];
  Double_t fLGTotalEnergy[N_LEADGLASS];
  Double_t fLGNPoTs;
  Double_t fLGBunchLength;
  Double_t fLGBunchBBQ;

  ClassDef(TLeadGlassRecoEvent,1);
};
#endif
