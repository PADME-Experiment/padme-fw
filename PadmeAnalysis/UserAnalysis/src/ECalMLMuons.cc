#include "ECalMLMuons.hh"
#include "HistoSvc.hh"
#include "TH1D.h"
#include <iostream>

static ECalMLMuons ThisAnalyser("ECalMLMuons");

ECalMLMuons::ECalMLMuons(const char *s,int valid, int verb)
  : PadmeVAnalyser(s,valid,verb)
{;}

ECalMLMuons::ECalMLMuons(const char *s):
  PadmeVAnalyser(s)
{;}

Bool_t ECalMLMuons::InitHistos(){

  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  hSvcVal->makeFileDir(GetName());
  hSvcVal->BookHisto(this->GetName()+"_ECalML_NHits",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_NHits",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_Muons_Energy",1000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_Muons_Energy",1000,0.0,1000.0);
  hSvcVal->BookHisto2(this->GetName()+"ECal_Map",32,0.0,31.0,32,0.0,31.0);
   hSvcVal->BookHisto2(this->GetName()+"ECal_Map_AllHits",32,0.0,31.0,32,0.0,31.0);
  hSvcVal->BookHisto2(this->GetName()+"ECal_Map_FoundHits",32,0.0,31.0,32,0.0,31.0);
  
  for (int i = 0; i < 32; i++) {
    for (int j = 0; j < 32; j++) {
      const std::string mlHistogramName = this->GetName() + "_ECalML_muons_energy" + std::to_string(100 * i + j);
      const std::string histogramName = this->GetName() + "_ECal_muons_energy" + std::to_string(100 * i + j);
      
      hSvcVal->BookHisto(mlHistogramName, 150, 0.0, 300.0);
      hSvcVal->BookHisto(histogramName, 100, 0.0, 200.0);
      
     
    }
  }
  
  return true;
}


Bool_t ECalMLMuons::Process(){
  HistoSvc* hSvc =  HistoSvc::GetInstance();

  //if(evt->ECalMLRecoEvent) 
  // hSvc->FillHisto(this->GetName()+"_ECalML_NHits",evt->ECalMLRecoEvent->GetNHits());
  TRecoEvent* fRecoMLEvent = NULL;
  TRecoEvent* fRecoEvent = NULL;
  TRecoVHit* ECalhit=NULL;
  TRecoVHit* ECalMLhit=NULL;
  Int_t Channel;
  Int_t ChannelML;
  Int_t ECalHits;
  Int_t ECalMLHits;
  Float_t Energy;
  Float_t EnergyML;
  TRecoVHit* ECal[32][32];
  TRecoVHit* ECalML[32][32];
  Int_t ECalMLAllHits[29][29];
  Int_t ECalMLFoundHits[29][29];
  Int_t ECalMLMap[29][29];
  
  for(int i=0;i<32;i++){
    for(int j=0;j<32;j++){
      ECalML[i][j]=NULL;
      ECal[i][j]=NULL;
    }
  }

  
  if (evt->ECalMLRecoEvent) 
    {
      fRecoMLEvent = evt->RecoEvent;
      if (fRecoMLEvent->GetTriggerMaskBit(TRECOEVENT_TRIGMASKBIT_COSMICS))
	{
	  hSvc->FillHisto(this->GetName()+"_ECalML_NHits",evt->ECalMLRecoEvent->GetNHits());
	  
	  ECalMLHits=evt->ECalMLRecoEvent->GetNHits();
	  for (int i=0;i<ECalMLHits;i++){
    
	    ECalMLhit=evt->ECalMLRecoEvent->Hit(i);
	    ChannelML=ECalMLhit->GetChannelId();
	    int I = ChannelML/100;
	    int J = ChannelML%100;
      
	    if(I >= 0 && I < 32 && J >= 0 && J < 32) {
	      if(ECalMLhit->GetEnergy() > 50) {
		ECalML[I][J] = ECalMLhit;
		//hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",I,J);
	      }
	    }
	     
	  }

	  
	  for(int i=0;i<32;i++){
	    for(int j=1;j<31;j++){
	      if ((ECalMLhit = ECalML[i][j])  !=   NULL ){

		//for (int t=0;t<ECalMLHits;t++){
		//		  ECalMLhit=evt->ECalMLRecoEvent->Hit(t);
		ChannelML=ECalMLhit->GetChannelId();
		
		if (i==ChannelML/100 && j==ChannelML%100 && j>0 && j < 31 ) {

		  if (ECalML[i][j+1] != NULL  && ECalML[i][j-1] != NULL ){
		    EnergyML=ECalMLhit->GetEnergy();
		    hSvc->FillHisto(this->GetName()+"_ECalML_muons_energy"+std::to_string(100*i+j),EnergyML);
		    hSvc->FillHisto(this->GetName()+"_ECalML_Muons_Energy",EnergyML);
		  }
		    
		  //		  }
		 
		} else {
		  std::cout << "===== ERROR=====: INDEX MISMATCH !!!" << std::endl;
		}

		
	      }
	      
	    }
	  }

	   if (ECalML[1][10]!= NULL && ECalML[1][11]!= NULL && ECalML[1][12]!= NULL && ECalML[1][16]!= NULL && ECalML[1][17]!= NULL && ECalML[1][18]!= NULL){
		    for (int p=13; p<16;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",1,p);
		      if (ECalML[1][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",1,p);}
				       //else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",1,p] = 0;}
		    }
		    
		  }

		  if (ECalML[2][8]!= NULL && ECalML[2][9]!= NULL && ECalML[2][10]!= NULL && ECalML[2][18]!= NULL && ECalML[2][19]!= NULL && ECalML[2][20]!= NULL){
		    for (int p=11; p<18;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",2,p);
		      if (ECalML[2][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",2,p);}
				       // else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",2][p] = 0;}
		    }
		    
		  }
		 
		  if (ECalML[3][6]!= NULL && ECalML[3][7]!= NULL && ECalML[3][8]!= NULL && ECalML[3][20]!= NULL && ECalML[3][21]!= NULL && ECalML[3][22]!= NULL){
		    for (int p=9; p<20;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",3,p);
		      if (ECalML[3][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",3,p);}
				       //else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",3][p] = 0;}
		    }
		    
		  }

		  if (ECalML[4][5]!= NULL && ECalML[4][6]!= NULL && ECalML[4][7]!= NULL && ECalML[4][21]!= NULL && ECalML[4][22]!= NULL && ECalML[4][23]!= NULL){
		    for (int p=8; p<21;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",4,p);
		      if (ECalML[4][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",4,p);}
				       ////else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",4][p] = 0;}
		    }
		    
		  }



		  if (ECalML[5][4]!= NULL && ECalML[5][5]!= NULL && ECalML[5][6]!= NULL && ECalML[5][22]!= NULL && ECalML[5][23]!= NULL && ECalML[5][24]!= NULL){
		    for (int p=7; p<22;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",5,p);
		      if (ECalML[5][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",5,p);}
				       //else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",5][p] = 0;}
		    }
		    
		  }

		  
		  if (ECalML[6][4]!= NULL && ECalML[6][5]!= NULL && ECalML[6][6]!= NULL && ECalML[6][22]!= NULL && ECalML[6][23]!= NULL && ECalML[6][24]!= NULL){
		    for (int p=7; p<22;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",6,p);
		      if (ECalML[6][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",6,p);}
				       //else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",6][p] = 0;}
		    }
		    
		  }

		  if (ECalML[7][3]!= NULL && ECalML[7][4]!= NULL && ECalML[7][5]!= NULL && ECalML[7][23]!= NULL && ECalML[7][24]!= NULL && ECalML[7][25]!= NULL){
		    for (int p=6; p<23;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",7,p);
		      if (ECalML[7][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",7,p);}
				       // else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",7][p] = 0;}
		    }
		    
		  }
		  
		  if (ECalML[8][3]!= NULL && ECalML[8][4]!= NULL && ECalML[8][5]!= NULL && ECalML[8][23]!= NULL && ECalML[8][24]!= NULL && ECalML[8][25]!= NULL){
		    for (int p=6; p<23;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",8,p);
		      if (ECalML[8][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",8,p);}
				       // else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",8][p] = 0;}
		    }
		    
		  }

		  if (ECalML[9][2]!= NULL && ECalML[9][3]!= NULL && ECalML[9][4]!= NULL && ECalML[9][24]!= NULL && ECalML[9][25]!= NULL && ECalML[9][26]!= NULL){
		    for (int p=5; p<24;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",9,p);
		      if (ECalML[9][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",9,p);}
				       // else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",9][p] = 0;}
		    }
		    
		  }


		  if (ECalML[10][2]!= NULL && ECalML[10][3]!= NULL && ECalML[10][4]!= NULL && ECalML[10][24]!= NULL && ECalML[10][25]!= NULL && ECalML[10][26]!= NULL){
		    for (int p=5; p<24;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",10,p);
		      if (ECalML[10][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",10,p);}
				       //else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",10][p] = 0;}
		    }
		    
		  }



		  if (ECalML[11][2]!= NULL && ECalML[11][3]!= NULL && ECalML[11][4]!= NULL && ECalML[11][24]!= NULL && ECalML[11][25]!= NULL && ECalML[11][26]!= NULL){
		    for (int p=5; p<24;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",11,p);
		      if (ECalML[11][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",11,p);}
				       // else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",11][p] = 0;}
		    }
		    
		  }



		  if (ECalML[12][1]!= NULL && ECalML[12][2]!= NULL && ECalML[12][3]!= NULL && ECalML[12][25]!= NULL && ECalML[12][26]!= NULL && ECalML[12][27]!= NULL){
		    for (int p=5; p<25;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",12,p);
		      if (ECalML[12][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",12,p);}
				       //else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",12][p] = 0;}
		    }
		    
		  }
 
		  if (ECalML[13][1]!= NULL && ECalML[13][2]!= NULL && ECalML[13][3]!= NULL && ECalML[13][25]!= NULL && ECalML[13][26]!= NULL && ECalML[13][27]!= NULL){
		    for (int p=5; p<25;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",13,p);
		      if (ECalML[13][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",13,p);}
		      //else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",13][p] = 0;}
		    }
		    
		  }

		  if (ECalML[14][1]!= NULL && ECalML[14][2]!= NULL && ECalML[14][3]!= NULL && ECalML[14][25]!= NULL && ECalML[14][26]!= NULL && ECalML[14][27]!= NULL){
		    for (int p=5; p<25;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",14,p);
		      if (ECalML[14][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",14,p);}
				       //else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",14][p] = 0;}
		    }
		    
		  }

		  if (ECalML[15][1]!= NULL && ECalML[15][2]!= NULL && ECalML[15][3]!= NULL && ECalML[15][25]!= NULL && ECalML[15][26]!= NULL && ECalML[15][27]!= NULL){
		    for (int p=5; p<25;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",15,p);
		      if (ECalML[15][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",15,p);}
				       // else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",15][p] = 0;}
		    }
		    
		  }

		  if (ECalML[16][1]!= NULL && ECalML[16][2]!= NULL && ECalML[16][3]!= NULL && ECalML[16][25]!= NULL && ECalML[16][26]!= NULL && ECalML[16][27]!= NULL){
		    for (int p=5; p<25;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",16,p);
		      if (ECalML[16][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",16,p);}
		      //  else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",16][p] = 0;}
		    }
		    
		  }

		  if (ECalML[17][2]!= NULL && ECalML[17][3]!= NULL && ECalML[17][4]!= NULL && ECalML[17][24]!= NULL && ECalML[17][25]!= NULL && ECalML[17][26]!= NULL){
		    for (int p=5; p<24;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",17,p);
		      if (ECalML[17][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",17,p);}
		      // else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",17][p] = 0;}
		    }
		    
		  }


		  if (ECalML[18][2]!= NULL && ECalML[18][3]!= NULL && ECalML[18][24]!= NULL && ECalML[18][25]!= NULL && ECalML[18][26]!= NULL){
		    for (int p=5; p<24;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",18,p);
		      if (ECalML[18][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",18,p);}
		      //   else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",18][p] = 0;}
		    }
		    
		  }



		  if (ECalML[19][2]!= NULL && ECalML[19][3]!= NULL && ECalML[19][4]!= NULL && ECalML[19][24]!= NULL && ECalML[19][25]!= NULL && ECalML[19][26]!= NULL){
		    for (int p=6; p<24;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",19,p);
		      if (ECalML[19][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",19,p);}
		      //else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",19][p] = 0;}
		    }
		    
		  }
		  if (ECalML[20][3]!= NULL && ECalML[20][4]!= NULL && ECalML[20][5]!= NULL && ECalML[20][23]!= NULL && ECalML[20][24]!= NULL && ECalML[20][25]!= NULL){
		    for (int p=6; p<23;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",20,p);
		      if (ECalML[20][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",20,p) ;}
				       //else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",20][p] += 0;}
		    }
		    
		  }
		  
		  if (ECalML[21][3]!= NULL && ECalML[21][4]!= NULL && ECalML[21][5]!= NULL && ECalML[21][23]!= NULL && ECalML[21][24]!= NULL && ECalML[21][25]!= NULL){
		    for (int p=6; p<23;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",21,p);
		      if (ECalML[21][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",21,p);}
				       // else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",21][p] += 0;}
		    }
		    
		  }

		    if (ECalML[22][4]!= NULL && ECalML[22][22]!= NULL && ECalML[22][6]!= NULL && ECalML[22][22]!= NULL && ECalML[22][23]!= NULL && ECalML[22][24]!= NULL){
		      for (int p=7; p<22;p++){
			hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",22,p);
			if (ECalML[22][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",22,p);}
				       // else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",22][p] += 0;}
		    }
		    
		  }

		  
		  if (ECalML[23][4]!= NULL && ECalML[23][5]!= NULL && ECalML[23][6]!= NULL && ECalML[23][22]!= NULL && ECalML[23][23]!= NULL && ECalML[23][24]!= NULL){
		    for (int p=7; p<22;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",23,p);
		      if (ECalML[23][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",23,p);}
				       // else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",23][p] += 0;}
		    }
		    
		  }


		 
		  

		  if (ECalML[24][5]!= NULL && ECalML[24][6]!= NULL && ECalML[24][7]!= NULL && ECalML[24][21]!= NULL && ECalML[24][22]!= NULL && ECalML[24][23]!= NULL){
		    for (int p=8; p<21;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",24,p);
		      if (ECalML[24][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",24,p);}
				       // else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",24][p] += 0;}
		    }
		    
		  }
		  

		  if (ECalML[25][6]!= NULL && ECalML[25][7]!= NULL && ECalML[25][8]!= NULL && ECalML[25][20]!= NULL && ECalML[25][21]!= NULL && ECalML[25][22]!= NULL){
		    for (int p=9; p<20;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",25,p);
				       if (ECalML[25][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",25,p);}
				       // else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",25][p] += 0;}
		    }
		    
		  }


		  if (ECalML[26][8]!= NULL && ECalML[26][9]!= NULL && ECalML[26][10]!= NULL && ECalML[26][18]!= NULL && ECalML[26][19]!= NULL && ECalML[26][20]!= NULL){
		    for (int p=11; p<18;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",26,p);
		      if (ECalML[26][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",26,p);}
				       //  else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",26][p] += 0;}
		    }
		    
		  }

		  if (ECalML[27][10]!= NULL && ECalML[27][11]!= NULL && ECalML[27][12]!= NULL && ECalML[27][16]!= NULL && ECalML[27][17]!= NULL && ECalML[27][18]!= NULL){
		    for (int p=13; p<16;p++){
		      hSvc->FillHisto2(this->GetName()+"ECal_Map_AllHits",27,p);
		      if (ECalML[27][p] != NULL) {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",27,p);}
				       // else {hSvc->FillHisto2(this->GetName()+"ECal_Map_FoundHits",27][p] += 0;}
		    }
		    
		  }
        

	}    
    }
  
  if (evt->ECalRecoEvent) 
    {
      fRecoEvent = evt->RecoEvent;
      if (fRecoEvent->GetTriggerMaskBit(TRECOEVENT_TRIGMASKBIT_COSMICS))
	{
	  hSvc->FillHisto(this->GetName()+"_ECal_NHits",evt->ECalRecoEvent->GetNHits());
	  
	  ECalHits=evt->ECalRecoEvent->GetNHits();
	  for (int i=0;i<ECalHits;i++){
    
	    ECalhit=evt->ECalRecoEvent->Hit(i);
	    Channel=ECalhit->GetChannelId();
	    int I = Channel/100;
	    int J = Channel%100;
      
	    if(I >= 0 && I < 32 && J >= 0 && J < 32) {
	      if(ECalhit->GetEnergy() > 10) {
		ECal[I][J] = ECalhit;
	      }
	    }
	     
	  }

	  
	  for(int i=0;i<32;i++){
	    for(int j=1;j<31;j++){
	      if ((ECalhit = ECal[i][j])  !=   NULL ){

		//for (int t=0;t<ECalHits;t++){
		//		  ECalhit=evt->ECalRecoEvent->Hit(t);
		Channel=ECalhit->GetChannelId();
		
		if (i==Channel/100 && j==Channel%100 && j>0 && j < 31 ) {

		  if (ECal[i][j+1] != NULL  && ECal[i][j-1] != NULL ){
		    Energy=ECalhit->GetEnergy();
		    hSvc->FillHisto(this->GetName()+"_ECal_muons_energy"+std::to_string(100*i+j),Energy);
		    hSvc->FillHisto(this->GetName()+"_ECal_Muons_Energy",Energy);
		    
		  }

		    

		    
		  //		  }
		} else {
		  std::cout << "===== ERROR=====: INDEX MISMATCH !!!" << std::endl;
		}
	      }
	    }
	  }
	}
    }
  
 
  //std::cout<< "Proba" << std::endl;
  fResult = true;
  return fResult;
}


