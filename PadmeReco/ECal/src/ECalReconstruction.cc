// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "ECalReconstruction.hh"
#include "ECalCalibration.hh"
#include "ECalGeometry.hh"
#include "DigitizerChannelECal.hh"

#include "ECalParameters.hh"
#include "ECalCrystalHandler.hh"
#include "ECalCrystal.hh"
#include "ECalClusterFinderIsland.hh"
#include "ECalClusterFinderRadius.hh"
#include "ECalSimpleClusterization.hh"
#include "ECalCluster.hh"

#include "TECalMCEvent.hh"
#include "TECalMCHit.hh"
#include "TECalMCDigi.hh"
#include "TH2F.h"
#include "TH1F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TRecoVCluster.hh"
#include "TRandom2.h"
#include <time.h>
#include <stdlib.h>

struct TimeEnergy 
{
    double digiTime;
    double digiEnergy;
};
struct by_time 
{
    bool operator()(TimeEnergy const &a,TimeEnergy const &b) const noexcept    
	{
	    return a.digiTime < b.digiTime;
	}
};



ECalReconstruction::ECalReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "ECal", ConfigFileName)
{
  NNoHits=0;
  //fRecoEvent = new TRecoECalEvent();
  //ParseConfFile(ConfigFileName);
  fChannelReco = new DigitizerChannelECal();
  fClusterization = new ECalSimpleClusterization();
  fGeometry = new ECalGeometry(); 
  //fTriggerProcessor = new PadmeVTrigger(); // this is done for all detectors in the constructor of PadmeVReconstruction
  fChannelCalibration = new ECalCalibration();
  r = new TRandom2();    
  gRandom->SetSeed(time(NULL));
  
  fEnergyCompensation = NULL;
  
  fMultihitForMC          = (Int_t)fConfig->GetParOrDefault("RECO","Multihit",0);
  fClusterizationAlgo     = (Int_t)fConfig->GetParOrDefault("RECOCLUSTER", "ClusterizationAlgo", 1);
  fClDeltaTime            = (Double_t)fConfig->GetParOrDefault("RECOCLUSTER", "ClusterDeltaTimeMax", 1.);
  fClDeltaCellMax         = (Int_t)fConfig->GetParOrDefault("RECOCLUSTER", "ClusterDeltaCellMax", 3);
  fClEnThrForHit          = (Double_t)fConfig->GetParOrDefault("RECOCLUSTER", "ClusterEnergyThresholdForHit", 1.);
  fClEnThrForSeed         = (Double_t)fConfig->GetParOrDefault("RECOCLUSTER", "ClusterEnergyThresholdForSeed", 50.);
  fCompensateMissingE     = (Int_t)fConfig->GetParOrDefault("RECOCLUSTER", "CompensateMissingE", 1);
  std::cout<<"ECAL Clusterization ALGO = "<<fClusterizationAlgo<<std::endl;
  fClusterTimeAlgo = (Int_t)fConfig->GetParOrDefault("RECOCLUSTER", "ClusterTimeAlgo", 1);
  fDeteriorateEnergyResolution = (Int_t)fConfig->GetParOrDefault("RECOCLUSTER", "ClusterDeteriorateEnergyResolution", 0);
  fReproductSACbunchStructure = (Int_t)fConfig->GetParOrDefault("RECO", "BunchStructureSAC_runJuly", 0);
  if(fReproductSACbunchStructure){
    TFile *file1 =new TFile("TimeDistribution.root");
    hSAC_HitTime = (TH1F*)file1->Get("SAC_HitTime");
    // hSAC_HitTime->GetXaxis()->SetRange(-100,50);
    //hCumulative = new TH1F(); 
    hCumulative= (TH1F*)hSAC_HitTime->GetCumulative();
    fComulativeMax = hCumulative->GetMaximum();
  }

  //  fClusters.clear();
}
void ECalReconstruction::HistoInit(){
  AddHisto("ECalOccupancy",new TH2F("ECalOccupancy","ECalOccupancy",31,0,31,31,0,31));
  AddHisto("ECalOccupancyOffTime",new TH2F("ECalOccupancyOffTime","ECalOccupancyOffTime",31,0,31,31,0,31));
  AddHisto("ECalEvent",new TH2F("ECalEvent","ECalEvent",31,0,31,31,0,31));
  AddHisto("ECalChEvent",new TH2F("ECalChEvent","ECalChEvent",31,0,31,31,0,31));
  AddHisto("ECalCharge",new TH2F("ECalCharge","ECalCharge",31,0,31,31,0,31));
  //  AddHisto("ECalTotCharge",new TH1F("ECalTotCharge","ECalTotCharge",1001,0,100));
  AddHisto("ECalChCharge",new TH1F("ECalChCharge","ECalChCharge",1001,0,100));
  //  AddHisto("ECalTime",new TH1F("ECalTime","ECalTime",1000,-200,800));

  AddHisto("ECALClPos",new TH2F("ECALClPos","ECALClPos",30,-15,15,30,-15,15));
  AddHisto("ECALCellPos",new TH2F("ECALCellPos","ECALCellPos",30,0,30,30,0,30));
  //  AddHisto("ECALVoters",new TH2F("ECALVoters","ECALVoters",1000,0.,1000.,26,-0.5,25.5));

  AddHisto("ECALTime",new TH1F("ECALTime","ECALTime",2000,-200,800));
  AddHisto("ECALTimeCut",new TH1F("ECALTimeCut","ECALTimeCut",500,0,800));
  AddHisto("ECALClTime",new TH1F("ECALClTime","ECALClTime",500,-200,800));
  AddHisto("ECALClTimeCut",new TH1F("ECALClTimeCut","ECALClTimeCut",500,-200,800));
  AddHisto("ECALETot",new TH1F("ECALETot","ECALETot",500,0,2500));
  AddHisto("ECALQTot",new TH1F("ECALQTot","ECALQTot",500,0,2500));
  AddHisto("ECALNPart",new TH1F("ECALNPart","ECALNPart",200,0,200));
  AddHisto("ECALNClus",new TH1F("ECALNClus","ECALNClus",200,0,200));

  AddHisto("ECALDropHitE",new TH1F("ECALDropHitE","ECALDropHitE",500,0,500));
  AddHisto("ECALHitE",new TH1F("ECALHitE","ECALHitE",550,0,550));

  //  AddHisto("ECALRawClus",new TH1F("ECALRawClus","ECALRawClus",200,0,1000));
  AddHisto("ECALClE",new TH1F("ECALClE","ECALClE",500,0,1000));

  AddHisto("ECALClNCry",new TH1F("ECALClNCry","ECALClNCry",30,0,30));
  AddHisto("ECALClTDiff",new TH1F("ECALClTDiff","ECALClTDiff",100,-10,10));

  AddHisto("ECALClSeed",new TH1F("ECALClSeed","ECALClSeed",3000,0,3000));
  AddHisto("ECALClSeedEn",new TH1F("ECALClSeedEn","ECALClSeedEn",550,0,550));
  AddHisto("ECALNeig",new TH1F("ECALNeig","ECALNeig",9,-4.5,4.5));

  //histograms to debug mc wave IO
  AddHisto("nDigiForCh",new TH1F("nDigiForCh","nDigiForCh",20,0,20));
  AddHisto("nDigiForCh_1RecoHit", new TH1F("nDigiForCh_1RecoHi","nDigiForCh_1RecoHi",11, -0.5, 10.5));
  AddHisto("DEnergy_1RecoHit", new TH1F("DEnergy_1RecoHit","DEnergy_1RecoHit",400, -200, 200));
  AddHisto("DTime_1RecoHit", new TH1F("DTime_1RecoHit","DTime_1RecoHit",400, -200, 200));
  AddHisto("DEnergy_1RecoSaturatedHit", new TH1F("DEnergy_1RecoSaturatedHit","DEnergy_1RecoSaturatedHit",400, -1000, 1000));
  AddHisto("DTime_1RecoSaturatedHit", new TH1F("DTime_1RecoSaturatedHit","DTime_1RecoSaturatedHit",400, -200, 200));
  AddHisto("DigiEnergy_1RecoHitDTimeOut15", new TH1F("DigiEnergy_1RecoHitDTimeOut15", "DigiEnergy_1RecoHitDTimeOut15", 4000, 0., 4000.));

  // new histograms from Venelin
  AddHisto("ECALCellPos",new TH2F("ECALCellPos0cut","ECALCellPos0cut",30,0,30,30,0,30));
  AddHisto("ECALTDiffCos",new TH1F("ECALTDiffCos","ECALTDiffCos",200,-20,20));
  AddHisto("ECALHitTDiff",new TH1F("ECALHitTDiff","ECALHitTDiff",1000,-125,125));
  AddHisto("EtotInner",new TH1F("EtotInner","EtotInner",550,-1000.,10000.));
  AddHisto("EtotOuter",new TH1F("EtotOuter","EtotOuter",550,-1000.,10000.));
  AddHisto("EtotMiddle",new TH1F("EtotMiddle","EtotMiddle",550,-1000.,10000.));
  AddHisto("EtotCorner",new TH1F("EtotCorner","EtotCorner",550,-1000.,10000.));
  AddHisto("Etot",     new TH1F("Etot","Etot",1200,-1000.,5000.));
  AddHisto("Esum-2",     new TH1F("Esum-2","Esum-2",1000,0.,1000.));  
  AddHisto("Esum-2G",     new TH1F("Esum-2G","Esum-2G",1000,0.,1000.));

}

