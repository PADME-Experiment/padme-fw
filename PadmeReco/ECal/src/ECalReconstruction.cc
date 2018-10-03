// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "ECalReconstruction.hh"

#include "ECalParameters.hh"
#include "ECalCrystalHandler.hh"
#include "ECalCrystal.hh"
#include "ECalClusterFinderIsland.hh"
#include "ECalClusterFinderRadius.hh"
#include "ECalCluster.hh"

#include "TECalMCEvent.hh"
#include "TECalMCHit.hh"
#include "TECalMCDigi.hh"
#include "DigitizerChannelReco.hh"
#include "TH2F.h"
#include "TCanvas.h"

ECalReconstruction::ECalReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "ECal", ConfigFileName)
{
  //fRecoEvent = new TRecoECalEvent();
  //ParseConfFile(ConfigFileName);
  fChannelReco = new DigitizerChannelReco();
}

void ECalReconstruction::HistoInit(){
  AddHisto("ECalOccupancy",new TH2F("ECalOccupancy","ECalOccupancy",31,0,31,31,0,31));
  AddHisto("ECalEvent",new TH2F("ECalEvent","ECalEvent",31,0,31,31,0,31));

  AddHisto("ECalCharge",new TH2F("ECalCharge","ECalCharge",31,0,31,31,0,31));

}




ECalReconstruction::~ECalReconstruction()
{;}

// void ECalReconstruction::Init(PadmeVReconstruction* MainReco)
// {

//   //common part for all the subdetectors
//   PadmeVReconstruction::Init(MainReco);

// }

// // Read ECal reconstruction parameters from a configuration file
// void ECalReconstruction::ParseConfFile(TString ConfFileName) {

//   std::ifstream confFile(ConfFileName.Data());
//   if (!confFile.is_open()) {
//     perror(ConfFileName);
//     exit(1);
//   }

//   TString Line;
//   while (Line.ReadLine(confFile)) {
//     if (Line.BeginsWith("#")) continue;
//   }
//   confFile.close();
// }

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


void ECalReconstruction::ProcessEvent(TMCVEvent* tEvent, TMCEvent* tMCEvent)
{

  ECalParameters* para = ECalParameters::GetInstance();

  PadmeVReconstruction::ProcessEvent(tEvent,tMCEvent);
  TECalMCEvent* tECalEvent = (TECalMCEvent*)tEvent;
  std::cout << "--- ECalReconstruction --- run/event/#hits/#digi " << tECalEvent->GetRunNumber() << " " << tECalEvent->GetEventNumber() << " " << tECalEvent->GetNHits() << " " << tECalEvent->GetNDigi() << std::endl;
  for (Int_t iH=0; iH<tECalEvent->GetNHits(); iH++) {
    TECalMCHit* hit = (TECalMCHit*)tECalEvent->Hit(iH);
    hit->Print();
  }
  for (Int_t iD=0; iD<tECalEvent->GetNDigi(); iD++) {
    TECalMCDigi* digi = (TECalMCDigi*)tECalEvent->Digi(iD);
    digi->Print();
  }

  // Let's do some cluster finding
  ECalCrystalHandler* cryHand = new ECalCrystalHandler();
  for (Int_t iD=0; iD<tECalEvent->GetNDigi(); iD++) {
    TECalMCDigi* digi = (TECalMCDigi*)tECalEvent->Digi(iD);
    Int_t ch = digi->GetChannelId();
    Double_t sig = digi->GetSignal();
    Int_t iX = ch/100;
    Int_t iY = ch%100;
    ECalCrystal* cry = cryHand->CreateCrystal(iX,iY);
    cry->SetCharge(sig);
    cry->SetEnergy(sig*para->GetSignalToEnergy(ch));
    cry->SetTime(digi->GetTime());
    cry->Print();
  }
  cryHand->SortEnergy();

  // Find clusters with PadmeIsland algorithm
  ECalClusterHandler* cluHandIsl = new ECalClusterHandler();
  ECalClusterFinderIsland* cluFindIsl = new ECalClusterFinderIsland(cryHand,cluHandIsl);
  Int_t newNClu = cluFindIsl->FindClusters();
  printf("- Cluster finding result - PadmeIsland algorithm -\n");
  cluHandIsl->Print();

  // Find clusters with PadmeRadius algorithm
  ECalClusterHandler* cluHandRad = new ECalClusterHandler();
  ECalClusterFinderRadius* cluFindRad = new ECalClusterFinderRadius(cryHand,cluHandRad);
  Int_t newNCluRad = cluFindRad->FindClusters();
  printf("- Cluster finding result - PadmeRadius algorithm -\n");
  cluHandRad->Print();

  // Here you can do something with your clusters

  // Final cleanup

  delete cryHand;
  delete cluHandIsl;
  delete cluFindIsl;
  delete cluHandRad;
  delete cluFindRad;

}

// void ECalReconstruction::EndProcessing()
// {;}
void ECalReconstruction::AnalyzeEvent(TRawEvent* rawEv){
  static int nevt;
  static TCanvas c;
  static int filled  = 0;

  vector<TRecoVHit *> &Hits  = GetRecoHits();
  //  return;

  float q1 = 0.;
  float q2 = 0.;
  float q3 = 0.;
  float q4 = 0.;


  for(unsigned int iHit1 =  0; iHit1 < Hits.size(); ++iHit1) {
    int ich = Hits[iHit1]->GetChannelId();
    GetHisto("ECalOccupancy") -> Fill(ich/100,ich%100);
    //    GetHisto("ECalCharge") -> Fill(ich/10,ich%10,Hits[iHit1]->GetEnergy());
    
    int ix = ich/100;
    int iy = ich%100;
    
    if(ix < 14 && iy < 14) q3+= Hits[iHit1]->GetEnergy();
    if(ix < 14 && iy > 14) q2+= Hits[iHit1]->GetEnergy();
    if(ix > 14 && iy > 14) q1+= Hits[iHit1]->GetEnergy();
    if(ix > 14 && iy < 14) q4+= Hits[iHit1]->GetEnergy();
        
  }

  if (q1 > 200 && q3 > 200 && filled==0) { 
    for(unsigned int iHit1 =  0; iHit1 < Hits.size(); ++iHit1) {
      int ich = Hits[iHit1]->GetChannelId();
      GetHisto("ECalEvent") -> SetBinContent(ich/100,ich%100, Hits[iHit1]->GetEnergy() );
      filled = 1;
    }
  }
  



  // if(nevt % 100 == 0) {
  //   c.cd();
  //   GetHisto("ECalOccupancy") -> Draw();
  //   c.Update();
  // }

  nevt ++;
}

