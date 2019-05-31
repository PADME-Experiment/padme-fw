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

int waveformmax = 10;//number of waveforms to be considered

void PVetoReconstruction::HistoInit(){
  AddHisto("nboards", new TH1F("nboards","Number of boards",100,0.0,100.0));
  AddHisto("ADCs",new TH1F("ADCs","ADC ID",100,0.0,100.));
  AddHisto("nchannels", new TH1F("nchannels","Number of channels",100,0.0,100.0));
  AddHisto("ntriggers", new TH1F("ntriggers","Number of trigger channels",100,0.0,100.0));
  AddHisto("HitTimeDifference",new TH1F("HitTimeDifference","Difference in time",400,-100.,100.));
  AddHisto("PVetoOccupancy",new TH1F("PVetoOccupancy","PVeto Occupancy",100,0.0,100.0));
  AddHisto("PVetoOccupancyLast",new TH1F("PVetoOccupancyLast","PVeto OccupancyLast",100,0.0,100.0));


  AddHisto("PVetoEnergy",new TH1F("PVetoEnergy","PVeto Energy",1200,0.0,12.0));
  AddHisto("PVetoTime",new TH1F("PVetoTime","PVeto Time",400,0.0,400.0));
 
  AddHisto("PVetoTimeVsChannelID",new TH2F("PVetoTimeVsChannelID","PVeto Time vs Ch. ID",100,0,100,100,0.0,400.0) );
  AddHisto("PVetoTimeVsPVetoTime",new TH2F("PVetoTimeVsPVetoTime","PVeto Time vs PVetoTime",400,0.0,400.0, 400,0.0,400.0));

 


  char name[256];

  for (int i=0; i<95; i++) { 
    sprintf(name, "PVetoDTch%dch%d",i,i+1);
    AddHisto(name, new TH1F(name,"Difference in time",400,-10.,10.));
  }
  
  for(int waveformno =0; waveformno<waveformmax; waveformno++){
    sprintf(name, "PVetoWaveform%d",waveformno);
    AddHisto(name, new TH1F(name,"PVeto Waveform",1024,0,1024));
    sprintf(name, "SmoothedPVetoWaveform%d",waveformno);
    AddHisto(name, new TH1F(name,"Smoothed PVeto Waveform",1024,0,1024));
    sprintf(name, "PVetoWaveformderivative%d",waveformno);
    AddHisto(name, new TH1F(name,"PVeto Waveform Derivative",1024,0,1024));
    sprintf(name, "SmoothedPVetoWaveformderivative%d",waveformno);
    AddHisto(name, new TH1F(name,"Smoothed PVeto Waveform Derivative",1024,0,1024));
    sprintf(name, "PVetoWaveformderivativeRatio%d",waveformno);
    AddHisto(name, new TH1F(name,"PVeto Waveform Derivative Ratio",1024,0,1024));
    sprintf(name, "SmoothedPVetoWaveformderivativeRatio%d",waveformno);
    AddHisto(name, new TH1F(name,"Smoothed PVeto Waveform Derivative Ratio",1024,0,1024));
  }

  //  AddHisto("PVetoDTch1ch2",new TH1F("PVetoDTch1ch2","Difference in time",100,-10.,10.));

}


