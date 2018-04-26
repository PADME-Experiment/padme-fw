#ifndef RecoVRootIO_h
#define RecoVRootIO_h 1


#include "TFile.h"
#include "PadmeVNamedModule.hh"

class PadmeVNamedModule;


class RecoVRootIO: public PadmeVNamedModule
{

public:
  
  RecoVRootIO(TString);
  // In the concrete fInstance you need to implement
  // the mandatory virtual methods
  virtual ~RecoVRootIO();
  virtual void NewRun(Int_t, TFile*) = 0;
  virtual void EndRun() = 0;
  virtual void SaveEvent() = 0;
  virtual void Close() = 0;

public:

  Int_t GetBufSize()                { return fBufSize;      };
  void  SetBufSize(Int_t value)     { fBufSize = value;     };

  Int_t GetBranchStyle()            { return fBranchStyle;  };
  void  SetBranchStyle(Int_t value) { fBranchStyle = value; };

  Int_t GetVerbose()                { return fVerbose;      };
  void  SetVerbose(Int_t value)     { fVerbose = value;     };

  bool   GetEnabled()             { return fEnabled;  };
  void     SetEnabled(bool value) { fEnabled = value; };


protected:

  Int_t fBufSize;
  Int_t fBranchStyle;

  Int_t fRunNumber;
  Int_t fVerbose;


  bool fEnabled;

};

#endif
