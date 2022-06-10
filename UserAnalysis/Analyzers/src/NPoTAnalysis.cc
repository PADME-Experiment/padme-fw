// Written by E. Leonardi 
// Last modified by M. Raggi 2/08/2021
#include "NPoTAnalysis.hh"
#include "TGraph.h"

NPoTAnalysis::NPoTAnalysis(TString cfgFile, Int_t verbose)
{
  Neve=0;
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating NPoTAnalysis\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  fHS = HistoSvc::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
}

NPoTAnalysis::~NPoTAnalysis(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t NPoTAnalysis::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing NPoTAnalysis\n");
  fEvent = event;
  InitHistos();
  Neve=0;
  fCurrentRun=-1;
  return true;
}

Bool_t NPoTAnalysis::InitHistos(){
  // NPoTAnalysis directory will contain all histograms related to this analysis
  fHS->CreateList("NPoTAnalysis");
  fHS->BookHistoList("NPoTAnalysis","NPoT",600,-1000.,59000.);
  fHS->BookHistoList("NPoTAnalysis","NPoTNoPhys",600,-1000.,59000.);
  fHS->BookHistoList("NPoTAnalysis","NPoTPhys",600,-1000.,59000.);
  fHS->BookHistoList("NPoTAnalysis","NPoTPhys5K",600,-1000.,59000.);
  fHS->BookHistoList("NPoTAnalysis","NPoTIsa",600,-1000.,59000.);

  fHS->BookHistoList("NPoTAnalysis","XPos",600,-15.,15.);
  fHS->BookHistoList("NPoTAnalysis","YPos",600,-15.,15.);

  return true;
}

Bool_t NPoTAnalysis::Process(){
  Bool_t IsGoodNPoT = false;
  fNRun = fEvent->RecoEvent->GetRunNumber(); //30000 vale solo per il 2022
  fNPoT = fEvent->TargetRecoBeam->getnPOT();
  fXPos = fEvent->TargetRecoBeam->getX();
  fYPos = fEvent->TargetRecoBeam->getY();
  fTimeStamp=fEvent->RecoEvent->GetEventTime(); 
  //  cout<<" X tar "<<fXPos<<" "<<fYPos<<endl;

  TotPoT+=fNPoT;
  TotPoTRun+=fNPoT;

  vNPoT.push_back(fNPoT);
  vTotPoT.push_back(TotPoT);
  vNEvt.push_back(fTimeStamp);

  if(fCurrentRun != -1 && fCurrentRun != fNRun){
    vNRun.push_back(fNRun);
    vNPoTRun.push_back(TotPoTRun);
    TotPoTRun=0;
    cout<<"Changing RUN "<<fNRun<<" "<<fCurrentRun<<endl;
  }

  fHS->FillHistoList("NPoTAnalysis","NPoT",fNPoT);
  if(fNPoT>5E3) {
    fHS->FillHistoList("NPoTAnalysis","XPos",fXPos);
    fHS->FillHistoList("NPoTAnalysis","YPos",fYPos);
  }

  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  if( !(trigMask & (1 << 0)) ){
    fHS->FillHistoList("NPoTAnalysis","NPoTNoPhys",fNPoT);
    TotPoTNoPhys += fNPoT;
  }
  
  if( (trigMask & (1 << 0)) ) {
    fHS->FillHistoList("NPoTAnalysis","NPoTPhys",fNPoT);
    TotPoTPhys += fNPoT;
    if(fNPoT>5E3) {
      fHS->FillHistoList("NPoTAnalysis","NPoTPhys5K",fNPoT);
      TotPoTOver5K += fNPoT;
    }
    // the actual value is 5 sigma need to check and move 
    if(fNPoT>15E3 && fNPoT<45E3) {
      fHS->FillHistoList("NPoTAnalysis","NPoTIsa",fNPoT);
      IsGoodNPoT = true;
    }
    if(fNPoT>30E3) TotPoTOver30K += fNPoT;
  }
  
  fCurrentRun = fNRun;
  Neve++;
  return IsGoodNPoT;
}

Bool_t NPoTAnalysis::Finalize()
{
  if(vNRun.size()==0 && vNPoTRun.size()==0){
    vNRun.push_back(fNRun);
    vNPoTRun.push_back(TotPoTRun);
  }
  TGraph* nPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vNPoT[0]);
  fHS->SaveTGraphList("NPoTAnalysis","NPotVsTime",nPotVsTime);

  //  TGraph* TotPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vTotPoT[0]);
  //  fHS->SaveTGraphList("NPoTAnalysis","TotPotVsTime",TotPotVsTime);

  TGraph* TotPotVsRun = new TGraph((Int_t)vNPoTRun.size(),&vNRun[0],&vNPoTRun[0]);
  fHS->SaveTGraphList("NPoTAnalysis","TotPotVsRun",TotPotVsRun);

  std::cout<<"TotPot        = "<<TotPoT<<std::endl;
  std::cout<<"TotPotPhys    = "<<TotPoTPhys<<std::endl;
  std::cout<<"TotPotNoPhys  = "<<TotPoTNoPhys<<std::endl;
  std::cout<<"TotPot Over30 = "<<TotPoTOver30K<<std::endl;
  std::cout<<"TotPot Over5  = "<<TotPoTOver5K<<std::endl;

  if (fVerbose) printf("---> Finalizing NPoTAnalysis\n");
  return true;
}
