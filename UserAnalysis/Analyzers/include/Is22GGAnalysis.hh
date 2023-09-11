#ifndef Is22GGAnalysis_h
#define Is22GGAnalysis_h 1

#include <TRandom3.h>
#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"
#include "ECalCalib.hh"
#include "MCTruth.hh"
#include <map>
#include "TLorentzVector.h"

class Is22GGAnalysis {

public:

  Is22GGAnalysis(TString c = "config/UserAnalysis.conf", Int_t v = 0);
  ~Is22GGAnalysis();

  Bool_t Init(PadmeAnalysisEvent*);
  Bool_t Process();
  Bool_t Finalize();

  double GetETotECal(){return ETotECal;};
  
private:
  Bool_t InitHistos();

  Bool_t CheckThetaAngle(double Ei, double Thetai);
  double CompCOG(std::vector<double> Ei,std::vector<double> Posi);
  double GetVertex(std::vector<double> Ei,std::vector<double> PosX,std::vector<double> PosY);
  double ComputeInvariantMass(std::vector<double> Ei,std::vector<double> PosX,std::vector<double> PosY);

// trasform to rest frame classes
 TLorentzVector TransformToRestFrame(TLorentzVector& P4beam,TLorentzVector& P4Part);
//TLorentzVector FillTLorentzVector(Double_t XPart, Double_t YPart,Double_t E);

  TLorentzVector FillTLorentzVector(double XPart,double YPart,double E);
  Bool_t IsMCGG(double T1, double T2, double Ei,double Ej);

  Int_t fVerbose;
  Bool_t fisMC = false; 
  Double_t fBeamE;
  Double_t fSqrtS;
  Int_t fNRun;

  PadmeAnalysisEvent* fEvent;
  TMCVertex* mcVtx;

  utl::ConfigParser* fCfgParser;

  //Connect external class
  HistoSvc* fHS;
  ECalCalib* fECalCalib;
  MCTruth* fMCTruth;
  std::map<std::pair<int, int>, double> ECalHitMap;

  Int_t NGG;
  Int_t Neve;
  Int_t NGG_MC;

  //  Double_t ETotECal;
  //trend plots
  std::vector<double> vNPoT;
  std::vector<double> vRatio;
  std::vector<double> vETotECal;
  std::vector<double> vNEvt;
  std::vector<double> vETotOvNPot;

  Int_t Navg;
  Double_t NPoTAvg;
  Double_t ETotECal;
  Double_t EToTAvg;
  Double_t HitEToTAvg;
  
  // GG standard Cuts
  Double_t MinECluster;
  Double_t MaxECluster;
  Double_t TMin       ;
  Double_t TMax       ;
  Double_t TWin       ;
  Double_t ClRadMin   ;
  Double_t ClRadMax   ;
  Double_t COGMax     ; 
  Double_t DistMin    ;
  Int_t ChId;
  std::vector<double> vEi;
  std::vector<double> vPosX;
  std::vector<double> vPosY;

  std::vector<double> EGoodCluster;
  std::vector<double> TGoodCluster;
  std::vector<double> PosXGoodCluster;
  std::vector<double> PosYGoodCluster;
  std::vector<double> AngleGoodCluster;
  std::vector<int>    IsGGGoodCluster;

  std::vector<int> PairGClIndex1;  //index1 of the paired crystal in the good cluster structure  
  std::vector<int> PairGClIndex2;  //index2 of the paired crystal in the good cluster structure  
  std::vector<double> PairCOGX;
  std::vector<double> PairCOGY;
  std::vector<double> PairDist;
  std::vector<double> PairVertex;

};
#endif
