// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-21
//
// --------------------------------------------------------------
#ifndef PadmeVReconstruction_H
#define PadmeVReconstruction_H

#include "TFile.h"
#include "TDirectory.h"

#include "TRawEvent.hh"
#include "TMCEvent.hh"
#include "TMCVEvent.hh"
#include "TRecoVHit.hh"
#include "TRecoVCluster.hh"
#include "TRecoVObject.hh"

#include "PadmeVNamedModule.hh"
#include "utlConfigParser.hh"
#include "PadmeVRecoConfig.hh"
#include "RecoVChannelID.hh"
#include "PadmeVCalibration.hh"

#include "TH1.h"
#include <vector>
#include <string>
#include <map>

#include "ChannelVReco.hh"
#include "PadmeVTrigger.hh"

using namespace std;

class TRecoEvent;
class PadmeVClusterization;
class PadmeVGeometry;
class GlobalRecoConfigOptions;

class PadmeVReconstruction : public PadmeVNamedModule, public RecoVChannelID
{
public:

  PadmeVReconstruction(TFile*, TString, TString);
  virtual ~PadmeVReconstruction();
  //virtual TRecoVEvent* ProcessEvent(TDetectorVEvent* = 0, Event* = 0) = 0;
  virtual void ProcessEvent(TMCVEvent* = 0,TMCEvent* = 0);
  virtual void ProcessEvent(TRawEvent* = 0);
  virtual void ProcessEvent(TRecoVObject* =0, TRecoEvent* =0);
  virtual bool TriggerToBeSkipped();
  virtual void ClearHits();
  virtual void ClearClusters();
  virtual void BuildHits(TRawEvent*);
  virtual void ReadHits(TRecoVObject*, TRecoEvent*);
  virtual void ConvertMCDigitsToRecoHits(TMCVEvent*, TMCEvent*);
  virtual void BuildClusters();
  //  virtual Int_t IsSeedNeig(Int_t seedID, Int_t cellID);
  //  virtual Int_t findSeed(std::vector<Int_t> hUsed);
  virtual void BuildTriggerInfo(TRawEvent* );
  virtual void AnalyzeEvent(TRawEvent* = 0);
  virtual void Init(PadmeVReconstruction*);
  //virtual void InitRunningModeFlags();
  virtual void EndProcessing(); ///< Call from derived classes
  virtual void ParseConfFile(TString);
  virtual void HistoInit(); 
  virtual void HistoExit();
  virtual void AddHisto(string,TH1 *);
  virtual TH1* GetHisto(string);  
  static void Exception(TString);
  GlobalRecoConfigOptions* GetGlobalRecoConfigOptions(){return fGlobalRecoConfigOptions;}

public:

  //TRecoVEvent* GetRecoEvent() { return fRecoEvent; };
  //void         SetRecoEvent(TRecoVEvent* value) { fRecoEvent = value; };

  PadmeVReconstruction* GetMainReco() { return fMainReco; };
  void                  SetMainReco(PadmeVReconstruction* value) { fMainReco = value; };

  TFile* GetHistoFile() { return fHistoFile; };

  TString GetConfigFileName() { return fConfigFileName; };
  void    SetConfigFileName(TString val) { fConfigFileName = val; };
  utl::ConfigParser *GetConfigParser(){return fConfigParser;};
  PadmeVRecoConfig *GetConfig(){return fConfig;};
  vector<TRecoVHit *> &GetRecoHits(){return fHits;};
  vector<TRecoVCluster *> &GetClusters(){return fClusters;}

  PadmeVTrigger *GetTriggerProcessor(){return fTriggerProcessor;};


  Bool_t writeHits(){return fWriteHits;}
  Bool_t writeClusters(){return fWriteClusters;}
  /*
  void  SetIsPedOn()     { fIsPed = true; }
  void  SetIsPedOff()    { fIsPed = false; }
  bool  GetIsPed()      { return fIsPed;}

  void  SetIsRecoOn()    { fIsReco = true; }
  void  SetIsRecoOff()   { fIsReco = false; }
  bool  GetIsReco()      { return fIsReco;}

  void  SetIsCosmicOn()  { fIsCosmic = true; }
  void  SetIsCosmicOff() { fIsCosmic = false; }
  bool  GetIsCosmic()    { return fIsCosmic;}

  void  SetIsMonitorOn()  { fIsMonitor = true; }
  void  SetIsMonitorOff() { fIsMonitor = false; }
  bool  GetIsMonitor()    { return fIsMonitor;}

  void  SetIsGlobalDebug(Int_t value) { fIsGlobalDebug = value; }
  void  SetIsLocalDebug(Int_t value)  { fIsLocalDebug = value; }
  Int_t GetIsGlobalDebug()  { return fIsGlobalDebug; }
  
  Int_t GetRunningMode() {return fRunningMode;}
  */

  // Use to get an existing directory or create if not already made
  //TDirectory* GetOrMakeDir(TDirectory *inDir,TString dirName);	  

protected:

  TFile* fHistoFile;
  PadmeVReconstruction* fMainReco;

  //TRecoVEvent * fRecoEvent;

  TString fConfigFileName;
  utl::ConfigParser *fConfigParser;
  PadmeVRecoConfig *fConfig;


  map<string,TH1 *> fHistoMap;

  vector<TRecoVHit *> fHits;
  vector<TRecoVCluster *> fClusters;

  ChannelVReco *fChannelReco;
  PadmeVClusterization *fClusterization;
  PadmeVCalibration *fChannelCalibration;
  PadmeVTrigger *fTriggerProcessor;
  PadmeVGeometry *fGeometry;

  Bool_t fWriteHits;
  Bool_t fWriteClusters;

  GlobalRecoConfigOptions* fGlobalRecoConfigOptions;
  /*
  Bool_t fIsPed;
  Bool_t fIsReco;
  Bool_t fIsCosmic;
  Bool_t fIsMonitor;
  Int_t  fIsLocalDebug;
  Int_t  fIsGlobalDebug;
  Int_t  fRunningMode;
  */

};
#endif
