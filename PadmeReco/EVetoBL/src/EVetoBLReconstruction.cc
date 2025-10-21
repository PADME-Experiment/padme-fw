// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
// Modified by Beth Long 2022-04-07 
// --------------------------------------------------------------
#include "Riostream.h"

#include "EVetoBLReconstruction.hh"

#include "TEVetoMCEvent.hh"
#include "TEVetoMCHit.hh"
#include "TEVetoMCDigi.hh"
#include "DigitizerChannelEVetoBL.hh"
#include "EVetoBLCalibration.hh"
#include "EVetoBLGeometry.hh"
#include "EVetoBLSimpleClusterization.hh"
#include "VetoClusterization.hh"
#include "VetoClusterHits.hh"
#include "TRecoVCluster.hh"

#include "TH1F.h"
#include "TH2F.h"
#include "TDirectory.h"

EVetoBLReconstruction::EVetoBLReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "EVetoBL", ConfigFileName)
{
  // configurable parameters 
  fSigmaNoiseForMC         = (Double_t)fConfig->GetParOrDefault("RECO", "SigmaNoiseForMC", .4);
  fEVetoBLDigiTimeWindow     = (Double_t)fConfig->GetParOrDefault("RECO", "DigitizationTimeWindowForMC", 17.);
  fClusterAlgo     = (Double_t)fConfig->GetParOrDefault("RECOCLUSTER", "ClusterAlgo", 0.);
  fMCSimBrokenChas = (Bool_t)fConfig->GetParOrDefault("RECO", "MCSimBrokenChas", 1);
  fMCEnergyScale           = (Double_t)fConfig->GetParOrDefault("RECO","MCEnergyScale",0.8636);
  fMCEnergyThr             = (Double_t)fConfig->GetParOrDefault("RECO","MCEnergyThr",0.366);
  fClusterHitEnThr         = (Double_t)fConfig->GetParOrDefault("RECO","ClusterHitEnThr",0.366);

//  fChannelReco = new DigitizerChannelReco();
  fChannelReco = new DigitizerChannelEVetoBL();
  fChannelCalibration = new EVetoBLCalibration();
  if(fClusterAlgo==0){//Use old clusterisation
    std::cout<<"EVETOCLUSTERS:clusterising oldly"<<std::endl;
    fClusterization = new EVetoBLSimpleClusterization();
  }
  else if(fClusterAlgo==1){//Use new clusterisation
    std::cout<<"EVETOCLUSTERS:clusterising newly"<<std::endl;
    fClusStruc = VetoClusterStructure();
    fClusterHits = VetoClusterHits();
  }
  //fChannelCalibration  = new PadmeVCalibration();
  fTriggerProcessor = new PadmeVTrigger();
  fGeometry = new EVetoBLGeometry();

  random = new TRandom2();    
  gRandom->SetSeed(time(NULL));

}

EVetoBLReconstruction::~EVetoBLReconstruction()
{;}

void EVetoBLReconstruction::HistoInit(){
  AddHisto("nboards", new TH1F("nboards","Number of boards",100,0.0,100.0));
  AddHisto("ADCs",new TH1F("ADCs","ADC ID",100,0.0,100.));
  AddHisto("nchannels", new TH1F("nchannels","Number of channels",100,0.0,100.0));
  AddHisto("ntriggers", new TH1F("ntriggers","Number of trigger channels",100,0.0,100.0));
  //AddHisto("HitTimeDifference",new TH1F("HitTimeDifference","Difference in time",400,-100.,100.));
  AddHisto("HitTimeDifference",new TH1F("HitTimeDifference","Difference in time",400,-40.,40.));
  AddHisto("EVetoBLOccupancy",new TH1F("EVetoBLOccupancy","EVetoBL Occupancy",100,0.0,100.0));
  AddHisto("EVetoBLOccupancyLast",new TH1F("EVetoBLOccupancyLast","EVetoBL OccupancyLast",100,0.0,100.0));


  AddHisto("EVetoBLEnergy",new TH1F("EVetoBLEnergy","EVetoBL Energy",2000,0.0,40.0));
  AddHisto("EVetoBLEnergyClean",new TH1F("EVetoBLEnergyClean","EVetoBL Energy",2000,0.0,.4));
  //AddHisto("EVetoBLTime",new TH1F("EVetoBLTime","EVetoBL Time",600,-200.0,400.0));
  AddHisto("EVetoBLTime",new TH1F("EVetoBLTime","EVetoBL Time",400,-150.0,250.0));

  AddHisto("EVetoBLTimeVsChannelID",new TH2F("EVetoBLTimeVsChannelID","EVetoBL Time vs Ch. ID",100,0,100,100,-200.0,200.0) );
  AddHisto("EVetoBLTimeVsEVetoBLTime",new TH2F("EVetoBLTimeVsEVetoBLTime","EVetoBL Time vs EVetoBLTime",400,-200.0,200.0, 400,-200.0,200.0));

  char name[256];

  for (int i=0; i<95; i++) { 
    sprintf(name, "EVetoBLDTch%dch%d",i,i+1);
    AddHisto(name, new TH1F(name,"Difference in time",100,-25.,25.));
  }
  
  for (int i=0; i<96; i++) { 
    sprintf(name, "EVetoBLCharge-%d",i);
    AddHisto(name, new TH1F(name,"Charge",2000,00.,.4));
  }


  //AddHisto("EVetoBLDTch1ch2",new TH1F("EVetoBLDTch1ch2","Difference in time",100,-10.,10.));

}

