// Written by E. Leonardi v
// Last modified by M. Raggi 2/08/2021
#include "NPoTAnalysis.hh"
#include "TGraph.h"
#include "TString.h"


NPoTAnalysis* NPoTAnalysis::fInstance = 0;

NPoTAnalysis* NPoTAnalysis::GetInstance(){

  if ( fInstance == 0 ) { fInstance = new NPoTAnalysis(); }
  return fInstance;
}

NPoTAnalysis::~NPoTAnalysis(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t NPoTAnalysis::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing NPoTAnalysis\n");
  TString cfgFile = "config/UserAnalysis.conf";
  fHS = HistoSvc::GetInstance();
  fGeneralInfo = GeneralInfo::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
  
  fEvent = event;
  cout<<" Creating Hystograms for Run Init "<<fNRun<<" "<<endl;  
  fNRun = fEvent->RecoEvent->GetRunNumber(); //30000 vale solo per il 2022
  InitHistos();
  Neve=0;
  fCurrentRun=-1;
  return true;
}

Bool_t NPoTAnalysis::InitHistos(){
  // NPoTAnalysis directory will contain all histograms related to this analysis

  fHS->CreateList("NPoTAnalysis");
  fHS->CreateList("NPoTAnalysisLG");
  cout<<" Creating Hystograms for Run "<<fNRun<<" "<<endl;  

  Int_t Min_POT=-500;
  Int_t Max_POT=10500;
  Int_t NBinsPOT= (Max_POT-Min_POT)/10; //every 10 POT 

  // Target related quantities 
  fHS->BookHistoList("NPoTAnalysis","NPoT",600,-1000.,59000.);
  fHS->BookHistoList("NPoTAnalysis","NPoTNoPhys",600,-1000.,59000.);
  fHS->BookHistoList("NPoTAnalysis","NPoTPhys",600,-1000.,59000.);
  fHS->BookHistoList("NPoTAnalysis","NPoTPhys5K",600,-1000.,59000.);
  fHS->BookHistoList("NPoTAnalysis","NPoTIsa",600,-1000.,59000.);
  fHS->BookHistoList("NPoTAnalysis","XPos",600,-15.,15.);
  fHS->BookHistoList("NPoTAnalysis","YPos",600,-15.,15.);

  // Lead Glass related quantities 

  fHS->BookHistoList("NPoTAnalysisLG","NPoTLG",NBinsPOT,Min_POT,Max_POT);
  fHS->BookHistoList("NPoTAnalysisLG","NPoTLGCorr",NBinsPOT,Min_POT,Max_POT);
  fHS->BookHisto2List("NPoTAnalysisLG","NPoTLGCorrvsBunchID",20000,0,5e7, NBinsPOT,Min_POT,Max_POT);
  fHS->BookHistoList("NPoTAnalysisLG","NPoTNoPhys",NBinsPOT,Min_POT,Max_POT);
  fHS->BookHistoList("NPoTAnalysisLG","NPoTPhys",NBinsPOT,Min_POT,Max_POT);
  fHS->BookHistoList("NPoTAnalysisLG","NPoTGood",NBinsPOT,Min_POT,Max_POT);
  fHS->BookHistoList("NPoTAnalysisLG","PoTDens",200,0.,50.);
  fHS->BookHistoList("NPoTAnalysisLG","LGCharge",2000,0.,2000.);
  fHS->BookHistoList("NPoTAnalysisLG","LGPed",500,3500.,4000);
  fHS->BookHisto2List("NPoTAnalysisLG","LGPedvsBunchID",20000,0,5e7,500,3500.,4000);
  fHS->BookHisto2List("NPoTAnalysisLG","LGPedvsLGCharge",500,3500.,4000, 2000,0.,2000.);
  fHS->BookHistoList("NPoTAnalysisLG","BunchLenghtLG",500,0.,500.);
  fHS->BookHisto2List("NPoTAnalysisLG","BunchLenghtLGvsBunchID",20000,0,5e7,500,0.,500.);
  fHS->BookHisto2List("NPoTAnalysisLG","BBQvsBunchID",20000,0,5e7,500,0.,500.);
  fHS->BookHisto2List("NPoTAnalysisLG","NPoTTarvsNPoTLG",NBinsPOT,Min_POT,Max_POT,NBinsPOT,Min_POT,Max_POT);
  fHS->BookHisto2List("NPoTAnalysisLG","NPoTLGvsXTar",600,-15.,15.,NBinsPOT,Min_POT,Max_POT);
  return true;
}

