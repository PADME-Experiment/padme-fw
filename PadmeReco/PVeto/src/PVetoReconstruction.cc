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
#include "TPVetoRecoEvent.hh"

#include "TH1F.h"
#include "TDirectory.h"

PVetoReconstruction::PVetoReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "PVeto", ConfigFileName)
{
  //fRecoEvent = new TRecoPVetoEvent();
  //ParseConfFile(ConfigFileName);
  fChannelReco = new ADCChannelVReco();
}

PVetoReconstruction::~PVetoReconstruction()
{;}


// void PVetoReconstruction::HistoInit(){
//   if(fHistoFile == NULL) {
//     std::cout << "Output histo file not existing" << std::endl;
//     return;
//   }
//   fHistoFile->cd();
//   TDirectory *dir =  (TDirectory *)fHistoFile->Get(this->GetName().Data());
//   if(dir == NULL) {
//     std::cout << "Output directory does not exist: " << this->GetName().Data() << std::endl;
//     return;
//   }
//   dir->cd();
//   std::cout << "Creating the histograms" << std::endl;
//   TH1F * nb = new TH1F("nboards","Number of boards",100,0.0,100.0);

  

// }

void PVetoReconstruction::HistoInit(){
  AddHisto("nboards", new TH1F("nboards","Number of boards",100,0.0,100.0));
  AddHisto("ADCs",new TH1F("ADCs","ADC ID",100,0.0,100.));
  AddHisto("nchannels", new TH1F("nchannels","Number of channels",100,0.0,100.0));
  AddHisto("ntriggers", new TH1F("ntriggers","Number of trigger channels",100,0.0,100.0));

  AddHisto("HitTimeDifference",new TH1F("HitTimeDifference","Difference in time",100,-100.,100.));

}


void PVetoReconstruction::Init(PadmeVReconstruction* MainReco)
{
  std::cout << "PVeto: Initializing" << std::endl;
  //common part for all the subdetectors
  PadmeVReconstruction::Init(MainReco);
  fChannelReco->Init(GetConfig());
  InitChannelID(GetConfig());
  HistoInit();
  // if(GetConfigParser()->HasConfig("ADC","NADC"))
  //   std::cout << "Number of ADCs for detector:  " << this->GetName() << "  " << GetConfigParser()->GetSingleArg("ADC","NADC") << std::endl;

  std::cout <<"Number of ADCs for detector: " << this->GetName() << ": " << GetConfig()-> GetNBoards() << std::endl;

  

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
  //Perform some cleaning before:
  vector<TRecoVHit *> &Hits  = GetRecoHits();
  for(int iHit = 0;iHit < Hits.size();iHit++){
    delete Hits[iHit];
  }
  Hits.clear();


  UChar_t nBoards = rawEv->GetNADCBoards();
  // printf("PVETO:  Run nr %d Event nr %d ADC boards %d\n",
  //        rawEv->GetRunNumber(),rawEv->GetEventNumber(),nBoards);
  GetHisto("nboards")->Fill( (Int_t) nBoards );

  TADCBoard* ADC;

  for(Int_t iBoard = 0; iBoard < nBoards; iBoard++) {
    ADC = rawEv->ADCBoard(iBoard);
    GetHisto("ADCs")->Fill(ADC->GetBoardId());
    GetHisto("nchannels")->Fill(ADC->GetNADCChannels());
    GetHisto("ntriggers")->Fill(ADC->GetNADCTriggers());
  }
  for(Int_t iBoard = 0; iBoard < nBoards; iBoard++) {
    ADC = rawEv->ADCBoard(iBoard);
    if(GetConfig()->BoardIsMine( ADC->GetBoardId())) {
      
      // std::cout << "ADC " << (Int_t) ADC->GetBoardId() << " is mine. Processing" << std::endl;
      //Loop over the channels and perform reco
      for(unsigned ich = 0; ich < ADC->GetNADCChannels();ich++) {
	TADCChannel* chn = ADC->ADCChannel(ich);
	fChannelReco->SetSamples(chn->GetNSamples(),chn->GetSamplesArray());
	unsigned int nHitsBefore = Hits.size();
	fChannelReco->Reconstruct(Hits);
	unsigned int nHitsAfter = Hits.size();
	// std::cout << "Added  " << nHitsAfter - nHitsBefore << "  hits for channel " << ich<<  std::endl;
	//Set the proper channel ID to the hit
	
	for(unsigned int iHit = nHitsBefore; iHit < nHitsAfter;++iHit) {
	  Hits[iHit]->SetChannelId(GetChannelID(ADC->GetBoardId(),ich));
	  //	  std::cout << "Setting channel ID for hit: " << iHit << "  chID: " <<   GetChannelID(ADC->GetBoardId(),ich) << std::endl;
	}
      }
    } else {
      // std::cout << "ADC " << (int) ADC->GetBoardId() << " is NOT mine. Skipping" << std::endl;
    }
    
    // std::cout << "ADC BID " << (int) ADC->GetBoardId() << std::endl;
  }

  for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
    for(unsigned int iHit2 = iHit1+1; iHit2 < Hits.size();++iHit2) {
      GetHisto("HitTimeDifference")->Fill(Hits[iHit1]->GetTime() - Hits[iHit2]->GetTime());
    }
      
  }
 
}


void PVetoReconstruction::EndProcessing()
{
  cout << "Entering End processing for " << this->GetName() << endl;

  HistoExit();
}
