// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-17
//
// --------------------------------------------------------------
#ifndef TSACMCHit_H
#define TSACMCHit_H

#include "TMCVHit.hh"

//#include "TH1D.h"

#define TSACMCHIT_N_BINS 400

class TSACMCHit : public TMCVHit
{

public:

  TSACMCHit();
  ~TSACMCHit();

  void ResetTHisto();
  void PrintTHisto();
  void SetTHistoStart(Double_t v) { fTHistoStart = v; }
  void SetTHistoStep(Double_t v)  { fTHistoStep = v; }
  Double_t GetTHistoNBins() { return TSACMCHIT_N_BINS; };
  Double_t GetTHistoStart() { return fTHistoStart; };
  Double_t GetTHistoStep()  { return fTHistoStep; };
  Double_t GetTHistoEnd()   { return fTHistoStart+fTHistoStep*TSACMCHIT_N_BINS; };
  void     SetTHistoBin(Int_t i,Double_t e) { fTHisto[i] = e; }
  Double_t GetTHistoBin(Int_t i) { return fTHisto[i]; }
  //void CreateEnergyHisto(Int_t,Double_t,Double_t); // Nbin,Tstart,Tend
  //TH1D* GetEnergyHisto() { return fEnergyHisto; }
  //TH1D* GetEnergyHisto() { return &fEnergyHisto; }
  void     AddEnergyAtTime(Double_t,Double_t);
  Double_t GetEnergyAtTime(Double_t);

private:

  //TH1D* fEnergyHisto;
  //TH1D fEnergyHisto;
  Double_t fTHistoStart;
  Double_t fTHistoStep;
  Double_t fTHisto[TSACMCHIT_N_BINS];

  ClassDef(TSACMCHit,1);
};
#endif
