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
#include "DigitizerChannelReco.hh"
#include "PVetoCalibration.hh"
#include "PVetoSimpleClusterization.hh"
#include "ADCChannelVReco.hh"

#include "TH1F.h"
#include "TH2F.h"
#include "TDirectory.h"

PVetoReconstruction::PVetoReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "PVeto", ConfigFileName)
{
  fChannelReco = new DigitizerChannelReco();
  fChannelCalibration = new PVetoCalibration();
  fClusterization = new PVetoSimpleClusterization();
  //fChannelCalibration  = new PadmeVCalibration();
  fTriggerProcessor = new PadmeVTrigger();
}

PVetoReconstruction::~PVetoReconstruction()
{;}



void PVetoReconstruction::HistoInit(){
  AddHisto("nboards", new TH1F("nboards","Number of boards",100,0.0,100.0));
  AddHisto("ADCs",new TH1F("ADCs","ADC ID",100,0.0,100.));
  AddHisto("nchannels", new TH1F("nchannels","Number of channels",100,0.0,100.0));
  AddHisto("ntriggers", new TH1F("ntriggers","Number of trigger channels",100,0.0,100.0));
  AddHisto("HitTimeDifference",new TH1F("HitTimeDifference","Difference in time",400,-100.,100.));
  AddHisto("PVetoOccupancy",new TH1F("PVetoOccupancy","PVeto Occupancy",100,0.0,100.0));
  AddHisto("PVetoOccupancyLast",new TH1F("PVetoOccupancyLast","PVeto OccupancyLast",100,0.0,100.0));


  AddHisto("PVetoEnergy",new TH1F("PVetoEnergy","PVeto Energy",1200,0.0,3500.0));
  AddHisto("PVetoTime",new TH1F("PVetoTime","PVeto Time",400,0.0,400.0));

  AddHisto("PVetoTimeVsChannelID",new TH2F("PVetoTimeVsChannelID","PVeto Time vs Ch. ID",100,0,100,100,0.0,400.0) );
  AddHisto("PVetoTimeVsPVetoTime",new TH2F("PVetoTimeVsPVetoTime","PVeto Time vs PVetoTime",400,0.0,400.0, 400,0.0,400.0));

  char name[256];

  for (int i=0; i<95; i++) { //There are 96 channels (0-95), but here we compare one channel to the next, meaning only 95 histograms are needed 
    sprintf(name, "PVetoDTch%dch%d",i,i+1);
    AddHisto(name, new TH1F(name,"Difference in time",400,-10.,10.));
  }

  for (int i=0; i<96; i++) {  //Here we need one histogram for each channel
    sprintf(name, "SignalAmplitudePerEventChannel%d",i); //This histogram stores the the spectrum of signal amplitudes
    AddHisto(name, new TH1F(name,"SignalAmplitudePerEvent",100,0.0,3500.0));

    sprintf(name, "HitPedestalChannel%d",i); //This histogram stores the spectrum of hit pedestals
    AddHisto(name, new TH1F(name,"HitPedestal",100,1500.0,4000.0));

    sprintf(name, "Amplitude/ChargeRatioChannel%d",i);
    AddHisto(name, new TH1F(name,"Amplitude/ChargeRatio",100,0.0,5000.0));

    sprintf(name, "AmplitudeVsChargeChannel%d",i);
    AddHisto(name, new TH2F(name,"AmplitudeVsCharge",500,0.,4000.,1000.,0.,4.));

  }
  
  //  AddHisto("PVetoDTch1ch2",new TH1F("PVetoDTch1ch2","Difference in time",100,-10.,10.));

}


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

