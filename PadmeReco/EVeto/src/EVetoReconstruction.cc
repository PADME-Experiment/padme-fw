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
#include "EVetoGeometry.hh"
#include "EVetoSimpleClusterization.hh"
#include "VetoClusterization.hh"
#include "VetoClusterHits.hh"
#include "TRecoVCluster.hh"

#include "TH1F.h"
#include "TH2F.h"
#include "TDirectory.h"

EVetoReconstruction::EVetoReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "EVeto", ConfigFileName)
{
  // configurable parameters 
  fSigmaNoiseForMC         = (Double_t)fConfig->GetParOrDefault("RECO", "SigmaNoiseForMC", .4);
  fEVetoDigiTimeWindow     = (Double_t)fConfig->GetParOrDefault("RECO", "DigitizationTimeWindowForMC", 17.);
  fClusterAlgo     = (Double_t)fConfig->GetParOrDefault("RECOCLUSTER", "ClusterAlgo", 0.);
  fMCSimBrokenChas = (Bool_t)fConfig->GetParOrDefault("RECO", "MCSimBrokenChas", 1);
  fMCEnergyScale           = (Double_t)fConfig->GetParOrDefault("RECO","MCEnergyScale",0.8636);
  fMCEnergyThr             = (Double_t)fConfig->GetParOrDefault("RECO","MCEnergyThr",0.366);
  fClusterHitEnThr         = (Double_t)fConfig->GetParOrDefault("RECO","ClusterHitEnThr",0.366);

//  fChannelReco = new DigitizerChannelReco();
  fChannelReco = new DigitizerChannelEVeto();
  fChannelCalibration = new EVetoCalibration();
  if(fClusterAlgo==0){//Use old clusterisation
    std::cout<<"EVETOCLUSTERS:clusterising oldly"<<std::endl;
    fClusterization = new EVetoSimpleClusterization();
  }
  else if(fClusterAlgo==1){//Use new clusterisation
    std::cout<<"EVETOCLUSTERS:clusterising newly"<<std::endl;
    fClusStruc = VetoClusterStructure();
    fClusterHits = VetoClusterHits();
  }
  //fChannelCalibration  = new PadmeVCalibration();
  fTriggerProcessor = new PadmeVTrigger();
  fGeometry = new EVetoGeometry();

  random = new TRandom2();    
  gRandom->SetSeed(time(NULL));

}

EVetoReconstruction::~EVetoReconstruction()
{;}

void EVetoReconstruction::HistoInit(){
  AddHisto("nboards", new TH1F("nboards","Number of boards",100,0.0,100.0));
  AddHisto("ADCs",new TH1F("ADCs","ADC ID",100,0.0,100.));
  AddHisto("nchannels", new TH1F("nchannels","Number of channels",100,0.0,100.0));
  AddHisto("ntriggers", new TH1F("ntriggers","Number of trigger channels",100,0.0,100.0));
  //AddHisto("HitTimeDifference",new TH1F("HitTimeDifference","Difference in time",400,-100.,100.));
  AddHisto("HitTimeDifference",new TH1F("HitTimeDifference","Difference in time",400,-40.,40.));
  AddHisto("EVetoOccupancy",new TH1F("EVetoOccupancy","EVeto Occupancy",100,0.0,100.0));
  AddHisto("EVetoOccupancyLast",new TH1F("EVetoOccupancyLast","EVeto OccupancyLast",100,0.0,100.0));


  AddHisto("EVetoEnergy",new TH1F("EVetoEnergy","EVeto Energy",2000,0.0,40.0));
  AddHisto("EVetoEnergyClean",new TH1F("EVetoEnergyClean","EVeto Energy",2000,0.0,.4));
  //AddHisto("EVetoTime",new TH1F("EVetoTime","EVeto Time",600,-200.0,400.0));
  AddHisto("EVetoTime",new TH1F("EVetoTime","EVeto Time",400,-150.0,250.0));

  AddHisto("EVetoTimeVsChannelID",new TH2F("EVetoTimeVsChannelID","EVeto Time vs Ch. ID",100,0,100,100,-200.0,200.0) );
  AddHisto("EVetoTimeVsEVetoTime",new TH2F("EVetoTimeVsEVetoTime","EVeto Time vs EVetoTime",400,-200.0,200.0, 400,-200.0,200.0));

  char name[256];

  for (int i=0; i<95; i++) { 
    sprintf(name, "EVetoDTch%dch%d",i,i+1);
    AddHisto(name, new TH1F(name,"Difference in time",100,-25.,25.));
  }
  
  for (int i=0; i<96; i++) { 
    sprintf(name, "EVetoCharge-%d",i);
    AddHisto(name, new TH1F(name,"Charge",2000,00.,.4));
  }


  //AddHisto("EVetoDTch1ch2",new TH1F("EVetoDTch1ch2","Difference in time",100,-10.,10.));

}

