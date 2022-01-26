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
#include "PVetoGeometry.hh"
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
  fGeometry = new PVetoGeometry();

  random = new TRandom2();    
  gRandom->SetSeed(time(NULL));


  // configurable parameters 
  fSigmaNoiseForMC         = (Double_t)fConfig->GetParOrDefault("RECO", "SigmaNoiseForMC", .4);
  fPVetoDigiTimeWindow     = (Double_t)fConfig->GetParOrDefault("RECO", "DigitizationTimeWindowForMC", 17.);

}

PVetoReconstruction::~PVetoReconstruction()
{;}


void PVetoReconstruction::BuildHits(TRawEvent* rawEv)//copied from ECal 24/6/19 to have board & channel ID in digitizer
{
  //  std::cout<<"Event no "<<rawEv->GetEventNumber()<<std::endl;
  ClearHits();
  vector<TRecoVHit *> &Hits  = GetRecoHits();
  ((DigitizerChannelReco*)fChannelReco)->SetTrigMask(GetTriggerProcessor()->GetTrigMask());
  UChar_t nBoards = rawEv->GetNADCBoards();
  ((DigitizerChannelReco*)fChannelReco)->SetEventNumber(rawEv->GetEventNumber());
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
 	Int_t ElChID = chn->GetChannelNumber();
	//Store info for the digitizer class
 	((DigitizerChannelReco*)fChannelReco)->SetChID(ChID);
 	((DigitizerChannelReco*)fChannelReco)->SetElChID(ElChID);
 	((DigitizerChannelReco*)fChannelReco)->SetBdID(iBdID);
	
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


void PVetoReconstruction::HistoInit(){
  AddHisto("nboards", new TH1F("nboards","Number of boards",100,0.0,100.0));
  AddHisto("ADCs",new TH1F("ADCs","ADC ID",100,0.0,100.));
  AddHisto("nchannels", new TH1F("nchannels","Number of channels",100,0.0,100.0));
  AddHisto("ntriggers", new TH1F("ntriggers","Number of trigger channels",100,0.0,100.0));
  //AddHisto("HitTimeDifference",new TH1F("HitTimeDifference","Difference in time",400,-100.,100.));
  AddHisto("HitTimeDifference",new TH1F("HitTimeDifference","Difference in time",400,-40.,40.));
  AddHisto("PVetoOccupancy",new TH1F("PVetoOccupancy","PVeto Occupancy",100,0.0,100.0));
  AddHisto("PVetoOccupancyLast",new TH1F("PVetoOccupancyLast","PVeto OccupancyLast",100,0.0,100.0));


  AddHisto("PVetoEnergy",new TH1F("PVetoEnergy","PVeto Energy",2000,0.0,40.0));
  AddHisto("PVetoEnergyClean",new TH1F("PVetoEnergyClean","PVeto Energy",2000,0.0,.4));
  //AddHisto("PVetoTime",new TH1F("PVetoTime","PVeto Time",600,-200.0,400.0));
  AddHisto("PVetoTime",new TH1F("PVetoTime","PVeto Time",400,-150.0,250.0));

  AddHisto("PVetoTimeVsChannelID",new TH2F("PVetoTimeVsChannelID","PVeto Time vs Ch. ID",100,0,100,100,-200.0,200.0) );
  AddHisto("PVetoTimeVsPVetoTime",new TH2F("PVetoTimeVsPVetoTime","PVeto Time vs PVetoTime",400,-200.0,200.0, 400,-200.0,200.0));

  char name[256];

  for (int i=0; i<95; i++) { 
    sprintf(name, "PVetoDTch%dch%d",i,i+1);
    AddHisto(name, new TH1F(name,"Difference in time",100,-25.,25.));
  }
  
  for (int i=0; i<96; i++) { 
    sprintf(name, "PVetoCharge-%d",i);
    AddHisto(name, new TH1F(name,"Charge",2000,00.,.4));
  }


  //  AddHisto("PVetoDTch1ch2",new TH1F("PVetoDTch1ch2","Difference in time",100,-10.,10.));

}


/* only for debugging 
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
*/

void PVetoReconstruction::ConvertMCDigitsToRecoHits(TMCVEvent* tEvent,TMCEvent* tMCEvent) 
{
  if (tEvent==NULL) return;
  fHits.clear();
  // MC to reco hits
  //std::cout<<"New Event ----------- nDigi = "<< tEvent->GetNDigi()<<std::endl;
  for (Int_t i=0; i<tEvent->GetNDigi(); ++i) {
    TMCVDigi* digi = tEvent->Digi(i);
    //TRecoVHit *Hit = new TRecoVHit(digi);

    Int_t    digiCh = digi->GetChannelId();
    Double_t digiT  = digi->GetTime();
    Double_t digiE  = digi->GetEnergy();
    //std::cout<<"Digit n. "<<i<<" Ch="<<digiCh<<" time "<<digiT<<" nhits so far = "<<fHits.size()<<std::endl;

    Bool_t toBeMerged = false;
    // merge digits in the same channel closer in time than a configurable parameter (fPVetoDigiTimeWindow){
    if (fPVetoDigiTimeWindow > 0) {
      for (unsigned int ih=0; ih<fHits.size(); ++ih)
	{
	  if (fHits[ih]->GetChannelId() != digiCh) continue;
	  if (fabs(fHits[ih]->GetTime()/fHits[ih]->GetEnergy()-digiT)<fPVetoDigiTimeWindow)
	    {
	      toBeMerged = true;
	      // this digit must be merged with a previously defined recoHit
	      //std::cout<<" -- merging with hit in ch "<<fHits[ih]->GetChannelId()<<" at time "<<fHits[ih]->GetTime()/fHits[ih]->GetEnergy()<<" diffT = "<<fabs(fHits[ih]->GetTime()/fHits[ih]->GetEnergy()-digiT)<<std::endl;
	      fHits[ih]->SetEnergy(fHits[ih]->GetEnergy() + digiE);
	      fHits[ih]->SetTime(fHits[ih]->GetTime() + digiE*digiT);
	      //std::cout<<" -- updated  Ch "<<fHits[ih]->GetChannelId()<<" time "<<fHits[ih]->GetTime()/fHits[ih]->GetEnergy()<<" so far "<<std::endl;
	    }
	}
    }
    if (!toBeMerged)
      {
	TRecoVHit *Hit = new TRecoVHit();
	Hit->SetChannelId(digiCh);
	Hit->SetEnergy   (digiE);
	Hit->SetTime     (digiT*digiE);
	Hit->SetPosition (TVector3(0.,0.,0.)); 
	fHits.push_back(Hit);
	//std::cout<<"   New hit Ch "<<Hit->GetChannelId()<<" time "<<Hit->GetTime()/Hit->GetEnergy()<<" so far "<<fHits.size()<<" hits"<<std::endl;
      }
  }
  // last loop to correct the time 
  TRecoVHit *Hit;
  Double_t Noise=0.;
  for (unsigned int ih=0; ih<fHits.size(); ++ih)
    {
      Hit = fHits[ih];
      Hit->SetTime(Hit->GetTime()/Hit->GetEnergy());

      if (fSigmaNoiseForMC >0.0001) {
	Noise=random->Gaus(0.,fSigmaNoiseForMC);   
	Hit->SetEnergy(Hit->GetEnergy()+Noise);
      }
    }
    // end of merge digits in the same channel closer in time than a configurable parameter (fPVetoDigiTimeWindow){
  return;
}


void PVetoReconstruction::AnalyzeEvent(TRawEvent* rawEv){

  float charges[96];
  for(int i=0;i<96;i++) charges[i] = -1.;
  

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

  for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
    //    if(Hits[iHit1]->GetTime() < 10.) continue;

    GetHisto("PVetoOccupancy")->Fill(Hits[iHit1]->GetChannelId());
    GetHisto("PVetoTime")->Fill(Hits[iHit1]->GetTime());

    (  (TH2F *) GetHisto("PVetoTimeVsChannelID"))  ->Fill(Hits[iHit1]->GetChannelId(), Hits[iHit1]->GetTime());



    //for(unsigned int iHit2 = iHit1+1; iHit2 < Hits.size();++iHit2) {
    //  (  (TH2F *) GetHisto("PVetoTimeVsPVetoTime"))  ->Fill(Hits[iHit1]->GetTime(),Hits[iHit2]->GetTime());
    //  //      if(Hits[iHit1]->GetTime() > 20. && Hits[iHit2]->GetTime() > 20.) {
    //	GetHisto("HitTimeDifference")->Fill(Hits[iHit1]->GetTime() - Hits[iHit2]->GetTime());
    //	//      }
    //}        
    for(unsigned int iHit2 = iHit1+1; iHit2 < Hits.size();++iHit2) {
      ((TH2F *)GetHisto("PVetoTimeVsPVetoTime"))->Fill(Hits[iHit1]->GetTime(),Hits[iHit2]->GetTime());
      if( Hits[iHit1]->GetChannelId() > 20 && Hits[iHit1]->GetChannelId() < 70
	  && Hits[iHit2]->GetChannelId() > 20 && Hits[iHit2]->GetChannelId() < 70 ) {
	GetHisto("HitTimeDifference")->Fill(Hits[iHit1]->GetTime() - Hits[iHit2]->GetTime());
      }
    }

    GetHisto("PVetoEnergy") -> Fill(Hits[iHit1]->GetEnergy() );
    int chid = Hits[iHit1]->GetChannelId();
    
    charges[Hits[iHit1]->GetChannelId()] = Hits[iHit1]->GetEnergy();
    
  }
  
  char name[256];
  
  int ih1,ih2;

  for(int i = 1; i < 95; i++) {
    if(charges[i] > 0. && charges[i-1] < 0. && charges[i+1] < 0.) {      
      sprintf(name, "PVetoCharge-%d", i);
      GetHisto(name)->Fill(charges[i]);
      GetHisto("PVetoEnergyClean") -> Fill(charges[i] );

    }
  }
  

  for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
    for(unsigned int iHit2 = 0; iHit2 < Hits.size();++iHit2) {
      if(Hits[iHit1]->GetChannelId() + 1 ==  Hits[iHit2]->GetChannelId()   ) {

	sprintf(name, "PVetoDTch%dch%d", Hits[iHit1]->GetChannelId() ,Hits[iHit1]->GetChannelId()+1);
	//	if(Hits[iHit1]->GetTime() > 20. && Hits[iHit2]->GetTime() > 20.) {	  
	  GetHisto(name)->Fill(Hits[iHit1]->GetTime() - Hits[iHit2]->GetTime());
	  //	}
      }
    }
  }

  if(GetHisto("PVetoOccupancyLast")->GetEntries()){    
    for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
      GetHisto("PVetoOccupancyLast")->Fill(Hits[iHit1]->GetChannelId());
    }
  }  

  
  
  
  
}


