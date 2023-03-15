#ifndef ECalReconstruction_H
#define ECalReconstruction_H

class TString;

class TRawEvent;
class TMCVEvent;
class TMCEvent;
class TECalRecoEvent;
class TRecoEvent;
class TRecoVHit;
class TRecoVCluster;

class HistoSvc;
class TrigTimeSvc;
class RunConditionSvc;
class RunConfigurationSvc;

class ECalDigitizer;
class ECalClusterizer;
class ECalHit;
class ECalCluster;

class ECalReconstruction
{

public:
  
  ECalReconstruction(TString);
  ~ECalReconstruction();

  void Init();
  void ProcessEvent(TRawEvent*);
  void ProcessEvent(TMCVEvent*,TMCEvent*);
  void ProcessEvent(TECalRecoEvent*,TRecoEvent*);
  void EndProcessing();

  Bool_t writeHits()     { return fWriteHits; }
  Bool_t writeClusters() { return fWriteClusters; }

  vector<ECalHit*>     &GetRecoHits()     { return fHits; }
  vector<ECalCluster*> &GetRecoClusters() { return fClusters; }

  Int_t GetVerbose() { return fVerbose; }

private:

  HistoSvc* fHistoSvc;
  TrigTimeSvc* fTrigTimeSvc;
  RunConditionSvc* fRunConditionSvc;
  RunConfigurationSvc* fRunConfigurationSvc;

  PadmeVRecoConfig* fECalConfig;

  ECalDigitizer* fECalDigitizer;
  ECalClusterizer* fECalClusterizer;

  Bool_t fWriteHits;
  Bool_t fWriteClusters;

  vector<ECalHit*> fHits;
  vector<ECalCluster*> fClusters;

  Int_t fVerbose;

};
#endif
