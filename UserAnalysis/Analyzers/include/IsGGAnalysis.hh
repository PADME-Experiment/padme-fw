#ifndef IsGGAnalysis_h
#define IsGGAnalysis_h 1

#include <TRandom3.h>
#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"

class IsGGAnalysis {

public:

  IsGGAnalysis(TString c = "config/UserAnalysis.conf", Int_t v = 0);
  ~IsGGAnalysis();

  Bool_t Init(PadmeAnalysisEvent*);
  Bool_t Process();
  Bool_t Finalize();

  double GetETotECal(){return ETotECal;};

  
private:

  Bool_t InitHistos();

  double GetCOG(std::vector<double> Ei,std::vector<double> Posi);
  double GetVertex(std::vector<double> Ei,std::vector<double> PosX,std::vector<double> PosY);

  Int_t fVerbose;

  PadmeAnalysisEvent* fEvent;

  utl::ConfigParser* fCfgParser;

  HistoSvc* fHS;
  Int_t NGG;
  Double_t ETotECal;
};
#endif
