#include <iostream>
#include <TGraph.h>
#include <TGraphErrors.h>
#include "UserAnalysis.hh"

#include "ECalCalib.hh"
#include "NPoTAnalysis.hh" //MR
//#include "ETagAnalysis.hh" //MR
//#include "IsGGAnalysis.hh" //MR
#include "Is22GGAnalysis.hh" //MR
#include "SinglePhoton.hh" //MR single photon
#include "Is3GAnalysis.hh" //MR
#include "MCTruth.hh"     //MR

#include "HistoSvc.hh"
#include "TempCorr.hh"

UserAnalysis::UserAnalysis(TString cfgFile, Int_t verbose)
{
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating UserAnalysis\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  fHS = HistoSvc::GetInstance();
  fCfgParser    = new utl::ConfigParser((const std::string)cfgFile.Data());
  fECalCalib    = ECalCalib::GetInstance();
  fMCTruth      = MCTruth::GetInstance();

  //Physics analysis last reviewed by M. Raggi 05/22
  fNPoTAnalysis = new NPoTAnalysis(cfgFile,fVerbose);
  //  fIsGGAnalysis = new IsGGAnalysis(cfgFile,fVerbose);
  fIs22GGAnalysis = new Is22GGAnalysis(cfgFile,fVerbose);
  fSinglePhoton   = new SinglePhoton(cfgFile,fVerbose);
  //  fETagAnalysis   = new ETagAnalysis(cfgFile,fVerbose);
  //  fIs3GAnalysis = new Is3GAnalysis(cfgFile,fVerbose);
}

UserAnalysis::~UserAnalysis(){
  delete fCfgParser;
  delete fECalCalib;
  delete fNPoTAnalysis;
  //  delete fETagAnalysis;
  //  delete fIsGGAnalysis;
  delete fIs22GGAnalysis;
  delete fSinglePhoton;
  //  delete fIs3GAnalysis;
  delete fMCTruth;
}

Bool_t UserAnalysis::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing UserAnalysis\n");
  fEvent = event;
  InitHistos();
  fECalCalib->Init();

  if(fEvent->MCTruthEvent) fMCTruth->Init(fEvent);
  fNPoTAnalysis->Init(fEvent);
  //  fETagAnalysis->Init(fEvent);
  //  fIsGGAnalysis->Init(fEvent);  
  fIs22GGAnalysis->Init(fEvent);
  fSinglePhoton->Init(fEvent);
  //  fIs3GAnalysis->Init(fEvent);
  return true;
}

Bool_t UserAnalysis::InitHistos(){
  // This will go to the top directory in the output file
  fHS->BookHistoList("TOP","TestTOP",10,0.,10.);
  // No list defined: this will go the EXTRA directory in the output file
  fHS->BookHisto("Test",10,0.,10.);
  fHS->CreateList("MyHistos");
  fHS->BookHistoList("MyHistos","Trigger Mask",256,0.,256.);
  fHS->BookHistoList("MyHistos","Triggers",8,0.,8.);
  fHS->BookHistoList("MyHistos","NPoTs",500,0.,50000.);
  fHS->BookHisto2List("MyHistos","Test2D",10,0.,10.,10,0.,10.);
  return true;
}

#define ABS(x)  ((x) > 0 ? (x):(-x))

Bool_t UserAnalysis::Process(){
  Bool_t isMC     = false;
  Bool_t IsNPotOk = false;
  if (fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) isMC=true;

  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  fHS->FillHistoList("MyHistos","Trigger Mask",trigMask,1.);
  for (int i=0;i<8;i++) { 
    if (trigMask & (1 << i)) fHS->FillHistoList("MyHistos","Triggers",i,1.); 
  }

  //Cut on physics trigger Data Only ??
  if(fEvent->MCTruthEvent) fMCTruth->Process(); //MR 04/22
  IsNPotOk=fNPoTAnalysis->Process();
  //  if(!IsNPotOk && !isMC) return true; //Drops events with strange NPoT from target
  fECalCalib->Process(fEvent); 
  // cout<<"User Analysis NPOT "<<fNPoTAnalysis->GetNPoT()<<endl;
  if(!isMC){
    fECalCalib->SetEScale();
    //    cout<<"User Analysis NPOT "<<fNPoTAnalysis->GetNPoT()<<endl;
    //    fECalCalib->CorrectESlope();
    //    fECalCalib->FixPosition(); //need to change values into the structure.
  }
  
  //  fIsGGAnalysis->Process();
  //  fETagAnalysis->Process();
  fIs22GGAnalysis->Process();
  fSinglePhoton->Process();
  //  fIs3GAnalysis->Process();   
  return true;
}

Bool_t UserAnalysis::Finalize()
{
  if (fVerbose) printf("---> Finalizing UserAnalysis\n");
  if(fEvent->MCTruthEvent) fMCTruth->Finalize();
  fNPoTAnalysis->Finalize();
  //  fIsGGAnalysis->Finalize();
  fIs22GGAnalysis->Finalize();
  fSinglePhoton->Finalize();

  //  fETagAnalysis->Finalize();
  //  fIs3GAnalysis->Finalize();
//  // TGraph example
//  Double_t x[5] = {1.,2.,3.,4.,5.};
//  Double_t xe[5] = {.1,.1,.2,.2,.3};
//  Double_t y[5] = {2.,4.,6.,8.,10.};
//  Double_t ye[5] = {.1,.1,.2,.2,.3};
//  TGraph* g = new TGraph(5,x,y);
//  fHS->SaveTGraphList("MyHistos","TestTGraph",g);
//  TGraphErrors* ge = new TGraphErrors(5,x,y,xe,ye);
//  fHS->SaveTGraphList("MyHistos","TestTGraphErrors",ge);
  return true;
}
