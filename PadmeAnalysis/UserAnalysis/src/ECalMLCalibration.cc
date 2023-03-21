#include "ECalMLCalibration.hh"
#include "HistoSvc.hh"
#include <iostream>
#include <string>

static ECalMLCalibration ThisAnalyser("ECalMLCalibration");

ECalMLCalibration::ECalMLCalibration(const char *s,int valid, int verb)
  : PadmeVAnalyser(s,valid,verb)
{;}

ECalMLCalibration::ECalMLCalibration(const char *s):
  PadmeVAnalyser(s)
{;}

Bool_t ECalMLCalibration::InitHistos(){

  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  hSvcVal->makeFileDir(GetName());
  //hSvcVal->BookHisto(this->GetName()+"_PVeto_NHits",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_NHits",100,0.0,100.0);
  hSvcVal->BookHisto2(this->GetName()+"_NECal_vs_NECalML",10,0.0,10.0,10,0.0,10.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_NHits",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_NHits",100,0.0,100.0);

  for(int i = 0;i<32;i++){
    for(int j=0;j<32;j++) {
      hSvcVal->BookHisto(this->GetName()+"_ECalML_calib_"+std::to_string(100*i+j),100,0.0,100.0);
    }
  }
  
  return true;
}


Bool_t ECalMLCalibration::Process(){
  HistoSvc* hSvc =  HistoSvc::GetInstance();

  // if(evt->PVetoRecoEvent) 
  //   hSvc->FillHisto(this->GetName()+"_PVeto_NHits",evt->PVetoRecoEvent->GetNHits());
  if(evt->ECalMLRecoEvent) 
    hSvc->FillHisto(this->GetName()+"_ECalML_NHits",evt->ECalMLRecoEvent->GetNHits());
  
  TRecoVHit* ECalhit=NULL;
  TRecoVHit* ECalMLhit=NULL;
  
  Int_t ECalHits;
  Int_t ECalMLHits;

  Int_t Channel;
  Int_t Energy;
  
  Int_t ChannelML;
  Int_t EnergyML;

  int ECal[32][32];
  int ECalML[32][32];
  if(!evt->ECalRecoEvent || !evt->ECalMLRecoEvent) return true;

  if(evt->ECalRecoEvent && evt->ECalMLRecoEvent){

    for(int i=0;i<32;i++){
      for(int j=0;j<32;j++){
	ECal[i][j]=0;
	ECalML[i][j]=0;
      }
    }
    
    ECalHits=evt->ECalRecoEvent->GetNHits();
    ECalMLHits=evt->ECalMLRecoEvent->GetNHits();

    
    for (int i=0;i<ECalHits;i++){
      ECalhit=evt->ECalRecoEvent->Hit(i);
      Channel=ECalhit->GetChannelId();
      //Energy=ECalhit->GetEnergy();
      int I = Channel%100;
      int J = Channel/100;
      
      if(I >= 0 && I < 32 && J >= 0 && J < 32) {
	ECal[I][J]+=1;
      }

      
      // for(int i=0;i<32;i++){
      // 	for(int j=0;j<32;j++){
      // 	  if (i == Channel%100 && j==Channel/100)
      // 	    ECal[i][j]+=1;
      // 	  else
      // 	    ECal[i][j]+=0;
      // 	}
      // }
    }
    
    for (int i=0;i<ECalMLHits;i++){
    
      ECalMLhit=evt->ECalMLRecoEvent->Hit(i);
      ChannelML=ECalMLhit->GetChannelId();
      //EnergyML=ECalMLhit->GetEnergy();
      int I = ChannelML%100;
      int J = ChannelML/100;
      
      if(I >= 0 && I < 32 && J >= 0 && J < 32) {
	ECalML[I][J]+=1;
      }
      // for(int i=0;i<32;i++){
      // 	for(int j=0;j<32;j++){
      // 	  if (i==ChannelML%100 && j==ChannelML/100) ECalML[i][j]+=1;
      // 	  else ECalML[i][j]+=0;
      // 	}
      // }
    }

   
    for(int i=0;i<32;i++){
      for(int j=0;j<32;j++){

	hSvc->FillHisto2(this->GetName()+"_NECal_vs_NECalML",ECal[i][j], ECalML[i][j]);

	if (ECal[i][j]==1 && ECalML[i][j]==1){
	  
	  /*
	  for (int i=0;i<ECalHits;i++){
	    ECalhit=evt->ECalRecoEvent->Hit(i);
	    Channel=ECalhit->GetChannelId();
	  }
	  
	  for (int i=0;i<ECalMLHits;i++){    
	    ECalMLhit=evt->ECalMLRecoEvent->Hit(i);
	    ChannelML=ECalMLhit->GetChannelId();
	  }
	  
	  if (i==ChannelML%100 && j==ChannelML/100) {
	    Energy=ECalhit->GetEnergy();
	    EnergyML=ECalMLhit->GetEnergy();
	    //And we fill a histogram maybe with the two energies...
	  }
	  */
	}
	
      }
   
    }
    
  }
  
  fResult = true;
  return fResult;
}
