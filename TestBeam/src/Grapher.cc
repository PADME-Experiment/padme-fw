#include "Grapher.hh"

//#include <stdlib.h>
//#include <stdio.h>
#include <iostream>
#include <fstream>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include "TRawEvent.hh"

#include "TEveManager.h"
#include "TEveBrowser.h"

#include "TRootEmbeddedCanvas.h"
#include "TGClient.h"
#include "TGFont.h"
#include "TGLabel.h"
#include "TGResourcePool.h"

#include "HistoManager.hh"

#include "ECALAnal.hh"
#include "TargetAnal.hh"

#include "ECALOnlineMonitor.hh"
#include "TargetOnlineMonitor.hh"

ClassImp(Grapher)

Grapher* Grapher::fInstance = 0;

Grapher::Grapher()
{

  // Set default parameters
  fFileName = "";
  fDataFile = "";
  fNEvents = 0;
  fApplication = 0;
  fCurrentStatus = 1;

  // Create timer for automatic histo update
  fTimer = new TTimer();
  fTimer->Connect("Timeout()","Grapher",this,"AnalyzeEvents()");

  // Create main EVE window with no interactive section
  TEveManager::Create(kTRUE,"VF");
  gEve->GetBrowser()->HideBottomTab();

  // Create tab for ECAL
  fOnlineMonitorList.push_back(new ECALOnlineMonitor());

  // Create tab for Target
  //fOnlineMonitorList.push_back(new TargetOnlineMonitor());

  // Create external window with OnlineMonitor command buttons
  TGMainFrame* frame = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
  frame->SetWindowName("PADME Online Monitor");

  // Setup text labels
  TGGC* textGC;
  TGFontPool* fpool = new TGFontPool(gClient);
  const TGFont* font = fpool->GetFont("helvetica",-32,kFontWeightNormal,kFontSlantRoman);
  if(!font) printf("Font not found\n");
  font->Print();
  //const TGFont *font = gClient->GetFont("-*-times-bold-r-*-*-32-*-*-*-*-*-*-*");
  //if (!font) font = gClient->GetResourcePool()->GetDefaultFont();
  FontStruct_t labelfont = font->GetFontStruct();
  GCValues_t gval;
  gval.fMask = kGCBackground | kGCFont | kGCForeground;
  gval.fFont = font->GetFontHandle();
  gClient->GetColorByName("yellow", gval.fBackground);
  textGC = gClient->GetGC(&gval, kTRUE);

  // If more colors are needed set ...
  //ULong_t bcolor, ycolor;
  //gClient->GetColorByName("yellow", ycolor);
  //gClient->GetColorByName("blue", bcolor);
  // ... and then use them with
  //TGLabel* label = new TGLabel(frame, "PADME Online Monitor",textGC->GetGC(),labelfont,kChildFrame,bcolor);
  //label->SetTextColor(ycolor);

  TGLabel* label = new TGLabel(frame, "PADME Online Monitor",textGC->GetGC(),labelfont,kChildFrame);
  frame->AddFrame(label, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
  // The following uses a predefined size for the label window
  label->ChangeOptions(label->GetOptions() | kFixedSize);
  label->Resize(350, 80);

  // Create a horizontal frame widget with buttons 
  TGHorizontalFrame* hframe = new TGHorizontalFrame(frame,200,80);

  fHoldButton = new TGTextButton(hframe,"&Hold");
  fHoldButton->Connect("Clicked()","Grapher",this,"HoldMonitor()");
  hframe->AddFrame(fHoldButton, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  fResumeButton = new TGTextButton(hframe,"&Resume");
  fResumeButton->Connect("Clicked()","Grapher",this,"ResumeMonitor()");
  hframe->AddFrame(fResumeButton, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  fNextButton = new TGTextButton(hframe,"&Next");
  fNextButton->Connect("Clicked()","Grapher",this,"AnalyzeEvents()");
  hframe->AddFrame(fNextButton, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  fExitButton = new TGTextButton(hframe,"&Exit","gApplication->Terminate(0)");
  hframe->AddFrame(fExitButton, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  frame->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,2,2,2,2));

  frame->MapSubwindows();
  //frame->Resize();
  frame->Resize(frame->GetDefaultSize());
  frame->MapWindow();

  //frame->SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputModeless);
  //gEve->GetBrowser()->StopEmbedding("Commands");

  // Set hold/resume buttons status according to default status
  if (fCurrentStatus==0) {
    fHoldButton->SetEnabled(kFALSE);
    fResumeButton->SetEnabled(kTRUE);
  } else {
    fHoldButton->SetEnabled(kTRUE);
    fResumeButton->SetEnabled(kFALSE);
  }

}

Grapher::~Grapher()
{
  for ( std::vector<VOnlineMonitor*>::iterator it = fOnlineMonitorList.begin() ; it != fOnlineMonitorList.end(); ++it )
    delete *it;
}

Grapher* Grapher::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new Grapher(); }
  return fInstance;
}

void Grapher::HoldMonitor()
{
  fCurrentStatus = 0;
  fHoldButton->SetEnabled(kFALSE);
  fResumeButton->SetEnabled(kTRUE);
  //fTimer->Reset();
}

void Grapher::ResumeMonitor()
{
  fCurrentStatus = 1;
  fHoldButton->SetEnabled(kTRUE);
  fResumeButton->SetEnabled(kFALSE);
  //fTimer->Start(10000,kTRUE);
}

VOnlineMonitor* Grapher::OnlineMonitor(TString name)
{
  for ( std::vector<VOnlineMonitor*>::iterator it = fOnlineMonitorList.begin() ; it != fOnlineMonitorList.end(); ++it ) {
    if ((*it)->GetName() == name) return *it;
  }
  return 0;
}

void Grapher::AnalyzeEvents()
{

  if (fFileName == "") {
    printf("ERROR - No filename defined\n");
    return;
  }

  // Hold the timer
  fTimer->Reset();

  // Get name of file with data to analyze from fFileName
  //std::ifstream list;
  std::string line;
  TString newFile;
  //list.open(fFileName.Data());
  std::ifstream list(fFileName.Data(),std::ifstream::in);
  while(!list.eof()){
    getline(list,line);
    if (line.compare("")!=0) { newFile = line; }
  }
  list.close();

  // If file has changed, analyze it
  if ( newFile != fDataFile ) {

    // Disable user buttons while we analyze the data
    fHoldButton->SetEnabled(kFALSE);
    fResumeButton->SetEnabled(kFALSE);
    fNextButton->SetEnabled(kFALSE);

    fDataFile = newFile;
    printf("- New data file found: %s\n",fDataFile.Data());

    // Reset all histograms
    HistoManager::GetInstance()->Reset();

    // Prepare the analyses
    ECALAnal* ecalA = new ECALAnal();
    TargetAnal* targetA = new TargetAnal();

    // Connect to raw events file
    TFile* fRawEv = new TFile(fDataFile);
    TTree* tRawEv = (TTree*)fRawEv->Get("RawEvents");
    TBranch* bRawEv = tRawEv->GetBranch("RawEvent");
    TRawEvent* rawEv = new TRawEvent();
    bRawEv->SetAddress(&rawEv);

    UInt_t nevt = tRawEv->GetEntries();
    printf("TTree RawEvents contains %d events\n",nevt);

    // Set number of events to read
    UInt_t ntoread = nevt;
    if (fNEvents && fNEvents<nevt) ntoread = fNEvents;
    printf("Reading the first %d events\n",ntoread);

    for(UInt_t iev=0;iev<ntoread;iev++){

      // Read event
      printf("Reading event %d\n",iev);
      bRawEv->GetEntry(iev);

      // Do ECAL analysis
      ecalA->SetEvent(rawEv);
      ecalA->AnalyzeCharge();
      //ecalA->AnalyzePosition();

      /*
      // Do Target analysis
      targetA->SetEvent(rawEv);
      targetA->AnalyzeCharge();
      targetA->AnalyzePosition();
      */

      // Clear event
      rawEv->Clear("C");

    }

    // Final cleanup
    delete rawEv;
    delete bRawEv;
    delete tRawEv;
    fRawEv->Close();
    delete fRawEv;
    delete ecalA;
    delete targetA;

    for ( std::vector<VOnlineMonitor*>::iterator it = fOnlineMonitorList.begin() ; it != fOnlineMonitorList.end(); ++it ) (*it)->UpdateAllCanvas();

    // We are done: re-enable user buttons
    fNextButton->SetEnabled(kTRUE);
    if (fCurrentStatus == 0) {
      fHoldButton->SetEnabled(kFALSE);
      fResumeButton->SetEnabled(kTRUE);
    } else {
      fHoldButton->SetEnabled(kTRUE);
      fResumeButton->SetEnabled(kFALSE);
    }

  } else {
    printf("- No new data\n");
  }

  // Repeat check after 10 seconds
  //printf("Setting timer to 10 secs\n");
  //fTimer->Start(10000,kTRUE);

}
