// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
// Modified by Beth Long 2022-04-07 
// --------------------------------------------------------------
#include "Riostream.h"

#include "PVetoReconstruction.hh"

#include "TPVetoMCEvent.hh"
#include "TPVetoMCHit.hh"
#include "TPVetoMCDigi.hh"
#include "TPVetoRecoEvent.hh"
//#include "DigitizerChannelReco.hh"
#include "DigitizerChannelPVeto.hh"
#include "PVetoCalibration.hh"
#include "PVetoGeometry.hh"
#include "PVetoSimpleClusterization.hh"
#include "ADCChannelVReco.hh"
#include "PVetoClusterization.hh"
#include "TRecoVCluster.hh"

#include "TH1F.h"
#include "TH2F.h"
#include "TDirectory.h"

PVetoReconstruction::PVetoReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "PVeto", ConfigFileName)
{
//  fChannelReco = new DigitizerChannelReco();
  fChannelReco = new DigitizerChannelPVeto();
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

void PVetoReconstruction::ConvertMCDigitsToRecoHits(TMCVEvent* tEvent,TMCEvent* tMCEvent) 
{
  if (tEvent==NULL) return;
  for(Int_t i=0; i < fHits.size(); i++) delete fHits[i];
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


void PVetoReconstruction::ProcessEvent(TRawEvent* rawEv){//Beth 22/2/22: copied from virtual class to override virtual class. I removed the calibration it's done by gain equalisation directly in digitizer. I will want to change as it  will use the new battleships algorithm

  //  std::cout<<"!?><using pveto process event"<<std::endl;

  // use trigger info 
  if(fTriggerProcessor) {
    //std::cout<<"Reconstruction named <"<<GetName()<<"> processing TriggerInfo .... "<<std::endl;
    BuildTriggerInfo(rawEv);
    if (TriggerToBeSkipped()) return;
  }
    
  // from waveforms to Hits
  BuildHits(rawEv);

  if(fGeometry)           fGeometry->ComputePositions(GetRecoHits());
  //    std::cout<<"about to clusterise pveto"<<std::endl;
  // from Hits to Clusters
  //  ClearClusters();
  BuildClusters();
  //  if(fChannelCalibration) fChannelCalibration->PerformCalibration(GetClusters());

  //Processing is over, let's analyze what's here, if foreseen
  if(fGlobalRecoConfigOptions->IsMonitorMode()) {
    AnalyzeEvent(rawEv);
  }

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


void PVetoReconstruction::BuildHits(TRawEvent* rawEv)//copied from ECal 24/6/19 to have board & channel ID in digitizer
{
  ClearHits();
  vector<TRecoVHit *> &Hits  = GetRecoHits();
  ((DigitizerChannelPVeto*)fChannelReco)->SetTrigMask(GetTriggerProcessor()->GetTrigMask());
  UChar_t nBoards = rawEv->GetNADCBoards();
  ((DigitizerChannelPVeto*)fChannelReco)->SetEventNumber(rawEv->GetEventNumber());
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
 	((DigitizerChannelPVeto*)fChannelReco)->SetChID(ChID);
 	((DigitizerChannelPVeto*)fChannelReco)->SetElChID(ElChID);
 	((DigitizerChannelPVeto*)fChannelReco)->SetBdID(iBdID);
	
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

void PVetoReconstruction::BuildClusters()
{
  PVetoClusterHits PVetoClusterHit;// = new ClusterHits();
  ClusterStructure PVetoClusStruc;//contains a structure for vectors of clusters for each event
  std::vector<PVetoClusterHits> PVetoClusterHitVec;//Contains all the PVetoHits to be clusterised per event
  Int_t nhitpass=0;
  std::vector<Cluster*> vPVetoClusters;

  vector<TRecoVHit *> &Hits  = GetRecoHits();
  std::vector<TRecoVCluster *> &myClusters = GetClusters();
  myClusters.clear();

  TRecoVCluster* myCl;

  for(int iHit=0;iHit<Hits.size();iHit++){
    //    if(Hits[iHit]->GetEnergy()<0) std::cout<<Hits[iHit]->GetEnergy()<<std::endl;
    PVetoClusterHit.Clear();
    //	std::cout<<"ChID "<<PVetoClusterHit.GetChannelId()<<" time "<<PVetoClusterHit.GetTime()<<std::endl;
    PVetoClusterHit.SetEnergy(Hits[iHit]->GetEnergy());
    PVetoClusterHit.SetTime(Hits[iHit]->GetTime());
    PVetoClusterHit.SetChannelId(Hits[iHit]->GetChannelId());
    PVetoClusterHit.SetPosition(Hits[iHit]->GetPosition());
    PVetoClusterHitVec.push_back(PVetoClusterHit);
    //	std::cout<<"Making the Cluster Hits ChID "<<PVetoClusterHit.GetChannelId()<<" time "<<PVetoClusterHit.GetTime()<<std::endl;
  }
  //  std::cout<<"Pveto hit size "<< PVetoClusterHitVec.size()<<std::endl;
  vPVetoClusters.clear();
  PVetoClusStruc.Clear();//contains a structure for vectors of clusters for each event
  // PVetoClusterHitVec.clear();

  for(Int_t iPHit=0;iPHit<PVetoClusterHitVec.size();iPHit++){
    //      std::cout<<"Reading Cluster Hits ChID "<<PVetoClusterHitVec[iPHit].GetChannelId()<<" time "<<PVetoClusterHitVec[iPHit].GetTime()<<std::endl;
    //   if(PVetoClusterHitVec[iPHit].GetEnergy()>0.5){
      nhitpass++;
      PVetoClusStruc.AddHit(PVetoClusterHitVec[iPHit],iPHit);
      //}
  }


  PVetoClusStruc.HitSort();//sort hits in time
  PVetoClusStruc.Clusterise();//clusterise hits
  PVetoClusStruc.MergeClusters();//merge adjacent, in time clusters
  vPVetoClusters = PVetoClusStruc.GetClusters();//vector of clusters

  for(int iPClus=0;iPClus<vPVetoClusters.size();iPClus++){
    myCl = new TRecoVCluster();

    int chID;
    double clE;
    double clT;
    double clX;
    double clY;
    double clZ;
    int clSize;

    chID = vPVetoClusters[iPClus]->GetMostUpstreamChannel();
    clE = vPVetoClusters[iPClus]->GetEnergy();
    clT = vPVetoClusters[iPClus]->GetAverageTime();
    clSize = vPVetoClusters[iPClus]->GetNHits();
    TVector3 clPos = fGeometry->LocalPosition(chID);
    std::vector<Int_t> clHitIndices = vPVetoClusters[iPClus]->GetHitIndex();

    if(clE>100){
      for(int ii=0;ii<clSize;ii++){
	std::cout<<"clE "<<clE<<" ii "<<ii<<" hit "<<clHitIndices[ii]<< " hitE "<<Hits[clHitIndices[ii]]->GetEnergy()<<std::endl;
      }
    }

    myCl->SetChannelId   ( chID );
    myCl->SetEnergy      ( clE );
    myCl->SetTime        ( clT );
    myCl->SetPosition    ( clPos );
    myCl->SetNHitsInClus ( clSize );
    myCl->SetHitVecInClus( clHitIndices );
    //     myCl->SetSeed        ( iSeed );

    myClusters.push_back(myCl);
  }
  //  std::cout<<"size "<<myClusters.size()<<std::endl;
}

bool PVetoReconstruction::TriggerToBeSkipped()
{
  //if ( GetGlobalRecoConfigOptions()->IsRecoMode()    && !(GetTriggerProcessor()->IsBTFTrigger())     ) return true;
  if ( GetGlobalRecoConfigOptions()->IsPedestalMode()&& !(GetTriggerProcessor()->IsAutoTrigger())    ) return true;
  if ( GetGlobalRecoConfigOptions()->IsCosmicsMode() && !(GetTriggerProcessor()->IsCosmicsTrigger()) ) return true;
  return false; 
}
