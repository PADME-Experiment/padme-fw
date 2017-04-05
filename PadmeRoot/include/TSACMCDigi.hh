// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-07-21
//
// --------------------------------------------------------------
#ifndef TSACMCDigi_H
#define TSACMCDigi_H

#include "TMCVDigi.hh"

#define TSACMCDIGI_N_BINS 1024

class TSACMCDigi : public TMCVDigi
{

public:

  TSACMCDigi();
  ~TSACMCDigi();

  void Print(Option_t* option="") const;

  void ResetTHisto();
  void SetTHistoStart(Double_t v) { fTHistoStart = v; }
  void SetTHistoStep(Double_t v)  { fTHistoStep = v; }
  Double_t GetTHistoNBins() { return TSACMCDIGI_N_BINS; };
  Double_t GetTHistoStart() { return fTHistoStart; };
  Double_t GetTHistoStep()  { return fTHistoStep; };
  Double_t GetTHistoEnd()   { return fTHistoStart+fTHistoStep*TSACMCDIGI_N_BINS; };
  void     SetTHistoBin(Int_t i,Double_t e) { fTHisto[i] = e; }
  Double_t GetTHistoBin(Int_t i) { return fTHisto[i]; }

private:

  Double_t fTHistoStart;
  Double_t fTHistoStep;
  Double_t fTHisto[TSACMCDIGI_N_BINS];

  ClassDef(TSACMCDigi,1);
};
#endif