Bool_t NPoTAnalysis::Process(){
  Bool_t IsGoodNPoT = false;
  fNPoT = 0;
  if(fEvent->TargetRecoBeam!=0){
    fNRun = fEvent->RecoEvent->GetRunNumber(); //30000 vale solo per il 2022
    fNPoT = fEvent->TargetRecoBeam->getnPOT();
    fXPos = fEvent->TargetRecoBeam->getX();
    fYPos = fEvent->TargetRecoBeam->getY();
    fTimeStamp=fEvent->RecoEvent->GetEventTime(); 
    //  cout<<" X tar "<<fXPos<<" "<<fYPos<<endl;
    TotPoT    +=fNPoT;    
    TotPoTRun +=fNPoT;
    vTotPoT.push_back(TotPoT);
  }
   fNPoTLG =0.;
   fNPoTLGCorr =0;
   fNPoTBL =0.;
   Double_t fLGPed =0.;
   Double_t fLGCharge=0.;

  if(fEvent->LeadGlassRecoEvent!=0){
    fNPoTLG   =  402.5*fEvent->LeadGlassRecoEvent->GetNPoTs()/fGeneralInfo->GetBeamEnergy();
    fNPoTLGCorr = 402.5*fNPoTLG/(fGeneralInfo->GetBeamEnergy()*fGeneralInfo->GetLGCorr()); //DA VALUTARE
    fNPoTBL   = fEvent->LeadGlassRecoEvent->GetBunchLength();
    fLGCharge = fEvent->LeadGlassRecoEvent->GetTotalCharge();
    fLGPed    = fEvent->LeadGlassRecoEvent->GetPedestal();
    TotPoTLG  +=fNPoTLG;
    TotPoTRun +=fNPoTLG;
    vTotPoTLG.push_back(TotPoTLG);
  }
  //  cout<<" NPOT "<<fNPoTLG<<" "<<fNPoT<<" BL "<<fNPoTBL<<endl;


  if(Neve%250 ==0){
    vNPoT.push_back(fNPoTLG);
    vNEvt.push_back(fTimeStamp);
  }
  if(fCurrentRun != -1 && fCurrentRun != fNRun){
    vNRun.push_back(fNRun);
    vNPoTRun.push_back(TotPoTRun);
    TotPoTRun=0;
    cout<<"Changing RUN "<<fNRun<<" "<<fCurrentRun<<endl;
  }

  fHS->FillHistoList("NPoTAnalysis","NPoT",fNPoT);
  fHS->FillHistoList("NPoTAnalysisLG","NPoTLG",fNPoTLG);
  fHS->FillHistoList("NPoTAnalysisLG","NPoTLGCorr",fNPoTLGCorr);
  fHS->FillHisto2List("NPoTAnalysisLG","NPoTLGCorrvsBunchID",fEvent->RecoEvent->GetEventNumber(),fNPoTLGCorr);
  fHS->FillHisto2List("NPoTAnalysisLG","BBQvsBunchID",fEvent->RecoEvent->GetEventNumber(),fEvent->LeadGlassRecoEvent->GetBunchBBQ());

 if(fNPoTLG>250) {
    fHS->FillHistoList("NPoTAnalysisLG","NPoTGood",fNPoTLG);
    fHS->FillHistoList("NPoTAnalysisLG","PoTDens",(float)fNPoTLG/(float)fNPoTBL);
    fHS->FillHistoList("NPoTAnalysisLG","LGCharge",fLGCharge);
    fHS->FillHistoList("NPoTAnalysisLG","LGPed",fLGPed);
    fHS->FillHistoList("NPoTAnalysisLG","BunchLenghtLG",fNPoTBL);
    fHS->FillHistoList("NPoTAnalysisLG","BunchLenghtLGvsBunchID",fEvent->RecoEvent->GetEventNumber(),fNPoTBL);
    fHS->FillHisto2List("NPoTAnalysisLG","LGPedvsBunchID",fEvent->RecoEvent->GetEventNumber(),fLGPed);
    fHS->FillHisto2List("NPoTAnalysisLG","LGPedvsLGCharge",fLGPed, fLGCharge);
  
  }

  fHS->FillHistoList("NPoTAnalysisLG","NPoTTarvsNPoTLG",fNPoTLG,fNPoT);
  fHS->FillHistoList("NPoTAnalysisLG","NPoTLGvsXTar",fXPos,fNPoTLG);
  if(fNPoT>5E3) {
    fHS->FillHistoList("NPoTAnalysis","XPos",fXPos);
    fHS->FillHistoList("NPoTAnalysis","YPos",fYPos);
  }

  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  if( !(trigMask & (1 << 0)) ){
    fHS->FillHistoList("NPoTAnalysisLG","NPoTNoPhys",fNPoTLG);
    fHS->FillHistoList("NPoTAnalysis","NPoTNoPhys",fNPoT);
    TotPoTNoPhys += fNPoT;
  }
  
  if( (trigMask & (1 << 0)) ) {
    fHS->FillHistoList("NPoTAnalysisLG","NPoTPhys",fNPoTLG);
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
  //  fEvtCount++;
  return IsGoodNPoT;
}

Bool_t NPoTAnalysis::Finalize()
{
  if(vNRun.size()==0 && vNPoTRun.size()==0){
    vNRun.push_back(fNRun);
    vNPoTRun.push_back(TotPoTRun);
  }
  TGraph* nPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vNPoT[0]);
  nPotVsTime->SetMarkerStyle(21);
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
