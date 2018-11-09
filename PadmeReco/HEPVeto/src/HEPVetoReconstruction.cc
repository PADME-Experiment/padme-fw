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
#include "PadmeReconstruction.hh"
#include "TH2F.h"

HEPVetoReconstruction::HEPVetoReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "HEPVeto", ConfigFileName)
{
  //fRecoEvent = new TRecoHEPVetoEvent();
  //ParseConfFile(ConfigFileName);
  fChannelReco = new DigitizerChannelReco();
}




void HEPVetoReconstruction::HistoInit(){
  AddHisto("HEPVetoOccupancy",new TH1F("HEPVetoOccupancy","HEPVeto Occupancy",32,0.0,32.0));
  AddHisto("HEPVetoTime",new TH1F("HEPVetoTime","HEPVeto Time",1000,0.0,1000.0));
  AddHisto("HEPVetoMultiplicity",new TH1F("HEPVetoMultiplicity","Number of hits in the HEPVeto per event", 100, 0.0,100.0));
  AddHisto("HEPVetoTimeVsPVetoTime",new TH2F("HEPVetoTimeVsPVetoTime","HEPVeto Time vs PVetoTime",100,100.0,500.0, 100,100.0,500.0));
 
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
    if( Hits[iHit1]->GetTime() > 50.) {
      //Real hit
        GetHisto("HEPVetoOccupancy")->Fill(Hits[iHit1]->GetChannelId());
    }
    GetHisto("HEPVetoTime")->Fill(Hits[iHit1]->GetTime());
  }

  PadmeReconstruction* MainReco = (PadmeReconstruction*  ) fMainReco;
  
  
  PadmeVReconstruction* PVetoReco = ( MainReco->FindReco("PVeto") );
  
  vector<TRecoVHit *> &PVetoHits  = PVetoReco->GetRecoHits();
  
  for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
    for(unsigned int iHit2 = 0; iHit2 < PVetoHits.size();++iHit2) {
      if ( PVetoHits[iHit2]->GetChannelId() > 80 ) {
          (  (TH2F *) GetHisto("HEPVetoTimeVsPVetoTime"))  ->Fill(Hits[iHit1]->GetTime(),PVetoHits[iHit2]->GetTime());
      }
      
    }
  }
 




}
