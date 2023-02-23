#include "Riostream.h"

#include <TString.h>

#include "ETagHit.hh"
#include "ETagCluster.hh"

#include "ETagReconstruction.hh"

ETagReconstruction::ETagReconstruction(TString ConfigFileName)
{
  printf("ETagReconstruction::ETagReconstruction\n");
}

ETagReconstruction::~ETagReconstruction()
{
  printf("ETagReconstruction::~ETagReconstruction\n");
}

void ETagReconstruction::Init()
{
  printf("ETagReconstruction::Init\n");
}

void ETagReconstruction::ProcessEvent(TMCVEvent* tEvent,TMCEvent* tMCEvent)
{
  printf("ETagReconstruction::ProcessEvent MonteCarlo\n");
}

void ETagReconstruction::ProcessEvent(TETagRecoEvent* tEvent,TRecoEvent* tRecoEvent)
{
  printf("ETagReconstruction::ProcessEvent Reprocessing\n");
}

void ETagReconstruction::ProcessEvent(TRawEvent* rawEvent)
{
  printf("ETagReconstruction::ProcessEvent RawData\n");

  fHits.clear();
  fClusters.clear();

  ETagHit* hit = new ETagHit();
  hit->SetADCBoard(23);
  hit->SetADCChannel(11);
  hit->SetChannelId(345);
  hit->SetStatus(0x0000000000000003);
  hit->SetPosition(TVector3(1.,1.,2.));
  hit->SetEnergy(324.3);
  hit->SetTime(23456.456);
  fHits.push_back(hit);

  ETagCluster* clus = new ETagCluster();
  clus->SetStatus(0x0000000000000003);
  clus->SetPosition(TVector3(1.,1.,2.));
  clus->SetEnergy(324.3);
  clus->SetTime(23456.456);
  clus->SetSeed(1);
  clus->SetNHits(1);
  std::vector<Int_t> v;
  v.push_back(1);
  clus->SetHitsVector(v);
  fClusters.push_back(clus);
}

void ETagReconstruction::EndProcessing()
{
  printf("ETagReconstruction::EndProcessing\n");
}
