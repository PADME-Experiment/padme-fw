#ifndef RecoVRootIO_h
#define RecoVRootIO_h 1


#include "TFile.h"
#include "PadmeVNamedModule.hh"
#include "TRecoVObject.hh"
#include "TTree.h"
#include "TBranch.h"
#include "TRecoVHit.hh"

class PadmeVNamedModule;


class RecoVRootIO: public PadmeVNamedModule
{

public:
  
  RecoVRootIO(TString);
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

  bool   GetEnabled()             { return fEnabled;  };
  void   SetEnabled(bool value)   { fEnabled = value; };


protected:

  Int_t fBufSize;
  Int_t fBranchStyle;

  Int_t fRunNumber;
  Int_t fVerbose;


  bool fEnabled;

  TTree* fEventTree;
  TBranch* fBranch;
  TRecoVObject* fEvent;

};

#endif
