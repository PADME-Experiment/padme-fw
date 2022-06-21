#include <iostream>

#include <TGraph.h>
#include <TGraphErrors.h>

#include "UserAnalysis.hh"
#include "NPoTAnalysis.hh"
#include "IsGGAnalysis.hh" //MR
#include "Is3GAnalysis.hh" //MR
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
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
  fNPoTAnalysis = new NPoTAnalysis(cfgFile,fVerbose);
  fIsGGAnalysis = new IsGGAnalysis(cfgFile,fVerbose);
  //  fIs3GAnalysis = new Is3GAnalysis(cfgFile,fVerbose);
}

UserAnalysis::~UserAnalysis(){
  delete fCfgParser;
  delete fNPoTAnalysis;
  delete fIsGGAnalysis;
  //  delete fIs3GAnalysis;
}

Bool_t UserAnalysis::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing UserAnalysis\n");
  fEvent = event;
  InitHistos();
  fNPoTAnalysis->Init(fEvent);
  fIsGGAnalysis->Init(fEvent);
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

  // Histograms for MCTruth information
  fHS->CreateList("MCTruth");
  fHS->BookHistoList("MCTruth","Vertices",10,0.,10.); // Number of vertices in event
  fHS->BookHistoList("MCTruth","Vertex Type",10,0.,10.); // 0:eBrem - 1:eIoni - 2:annihil - 9:other
  fHS->BookHistoList("MCTruth","Bremstrahlung Gamma Energy",1000,0.,1000.);

  return true;
}

#define ABS(x)  ((x) > 0 ? (x):(-x))

Bool_t UserAnalysis::Process(){

  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  fHS->FillHistoList("MyHistos","Trigger Mask",trigMask,1.);
  for (int i=0;i<8;i++) { if (trigMask & (1 << i)) fHS->FillHistoList("MyHistos","Triggers",i,1.); }
  fNPoTAnalysis->Process();
  //  if(fNPoTAnalysis->GetNPoT()<5000.) return true;   //cut on events with less than 5000 POTs //Commented by Beth 20/9/21 for X17 analysis
  fIsGGAnalysis->Process();
  //  fIs3GAnalysis->Process();
  //std::cout<<"E Ecal "<<fIsGGAnalysis->GetETotECal()<<std::endl;

  /*
  for(int ipv = 0;ipv <  fEvent->PVetoRecoEvent->GetNHits(); ipv++) {
    double tPv = fEvent->PVetoRecoEvent->Hit(ipv)->GetTime();
    int chPV = fEvent->PVetoRecoEvent->Hit(ipv)->GetChannelId();

    //Correct the PVeto timing:
    // linear inyterpolation 
    //tPv = tPv - (30.9219 + 0.137262*chPV);
    // channel by channel 
    tPv = tPv - timeOffsetPVeto[chPV];
    fEvent->PVetoRecoEvent->Hit(ipv)->SetTime(tPv);
  }
  */

  /*
  //Calo-Calo checks
  for(int ical = 0;ical <  fEvent->ECalRecoCl->GetNElements(); ical++) {
    double eECal =  fEvent->ECalRecoCl->Element(ical)->GetEnergy();
    double tECal =  fEvent->ECalRecoCl->Element(ical)->GetTime();
    TVector3 pos1 =  fEvent->ECalRecoCl->Element(ical)->GetPosition();
    fHS->FillHisto("ECalClEnergy",eECal,1);
  }
  */

  // MCTruth analysis
  if (fEvent->MCTruthEvent) {
    fHS->FillHistoList("MCTruth","Vertices",fEvent->MCTruthEvent->GetNVertices(),1.);
    if (fEvent->MCTruthEvent->GetNVertices()>0) {
      for(Int_t iV = 0; iV<fEvent->MCTruthEvent->GetNVertices(); iV++) {
	TMCVertex* mcVtx = fEvent->MCTruthEvent->Vertex(iV);
	if (mcVtx->GetProcess() == "eBrem") {
	  fHS->FillHistoList("MCTruth","Vertex Type",0,1.);
	  for(Int_t iO = 0; iO<mcVtx->GetNParticleOut(); iO++) {
	    TMCParticle* mcOPart = mcVtx->ParticleOut(iO);
	    if (mcOPart->GetPDGCode() == 22) {
	      fHS->FillHistoList("MCTruth","Bremstrahlung Gamma Energy",mcOPart->GetEnergy(),1.);
	    }
	  }
	} else if (mcVtx->GetProcess() == "eIoni") {
	  fHS->FillHistoList("MCTruth","Vertex Type",1,1.);
	} else if (mcVtx->GetProcess() == "annihil") {
	  fHS->FillHistoList("MCTruth","Vertex Type",2,1.);
	} else {
	  fHS->FillHistoList("MCTruth","Vertex Type",9,1.);
	}
      }
    }
  }

  return true;
}

Bool_t UserAnalysis::Finalize()
{

  if (fVerbose) printf("---> Finalizing UserAnalysis\n");

  fNPoTAnalysis->Finalize();
  fIsGGAnalysis->Finalize();
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
