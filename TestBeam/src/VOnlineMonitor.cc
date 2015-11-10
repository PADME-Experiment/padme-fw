#include "VOnlineMonitor.hh"

#include "TEveManager.h"
#include "TEveBrowser.h"
#include "TRootEmbeddedCanvas.h"

VOnlineMonitor::VOnlineMonitor(TString name)
{

  fName = name;

  // Create main tab
  gEve->GetBrowser()->StartEmbedding(1,-1);

  fMainTabFrame = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
  fMainTabFrame->SetName(fName);
  fMainTab = new TGTab(fMainTabFrame,300,300);

}

VOnlineMonitor::~VOnlineMonitor()
{}

void VOnlineMonitor::CompleteTab(){

  fMainTab->SetTab(0);
  fMainTab->Resize();
  fMainTabFrame->AddFrame(fMainTab, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY,2,2,2,2));

  fMainTabFrame->SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputModeless);
  fMainTabFrame->MapSubwindows();
  fMainTabFrame->Resize();
  fMainTabFrame->MapWindow();
  gEve->GetBrowser()->StopEmbedding(fName);

}

TCanvas* VOnlineMonitor::AddCanvasTab(TString name){

  // container of Tab
  TGCompositeFrame* subTabCompositeFrame = fMainTab->AddTab(name);
  subTabCompositeFrame->SetLayoutManager(new TGVerticalLayout(subTabCompositeFrame));

  // embedded canvas
  TRootEmbeddedCanvas* rootEmbeddedCanvas = new TRootEmbeddedCanvas(0,subTabCompositeFrame,300,300);
  subTabCompositeFrame->AddFrame(rootEmbeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY,2,2,2,2));

  fCanvasList.push_back(rootEmbeddedCanvas->GetCanvas());
  return rootEmbeddedCanvas->GetCanvas();

}

TCanvas* VOnlineMonitor::AddCanvas(TString name){
  fCanvasList.push_back(new TCanvas(name, name, 300, 300));
  return fCanvasList.back();
}

void VOnlineMonitor::UpdateAllCanvas()
{

  printf("%s - Updating all canvas\n",fName.Data());
  for ( std::vector<TCanvas*>::iterator it = fCanvasList.begin() ; it != fCanvasList.end(); ++it ) {
    Int_t nFrames = (*it)->GetListOfPrimitives()->GetEntries();
    for(Int_t iFrame = 0; iFrame < nFrames; iFrame++){
      TPad* pad = (TPad*)(*it)->GetListOfPrimitives()->At(iFrame);
      if(pad){
        pad->Modified();
        pad->Update();
      }   
    }
  }
}
