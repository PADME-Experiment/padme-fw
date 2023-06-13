#include "ECalMLMuons.hh"
#include "HistoSvc.hh"
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
  for(int i = 0;i<32;i++){
    for(int j=0;j<32;j++) {
      hSvcVal->BookHisto(this->GetName()+"_ECalML_muons_energy"+std::to_string(100*i+j),1000,0.0,1000.0);
    }
  }
  
  
  return true;
}


Bool_t ECalMLMuons::Process(){
  HistoSvc* hSvc =  HistoSvc::GetInstance();

  //if(evt->ECalMLRecoEvent) 
    // hSvc->FillHisto(this->GetName()+"_ECalML_NHits",evt->ECalMLRecoEvent->GetNHits());
  TRecoEvent* fRecoEvent = NULL;
  TRecoVHit* ECalMLhit=NULL;
  Int_t ChannelML;
  Int_t ECalMLHits;
  Float_t EnergyML;
  TRecoVHit* ECalML[32][32];
  
  for(int i=0;i<32;i++){
      for(int j=0;j<32;j++){
	ECalML[i][j]=NULL;
      }
  }

  
  if (evt->ECalMLRecoEvent) 
    {
      fRecoEvent = evt->RecoEvent;
      if (fRecoEvent->GetTriggerMaskBit(TRECOEVENT_TRIGMASKBIT_COSMICS))
	{
	  hSvc->FillHisto(this->GetName()+"_ECalML_NHits",evt->ECalMLRecoEvent->GetNHits());
	  
	  ECalMLHits=evt->ECalMLRecoEvent->GetNHits();
	  for (int i=0;i<ECalMLHits;i++){
    
	    ECalMLhit=evt->ECalMLRecoEvent->Hit(i);
	    ChannelML=ECalMLhit->GetChannelId();
	    int I = ChannelML%100;
	    int J = ChannelML/100;
      
	    if(I >= 0 && I < 32 && J >= 0 && J < 32) {
	      if(ECalMLhit->GetEnergy() > 20) {
		ECalML[I][J] = ECalMLhit;
	      }
	    }
	     
	  }
	  for(int i=0;i<32;i++){
	    for(int j=1;j<31;j++){
	      if ((ECalMLhit = ECalML[i][j])  !=   NULL ){

		//for (int t=0;t<ECalMLHits;t++){
		//		  ECalMLhit=evt->ECalMLRecoEvent->Hit(t);
		ChannelML=ECalMLhit->GetChannelId();
		
		if (i==ChannelML%100 && j==ChannelML/100 && j>0 && j < 31 ) {

		  if (ECalML[i][j+1] != NULL  && ECalML[i][j-1] != NULL ){
		    EnergyML=ECalMLhit->GetEnergy();
		    hSvc->FillHisto(this->GetName()+"_ECalML_muons_energy"+std::to_string(100*i+j),EnergyML);
		  }

		    

		    
		    //		  }
		}
	      }
	    }
	  }
	}
    }
  
  
  
  fResult = true;
  return fResult;
}
