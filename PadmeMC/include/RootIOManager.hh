// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------
#ifndef RootIOManager_H
#define RootIOManager_H 1

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TClonesArray.h"
//#include "TGraph.h"
//#include "TAxis.h"

#include "globals.hh"

#include "MCVRootIO.hh"
#include <list>

class TPadmeRun;
class TMCEvent;

class G4Event;

class RootIOManager
{
public:

  virtual ~RootIOManager();

  static RootIOManager* GetInstance();

  void NewRun(G4int);
  void EndRun();

  void SaveEvent(const G4Event*);

  void Close();

  MCVRootIO* FindRootIO(G4String);

  //G4double GetVirtualMemoryUsage();

private:

  static RootIOManager* fInstance;

protected:

  RootIOManager();

public:

  void EnableSubDetectorIO(G4String);
  void DisableSubDetectorIO(G4String);

  Int_t GetBufSize()                { return fBufSize;      };
  void  SetBufSize(Int_t value)     { fBufSize = value;     };

  Int_t GetBranchStyle()            { return fBranchStyle;  };
  void  SetBranchStyle(Int_t value) { fBranchStyle = value; };

  Int_t GetCompLevel()              { return fCompLevel;    };
  void  SetCompLevel(Int_t value)   { fCompLevel = value;   };

  G4int GetVerbose()                { return fVerbose;      };
  void  SetVerbose(G4int value)     { fVerbose = value;     };

  G4String GetFileName()            { return fFileName;     };
  void     SetFileName(G4String value);

  G4bool   GetFileNameHasChanged()             { return fFileNameHasChanged;  }
  void     SetFileNameHasChanged(G4bool value) { fFileNameHasChanged = value; }

  TFile*   GetFile()              { return fFile;          }
  void     SetFile(TFile * value) { fFile = value;         }

  TTree*   GetRunTree()              { return fRunTree;    }
  void     SetRunTree(TTree * value) { fRunTree = value;   }

  TTree*   GetEventTree()               { return fEventTree;     }
  void     SetEventTree(TTree * value)  { fEventTree = value;    }

  TBranch* GetRunBranch()                { return fRunBranch;  }
  void     SetRunBranch(TBranch * value) { fRunBranch = value; }

  TBranch* GetEventBranch()                { return fEventBranch;   }
  void     SetEventBranch(TBranch * value) { fEventBranch = value;  }

  TPadmeRun* GetRun()                 { return fRun;    }
  void       SetRun(TPadmeRun* value) { fRun = value;   }

  TMCEvent* GetEvent()                 { return fEvent;  }
  void      SetEvent(TMCEvent * value) { fEvent = value; }

  //RootIOList GetRootIOList()                 { return fRootIOList;  };
  //void       SetRootIOList(RootIOList value) { fRootIOList = value; };

private:

  Int_t fBufSize;
  Int_t fBranchStyle;
  Int_t fCompLevel;
  G4int fVerbose;

  G4String fFileName;
  G4bool   fFileNameHasChanged;

  TFile*   fFile;
  TTree*   fRunTree;   //Tree to hold all runs in one file
  TTree*   fEventTree; //Tree to hold all events in one run
  TBranch* fRunBranch;
  TBranch* fEventBranch;
  TPadmeRun* fRun;
  TMCEvent*  fEvent;
  //TGraph * fGVirtMem;

  typedef std::list<MCVRootIO*> RootIOList;
  RootIOList fRootIOList;

};
#endif // RootIOManager_H
