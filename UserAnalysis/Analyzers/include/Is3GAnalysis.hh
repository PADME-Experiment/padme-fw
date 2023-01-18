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

  std::vector<double> EClusterPair;
  std::vector<double> TClusterPair;
  std::vector<double> PosXClusterPair;
  std::vector<double> PosYClusterPair;

  std::vector<double> Ei;
  std::vector<double> PosX;
  std::vector<double> PosY;

  std::vector<double> EGoodCluster;
  std::vector<double> TGoodCluster;
  std::vector<double> PosXGoodCluster;
  std::vector<double> PosYGoodCluster;

  std::vector<double> ETotPair;  
  std::vector<double> TDiffPair; 
  std::vector<double> ETotTris;    
  
  // Standard cuts list
  Double_t MinECluster;
  Double_t TMin;
  Double_t TMax;
  Double_t TWin;
  Double_t ClRadMin;
  Double_t ClRadMax;
  Double_t COGMax  ;
  Double_t DistMax ;
};
#endif
