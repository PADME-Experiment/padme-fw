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
#include "HEPVetoSimpleClusterization.hh"
#include "TH2F.h"



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

  


  AddHisto("TimeDiffPVetoSAC" , new TH1F("TimeDiffPVetoSAC", "TimeDiffPVetoSAC",2000, -1000, 1000));
  AddHisto("TimeDiffHEPVetoSAC" , new TH1F("TimeDiffHEPVetoSAC", "TimeDiffHEPVetoSAC",2000, -1000, 1000));
  AddHisto("TimeDiffECalPVeto" , new TH1F("TimeDiffECalPVeto", "TimeDiffECalPVeto",2000, -1000, 1000));
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

// void HEPVetoReconstruction::EndProcessing()
// {;}

void HEPVetoReconstruction::AnalyzeEvent(TRawEvent* rawEv){

  
  vector<TRecoVHit *> &Hits  = GetRecoHits();
  
  UChar_t nBoards = rawEv->GetNADCBoards();
  
  TADCBoard* ADC;
  
  GetHisto("HEPVetoMultiplicity")->Fill(Hits.size());
  for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
    if( Hits[iHit1]->GetTime() > 10.) {
      //Real hit
        GetHisto("HEPVetoOccupancy")->Fill(Hits[iHit1]->GetChannelId());
    
    GetHisto("HEPVetoTime")->Fill(Hits[iHit1]->GetTime());
    }
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
