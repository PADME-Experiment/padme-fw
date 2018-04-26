// --------------------------------------------------------------
#ifndef RootIOManager_H
#define RootIOManager_H 1

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TClonesArray.h"

#include "TRecoEvent.hh"

#include "RecoVRootIO.hh"
#include <list>


class TRecoEvent;


class RecoRootIOManager
{
public:

  virtual ~RecoRootIOManager();

  static RecoRootIOManager* GetInstance();

  void NewRun(Int_t);
  void EndRun();
  void SaveEvent();

  void Close();

  RecoVRootIO* FindRecoRootIO(TString);


private:

  static RecoRootIOManager* fInstance;

protected:

  RecoRootIOManager();

public:


  TFile*   GetFile()              { return fFile;          }
  void     SetFile(TFile * value) { fFile = value;         }

  TRecoEvent* GetEvent()                 { return fEvent;  }
  void      SetEvent(TRecoEvent * value) { fEvent = value; }

  TTree*   GetEventTree()               { return fEventTree;     }
  void     SetEventTree(TTree * value)  { fEventTree = value;    }

  void SetFileName(TString newName);
  TString GetFileName(){return fFileName;};



private:

  Int_t fBufSize;
  Int_t fBranchStyle;
  Int_t fCompLevel;
  Int_t fVerbose;

  TString fFileName;
  bool   fFileNameHasChanged;

  TFile*   fFile;
  TTree*   fEventTree; //Tree to hold all events in one run
  TBranch* fEventBranch;
  TRecoEvent*  fEvent;

  typedef std::list<RecoVRootIO*> RootIOList;
  RootIOList fRootIOList;

};
#endif // RootIOManager_H
