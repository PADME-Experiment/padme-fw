// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2022-06-16
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "ETagReconstruction.hh"

#include "TETagMCEvent.hh"
#include "TETagMCHit.hh"
#include "TETagMCDigi.hh"
#include "TETagRecoEvent.hh"
//#include "DigitizerChannelReco.hh"
#include "DigitizerChannelETag.hh"
#include "ETagCalibration.hh"
#include "ETagGeometry.hh"
#include "ETagSimpleClusterization.hh"
#include "ADCChannelVReco.hh"

#include "TH1F.h"
#include "TH2F.h"
#include "TDirectory.h"

ETagReconstruction::ETagReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "ETag", ConfigFileName)
{

  //fChannelReco = new DigitizerChannelReco();
  fChannelReco = new DigitizerChannelETag();
  //  fChannelCalibration = new ETagCalibration();
  fClusterization = new ETagSimpleClusterization();
  fTriggerProcessor = new PadmeVTrigger();
  fGeometry = new ETagGeometry();

  random = new TRandom2();    
  gRandom->SetSeed(time(NULL));

  // configurable parameters 
  fSigmaNoiseForMC    = (Double_t)fConfig->GetParOrDefault("RECO", "SigmaNoiseForMC", .4);
  fETagDigiTimeWindow = (Double_t)fConfig->GetParOrDefault("RECO", "DigitizationTimeWindowForMC", 17.);

}

ETagReconstruction::~ETagReconstruction()
{;}

void ETagReconstruction::HistoInit(){
  AddHisto("nboards", new TH1F("nboards","Number of boards",100,0.0,100.0));
  AddHisto("ADCs",new TH1F("ADCs","ADC ID",100,0.0,100.));
  AddHisto("nchannels", new TH1F("nchannels","Number of channels",100,0.0,100.0));
  AddHisto("ntriggers", new TH1F("ntriggers","Number of trigger channels",100,0.0,100.0));
  //AddHisto("HitTimeDifference",new TH1F("HitTimeDifference","Difference in time",400,-100.,100.));
  AddHisto("HitTimeDifference",new TH1F("HitTimeDifference","Difference in time",400,-40.,40.));
  AddHisto("ETagOccupancy",new TH1F("ETagOccupancy","ETag Occupancy",100,0.0,100.0));
  AddHisto("ETagOccupancyLast",new TH1F("ETagOccupancyLast","ETag OccupancyLast",100,0.0,100.0));
  AddHisto("ETagEnergy",new TH1F("ETagEnergy","ETag Energy",2000,0.0,40.0));
  AddHisto("ETagEnergyClean",new TH1F("ETagEnergyClean","ETag Energy",2000,0.0,.4));
  //AddHisto("ETagTime",new TH1F("ETagTime","ETag Time",600,-200.0,400.0));
  AddHisto("ETagTime",new TH1F("ETagTime","ETag Time",400,-150.0,250.0));
  AddHisto("ETagTimeVsChannelID",new TH2F("ETagTimeVsChannelID","ETag Time vs Ch. ID",100,0,100,100,-200.0,200.0) );
  AddHisto("ETagTimeVsETagTime",new TH2F("ETagTimeVsETagTime","ETag Time vs ETagTime",400,-200.0,200.0, 400,-200.0,200.0));

  char name[256];

  //for (int i=0; i<95; i++) { 
  //  sprintf(name, "ETagDTch%dch%d",i,i+1);
  //  AddHisto(name, new TH1F(name,"Difference in time",100,-25.,25.));
  //}
  //
  //for (int i=0; i<96; i++) { 
  //  sprintf(name, "ETagCharge-%d",i);
  //  AddHisto(name, new TH1F(name,"Charge",2000,00.,.4));
  //}

  //AddHisto("ETagDTch1ch2",new TH1F("ETagDTch1ch2","Difference in time",100,-10.,10.));

}