void PVetoReconstruction::ProcessEvent(TMCVEvent* tEvent, TMCEvent* tMCEvent)
{
  PadmeVReconstruction::ProcessEvent(tEvent,tMCEvent);
  TPVetoMCEvent* tPVetoEvent = (TPVetoMCEvent*)tEvent;
  //std::cout << "--- PVetoReconstruction --- run/event/#hits/#digi " << tPVetoEvent->GetRunNumber() << " " << tPVetoEvent->GetEventNumber() << " " << tPVetoEvent->GetNHits() << " " << tPVetoEvent->GetNDigi() << std::endl;
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

  // std::cout << "--- PVetoReconstruction --- Event number "<<rawEv<<std::endl;

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

   static int waveformhit =0; //static means that it isn't reset when AnalyzeEvent() is called during the loop over events
  //  int waveformhit;
  for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
    //    if(Hits[iHit1]->GetTime() < 10.) continue;
    //    std::cout<<"iHit1 = " << iHit1 << " passes time cut "<<std::endl;
    int derivbins = 10;
    int nsmoothing =1;//Number of samples either side used to smooth the signal
    char name[256];
    double Signal[1024];
    double SmoothedSignal[1024];

    if (waveformhit < waveformmax){
      std::cout<<"waveformhit before = "<<waveformhit<<std::endl;
      
      for(int SignalSample = 0; SignalSample < 1024; SignalSample++){
	Signal[SignalSample] = (Hits[iHit1]->GetWaveform())[SignalSample];
      }
      
      for(int SmoothSample = 0; SmoothSample < 1024; ++SmoothSample){
	double SignalTot=0;
	//	std::cout<< "SmoothSample at the beginning is "<<SmoothSample<<" Signal at the beginning is "<< Signal[SmoothSample]<<std::endl;
	for(int k = -1*nsmoothing;k<nsmoothing+1;k++){
	  if(SmoothSample <nsmoothing||SmoothSample>(1024-nsmoothing)) continue;
	  SignalTot += Signal[SmoothSample+k];
	  //std::cout<< "SmoothSample here is "<<SmoothSample<<" k is "<< k << " Signal[SmoothSample+k] is "<< Signal[SmoothSample+k] << " SignalTot is "<< SignalTot<<std::endl;
	}
	SmoothedSignal[SmoothSample]=SignalTot/(2.*nsmoothing+1.);

      }

      for(int iSample = 0; iSample < 1024; ++iSample){
	//std::cout<<"iSample = "<<iSample<<std::endl;
	double SignalDerivative = 0;
	if(iSample>=derivbins&&iSample<1024){ //otherwise the derivative doesn't make sense - you can't calculate the derivative of the first bin because you have nothing before it to compare it to because the first position in SignalSmoothed will be 0
	  //	SignalDerivative = (Signal[iSample+derivbins]-Signal[iSample-derivbins])/((2*derivbins)*0.4); //0.4 = timebin = time between samples. Samples are taken at 2.5GHz, so time =1/2.5e9 = 4ns. Maybe there's a way of reading this directly from the config file in case the frequency changes?
	  SignalDerivative = (Signal[iSample]-Signal[iSample-derivbins])/((derivbins)*0.4); //Calculating the derivative only backwards gives the highest possible value for the derivative as it isn't confused by the peak
	}
	double SignalDerivativeSmoothed =0;
       	if(iSample>=derivbins&&iSample<1024){
	  //	  SignalDerivativeSmoothed = (SmoothedSignal[iSample+derivbins]-SmoothedSignal[iSample-derivbins])/((2*derivbins)*0.4);
	  SignalDerivativeSmoothed = (SmoothedSignal[iSample]-SmoothedSignal[iSample-derivbins])/((derivbins)*0.4);
	}

	//	std::cout<<"iSample = "<< iSample << " Signal = "<<Signal[iSample]<<" smoothed signal = "<<SmoothedSignal[iSample]<<std::endl;
	//	if(iSample+derivbins<1024){
	  //std::cout<<"Signal = "<<Signal[iSample]<< " Signal + derivbins = "<<Signal[iSample + derivbins]<<" Signal - derivbins = "<<Signal[iSample - derivbins]<<std::endl;
	//std::cout<<"Smoothed signal = "<<SmoothedSignal[iSample]<< " Smoothed signal + derivbins = "<<SmoothedSignal[iSample + derivbins]<<" Smoothed signal - derivbins = "<<SmoothedSignal[iSample - derivbins]<<std::endl;
	//	}
	//	std::cout<<"SignalDerivative = "<<SignalDerivative<<" SignalDerivativeSmoothed = "<<SignalDerivativeSmoothed<<std::endl;
	double DerivativetoSignalRatio = SignalDerivative/Signal[iSample];
	//	if(SmoothedSignal[iSample]==0) std::cout<<"iSample = "<< iSample << " Signal = "<<Signal[iSample]<<" smoothed signal = "<<SmoothedSignal[iSample]<<std::endl;
	double DerivativetoSignalRatioSmoothed = SignalDerivativeSmoothed/SmoothedSignal[iSample];
	if(waveformhit == 3){
	  //	  std::cout<<"iSample = "<< iSample << " Signal = "<<Signal[iSample]<<" smoothed signal = "<<SmoothedSignal[iSample]<<std::endl;
	}
	sprintf(name, "PVetoWaveform%d",waveformhit);
	GetHisto(name)->SetBinContent(iSample+1,Signal[iSample]);//iSample starts from 0, whereas bins start from 1. Therefore, fill the Signal into bin number iSample+1, giving the waveform
	sprintf(name, "SmoothedPVetoWaveform%d",waveformhit);
	GetHisto(name)->SetBinContent(iSample+1,SmoothedSignal[iSample]);
	sprintf(name, "PVetoWaveformderivative%d",waveformhit);
	GetHisto(name)->SetBinContent(iSample+1,SignalDerivative);
	sprintf(name, "SmoothedPVetoWaveformderivative%d",waveformhit);
	GetHisto(name)->SetBinContent(iSample+1,SignalDerivativeSmoothed);

	if(Signal[iSample]!=0){//waveform 3 has some bins which are 0. If this is true, the ratio will be infinity, so these bins are not filled
	  sprintf(name, "PVetoWaveformderivativeRatio%d",waveformhit);
	  GetHisto(name)->SetBinContent(iSample+1,DerivativetoSignalRatio);
	}
	else if(Signal[iSample]==0&&iSample>nsmoothing&&iSample<1024-nsmoothing) std::cout<<"Signal is 0 at waveform number "<<waveformhit<<" sample number "<<iSample<<". This bin is not filled in PVetoWaveformderivativeRatio"<<waveformhit<<"."<<std::endl;


	if(SmoothedSignal[iSample]!=0){//when smoothed, waveform 3 has some bins which are 0. If this is true, the ratio will be infinity, so these bins are not filled
	  if(iSample>=nsmoothing&&iSample<1024-nsmoothing){//at iSample = 0, SmoothedSignal[iSample]=0 by definition, meaning that DerivativetoSignalRatioSmoothed will be infinity
	    sprintf(name, "SmoothedPVetoWaveformderivativeRatio%d",waveformhit);
	    GetHisto(name)->SetBinContent(iSample+1,DerivativetoSignalRatioSmoothed);
	  }
	}
	else if(SmoothedSignal[iSample]==0&&iSample>nsmoothing&&iSample<1024-nsmoothing) std::cout<<"Smoothed Signal is 0 at waveform number "<<waveformhit<<" sample number "<<iSample<<". This bin is not filled in SmoothedPVetoWaveformderivativeRatio"<<waveformhit<<"."<<std::endl;

      }
      waveformhit++;
      //      std::cout<<"waveformhit after = "<<waveformhit<<std::endl;
      //      GetHisto("PVetoWaveform")->Write();
    }


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

  }


  char name[256];
  
  int ih1,ih2;

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
