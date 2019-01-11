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


using namespace std;

class TRecoEvent;

class PadmeVReconstruction : public PadmeVNamedModule, public RecoVChannelID
{
public:

  PadmeVReconstruction(TFile*, TString, TString);
  virtual ~PadmeVReconstruction();
  //virtual TRecoVEvent* ProcessEvent(TDetectorVEvent* = 0, Event* = 0) = 0;
  virtual void ProcessEvent(TMCVEvent* = 0,TMCEvent* = 0);
  virtual void ProcessEvent(TRawEvent* = 0);
  virtual void ProcessEvent(TRecoVObject* =0, TRecoEvent* =0);
  virtual void ClearHits();
  virtual void ClearClusters();
  virtual void BuildHits(TRawEvent*);
  virtual void ReadHits(TRecoVObject*, TRecoEvent*); 
  virtual void BuildClusters();
  virtual Int_t IsSeedNeig(Int_t seedID, Int_t cellID);
  virtual Int_t findSeed(std::vector<Int_t> hUsed);

  virtual void AnalyzeEvent(TRawEvent* = 0);
  virtual void Init(PadmeVReconstruction*);
  virtual void EndProcessing(); ///< Call from derived classes
  virtual void ParseConfFile(TString);
  virtual void HistoInit(); 
  virtual void HistoExit();
  virtual void AddHisto(string,TH1 *);
  virtual TH1* GetHisto(string);  
  static void Exception(TString);

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
  Double_t                fClusterTimeCut;
  vector<TRecoVCluster *> fClusters;

  ChannelVReco *fChannelReco;
  PadmeVCalibration *fChannelCalibration;


};
#endif
