#ifndef ETagReconstruction_H
#define ETagReconstruction_H

class TString;

class TRawEvent;
class TMCVEvent;
class TMCEvent;
class TETagRecoEvent;
class TRecoEvent;
class TRecoVHit;
class TRecoVCluster;

class HistoSvc;
class TrigTimeSvc;
class RunConditionSvc;
class RunConfigurationSvc;

class ETagDigitizer;
class ETagHit;
class ETagCluster;

class ETagReconstruction
{

public:
  
  ETagReconstruction(TString);
  ~ETagReconstruction();

  void Init();
  void ProcessEvent(TRawEvent*);
  void ProcessEvent(TMCVEvent*,TMCEvent*);
  void ProcessEvent(TETagRecoEvent*,TRecoEvent*);
  void EndProcessing();

  Bool_t writeHits()     { return fWriteHits; }
  Bool_t writeClusters() { return fWriteClusters; }

  vector<ETagHit*>     &GetRecoHits()     { return fHits; }
  vector<ETagCluster*> &GetRecoClusters() { return fClusters; }

  Int_t GetVerbose() { return fVerbose; }

private:

  HistoSvc* fHistoSvc;
  TrigTimeSvc* fTrigTimeSvc;
  RunConditionSvc* fRunConditionSvc;
  RunConfigurationSvc* fRunConfigurationSvc;

  PadmeVRecoConfig* fETagConfig;

  ETagDigitizer* fETagDigitizer;

  Bool_t fWriteHits;
  Bool_t fWriteClusters;

  vector<ETagHit*> fHits;
  vector<ETagCluster*> fClusters;

  Int_t fVerbose;

};
#endif
