// Written by E. Leonardi 
// Last modified by M. Raggi 2/08/2021
#include "SinglePhoton.hh"
#include "TGraph.h"

SinglePhoton::SinglePhoton(TString cfgFile, Int_t verbose)
{
  Neve=0;
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating SinglePhoton\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  fHS = HistoSvc::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
}

SinglePhoton::~SinglePhoton(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t SinglePhoton::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing SinglePhoton\n");
  fEvent = event;
  cout<<" Creating Hystograms for Run Init "<<fNRun<<" "<<endl;  
  fNRun = fEvent->RecoEvent->GetRunNumber(); //30000 vale solo per il 2022
  InitHistos();
  Neve=0;
  fCurrentRun=-1;

  Navg=500;  //10 s at 49 Hz
  NPoTAvg=0;
  ETotECal=0;
  EToTAvg=0;
  
  //Defining Cuts
  ClRadMin = 90.;
  ClRadMax = 270.;

  MinECluster = 50.;
  MaxECluster = 270.;

  return true;
}

Bool_t SinglePhoton::InitHistos(){
  // SinglePhoton directory will contain all histograms related to this analysis
  fHS->CreateList("SinglePhoton");
  cout<<" Creating Hystograms for Run "<<fNRun<<" "<<endl;  
  // Target related quantities 
  fHS->BookHistoList("SinglePhoton","ETotECal",1000,0.,10000.);
  fHS->BookHistoList("SinglePhoton","ECalNClusters",50,-0.5,49.5);
  fHS->BookHistoList("SinglePhoton","ECalClE",1000,0.,1000.);
  fHS->BookHistoList("SinglePhoton","NPoTLG",1000,0.,10000.);
  fHS->BookHistoList("SinglePhoton","HitETotOvNPoT",100,0.,2.);
  fHS->BookHisto2List("SinglePhoton","EnergyMapRawCol",30,0.,30.,30,0.,30.);   
  // Lead Glass related quantities 
  //  fHS->BookHisto2List("SinglePhotonLG","NPoTLGvsXTar",600,-15.,15.,NBinsPOT,Min_POT,Max_POT);
  return true;
}

Bool_t SinglePhoton::Process(){
  Double_t ETotECal = 0;
  Int_t NGoodCl=0;

  fNRun = fEvent->RecoEvent->GetRunNumber(); //30000 vale solo per il 2022
  //  fTimeStamp=fEvent->RecoEvent->GetEventTime(); 
  //  cout<<"Run Number "<<fNRun<<" "<<endl;

  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  //Cut on physics trigger Data Only
  if( !(trigMask & (1 << 0))) return false;

  Double_t NPoTLG=0; 
  if(fEvent->LeadGlassRecoEvent!=0) NPoTLG = fEvent->LeadGlassRecoEvent->GetNPoTs();
  fHS->FillHistoList("SinglePhoton","NPoTLG",NPoTLG);

  //Cut on empty events.
  if(NPoTLG<250) return false;

  Double_t TimeStamp=fEvent->RecoEvent->GetEventTime(); 
  ETotECal=0;
  NPoTAvg+=NPoTLG;

  Int_t NClusters =fEvent->ECalRecoCl->GetNElements();
  fHS->FillHistoList("SinglePhoton","ECalNClusters",NClusters);
  //   UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();

  for(int ical = 0;ical < NClusters; ical++){
    double eECal = fEvent->ECalRecoCl->Element(ical)->GetEnergy();
    ETotECal+=eECal;
  }
  Int_t NEventHits = fEvent->ECalRecoEvent->GetNHits();
  Double_t TotHitE=0;
  for(int iHit = 0;iHit <NEventHits ; iHit++) {
    //    ChId = fEvent->ECalRecoEvent->Hit(iHit)->GetChannelId();
    double HitE = fEvent->ECalRecoEvent->Hit(iHit)->GetEnergy();
    //  Row=ChId%100;   Col=ChId/100;
    //    cout<<" Entering NHits "<<NEventHits<<" IHit "<<iHit<<" "<<ChId<<" Row "<<Row<<" Col "<<Col<<""<<HitE<<endl;
    //    if(fEvent->ECalRecoEvent->Hit(iHit)->GetEnergy()>20.) fHS->FillHistoList("SinglePhoton","EnergyMapRawCol",Col,Row);
    TotHitE+=HitE;
  }

  //FROM IS22gg   
  HitEToTAvg += TotHitE;
  EToTAvg    += ETotECal;
  if(Neve%Navg==0){
    //    cout<<Neve<<" NPot "<<NPoTAvg<<" NAVG "<<Navg<<" NPotLG "<<NPoTLG<<endl;
    NPoTAvg/=Navg;
    EToTAvg/=Navg;
    HitEToTAvg/=Navg;

    vNPoT.push_back(NPoTAvg);
    vNEvt.push_back(TimeStamp);
    vETotECal.push_back(EToTAvg);
    vRatio.push_back(EToTAvg/NPoTAvg);
    //    cout<<"NPot "<<NPoTAvg<<endl;
    //    fHS->FillHistoList("GGAnalysis","ETotOvNPoT",EToTAvg/NPoTAvg);
    fHS->FillHistoList("SinglePhoton","HitETotOvNPoT",HitEToTAvg/NPoTAvg);
    NPoTAvg=0;
    EToTAvg=0;
    HitEToTAvg=0;
  }

  for(int ical = 0;ical < NClusters; ical++) {
    double eECal    =  fEvent->ECalRecoCl->Element(ical)->GetEnergy();
    double tECal    =  fEvent->ECalRecoCl->Element(ical)->GetTime();
    TVector3 pos    =  fEvent->ECalRecoCl->Element(ical)->GetPosition();  
    fHS->FillHistoList("SinglePhoton","ECalClE",eECal);
    
    ETotECal+=eECal; 

    if(eECal<MinECluster) continue; 
    if(eECal>MaxECluster) continue; 
    NGoodCl++;
    //    fHS->FillHistoList("SinglePhoton","ECalGoodClusters",NGoodCl);
  }

  fHS->FillHistoList("SinglePhoton","ETotECal",ETotECal);
  //  if( !(trigMask & (1 << 0)) ) is non physics trigger
  //Data cut o
  //  if( (trigMask & (1 << 0)) )  is A PHYSIC TRIGGER there is beam
  
  fCurrentRun = fNRun;
  Neve++;
  return Neve++;
}

Bool_t SinglePhoton::Finalize()
{
  TGraph* nPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vNPoT[0]);
  nPotVsTime->SetMarkerStyle(21);
  fHS->SaveTGraphList("SinglePhoton","NPotVsTime",nPotVsTime);


  TGraph* EovNPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vRatio[0]);
  EovNPotVsTime->SetMarkerStyle(21);
  fHS->SaveTGraphList("SinglePhoton","EovNPotVsTime",EovNPotVsTime);

//  std::cout<<"TotPot Over5  = "<<TotPoTOver5K<<std::endl;

  if (fVerbose) printf("---> Finalizing SinglePhoton\n");
  return true;
}
