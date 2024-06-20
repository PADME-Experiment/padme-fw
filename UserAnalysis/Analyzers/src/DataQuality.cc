#include "DataQuality.hh"
#include "TGraph.h"
#include "TString.h"


DataQuality* DataQuality::fInstance = 0;

DataQuality* DataQuality::GetInstance(){
  if ( fInstance == 0 ) { fInstance = new DataQuality(); }
  return fInstance;
}

DataQuality::~DataQuality(){
  delete fCfgParser;
}

Bool_t DataQuality::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing DataQuality\n");
  TString cfgFile = "config/UserAnalysis.conf";
  fHS = HistoSvc::GetInstance();
  fGeneralInfo = GeneralInfo::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
  
  fEvent = event;
  Int_t nRun = fEvent->RecoEvent->GetRunNumber(); //30000 vale solo per il 2022
  cout<<" Creating DataQuality Hystograms for Run Init "<< nRun<<endl;  
  
  fTimeBin = 5.; // sec
  fTimeBinCoarse = 60.; // sec

  fNTimeBins = ((int)(fGeneralInfo->GetRunStopTime()-fGeneralInfo->GetRunStartTime()))/fTimeBin; //check carefully this ratio because there are two long long int subtracted
  fNTimeBinsCoarse = ((int)(fGeneralInfo->GetRunStopTime()-fGeneralInfo->GetRunStartTime()))/fTimeBinCoarse; 

  const int nObservables = 5;
  TString obsNames[nObservables] = {"POT","ECalOverPOT","POTBunchLength","LGPed","POTTarg"};
  for (int i=0; i<5; i++){
    observable obsn;
    obsn.name = obsNames[i].Data();

    obsn.valueAvg = new Double_t[fNTimeBins];
    obsn.valueAvgCoarse = new Double_t[fNTimeBinsCoarse];

    obsn.nCounts = new Int_t[fNTimeBins];
    obsn.nCountsCoarse = new Int_t[fNTimeBinsCoarse];

    fObservables.push_back(obsn);
  }


  InitHistos(nRun);
  return true;
}

Bool_t DataQuality::InitHistos(Int_t nRun){
  // DataQuality directory will contain all histograms related to this analysis

  fHS->CreateList("DataQuality");
  fHS->CreateList("DataQualityLG");
  cout<<" Creating Hystograms for Run "<<nRun<<" "<<endl;    
  return true;
}

Bool_t DataQuality::Process(){
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
    fNPoTLG   = fEvent->LeadGlassRecoEvent->GetNPoTs();
    
    fNPoTLGCorr = 402.5*fNPoTLG*fGeneralInfo->GetLGCorr()/fGeneralInfo->GetBeamEnergy(); //DA VALUTARE
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

  fHS->FillHistoList("DataQuality","NPoT",fNPoT);
  fHS->FillHistoList("DataQualityLG","NPoTLG",fNPoTLG);
  fHS->FillHistoList("DataQualityLG","NPoTLGCorr",fNPoTLGCorr);
  fHS->FillHisto2List("DataQualityLG","NPoTLGCorrvsBunchID",fEvent->RecoEvent->GetEventNumber(),fNPoTLGCorr);
  fHS->FillHisto2List("DataQualityLG","BBQvsBunchID",fEvent->RecoEvent->GetEventNumber(),fEvent->LeadGlassRecoEvent->GetBunchBBQ());

 if(fNPoTLG>250) {
    fHS->FillHistoList("DataQualityLG","NPoTGood",fNPoTLG);
    fHS->FillHistoList("DataQualityLG","PoTDens",(float)fNPoTLG/(float)fNPoTBL);
    fHS->FillHistoList("DataQualityLG","LGCharge",fLGCharge);
    fHS->FillHistoList("DataQualityLG","LGPed",fLGPed);
    fHS->FillHistoList("DataQualityLG","BunchLenghtLG",fNPoTBL);
    fHS->FillHistoList("DataQualityLG","BunchLenghtLGvsBunchID",fEvent->RecoEvent->GetEventNumber(),fNPoTBL);
    fHS->FillHisto2List("DataQualityLG","LGPedvsBunchID",fEvent->RecoEvent->GetEventNumber(),fLGPed);
    fHS->FillHisto2List("DataQualityLG","LGPedvsLGCharge",fLGPed, fLGCharge);
  
  }

  fHS->FillHistoList("DataQualityLG","NPoTTarvsNPoTLG",fNPoTLG,fNPoT);
  fHS->FillHistoList("DataQualityLG","NPoTLGvsXTar",fXPos,fNPoTLG);
  if(fNPoT>5E3) {
    fHS->FillHistoList("DataQuality","XPos",fXPos);
    fHS->FillHistoList("DataQuality","YPos",fYPos);
  }

  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  if( !(trigMask & (1 << 0)) ){
    fHS->FillHistoList("DataQualityLG","NPoTNoPhys",fNPoTLG);
    fHS->FillHistoList("DataQuality","NPoTNoPhys",fNPoT);
    TotPoTNoPhys += fNPoT;
  }
  
  if( (trigMask & (1 << 0)) ) {
    fHS->FillHistoList("DataQualityLG","NPoTPhys",fNPoTLG);
    fHS->FillHistoList("DataQuality","NPoTPhys",fNPoT);
    TotPoTPhys += fNPoT;
    if(fNPoT>5E3) {
      fHS->FillHistoList("DataQuality","NPoTPhys5K",fNPoT);
      TotPoTOver5K += fNPoT;
    }
    // the actual value is 5 sigma need to check and move 
    if(fNPoT>15E3 && fNPoT<45E3) {
      fHS->FillHistoList("DataQuality","NPoTIsa",fNPoT);
      IsGoodNPoT = true;
    }
    if(fNPoT>30E3) TotPoTOver30K += fNPoT;
  }
  
  fCurrentRun = fNRun;
  Neve++;
  //  fEvtCount++;
  return IsGoodNPoT;
}

Bool_t DataQuality::Finalize()
{
  if(vNRun.size()==0 && vNPoTRun.size()==0){
    vNRun.push_back(fNRun);
    vNPoTRun.push_back(TotPoTRun);
  }
  TGraph* nPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vNPoT[0]);
  nPotVsTime->SetMarkerStyle(21);
  fHS->SaveTGraphList("DataQuality","NPotVsTime",nPotVsTime);

  //  TGraph* TotPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vTotPoT[0]);
  //  fHS->SaveTGraphList("DataQuality","TotPotVsTime",TotPotVsTime);

  TGraph* TotPotVsRun = new TGraph((Int_t)vNPoTRun.size(),&vNRun[0],&vNPoTRun[0]);
  fHS->SaveTGraphList("DataQuality","TotPotVsRun",TotPotVsRun);

  std::cout<<"TotPot        = "<<TotPoT<<std::endl;
  std::cout<<"TotPotPhys    = "<<TotPoTPhys<<std::endl;
  std::cout<<"TotPotNoPhys  = "<<TotPoTNoPhys<<std::endl;
  std::cout<<"TotPot Over30 = "<<TotPoTOver30K<<std::endl;
  std::cout<<"TotPot Over5  = "<<TotPoTOver5K<<std::endl;

  if (fVerbose) printf("---> Finalizing DataQuality\n");
  return true;
}
