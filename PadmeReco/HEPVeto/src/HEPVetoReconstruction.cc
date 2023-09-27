// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "HEPVetoReconstruction.hh"

#include "THEPVetoMCEvent.hh"
#include "THEPVetoMCHit.hh"
#include "THEPVetoMCDigi.hh"
#include "DigitizerChannelReco.hh"
//#include "DigitizerChannelSAC.hh"
#include "PadmeReconstruction.hh"
#include "HEPVetoCalibration.hh"
#include "HEPVetoGeometry.hh"
#include "HEPVetoSimpleClusterization.hh"
#include "TH2F.h"
//#include "TRandom2.h"
#include <time.h>



HEPVetoReconstruction::HEPVetoReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "HEPVeto", ConfigFileName)
{
  //fRecoEvent = new TRecoHEPVetoEvent();
  //ParseConfFile(ConfigFileName);
  fChannelReco = new DigitizerChannelReco();
  fChannelCalibration = new HEPVetoCalibration();
  fClusterization = new HEPVetoSimpleClusterization();
  //fChannelReco = new DigitizerChannelSAC();
  fTriggerProcessor = new PadmeVTrigger();
  fGeometry = new HEPVetoGeometry();

  random = new TRandom2();    
  gRandom->SetSeed(time(NULL));

  // configurable parameters 
  fSigmaNoiseForMC         = (Double_t)fConfig->GetParOrDefault("RECO", "SigmaNoiseForMC", .4);
  fHEPVetoDigiTimeWindow   = (Double_t)fConfig->GetParOrDefault("RECO", "DigitizationTimeWindowForMC", 17.);

  
}




void HEPVetoReconstruction::HistoInit(){
  AddHisto("HEPVetoOccupancy",new TH1F("HEPVetoOccupancy","HEPVeto Occupancy",32,0.0,32.0));
  AddHisto("HEPVetoTime",new TH1F("HEPVetoTime","HEPVeto Time",400,0.0,400.0));
  AddHisto("HEPVetoMultiplicity",new TH1F("HEPVetoMultiplicity","Number of hits in the HEPVeto per event", 100, 0.0,100.0));
  AddHisto("HEPVetoTimeVsPVetoTime",new TH2F("HEPVetoTimeVsPVetoTime","HEPVeto Time vs PVetoTime",100,100.0,500.0, 100,100.0,500.0));
 

  
  AddHisto("TriggerTimeDiffECalSAC"  , new TH1F("TriggerTimeDiffECalSAC", "TriggerTimeDiffECalSAC",1000, -200, 200));
  AddHisto("TriggerTimeDiffECalPVeto", new TH1F("TriggerTimeDiffECalPVeto", "TriggerTimeDiffECalPVeto",1000, -200, 200));
  AddHisto("TriggerTimeDiffPVetoSAC" , new TH1F("TriggerTimeDiffPVetoSAC", "TriggerTimeDiffPVetoSAC",1000, -200, 200));
  AddHisto("TriggerTimeDiffECalECal" , new TH1F("TriggerTimeDiffECalECal", "TriggerTimeDiffECalECal",1000, -100, 100));

  


  AddHisto("TimeDiffPVetoSAC" , new TH1F("TimeDiffPVetoSAC", "TimeDiffPVetoSAC",2000, -200, 200));
  AddHisto("TimeDiffHEPVetoSAC" , new TH1F("TimeDiffHEPVetoSAC", "TimeDiffHEPVetoSAC",2000, -200, 200));
  AddHisto("TimeDiffECalPVeto" , new TH1F("TimeDiffECalPVeto", "TimeDiffECalPVeto",2000, -200, 200));
  AddHisto("TimeDiffECalECal" , new TH1F("TimeDiffECalECal", "TimeDiffECalECal",2000, -200, 200));
  AddHisto("TimeDiffSACSAC" , new TH1F("TimeDiffSACSAC", "TimeDiffSACSAC",2000, -100, 100));
  AddHisto("ECalTimeVsECalTime",new TH2F("ECalTimeVsECalTime","ECalTimeVsECalTime",
					 2000,-1000.0,1000.0, 2000,-1000.0,1000.0));

}


