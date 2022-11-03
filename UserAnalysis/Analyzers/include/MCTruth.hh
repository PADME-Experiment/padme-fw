#ifndef MCTruth_h
#define MCTruth_h 1

#include <TRandom3.h>
#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"

class MCTruth 
{

protected:
  //  MCTruth();//??
  MCTruth(TString c = "config/UserAnalysis.conf", Int_t v = 0);
public:
  ~MCTruth();
  static MCTruth* GetInstance();//??


  Bool_t Init(PadmeAnalysisEvent*);
  Bool_t Process();
  Bool_t Finalize();
  Double_t GetBeamEnergy(){return fBeamEnergy;};

private:
  static MCTruth* fInstance; //??
  Bool_t InitHistos();
  Int_t fVerbose;

  Double_t AnalyzeG4_Brems(Int_t iVtx);
  Double_t AnalyzeG4_GG(Int_t iVtx);
  Double_t AnalyzeG4_EE(Int_t iVtx);

  Double_t AnalyzeCALCHEP_GG(Int_t iVtx);
  Double_t AnalyzeCALCHEP_BhaBha(Int_t iVtx);
  Double_t AnalyzeCALCHEP_GGDisplaced(Int_t iVtx);

  Double_t AnalyzeAP_SinglePhoton(Int_t iVtx);  // e+e- -> gA' process
  
  PadmeAnalysisEvent* fEvent;
  utl::ConfigParser* fCfgParser;
  HistoSvc* fHS;
  TMCVertex* mcVtx;

  Double_t fBeamEnergy;

};
#endif
