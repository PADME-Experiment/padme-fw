// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#ifndef ECalReconstruction_H
#define ECalReconstruction_H

#include "PadmeVReconstruction.hh"
#include "TRandom2.h"

class TRecoVCluster;
struct TimeChIdx
{ int digiIndex;
  double digiTime;
  double digiChannel;
};

class ECalReconstruction : public PadmeVReconstruction
{

public:

  ECalReconstruction(TFile*, TString);
  ~ECalReconstruction();

  // void ParseConfFile(TString);
  // virtual void Init(PadmeVReconstruction*);
  // void Init(PadmeVReconstruction* MainReco);
  void ProcessEvent(TMCVEvent*,TMCEvent*);
  void ProcessEvent(TRawEvent*);
  void BuildHits(TRawEvent* rawEv);
  Double_t CompensateMissingE(Double_t ECl,Int_t ClSeed); //M. Raggi 21/05/2019
  virtual void BuildClusters();
  bool TriggerToBeSkipped();
  void BuildSimpleECalClusters();
  void BuildECalIslandRadiusClusters(Int_t type);
  //void BuildECalIslandClusters();
  //void BuildECalRadiusClusters();
  // virtual void EndProcessing();
  
  virtual void ConvertMCDigitsToRecoHits(TMCVEvent* tEvent,TMCEvent* tMCEvent);
  virtual void ConvertMCDigitsToRecoHitsWave(TMCVEvent* tEvent,TMCEvent* tMCEvent);
  virtual void HistoInit();
  virtual void AnalyzeEvent(TRawEvent* evt);
  Int_t FindSeed(Int_t nele, Int_t * Used, Double_t* Ene);
  Int_t IsSeedNeig(Int_t seedID, Int_t cellID);
  //  vector<TRecoVCluster *> &getClusters(){return fClusters;}
  Bool_t SimulateBrokenSU(Int_t x ,Int_t y);
  Double_t EnergyResolution(Double_t energy);

  Double_t compute_rms(Short_t* samples);

private:
  Int_t fClusterizationAlgo;
  Int_t fCompensateMissingE;
  Double_t fClDeltaTime;
  Int_t fClDeltaCellMax;
  Double_t fClEnThrForHit;
  Double_t fClEnThrForSeed;
  Int_t NNoHits;
  Int_t fClusterTimeAlgo;
  Int_t fMultihitForMC;
  //Int_t fDeteriorateEnergyResolution;
  Int_t fDeteriorateHitEnResolution;
  Int_t fDeteriorateHitTimeResolution;

  Int_t fReproductSACbunchStructure;

  Double_t fTemplate[5001];
  Double_t fmaxValuemyTemplate;

  TRandom2 *r;
  
  TH1F *hSAC_HitTime;
  TH1F *hCumulative;
  Double_t fComulativeMax;
  Double_t fTimeMin;
  Double_t fTimeMax;
  void MultiHit1Algo(std::vector<TimeChIdx> vsorteddigi, TMCVEvent* tEvent);
  void MultiHit0Algo(std::vector<TimeChIdx> vsorteddigi, TMCVEvent* tEvent);
  void MultiHit2Algo(std::vector<TimeChIdx> vsorteddigi, TMCVEvent* tEvent);
  void MultiHitminus2Algo(std::vector<TimeChIdx> vsorteddigi, TMCVEvent* tEvent);

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
  TF1 *fEnergyCompensation;

  //  vector<TRecoVCluster *> fClusters;


};
#endif
