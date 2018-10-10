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
#include "DigitizerChannelReco.hh"
#include "TH2F.h"
#include "TCanvas.h"


SACReconstruction::SACReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "SAC", ConfigFileName)
{
  //fRecoEvent = new TRecoSACEvent();
  //ParseConfFile(ConfigFileName);
  fChannelReco = new DigitizerChannelReco();
}


void SACReconstruction::HistoInit(){
  AddHisto("SACOccupancy",new TH2F("SACOccupancy","SACOccupancy",5,0,5,5,0,5));
  AddHisto("SACCharge",new TH2F("SACCharge","SACCharge",5,0,5,5,0,5));

}



SACReconstruction::~SACReconstruction()
{;}

// void SACReconstruction::Init(PadmeVReconstruction* MainReco)
// {

//   //common part for all the subdetectors
//   PadmeVReconstruction::Init(MainReco);

// }

// // Read SAC reconstruction parameters from a configuration file
// void SACReconstruction::ParseConfFile(TString ConfFileName) {

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

// void SACReconstruction::EndProcessing()
// {;}

void SACReconstruction::AnalyzeEvent(TRawEvent* rawEv){
  static int nevt;
  static TCanvas c;

  vector<TRecoVHit *> &Hits  = GetRecoHits();
  //  return;

  for(unsigned int iHit1 =  0; iHit1 < Hits.size(); ++iHit1) {
    int ich = Hits[iHit1]->GetChannelId();
    GetHisto("SACOccupancy") -> Fill(ich/10,ich%10);
    //    GetHisto("SACCharge") -> Fill(ich/10,ich%10,Hits[iHit1]->GetEnergy());

  }

  if(nevt % 100 == 0) {
    c.cd();
    GetHisto("SACOccupancy") -> Draw();
    c.Update();
  }

  nevt ++;
}

