// Written by Beth Long 13/6/22
#include "HitCharacteristicsAnalysis.hh"
#include "TRecoVHit.hh"
#include "TLorentzVector.h"
#include "PadmeVRecoConfig.hh"

HitCharacteristicsAnalysis::HitCharacteristicsAnalysis(TString cfgFile, Int_t verbose)
{
  //  Neve=0;
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating HitCharacteristicsAnalysis\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  fHS = HistoSvc::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
  fConfig = new PadmeVRecoConfig(fCfgParser,"PadmeReconstructionConfiguration");
}

HitCharacteristicsAnalysis::~HitCharacteristicsAnalysis(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t HitCharacteristicsAnalysis::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing HitCharacteristicsAnalysis\n");
  fEvent = event;
  InitHistos();
  fSwimmerInit=0;
  return true;
}

Bool_t HitCharacteristicsAnalysis::InitHistos(){  
  fHS->CreateList("PVetoHits");
  fHS->CreateList("EVetoHits");
  
  //Collinear particles channelID->energy
  fHS->BookHistoList("PVetoHits","hPVetoHitEnergy",600,0,6);
  fHS->BookHistoList("EVetoHits","hEVetoHitEnergy",600,0,6);
 
  fHS->BookHistoList("PVetoHits","hPVetoHitCh",90,0,90);
  fHS->BookHistoList("EVetoHits","hEVetoHitCh",96,0,96);
 
  return true;
}

Bool_t HitCharacteristicsAnalysis::Process(){
  //  std::cout<<"processing brem"<<std::endl;
  //Check if is MC or data
  Bool_t isMC = false;
  if (fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) {
    isMC=true;
    //    std::cout<<"input data are simulatetd "<<std::endl;
  }
  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();

  //Cut on physics trigger Data Only
  if( !(trigMask & (1 << 0)) && isMC==false) return false;

  //Number of PVeto hits per event
  Int_t NPVetoHit = fEvent->PVetoRecoEvent->GetNHits();

  //Number of EVeto hits per event
  Int_t NEVetoHit = fEvent->EVetoRecoEvent->GetNHits();
  
  //time
  double tHitPVeto;
  double tHitEVeto;

  //channel
  int chHitPVeto;
  int chHitEVeto;

  //energy
  double enHitPVeto;
  double enHitEVeto;
      
  for(int kk = 0; kk<NPVetoHit;kk++){
    
    //import PVeto variables
    tHitPVeto     =  fEvent->PVetoRecoEvent->Hit(kk)->GetTime();
    chHitPVeto    =  fEvent->PVetoRecoEvent->Hit(kk)->GetChannelId();
    enHitPVeto    =  fEvent->PVetoRecoEvent->Hit(kk)->GetEnergy();
    
    fHS->FillHistoList("PVetoHits","hPVetoHitEnergy",enHitPVeto);
    fHS->FillHistoList("PVetoHits","hPVetoHitCh",chHitPVeto);
    //    fHS->FillHistoList("PVetoHits","hPVetoHitChVsHitEnergy",chHitPVeto,enHitPVeto);
  }

  for(int mm = 0; mm<NEVetoHit;mm++){

    //import EVeto variables
    tHitEVeto     =  fEvent->EVetoRecoEvent->Hit(mm)->GetTime();
    chHitEVeto    =  fEvent->EVetoRecoEvent->Hit(mm)->GetChannelId();
    enHitEVeto    =  fEvent->EVetoRecoEvent->Hit(mm)->GetEnergy();

    fHS->FillHistoList("EVetoHits","hEVetoHitEnergy",enHitEVeto);
    fHS->FillHistoList("EVetoHits","hEVetoHitCh",chHitEVeto);
    //    fHS->FillHistoList("EVetoHits","hEVetoHitChVsHitEnergy",chHitEVeto,enHitEVeto);
  }

  return true;
}

Bool_t HitCharacteristicsAnalysis::Finalize()
{
 // TGraph* nPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vNPoT[0]);
 // fHS->SaveTGraphList("GGAnalysis","NPotVsTime",nPotVsTime);
 
 if (fVerbose) printf("---> Finalizing HitCharacteristicsAnalysis\n");
  return true;
}


