// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#ifndef ECalReconstruction_H
#define ECalReconstruction_H

#include "PadmeVReconstruction.hh"

class TRecoVCluster;


class ECalReconstruction : public PadmeVReconstruction
{

public:

  ECalReconstruction(TFile*, TString);
  ~ECalReconstruction();

  // void ParseConfFile(TString);
  // virtual void Init(PadmeVReconstruction*);
  //  virtual void ProcessEvent(TMCVEvent*,TMCEvent*);
  virtual void ProcessEvent(TRawEvent*);
  virtual void BuildClusters();
  void BuildSimpleECalClusters();
  void BuildECalIslandRadiusClusters(Int_t type);
  //void BuildECalIslandClusters();
  //void BuildECalRadiusClusters();
  // virtual void EndProcessing();
  virtual void HistoInit();
  virtual void InitFlags();
  virtual void AnalyzeEvent(TRawEvent* evt);
  Int_t FindSeed(Int_t nele, Int_t * Used, Double_t* Ene);
  Int_t IsSeedNeig(Int_t seedID, Int_t cellID);
  //  vector<TRecoVCluster *> &getClusters(){return fClusters;}

private:
  Int_t fClusterizationAlgo;
  Double_t fClDeltaTime;
  Int_t fClDeltaCellMax;
  Double_t fClEnThrForHit;
  Double_t fClEnThrForSeed;

  //Clusters vectors
  std::vector<double> ClE;
  std::vector<int>    ClSeed;
  std::vector<double> ClX;
  std::vector<double> ClY;
  std::vector<double> ClTime;
  std::vector<int>    ClNCry;

  //Seeds vectors
  std::vector<double> SdEn;
  std::vector<double> SdTime;
  std::vector<double> SdCell;

  //  std::vector<double> TTotECAL;
  //  std::vector<double> QTotECAL;

  double EvTotE;

  utl::ConfigParser *fConfigParser;
  PadmeVRecoConfig *fConfig;

// Run type variables
  Bool_t fIsPed;
  Bool_t fIsReco;
  Bool_t fIsCosmic;
  Bool_t fIsMonitor;
  Int_t  fIsGlobalDebug;
  Int_t  fIsLocalDebug;

  //  vector<TRecoVCluster *> fClusters;


};
#endif
