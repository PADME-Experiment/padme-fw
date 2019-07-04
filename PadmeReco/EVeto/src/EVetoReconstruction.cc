// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "EVetoReconstruction.hh"

#include "TEVetoMCEvent.hh"
#include "TEVetoMCHit.hh"
#include "TEVetoMCDigi.hh"
#include "DigitizerChannelReco.hh"
#include "EVetoCalibration.hh"
#include "EVetoSimpleClusterization.hh"
#include "EVetoGeometry.hh"

#include "TH1F.h"
#include "TH2F.h"


EVetoReconstruction::EVetoReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "EVeto", ConfigFileName)
{
  //fRecoEvent = new TRecoEVetoEvent();
  //ParseConfFile(ConfigFileName);
  fChannelReco = new DigitizerChannelReco();
  fChannelCalibration = new EVetoCalibration();
  fClusterization = new EVetoSimpleClusterization();
  fTriggerProcessor = new PadmeVTrigger();
  fGeometry = new EVetoGeometry();
}

void EVetoReconstruction::HistoInit(){

  AddHisto("EVetoOccupancy",new TH1F("EVetoOccupancy","EVeto Occupancy",100,0.0,100.0));
  AddHisto("EVetoEnergy",new TH1F("EVetoEnergy","EVeto Energy",1200,0.0,12.0));
  AddHisto("EVetoTime",new TH1F("EVetoTime","EVeto Time",400,0.0,400.0));
  AddHisto("EVetoTimeVsChannelID",new TH2F("EVetoTimeVsChannelID","EVeto Time vs Ch. ID",100,0,100,100,0.0,400.0));
  AddHisto("EVetoHitTimeDifference",new TH1F("EVetoHitTimeDifference","Difference in time",400,-100.,100.));
  AddHisto("EVetoTimeVsEVetoTime",new TH2F("EVetoTimeVsEVetoTime","EVeto Time vs EVetoTime",400,0.0,400.0, 400,0.0,400.0));

  char name[256];

  for (int i=0; i<95; i++) { 
    sprintf(name, "EVetoDTch%dch%d",i,i+1);
    AddHisto(name, new TH1F(name,"Difference in time",400,-10.,10.));
  }
  


}


EVetoReconstruction::~EVetoReconstruction()
{;}

// void EVetoReconstruction::Init(PadmeVReconstruction* MainReco)
// {

//   //common part for all the subdetectors
//   PadmeVReconstruction::Init(MainReco);

// }

// // Read EVeto reconstruction parameters from a configuration file
// void EVetoReconstruction::ParseConfFile(TString ConfFileName) {

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
TRecoVEvent * EVetoReconstruction::ProcessEvent(TDetectorVEvent* tEvent, Event* tGenEvent)
{
  //common part for all the subdetectors 
  PadmeVReconstruction::ProcessEvent(tEvent, tGenEvent);

  TEVetoEvent* tEVetoEvent = (TEVetoEvent*)tEvent;
  const TClonesArray& Digis = (*(tEVetoEvent->GetHits()));
  return fRecoEvent;
}
*/

/* this was just for debugging via printout 
void EVetoReconstruction::ProcessEvent(TMCVEvent* tEvent, TMCEvent* tMCEvent)
{
  PadmeVReconstruction::ProcessEvent(tEvent,tMCEvent);

  TEVetoMCEvent* tEVetoEvent = (TEVetoMCEvent*)tEvent;
  std::cout << "--- EVetoReconstruction --- run/event/#hits/#digi " << tEVetoEvent->GetRunNumber() << " " << tEVetoEvent->GetEventNumber() << " " << tEVetoEvent->GetNHits() << " " << tEVetoEvent->GetNDigi() << std::endl;
  for (Int_t iH=0; iH<tEVetoEvent->GetNHits(); iH++) {
    TEVetoMCHit* hit = (TEVetoMCHit*)tEVetoEvent->Hit(iH);
    hit->Print();
  }
  for (Int_t iD=0; iD<tEVetoEvent->GetNDigi(); iD++) {
    TEVetoMCDigi* digi = (TEVetoMCDigi*)tEVetoEvent->Digi(iD);
    digi->Print();
  }
}
*/

// void EVetoReconstruction::EndProcessing()
// {;}
void EVetoReconstruction::AnalyzeEvent(TRawEvent* rawEv){

  vector<TRecoVHit *> &Hits  = GetRecoHits();
  for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
    if(Hits[iHit1]->GetTime() < 10.) continue;

    
    GetHisto("EVetoOccupancy")->Fill(Hits[iHit1]->GetChannelId());
    GetHisto("EVetoTime")->Fill(Hits[iHit1]->GetTime());
    GetHisto("EVetoEnergy") -> Fill(Hits[iHit1]->GetEnergy() );
    ((TH2F *) GetHisto("EVetoTimeVsChannelID"))->Fill(Hits[iHit1]->GetChannelId(), Hits[iHit1]->GetTime());
    
    

  }    

  char name[256];
  
  int ih1,ih2;

  for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
    for(unsigned int iHit2 = 0; iHit2 < Hits.size();++iHit2) {

      if(Hits[iHit1]->GetTime() > 20. && Hits[iHit2]->GetTime() > 20.) {
	(  (TH2F *) GetHisto("EVetoTimeVsEVetoTime"))  ->Fill(Hits[iHit1]->GetTime(),Hits[iHit2]->GetTime());
	GetHisto("EVetoHitTimeDifference")->Fill(Hits[iHit1]->GetTime() - Hits[iHit2]->GetTime());
      }


      if(Hits[iHit1]->GetChannelId() + 1 ==  Hits[iHit2]->GetChannelId()   ) {

	sprintf(name, "EVetoDTch%dch%d", Hits[iHit1]->GetChannelId() ,Hits[iHit1]->GetChannelId()+1);
	if(Hits[iHit1]->GetTime() > 20. && Hits[iHit2]->GetTime() > 20.) {	  
	  GetHisto(name)->Fill(Hits[iHit1]->GetTime() - Hits[iHit2]->GetTime());
	}
      }
    }
  }


}