ECalReconstruction::~ECalReconstruction()
{;}


/*
TRecoVEvent * ECalReconstruction::ProcessEvent(TDetectorVEvent* tEvent, Event* tGenEvent)
{
  //common part for all the subdetectors
  PadmeVReconstruction::ProcessEvent(tEvent, tGenEvent);

  TECalEvent* tECalEvent = (TECalEvent*)tEvent;
  const TClonesArray& Digis = (*(tECalEvent->GetHits()));
  return fRecoEvent;
}
*/


void ECalReconstruction::ProcessEvent(TRawEvent* rawEv){

  // use trigger info
  if(fTriggerProcessor) {
    //std::cout<<"Reconstruction named <"<<GetName()<<"> processing TriggerInfo .... "<<std::endl;
    BuildTriggerInfo(rawEv);
    if (TriggerToBeSkipped()) return;
  }

  // from waveforms to Hits
  BuildHits(rawEv);

  if(fChannelCalibration) ((ECalCalibration*)fChannelCalibration)->PerformCalibration(GetRecoHits(),rawEv);
  if(fGeometry)           fGeometry->ComputePositions(GetRecoHits());

  // from Hits to Clusters
  ClearClusters();
  BuildClusters();
  if(fChannelCalibration) fChannelCalibration->PerformCalibration(GetClusters());

  //Processing is over, let's analyze what's here, if foreseen
  if(fGlobalRecoConfigOptions->IsMonitorMode()) {
    AnalyzeEvent(rawEv);
  }

}



bool ECalReconstruction::TriggerToBeSkipped()
{
  if ( GetGlobalRecoConfigOptions()->IsRecoMode()    && !(GetTriggerProcessor()->IsBTFTrigger())     ) return true;
  if ( GetGlobalRecoConfigOptions()->IsPedestalMode()&& !(GetTriggerProcessor()->IsAutoTrigger())    ) return true;
  if ( GetGlobalRecoConfigOptions()->IsCosmicsMode() && !(GetTriggerProcessor()->IsCosmicsTrigger()) ) return true;
  return false; 
}

