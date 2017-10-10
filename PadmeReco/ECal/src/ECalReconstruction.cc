// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "ECalReconstruction.hh"

#include "ECalParameters.hh"
#include "ECalCrystalHandler.hh"
#include "ECalCrystal.hh"
#include "ECalClusterFinderIsland.hh"
#include "ECalClusterFinderRadius.hh"
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

  ECalParameters* para = ECalParameters::GetInstance();

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
    Int_t ch = digi->GetChannelId();
    Double_t sig = digi->GetSignal();
    Int_t iX = ch/100;
    Int_t iY = ch%100;
    ECalCrystal* cry = cryHand->CreateCrystal(iX,iY);
    cry->SetCharge(sig);
    cry->SetEnergy(sig*para->GetSignalToEnergy(ch));
    cry->SetTime(digi->GetTime());
    cry->Print();
  }
  cryHand->SortEnergy();

  // Find clusters with PadmeIsland algorithm
  ECalClusterHandler* cluHandIsl = new ECalClusterHandler();
  ECalClusterFinderIsland* cluFindIsl = new ECalClusterFinderIsland(cryHand,cluHandIsl);
  Int_t newNClu = cluFindIsl->FindClusters();
  printf("- Cluster finding result - PadmeIsland algorithm -\n");
  cluHandIsl->Print();

  // Find clusters with PadmeRadius algorithm
  ECalClusterHandler* cluHandRad = new ECalClusterHandler();
  ECalClusterFinderRadius* cluFindRad = new ECalClusterFinderRadius(cryHand,cluHandRad);
  Int_t newNCluRad = cluFindRad->FindClusters();
  printf("- Cluster finding result - PadmeRadius algorithm -\n");
  cluHandRad->Print();

  // Here you can do something with your clusters

  // Final cleanup

  delete cryHand;
  delete cluHandIsl;
  delete cluFindIsl;
  delete cluHandRad;
  delete cluFindRad;

}

void ECalReconstruction::EndProcessing()
{;}
