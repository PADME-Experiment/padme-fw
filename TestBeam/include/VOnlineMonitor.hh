#ifndef VOnlineMonitor_h
#define VOnlineMonitor_h

#include "TCanvas.h"
#include "TGTab.h"
#include "TGFrame.h"

class VOnlineMonitor {

public:

  VOnlineMonitor(TString);
  ~VOnlineMonitor();

  void UpdateAllCanvas();

  TString GetName() { return fName; };

  TCanvas* AddCanvas(TString);
  TCanvas* AddCanvasTab(TString);

protected:

  void CompleteTab();

  TString fName;

  TGTab* fMainTab;
  TGMainFrame* fMainTabFrame;
  
  std::vector<TCanvas*> fCanvasList;

};
#endif