void EVetoBLReconstruction::ConvertMCDigitsToRecoHits(TMCVEvent* tEvent,TMCEvent* tMCEvent) 
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
    // merge digits in the same channel closer in time than a configurable parameter (fEVetoBLDigiTimeWindow){
    if (fEVetoBLDigiTimeWindow > 0) {
      for (unsigned int ih=0; ih<TempHits.size(); ++ih)
	{
	  if (TempHits[ih]->GetChannelId() != digiCh) continue;
	  if (fabs(TempHits[ih]->GetTime()/TempHits[ih]->GetEnergy()-digiT)<fEVetoBLDigiTimeWindow)
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

void EVetoBLReconstruction::ProcessEvent(TRawEvent* rawEv){//Beth 22/2/22: copied from virtual class to override virtual class. I removed the calibration it's done by gain equalisation directly in digitizer. I will want to change as it  will use the new battleships algorithm

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
    EVetoBLReconstruction::BuildClusters(rawEv);
  //  if(fChannelCalibration) fChannelCalibration->PerformCalibration(GetClusters());

  //Processing is over, let's analyze what's here, if foreseen
  if(fGlobalRecoConfigOptions->IsMonitorMode()) {
    AnalyzeEvent(rawEv);
  }

}

void EVetoBLReconstruction::ProcessEvent(TMCVEvent* tEvent,TMCEvent* tMCEvent){//Beth 22/2/22: copied from virtual class to override virtual class. I removed the calibration it's done by gain equalisation directly in digitizer. I will want to change as it  will use the new battleships algorithm

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
    EVetoBLReconstruction::BuildClusters(tMCEvent);
  //  if(fChannelCalibration) fChannelCalibration->PerformCalibration(GetClusters());

}


void EVetoBLReconstruction::AnalyzeEvent(TRawEvent* rawEv){

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

    GetHisto("EVetoBLOccupancy")->Fill(Hits[iHit1]->GetChannelId());
    GetHisto("EVetoBLTime")->Fill(Hits[iHit1]->GetTime());

    (  (TH2F *) GetHisto("EVetoBLTimeVsChannelID"))  ->Fill(Hits[iHit1]->GetChannelId(), Hits[iHit1]->GetTime());



    //for(unsigned int iHit2 = iHit1+1; iHit2 < Hits.size();++iHit2) {
    //  (  (TH2F *) GetHisto("EVetoBLTimeVsEVetoBLTime"))  ->Fill(Hits[iHit1]->GetTime(),Hits[iHit2]->GetTime());
    //  //      if(Hits[iHit1]->GetTime() > 20. && Hits[iHit2]->GetTime() > 20.) {
    //	GetHisto("HitTimeDifference")->Fill(Hits[iHit1]->GetTime() - Hits[iHit2]->GetTime());
    //	//      }
    //}        
    for(unsigned int iHit2 = iHit1+1; iHit2 < Hits.size();++iHit2) {
      ((TH2F *)GetHisto("EVetoBLTimeVsEVetoBLTime"))->Fill(Hits[iHit1]->GetTime(),Hits[iHit2]->GetTime());
      if( Hits[iHit1]->GetChannelId() > 20 && Hits[iHit1]->GetChannelId() < 70
	  && Hits[iHit2]->GetChannelId() > 20 && Hits[iHit2]->GetChannelId() < 70 ) {
	GetHisto("HitTimeDifference")->Fill(Hits[iHit1]->GetTime() - Hits[iHit2]->GetTime());
      }
    }

    GetHisto("EVetoBLEnergy") -> Fill(Hits[iHit1]->GetEnergy() );
    int chid = Hits[iHit1]->GetChannelId();
    
    charges[Hits[iHit1]->GetChannelId()] = Hits[iHit1]->GetEnergy();
    
  }
  
  char name[256];
  
  int ih1,ih2;

  for(int i = 1; i < 95; i++) {
    if(charges[i] > 0. && charges[i-1] < 0. && charges[i+1] < 0.) {      
      sprintf(name, "EVetoBLCharge-%d", i);
      GetHisto(name)->Fill(charges[i]);
      GetHisto("EVetoBLEnergyClean") -> Fill(charges[i] );

    }
  }
  

  for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
    for(unsigned int iHit2 = 0; iHit2 < Hits.size();++iHit2) {
      if(Hits[iHit1]->GetChannelId() + 1 ==  Hits[iHit2]->GetChannelId()   ) {

	sprintf(name, "EVetoBLDTch%dch%d", Hits[iHit1]->GetChannelId() ,Hits[iHit1]->GetChannelId()+1);
	//	if(Hits[iHit1]->GetTime() > 20. && Hits[iHit2]->GetTime() > 20.) {	  
	  GetHisto(name)->Fill(Hits[iHit1]->GetTime() - Hits[iHit2]->GetTime());
	  //	}
      }
    }
  }

  if(GetHisto("EVetoBLOccupancyLast")->GetEntries()){    
    for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
      GetHisto("EVetoBLOccupancyLast")->Fill(Hits[iHit1]->GetChannelId());
    }
  }  
 
}


