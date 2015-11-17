#ifndef Grapher_h
#define Grapher_h

#include "TApplication.h"
#include "TTimer.h"
#include "TGButton.h"

#include "VOnlineMonitor.hh"

class Grapher {

public:

  virtual ~Grapher();
  static Grapher* GetInstance();

  void    SetFileName(TString name) { fFileName = name; }
  TString GetFileName()             { return fFileName; }

  void   SetNEvents(UInt_t n) { fNEvents = n; }
  UInt_t GetNEvents()         { return fNEvents; }

  void SetApplication(TApplication* app) { fApplication = app; }

  VOnlineMonitor* OnlineMonitor(TString);

  void AnalyzeEvents();
  void HoldMonitor();
  void ResumeMonitor();

private:

  static Grapher* fInstance;

protected:

  Grapher();

  TString fFileName;
  UInt_t  fNEvents;
  TString fDataFile;

  Int_t fCurrentStatus; // 0: on hold, 1:updating

  TApplication* fApplication;
  TTimer* fTimer;
  TGTextButton* fHoldButton;
  TGTextButton* fResumeButton;
  TGTextButton* fNextButton;
  TGTextButton* fExitButton;

  std::vector<VOnlineMonitor*> fOnlineMonitorList;

  ClassDef(Grapher,0)
};
#endif
