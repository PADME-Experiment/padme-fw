#ifndef Histo_h
#define Histo_h

#include "TH1D.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TList.h"

#include "TEveManager.h"
#include "TEveBrowser.h"
#include "TGFrame.h"
#include "TGTab.h"
//#include <vector>

class Histo {
  TRootBrowser * GetMainWindow() { return fMainWindow; }
  TCanvas * AddCanvasTab(TString);
  TCanvas * AddCanvas(TString);

public :
  //  Histo();
  ~Histo();
  static Histo* GetInstance();
  
  void WriteHisto();
  void InitMonitor();

  void DrawCalo(Int_t,Int_t);
  void DrawTarg(Int_t,Int_t);

  TH1D* Get1DHisto(const char* name){return (TH1D*)hListCal->FindObject(name);}; 
  TH2D* Get2DHisto(const char* name){return (TH2D*)hListCal->FindObject(name);};
  TGraphErrors* GetGraph(const char* name){return (TGraphErrors*)hListCal->FindObject(name);};

  Int_t Fill1D(const char* ,Float_t );
  Int_t Fill2D(const char* ,Float_t ,Float_t);
  Int_t FillGraph(const char* ,Int_t, Int_t ,Float_t *,Float_t *);

private:
  void InitHisto();
private:
  TList* hListCal;  
  TGraphErrors* gCaloRaw[32];
  TGraphErrors* gCaloTrigRaw[4];

  TGraphErrors* gCaloReco[32];
  TGraphErrors* gCaloTrigReco[4];

  TH1D ** hPedCalo;
  TH1D ** hPedTarg;

  TH1D ** hPedRMSCalo;
  TH1D ** hPedRMSTarg;

  TH1D ** hQCh;

  TH1D * hCaloQTot;
  TH1D * hTargQTot;

  static Histo* fInstance;
  
public:
  Int_t NCalo;
  Int_t NTrigCalo;
 protected:
  Histo();

  TRootBrowser * fMainWindow;
  TGMainFrame *  fMainTabFrame;
  TGTab *        fMainTab;
  //  vector<TCanvas*> fCanvases;  
};
#endif
