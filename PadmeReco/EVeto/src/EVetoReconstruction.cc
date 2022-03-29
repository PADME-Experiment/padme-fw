// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "EVetoReconstruction.hh"

#include "TEVetoMCEvent.hh"
#include "TEVetoMCHit.hh"
#include "TEVetoMCDigi.hh"
//#include "DigitizerChannelReco.hh"
#include "DigitizerChannelEVeto.hh"
#include "EVetoCalibration.hh"
#include "EVetoSimpleClusterization.hh"
#include "EVetoGeometry.hh"

#include "TH1F.h"
#include "TH2F.h"


EVetoReconstruction::EVetoReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "EVeto", ConfigFileName)
{
  //fRecoEvent = new TRecoEVetoEvent();
  //ParseConfFile(ConfigFileName);
//  fChannelReco = new DigitizerChannelReco();
  fChannelReco = new DigitizerChannelEVeto();
  fChannelCalibration = new EVetoCalibration();
  fClusterization = new EVetoSimpleClusterization();
  fTriggerProcessor = new PadmeVTrigger();
  fGeometry = new EVetoGeometry();
}

EVetoReconstruction::~EVetoReconstruction()
{;}


void EVetoReconstruction::HistoInit(){

  AddHisto("EVetoOccupancy",new TH1F("EVetoOccupancy","EVeto Occupancy",100,0.0,100.0));
  AddHisto("EVetoEnergy",new TH1F("EVetoEnergy","EVeto Energy",1200,0.0,12.0));
  AddHisto("EVetoEnergyClean",new TH1F("EVetoEnergyClean","EVeto Energy",2000,0.0,.4));
  AddHisto("EVetoTime",new TH1F("EVetoTime","EVeto Time",400,0.0,400.0));
  AddHisto("EVetoTimeVsChannelID",new TH2F("EVetoTimeVsChannelID","EVeto Time vs Ch. ID",100,0,100,100,0.0,400.0));
  AddHisto("EVetoHitTimeDifference",new TH1F("EVetoHitTimeDifference","Difference in time",400,-100.,100.));
  AddHisto("EVetoTimeVsEVetoTime",new TH2F("EVetoTimeVsEVetoTime","EVeto Time vs EVetoTime",400,0.0,400.0, 400,0.0,400.0));

  char name[256];

  
  for (int i=0; i<95; i++) { 
    sprintf(name, "EVetoDTch%dch%d",i,i+1);
    AddHisto(name, new TH1F(name,"Difference in time",100,-25.,25.));
  }
  
  for (int i=0; i<96; i++) { 
    sprintf(name, "EVetoCharge-%d",i);
    AddHisto(name, new TH1F(name,"Charge",2000,00.,0.4));
  }


}


// void EVetoReconstruction::Init(PadmeVReconstruction* MainReco)
// {

//   //common part for all the subdetectors
//   PadmeVReconstruction::Init(MainReco);

// }

// // Read EVeto reconstruction parameters from a configuration file
// void EVetoReconstruction::ParseConfFile(TString ConfFileName) {

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
TRecoVEvent * EVetoReconstruction::ProcessEvent(TDetectorVEvent* tEvent, Event* tGenEvent)
{
  //common part for all the subdetectors 
  PadmeVReconstruction::ProcessEvent(tEvent, tGenEvent);

  TEVetoEvent* tEVetoEvent = (TEVetoEvent*)tEvent;
  const TClonesArray& Digis = (*(tEVetoEvent->GetHits()));
  return fRecoEvent;
}
*/

/* this was just for debugging via printout 
void EVetoReconstruction::ProcessEvent(TMCVEvent* tEvent, TMCEvent* tMCEvent)
{
  PadmeVReconstruction::ProcessEvent(tEvent,tMCEvent);

  TEVetoMCEvent* tEVetoEvent = (TEVetoMCEvent*)tEvent;
  std::cout << "--- EVetoReconstruction --- run/event/#hits/#digi " << tEVetoEvent->GetRunNumber() << " " << tEVetoEvent->GetEventNumber() << " " << tEVetoEvent->GetNHits() << " " << tEVetoEvent->GetNDigi() << std::endl;
  for (Int_t iH=0; iH<tEVetoEvent->GetNHits(); iH++) {
    TEVetoMCHit* hit = (TEVetoMCHit*)tEVetoEvent->Hit(iH);
    hit->Print();
  }
  for (Int_t iD=0; iD<tEVetoEvent->GetNDigi(); iD++) {
    TEVetoMCDigi* digi = (TEVetoMCDigi*)tEVetoEvent->Digi(iD);
    digi->Print();
  }
}
*/

void EVetoReconstruction::ProcessEvent(TRawEvent* rawEv){//Beth 22/2/22: copied from virtual class to override virtual class. Removed both calibration and clusterisation since calibration (gain equalisation) will be done directly in digitizer and clusterisation will use the new battleships algorithm

  //  std::cout<<"!?><using pveto process event"<<std::endl;

  // use trigger info 
  if(fTriggerProcessor) {
    //std::cout<<"Reconstruction named <"<<GetName()<<"> processing TriggerInfo .... "<<std::endl;
    BuildTriggerInfo(rawEv);
    if (TriggerToBeSkipped()) return;
  }
    
  // from waveforms to Hits
  BuildHits(rawEv);

  //Processing is over, let's analyze what's here, if foreseen
  if(fGlobalRecoConfigOptions->IsMonitorMode()) {
    //    AnalyzeEvent(rawEv);
  }

}

