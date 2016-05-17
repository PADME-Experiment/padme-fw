// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "SACReconstruction.hh"

#include "TSACMCEvent.hh"

SACReconstruction::SACReconstruction(TFile* HistoFile, TString ConfigFileName) : PadmeVReconstruction(HistoFile, "SAC", ConfigFileName)
{
  //fRecoEvent = new TRecoSACEvent();
  ParseConfFile(ConfigFileName);
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

  ifstream confFile(ConfFileName.Data());
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
