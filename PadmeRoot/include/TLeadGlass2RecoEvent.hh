#ifndef TLeadGlass2RecoEvent_H
#define TLeadGlass2RecoEvent_H

#include "TRecoVObject.hh"

class TLeadGlass2RecoEvent : public TRecoVObject {

public:

  TLeadGlass2RecoEvent(); 
  ~TLeadGlass2RecoEvent();

  void Clear();

  void SetPedestal(Double_t c) { fLGPedestal = c; }
  Double_t GetPedestal(UChar_t lg) { return fLGPedestal; }

  void SetPedestalRMS(Double_t c) { fLGPedestalRMS = c; }
  Double_t GetPedestalRMS(UChar_t lg) { return fLGPedestalRMS; }

  void SetPedestalV2(Double_t c) { fLGPedestal_v2 = c; }
  Double_t GetPedestalV2(UChar_t lg) { return fLGPedestal_v2; }

  void SetPedestalRMSV2(Double_t c) { fLGPedestalRMS_v2 = c; }
  Double_t GetPedestalRMSV2(UChar_t lg) { return fLGPedestalRMS_v2; }

  void SetStartIndexCell(Int_t i) { fLGStartIndexCell = i; }
  Int_t GetStartIndexCell(UChar_t lg) { return fLGStartIndexCell; }

  void SetTotalCharge(Double_t c) { fLGTotalCharge = c; }
  Double_t GetTotalCharge(UChar_t lg) { return fLGTotalCharge; }  
  
  void SetTotalChargeV2(Double_t c) { fLGTotalCharge_v2= c; }
  Double_t GetTotalChargeV2(UChar_t lg) { return fLGTotalCharge_v2; }  
  
  void SetTotalChargeWPed(Double_t c) { fLGTotalChargeWPed = c; }
  Double_t GetTotalChargeWPed(UChar_t lg) { return fLGTotalChargeWPed; }

  void SetTotalEnergy(Double_t e) { fLGTotalEnergy = e; }
  Double_t GetTotalEnergy(UChar_t lg) { return fLGTotalEnergy; }

  void SetNPoTs(Double_t p) { fLGNPoTs = p; }
  Double_t GetNPoTs() { return fLGNPoTs; }

  void SetBunchLength(Double_t b) { fLGBunchLength = b; }
  Double_t GetBunchLength() { return fLGBunchLength; }

  void SetBunchBBQ(Double_t b) { fLGBunchBBQ = b; }
  Double_t GetBunchBBQ() { return fLGBunchBBQ; }

private:
  
  Double_t fLGPedestal;
  Double_t fLGPedestalRMS;
  Double_t fLGPedestal_v2;
  Double_t fLGPedestalRMS_v2;

  Int_t fLGStartIndexCell;

  Double_t fLGTotalCharge;
  Double_t fLGTotalCharge_v2;
  Double_t fLGTotalChargeWPed;
  Double_t fLGTotalEnergy;
  Double_t fLGNPoTs;
  Double_t fLGBunchLength;
  Double_t fLGBunchBBQ;

  ClassDef(TLeadGlass2RecoEvent,1);
};
#endif
