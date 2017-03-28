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

EVetoReconstruction::EVetoReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "EVeto", ConfigFileName)
{
  //fRecoEvent = new TRecoEVetoEvent();
  //ParseConfFile(ConfigFileName);
}

EVetoReconstruction::~EVetoReconstruction()
{;}

void EVetoReconstruction::Init(PadmeVReconstruction* MainReco)
{

  //common part for all the subdetectors
  PadmeVReconstruction::Init(MainReco);

}

// Read EVeto reconstruction parameters from a configuration file
void EVetoReconstruction::ParseConfFile(TString ConfFileName) {

  std::ifstream confFile(ConfFileName.Data());
  if (!confFile.is_open()) {
    perror(ConfFileName);
    exit(1);
  }

  TString Line;
  while (Line.ReadLine(confFile)) {
    if (Line.BeginsWith("#")) continue;
  }
  confFile.close();
}

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

void EVetoReconstruction::EndProcessing()
{;}
