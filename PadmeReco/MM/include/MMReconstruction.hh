// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2022-10-25
//
// --------------------------------------------------------------
#ifndef MMReconstruction_H
#define MMReconstruction_H

#include "PadmeVReconstruction.hh"

class MMReconstruction : public PadmeVReconstruction
{

public:

  MMReconstruction(TFile*, TString);
  ~MMReconstruction();

  void ProcessEvent(TRawEvent*, TMMRawEvent*);
  virtual void HistoInit();

  Bool_t MMFound()     { return fMMFound; }

  Double_t GetCharge() { return fMMCharge;    }

private:

  Bool_t TriggerToBeSkipped();
  void AnalyzeEvent(TRawEvent*);
  void AnalyzeChannel(Short_t*);
  void ComputeTotalCharge(Short_t*);
  void ComputeBunchLength(Short_t*);

  // Flag to signal if MM was found in this event
  Bool_t fMMFound;


  // Results of pedestal and total charge evaluation
  Double_t fMMCharge;   // Total charge 


  // Histograms
  TH1D* fHMMTotCharge;

};
#endif
