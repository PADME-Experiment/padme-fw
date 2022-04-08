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

  Bool_t CheckThetaAngle(double Ei, double Thetai);
  double GetCOG(std::vector<double> Ei,std::vector<double> Posi);
  double GetVertex(std::vector<double> Ei,std::vector<double> PosX,std::vector<double> PosY);

  Int_t fVerbose;

  PadmeAnalysisEvent* fEvent;

  utl::ConfigParser* fCfgParser;

  HistoSvc* fHS;
  Int_t NGG;
  Double_t ETotECal;

  // GG standard Cuts
  Double_t MinECluster ;
  Double_t TMin        ;
  Double_t TMax        ;
  Double_t TWin        ;
  Double_t ClRadMin    ;
  Double_t ClRadMax    ;
  Double_t COGMax      ; 
  Double_t DistMax     ;

  std::vector<double> vEi;
  std::vector<double> vPosX;
  std::vector<double> vPosY;

  std::vector<double> EGoodCluster;
  std::vector<double> TGoodCluster;
  std::vector<double> PosXGoodCluster;
  std::vector<double> PosYGoodCluster;
  std::vector<double> AngleGoodCluster;

  std::vector<int> PairGClIndex1;  //index1 of the paired crystal in the good cluster structure  
  std::vector<int> PairGClIndex2;  //index2 of the paired crystal in the good cluster structure  
  std::vector<double> PairCOGX;
  std::vector<double> PairCOGY;
  std::vector<double> PairDist;
  std::vector<double> PairVertex;

};
#endif
