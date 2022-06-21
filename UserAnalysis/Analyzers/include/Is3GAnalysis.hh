#ifndef Is3GAnalysis_h
#define Is3GAnalysis_h 1

#include <TRandom3.h>
#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"

class Is3GAnalysis {

public:

  Is3GAnalysis(TString c = "config/UserAnalysis.conf", Int_t v = 0);
  ~Is3GAnalysis();

  Bool_t Init(PadmeAnalysisEvent*);
  Bool_t Process();
  Bool_t Finalize();

  double GetETotECal(){return ETotECal;};

  
private:

  Bool_t InitHistos();

  double GetCOG(std::vector<double> Ei,std::vector<double> Posi);

  Int_t fVerbose;

  PadmeAnalysisEvent* fEvent;

  utl::ConfigParser* fCfgParser;

  HistoSvc* fHS;
  Int_t N3G;
  Double_t ETotECal;
};
#endif
