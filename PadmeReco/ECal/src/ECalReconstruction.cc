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
#include "TH1F.h"
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
  AddHisto("ECalOccupancyOffTime",new TH2F("ECalOccupancyOffTime","ECalOccupancyOffTime",31,0,31,31,0,31));
  AddHisto("ECalEvent",new TH2F("ECalEvent","ECalEvent",31,0,31,31,0,31));
  AddHisto("ECalChEvent",new TH2F("ECalChEvent","ECalChEvent",31,0,31,31,0,31));
  AddHisto("ECalCharge",new TH2F("ECalCharge","ECalCharge",31,0,31,31,0,31));
  AddHisto("ECalTotCharge",new TH1F("ECalTotCharge","ECalTotCharge",1001,0,100));
  AddHisto("ECalChCharge",new TH1F("ECalChCharge","ECalChCharge",1001,0,100));
  AddHisto("ECalTime",new TH1F("ECalTime","ECalTime",1000,0,1000));

  //Waveform histograms
  for(int iCh=0; iCh!=15+1 ; iCh++){
    char iName[100];

    sprintf(iName,"EcalCh%d",iCh);
    AddHisto(iName, new TH1F(iName, iName,  1024,  0, 1024 ));
    (GetHisto(iName))->GetXaxis()->SetTitle("Sampling #");
    (GetHisto(iName))->GetYaxis()->SetTitle("Amplitude[V]");

    sprintf(iName,"EcalLastCh%d",iCh);
    AddHisto(iName, new TH1F(iName, iName,  1024,  0, 1024 ));
    (GetHisto(iName))->GetXaxis()->SetTitle("Sampling #");
    (GetHisto(iName))->GetYaxis()->SetTitle("Amplitude[V]");
  }

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
  static int filled    = 0;
  vector<TRecoVHit *> &Hits  = GetRecoHits();
  //  return;

  float q1 = 0.;
  float q2 = 0.;
  float q3 = 0.;
  float q4 = 0.;
  float summa = 0.;

  int NHighEgamma = 0;

  for(unsigned int iHit1 =  0; iHit1 < Hits.size(); ++iHit1) {
    int ich = Hits[iHit1]->GetChannelId();
    /*
    if( 
//	1 ||  
	Hits[iHit1]->GetTime() < 450.  ) {
    */
    GetHisto("ECalOccupancy") -> Fill(ich/100,ich%100);
	//    }

    /*    if(
        Hits[iHit1]->GetTime() >450 && Hits[iHit1]->GetTime() < 800  ) {
        GetHisto("ECalOccupancyOffTime") -> Fill(ich/100,ich%100);
    }
    */

    ((TH2F *) GetHisto("ECalCharge")) -> Fill(ich/100,ich%100,Hits[iHit1]->GetEnergy());
    (GetHisto("ECalChCharge")) -> Fill(Hits[iHit1]->GetEnergy());
   
    if(Hits[iHit1]->GetEnergy()>5)NHighEgamma++;

    int ix = ich/100;
    int iy = ich%100;

    int nnn=nevt+1;
    summa += Hits[iHit1]->GetEnergy();
    double carica, newcarica;

    carica = GetHisto("ECalOccupancyOffTime") -> GetBinContent(ix,iy);
    newcarica = (carica*nevt+Hits[iHit1]->GetEnergy())/nnn;
    GetHisto("ECalOccupancyOffTime") -> SetBinContent(ix,iy,newcarica);
    double tempo = Hits[iHit1]->GetTime();

    //    std::cout << "ECal time:  "<< tempo << "  "  << ich << std::endl;
    if (tempo > 0) ((TH1F *) GetHisto("ECalTime")) -> Fill(tempo);
    if(ix > 14 && iy > 14) q1+= Hits[iHit1]->GetEnergy();
    if(ix < 14 && iy < 14) q3+= Hits[iHit1]->GetEnergy();
    if(ix < 14 && iy > 14) q2+= Hits[iHit1]->GetEnergy();
    if(ix > 14 && iy < 14) q4+= Hits[iHit1]->GetEnergy();
    
  }
  
  ((TH1F *) GetHisto("ECalTotCharge")) -> Fill(summa);

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


  // if(nevt % 100 == 0) {
  //   c.cd();
  //   GetHisto("ECalOccupancy") -> Draw();
  //   c.Update();
  // }

  //Waveform histograms
  char iName[1000];
  UChar_t nBoards = rawEv->GetNADCBoards();
  Double_t adc_count[1024][25]        ; 
  Double_t adc_pedestal   [25]        ;
  Double_t adc_chsum    [1024]        ; 
  for(UShort_t s=0;s<1024;s++){  
     adc_chsum    [s] = 0;
  }
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


  nevt ++;
}