HEPVetoReconstruction::~HEPVetoReconstruction()
{;}

// void HEPVetoReconstruction::Init(PadmeVReconstruction* MainReco)
// {

//   //common part for all the subdetectors
//   PadmeVReconstruction::Init(MainReco);

// }

// Read HEPVeto reconstruction parameters from a configuration file
// void HEPVetoReconstruction::ParseConfFile(TString ConfFileName) {

//   std::ifstream confFile(ConfFileName.Data());
//   if (!confFile.is_open()) {
//     perror(ConfFileName);
//     exit(1);
//   }

//   TString Line;
//   while (Line.ReadLine(confFile)) {
//     if (Line.BeginsWith("#")) continue;
//   }
//   confFile.close();
// }

/*
TRecoVEvent * HEPVetoReconstruction::ProcessEvent(TDetectorVEvent* tEvent, Event* tGenEvent)
{
  //common part for all the subdetectors 
  PadmeVReconstruction::ProcessEvent(tEvent, tGenEvent);

  THEPVetoEvent* tHEPVetoEvent = (THEPVetoEvent*)tEvent;
  const TClonesArray& Digis = (*(tHEPVetoEvent->GetHits()));
  return fRecoEvent;
}
*/

/* for debugging only 
void HEPVetoReconstruction::ProcessEvent(TMCVEvent* tEvent, TMCEvent* tMCEvent)
{
  PadmeVReconstruction::ProcessEvent(tEvent,tMCEvent);
  THEPVetoMCEvent* tHEPVetoEvent = (THEPVetoMCEvent*)tEvent;
  std::cout << "--- HEPVetoReconstruction --- run/event/#hits/#digi " << tHEPVetoEvent->GetRunNumber() << " " << tHEPVetoEvent->GetEventNumber() << " " << tHEPVetoEvent->GetNHits() << " " << tHEPVetoEvent->GetNDigi() << std::endl;
  for (Int_t iH=0; iH<tHEPVetoEvent->GetNHits(); iH++) {
    THEPVetoMCHit* hit = (THEPVetoMCHit*)tHEPVetoEvent->Hit(iH);
    hit->Print();
  }
  for (Int_t iD=0; iD<tHEPVetoEvent->GetNDigi(); iD++) {
    THEPVetoMCDigi* digi = (THEPVetoMCDigi*)tHEPVetoEvent->Digi(iD);
    digi->Print();
  }
}
*/

// void HEPVetoReconstruction::EndProcessing()
// {;}


