// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "SACReconstruction.hh"

#include "TSACMCEvent.hh"
#include "TSACMCHit.hh"
#include "TSACMCDigi.hh"

SACReconstruction::SACReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "SAC", ConfigFileName)
{
  //fRecoEvent = new TRecoSACEvent();
  //ParseConfFile(ConfigFileName);
}

SACReconstruction::~SACReconstruction()
{;}

void SACReconstruction::Init(PadmeVReconstruction* MainReco)
{

  //common part for all the subdetectors
  PadmeVReconstruction::Init(MainReco);

}

// Read SAC reconstruction parameters from a configuration file
void SACReconstruction::ParseConfFile(TString ConfFileName) {

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
TRecoVEvent * SACReconstruction::ProcessEvent(TDetectorVEvent* tEvent, Event* tGenEvent)
{
  //common part for all the subdetectors 
  PadmeVReconstruction::ProcessEvent(tEvent, tGenEvent);

  TSACEvent* tSACEvent = (TSACEvent*)tEvent;
  const TClonesArray& Digis = (*(tSACEvent->GetHits()));
  return fRecoEvent;
}
*/
void SACReconstruction::ProcessEvent(TMCVEvent* tEvent, TMCEvent* tMCEvent)
{
  PadmeVReconstruction::ProcessEvent(tEvent,tMCEvent);
  TSACMCEvent* tSACEvent = (TSACMCEvent*)tEvent;
  std::cout << "--- SACReconstruction --- run/event/#hits/#digi " << tSACEvent->GetRunNumber() << " " << tSACEvent->GetEventNumber() << " " << tSACEvent->GetNHits() << " " << tSACEvent->GetNDigi() << std::endl;
  for (Int_t iH=0; iH<tSACEvent->GetNHits(); iH++) {
    TSACMCHit* hit = (TSACMCHit*)tSACEvent->Hit(iH);
    hit->Print();
  }
  for (Int_t iD=0; iD<tSACEvent->GetNDigi(); iD++) {
    TSACMCDigi* digi = (TSACMCDigi*)tSACEvent->Digi(iD);
    digi->Print();
  }
}

void SACReconstruction::EndProcessing()
{;}