// void EVetoReconstruction::EndProcessing()
// {;}
void EVetoReconstruction::AnalyzeEvent(TRawEvent* rawEv){
  float charges[96];
  for(int i=0;i<96;i++) charges[i] = -1.;

  vector<TRecoVHit *> &Hits  = GetRecoHits();
  for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
    charges[Hits[iHit1]->GetChannelId()] = Hits[iHit1]->GetEnergy();
    //    if(Hits[iHit1]->GetTime() < 10.) continue;

    
    GetHisto("EVetoOccupancy")->Fill(Hits[iHit1]->GetChannelId());
    GetHisto("EVetoTime")->Fill(Hits[iHit1]->GetTime());
    GetHisto("EVetoEnergy") -> Fill(Hits[iHit1]->GetEnergy() );
    ((TH2F *) GetHisto("EVetoTimeVsChannelID"))->Fill(Hits[iHit1]->GetChannelId(), Hits[iHit1]->GetTime());
    
    

  }    

  char name[256];
  
  int ih1,ih2;

  for(int i = 1; i < 95; i++) {
    if(charges[i] > 0. && charges[i-1] < 0. && charges[i+1] < 0.) {      
      sprintf(name, "EVetoCharge-%d", i);
      GetHisto(name)->Fill(charges[i]);
      GetHisto("EVetoEnergyClean") -> Fill(charges[i] );

    }
  }
  

  
  for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
    for(unsigned int iHit2 = 0; iHit2 < Hits.size();++iHit2) {

      if(Hits[iHit1]->GetTime() > 20. && Hits[iHit2]->GetTime() > 20.) {
	(  (TH2F *) GetHisto("EVetoTimeVsEVetoTime"))  ->Fill(Hits[iHit1]->GetTime(),Hits[iHit2]->GetTime());
	GetHisto("EVetoHitTimeDifference")->Fill(Hits[iHit1]->GetTime() - Hits[iHit2]->GetTime());
      }


      if(Hits[iHit1]->GetChannelId() + 1 ==  Hits[iHit2]->GetChannelId()   ) {

	sprintf(name, "EVetoDTch%dch%d", Hits[iHit1]->GetChannelId() ,Hits[iHit1]->GetChannelId()+1);
	//	if(Hits[iHit1]->GetTime() > 20. && Hits[iHit2]->GetTime() > 20.) {	  
	  GetHisto(name)->Fill(Hits[iHit1]->GetTime() - Hits[iHit2]->GetTime());
	  //	}
      }
    }
  }


}

void EVetoReconstruction::BuildHits(TRawEvent* rawEv)//copied from ECal 24/6/19 to have board & channel ID in digitizer
{
  ClearHits();
  vector<TRecoVHit *> &Hits  = GetRecoHits();
  ((DigitizerChannelEVeto*)fChannelReco)->SetTrigMask(GetTriggerProcessor()->GetTrigMask());
  UChar_t nBoards = rawEv->GetNADCBoards();
  ((DigitizerChannelEVeto*)fChannelReco)->SetEventNumber(rawEv->GetEventNumber());
  TADCBoard* ADC;

  for(Int_t iBoard = 0; iBoard < nBoards; iBoard++) {
    ADC = rawEv->ADCBoard(iBoard);
    Int_t iBdID=ADC->GetBoardId();
    //    std::cout<<"iBdID "<<iBdID<<std::endl;
    if(GetConfig()->BoardIsMine( ADC->GetBoardId())) {
      //Loop over the channels and perform reco
      for(unsigned ich = 0; ich < ADC->GetNADCChannels();ich++) {
	TADCChannel* chn = ADC->ADCChannel(ich);
	fChannelReco->SetDigis(chn->GetNSamples(),chn->GetSamplesArray());

	//New M. Raggi
 	Int_t ChID   = GetChannelID(ADC->GetBoardId(),chn->GetChannelNumber()); //give the geographical position
	//	std::cout<<"Event no "<<rawEv->GetEventNumber()<<" ChID "<<ChID<<std::endl; 
	Int_t ElChID = chn->GetChannelNumber();
	//Store info for the digitizer class
 	((DigitizerChannelEVeto*)fChannelReco)->SetChID(ChID);
 	((DigitizerChannelEVeto*)fChannelReco)->SetElChID(ElChID);
 	((DigitizerChannelEVeto*)fChannelReco)->SetBdID(iBdID);
	
	unsigned int nHitsBefore = Hits.size();
	fChannelReco->Reconstruct(Hits);
	unsigned int nHitsAfter = Hits.size();
	for(unsigned int iHit = nHitsBefore; iHit < nHitsAfter;++iHit) {
	  Hits[iHit]->SetChannelId(GetChannelID(ADC->GetBoardId(),chn->GetChannelNumber()));
	  Hits[iHit]->setBDCHid( ADC->GetBoardId(), chn->GetChannelNumber() );
	  if(fTriggerProcessor)
	    Hits[iHit]->SetTime(
				Hits[iHit]->GetTime() - 
				fTriggerProcessor->GetChannelTriggerTime( ADC->GetBoardId(), chn->GetChannelNumber() )
				);
	}
      }
    } else {
      //std::cout<<GetName()<<"::Process(TRawEvent*) - unknown board .... "<<std::endl;
    }
  }    
}
