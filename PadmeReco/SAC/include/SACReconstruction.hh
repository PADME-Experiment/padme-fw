// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#ifndef SACReconstruction_H
#define SACReconstruction_H

#include "PadmeVReconstruction.hh"
	     
class TRecoVCluster;

class SACReconstruction : public PadmeVReconstruction
{
 
public:
  
  SACReconstruction(TFile*, TString);
  ~SACReconstruction();

  // void ParseConfFile(TString);
  // virtual void Init(PadmeVReconstruction*);
  virtual void ProcessEvent(TMCVEvent*,TMCEvent*);
  // virtual void EndProcessing();
  virtual void HistoInit();
  virtual void AnalyzeEvent(TRawEvent* evt);
  //  virtual void ProcessEvent(TRawEvent*);
  virtual void BuildClusters();
  Int_t FindSeed(Int_t nele, Int_t * Used, Double_t* Ene);
  Int_t IsSeedNeig(Int_t seedID, Int_t cellID);
  //  vector<TRecoVCluster *> &getClusters(){return fClusters;}

private:
  //Clusters vectors
  std::vector<double> ClE;
  std::vector<int> ClSeed;
  std::vector<double> ClX;
  std::vector<double> ClY;
  std::vector<double> ClTime;
  std::vector<int> ClNCry;

  //Seeds vectors
  std::vector<double> SdEn;
  std::vector<double> SdTime;
  std::vector<double> SdCell;

  std::vector<double> TTotSAC;
  std::vector<double> QTotSAC;
  double EvTotE;


  //  vector<TRecoVCluster *> fClusters;

>>>>>>> develop
};
#endif
