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

HEPVetoReconstruction::HEPVetoReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "HEPVeto", ConfigFileName)
{
  //fRecoEvent = new TRecoHEPVetoEvent();
  //ParseConfFile(ConfigFileName);
}

HEPVetoReconstruction::~HEPVetoReconstruction()
{;}

void HEPVetoReconstruction::Init(PadmeVReconstruction* MainReco)
{

  //common part for all the subdetectors
  PadmeVReconstruction::Init(MainReco);

}

// Read HEPVeto reconstruction parameters from a configuration file
void HEPVetoReconstruction::ParseConfFile(TString ConfFileName) {

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

void HEPVetoReconstruction::EndProcessing()
{;}