/* only for debugging 
void ETagReconstruction::ProcessEvent(TMCVEvent* tEvent, TMCEvent* tMCEvent)
{
  PadmeVReconstruction::ProcessEvent(tEvent,tMCEvent);
  TETagMCEvent* tETagEvent = (TETagMCEvent*)tEvent;
  std::cout << "--- ETagReconstruction --- run/event/#hits/#digi " << tETagEvent->GetRunNumber() << " " << tETagEvent->GetEventNumber() << " " << tETagEvent->GetNHits() << " " << tETagEvent->GetNDigi() << std::endl;
  for (Int_t iH=0; iH<tETagEvent->GetNHits(); iH++) {
    TETagMCHit* hit = (TETagMCHit*)tETagEvent->Hit(iH);
    hit->Print();
  }
  for (Int_t iD=0; iD<tETagEvent->GetNDigi(); iD++) {
    TETagMCDigi* digi = (TETagMCDigi*)tETagEvent->Digi(iD);
    digi->Print();
  }

}
*/

void ETagReconstruction::ConvertMCDigitsToRecoHits(TMCVEvent* tEvent,TMCEvent* tMCEvent) 
{

  if (tEvent==NULL) return;

  for(Int_t i=0; i<fHits.size(); i++) delete fHits[i];
  fHits.clear();

  // MC to reco hits
  for (Int_t i=0; i<tEvent->GetNDigi(); i++) {

    TMCVDigi* digi = tEvent->Digi(i);
    Int_t    digiCh = digi->GetChannelId();
    Double_t digiT  = digi->GetTime();
    Double_t digiE  = digi->GetEnergy();


    // merge digits in the same channel closer in time than a configurable parameter (fETagDigiTimeWindow){
    Bool_t toBeMerged = false;
    if (fETagDigiTimeWindow > 0) {
      for (unsigned int ih=0; ih<fHits.size(); ++ih) {
	if (fHits[ih]->GetChannelId() != digiCh) continue;
	//	std::cout<<"Digit n. "<<i<<" Ch="<<digiCh<<" time "<<digiT<<" nhits so far = "<<fHits.size()<<" "<<fHits[ih]->GetChannelId()<<std::endl;
	if (fabs(fHits[ih]->GetTime()/fHits[ih]->GetEnergy()-digiT)<fETagDigiTimeWindow) {
	  toBeMerged = true; // this digit must be merged with a previously defined recoHit
	  fHits[ih]->SetEnergy(fHits[ih]->GetEnergy() + digiE);
	  fHits[ih]->SetTime(fHits[ih]->GetTime() + digiE*digiT);
	}
      }
    }
    if (!toBeMerged) {
      //      std::cout<<"Pushing Digit n. "<<i<<" Ch="<<digiCh<<" time "<<digiT<<" nhits so far = "<<fHits.size()<<std::endl;
      TRecoVHit* Hit = new TRecoVHit();
      Hit->SetChannelId(digiCh);
      Hit->SetEnergy   (digiE);
      Hit->SetTime     (digiT*digiE);
      Hit->SetPosition (TVector3(0.,0.,0.)); 
      fHits.push_back(Hit);
    }

  }
  // last loop to correct the time 
  TRecoVHit* Hit;
  Double_t Noise=0.;
  for (unsigned int ih=0; ih<fHits.size(); ++ih) {
    Hit = fHits[ih];
    Hit->SetTime(Hit->GetTime()/Hit->GetEnergy());
//    if (fSigmaNoiseForMC > 0.0001) {
//      Noise=random->Gaus(0.,fSigmaNoiseForMC);   
//      Hit->SetEnergy(Hit->GetEnergy()+Noise);
//    }
  }
  // end of merge digits in the same channel closer in time than a configurable parameter (fETagDigiTimeWindow){

  return;
}