void ECalReconstruction::BuildHits(TRawEvent* rawEv)
{
  //// This method differ from  PadmeVReconstruction::BuildHits(TRawEvent* rawEv) only because it attach board and elementID to the DigitizerChannelEcal ====>>> if ChannelVReco woudl store bd and element id, this assignemnt might be don in the base reco class. 
  
  ClearHits();
  vector<TRecoVHit *> &Hits  = GetRecoHits();

  ((DigitizerChannelECal*)fChannelReco)->SetTrigMask(GetTriggerProcessor()->GetTrigMask());
  UChar_t nBoards = rawEv->GetNADCBoards();

  TADCBoard* ADC;

  for(Int_t iBoard = 0; iBoard < nBoards; iBoard++) {
    ADC = rawEv->ADCBoard(iBoard);
    Int_t iBdID=ADC->GetBoardId();
    //std::cout<<"iBdID "<<iBdID<<std::endl;
    if(GetConfig()->BoardIsMine( iBdID )) {
      //Loop over the channels and perform reco
      for(unsigned ich = 0; ich < ADC->GetNADCChannels();ich++) {
	TADCChannel* chn = ADC->ADCChannel(ich);
	fChannelReco->SetDigis(chn->GetNSamples(),chn->GetSamplesArray());

	//New M. Raggi
 	Int_t ChID   = GetChannelID(ADC->GetBoardId(),chn->GetChannelNumber()); //give the geographical position
 	Int_t ElChID = chn->GetChannelNumber();
	//Store info for the digitizer class
 	((DigitizerChannelECal*)fChannelReco)->SetChID(ChID);
 	((DigitizerChannelECal*)fChannelReco)->SetElChID(ElChID);
 	((DigitizerChannelECal*)fChannelReco)->SetBdID(iBdID);
	/*
	if (ChID<0)std::cout<<"Why chId<0 ???? ChID="<<ChID<<" iBD/iCh= "<<iBoard<<"/"<<ich<<" ... ADC channel = "<<(Int_t)chn->GetChannelNumber()<<" in total NADCChannels() for this board = "<<(Int_t)ADC->GetNADCChannels()<<" ADB board n. "<<iBdID<<" tot#OfBoards = "<<(Int_t)nBoards<<std::endl;
	else std::cout<<"ChID="<<ChID<<" iBD/iCh= "<<iBoard<<"/"<<ich
		      <<" ... idBD/idCH = "<<(Int_t)ADC->GetBoardId()<<"/"<<(Int_t)chn->GetChannelNumber()
		      <<" in total NADCChannels() for this board = "<<(Int_t)ADC->GetNADCChannels()<<" tot#OfBoards = "<<(Int_t)nBoards<<std::endl;
	*/
	
	unsigned int nHitsBefore = Hits.size();
	fChannelReco->Reconstruct(Hits);
	unsigned int nHitsAfter = Hits.size();
	//if (ChID==1625) std::cout<<" n. of hits in chid 1625 = "<<nHitsAfter-nHitsBefore<<std::endl;
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


//void ECalReconstruction::ProcessEvent(TMCVEvent* tEvent, TMCEvent* tMCEvent)
void ECalReconstruction::BuildECalIslandRadiusClusters(Int_t type)
{
  /*
  if (type==3)std::cout<<"In BuildECalIslandClusters "<<std::endl;
  else if (type==4)std::cout<<"In BuildECalRadiusClusters "<<std::endl;
  */
  vector<TRecoVCluster *> &myClusters  = GetClusters();
  for(unsigned int iCl = 0;iCl < myClusters.size();iCl++){
    delete myClusters[iCl];
  }
  myClusters.clear();

  ECalParameters* para = ECalParameters::GetInstance();

  vector<TRecoVHit *> Hits = GetRecoHits();
  // Let's do some cluster finding
  ECalCrystalHandler* cryHand = new ECalCrystalHandler();
  for (Int_t ih=0; ih<Hits.size(); ++ih) {
    TRecoVHit* rhit = Hits[ih];
    Int_t     ch = rhit->GetChannelId();
    Double_t sig = rhit->GetEnergy();
    //std::cout<<" Crystal is given Energy / Time = "<<sig<<"  "<<rhit->GetTime()<<std::endl;
    Int_t iX = ch/100;
    Int_t iY = ch%100;
    ECalCrystal* cry = cryHand->CreateCrystal(iX,iY);
    cry->SetCharge(rhit->GetEnergy()/para->GetSignalToEnergy(ch));
    cry->SetEnergy(sig);
    cry->SetTime(rhit->GetTime());
    cry->SetHitIndex(ih);
    //cry->Print();
  }
  cryHand->SortEnergy();

  // Find clusters with PadmeIsland algorithm
  ECalClusterHandler* cluHand = new ECalClusterHandler();
  ECalClusterFinderIsland* cluFindIsl=NULL;
  ECalClusterFinderRadius* cluFindRad=NULL;
  Int_t newNClu = 0;
  if       (type==3) {
    cluFindIsl = new ECalClusterFinderIsland(cryHand,cluHand);
    newNClu = cluFindIsl->FindClusters();
    //printf("- Cluster finding result - PadmeIsland algorithm -\n");
  }
  else if  (type==4) {
    cluFindRad = new ECalClusterFinderRadius(cryHand,cluHand);
    newNClu = cluFindRad->FindClusters();
    //printf("- Cluster finding result - PadmeRadius algorithm -\n");
  }
  //cluHand->Print();

  std::vector<Int_t> vHitInCl;
  for (Int_t ic=0; ic<newNClu; ++ic) {
    TRecoVCluster* myCl = new TRecoVCluster();
    ECalCluster* cl = cluHand->GetCluster(ic);
    myCl->SetSeed        ( cl->GetSeed() );
    TRecoVHit* seedhit = Hits[cl->GetSeed()];
    myCl->SetChannelId   ( seedhit->GetChannelId() );
    myCl->SetEnergy      ( cl->GetEnergy() );
    myCl->SetTime        ( cl->GetTime() );
    //std::cout<<"Cluster is given energy / time = "<< cl->GetEnergy() << " "<< cl->GetTime()<<std::endl;
    myCl->SetPosition    ( TVector3( cl->GetXCenter(), cl->GetYCenter(), 1000.) );
    Int_t clSize = cl->GetNCrystals();
    myCl->SetNHitsInClus ( clSize );
    vHitInCl.clear();
    for (Int_t j=0; j<clSize; ++j)
      {
	vHitInCl.push_back( cl->GetCrystal(j)->GetHitIndex() );
      }
    myCl->SetHitVecInClus( vHitInCl );
    myClusters.push_back ( myCl );
  }



  delete cryHand;
  delete cluHand;
  if (cluFindIsl) delete cluFindIsl;
  if (cluFindRad) delete cluFindRad;
}

void ECalReconstruction::AnalyzeEvent(TRawEvent* rawEv){
  static int nevt;
  static TCanvas c;
  static int filled    = 0;
  vector<TRecoVHit *> &Hits  = GetRecoHits();
  //  return;
  //  std::cout<<" calling cluster routine "<<std::endl;
  // M. Raggi 22/11/2018 bulding ECal clusters
  // ClNCry.clear();
  // ClTime.clear();
  // ClE.clear();
  // ClX.clear();
  // ClY.clear();
  // ClSeed.clear();
  // SdTime.clear();
  // SdEn.clear();
  // SdCell.clear();
  //EvTotE=0;
  vector<TRecoVCluster *> myClus = GetClusters();
  int NClusters = myClus.size();//ECalBuildClusters(rawEv);

  //  GetHisto("ECALNPart")->Fill(ECALNPart);
  GetHisto("ECALNClus")->Fill(NClusters);
  //  GetHisto("ECALETot") ->Fill(EvTotE);


  for(int gg=0;gg<NClusters;gg++){
    GetHisto("ECALClE")->Fill( myClus[gg]->GetEnergy() );
    GetHisto("ECALClNCry")->Fill( myClus[gg]->GetNHitsInClus() );
    GetHisto("ECALClTime")->Fill( myClus[gg]->GetTime() );
    if(myClus[gg]->GetNHitsInClus()>1 &&  myClus[gg]->GetEnergy()>50.) {
      GetHisto("ECALClTimeCut")->Fill( myClus[gg]->GetTime() );
    }
  }

  float q1 = 0.;
  float q2 = 0.;
  float q3 = 0.;
  float q4 = 0.;
  float summa = 0.;
  float Einner = 0.;
  float Eouter = 0.;
  float Emiddle = 0.;
  float Ecorner = 0.;
  int NHighEgamma = 0;

  for(unsigned int iHit1 =  0; iHit1 < Hits.size(); ++iHit1) {
    //    std::cout<<"gli hit di Venelin "<<Hits.size()<<std::endl;
    int ich = Hits[iHit1]->GetChannelId();
    GetHisto("ECalOccupancy") -> Fill(ich/100,ich%100);

    ((TH2F *) GetHisto("ECalCharge")) -> Fill(ich/100,ich%100,Hits[iHit1]->GetEnergy());
    (GetHisto("ECalChCharge")) -> Fill(Hits[iHit1]->GetEnergy());

    if(Hits[iHit1]->GetEnergy()>5)NHighEgamma++;

    int ix = ich/100;
    int iy = ich%100;

    int nnn=nevt+1;
    summa += Hits[iHit1]->GetEnergy();
    if(ix>=7 && ix<22 && iy>=7 && iy<22) {
      Einner+= Hits[iHit1]->GetEnergy();
    } else {
      Eouter+= Hits[iHit1]->GetEnergy();
      float rr = sqrt( (ix-14.5)* (ix-14.5) +  (iy-14.5)* (iy-14.5) );
      if( rr < 13.) {
	Emiddle+= Hits[iHit1]->GetEnergy();
      } else {
	Ecorner+= Hits[iHit1]->GetEnergy();
      }
    }
    double carica, newcarica;

    carica = GetHisto("ECalOccupancyOffTime") -> GetBinContent(ix,iy);
    newcarica = (carica*nevt+Hits[iHit1]->GetEnergy())/nnn;
    GetHisto("ECalOccupancyOffTime") -> SetBinContent(ix,iy,newcarica);
    double tempo = Hits[iHit1]->GetTime();

    //    std::cout << "ECal time:  "<< tempo << "  "  << ich << std::endl;
    ((TH1F *) GetHisto("ECALTime")) -> Fill(tempo);
    if(ix > 14 && iy > 14) q1+= Hits[iHit1]->GetEnergy();
    if(ix < 14 && iy < 14) q3+= Hits[iHit1]->GetEnergy();
    if(ix < 14 && iy > 14) q2+= Hits[iHit1]->GetEnergy();
    if(ix > 14 && iy < 14) q4+= Hits[iHit1]->GetEnergy();
  }

  (GetHisto("Etot")) -> Fill(summa);
  (GetHisto("EtotInner")) -> Fill(Einner);
  (GetHisto("EtotOuter")) -> Fill(Eouter);
  (GetHisto("EtotCorner")) -> Fill(Ecorner);
  (GetHisto("EtotMiddle")) -> Fill(Emiddle);

  //  ((TH1F *) GetHisto("ECalTotCharge")) -> Fill(summa);

  //  std::cout << "Quadrants:  "<< q1 << "  "  << q2 << "  " << q3 << "  " << q4 << std::endl;

  if (q1 > 25. && q3 > 25. && filled==0) {
    for(unsigned int iHit1 =  0; iHit1 < Hits.size(); ++iHit1) {
      int ich = Hits[iHit1]->GetChannelId();
      GetHisto("ECalEvent")->SetBinContent(ich/100+1,ich%100+1, Hits[iHit1]->GetEnergy() );
      filled = 1;
    }
  }
  if (q2 > 25. && q4 > 25. && filled==0) {
    for(unsigned int iHit1 =  0; iHit1 < Hits.size(); ++iHit1) {
      int ich = Hits[iHit1]->GetChannelId();
      GetHisto("ECalEvent")->SetBinContent(ich/100+1,ich%100+1, Hits[iHit1]->GetEnergy() );
      filled = 1;
    }
  }


  //  HighEGamma

  if (NHighEgamma>1&&(GetHisto("ECalChEvent"))->GetEntries()==0) {
    for(unsigned int iHit1 =  0; iHit1 < Hits.size(); ++iHit1) {
      int ich = Hits[iHit1]->GetChannelId();
      GetHisto("ECalChEvent")->SetBinContent(ich/100+1,ich%100+1, Hits[iHit1]->GetEnergy() );
    }
  }

  //2gamma control histograms 
  float xcl[10];
  float ycl[10];
  float ecl[10];
  float tcl[10];

  for(int ii=0;ii<10;ii++) {
    xcl[ii] = 0.;
    ycl[ii] = 0.;
    ecl[ii] = 0.;
    tcl[ii] = 0.;
  }

  int icl=0;

  for(unsigned int iHit1 =  0; iHit1 < Hits.size() && icl < 10  ; ++iHit1) {
    if( Hits[iHit1]->GetEnergy()< 50  ) continue;
    //Otherwise - good seed:    
    int ich1 = Hits[iHit1]->GetChannelId();
    int ix1 = ich1/100;
    int iy1 = ich1%100;
    
    xcl[icl] = ix1;
    ycl[icl] = iy1;

    for(unsigned int iHit2 = iHit1 ; iHit2 < Hits.size(); ++iHit2) {
      //      if  ( Hits[iHit1]->GetEnergy()< 50 || Hits[iHit2]->GetEnergy()< 50  ) continue;
      int ich2 = Hits[iHit2]->GetChannelId();

      int ix2 = ich2/100;
      int iy2 = ich2%100;

      //Getting the cluster energy:
      if(fabs(ix1 - ix2) < 2 && fabs(iy1 - iy2) < 2) {
	//Cell belongs to the cluster
	ecl[icl] +=  Hits[iHit2]->GetEnergy();
      }
    }
    
    icl++;
  }

  if(icl == 2) {    
    GetHisto("Esum-2") ->Fill( ecl[0] + ecl[1]);
        
    if( xcl[0] < 10 && xcl[1] > 20  )   
      GetHisto("Esum-2G") ->Fill(ecl[0] + ecl[1] ); 
    if( xcl[0] > 20 && xcl[1] < 10  )   
      GetHisto("Esum-2G") ->Fill(ecl[0] + ecl[1] ); 
    
  }

  //Waveform histograms
//  char iName[1000];
//  UChar_t nBoards = rawEv->GetNADCBoards();
//  Double_t adc_count[1024][25]        ;
//  Double_t adc_pedestal   [25]        ;
//  Double_t adc_chsum    [1024]        ;
//  for(UShort_t s=0;s<1024;s++){
//     adc_chsum    [s] = 0;
//  }
  /*
  for(UChar_t b=0;b<nBoards;b++)// Loop over boards
  {
    TADCBoard* adcB = rawEv->ADCBoard(b);
    if(adcB->GetBoardId()!=18)continue; //should correspond to target board ;
    UChar_t nChn       = adcB ->GetNADCChannels(  );


   for(UChar_t c=0;c<nChn;c++){// Loop over channels
	 TADCChannel* chn = adcB->ADCChannel(c);
         UChar_t ch = chn->GetChannelNumber();
         if(ch>15)continue;
         for(UShort_t s=0;s<1024;s++){// 1-st loop over sampling
              adc_count[s][ch]= (Double_t) (chn->GetSample(s) ) ;
              adc_chsum[s] += adc_count[s][ch]/16;
	 }//End 1-st loop over sampling

         adc_pedestal[ch]=0;
         for(UShort_t s=0;s<200;s++){// 2-nd loop over sampling to calculate event by event pedestal
              adc_pedestal[ch] += adc_count[s][ch]/200 ;
	 }//End 2-nd loop over sampling


         for(UShort_t s=0;s<1024;s++){// 3-rd loop over sampling to remove event by event pedestal and fill waveform
             adc_count[s][ch] = adc_count[s][ch] - adc_pedestal[ch];
             float adc = float(adc_count[s][ch])/4096;
             sprintf(iName,"EcalCh%d",ch);
             (GetHisto(iName))->Fill(s,adc);
             sprintf(iName,"EcalLastCh%d",ch);
             (GetHisto(iName))->SetBinContent(s+1,     adc);
             (GetHisto(iName))->SetBinError  (s+1,  1/4096);
	 }
    }// End loop over channels
  }// End loop over boards
  */

  nevt ++;
}

//  Wirtten by M Raggi 22/11/2018
Int_t ECalReconstruction::FindSeed(Int_t nele, Int_t * Used, Double_t* Ene) {

  Int_t iMax=-1;
  Double_t fMax=0;

  for(UShort_t i = 0;i<nele;i++){
    //    std::cout<<"N hits "<<nele<<"Ene "<<Ene[i]<<" Ene "<<Used[i]<<std::endl;
    if (fMax < Ene[i] && Used[i]==0 && Ene[i]>fClEnThrForSeed) { //ene cut to check
      fMax = Ene[i];
      iMax = i;
    }
  }

  if(iMax>1024) std::cout<<"N hits "<<nele<<" "<<iMax<<" ene "<<fMax<<std::endl;
  return iMax;
}

//  Written by M. Raggi 22/11/2018
Int_t ECalReconstruction::IsSeedNeig(Int_t seedID, Int_t cellID) {
  //uses cellID to find neig cells wrt seed of the cluster
  Int_t IsNeig=-1;
  Int_t SeedRow=seedID/100;
  Int_t SeedCol=seedID%100;

  Int_t CellRow=cellID/100;
  Int_t CellCol=cellID%100;
  //excludes the seed cell
  if( abs(SeedRow-CellRow)<=fClDeltaCellMax && abs(SeedCol-CellCol)<=fClDeltaCellMax) IsNeig=1;
  //  std::cout<<"seedID "<<seedID<<" cellID "<<cellID<<" Is Neig "<<IsNeig<<std::endl;
  return IsNeig;
}

//  Written by M. Raggi 22/11/2018
void ECalReconstruction::BuildClusters()
{

  if      (fClusterizationAlgo==1) ECalReconstruction::BuildSimpleECalClusters();
  else if (fClusterizationAlgo==2) PadmeVReconstruction::BuildClusters();
  else if (fClusterizationAlgo==3 || fClusterizationAlgo==4) {// use island(3) or Radius(4) algo
    ECalReconstruction::BuildECalIslandRadiusClusters(fClusterizationAlgo);
  }
  return;
}


//  Written by M. Raggi 21/05/2019
Double_t ECalReconstruction::CompensateMissingE(Double_t ECl, Int_t ClSeed)
{
 Double_t EFraction;
  Int_t ClusterSize=5;
  //  EFraction[490]=0.95;
  //  cout<<"dime "<<fClDeltaCellMax<<endl;
  //std::cout << "in compensate missing e" << std::endl;
  if(fEnergyCompensation==NULL){
    fEnergyCompensation = new TF1("comp","pol4",0.,1000.);
    if(fClDeltaCellMax==2){
      fEnergyCompensation->SetParameters(0.915362,0.000196729,-4.50361e-07,4.58042e-10,-1.70299e-13); // pol4 fit of the MC
    }else{
      fEnergyCompensation->SetParameters(0.925666,0.000231776,-5.72621e-07,6.22445e-10,-2.44014e-13); // pol4 fit of the MC
    }
  }
  //  EFraction=0.95;
  EFraction = fEnergyCompensation->Eval(ECl);
  //std::cout << "fraction " << EFraction << std::endl;
  if(ECl>1000.) EFraction=1;
  if(ECl<30.)   EFraction=1;
  // std::cout<<ECl<<" Fraction "<<EFraction<<" Cl size"<<fClDeltaCellMax<<std::endl;
  // delete fEnergyCompensation;
  return EFraction;
}


void ECalReconstruction::BuildSimpleECalClusters()
{
  //std::cout<<"In BuildSimpleECalClusters "<<std::endl;
  vector<TRecoVCluster *> &myClusters  = GetClusters();
  for(unsigned int iCl = 0;iCl < myClusters.size();iCl++){
    delete myClusters[iCl];
  }
  myClusters.clear();
  //std::cout<<"myClusters is now cleared  "<<std::endl;

  ClNCry.clear();
  ClTime.clear();
  ClE.clear();
  ClX.clear();
  ClY.clear();
  ClSeed.clear();
  SdTime.clear();
  SdEn.clear();
  SdCell.clear();
  EvTotE=0;

  const int NMaxCl=600;
  const int NMaxHitsInCl=100;
  const int NRows=29;
  const int NCols=29;
  const int NTotCh=NRows*NCols;
  double TTotECALCh[NTotCh][NMaxCl];
  double ETotECALCh[NTotCh][NMaxCl];
  Double_t Time=0;

  vector<TRecoVHit *> &Hits  = GetRecoHits();
  int NCry=0;

  if(Hits.size()==0){
    NNoHits++;
    //    std::cout<<"No hits !!!! "<<NNoHits<<std::endl;
    return;// -1;
  }
  int NSeeds=0;
  int NGoodClus=0;
  //  std::cout<<"Ci sono No hits !!!!"<<Hits.size()<<std::endl;
  for(Int_t ll=0;ll<NMaxCl;ll++){
    //    ETotECAL[ll]=0;
    for(Int_t kk=0;kk<NTotCh;kk++){
      ETotECALCh[kk][ll]=0.;
      TTotECALCh[kk][ll]=0.;
      //      XECALCh[kk][ll]=0.;
      //      YECALCh[kk][ll]=0.;
    }
  }

  //fill the vector with hits informations  
  Double_t cTime[Hits.size()]={0.};
  Double_t cEnergy[Hits.size()]={0.};
  Int_t cChID[Hits.size()]={0};
  Int_t cUsed[Hits.size()]={0};
  Int_t cCellUsed[NTotCh]={0};


  for(unsigned int iHit1 =  0; iHit1 < Hits.size(); ++iHit1) {
    if (iHit1==3000) {
      std::cout<<"ECalReconstruction::BuildSimpleECalClusters--- WARNING: Too small buffers w.r.t. n. of hits in the event --- stip here"<<std::endl;
      break;
    }
    // std::cout<<"IOOOO entra nel loop " <<iHit1<<std::endl;
    cUsed[iHit1]  = 0;
    cTime[iHit1]  = Hits[iHit1]->GetTime();
    cEnergy[iHit1]= Hits[iHit1]->GetEnergy();
    if(cEnergy[iHit1]<fClEnThrForHit) {
      cUsed[iHit1]=1;
      //      std::cout<<"cUsed changed in loop " << std::endl;
    }
    cChID[iHit1]  = Hits[iHit1]->GetChannelId();
    //    std::cout<<iHit1<<" time in reco " <<cTime[iHit1]<<" chID "<<cChID[iHit1]<<" Ech "<<cEnergy[iHit1]<<std::endl;
  }

  Int_t iMax=0;
  Int_t HitUsed=0;
  Int_t clusMatrix[NMaxCl][NMaxHitsInCl]={0};
  //std::cout<<"Imax "<<iMax<<" "<<cEnergy[iHit1]<<" "<<NSeeds<<std::endl;
  while(iMax>-1){
    iMax = FindSeed(Hits.size(),cUsed, cEnergy);
    if(iMax<0) break;
    NCry=0;
    //    std::cout<<"Imax "<<iMax<<" "<<cEnergy[iMax]<<" "<<NSeeds<<std::endl;
    //  if(NSeeds==0){
    SdTime.push_back(cTime[iMax]);
    SdEn.push_back(cEnergy[iMax]);
    SdCell.push_back(cChID[iMax]);
    GetHisto("ECALClSeed")->Fill(SdCell[NSeeds]);
    GetHisto("ECALClSeedEn")->Fill(SdEn[NSeeds]);

    //create the cluster structure
    ClE.push_back(0.);
    ClTime.push_back(0.); //use seed time for the moment
    ClSeed.push_back(iMax);

    ClX.push_back(0.);  //carbon copy of the occupancy
    ClY.push_back(0.);  //carbon copy of the occupancy

    // we may want to use the space coordinates in this loop.
    for(unsigned int iHit1 =  0; iHit1 < Hits.size(); ++iHit1) {
      if (NCry >= NMaxHitsInCl)
	{
	  if (NCry == NMaxHitsInCl) std::cout<<"INFO: ECalReconstruction:: Maximum n. of Hits in this cluster reached ("<<NMaxHitsInCl<<"); go to next seed, if any"<<std::endl;
	  continue;
	}
      if( fabs(cTime[iHit1]-SdTime[NSeeds])<fClDeltaTime && cUsed[iHit1]==0 && IsSeedNeig(SdCell[NSeeds],cChID[iHit1])==1){
      //      if( fabs(cTime[iHit1]-SdTime[NSeeds])<15 && cUsed[iHit1]==0){// && IsSeedNeig(SdCell[NSeeds],cChID[iHit1])==1){

	//	Double_t XCl=(60.-cChID[iHit1]/10*30.);
	//	Double_t YCl=(-60.+cChID[iHit1]%10*30.);
	//std::cout<<"is neig "<<cChID[iHit1]<<" "<<cTime[iHit1]<<" "<<cEnergy[iHit1]<<std::endl;
	if (cEnergy[iHit1]<fClEnThrForHit) continue;
	//Int_t XCl=cChID[iHit1]/100;
	//Int_t YCl=cChID[iHit1]%100;
	Double_t XCl = Hits[iHit1]->GetPosition().X();
	Double_t YCl = Hits[iHit1]->GetPosition().Y();
	((TH2F *)GetHisto("ECALCellPos"))->Fill(XCl,YCl,cEnergy[iHit1]);
	cUsed[iHit1]=1;
	// keep track of the indices of hits contributing to the cluster
	clusMatrix[NSeeds][NCry] = iHit1;
	ClTime[NSeeds]+=cTime[iHit1]*cEnergy[iHit1];
	ClE[NSeeds]+=cEnergy[iHit1];
	ClX[NSeeds]+=XCl*cEnergy[iHit1];
	ClY[NSeeds]+=YCl*cEnergy[iHit1];
	if(SdCell[NSeeds]!=cChID[iHit1] && cEnergy[iHit1]>10.) GetHisto("ECALHitTDiff")->Fill( (cTime[iHit1]-SdTime[NSeeds]) );
	NCry++;
	HitUsed++;
	cCellUsed[cChID[iHit1]]=1;
      }
    }
    ClNCry.push_back(NCry);
    //GetHisto("ECALClE")->Fill(ClE[NSeeds]);
    //ClTime[NSeeds]=ClTime[NSeeds]/NCry;  //average time of the hit
    
    if (fClusterTimeAlgo == 0) {
      ClTime[NSeeds]= ClTime[NSeeds]/ClE[NSeeds];  //energy weighted average time of the hit
    } else {
      ClTime[NSeeds]= cTime[iMax];//ClTime[NSeeds]/ClE[NSeeds];  //energy weighted average time of the hit
    }    
    ClX[NSeeds]=ClX[NSeeds]/ClE[NSeeds];
    ClY[NSeeds]=ClY[NSeeds]/ClE[NSeeds];
    GetHisto("ECALClTDiff")->Fill(ClTime[NSeeds]-SdTime[NSeeds]);
    //    if(ClNCry[NSeeds]>9) std::cout<<NSeeds<<" Ncry >9 "<<NCry<<" Clseed "<<ClSeed[NSeeds]<<" ClE "<<ClE[NSeeds]<<std::endl;
    NSeeds++;
    if(NCry>1 || ClE[NSeeds]>10.) NGoodClus++;
    if (NSeeds==NMaxCl) 
      {
	std::cout<<"INFO: ECalReconstruction:: Maximum n. of clusters reached ("<<NMaxCl<<"); stop the search here"<<std::endl;
	iMax = -1;
      }
  }  //end of while loop on seeds

  std::vector<Int_t> tmpHitsInCl;
  for (Int_t iCl=0; iCl<NSeeds; ++iCl){
    // Correct the cluster energy for missing energy
    if(fCompensateMissingE) ClE[iCl]=ClE[iCl]/CompensateMissingE(ClE[iCl],ClSeed[iCl]);
    if(fDeteriorateEnergyResolution){//MC relative resolution 1.9%; data 4.9%
      Double_t sigma=EnergyResolution(ClE[iCl]);
      Double_t DetEnergy=r->Gaus(0.,sigma); //MeV
      //std::cout << DetEnergy << std::endl;
      ClE[iCl]=DetEnergy+ClE[iCl];
    }
    tmpHitsInCl.clear();
    TRecoVCluster* myCl = new TRecoVCluster();
    myCl->SetChannelId( SdCell[iCl] );
    myCl->SetEnergy( ClE[iCl]    );
    myCl->SetTime(   ClTime[iCl] );
    myCl->SetPosition(TVector3(ClX[iCl],ClY[iCl],Hits[0]->GetPosition().Z() ));
    myCl->SetSeed(ClSeed[iCl]);
    myCl->SetNHitsInClus(ClNCry[iCl]);
    //std::cout<<ClNCry[iCl]<<" Hits in cl. n. "<<iCl<<" = ";
    for (unsigned int j=0; j<ClNCry[iCl]; ++j)
      {
	tmpHitsInCl.push_back(clusMatrix[iCl][j]);
	//std::cout<<" "<<clusMatrix[iCl][j];
      }
    //std::cout<<endl;
    myCl->SetHitVecInClus(tmpHitsInCl);
    myClusters.push_back(myCl);
    tmpHitsInCl.clear();
    /*
    std::cout<<" done .. icl "<<iCl<<" energy = "<<myCl->GetEnergy()<<" "<<GetClusters()[iCl]->GetEnergy()<<std::endl;
    std::cout<<" cluster "<<iCl<<" id/e/t/nhit/hits"<<GetClusters()[iCl]->GetChannelId()<<"/"<<GetClusters()[iCl]->GetEnergy()<<"/"<<GetClusters()[iCl]->GetTime()<<"/"<<GetClusters()[iCl]->GetNHitsInClus()<<"/ ";
    for (int j=0; j<GetClusters()[iCl]->GetNHitsInClus(); ++j)
      {
	std::cout<<" "<<GetClusters()[iCl]->GetHitVecInClus()[j];
      }
    std::cout<<endl;
    */
  }


  for(unsigned int iHit1 =  0; iHit1 < Hits.size(); ++iHit1) {
    //	 ECALDropHitE
    GetHisto("ECALHitE")->Fill(cEnergy[iHit1]);
    if(cUsed[iHit1]==0){
      GetHisto("ECALDropHitE")->Fill(cEnergy[iHit1]);
    }
  }
  return;// NGoodClus;
}

void ECalReconstruction::ConvertMCDigitsToRecoHits(TMCVEvent* tEvent,TMCEvent* tMCEvent) {

   Int_t i=0;
  ifstream myfile;
  myfile.open ("newtemplate.txt");;
  if (myfile.is_open()){
   while (!myfile.eof()) {
   myfile >> fTemplate[i];
   //if(fGlobalMode->GetGlobalDebugMode()) hTemplate->SetBinContent(i,fTemplate[i]);
   // std::cout<< fTemplate[i] << std::endl;
   i++;
   }
  }
  fmaxValuemyTemplate=0.;
  for(int i=0; i<1001; i++){
    if(fTemplate[i]>fmaxValuemyTemplate){
     fmaxValuemyTemplate =fTemplate[i];
    }  
  }
  //std::cout << " max template " << fmaxValuemyTemplate << std::end;
  if (tEvent==NULL) return;
  fHits.clear();

 int idarray[50][50];
  for (Int_t j=0; j<50; ++j) {
      for (Int_t i=0; i<50; ++i) idarray[i][j]=0;
  }

  Double_t TimeShift=200.;
  for (Int_t i=0; i<tEvent->GetNDigi(); ++i) {
    TMCVDigi* digi = tEvent->Digi(i);
    Double_t oldTime=digi->GetTime();
    digi->SetTime(oldTime+TimeShift);
  }
  
  if(fReproductSACbunchStructure){
  Double_t minTime=30.;
  Double_t maxTime=170;
  for (Int_t i=0; i<tEvent->GetNDigi(); ++i) {
      TMCVDigi* digi = tEvent->Digi(i);
      Double_t digiTime=digi->GetTime();
      if(digiTime<minTime)minTime=digiTime;
      if(digiTime>maxTime)maxTime=digiTime;
      }
  Double_t width= maxTime-minTime;   
  
  for (Int_t i=0; i<tEvent->GetNDigi(); ++i) {
      TMCVDigi* digi = tEvent->Digi(i);
      Double_t digiTime=TMath::Nint(digi->GetTime());
      /*double xc = r->Rndm();
      double ydc = digiTime+(xc-0.5);
      double ys = fComulativeMax*(ydc-minTime)/width;*/
      double ys = fComulativeMax*(digiTime-minTime)/width;
      Int_t isbin = hCumulative->FindFirstBinAbove(ys);
      double xs = hCumulative->GetBinCenter(isbin);
      digi->SetTime(xs);
    }
 }

  std::vector<double> totalWaveform;
  std::vector<double> digiEne;
  std::vector<double> digiTime;
  std::vector<TimeEnergy> hitArrayInCrystal;
  hitArrayInCrystal.clear();
  vector<TRecoVHit *> &Hits  = GetRecoHits();

  //std::cout<<"Start new event conversion here ................................................................................"<<std::endl;
  int ndigi=0;
  
  for (Int_t i=0; i<tEvent->GetNDigi(); ++i) {
    TMCVDigi* digi = tEvent->Digi(i);
    //TRecoVHit *Hit = new TRecoVHit(digi);
    int i1 = digi->GetChannelId()/100;
    int i2 = digi->GetChannelId()%100;
    if (idarray[i1][i2]==1) continue;
    Bool_t BrokenSU=SimulateBrokenSU(i2,i1);
    if (BrokenSU)continue;
    std::cout<<"first loop , ev " << i<<" x " << i1 << " y " << i2 << " chID_first " <<  digi->GetChannelId()<< " at time " << digi->GetTime()<<" with energy " << digi->GetEnergy() << std::endl;
    hitArrayInCrystal.clear();
    ndigi=1;
    hitArrayInCrystal.push_back(TimeEnergy());
    hitArrayInCrystal.back().digiTime = digi->GetTime();
    hitArrayInCrystal.back().digiEnergy = digi->GetEnergy();
      
    int chIdN = i2*100+i1;
    //ss//std::cout<<"Hit in a new crystal   ......... id = "<<chIdN<<"  ... it was "<<digi->GetChannelId()<<std::endl;
    //double time = digi->GetTime();
    //double energy = digi->GetEnergy();
    for (Int_t j=i+1; j<tEvent->GetNDigi(); ++j) {
	TMCVDigi* digj = tEvent->Digi(j);
	int j1 = digj->GetChannelId()/100;
	int j2 = digj->GetChannelId()%100;
	//	std::cout<<"second loop , ev " << j<<" x " << j1 << " y " << j2 << " chID_first " <<  digj->GetChannelId() << std::endl;
	if (j1!=i1) continue;
	if (j2!=i2) continue;
	//std::cout<<" after continue " << std::endl;
	ndigi+=1;
	hitArrayInCrystal.push_back(TimeEnergy());
	hitArrayInCrystal.back().digiTime = digj->GetTime();
	hitArrayInCrystal.back().digiEnergy = digj->GetEnergy();
    }
    idarray[i1][i2]=1;
    GetHisto("nDigiForCh")->Fill(ndigi);
    std::cout<<"after second loop " << ndigi << std::endl;
    //Hit->SetChannelId(chIdN);
    //Hit->SetPosition(TVector3(0.,0.,0.)); 

    std::sort(hitArrayInCrystal.begin(), hitArrayInCrystal.end(), by_time());
    //std::cout<<"in crystal " << chIdN << " there are " << hitArrayInCrystal.size() << std::endl;
    // trasformare i digi/channel in waveform
    std::vector<double> tempVec;
    std::vector<double> wave;
    Double_t p0=1.097;
    Double_t p1=3.207;
    Double_t vMaxMC=0.; 
    for(int i=0; i< 1001; i++)wave.push_back(0);
    for(int j=0; j<hitArrayInCrystal.size(); j++){
      Double_t energy_= hitArrayInCrystal[j].digiEnergy;
      Double_t time_ = hitArrayInCrystal[j].digiTime;
      vMaxMC=p0+energy_*p1;
      std::cout << "for single waveform....time " << time_ << " energy "<< energy_ << " vmax " << vMaxMC << " vmaxVal " << fmaxValuemyTemplate <<std::endl;
      tempVec.clear();
      for(int i=0; i<5001; i++){
	tempVec.push_back(0);
      }
      for(int ll=0;ll<5000;ll++){
	if(ll+time_-20>= 0 && ll+time_-20<5000){
	  tempVec.at(ll+time_-20)=fTemplate[ll]*vMaxMC/fmaxValuemyTemplate;
	}
	//	std::cout<<"ll " <<ll <<" tempVec " << tempVec.at(ll) << " ftemplate " << fTemplate[ll]  << std::endl;
      }
      for(int h=0; h<1000; h++)wave.at(h)+=tempVec.at(h);
    }

    //capovolgere per renderla conforme ai dati
    Int_t ConversionFactor=4096/1000;
    Double_t meanPed=3776;
    Double_t sigmaPed=21.66;
    Double_t fAvg200=r->Gaus(meanPed, sigmaPed); // pedestal
    std::vector<double> waveLikeData;
    Short_t wave_forDigitiser[1024]={0.};
    Bool_t saturated=false;
    for(int i=0; i<1025; i++)waveLikeData.push_back(0);
    for(int ll=0;ll<1001;ll++){
      //if(ll< wave.size() && wave.at(ll)>900)wave.at(ll)=900;
      if(ll< wave.size())waveLikeData.at(ll)=-wave.at(ll)*ConversionFactor+fAvg200;
      else waveLikeData.at(ll)=0.;
      if(waveLikeData.at(ll)<10) {waveLikeData.at(ll)=10.;saturated=true;}
      // if(wave.at(ll)> 890) std::cout <<"wave close to the saturation " << waveLikeData.at(ll) << std::endl;
      //std::cout<<" in convert " << ll <<  " wave " << waveLikeData.at(ll) << " while starting point " << wave.at(ll) << std::endl;
      wave_forDigitiser[ll]= waveLikeData.at(ll);
      //std::cout<<"ll "<< ll << " wave for digitiser " <<  wave.at(ll) << std::endl;
    }
    for(int i=1000; i<1024; i++) wave_forDigitiser[i]=fAvg200;
    //introduce the noise
    Double_t meanNoise=-0.3492;
    Double_t sigmaNoise=1.95;
    for(int i=0; i<1024; i++){
      Double_t noise=r->Gaus(meanNoise, sigmaNoise);
      wave_forDigitiser[i]+=noise;
      }
    //for(int i=0; i< 1024 ; i++)std::cout << " i " << i <<" waveForDigitiser " <<  wave_forDigitiser[i] << std::endl;
    fChannelReco->SetDigis(1024, wave_forDigitiser);
    ((DigitizerChannelECal*)fChannelReco)->SetChID(chIdN);
    //std::cout<<"I've setted chId "<< ((DigitizerChannelECal*)fChannelReco)->GetChID() <<std::endl;
    unsigned int nHitsBefore = Hits.size();
    fChannelReco->Reconstruct(Hits);
    unsigned int nHitsAfter = Hits.size();
    //if (ChID==1625) std::cout<<" n. of hits in chid 1625 = "<<nHitsAfter-nHitsBefore<<std::endl;
    for(unsigned int iHit = nHitsBefore; iHit < nHitsAfter;++iHit) {
      Hits[iHit]->SetChannelId(chIdN);
    }
    if(nHitsAfter-nHitsBefore==1){
       GetHisto("nDigiForCh_1RecoHit")->Fill(ndigi);
       double energy=hitArrayInCrystal[0].digiEnergy;
       double ehitmax = energy;
       double thitemax = hitArrayInCrystal[0].digiTime;
       if (ndigi>1){
	 for (unsigned int i2 =  1; i2 < hitArrayInCrystal.size(); ++i2)
	   {
	     energy+=hitArrayInCrystal[i2].digiEnergy;
	     if (hitArrayInCrystal[i2].digiEnergy>ehitmax){
	       ehitmax = hitArrayInCrystal[i2].digiEnergy;
	       thitemax = hitArrayInCrystal[i2].digiTime;
	     }
	   }
       }
       GetHisto("DEnergy_1RecoHit")->Fill(energy-Hits[0]->GetEnergy());
       GetHisto("DTime_1RecoHit")->Fill(thitemax-Hits[0]->GetTime()); 
       if(fabs(thitemax-Hits[0]->GetTime())>15){
	 GetHisto("DigiEnergy_1RecoHitDTimeOut15")->Fill(energy);
       }
       if(saturated){
	 GetHisto("DEnergy_1RecoSaturatedHit")->Fill(energy-Hits[0]->GetEnergy());
	 GetHisto("DTime_1RecoSaturatedHit")->Fill(thitemax-Hits[0]->GetTime()); 
       }
    }
    // if(Hits.size()>1)std::cout<<"in loop setChId, time:  " << Hits.at(nHitsAfter-1)->GetTime() << " and energy " << Hits.at(nHitsAfter-1)->GetEnergy()<<std::endl;
    std::cout<<"--------------------------------------------end reconstruction " << std::endl;

  }



/*
  if (fMultihitForMC==1) {
    // if ideal multihit reconstruction is requested, convert each digit into a RecoHit 
    // MC to reco hits
    for (Int_t i=0; i<tEvent->GetNDigi(); ++i) {
      TMCVDigi* digi = tEvent->Digi(i);
      int i1 = digi->GetChannelId()/100;
      int i2 = digi->GetChannelId()%100;
      Bool_t BrokenSU=SimulateBrokenSU(i2,i1);
      if (BrokenSU)continue;
      TRecoVHit *Hit = new TRecoVHit();
      // @reconstruction level, the ECal ChIds are XXYY, while in MC they are YYXX 
      int chIdN = i2*100+i1;
      Hit->SetChannelId(chIdN);
      Hit->SetPosition(TVector3(0.,0.,0.)); 
      Hit->SetEnergy(digi->GetEnergy());
      Hit->SetTime(digi->GetTime());
      fHits.push_back(Hit);
    }
    return;
  }

  // emulating single hit reconstruction 
  if (fMultihitForMC == -2) {
    // special correction to compensate for different bunch length on data and MC
    for (Int_t i=0; i<tEvent->GetNDigi(); ++i) {
      TMCVDigi* digi = tEvent->Digi(i);
      digi->SetTime(0.52*(digi->GetTime()));
    }
  }
 

  // come here if fMultihitForMC==0 (sigle hit reco) or >=2 (2= multihit with threshold at 20 MeV for next to first hits in a crystal)
  int idarray[50][50];
  for (Int_t j=0; j<50; ++j) {
      for (Int_t i=0; i<50; ++i) idarray[i][j]=0;
  }

  std::vector<double> digiEne;
  std::vector<double> digiTime;
  std::vector<TimeEnergy> hitArrayInCrystal;
  hitArrayInCrystal.clear();

  //ss//std::cout<<"Start new event conversion here ................................................................................"<<std::endl;
  int ndigi=0;
  
  // MC to reco hits
  for (Int_t i=0; i<tEvent->GetNDigi(); ++i) {
    TMCVDigi* digi = tEvent->Digi(i);
    //TRecoVHit *Hit = new TRecoVHit(digi);
    int i1 = digi->GetChannelId()/100;
    int i2 = digi->GetChannelId()%100;
    if (idarray[i1][i2]==1) continue;
    Bool_t BrokenSU=SimulateBrokenSU(i2,i1);
    if (BrokenSU)continue;
    TRecoVHit *Hit = new TRecoVHit();
    hitArrayInCrystal.clear();
    ndigi=1;
    hitArrayInCrystal.push_back(TimeEnergy());
    hitArrayInCrystal.back().digiTime = digi->GetTime();
    hitArrayInCrystal.back().digiEnergy = digi->GetEnergy();
      
    int chIdN = i2*100+i1;
    //ss//std::cout<<"Hit in a new crystal   ......... id = "<<chIdN<<"  ... it was "<<digi->GetChannelId()<<std::endl;
    //double time = digi->GetTime();
    //double energy = digi->GetEnergy();
    for (Int_t j=i+1; j<tEvent->GetNDigi(); ++j) {
	TMCVDigi* digj = tEvent->Digi(j);
	int j1 = digj->GetChannelId()/100;
	int j2 = digj->GetChannelId()%100;
	if (j1!=i1) continue;
	if (j2!=i2) continue;
	ndigi+=1;
	hitArrayInCrystal.push_back(TimeEnergy());
	hitArrayInCrystal.back().digiTime = digj->GetTime();
	hitArrayInCrystal.back().digiEnergy = digj->GetEnergy();
    }
    //ss//std::cout<<" nDigi = "<<ndigi<<" time  array: ";for(Int_t j=0; j<ndigi; ++j) std::cout<<" "<<hitArrayInCrystal[j].digiTime;   std::cout<<std::endl;
    //ss//std::cout<<" nDigi = "<<ndigi<<" ener. array: ";for(Int_t j=0; j<ndigi; ++j) std::cout<<" "<<hitArrayInCrystal[j].digiEnergy; std::cout<<std::endl;
    idarray[i1][i2]=1;
    Hit->SetChannelId(chIdN);
    Hit->SetPosition(TVector3(0.,0.,0.)); 

    std::sort(hitArrayInCrystal.begin(), hitArrayInCrystal.end(), by_time());

    if (fMultihitForMC == 2)
      {
	Hit->SetEnergy(hitArrayInCrystal[0].digiEnergy);
	Hit->SetTime(hitArrayInCrystal[0].digiTime);
	fHits.push_back(Hit);
	for (Int_t dincr=1; dincr<3; ++dincr)
	  {
	    if (hitArrayInCrystal[dincr].digiEnergy < 20.) continue; 
	    TRecoVHit *Hit1 = new TRecoVHit();
	    Hit1->SetChannelId(chIdN);
	    Hit1->SetPosition(TVector3(0.,0.,0.));
	    Hit1->SetEnergy(hitArrayInCrystal[dincr].digiEnergy);
	    Hit1->SetTime(hitArrayInCrystal[dincr].digiTime);
	    fHits.push_back(Hit1);
	  }
	continue;
      }
    
    double energy=hitArrayInCrystal[0].digiEnergy;
    double ehit = 0;
    double ehitmax = energy;
    double thitemax = hitArrayInCrystal[0].digiTime;
    double t2 = 0;
    double t3 = 0;
    double tmin = hitArrayInCrystal[0].digiTime;
    double tEMaxIdeal = hitArrayInCrystal[0].digiTime;
    if (ndigi>1){
    for (unsigned int i2 =  1; i2 < hitArrayInCrystal.size(); ++i2)
      {
	energy+=hitArrayInCrystal[i2].digiEnergy;
	t2 = hitArrayInCrystal[i2].digiTime;

	if (hitArrayInCrystal[i2].digiEnergy>ehitmax){
	  ehitmax = hitArrayInCrystal[i2].digiEnergy;
	  tEMaxIdeal = t2;
	}

	for (unsigned int i3 =  0; i3<i2; ++i3)
	  {
	    t3 = hitArrayInCrystal[i3].digiTime;
	    ehit += hitArrayInCrystal[i2].digiEnergy*exp(-(t2-t3)/300.); // summing tails of previous hits
	  }
	ehit +=hitArrayInCrystal[i2].digiEnergy;//summing energy of this hit
	if (ehit > ehitmax)
	  {
	    ehitmax = ehit;
	    thitemax = t2;
	  }
      }
    }

    
    //if (multiHit)  
    //if (singleHit_firstTime) tmin 
    //if (singleHit_timeEmax)  tEmaxIdeal
    //if (singleHit_timeEmaxWFcorr) thitemax
    

    //ss//std::cout<<"RecoHit in the crystal   ......... id = "<<chIdN<<" tmin, tEMaxIdeal, thitemax = "<<tmin<<" "<<tEMaxIdeal<<" "<<thitemax<<"         energy = "<<energy<<std::endl;
    Hit->SetEnergy(energy);
    Hit->SetTime(thitemax);
    fHits.push_back(Hit);
  }*/
}




Bool_t ECalReconstruction::SimulateBrokenSU(Int_t x, Int_t y){
  Bool_t BrSU=false;
  if(x==16 && y==25)   BrSU=true;
  if(x==18 && y==10)  BrSU=true;
  if(x==22 && y==8 )  BrSU=true;
  if(x==18 && y==4 )  BrSU=true;
  return BrSU;                                                                                                                                                  

}




Double_t ECalReconstruction::EnergyResolution(Double_t energy){
  Double_t a=0.02;
  Double_t b=0.00003;
  Double_t c=0.012;
  Double_t E=energy/1000.;
  Double_t RelativeResolution=sqrt((a/sqrt(E))*(a/sqrt(E))+(b/E)*(b/E)+c*c);
  return RelativeResolution*energy; 
}
