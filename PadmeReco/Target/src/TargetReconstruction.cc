// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "TargetReconstruction.hh"

#include "TTargetMCEvent.hh"
#include "TTargetMCHit.hh"
#include "TTargetMCDigi.hh"

TargetReconstruction::TargetReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "Target", ConfigFileName)
{
  //fRecoEvent = new TRecoTargetEvent();
  //ParseConfFile(ConfigFileName);
}

TargetReconstruction::~TargetReconstruction()
{;}

void TargetReconstruction::Init(PadmeVReconstruction* MainReco)
{

  //common part for all the subdetectors
  PadmeVReconstruction::Init(MainReco);

}

// Read Target reconstruction parameters from a configuration file
void TargetReconstruction::ParseConfFile(TString ConfFileName) {

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

void TargetReconstruction::ProcessEvent(TMCVEvent* tEvent, TMCEvent* tMCEvent)
{
  PadmeVReconstruction::ProcessEvent(tEvent,tMCEvent);
  TTargetMCEvent* tTargetEvent = (TTargetMCEvent*)tEvent;
  std::cout << "--- TargetReconstruction --- run/event/#hits/#digi " << tTargetEvent->GetRunNumber() << " " << tTargetEvent->GetEventNumber() << " " << tTargetEvent->GetNHits() << " " << tTargetEvent->GetNDigi() << std::endl;
  for (Int_t iH=0; iH<tTargetEvent->GetNHits(); iH++) {
    TTargetMCHit* hit = (TTargetMCHit*)tTargetEvent->Hit(iH);
    hit->Print();
  }
  for (Int_t iD=0; iD<tTargetEvent->GetNDigi(); iD++) {
    TTargetMCDigi* digi = (TTargetMCDigi*)tTargetEvent->Digi(iD);
    digi->Print();
  }
}

void TargetReconstruction::EndProcessing()
{;}