void PVetoReconstruction::AnalyzeEvent(TRawEvent* rawEv){

  vector<TRecoVHit *> &Hits  = GetRecoHits();

  UChar_t nBoards = rawEv->GetNADCBoards();
  GetHisto("nboards")->Fill( (Int_t) nBoards );

  TADCBoard* ADC;

  for(Int_t iBoard = 0; iBoard < nBoards; iBoard++) {
    ADC = rawEv->ADCBoard(iBoard);
    GetHisto("ADCs")->Fill(ADC->GetBoardId());
    GetHisto("nchannels")->Fill(ADC->GetNADCChannels());
    GetHisto("ntriggers")->Fill(ADC->GetNADCTriggers());
  }
  
  //  std::cout << "Number of hits " << Hits.size() << std::endl;
  
  //  return;

  double hitEmax=0;//variable to hold the maximum hit energy per event
  int channelEmax=-1;//variable to hold the channel where the maximum energy hit occurs. Initialised to -1 as sanity check that a channel exists for this hit. Some hits being assigned to channels of a random number, unsure why but this causes a seg fault
  unsigned int iHitEmax;
  char name[256];
  for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
    if(Hits[iHit1]->GetTime() < 10.) continue;

    GetHisto("PVetoOccupancy")->Fill(Hits[iHit1]->GetChannelId());
    GetHisto("PVetoTime")->Fill(Hits[iHit1]->GetTime());
    
    (  (TH2F *) GetHisto("PVetoTimeVsChannelID"))  ->Fill(Hits[iHit1]->GetChannelId(), Hits[iHit1]->GetTime());
    
    
    for(unsigned int iHit2 = iHit1+1; iHit2 < Hits.size();++iHit2) {
      
      (  (TH2F *) GetHisto("PVetoTimeVsPVetoTime"))  ->Fill(Hits[iHit1]->GetTime(),Hits[iHit2]->GetTime());
      if(Hits[iHit1]->GetTime() > 20. && Hits[iHit2]->GetTime() > 20.) {
	GetHisto("HitTimeDifference")->Fill(Hits[iHit1]->GetTime() - Hits[iHit2]->GetTime());
      }
    }        

    GetHisto("PVetoEnergy") -> Fill(Hits[iHit1]->GetEnergy() );
    
    // if((Hits[iHit1]->GetEnergy()) > 3500){
    //   std::cout<<"Emax > 3500"<<std::endl;
    // }
  }

  //  std::cout<<"channelEmax = "<<channelEmax<<std::endl;
  //  std::cout<<"hitEmax = "<<hitEmax<<std::endl;

  //  char name[256];
  
  //  int ih1,ih2;
  int MaxAmpPerChannel = -1;
  for(int chID = 0; chID<96; chID++){
    for(unsigned int iHit = 0; iHit < Hits.size();++iHit){
      if(Hits[iHit]->GetChannelId()!=chID){
	continue;}
      //if(Hits[iHit]->GetTime() < 10.) continue;
      sprintf(name, "HitPedestalChannel%d",chID);
      GetHisto(name)->Fill(Hits[iHit]->GetPed());

      sprintf(name, "AmplitudeVsChargeChannel%d",chID);
      GetHisto(name)->Fill(Hits[iHit]->GetAmplitude(),Hits[iHit]->GetCharge());

      sprintf(name,"Amplitude/ChargeRatioChannel%d",chID);
      GetHisto(name)->Fill(Hits[iHit]->GetAmplitude()/Hits[iHit]->GetCharge());

      if(Hits[iHit]->GetAmplitude()>MaxAmpPerChannel){
   	MaxAmpPerChannel = Hits[iHit]->GetAmplitude();
      }
    }
      sprintf(name, "SignalAmplitudePerEventChannel%d",chID);
      GetHisto(name)->Fill(MaxAmpPerChannel);
  }

  for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
   
    for(unsigned int iHit2 = 0; iHit2 < Hits.size();++iHit2) {
      if(Hits[iHit1]->GetChannelId() + 1 ==  Hits[iHit2]->GetChannelId()   ) {
	
	sprintf(name, "PVetoDTch%dch%d", Hits[iHit1]->GetChannelId() ,Hits[iHit1]->GetChannelId()+1);
	if(Hits[iHit1]->GetTime() > 20. && Hits[iHit2]->GetTime() > 20.) {	  
	  GetHisto(name)->Fill(Hits[iHit1]->GetTime() - Hits[iHit2]->GetTime());
	}
      }
    }
  }

  if(GetHisto("PVetoOccupancyLast")->GetEntries()){    
    for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
      GetHisto("PVetoOccupancyLast")->Fill(Hits[iHit1]->GetChannelId());
    }
  }  
  
  
}


