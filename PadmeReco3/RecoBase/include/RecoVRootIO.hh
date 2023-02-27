#ifndef RecoVRootIO_h
#define RecoVRootIO_h 1


#include "TFile.h"
//#include "PadmeVNamedModule.hh"
#include "TTree.h"
#include "TBranch.h"
#include "TRecoVCluster.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVObject.hh"
#include "PadmeVNamedModule.hh"

//class PadmeVNamedModule;
class RecoRootIOManager;

class RecoVRootIO: public PadmeVNamedModule
{

public:
  
  RecoVRootIO(TString,RecoRootIOManager*);
  // In the concrete fInstance you need to implement
  // the mandatory virtual methods
  virtual ~RecoVRootIO();
  virtual void NewRun(Int_t, TFile*);
  virtual void EndRun();
  virtual void SaveEvent();
  virtual void Close();

public:

  Int_t GetBufSize()                { return fBufSize;      };
  void  SetBufSize(Int_t value)     { fBufSize = value;     };

  Int_t GetBranchStyle()            { return fBranchStyle;  };
  void  SetBranchStyle(Int_t value) { fBranchStyle = value; };

  Int_t GetVerbose()                { return fVerbose;      };
  void  SetVerbose(Int_t value)     { fVerbose = value;     };

  Bool_t GetEnabled()             { return fEnabled;  };
  void   SetEnabled(Bool_t value) { fEnabled = value; };

protected:

  Int_t fBufSize;
  Int_t fBranchStyle;

  Int_t fRunNumber;
  Int_t fVerbose;

  Bool_t fEnabled;

  RecoRootIOManager* fRecoRootIOManager;

  TTree* fEventTree;
  TBranch* fBranch;
  TRecoVObject* fEvent;
  TBranch* fBranchClusColl;
  TRecoVClusCollection* fClusColl;

};

#endif
