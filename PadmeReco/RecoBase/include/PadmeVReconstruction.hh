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

#include "PadmeVNamedModule.hh"
#include "utlConfigParser.hh"
#include "PadmeVRecoConfig.hh"

#include "TH1.h"
#include <vector>
#include <string>
#include <map>

using namespace std;

class PadmeVReconstruction : public PadmeVNamedModule
{
public:

  PadmeVReconstruction(TFile*, TString, TString);
  virtual ~PadmeVReconstruction();
  //virtual TRecoVEvent* ProcessEvent(TDetectorVEvent* = 0, Event* = 0) = 0;
  virtual void ProcessEvent(TMCVEvent* = 0,TMCEvent* = 0);
  virtual void ProcessEvent(TRawEvent* = 0);
  virtual void Init(PadmeVReconstruction*) = 0;
  virtual void EndProcessing() = 0; ///< Call from derived classes
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
  vector<TRecoVHit *> GetRecoHits(){return fHits;};
  

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

};
#endif
