// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2017-03-27
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "TPixReconstruction.hh"

#include "TTPixMCEvent.hh"
#include "TTPixMCHit.hh"
#include "TTPixMCDigi.hh"

TPixReconstruction::TPixReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "TPix", ConfigFileName)
{
  //fRecoEvent = new TRecoTPixEvent();
  //ParseConfFile(ConfigFileName);
}

TPixReconstruction::~TPixReconstruction()
{;}

void TPixReconstruction::Init(PadmeVReconstruction* MainReco)
{

  //common part for all the subdetectors
  PadmeVReconstruction::Init(MainReco);

}

// Read TPix reconstruction parameters from a configuration file
void TPixReconstruction::ParseConfFile(TString ConfFileName) {

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

void TPixReconstruction::ProcessEvent(TMCVEvent* tEvent, TMCEvent* tMCEvent)
{
  PadmeVReconstruction::ProcessEvent(tEvent,tMCEvent);
  TTPixMCEvent* tTPixEvent = (TTPixMCEvent*)tEvent;
  std::cout << "--- TPixReconstruction --- run/event/#hits/#digi " << tTPixEvent->GetRunNumber() << " " << tTPixEvent->GetEventNumber() << " " << tTPixEvent->GetNHits() << " " << tTPixEvent->GetNDigi() << std::endl;
  for (Int_t iH=0; iH<tTPixEvent->GetNHits(); iH++) {
    TTPixMCHit* hit = (TTPixMCHit*)tTPixEvent->Hit(iH);
    hit->Print();
  }
  for (Int_t iD=0; iD<tTPixEvent->GetNDigi(); iD++) {
    TTPixMCDigi* digi = (TTPixMCDigi*)tTPixEvent->Digi(iD);
    digi->Print();
  }
}

void TPixReconstruction::EndProcessing()
{;}
