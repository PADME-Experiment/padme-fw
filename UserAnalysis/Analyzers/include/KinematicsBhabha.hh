#ifndef KinematicsBhabha_h
#define KinematicsBhabha_h 1

#include <TRandom3.h>
#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"
#include "PadmeVRecoConfig.hh"

class VetoEndPoint;

class KinematicsBhabha {

public:

  KinematicsBhabha(TString c = "config/UserAnalysis.conf", Int_t v = 0);
  ~KinematicsBhabha();

  Bool_t Init(PadmeAnalysisEvent*);
  Bool_t Process();
  Bool_t Finalize();

  
private:

  Bool_t InitHistos();

  Int_t fVerbose;

  PadmeAnalysisEvent* fEvent;

  utl::ConfigParser* fCfgParser;
  PadmeVRecoConfig* fConfig;

  HistoSvc* fHS;

  Bool_t fSwimBhabha;

  Bool_t isMC;

  TMCVertex* mcVtx;

  Int_t fNPoT; 

  TMCParticle* mcOutPart;

  //  TVector3* VertexPos = new TVector3();

  VetoEndPoint * fVetoEndPoint;
};
#endif
