// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "PVetoReconstruction.hh"

#include "TPVetoMCEvent.hh"
#include "TPVetoMCHit.hh"
#include "TPVetoMCDigi.hh"

#include "TH1F.h"
#include "TDirectory.h"

PVetoReconstruction::PVetoReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "PVeto", ConfigFileName)
{
  //fRecoEvent = new TRecoPVetoEvent();
  //ParseConfFile(ConfigFileName);
}

PVetoReconstruction::~PVetoReconstruction()
{;}


void PVetoReconstruction::HistoInit(){
  if(fHistoFile == NULL) {
    std::cout << "Output histo file not existing" << std::endl;
    return;
  }
  fHistoFile->cd();
  TDirectory *dir =  (TDirectory *)fHistoFile->Get(this->GetName().Data());
  if(dir == NULL) {
    std::cout << "Output directory does not exist: " << this->GetName().Data() << std::endl;
    return;
  }
  dir->cd();
  std::cout << "Creating the histograms" << std::endl;
  TH1F * nb = new TH1F("nboards","Number of boards",100,0.0,100.0);
}

void PVetoReconstruction::Init(PadmeVReconstruction* MainReco)
{

  //common part for all the subdetectors
  PadmeVReconstruction::Init(MainReco);
  HistoInit();

}

// Read PVeto reconstruction parameters from a configuration file
void PVetoReconstruction::ParseConfFile(TString ConfFileName) {

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
TRecoVEvent * PVetoReconstruction::ProcessEvent(TDetectorVEvent* tEvent, Event* tGenEvent)
{
  //common part for all the subdetectors 
  PadmeVReconstruction::ProcessEvent(tEvent, tGenEvent);

  TPVetoEvent* tPVetoEvent = (TPVetoEvent*)tEvent;
  const TClonesArray& Digis = (*(tPVetoEvent->GetHits()));
  return fRecoEvent;
}
*/
void PVetoReconstruction::ProcessEvent(TMCVEvent* tEvent, TMCEvent* tMCEvent)
{
  PadmeVReconstruction::ProcessEvent(tEvent,tMCEvent);
  TPVetoMCEvent* tPVetoEvent = (TPVetoMCEvent*)tEvent;
  std::cout << "--- PVetoReconstruction --- run/event/#hits/#digi " << tPVetoEvent->GetRunNumber() << " " << tPVetoEvent->GetEventNumber() << " " << tPVetoEvent->GetNHits() << " " << tPVetoEvent->GetNDigi() << std::endl;
  for (Int_t iH=0; iH<tPVetoEvent->GetNHits(); iH++) {
    TPVetoMCHit* hit = (TPVetoMCHit*)tPVetoEvent->Hit(iH);
    hit->Print();
  }
  for (Int_t iD=0; iD<tPVetoEvent->GetNDigi(); iD++) {
    TPVetoMCDigi* digi = (TPVetoMCDigi*)tPVetoEvent->Digi(iD);
    digi->Print();
  }
}

void PVetoReconstruction::ProcessEvent(TRawEvent* rawEv){
  UChar_t nBoards = rawEv->GetNADCBoards();
  printf("PVETO:  Run nr %d Event nr %d ADC boards %d\n",
         rawEv->GetRunNumber(),rawEv->GetEventNumber(),nBoards);
  TDirectory *dir =  (TDirectory *)fHistoFile->Get(this->GetName().Data());
  TH1F * nb = (TH1F *) dir->Get("nboards");
  

  //  ((TH1F *) ((TDirectory *)fHistoFile->Get(this->GetName().Data()))->Get("nboards")) ->Fill(nBoards);
  //  nb->Fill((Int_t) nBoards  );


}


void PVetoReconstruction::EndProcessing()
{;}
