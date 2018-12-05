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
#include "TRecoVCluster.hh"


ECalReconstruction::ECalReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "ECal", ConfigFileName)
{
  //fRecoEvent = new TRecoECalEvent();
  //ParseConfFile(ConfigFileName);
  fChannelReco = new DigitizerChannelReco();
  fClusters.clear();
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




  AddHisto("ECALClPos",new TH2F("ECALClPos","ECALClPos",30,-15,15,30,-15,15));
  AddHisto("ECALCellPos",new TH2F("ECALCellPos","ECALCellPos",30,0,30,30,0,30));
  //  AddHisto("ECALVoters",new TH2F("ECALVoters","ECALVoters",1000,0.,1000.,26,-0.5,25.5));

  AddHisto("ECALTime",new TH1F("ECALTime","ECALTime",200,0,1000));
  AddHisto("ECALTimeCut",new TH1F("ECALTimeCut","ECALTimeCut",200,0,1000));
  AddHisto("ECALClTime",new TH1F("ECALClTime","ECALClTime",200,0,1000));
  AddHisto("ECALClTimeCut",new TH1F("ECALClTimeCut","ECALClTimeCut",200,0,1000));
  AddHisto("ECALETot",new TH1F("ECALETot","ECALETot",500,0,2500));
  AddHisto("ECALQTot",new TH1F("ECALQTot","ECALQTot",500,0,2500));
  AddHisto("ECALNPart",new TH1F("ECALNPart","ECALNPart",200,0,200));
  AddHisto("ECALNClus",new TH1F("ECALNClus","ECALNClus",200,0,200));

  AddHisto("ECALDropHitE",new TH1F("ECALDropHitE","ECALDropHitE",500,0,500));
  AddHisto("ECALHitE",new TH1F("ECALHitE","ECALHitE",550,0,550));

  AddHisto("ECALRawClus",new TH1F("ECALRawClus","ECALRawClus",200,0,1000));
  AddHisto("ECALClE",new TH1F("ECALClE","ECALClE",200,0,1000));
  
  AddHisto("ECALClNCry",new TH1F("ECALClNCry","ECALClNCry",30,0,30));
  AddHisto("ECALClTDiff",new TH1F("ECALClTDiff","ECALClTDiff",100,-10,10));

  AddHisto("ECALClSeed",new TH1F("ECALClSeed","ECALClSeed",3000,0,3000));
  AddHisto("ECALClSeedEn",new TH1F("ECALClSeedEn","ECALClSeedEn",550,0,550));
  AddHisto("ECALNeig",new TH1F("ECALNeig","ECALNeig",9,-4.5,4.5));


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


void ECalReconstruction::ProcessEvent(TRawEvent* rawEv)
{
  PadmeVReconstruction::ProcessEvent(rawEv);
  BuildClusters();
}


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
  EvTotE=0;
  //  std::cout<<"Builing clusters"<<std::endl;
  int NClusters = fClusters.size();//ECalBuildClusters(rawEv);
  //  GetHisto("ECALNPart")->Fill(ECALNPart);
  GetHisto("ECALNClus")->Fill(NClusters);
  //  GetHisto("ECALETot") ->Fill(EvTotE);
  
  for(int gg=0;gg<NClusters;gg++){ 
    GetHisto("ECALClE")->Fill(ClE[gg]);
    GetHisto("ECALClNCry")->Fill(ClNCry[gg]);
    GetHisto("ECALClTime")->Fill(ClTime[gg]);
    if(ClNCry[gg]>1 && ClE[gg]>10.) {
      GetHisto("ECALClTimeCut")->Fill(ClTime[gg]);
      //      GetHisto("ECALClPos")->Fill(ClX[gg],ClY[gg]);
    }
    //    std::cout<<gg<<" Ncry "<<ClNCry[gg]<<" "<<" "<<std::endl;
  }	 

  float q1 = 0.;
  float q2 = 0.;
  float q3 = 0.;
  float q4 = 0.;
  float summa = 0.;

  int NHighEgamma = 0;

  for(unsigned int iHit1 =  0; iHit1 < Hits.size(); ++iHit1) {
    //    std::cout<<"gli hit di Venelin "<<Hits.size()<<std::endl;
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

//  Wirtten by M. Raggi 22/11/2018 
Int_t ECalReconstruction::FindSeed(Int_t nele, Int_t * Used, Double_t* Ene) {

  Int_t iMax=-1;
  Double_t fMax=0;

  for(UShort_t i = 0;i<nele;i++){
    //    std::cout<<"N hits "<<nele<<"Ene "<<Ene[i]<<" Ene "<<Used[i]<<std::endl;  
    if (fMax < Ene[i] && Used[i]==0 && Ene[i]>10.) { //ene cut to check
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
  if( abs(SeedRow-CellRow)<=2 && abs(SeedCol-CellCol)<=2) IsNeig=1;
  //  std::cout<<"seedID "<<seedID<<" cellID "<<cellID<<" Is Neig "<<IsNeig<<std::endl;
  return IsNeig;
}

//  Written by M. Raggi 22/11/2018 
void ECalReconstruction::BuildClusters()
{

  std::cout<<"In ECalBuildClusters "<<std::endl;
  vector<TRecoVCluster *> &myClusters  = getClusters();
  for(unsigned int iCl = 0;iCl < myClusters.size();iCl++){
    delete myClusters[iCl];
  }
  myClusters.clear();
  std::cout<<"myClusters is now cleared  "<<std::endl;
  

  
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

  const int NMaxCl=100;
  const int NRows=29;
  const int NCols=29;
  const int NTotCh=NRows*NCols;
  double TTotECALCh[NTotCh][NMaxCl];
  double ETotECALCh[NTotCh][NMaxCl];
  Double_t Time=0;
 
  vector<TRecoVHit *> &Hits  = GetRecoHits();
  int NCry=0;
 
  if(Hits.size()==0){
    //    std::cout<<"No hits !!!!"<<std::endl;
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

  Double_t cTime[Hits.size()]={0.};
  Double_t cEnergy[Hits.size()]={0.};
  Int_t cChID[Hits.size()]={0};
  Int_t cUsed[Hits.size()]={0};
  Int_t cCellUsed[NTotCh]={0};
  for(Int_t mm=0;mm<50;mm++) cCellUsed[mm]=0;
  //fill the vector with hits informations
  for(unsigned int iHit1 =  0; iHit1 < Hits.size(); ++iHit1) {
    cUsed[iHit1]  = {0};
    cTime[iHit1]  = Hits[iHit1]->GetTime();;	
    cEnergy[iHit1]= Hits[iHit1]->GetEnergy();;	
    cChID[iHit1]  = Hits[iHit1]->GetChannelId();
  }  

  Int_t iMax=0;
  Int_t HitUsed=0;
  Int_t clusMatrix[NMaxCl][20]={0};
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
      if( fabs(cTime[iHit1]-SdTime[NSeeds])<15 && cUsed[iHit1]==0 && IsSeedNeig(SdCell[NSeeds],cChID[iHit1])==1){
      //      if( fabs(cTime[iHit1]-SdTime[NSeeds])<15 && cUsed[iHit1]==0){// && IsSeedNeig(SdCell[NSeeds],cChID[iHit1])==1){

	//	Double_t XCl=(60.-cChID[iHit1]/10*30.);
	//	Double_t YCl=(-60.+cChID[iHit1]%10*30.);
	//std::cout<<"is neig "<<cChID[iHit1]<<" "<<cTime[iHit1]<<" "<<cEnergy[iHit1]<<std::endl;
	Int_t XCl=cChID[iHit1]/100;
	Int_t YCl=cChID[iHit1]%100;
	((TH2F *)GetHisto("ECALCellPos"))->Fill(XCl,YCl,cEnergy[iHit1]);
	cUsed[iHit1]=1;
	ClTime[NSeeds]+=cTime[iHit1];
	ClE[NSeeds]+=cEnergy[iHit1];
	ClX[NSeeds]+=XCl*cEnergy[iHit1];
	ClY[NSeeds]+=YCl*cEnergy[iHit1];
	NCry++;
	HitUsed++;
	cCellUsed[cChID[iHit1]]=1;
      }
    }
    ClNCry.push_back(NCry);
    GetHisto("ECALClE")->Fill(ClE[NSeeds]);
    ClTime[NSeeds]=ClTime[NSeeds]/NCry;  //average time of the hit
    ClX[NSeeds]=ClX[NSeeds]/ClE[NSeeds];
    ClY[NSeeds]=ClY[NSeeds]/ClE[NSeeds];
    GetHisto("ECALClTDiff")->Fill(ClTime[NSeeds]-SdTime[NSeeds]);
    //    if(ClNCry[NSeeds]>9) std::cout<<NSeeds<<" Ncry >9 "<<NCry<<" Clseed "<<ClSeed[NSeeds]<<" ClE "<<ClE[NSeeds]<<std::endl;
    NSeeds++;
    if(NCry>1 || ClE[NSeeds]>10.) NGoodClus++;
  }  //end of while loop on seeds

  std::vector<Int_t> tmpHitsInCl;
  for (Int_t iCl=0; iCl<NSeeds; ++iCl){
    tmpHitsInCl.clear();
    TRecoVCluster* myCl = new TRecoVCluster();
    myCl->SetChannelId( SdCell[iCl] );
    myCl->SetEnergy( ClE[iCl]    );
    myCl->SetTime(   ClTime[iCl] );
    myCl->SetPosition(TVector3(ClX[iCl],ClY[iCl],0.));
    myCl->SetSeed(ClSeed[iCl]);
    myCl->SetNHitsInClus(ClNCry[iCl]);
    std::cout<<ClNCry[iCl]<<" Hits in cl. n. "<<iCl<<" = ";
    for (unsigned int j=0; j<ClNCry[iCl]; ++j)
      {
	tmpHitsInCl.push_back(clusMatrix[iCl][j]);
	std::cout<<" "<<clusMatrix[iCl][j];
      }
    std::cout<<endl;
    myCl->SetHitVecInClus(tmpHitsInCl);
    myClusters.push_back(myCl);
    tmpHitsInCl.clear();
    std::cout<<" done .. icl "<<iCl<<" energy = "<<myCl->GetEnergy()<<" "<<getClusters()[iCl]->GetEnergy()<<std::endl;
    std::cout<<" cluster "<<iCl<<" id/e/t/nhit/hits"<<getClusters()[iCl]->GetChannelId()<<"/"<<getClusters()[iCl]->GetEnergy()<<"/"<<getClusters()[iCl]->GetTime()<<"/"<<getClusters()[iCl]->GetNHitsInClus()<<"/ ";
    for (int j=0; j<getClusters()[iCl]->GetNHitsInClus(); ++j)
      {
	std::cout<<" "<<getClusters()[iCl]->GetHitVecInClus()[j];
      }
    std::cout<<endl;
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