void HEPVetoReconstruction::ConvertMCDigitsToRecoHits(TMCVEvent* tEvent,TMCEvent* tMCEvent) {

  if (tEvent==NULL) return;
  for(Int_t i=0; i < fHits.size(); i++) delete fHits[i];
  fHits.clear();
  // MC to reco hits
  for (Int_t i=0; i<tEvent->GetNDigi(); ++i) {
    TMCVDigi* digi = tEvent->Digi(i);
    //TRecoVHit *Hit = new TRecoVHit(digi);

    Int_t    digiCh = digi->GetChannelId();
    //digit Id increases with decreasing z; for recoHits chId increases with increasing z 
    if (digiCh<16) digiCh = 15-digiCh;
    else  digiCh = 47-digiCh;

    Double_t digiT  = digi->GetTime();
    Double_t digiE  = digi->GetEnergy();
    Bool_t toBeMerged = false;
    // merge digits in the same channel closer in time than a configurable parameter (fPVetoDigiTimeWindow){
    if (fHEPVetoDigiTimeWindow > 0) {
      for (unsigned int ih=0; ih<fHits.size(); ++ih)
	{
	  if (fHits[ih]->GetChannelId() != digiCh) continue;
	  if (fabs(fHits[ih]->GetTime()/fHits[ih]->GetEnergy()-digiT)<fHEPVetoDigiTimeWindow)
	    {
	      toBeMerged = true;
	      // this digit must be merged with a previously defined recoHit
	      fHits[ih]->SetEnergy(fHits[ih]->GetEnergy() + digiE);
	      fHits[ih]->SetTime(fHits[ih]->GetTime() + digiE*digiT);
	    }
	}
    }
    if (!toBeMerged)
      {
	TRecoVHit *Hit = new TRecoVHit();
	Hit->SetChannelId(digiCh);
	Hit->SetEnergy   (digiE);
	Hit->SetTime     (digiT*digiE);
	Hit->SetPosition (TVector3(0.,0.,0.)); 
	fHits.push_back(Hit);
      }
  }
  // last loop to correct the time 
  TRecoVHit *Hit;
  Double_t Noise=0.;
  for (unsigned int ih=0; ih<fHits.size(); ++ih)
    {
      Hit = fHits[ih];
      Hit->SetTime(Hit->GetTime()/Hit->GetEnergy());

      if (fSigmaNoiseForMC >0.0001) {
	Noise=random->Gaus(0.,fSigmaNoiseForMC);   
	Hit->SetEnergy(Hit->GetEnergy()+Noise);
      }
    }
    // end of merge digits in the same channel closer in time than a configurable parameter (fPVetoDigiTimeWindow){
  return;


  /*
  if (tEvent==NULL) return;
  random = new TRandom2();    
  gRandom->SetSeed(time(NULL));
  fHits.clear();
  Int_t chId=0;
  // MC to reco hits
  for (Int_t i=0; i<tEvent->GetNDigi(); ++i) {
    TMCVDigi* digi = tEvent->Digi(i);
    //TRecoVHit *Hit = new TRecoVHit(digi);
    TRecoVHit *Hit = new TRecoVHit();
    chId = digi->GetChannelId();
    //digit Id increases with decreasing z; for recoHits chId increases with increasing z 
    if (chId<16) chId = 15-chId;
    else  chId = 47-chId;
    Hit->SetChannelId(chId);
    Double_t sigma = 0.4;
    Double_t Noise=random->Gaus(0.,sigma); 
    Hit->SetEnergy(digi->GetEnergy()+Noise);
    //Hit->SetEnergy(digi->GetEnergy());
    Hit->SetTime(digi->GetTime());
    Hit->SetPosition(TVector3(0.,0.,0.)); 
    fHits.push_back(Hit);
  }
  */

}


