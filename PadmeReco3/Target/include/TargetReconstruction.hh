#ifndef TargetReconstruction_H
#define TargetReconstruction_H

class TString;
class TH1D;

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
//class TargetHit;
//class TargetCluster;
class TargetStripCharge;

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

  //commented by Beth 21/3/24: The concept of "hits" and "clusters" in the target doesn't really have any meaning. It would be better to compute the charge per strip in the digitizer and then convert it to NPoT and other physical quantities here
  /*
  Bool_t writeHits()     { return fWriteHits; }
  //Bool_t writeClusters() { return fWriteClusters; }

  vector<TargetHit*>     &GetRecoHits()     { return fHits; }
  //vector<TargetCluster*> &GetRecoClusters() { return fClusters; }
  */

  Bool_t writeStripCharge()     { return fWriteStripCharge; }
  vector<TargetStripCharge*>     &GetRecoStripCharge()     { return fStripCharge; }

  Int_t GetVerbose() { return fVerbose; }

private:

  HistoSvc* fHistoSvc;
  TrigTimeSvc* fTrigTimeSvc;
  RunConditionSvc* fRunConditionSvc;
  RunConfigurationSvc* fRunConfigurationSvc;

  PadmeVRecoConfig* fTargetConfig;

  TargetDigitizer* fTargetDigitizer;
  //TargetClusterizer* fTargetClusterizer;

  //commented by Beth 21/3/24: The concept of "hits" and "clusters" in the target doesn't really have any meaning. It would be better to compute the charge per strip in the digitizer and then convert it to NPoT and other physical quantities here
  /*
  Bool_t fWriteHits;
  //Bool_t fWriteClusters;

  vector<TargetHit*> fHits;
  //vector<TargetCluster*> fClusters;
  */

  Bool_t fWriteStripCharge;
  vector<TargetStripCharge*> fStripCharge;

  Int_t fVerbose;

};
#endif