void EVetoBLReconstruction::BuildHits(TRawEvent* rawEv)//copied from ECal 24/6/19 to have board & channel ID in digitizer
{
  ClearHits();
  vector<TRecoVHit *> &Hits  = GetRecoHits();
  ((DigitizerChannelEVetoBL*)fChannelReco)->SetTrigMask(GetTriggerProcessor()->GetTrigMask());
  UChar_t nBoards = rawEv->GetNADCBoards();
  ((DigitizerChannelEVetoBL*)fChannelReco)->SetEventNumber(rawEv->GetEventNumber());
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
 	((DigitizerChannelEVetoBL*)fChannelReco)->SetChID(ChID);
 	((DigitizerChannelEVetoBL*)fChannelReco)->SetElChID(ElChID);
 	((DigitizerChannelEVetoBL*)fChannelReco)->SetBdID(iBdID);
	unsigned int nHitsBefore = Hits.size();
	fChannelReco->Reconstruct(Hits);
	unsigned int nHitsAfter = Hits.size();
	for(unsigned int iHit = nHitsBefore; iHit < nHitsAfter;++iHit) {
	  Hits[iHit]->SetChannelId(GetChannelID(ADC->GetBoardId(),chn->GetChannelNumber()));
	  Hits[iHit]->setBDCHid( ADC->GetBoardId(), chn->GetChannelNumber() );
	  if(fTriggerProcessor)
	    Hits[iHit]->SetTime(
				Hits[iHit]->GetTime() - 
				fTriggerProcessor->GetChannelTriggerTime( ADC->GetBoardId(), chn->GetChannelNumber() ));
	}
      }
    } else {
      //std::cout<<GetName()<<"::Process(TRawEvent*) - unknown board .... "<<std::endl;
    }
  }    
}
  