void ETagReconstruction::AnalyzeEvent(TRawEvent* rawEv){

  //float charges[ETAG_NUMBER_OF_CHANNELS];
  //for(int i=0;i<ETAG_NUMBER_OF_CHANNELS;i++) charges[i] = -1.;

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

    GetHisto("ETagOccupancy")->Fill(Hits[iHit1]->GetChannelId());
    GetHisto("ETagTime")->Fill(Hits[iHit1]->GetTime());

    (  (TH2F *) GetHisto("ETagTimeVsChannelID"))  ->Fill(Hits[iHit1]->GetChannelId(), Hits[iHit1]->GetTime());

    for(unsigned int iHit2 = iHit1+1; iHit2 < Hits.size();++iHit2) {
      ((TH2F *)GetHisto("ETagTimeVsETagTime"))->Fill(Hits[iHit1]->GetTime(),Hits[iHit2]->GetTime());
      if( Hits[iHit1]->GetChannelId() > 20 && Hits[iHit1]->GetChannelId() < 70
	  && Hits[iHit2]->GetChannelId() > 20 && Hits[iHit2]->GetChannelId() < 70 ) {
	GetHisto("HitTimeDifference")->Fill(Hits[iHit1]->GetTime() - Hits[iHit2]->GetTime());
      }
    }

    GetHisto("ETagEnergy")->Fill(Hits[iHit1]->GetEnergy());
    int chid = Hits[iHit1]->GetChannelId();
    
    //charges[Hits[iHit1]->GetChannelId()] = Hits[iHit1]->GetEnergy();
    
  }
  
  char name[256];

  //for(int i = 1; i < ETAG_NUMBER_OF_CHANNELS-1; i++) {
  //  if(charges[i] > 0. && charges[i-1] < 0. && charges[i+1] < 0.) {      
  //    sprintf(name, "ETagCharge-%d", i);
  //    //GetHisto(name)->Fill(charges[i]);
  //    GetHisto("ETagEnergyClean") -> Fill(charges[i] );
  //  }
  //}
  
  for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
    for(unsigned int iHit2 = 0; iHit2 < Hits.size();++iHit2) {
      if(Hits[iHit1]->GetChannelId()+1 == Hits[iHit2]->GetChannelId()) {
	sprintf(name,"ETagDTch%dch%d",Hits[iHit1]->GetChannelId(),Hits[iHit2]->GetChannelId());
	//GetHisto(name)->Fill(Hits[iHit1]->GetTime() - Hits[iHit2]->GetTime());
      }
    }
  }

  if(GetHisto("ETagOccupancyLast")->GetEntries())
    for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1)
      GetHisto("ETagOccupancyLast")->Fill(Hits[iHit1]->GetChannelId());
}


void ETagReconstruction::BuildClusters()
{

  
  vector<TRecoVCluster *> &myClusters  = GetClusters();
  for(unsigned int iCl = 0;iCl < myClusters.size();iCl++){
    delete myClusters[iCl];
  }
  myClusters.clear();

  vector<TRecoVHit *> &Hits  = GetRecoHits();
  if(Hits.size()==0){
    return;
  }
  //  std::cout<<"Mi chiamo Etag e sono il nuovo detector "<<std::endl;
  if (fClusterization) fClusterization->Reconstruct(Hits, myClusters);
  
}

void ETagReconstruction::BuildHits(TRawEvent* rawEv)//copied from ECal 24/6/19 to have board & channel ID in digitizer
{
  //  std::cout<<"Event no "<<rawEv->GetEventNumber()<<std::endl;
  ClearHits();
  vector<TRecoVHit *> &Hits  = GetRecoHits();
  ((DigitizerChannelETag*)fChannelReco)->SetTrigMask(GetTriggerProcessor()->GetTrigMask());
  UChar_t nBoards = rawEv->GetNADCBoards();
  ((DigitizerChannelETag*)fChannelReco)->SetEventNumber(rawEv->GetEventNumber());
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
 	((DigitizerChannelETag*)fChannelReco)->SetChID(ChID);
 	((DigitizerChannelETag*)fChannelReco)->SetElChID(ElChID);
 	((DigitizerChannelETag*)fChannelReco)->SetBdID(iBdID);
	
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
