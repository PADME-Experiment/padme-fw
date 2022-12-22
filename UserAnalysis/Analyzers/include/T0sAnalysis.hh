#ifndef T0Analysis_h
#define T0Analysis_h 1

#include <TRandom3.h>
#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"

class VetoEndPoint;

class T0sAnalysis {

public:

  T0sAnalysis(TString c = "config/UserAnalysis.conf", Int_t v = 0);
  ~T0sAnalysis();

  Bool_t Init(PadmeAnalysisEvent*);
  Bool_t Process();
  Bool_t Finalize();

  
private:

  Bool_t InitHistos();

  Int_t fVerbose;

  PadmeAnalysisEvent* fEvent;

  utl::ConfigParser* fCfgParser;

  HistoSvc* fHS;

  char name[250];

  
  Bool_t isMC;

  TMCVertex* mcVtx;

  Int_t fNPoT; 

  TMCParticle* mcOutPart;

  TVector3* VertexPos = new TVector3();

  VetoEndPoint * fVetoEndPoint;

  TRandom3 *myRNG=new TRandom3(0);

};
#endif
