// --------------------------------------------------------------
#ifndef RootIOManager_H
#define RootIOManager_H 1

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TClonesArray.h"

#include "TRecoEvent.hh"
#include "PadmeReconstruction.hh"
#include "utlConfigParser.hh"
#include "PadmeVRecoConfig.hh"

#include "RecoVRootIO.hh"
#include <list>

class TRecoEvent;
class ETagRecoRootIO;
class TargetRecoRootIO;

class RecoRootIOManager
{
public:

  RecoRootIOManager(TString);
  ~RecoRootIOManager();

  void NewRun(Int_t);
  void EndRun();
  void SaveEvent();

  void Close();

  RecoVRootIO* FindRecoRootIO(TString);

public:

  Int_t GetVerbose()            { return fVerbose;  }
  void  SetVerbose(Int_t value) { fVerbose = value; }

  TFile* GetFile()             { return fFile;  }
  void   SetFile(TFile* value) { fFile = value; }

  TRecoEvent* GetEvent()                  { return fEvent;  }
  void        SetEvent(TRecoEvent* value) { fEvent = value; }

  TTree* GetEventTree()              { return fEventTree;  }
  void   SetEventTree(TTree * value) { fEventTree = value; }

  void    SetFileName(TString newName);
  TString GetFileName() { return fFileName; }

  void SetReconstruction(PadmeReconstruction *r) { fReco = r; }
  PadmeReconstruction * GetReconstruction()      { return fReco; }

private:

  Int_t fBufSize;
  Int_t fBranchStyle;
  Int_t fCompLevel;
  Int_t fVerbose;
  Bool_t firstTime;

  TString fFileName;
  bool   fFileNameHasChanged;

  TFile*   fFile;
  TTree*   fEventTree; //Tree to hold all events in one run
  TBranch* fEventBranch;

  TRecoEvent* fEvent;

  PadmeReconstruction *fReco;
  utl::ConfigParser *fConfigParser;
  PadmeVRecoConfig *fConfig;

  typedef std::list<RecoVRootIO*> RootIOList;
  RootIOList fRootIOList;

  ETagRecoRootIO* fETagRecoRootIO;
  TargetRecoRootIO* fTargetRecoRootIO;

};
#endif // RootIOManager_H
