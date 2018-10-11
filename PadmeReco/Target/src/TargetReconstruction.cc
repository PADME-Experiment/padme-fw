// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "TargetReconstruction.hh"

#include "TTargetMCEvent.hh"
#include "TTargetMCHit.hh"
#include "TTargetMCDigi.hh"
#include "DigitizerChannelTarget.hh"
#include "TH2F.h"
#include "TH1F.h"


TargetReconstruction::TargetReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "Target", ConfigFileName)
{
  //fRecoEvent = new TRecoTargetEvent();
  //ParseConfFile(ConfigFileName);
  fChannelReco = new DigitizerChannelTarget();
}

void TargetReconstruction::HistoInit(){
  AddHisto("TargetProfile", new TH1F("TargetProfile" ,"Target profile"           ,32,0.5,32.5));
  AddHisto("TargetProfileX",new TH1F("TargetProfileX","Target profile X"         ,16,-7.5,8.5));
  AddHisto("TargetProfileY",new TH1F("TargetProfileY","Target profile Y"         ,16,-7.5,8.5));
  AddHisto("TargetXYmap"   ,new TH2F("TargetXYmap"   ,"Target XY map",16,-7.5,8.5,16,-7.5,8.5));
  
  
  GetHisto("TargetProfile")->GetXaxis()->SetTitle("StripX or StripY+16");
  GetHisto("TargetProfile")->GetYaxis()->SetTitle("Charge[pC]");
  
  GetHisto("TargetProfileX")->GetXaxis()->SetTitle("x[mm]");
  GetHisto("TargetProfileX")->GetYaxis()->SetTitle("Charge[pC]");
   
  GetHisto("TargetProfileY")->GetXaxis()->SetTitle("y[mm]");
  GetHisto("TargetProfileY")->GetYaxis()->SetTitle("Charge[pC]");

  ((TH2F *)GetHisto("TargetXYmap"))->GetXaxis()->SetTitle("x[mm]");
  ((TH2F *)GetHisto("TargetXYmap"))->GetYaxis()->SetTitle("y[mm]");
}


TargetReconstruction::~TargetReconstruction()
{;}

// void TargetReconstruction::Init(PadmeVReconstruction* MainReco)
// {

//   //common part for all the subdetectors
//   PadmeVReconstruction::Init(MainReco);

// }

// // Read Target reconstruction parameters from a configuration file
// void TargetReconstruction::ParseConfFile(TString ConfFileName) {

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

void TargetReconstruction::ProcessEvent(TMCVEvent* tEvent, TMCEvent* tMCEvent)
{
  PadmeVReconstruction::ProcessEvent(tEvent,tMCEvent);
  TTargetMCEvent* tTargetEvent = (TTargetMCEvent*)tEvent;
  std::cout << "--- TargetReconstruction --- run/event/#hits/#digi " << tTargetEvent->GetRunNumber() << " " << tTargetEvent->GetEventNumber() << " " << tTargetEvent->GetNHits() << " " << tTargetEvent->GetNDigi() << std::endl;
  for (Int_t iH=0; iH<tTargetEvent->GetNHits(); iH++) {
    TTargetMCHit* hit = (TTargetMCHit*)tTargetEvent->Hit(iH);
    hit->Print();
  }
  for (Int_t iD=0; iD<tTargetEvent->GetNDigi(); iD++) {
    TTargetMCDigi* digi = (TTargetMCDigi*)tTargetEvent->Digi(iD);
    digi->Print();
  }
}

//void TargetReconstruction::EndProcessing()
//{;
 //GetHisto("TargetOccupancy")->Fit("gaus","","",16.5,32.5);
 //GetHisto("TargetOccupancy")->Fit("gaus","","",0.5, 16.5);
//}

void TargetReconstruction::AnalyzeEvent(TRawEvent* rawEv){
  vector<TRecoVHit *> &Hits  = GetRecoHits();

  //std::cout<<" SS Target Hits.size "<< Hits.size()<<std::endl;

  float Qx=0;
  float Qy=0;
  for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
    //std::cout<<" hit id, energy = "<<Hits[iHit1]->GetChannelId()<<" "<<Hits[iHit1]->GetEnergy()<<std::endl;
    GetHisto("TargetProfile" )->Fill(Hits[iHit1]->GetChannelId()     ,Hits[iHit1]->GetEnergy());
    if(iHit1<16){
     Qx += Hits[iHit1]->GetEnergy();
     GetHisto("TargetProfileX")->Fill(Hits[iHit1]->GetChannelId()   -8,Hits[iHit1]->GetEnergy());
    }
    else{
     Qy += Hits[iHit1]->GetEnergy();
     GetHisto("TargetProfileY")->Fill(Hits[iHit1]->GetChannelId()-16-8,Hits[iHit1]->GetEnergy());
    }
  }

  if(Hits.size()==32&&Qx>0&&Qy>0)for(unsigned int iHitx = 0; iHitx < 16;++iHitx) for(unsigned int iHity = 16; iHity < 32;++iHity){{
    float qx = Hits[iHitx]->GetEnergy()/Qx; if(qx<0)qx= 0.01;
    float qy = Hits[iHity]->GetEnergy()/Qy; if(qy>0)qy=-0.01;
    float x  = float(Hits[iHitx]->GetChannelId())   -8;
    float y  = float(Hits[iHity]->GetChannelId())-16-8;
    ((TH2F *)GetHisto("TargetXYmap"))->Fill(x, y, -qx*qy);
  }}



}
