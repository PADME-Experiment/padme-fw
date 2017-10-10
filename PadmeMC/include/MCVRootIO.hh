// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------
#ifndef MCVRootIO_h
#define MCVRootIO_h 1

#include "globals.hh"
#include "G4Event.hh"

#include "TFile.h"
#include "TDetectorInfo.hh"

class MCVRootIO
{

public:
  
  MCVRootIO(G4String);
  // In the concrete fInstance you need to implement
  // the mandatory virtual methods
  virtual ~MCVRootIO();
  virtual void NewRun(G4int, TFile*, TDetectorInfo*) = 0;
  virtual void EndRun() = 0;
  virtual void SaveEvent(const G4Event*) = 0;
  virtual void Close() = 0;

public:

  Int_t GetBufSize()                { return fBufSize;      };
  void  SetBufSize(Int_t value)     { fBufSize = value;     };

  Int_t GetBranchStyle()            { return fBranchStyle;  };
  void  SetBranchStyle(Int_t value) { fBranchStyle = value; };

  G4int GetVerbose()                { return fVerbose;      };
  void  SetVerbose(G4int value)     { fVerbose = value;     };

  G4String GetName()                { return fName;     };
  void     SetName(G4String value)  { fName = value;    };

  G4bool   GetEnabled()             { return fEnabled;  };
  void     SetEnabled(G4bool value) { fEnabled = value; };

  G4bool   GetHitsEnabled()             { return fHitsEnabled;  };
  void     SetHitsEnabled(G4bool value) { fHitsEnabled = value; };

  G4bool   GetDigisEnabled()             { return fDigisEnabled;  };
  void     SetDigisEnabled(G4bool value) { fDigisEnabled = value; };

protected:

  Int_t fBufSize;
  Int_t fBranchStyle;

  G4int fRunNumber;
  G4int fVerbose;

  G4String fName;

  G4bool fEnabled;
  G4bool fHitsEnabled;
  G4bool fDigisEnabled;

};

#endif
