#ifndef TLeadGlass2RecoEvent_H
#define TLeadGlass2RecoEvent_H

#include "TRecoVObject.hh"

class TLeadGlass2RecoEvent : public TRecoVObject {

public:

  TLeadGlass2RecoEvent(); 
  ~TLeadGlass2RecoEvent();

  void Clear();

  void SetPedestal(Double_t c) { fLGPedestal = c; }
  Double_t GetPedestal() { return fLGPedestal; }

  void SetPedestalRMS(Double_t c) { fLGPedestalRMS = c; }
  Double_t GetPedestalRMS() { return fLGPedestalRMS; }
  
  void SetStartIndexCell(Int_t i) { fLGStartIndexCell = i; }
  Int_t GetStartIndexCell() { return fLGStartIndexCell; }

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
  Int_t fLGStartIndexCell;

  Double_t fLGTotalCharge;
  Double_t fLGTotalEnergy;
  Double_t fLGNPoTs;
  Double_t fLGBunchLength;
  Double_t fLGBunchBBQ;

  ClassDef(TLeadGlass2RecoEvent,1);
};
#endif