void EVetoBLReconstruction::BuildClusters(TRawEvent* rawEv)
{
  //  VetoClusterHits VetoClusterHit;// = new ClusterHits();
  //  ClusterStructure VetoClusStruc;//contains a structure for vectors of clusters for each event
  std::vector<VetoClusterHits> VetoClusterHitVec;//Contains all the EVetoBLHits to be clusterised per event
  Int_t nhitpass=0;
  std::vector<VetoCluster*> vVetoClusters;

  vector<TRecoVHit *> &Hits  = GetRecoHits();
  std::vector<TRecoVCluster *> &myClusters = GetClusters();
  myClusters.clear();

  TRecoVCluster* myCl;
  VetoClusterHitVec.clear();

  for(int iHit=0;iHit<Hits.size();iHit++){
    fClusterHits.Clear();
    fClusterHits.SetEnergy(Hits[iHit]->GetEnergy());
    fClusterHits.SetTime(Hits[iHit]->GetTime());
    fClusterHits.SetChannelId(Hits[iHit]->GetChannelId());
    fClusterHits.SetPosition(Hits[iHit]->GetPosition());
    fClusterHits.SetIndex(iHit);
    VetoClusterHitVec.push_back(fClusterHits);
  }

  vVetoClusters.clear();
  fClusStruc.Clear();//contains a structure for vectors of clusters for each event

  for(Int_t iPHit=0;iPHit<VetoClusterHitVec.size();iPHit++){
    if(VetoClusterHitVec[iPHit].GetEnergy()>fClusterHitEnThr){//100 keV is the threshold for hits in the virtual class
      nhitpass++; 
      fClusStruc.AddHit(VetoClusterHitVec[iPHit],iPHit);
    }  
  }

  fClusStruc.HitSort();//sort hits in energy
  fClusStruc.Clusterise();//clusterise hits
  fClusStruc.MergeClusters();//merge adjacent, in time clusters (data)
  vVetoClusters = fClusStruc.GetClusters();//vector of clusters
  std::vector<Int_t> clHitIndices;

  for(int iPClus=0;iPClus<vVetoClusters.size();iPClus++){
    myCl = new TRecoVCluster();
    clHitIndices.clear();

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

    for(int ii=0;ii<clSize;ii++){
      if(clHitIndices[ii]>250){
	std::cout<<"rawEvNo "<<rawEv->GetEventNumber()<<" EVetoBL clE "<<clE<<std::endl;
	std::cout<<" ii "<<ii<<std::endl;
	std::cout<<" no. hits "<<clHitIndices.size()<<std::endl;
	std::cout<<" hit "<<clHitIndices[ii]<<std::endl;
	std::cout<< " hitE "<<Hits[clHitIndices[ii]]->GetEnergy()<<std::endl;
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
    //    std::cout<<"my clusters size "<<myClusters.size()<<std::endl;
  }
  //  std::cout<<"size "<<myClusters.size()<<std::endl;
}

void EVetoBLReconstruction::BuildClusters(TMCEvent* MCEv)
{

  //  VetoClusterHits VetoClusterHit;// = new ClusterHits();
  //  ClusterStructure VetoClusStruc;//contains a structure for vectors of clusters for each event
  std::vector<VetoClusterHits> VetoClusterHitVec;//Contains all the EVetoBLHits to be clusterised per event 
  Int_t nhitpass=0;
  std::vector<VetoCluster*> vVetoClusters;

  vector<TRecoVHit *> &Hits  = GetRecoHits();
  std::vector<TRecoVCluster *> &myClusters = GetClusters();
  myClusters.clear();

  TRecoVCluster* myCl;
  VetoClusterHitVec.clear();

  for(int iHit=0;iHit<Hits.size();iHit++){
    fClusterHits.Clear();
    fClusterHits.SetEnergy(Hits[iHit]->GetEnergy());
    fClusterHits.SetTime(Hits[iHit]->GetTime());
    fClusterHits.SetChannelId(Hits[iHit]->GetChannelId());
    fClusterHits.SetPosition(Hits[iHit]->GetPosition());
    fClusterHits.SetIndex(iHit);
    VetoClusterHitVec.push_back(fClusterHits);
  }

  vVetoClusters.clear();
  fClusStruc.Clear();//contains a structure for vectors of clusters for each event

  for(Int_t iPHit=0;iPHit<VetoClusterHitVec.size();iPHit++){
    if(VetoClusterHitVec[iPHit].GetEnergy()>fClusterHitEnThr){
      nhitpass++;
      fClusStruc.AddHit(VetoClusterHitVec[iPHit],iPHit);    
    }
  }

  fClusStruc.HitSort();//sort hits in energy
  fClusStruc.Clusterise();//clusterise hits
  fClusStruc.MergeClusters();//merge adjacent, in time clusters (MC)
  vVetoClusters = fClusStruc.GetClusters();//vector of clusters
  std::vector<Int_t> clHitIndices;


  for(int iPClus=0;iPClus<vVetoClusters.size();iPClus++){
    myCl = new TRecoVCluster();
    clHitIndices.clear();

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

    myCl->SetChannelId   ( chID );
    myCl->SetEnergy      ( clE );
    myCl->SetTime        ( clT );
    myCl->SetPosition    ( clPos );
    myCl->SetNHitsInClus ( clSize );
    myCl->SetHitVecInClus( clHitIndices );
    //     myCl->SetSeed        ( iSeed );

    myClusters.push_back(myCl);
  }
}

bool EVetoBLReconstruction::TriggerToBeSkipped()
{
  //if ( GetGlobalRecoConfigOptions()->IsRecoMode()    && !(GetTriggerProcessor()->IsBTFTrigger())     ) return true;
  if ( GetGlobalRecoConfigOptions()->IsPedestalMode()&& !(GetTriggerProcessor()->IsAutoTrigger())    ) return true;
  if ( GetGlobalRecoConfigOptions()->IsCosmicsMode() && !(GetTriggerProcessor()->IsCosmicsTrigger()) ) return true;
  return false; 
}
