// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "ECalReconstruction.hh"

#include "ECalCrystalHandler.hh"
#include "ECalCrystal.hh"
#include "ECalClusterFinder.hh"
#include "ECalClusterFinderBox.hh"
#include "ECalCluster.hh"

#include "TECalMCEvent.hh"
#include "TECalMCHit.hh"
#include "TECalMCDigi.hh"

ECalReconstruction::ECalReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "ECal", ConfigFileName)
{
  //fRecoEvent = new TRecoECalEvent();
  //ParseConfFile(ConfigFileName);
}

ECalReconstruction::~ECalReconstruction()
{;}

void ECalReconstruction::Init(PadmeVReconstruction* MainReco)
{

  //common part for all the subdetectors
  PadmeVReconstruction::Init(MainReco);

}

// Read ECal reconstruction parameters from a configuration file
void ECalReconstruction::ParseConfFile(TString ConfFileName) {

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
TRecoVEvent * ECalReconstruction::ProcessEvent(TDetectorVEvent* tEvent, Event* tGenEvent)
{
  //common part for all the subdetectors 
  PadmeVReconstruction::ProcessEvent(tEvent, tGenEvent);

  TECalEvent* tECalEvent = (TECalEvent*)tEvent;
  const TClonesArray& Digis = (*(tECalEvent->GetHits()));
  return fRecoEvent;
}
*/
void ECalReconstruction::ProcessEvent(TMCVEvent* tEvent, TMCEvent* tMCEvent)
{
  PadmeVReconstruction::ProcessEvent(tEvent,tMCEvent);
  TECalMCEvent* tECalEvent = (TECalMCEvent*)tEvent;
  std::cout << "--- ECalReconstruction --- run/event/#hits/#digi " << tECalEvent->GetRunNumber() << " " << tECalEvent->GetEventNumber() << " " << tECalEvent->GetNHits() << " " << tECalEvent->GetNDigi() << std::endl;
  for (Int_t iH=0; iH<tECalEvent->GetNHits(); iH++) {
    TECalMCHit* hit = (TECalMCHit*)tECalEvent->Hit(iH);
    hit->Print();
  }
  for (Int_t iD=0; iD<tECalEvent->GetNDigi(); iD++) {
    TECalMCDigi* digi = (TECalMCDigi*)tECalEvent->Digi(iD);
    digi->Print();
  }

  // Let's do some cluster finding
  ECalCrystalHandler* cryHand = new ECalCrystalHandler();
  for (Int_t iD=0; iD<tECalEvent->GetNDigi(); iD++) {
    TECalMCDigi* digi = (TECalMCDigi*)tECalEvent->Digi(iD);
    Int_t iX = digi->GetChannelId()/100;
    Int_t iY = digi->GetChannelId()%100;
    ECalCrystal* cry = cryHand->CreateCrystal(iX,iY);
    cry->SetEnergy(digi->GetSignal()); // Will need a signal->energy converter
    cry->Print();
  }
  cryHand->SortEnergy();

  // Find clusters with PadmeIsland algorithm
  ECalClusterHandler* cluHand = new ECalClusterHandler();
  ECalClusterFinder* cluFind = new ECalClusterFinder(cryHand,cluHand);
  cluFind->SetEThreshold(0.1); //min thr in each crystal
  cluFind->SetEThresholdSeed(10.); //min thr for seed crystal
  Int_t newNClu = cluFind->FindClusters();
  cluHand->Print();

  // Find clusters with PadmeRadius algorithm
  ECalClusterHandler* cluHandBox = new ECalClusterHandler();
  ECalClusterFinderBox* cluFindBox = new ECalClusterFinderBox(cryHand,cluHandBox);
  cluFindBox->SetEThreshold(0.1); //min thr in each crystal
  cluFindBox->SetEThresholdSeed(10.); //min thr for seed crystal
  Int_t newNCluBox = cluFindBox->FindClusters();
  cluHandBox->Print();

}

void ECalReconstruction::EndProcessing()
{;}