void HEPVetoReconstruction::AnalyzeEvent(TRawEvent* rawEv){

  
  vector<TRecoVHit *> &Hits  = GetRecoHits();
  
  UChar_t nBoards = rawEv->GetNADCBoards();
  
  TADCBoard* ADC;
  
  GetHisto("HEPVetoMultiplicity")->Fill(Hits.size());
  for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
    //if( Hits[iHit1]->GetTime() > 10.) {
      //Real hit
      GetHisto("HEPVetoOccupancy")->Fill(Hits[iHit1]->GetChannelId());
      GetHisto("HEPVetoTime")->Fill(Hits[iHit1]->GetTime());
    //}
  }

  PadmeReconstruction* MainReco = (PadmeReconstruction*  ) fMainReco;
  
  
  PadmeVReconstruction* PVetoReco = NULL;
  PVetoReco = MainReco->FindReco("PVeto") ;
  if (PVetoReco==NULL)                  return;
  if (MainReco->FindReco("ECal")==NULL) return;  
  if (MainReco->FindReco("SAC")==NULL)  return;  
  
  vector<TRecoVHit *> &PVetoHits  = PVetoReco->GetRecoHits();
  
  for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
    for(unsigned int iHit2 = 0; iHit2 < PVetoHits.size();++iHit2) {
      if ( PVetoHits[iHit2]->GetChannelId() > 80 ) {
          (  (TH2F *) GetHisto("HEPVetoTimeVsPVetoTime"))  ->Fill(Hits[iHit1]->GetTime(),PVetoHits[iHit2]->GetTime());
      }
      
    }
  }
 

  //Common treatment of multiple trigger infos.... a PATCH!!!!!

  PadmeVTrigger *ECalTrig  = (MainReco->FindReco("ECal") )->GetTriggerProcessor();
  PadmeVTrigger *PVetoTrig = (MainReco->FindReco("PVeto"))->GetTriggerProcessor();
  PadmeVTrigger *SACTrig   = (MainReco->FindReco("SAC")  )->GetTriggerProcessor();

  double tECal1=0;
  double tECal2=0;
  double tPVeto=0;
  double tSAC=0;

  
  tECal1 = ECalTrig->GetTriggerTime(0,0);
  tECal2 = ECalTrig->GetTriggerTime(15,0);
  tPVeto = PVetoTrig->GetTriggerTime(11,0);
  if(SACTrig)  tSAC = SACTrig->GetTriggerTime(27,0);
  
  
  GetHisto("TriggerTimeDiffECalECal")->Fill(tECal1 - tECal2);
  GetHisto("TriggerTimeDiffECalSAC"  )->Fill(tECal1 - tSAC );
  GetHisto("TriggerTimeDiffECalPVeto")->Fill(tECal1 - tPVeto);
  // std::cout <<"Trigger type:  " << ECalTrig->GetTrigMask() <<  "   DT = " << tECal1 - tPVeto << "   ECal=  "<< tECal1 << "  PVeto   " << tPVeto << std::endl;
  GetHisto("TriggerTimeDiffPVetoSAC" )->Fill(tPVeto - tSAC);

  
  //Hits check
  vector<TRecoVHit *> &ECalHits  = (MainReco->FindReco("ECal") )->GetRecoHits();
  vector<TRecoVHit *> &SACHits  =  (MainReco->FindReco("SAC") )->GetRecoHits();


  for(unsigned int iHP = 0; iHP < PVetoHits.size();++iHP) {
    for(unsigned int iHS = 0; iHS < SACHits.size();++iHS) {
      GetHisto("TimeDiffPVetoSAC" )->Fill( PVetoHits[iHP]->GetTime() -  SACHits[iHS]->GetTime() );
    }
  }
  for(unsigned int iHP = 0; iHP < Hits.size();++iHP) {
    for(unsigned int iHS = 0; iHS < SACHits.size();++iHS) {
      GetHisto("TimeDiffHEPVetoSAC" )->Fill( Hits[iHP]->GetTime() -  SACHits[iHS]->GetTime() );
    }
  }

  for(unsigned int iHP = 0; iHP < PVetoHits.size();++iHP) {
    for(unsigned int iHC = 0; iHC < ECalHits.size();++iHC) {
      GetHisto("TimeDiffECalPVeto" )->Fill( PVetoHits[iHP]->GetTime() -  ECalHits[iHC]->GetTime() );
    }
  }

  //Calo-Calo
  for(unsigned int iHP = 0; iHP < ECalHits.size();++iHP) {
    for(unsigned int iHC = iHP+1; iHC < ECalHits.size();++iHC) { 
      GetHisto("ECalTimeVsECalTime" )->Fill( ECalHits[iHP]->GetTime(), ECalHits[iHC]->GetTime() );
      if(ECalHits[iHP]->GetTime() > -100. && ECalHits[iHP]->GetTime()< 150 
	 && ECalHits[iHC]->GetTime() > -100. && ECalHits[iHC]->GetTime()< 150 ){
	GetHisto("TimeDiffECalECal" )->Fill( ECalHits[iHP]->GetTime() -  ECalHits[iHC]->GetTime() );
      }
    }
  }

  
  for(unsigned int iHP = 0; iHP < SACHits.size();++iHP) {
    for(unsigned int iHC = iHP+1; iHC < SACHits.size();++iHC) {
      if(SACHits[iHP]->GetChannelId() !=  SACHits[iHC]->GetChannelId()  ) 
	GetHisto("TimeDiffSACSAC" )->Fill( SACHits[iHP]->GetTime() -  SACHits[iHC]->GetTime() );
      
    }
  }


  

  
  //  vector<TRecoVHit *> &ECalHits  = PVetoReco->GetRecoHits();

  
  





}
