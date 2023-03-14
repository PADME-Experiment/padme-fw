#ifndef TargetReconstruction_H
#define TargetReconstruction_H

class TString;

class TRawEvent;
class TMCVEvent;
class TMCEvent;
class TTargetRecoEvent;
class TRecoEvent;
class TRecoVHit;
//class TRecoVCluster;

class HistoSvc;
class TrigTimeSvc;
class RunConditionSvc;
class RunConfigurationSvc;

class TargetDigitizer;
//class TargetClusterizer;
class TargetHit;
//class TargetCluster;

class TargetReconstruction
{

public:
  
  TargetReconstruction(TString);
  ~TargetReconstruction();

  void Init();
  void ProcessEvent(TRawEvent*);
  void ProcessEvent(TMCVEvent*,TMCEvent*);
  void ProcessEvent(TTargetRecoEvent*,TRecoEvent*);
  void EndProcessing();

  Bool_t writeHits()     { return fWriteHits; }
  //Bool_t writeClusters() { return fWriteClusters; }

  vector<TargetHit*>     &GetRecoHits()     { return fHits; }
  //vector<TargetCluster*> &GetRecoClusters() { return fClusters; }

  Int_t GetVerbose() { return fVerbose; }

private:

  HistoSvc* fHistoSvc;
  TrigTimeSvc* fTrigTimeSvc;
  RunConditionSvc* fRunConditionSvc;
  RunConfigurationSvc* fRunConfigurationSvc;

  PadmeVRecoConfig* fTargetConfig;

  TargetDigitizer* fTargetDigitizer;
  //TargetClusterizer* fTargetClusterizer;

  Bool_t fWriteHits;
  //Bool_t fWriteClusters;

  vector<TargetHit*> fHits;
  //vector<TargetCluster*> fClusters;

  Int_t fVerbose;

};
#endif
