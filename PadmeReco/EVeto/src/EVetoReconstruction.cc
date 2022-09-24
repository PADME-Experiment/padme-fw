// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
// Modified by Beth Long 2022-04-07 
// --------------------------------------------------------------
#include "Riostream.h"

#include "EVetoReconstruction.hh"

#include "TEVetoMCEvent.hh"
#include "TEVetoMCHit.hh"
#include "TEVetoMCDigi.hh"
#include "DigitizerChannelEVeto.hh"
#include "EVetoCalibration.hh"
#include "EVetoSimpleClusterization.hh"
#include "EVetoGeometry.hh"
#include "EVetoClusterization.hh"

#include "TH1F.h"
#include "TH2F.h"
//#include "TRandom2.h"
#include <time.h>


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

  random = new TRandom2();    
  gRandom->SetSeed(time(NULL));

  // configurable parameters 
  fSigmaNoiseForMC         = (Double_t)fConfig->GetParOrDefault("RECO", "SigmaNoiseForMC", .4);
  fEVetoDigiTimeWindow     = (Double_t)fConfig->GetParOrDefault("RECO", "DigitizationTimeWindowForMC", 17.);

}

void EVetoReconstruction::HistoInit(){

  AddHisto("EVetoOccupancy",new TH1F("EVetoOccupancy","EVeto Occupancy",100,0.0,100.0));
  AddHisto("EVetoEnergy",new TH1F("EVetoEnergy","EVeto Energy",1200,0.0,12.0));
  AddHisto("EVetoEnergyClean",new TH1F("EVetoEnergyClean","EVeto Energy",2000,0.0,.4));
  //AddHisto("EVetoTime",new TH1F("EVetoTime","EVeto Time",400,0.0,400.0));
  AddHisto("EVetoTime",new TH1F("EVetoTime","EVeto Time",400,-150.0,250.0));
  AddHisto("EVetoTimeVsChannelID",new TH2F("EVetoTimeVsChannelID","EVeto Time vs Ch. ID",100,0,100,100,0.0,400.0));
  AddHisto("EVetoHitTimeDifference",new TH1F("EVetoHitTimeDifference","Difference in time",400,-100.,100.));
  //AddHisto("EVetoTimeVsEVetoTime",new TH2F("EVetoTimeVsEVetoTime","EVeto Time vs EVetoTime",400,0.0,400.0, 400,0.0,400.0));
  AddHisto("EVetoTimeVsEVetoTime",new TH2F("EVetoTimeVsEVetoTime","EVeto Time vs EVetoTime",400,-150.0,250.0, 400,-150.0,250.0));

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


EVetoReconstruction::~EVetoReconstruction()
{;}


void EVetoReconstruction::ProcessEvent(TRawEvent* rawEv){//Beth 22/2/22: copied from virtual class to override virtual class. I removed the calibration it's done by gain equalisation directly in digitizer. I will want to change as it  will use the new battleships algorithm

  //  std::cout<<"!?><using eveto process event"<<std::endl;

  // use trigger info 
  if(fTriggerProcessor) {
    //std::cout<<"Reconstruction named <"<<GetName()<<"> processing TriggerInfo .... "<<std::endl;
    BuildTriggerInfo(rawEv);
    if (TriggerToBeSkipped()) return;
  }
    
  // from waveforms to Hits
  BuildHits(rawEv);

  if(fGeometry)           fGeometry->ComputePositions(GetRecoHits());
  //  std::cout<<"about to clusterise eveto"<<std::endl;
  // from Hits to Clusters
  ClearClusters();
  BuildClusters(rawEv);
  //  if(fChannelCalibration) fChannelCalibration->PerformCalibration(GetClusters());

  //Processing is over, let's analyze what's here, if foreseen
  if(fGlobalRecoConfigOptions->IsMonitorMode()) {
    AnalyzeEvent(rawEv);
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


void EVetoReconstruction::ConvertMCDigitsToRecoHits(TMCVEvent* tEvent,TMCEvent* tMCEvent) {

  if (tEvent==NULL) return;
  for(Int_t i=0; i < fHits.size(); i++) delete fHits[i];
  fHits.clear();
  // MC to reco hits
  for (Int_t i=0; i<tEvent->GetNDigi(); ++i) {
    TMCVDigi* digi = tEvent->Digi(i);
    //TRecoVHit *Hit = new TRecoVHit(digi);

    Int_t    digiCh = digi->GetChannelId();
    //digit Id increases with decreasing z; for recoHits chId increases with increasing z 

    Double_t digiT  = digi->GetTime();
    Double_t digiE  = digi->GetEnergy();
    Bool_t toBeMerged = false;
    // merge digits in the same channel closer in time than a configurable parameter (fEvetoDigiTimeWindow){
    if (fEVetoDigiTimeWindow > 0) {
      for (unsigned int ih=0; ih<fHits.size(); ++ih)
	{
	  if (fHits[ih]->GetChannelId() != digiCh) continue;
	  if (fabs(fHits[ih]->GetTime()/fHits[ih]->GetEnergy()-digiT)<fEVetoDigiTimeWindow)
	    {
	      toBeMerged = true;
	      // this digit must be merged with a previously defined recoHit
	      fHits[ih]->SetEnergy(fHits[ih]->GetEnergy() + digiE);
	      fHits[ih]->SetTime(fHits[ih]->GetTime() + digiE*digiT);
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
    // end of merge digits in the same channel closer in time than a configurable parameter (fEvetoDigiTimeWindow){
  return;

  /*
  if (tEvent==NULL) return;
  random = new TRandom2();    
  gRandom->SetSeed(time(NULL));
  fHits.clear();
  // MC to reco hits
  for (Int_t i=0; i<tEvent->GetNDigi(); ++i) {
    TMCVDigi* digi = tEvent->Digi(i);
    //TRecoVHit *Hit = new TRecoVHit(digi);
    TRecoVHit *Hit = new TRecoVHit();
    Hit->SetChannelId(digi->GetChannelId());
    Double_t sigma = 0.4;
    Double_t Noise=random->Gaus(0.,sigma); 
    Hit->SetEnergy(digi->GetEnergy()+Noise);
    //Hit->SetEnergy(digi->GetEnergy());
    Hit->SetTime(digi->GetTime());
    Hit->SetPosition(TVector3(0.,0.,0.)); 
    fHits.push_back(Hit);
  }
  */
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

void EVetoReconstruction::BuildClusters(TRawEvent* rawEv)
{
  EVetoClusterHits EVetoClusterHit;// = new ClusterHits();
  EVetoClusterStructure EVetoClusStruc;//contains a structure for vectors of clusters for each event
  std::vector<EVetoClusterHits> EVetoClusterHitVec;//Contains all the EVetoHits to be clusterised per event
  Int_t nhitpass=0;
  std::vector<EVetoCluster*> vEVetoClusters;

  vector<TRecoVHit *> &Hits  = GetRecoHits();
  std::vector<TRecoVCluster *> &myClusters = GetClusters();
  myClusters.clear();

  TRecoVCluster* myCl;
  EVetoClusterHitVec.clear();

  for(int iHit=0;iHit<Hits.size();iHit++){
    //    if(Hits[iHit]->GetEnergy()<0) std::cout<<Hits[iHit]->GetEnergy()<<std::endl;
    EVetoClusterHit.Clear();
    //	std::cout<<"ChID "<<EVetoClusterHit.GetChannelId()<<" time "<<EVetoClusterHit.GetTime()<<std::endl;
    EVetoClusterHit.SetEnergy(Hits[iHit]->GetEnergy());
    EVetoClusterHit.SetTime(Hits[iHit]->GetTime());
    EVetoClusterHit.SetChannelId(Hits[iHit]->GetChannelId());
    EVetoClusterHit.SetPosition(Hits[iHit]->GetPosition());
    EVetoClusterHitVec.push_back(EVetoClusterHit);
    //	std::cout<<"Making the Cluster Hits ChID "<<EVetoClusterHit.GetChannelId()<<" time "<<EVetoClusterHit.GetTime()<<std::endl;
  }
  //  std::cout<<"Eveto hit size "<< EVetoClusterHitVec.size()<<std::endl;
  vEVetoClusters.clear();
  EVetoClusStruc.Clear();//contains a structure for vectors of clusters for each event

  for(Int_t iEHit=0;iEHit<EVetoClusterHitVec.size();iEHit++){
    //      std::cout<<"Reading Cluster Hits ChID "<<EVetoClusterHitVec[iEHit].GetChannelId()<<" time "<<EVetoClusterHitVec[iEHit].GetTime()<<std::endl;
    //   if(EVetoClusterHitVec[iEHit].GetEnergy()>0.5){
      nhitpass++;
      
      EVetoClusStruc.AddHit(EVetoClusterHitVec[iEHit],iEHit);
      
  }

  // EVetoClusStruc.HitSort();//sort hits in time
  EVetoClusStruc.Clusterise();//clusterise hits
  EVetoClusStruc.MergeClusters();//merge adjacent, in time clusters
  vEVetoClusters = EVetoClusStruc.GetClusters();//vector of clusters
  std::vector<Int_t> clHitIndices;

  //  std::cout<<vEVetoClusters.size()<<std::endl;

  for(int iEClus=0;iEClus<vEVetoClusters.size();iEClus++){
    myCl = new TRecoVCluster();
    clHitIndices.clear();

    int chID;
    double clE;
    double clT;
    double clX;
    double clY;
    double clZ;
    int clSize;

    chID = vEVetoClusters[iEClus]->GetMostUpstreamChannel();
    clE = vEVetoClusters[iEClus]->GetEnergy();
    clT = vEVetoClusters[iEClus]->GetAverageTime();
    clSize = vEVetoClusters[iEClus]->GetNHits();
    TVector3 clPos = fGeometry->LocalPosition(chID);

    clHitIndices = vEVetoClusters[iEClus]->GetHitIndex();

    /*    if(clE>100){
      for(int ii=0;ii<clSize;ii++){
	std::cout<<"rawEvNo "<<rawEv->GetEventNumber()<<" EVeto clE "<<clE<<std::endl;
	std::cout<<" ii "<<ii<<std::endl;
	std::cout<<" no. hits "<<clHitIndices.size()<<std::endl;
	std::cout<<" hit "<<clHitIndices[ii]<<std::endl;
	std::cout<< " hitE "<<Hits[clHitIndices[ii]]->GetEnergy()<<std::endl;
      }
      }*/

    myCl->SetChannelId   ( chID );
    myCl->SetEnergy      ( clE );
    myCl->SetTime        ( clT );
    myCl->SetPosition    ( clPos );
    myCl->SetNHitsInClus ( clSize );
    myCl->SetHitVecInClus( clHitIndices );
    //     myCl->SetSeed        ( iSeed );

    myClusters.push_back(myCl);
    //    std::cout<<"my clusters size "<<myClusters.size()<<std::endl;
  }
  //  std::cout<<"size "<<myClusters.size()<<std::endl;
}


bool EVetoReconstruction::TriggerToBeSkipped()
{
  //if ( GetGlobalRecoConfigOptions()->IsRecoMode()    && !(GetTriggerProcessor()->IsBTFTrigger())     ) return true;
  if ( GetGlobalRecoConfigOptions()->IsPedestalMode()&& !(GetTriggerProcessor()->IsAutoTrigger())    ) return true;
  if ( GetGlobalRecoConfigOptions()->IsCosmicsMode() && !(GetTriggerProcessor()->IsCosmicsTrigger()) ) return true;
  return false; 
}