void EVetoReconstruction::ConvertMCDigitsToRecoHits(TMCVEvent* tEvent,TMCEvent* tMCEvent) 
{
  if (tEvent==NULL) return;
  for(Int_t i=0; i < fHits.size(); i++) delete fHits[i];
  fHits.clear();

  vector<TRecoVHit *> TempHits;
  
  // MC to reco hits
  //std::cout<<"New Event ----------- nDigi = "<< tEvent->GetNDigi()<<std::endl;
  for (Int_t i=0; i<tEvent->GetNDigi(); ++i) {
    TMCVDigi* digi = tEvent->Digi(i);
    //TRecoVHit *Hit = new TRecoVHit(digi);

    Int_t    digiCh = digi->GetChannelId();
    if(fMCSimBrokenChas&&(digiCh==52||digiCh==53||digiCh==54||digiCh==55)){
      std::cout<<"simulating broken channel "<<digiCh<<std::endl; 
      return;
    }
    Double_t digiT  = digi->GetTime();
    Double_t digiE  = digi->GetEnergy();
    //std::cout<<"Digit n. "<<i<<" Ch="<<digiCh<<" time "<<digiT<<" nhits so far = "<<TempHits.size()<<std::endl;

    Bool_t toBeMerged = false;
    // merge digits in the same channel closer in time than a configurable parameter (fEVetoDigiTimeWindow){
    if (fEVetoDigiTimeWindow > 0) {
      for (unsigned int ih=0; ih<TempHits.size(); ++ih)
	{
	  if (TempHits[ih]->GetChannelId() != digiCh) continue;
	  if (fabs(TempHits[ih]->GetTime()/TempHits[ih]->GetEnergy()-digiT)<fEVetoDigiTimeWindow)
	    {
	      toBeMerged = true;
	      // this digit must be merged with a previously defined recoHit
	      //std::cout<<" -- merging with hit in ch "<<TempHits[ih]->GetChannelId()<<" at time "<<TempHits[ih]->GetTime()/TempHits[ih]->GetEnergy()<<" diffT = "<<fabs(TempHits[ih]->GetTime()/TempHits[ih]->GetEnergy()-digiT)<<std::endl;
	      TempHits[ih]->SetEnergy(TempHits[ih]->GetEnergy() + digiE);
	      TempHits[ih]->SetTime(TempHits[ih]->GetTime() + digiE*digiT);
	      //std::cout<<" -- updated  Ch "<<TempHits[ih]->GetChannelId()<<" time "<<TempHits[ih]->GetTime()/TempHits[ih]->GetEnergy()<<" so far "<<std::endl;
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
	TempHits.push_back(Hit);
	//std::cout<<"   New hit Ch "<<Hit->GetChannelId()<<" time "<<Hit->GetTime()/Hit->GetEnergy()<<" so far "<<TempHits.size()<<" hits"<<std::endl;
      }
  }//end hit to digi merge
  
  // correct the time & add noise
  TRecoVHit *Hit;
  Double_t Noise=0.;
  for (unsigned int ih=0; ih<TempHits.size(); ++ih)
    {
      Hit = TempHits[ih];
      //pre-smear time = GetTime()/Hit->GetEnergy());
      //smearing in time
      //      Hit->SetTime(smearedtime);
      Hit->SetTime(Hit->GetTime()/Hit->GetEnergy());
      Noise=random->Gaus(0.,fSigmaNoiseForMC);   
      Hit->SetEnergy(fMCEnergyScale*(Hit->GetEnergy()+Noise));
      if(Hit->GetEnergy()>fMCEnergyThr) fHits.push_back(Hit);
      //      else std::cout<<"Dumping "<<Hit->GetEnergy()<<" MeV hit"<<std::endl;
    }
    //
  return;
}

void EVetoReconstruction::ProcessEvent(TRawEvent* rawEv){//Beth 22/2/22: copied from virtual class to override virtual class. I removed the calibration it's done by gain equalisation directly in digitizer. I will want to change as it  will use the new battleships algorithm

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
  if(fClusterAlgo==0){
    ClearClusters();
    PadmeVReconstruction::BuildClusters();
  }
  if(fClusterAlgo==1)
    EVetoReconstruction::BuildClusters(rawEv);
  //  if(fChannelCalibration) fChannelCalibration->PerformCalibration(GetClusters());

  //Processing is over, let's analyze what's here, if foreseen
  if(fGlobalRecoConfigOptions->IsMonitorMode()) {
    AnalyzeEvent(rawEv);
  }

}

void EVetoReconstruction::ProcessEvent(TMCVEvent* tEvent,TMCEvent* tMCEvent){//Beth 22/2/22: copied from virtual class to override virtual class. I removed the calibration it's done by gain equalisation directly in digitizer. I will want to change as it  will use the new battleships algorithm

  //  std::cout<<"!?><using pveto process event"<<std::endl;

  // MC to reco hits
  ConvertMCDigitsToRecoHits(tEvent, tMCEvent);
  if(fChannelCalibration) fChannelCalibration->PerformMCCalibration(GetRecoHits());
  if(fGeometry)           fGeometry->ComputePositions(GetRecoHits());


  if(fClusterAlgo==0){
    ClearClusters();
    PadmeVReconstruction::BuildClusters();
  }
  if(fClusterAlgo==1)
    EVetoReconstruction::BuildClusters(tMCEvent);
  //  if(fChannelCalibration) fChannelCalibration->PerformCalibration(GetClusters());

}


void EVetoReconstruction::AnalyzeEvent(TRawEvent* rawEv){

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

    GetHisto("EVetoOccupancy")->Fill(Hits[iHit1]->GetChannelId());
    GetHisto("EVetoTime")->Fill(Hits[iHit1]->GetTime());

    (  (TH2F *) GetHisto("EVetoTimeVsChannelID"))  ->Fill(Hits[iHit1]->GetChannelId(), Hits[iHit1]->GetTime());



    //for(unsigned int iHit2 = iHit1+1; iHit2 < Hits.size();++iHit2) {
    //  (  (TH2F *) GetHisto("EVetoTimeVsEVetoTime"))  ->Fill(Hits[iHit1]->GetTime(),Hits[iHit2]->GetTime());
    //  //      if(Hits[iHit1]->GetTime() > 20. && Hits[iHit2]->GetTime() > 20.) {
    //	GetHisto("HitTimeDifference")->Fill(Hits[iHit1]->GetTime() - Hits[iHit2]->GetTime());
    //	//      }
    //}        
    for(unsigned int iHit2 = iHit1+1; iHit2 < Hits.size();++iHit2) {
      ((TH2F *)GetHisto("EVetoTimeVsEVetoTime"))->Fill(Hits[iHit1]->GetTime(),Hits[iHit2]->GetTime());
      if( Hits[iHit1]->GetChannelId() > 20 && Hits[iHit1]->GetChannelId() < 70
	  && Hits[iHit2]->GetChannelId() > 20 && Hits[iHit2]->GetChannelId() < 70 ) {
	GetHisto("HitTimeDifference")->Fill(Hits[iHit1]->GetTime() - Hits[iHit2]->GetTime());
      }
    }

    GetHisto("EVetoEnergy") -> Fill(Hits[iHit1]->GetEnergy() );
    int chid = Hits[iHit1]->GetChannelId();
    
    charges[Hits[iHit1]->GetChannelId()] = Hits[iHit1]->GetEnergy();
    
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
      if(Hits[iHit1]->GetChannelId() + 1 ==  Hits[iHit2]->GetChannelId()   ) {

	sprintf(name, "EVetoDTch%dch%d", Hits[iHit1]->GetChannelId() ,Hits[iHit1]->GetChannelId()+1);
	//	if(Hits[iHit1]->GetTime() > 20. && Hits[iHit2]->GetTime() > 20.) {	  
	  GetHisto(name)->Fill(Hits[iHit1]->GetTime() - Hits[iHit2]->GetTime());
	  //	}
      }
    }
  }

  if(GetHisto("EVetoOccupancyLast")->GetEntries()){    
    for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
      GetHisto("EVetoOccupancyLast")->Fill(Hits[iHit1]->GetChannelId());
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
      //Loop over the channels and perform recoH
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
  //  VetoClusterHits VetoClusterHit;// = new ClusterHits();
  //  ClusterStructure VetoClusStruc;//contains a structure for vectors of clusters for each event
  std::vector<VetoClusterHits> VetoClusterHitVec;//Contains all the EVetoHits to be clusterised per event
  Int_t nhitpass=0;
  std::vector<VetoCluster*> vVetoClusters;

  vector<TRecoVHit *> &Hits  = GetRecoHits();
  std::vector<TRecoVCluster *> &myClusters = GetClusters();
  myClusters.clear();

  TRecoVCluster* myCl;
  VetoClusterHitVec.clear();

  for(int iHit=0;iHit<Hits.size();iHit++){
    if(Hits[iHit]->GetEnergy()<0) std::cout<<"EVeto negative hit energy "<<Hits[iHit]->GetEnergy()<<std::endl;
    fClusterHits.Clear();
    //	std::cout<<"ChID "<<fClusterHits.GetChannelId()<<" time "<<fClusterHits.GetTime()<<std::endl;
    fClusterHits.SetEnergy(Hits[iHit]->GetEnergy());
    fClusterHits.SetTime(Hits[iHit]->GetTime());
    fClusterHits.SetChannelId(Hits[iHit]->GetChannelId());
    fClusterHits.SetPosition(Hits[iHit]->GetPosition());
    VetoClusterHitVec.push_back(fClusterHits);
    //std::cout<<"Making the Cluster Hits ChID "<<fClusterHits.GetChannelId()<<" time "<<fClusterHits.GetTime()<<std::endl;
  }
  //  std::cout<<"Pveto hit size "<< fClusterHitsVec.size()<<std::endl;
  vVetoClusters.clear();
  fClusStruc.Clear();//contains a structure for vectors of clusters for each event

  for(Int_t iPHit=0;iPHit<VetoClusterHitVec.size();iPHit++){
    //std::cout<<"Reading Cluster Hits ChID "<<VetoClusterHitVec[iPHit].GetChannelId()<<" time "<<VetoClusterHitVec[iPHit].GetTime()<<std::endl;
    //   std::cout<<"EVETO!!!! before "<<VetoClusterHitVec[iPHit].GetEnergy()<<std::endl;
    if(VetoClusterHitVec[iPHit].GetEnergy()>fClusterHitEnThr){//100 keV is the threshold for hits in the virtual class
      nhitpass++; 
      fClusStruc.AddHit(VetoClusterHitVec[iPHit],iPHit);
      //     std::cout<<"after "<<VetoClusterHitVec[iPHit].GetEnergy()<<std::endl;
    }  
  }

  fClusStruc.HitSort();//sort hits in time
  fClusStruc.Clusterise();//clusterise hits
  fClusStruc.MergeClusters();//merge adjacent, in time clusters (data)
  vVetoClusters = fClusStruc.GetClusters();//vector of clusters
  std::vector<Int_t> clHitIndices;

  //  std::cout<<vVetoClusters.size()<<std::endl;

  for(int iPClus=0;iPClus<vVetoClusters.size();iPClus++){
    myCl = new TRecoVCluster();
    clHitIndices.clear();
    //    std::cout<<"----"<<std::endl;
    for(int iPClusHit=0;iPClusHit<(fClusStruc.GetHitVec()).size();iPClusHit++){
      //fClusStruc.GetHitVec()[iPClusHit].Print();

    }

    int chID;
    double clE;
    double clT;
    double clX;
    double clY;
    double clZ;
    int clSize;

    chID = vVetoClusters[iPClus]->GetMostUpstreamChannel();
    clE = vVetoClusters[iPClus]->GetEnergy();
    clT = vVetoClusters[iPClus]->GetAverageTime();
    clSize = vVetoClusters[iPClus]->GetNHits();
    TVector3 clPos = fGeometry->LocalPosition(chID);

    clHitIndices = vVetoClusters[iPClus]->GetHitIndex();

    //    if(clE>100){
      for(int ii=0;ii<clSize;ii++){
	if(clHitIndices[ii]>250){
	  std::cout<<"rawEvNo "<<rawEv->GetEventNumber()<<" EVeto clE "<<clE<<std::endl;
	  std::cout<<" ii "<<ii<<std::endl;
	  std::cout<<" no. hits "<<clHitIndices.size()<<std::endl;
	  std::cout<<" hit "<<clHitIndices[ii]<<std::endl;
	  std::cout<< " hitE "<<Hits[clHitIndices[ii]]->GetEnergy()<<std::endl;
	}
      }
      //}

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

void EVetoReconstruction::BuildClusters(TMCEvent* MCEv)
{

  //  VetoClusterHits VetoClusterHit;// = new ClusterHits();
  //  ClusterStructure VetoClusStruc;//contains a structure for vectors of clusters for each event
  std::vector<VetoClusterHits> VetoClusterHitVec;//Contains all the EVetoHits to be clusterised per event 
  Int_t nhitpass=0;
  std::vector<VetoCluster*> vVetoClusters;

  vector<TRecoVHit *> &Hits  = GetRecoHits();
  std::vector<TRecoVCluster *> &myClusters = GetClusters();
  myClusters.clear();

  TRecoVCluster* myCl;
  VetoClusterHitVec.clear();

  for(int iHit=0;iHit<Hits.size();iHit++){
    //    if(Hits[iHit]->GetEnergy()<0) std::cout<<Hits[iHit]->GetEnergy()<<std::endl;
    fClusterHits.Clear();
    //	std::cout<<"ChID "<<fClusterHits.GetChannelId()<<" time "<<fClusterHits.GetTime()<<std::endl;
    fClusterHits.SetEnergy(Hits[iHit]->GetEnergy());
    fClusterHits.SetTime(Hits[iHit]->GetTime());
    fClusterHits.SetChannelId(Hits[iHit]->GetChannelId());
    fClusterHits.SetPosition(Hits[iHit]->GetPosition());
    VetoClusterHitVec.push_back(fClusterHits);
    //std::cout<<"Making the Cluster Hits ChID "<<fClusterHits.GetChannelId()<<" time "<<fClusterHits.GetTime()<<std::endl;
  }
  //  std::cout<<"Pveto hit size "<< fClusterHitsVec.size()<<std::endl;
  vVetoClusters.clear();
  fClusStruc.Clear();//contains a structure for vectors of clusters for each event

  for(Int_t iPHit=0;iPHit<VetoClusterHitVec.size();iPHit++){
    //std::cout<<"Reading Cluster Hits ChID "<<VetoClusterHitVec[iPHit].GetChannelId()<<" time "<<VetoClusterHitVec[iPHit].GetTime()<<std::endl;
    if(VetoClusterHitVec[iPHit].GetEnergy()>fClusterHitEnThr){
      nhitpass++;
      fClusStruc.AddHit(VetoClusterHitVec[iPHit],iPHit);    
    }
  }

  fClusStruc.HitSort();//sort hits in time
  fClusStruc.Clusterise();//clusterise hits
  fClusStruc.MergeClusters();//merge adjacent, in time clusters (MC)
  vVetoClusters = fClusStruc.GetClusters();//vector of clusters
  std::vector<Int_t> clHitIndices;

  //  std::cout<<vVetoClusters.size()<<std::endl;

  for(int iPClus=0;iPClus<vVetoClusters.size();iPClus++){
    myCl = new TRecoVCluster();
    clHitIndices.clear();
    //    std::cout<<"----"<<std::endl;
    for(int iPClusHit=0;iPClusHit<(fClusStruc.GetHitVec()).size();iPClusHit++){
      //fClusStruc.GetHitVec()[iPClusHit].Print();

    }

    int chID;
    double clE;
    double clT;
    double clX;
    double clY;
    double clZ;
    int clSize;

    chID = vVetoClusters[iPClus]->GetMostUpstreamChannel();
    clE = vVetoClusters[iPClus]->GetEnergy();
    clT = vVetoClusters[iPClus]->GetAverageTime();
    clSize = vVetoClusters[iPClus]->GetNHits();
    TVector3 clPos = fGeometry->LocalPosition(chID);

    clHitIndices = vVetoClusters[iPClus]->GetHitIndex();

    //    if(clE>100){
      for(int ii=0;ii<clSize;ii++){
	if(clHitIndices[ii]>250){
	  std::cout<<"rawEvNo "<<MCEv->GetEventNumber()<<" EVeto clE "<<clE<<std::endl;
	  std::cout<<" ii "<<ii<<std::endl;
	  std::cout<<" no. hits "<<clHitIndices.size()<<std::endl;
	  std::cout<<" hit "<<clHitIndices[ii]<<std::endl;
	  std::cout<< " hitE "<<Hits[clHitIndices[ii]]->GetEnergy()<<std::endl;
	}
      }
      //}

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
