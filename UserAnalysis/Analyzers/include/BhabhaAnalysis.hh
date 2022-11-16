#ifndef BhabhaAnalysis_h
#define BhabhaAnalysis_h 1

#include <TRandom3.h>
#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"

class VetoEndPoint;

class BhabhaAnalysis {

public:

  BhabhaAnalysis(TString c = "config/UserAnalysis.conf", Int_t v = 0);
  ~BhabhaAnalysis();

  Bool_t Init(PadmeAnalysisEvent*);
  Bool_t Process();
  Bool_t Finalize();

  
private:

  Bool_t InitHistos();

  Int_t fVerbose;

  PadmeAnalysisEvent* fEvent;

  utl::ConfigParser* fCfgParser;

  HistoSvc* fHS;

  Bool_t isMC;

  TMCVertex* mcVtx;

  Int_t fNPoT; 

  TMCParticle* mcOutPart;

  TVector3* VertexPos = new TVector3();

  VetoEndPoint * fVetoEndPoint;
  int NSwum=0;
  int nswimandtruthpveto=0;
  int nswimandtrutheveto=0;
 
  
};
#endif
