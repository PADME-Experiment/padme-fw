#include "GlobalTimeAnalysis.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"
//#include "TString.hh"
#include <iostream>

/*
void CalibrateTimePVeto  (double *timeOffset);
void CalibrateTimeEVeto  (double *timeOffset);
void CalibrateTimeHEPVeto(double *timeOffset);
void CalibrateTimeSAC    (double *timeOffset);
void CalibrateTimeECal   (double *timeOffset);
*/

GlobalTimeAnalysis::GlobalTimeAnalysis(){
  ;
}

GlobalTimeAnalysis::~GlobalTimeAnalysis(){
  ;
}
Bool_t GlobalTimeAnalysis::Init(PadmeAnalysisEvent *event){
  fName = "GlobalTimeAnalysis";
  evt = event;

  // init to zero time calib constants
  for (Int_t i=0; i<  96; ++i) {ftimeOffsetPVeto[i]  =0.; ftimeOffsetEVeto[i]=0.; }
  for (Int_t i=0; i<  32; ++i)  ftimeOffsetHEPVeto[i]=0.;
  for (Int_t i=0; i<3000; ++i)  ftimeOffsetECal[i]   =0.;
  for (Int_t i=0; i<  50; ++i)  ftimeOffsetSAC[i]    =0.;

  fMC=false;
  if (evt->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) fMC=true;
  if (!fMC)
    {
      ApplyTimeCalibrationForData();
    }
  
  if (fMode < 2) InitHistos();//active on standard analysis and validation mode 
  return true;
}

Bool_t GlobalTimeAnalysis::ApplyTimeCalibrationForData(){
  ApplyTimeCalibPVeto  ();
  ApplyTimeCalibEVeto  ();
  ApplyTimeCalibHEPVeto();
  ApplyTimeCalibSAC    ();
  ApplyTimeCalibECal   ();
  return true;
}

Bool_t GlobalTimeAnalysis::InitHistos(){
  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  //HistoFile->mkdir(fName.Data());
  
  hSvcVal->BookHisto2("ECal_dT_vs_PVeto_50"     , 3000,0,3000,1000,-200, 200);
  hSvcVal->BookHisto2("ECal_dT_vs_PVeto"        , 3000,0,3000,1000,-200, 200);
  hSvcVal->BookHisto2("ECal_dT_vs_SAC_21"       , 3000,0,3000,1000,-200, 200);
  hSvcVal->BookHisto2("HEPVeto_dT_vs_SAC_21"    ,   32,0,  32,1000,-200, 200);
  hSvcVal->BookHisto2("HEPVeto_dT_adj"          ,   32,0,  32,1000,-200, 200);
  hSvcVal->BookHisto2("SAC_dT_vs_PVeto_50"      ,   50,0,  50,1000,-200, 200);
  hSvcVal->BookHisto2("SAC_dT_vs_PVeto"         ,   50,0,  50,1000,-200, 200);
  hSvcVal->BookHisto2("PVeto_dT_vs_SAC_21"      ,   96,0,  96,1000,-200, 200);
  hSvcVal->BookHisto2("SAC_dT_vs_SAC"           ,   50,0,  50,1000,-200, 200);
  hSvcVal->BookHisto2("ECal_dT_vs_ECal"         , 3000,0,3000,1000,-200, 200);
  hSvcVal->BookHisto2("EVeto_dT_vs_ECal"        ,   96,0,  96,1000,-200, 200);
  hSvcVal->BookHisto2("EVeto_dT_vs_PVeto"       ,   96,0,  96,1000,-200, 200);
  hSvcVal->BookHisto2("EVeto_dT_vs_SAC"         ,   96,0,  96,1000,-200, 200);
  hSvcVal->BookHisto2("EVeto_dT_vs_SAC_21"      ,   96,0,  96,1000,-200, 200);
  hSvcVal->BookHisto2("PVeto_dT_adj"            ,   96,0,  96,1000,-200, 200);
  hSvcVal->BookHisto2("EVeto_dT_adj"            ,   96,0,  96,1000,-200, 200);
  hSvcVal->BookHisto2("PVeto_vs_EVeto_dT_ECal"  ,   96,0,  96,  96,   0,  96);
  hSvcVal->BookHisto2("PVeto_vs_EVeto_NodT_ECal",   96,0,  96,  96,   0,  96);

 
  hSvcVal->BookHisto2("PVeto_PH_vs_chId"   ,   96,0,  96, 300, 0, 300); 
  hSvcVal->BookHisto2("EVeto_PH_vs_chId"   ,   96,0,  96, 300, 0, 300);  
  hSvcVal->BookHisto2("HEPVeto_PH_vs_chId" ,   32,0,  32, 300, 0, 300); 

  hSvcVal->BookHisto2("SAC_PH_vs_chId"     ,  50,0,  50,  300, 0, 300); 
  hSvcVal->BookHisto2("ECAL_PH_vs_chId"    ,3000,0,3000,  300, 0, 300); 
  hSvcVal->BookHisto2("ECAL_PH_vs_xy"      ,  30,0,  30,   30, 0,  30);

  return true;
}


#define ABS(x)  ((x) > 0 ? (x):(-x))

Bool_t GlobalTimeAnalysis::Process(){

  if (fMode > 1) return true; //active on standard analysis and validation mode 
  
  HistoSvc* hSvc =  HistoSvc::GetInstance();

////////Time Calibration

//PVeto
  
  for(int ipv = 0;ipv <  evt->PVetoRecoEvent->GetNHits(); ipv++) {
    int    chPV = evt->PVetoRecoEvent->Hit(ipv)->GetChannelId();
    double tPv1 = evt->PVetoRecoEvent->Hit(ipv)->GetTime()-ftimeOffsetPVeto[chPV];
    double  ePv = evt->PVetoRecoEvent->Hit(ipv)->GetEnergy();    
    hSvc->FillHisto2("PVeto_PH_vs_chId",chPV,ePv,1);
    
    if(ePv < 30. && (!fMC) ) continue;
    if(ePv >100. && (!fMC) ) continue;
    
     for(int isac = 0; isac < evt->SACRecoEvent->GetNHits(); isac++){
      int chSAC = evt->SACRecoEvent->Hit(isac)->GetChannelId();
      double tSAC = evt->SACRecoEvent->Hit(isac)->GetTime()-ftimeOffsetSAC[chSAC];
      double eSAC = evt->SACRecoEvent->Hit(isac)->GetEnergy();
      if(eSAC < 5 && (!fMC) ) continue;
      if(chSAC == 21) {
      
      hSvc->FillHisto2("PVeto_dT_vs_SAC_21",chPV,tPv1-tSAC,1);
    }
   }

     for(int ipv1 = 0;ipv1 <  evt->PVetoRecoEvent->GetNHits(); ipv1++) {
      int    chPV1 = evt->PVetoRecoEvent->Hit(ipv1)->GetChannelId();
      double tPv2  = evt->PVetoRecoEvent->Hit(ipv1)->GetTime()-ftimeOffsetPVeto[chPV1];
      double  ePv1 = evt->PVetoRecoEvent->Hit(ipv1)->GetEnergy();
     
    
      if(!(chPV1-chPV == 1 && chPV < 89 ))continue;
      if(ePv1 < 30. && (!fMC) ) continue;
      if(ePv1 >100. && (!fMC) ) continue;
      hSvc->FillHisto2("PVeto_dT_adj",chPV,tPv2-tPv1,1);
    }

  }

//EVeto
  
  for(int ipv = 0;ipv <  evt->EVetoRecoEvent->GetNHits(); ipv++) {
    int chPV = evt->EVetoRecoEvent->Hit(ipv)->GetChannelId();
    double tPv1 = evt->EVetoRecoEvent->Hit(ipv)->GetTime()-ftimeOffsetEVeto[chPV];
    double ePv = evt->EVetoRecoEvent->Hit(ipv)->GetEnergy();
    

    hSvc->FillHisto2("EVeto_PH_vs_chId",chPV,ePv,1);

    if(ePv <  30. && (!fMC) ) continue;
    if(ePv > 100. && (!fMC) ) continue;
   
    
     for(int iecal = 0; iecal < evt->ECalRecoEvent->GetNHits(); iecal++){
      int chECal = evt->ECalRecoEvent->Hit(iecal)->GetChannelId();
      double tECal = evt->ECalRecoEvent->Hit(iecal)->GetTime()-ftimeOffsetECal[chECal];
      double eECal = evt->ECalRecoEvent->Hit(iecal)->GetEnergy();
       
      if(eECal < 50 && (!fMC) ) continue;
      hSvc->FillHisto2("EVeto_dT_vs_ECal",chPV,tPv1-tECal,1);
     }
   
     for(int isac = 0; isac < evt->SACRecoEvent->GetNHits(); isac++){
      int chSAC = evt->SACRecoEvent->Hit(isac)->GetChannelId();
      double tSAC = evt->SACRecoEvent->Hit(isac)->GetTime()-ftimeOffsetSAC[chSAC];
      double eSAC = evt->SACRecoEvent->Hit(isac)->GetEnergy();
       
      if(eSAC < 5 && (!fMC) ) continue;
      hSvc->FillHisto2("EVeto_dT_vs_SAC",chPV,tPv1-tSAC,1);
      if(chSAC==21)hSvc->FillHisto2("EVeto_dT_vs_SAC_21",chPV,tPv1-tSAC,1);
     }
   
     for(int ipveto = 0; ipveto < evt->PVetoRecoEvent->GetNHits(); ipveto++){
      int chPVeto = evt->PVetoRecoEvent->Hit(ipveto)->GetChannelId();
      double tPVeto = evt->PVetoRecoEvent->Hit(ipveto)->GetTime()-ftimeOffsetPVeto[chPVeto];
      double ePVeto = evt->PVetoRecoEvent->Hit(ipveto)->GetEnergy();
       
      if(ePVeto < 30. && (!fMC) ) continue;
      if(ePVeto >100. && (!fMC) ) continue;
      hSvc->FillHisto2("EVeto_dT_vs_PVeto",chPV,tPv1-tPVeto,1);

 /*       
    	 for(int iecal = 0; iecal < evt->ECalRecoEvent->GetNHits(); iecal++){
   	   int checal = evt->ECalRecoEvent->Hit(iecal)->GetChannelId();
      	   double tecal = evt->ECalRecoEvent->Hit(iecal)->GetTime()-timeOffsetECal[checal];
      	   double eecal = evt->ECalRecoEvent->Hit(iecal)->GetEnergy();
       
      	   if(eecal < 50. && MC == 0 ) continue;
           if(fabs(tPVeto-tPv1+1.5)<1.5 && fabs(tecal-tPv1+8) <2){
      	    hSvc->FillHisto2("PVeto_vs_EVeto_dT_ECal",chPV,chPVeto,1);
           }
           else{hSvc->FillHisto2("PVeto_vs_EVeto_NodT_ECal",chPV,chPVeto,1);}

     	}
*/

     }

     for(int ipv1 = 0;ipv1 <  evt->EVetoRecoEvent->GetNHits(); ipv1++) {
      int    chPV1 = evt->EVetoRecoEvent->Hit(ipv1)->GetChannelId();
      double tPv2  = evt->EVetoRecoEvent->Hit(ipv1)->GetTime()-ftimeOffsetEVeto[chPV1];
      double  ePv1 = evt->EVetoRecoEvent->Hit(ipv1)->GetEnergy();
     
    
      if(!(chPV1-chPV == 1 && chPV < 89 ))continue;
      if(ePv1 < 30. && (!fMC) ) continue;
      if(ePv1 >100. && (!fMC) ) continue;
      hSvc->FillHisto2("EVeto_dT_adj",chPV,tPv2-tPv1,1);
    }

  }



//HEPVeto
  
  for(int ipv = 0;ipv <  evt->HEPVetoRecoEvent->GetNHits(); ipv++) {
    int chPV = evt->HEPVetoRecoEvent->Hit(ipv)->GetChannelId();
    double tPv1 = evt->HEPVetoRecoEvent->Hit(ipv)->GetTime()-ftimeOffsetHEPVeto[chPV];
    double ePv = evt->HEPVetoRecoEvent->Hit(ipv)->GetEnergy();
    
    hSvc->FillHisto2("HEPVeto_PH_vs_chId",chPV,ePv,1);

    if(ePv < 30. && (!fMC) ) continue;
    if(ePv >100. && (!fMC) ) continue;
  
     for(int isac = 0; isac < evt->SACRecoEvent->GetNHits(); isac++){
      int chSAC = evt->SACRecoEvent->Hit(isac)->GetChannelId();
      double tSAC = evt->SACRecoEvent->Hit(isac)->GetTime()-ftimeOffsetSAC[chSAC];
      double eSAC = evt->SACRecoEvent->Hit(isac)->GetEnergy();
      if(eSAC < 5 && (!fMC) ) continue;
      if(chSAC == 21) {
      hSvc->FillHisto2("HEPVeto_dT_vs_SAC_21",chPV,tPv1-tSAC,1);
    }
   }

    for(int ipv1 = 0;ipv1 <  evt->HEPVetoRecoEvent->GetNHits(); ipv1++) {
      int    chPV1 = evt->HEPVetoRecoEvent->Hit(ipv1)->GetChannelId();
      double tPv2  = evt->HEPVetoRecoEvent->Hit(ipv1)->GetTime()-ftimeOffsetHEPVeto[chPV1];
      double  ePv1 = evt->HEPVetoRecoEvent->Hit(ipv1)->GetEnergy();
      
      if(!(chPV1-chPV == 1 && chPV < 32 ))continue;
      if(ePv1 < 30. && (!fMC) ) continue;
      if(ePv1 >100. && (!fMC) ) continue;
      hSvc->FillHisto2("HEPVeto_dT_adj",chPV,tPv2-tPv1,1);
    }
 
  }


//ECal
  
  for(int ipv = 0;ipv <  evt->ECalRecoEvent->GetNHits(); ipv++) {
    int chPV = evt->ECalRecoEvent->Hit(ipv)->GetChannelId();
    double tPv1 = evt->ECalRecoEvent->Hit(ipv)->GetTime()-ftimeOffsetECal[chPV];
    double ePv = evt->ECalRecoEvent->Hit(ipv)->GetEnergy();

    hSvc->FillHisto2("ECAL_PH_vs_xy",chPV/100,chPV%100,ePv);
    hSvc->FillHisto2("ECAL_PH_vs_chId",chPV,ePv,1);

    
    if(ePv < 30. && (!fMC) ) continue;
  
     for(int iPVeto = 0; iPVeto < evt->PVetoRecoEvent->GetNHits(); iPVeto++){
      int chPVeto = evt->PVetoRecoEvent->Hit(iPVeto)->GetChannelId();
      double tPVeto = evt->PVetoRecoEvent->Hit(iPVeto)->GetTime()-ftimeOffsetPVeto[chPVeto];
      double ePVeto = evt->PVetoRecoEvent->Hit(iPVeto)->GetEnergy();
     
      if(ePVeto < 30 && (!fMC) ) continue;
      if(ePVeto >100 && (!fMC) ) continue;
      
      hSvc->FillHisto2("ECal_dT_vs_PVeto",chPV,tPv1-tPVeto,1);
    
      if(chPVeto == 50) { 
      hSvc->FillHisto2("ECal_dT_vs_PVeto_50",chPV,tPv1-tPVeto,1);
    }

   }
 

   
     for(int isac = 0; isac < evt->SACRecoEvent->GetNHits(); isac++){
      int chSAC = evt->SACRecoEvent->Hit(isac)->GetChannelId();
      double tSAC = evt->SACRecoEvent->Hit(isac)->GetTime()-ftimeOffsetSAC[chSAC];
      double eSAC = evt->SACRecoEvent->Hit(isac)->GetEnergy();
       
      if(eSAC < 5 && (!fMC) ) continue;
      if(chSAC==21)hSvc->FillHisto2("ECal_dT_vs_SAC_21",chPV,tPv1-tSAC,1);
     }

 
   for(int ipv1 = 0;ipv1 <  evt->ECalRecoEvent->GetNHits(); ipv1++) {
      int chPV1 = evt->ECalRecoEvent->Hit(ipv1)->GetChannelId();
      double tPv2 = evt->ECalRecoEvent->Hit(ipv1)->GetTime()-ftimeOffsetECal[chPV1];
      double ePv1 = evt->ECalRecoEvent->Hit(ipv1)->GetEnergy();  

      if(ePv1 < 50. && (!fMC) ) continue;
      
      int row  = chPV %100 ;
      int row1 = chPV1%100 ;
      int col  = chPV /100 ;
      int col1 = chPV1/100 ;
      
      if(row==row1&&(col==col1-1))hSvc->FillHisto2("ECal_dT_vs_ECal",chPV,tPv1-tPv2,1);
 
   }
  }


//SAC
  
  for(int ipv = 0;ipv <  evt->SACRecoEvent->GetNHits(); ipv++) {
    int chPV = evt->SACRecoEvent->Hit(ipv)->GetChannelId();
    double tPv1 = evt->SACRecoEvent->Hit(ipv)->GetTime()-ftimeOffsetSAC[chPV];
    double ePv = evt->SACRecoEvent->Hit(ipv)->GetEnergy();
   
    hSvc->FillHisto2("SAC_PH_vs_chId",chPV,ePv,1);

    if(ePv < 5. && (!fMC) ) continue;
  
     for(int iPVeto = 0; iPVeto < evt->PVetoRecoEvent->GetNHits(); iPVeto++){
      int chPVeto = evt->PVetoRecoEvent->Hit(iPVeto)->GetChannelId();
      double tPVeto = evt->PVetoRecoEvent->Hit(iPVeto)->GetTime()-ftimeOffsetPVeto[chPVeto];
      double ePVeto = evt->PVetoRecoEvent->Hit(iPVeto)->GetEnergy();
     
      if(ePVeto < 30 && (!fMC) ) continue;
      if(ePVeto >100 && (!fMC) ) continue;
      
      hSvc->FillHisto2("SAC_dT_vs_PVeto",chPV,tPv1-tPVeto,1);
    
      if(chPVeto == 50) {    
      hSvc->FillHisto2("SAC_dT_vs_PVeto_50",chPV,tPv1-tPVeto,1);
    }
   }
  
     

  for(int ipv1 = 0;ipv1 <  evt->SACRecoEvent->GetNHits(); ipv1++) {
    int chPV1 = evt->SACRecoEvent->Hit(ipv1)->GetChannelId();
    double tPv2 = evt->SACRecoEvent->Hit(ipv1)->GetTime()-ftimeOffsetSAC[chPV1];
    double ePv1 = evt->SACRecoEvent->Hit(ipv1)->GetEnergy();
    //if(ePv1 < 5. && (!fMC) ) continue;
    
    if(chPV==0  && chPV1==10)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV==10 && chPV1==20)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV==20 && chPV1==30)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV==30 && chPV1==40)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV==40 && chPV1==41)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV==41 && chPV1==31)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV==31 && chPV1==21)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV==21 && chPV1==11)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV==11 && chPV1==1 )hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV==1  && chPV1==2 )hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV==2  && chPV1==12)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV==12 && chPV1==22)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV==22 && chPV1==32)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV==32 && chPV1==42)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV==42 && chPV1==43)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV==43 && chPV1==33)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV==33 && chPV1==23)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV==23 && chPV1==13)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV1=13 && chPV1== 3)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV== 3 && chPV1== 4)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV== 4 && chPV1==14)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV==14 && chPV1==24)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV==24 && chPV1==34)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
    if(chPV==34 && chPV1==44)hSvc->FillHisto2("SAC_dT_vs_SAC",chPV,tPv2-tPv1,1);
   

  }

 
  }







  return true;
}


void GlobalTimeAnalysis::ApplyTimeCalibPVeto() {
/*
ftimeOffsetPVeto[0]=0 ; 
ftimeOffsetPVeto[1]=-0.185223 ; 
ftimeOffsetPVeto[2]=0.349405 ; 
ftimeOffsetPVeto[3]=0.106338 ; 
ftimeOffsetPVeto[4]=0.0514904 ; 
ftimeOffsetPVeto[5]=-0.268219 ; 
ftimeOffsetPVeto[6]=0.188752 ; 
ftimeOffsetPVeto[7]=0.370645 ; 
ftimeOffsetPVeto[8]=0.260729 ; 
ftimeOffsetPVeto[9]=-0.0171529 ; 
ftimeOffsetPVeto[10]=0.758248 ; 
ftimeOffsetPVeto[11]=0.602042 ; 
ftimeOffsetPVeto[12]=0.332325 ; 
ftimeOffsetPVeto[13]=0.0430155 ; 
ftimeOffsetPVeto[14]=0.450478 ; 
ftimeOffsetPVeto[15]=0.496555 ; 
ftimeOffsetPVeto[16]=0.152301 ; 
ftimeOffsetPVeto[17]=0.0507212 ; 
ftimeOffsetPVeto[18]=0.394068 ; 
ftimeOffsetPVeto[19]=0.273789 ; 
ftimeOffsetPVeto[20]=0.346727 ; 
ftimeOffsetPVeto[21]=0.210783 ; 
ftimeOffsetPVeto[22]=0.110795 ; 
ftimeOffsetPVeto[23]=0.589864 ; 
ftimeOffsetPVeto[24]=0.584523 ; 
ftimeOffsetPVeto[25]=0.339251 ; 
ftimeOffsetPVeto[26]=0.444891 ; 
ftimeOffsetPVeto[27]=0.543123 ; 
ftimeOffsetPVeto[28]=0.411852 ; 
ftimeOffsetPVeto[29]=0.628588 ; 
ftimeOffsetPVeto[30]=0.416585 ; 
ftimeOffsetPVeto[31]=0.378872 ; 
ftimeOffsetPVeto[32]=0.209886 ; 
ftimeOffsetPVeto[33]=0.00894525 ; 
ftimeOffsetPVeto[34]=0.517031 ; 
ftimeOffsetPVeto[35]=0.223812 ; 
ftimeOffsetPVeto[36]=-0.515125 ; 
ftimeOffsetPVeto[37]=0.130935 ; 
ftimeOffsetPVeto[38]=0.212283 ; 
ftimeOffsetPVeto[39]=0.166844 ; 
ftimeOffsetPVeto[40]=-0.104873 ; 
ftimeOffsetPVeto[41]=-0.248102 ; 
ftimeOffsetPVeto[42]=0.0455675 ; 
ftimeOffsetPVeto[43]=-0.00584186 ; 
ftimeOffsetPVeto[44]=-0.253998 ; 
ftimeOffsetPVeto[45]=-0.340922 ; 
ftimeOffsetPVeto[46]=0.175989 ; 
ftimeOffsetPVeto[47]=0.357804 ; 
ftimeOffsetPVeto[48]=-3.4791 ; 
ftimeOffsetPVeto[49]=-3.39888 ; 
ftimeOffsetPVeto[50]=-2.81786 ; 
ftimeOffsetPVeto[51]=-3.26652 ; 
ftimeOffsetPVeto[52]=-3.57222 ; 
ftimeOffsetPVeto[53]=-3.41885 ; 
ftimeOffsetPVeto[54]=-3.37243 ; 
ftimeOffsetPVeto[55]=-3.32543 ; 
ftimeOffsetPVeto[56]=-3.63335 ; 
ftimeOffsetPVeto[57]=-3.56397 ; 
ftimeOffsetPVeto[58]=-3.02615 ; 
ftimeOffsetPVeto[59]=-3.52038 ; 
ftimeOffsetPVeto[60]=-3.60302 ; 
ftimeOffsetPVeto[61]=-3.62492 ; 
ftimeOffsetPVeto[62]=-3.40941 ; 
ftimeOffsetPVeto[63]=-3.39878 ; 
ftimeOffsetPVeto[64]=-4.14744 ; 
ftimeOffsetPVeto[65]=-4.00297 ; 
ftimeOffsetPVeto[66]=-3.55354 ; 
ftimeOffsetPVeto[67]=-3.8833 ; 
ftimeOffsetPVeto[68]=-4.21377 ; 
ftimeOffsetPVeto[69]=-4.16495 ; 
ftimeOffsetPVeto[70]=-4.3218 ; 
ftimeOffsetPVeto[71]=-4.16276 ; 
ftimeOffsetPVeto[72]=-4.40376 ; 
ftimeOffsetPVeto[73]=-4.77954 ; 
ftimeOffsetPVeto[74]=-4.87394 ; 
ftimeOffsetPVeto[75]=-4.85514 ; 
ftimeOffsetPVeto[76]=-5.28272 ; 
ftimeOffsetPVeto[77]=-5.35442 ; 
ftimeOffsetPVeto[78]=-5.21305 ; 
ftimeOffsetPVeto[79]=-5.52315 ; 
ftimeOffsetPVeto[80]=-6.14597 ; 
ftimeOffsetPVeto[81]=-6.18159 ; 
ftimeOffsetPVeto[82]=-5.8238 ; 
ftimeOffsetPVeto[83]=-6.44256 ; 
ftimeOffsetPVeto[84]=-6.30873 ; 
ftimeOffsetPVeto[85]=-6.30873 ; 
ftimeOffsetPVeto[86]=-6.30873 ; 
ftimeOffsetPVeto[87]=-6.39123 ; 
ftimeOffsetPVeto[88]=-6.77991 ; 
ftimeOffsetPVeto[89]=-7.25289 ;
*/ 
}
void GlobalTimeAnalysis::ApplyTimeCalibEVeto() {
/*
ftimeOffsetEVeto[0]=0 ; 
ftimeOffsetEVeto[1]=0.216392 ; 
ftimeOffsetEVeto[2]=0.0129375 ; 
ftimeOffsetEVeto[3]=-0.049033 ; 
ftimeOffsetEVeto[4]=0.209616 ; 
ftimeOffsetEVeto[5]=0.177875 ; 
ftimeOffsetEVeto[6]=0.167179 ; 
ftimeOffsetEVeto[7]=-0.0704188 ; 
ftimeOffsetEVeto[8]=0.0238068 ; 
ftimeOffsetEVeto[9]=0.371396 ; 
ftimeOffsetEVeto[10]=-0.0203685 ; 
ftimeOffsetEVeto[11]=0.329521 ; 
ftimeOffsetEVeto[12]=0.193249 ; 
ftimeOffsetEVeto[13]=0.456476 ; 
ftimeOffsetEVeto[14]=0.308078 ; 
ftimeOffsetEVeto[15]=0.342395 ; 
ftimeOffsetEVeto[16]=-0.26366 ; 
ftimeOffsetEVeto[17]=0.175579 ; 
ftimeOffsetEVeto[18]=0.0468307 ; 
ftimeOffsetEVeto[19]=-0.0845242 ; 
ftimeOffsetEVeto[20]=0.0614764 ; 
ftimeOffsetEVeto[21]=-0.267333 ; 
ftimeOffsetEVeto[22]=-0.405091 ; 
ftimeOffsetEVeto[23]=-0.132629 ; 
ftimeOffsetEVeto[24]=0.106912 ; 
ftimeOffsetEVeto[25]=0.289285 ; 
ftimeOffsetEVeto[26]=0.0174216 ; 
ftimeOffsetEVeto[27]=-0.0769529 ; 
ftimeOffsetEVeto[28]=-0.286678 ; 
ftimeOffsetEVeto[29]=-0.134324 ; 
ftimeOffsetEVeto[30]=-0.121085 ; 
ftimeOffsetEVeto[31]=-0.34199 ; 
ftimeOffsetEVeto[32]=-0.354945 ; 
ftimeOffsetEVeto[33]=-0.0194155 ; 
ftimeOffsetEVeto[34]=-0.402375 ; 
ftimeOffsetEVeto[35]=-0.316218 ; 
ftimeOffsetEVeto[36]=-0.0581592 ; 
ftimeOffsetEVeto[37]=-0.186327 ; 
ftimeOffsetEVeto[38]=-0.330438 ; 
ftimeOffsetEVeto[39]=-0.371171 ; 
ftimeOffsetEVeto[40]=-0.148918 ; 
ftimeOffsetEVeto[41]=-0.39254 ; 
ftimeOffsetEVeto[42]=-0.478176 ; 
ftimeOffsetEVeto[43]=-0.459525 ; 
ftimeOffsetEVeto[44]=-0.452375 ; 
ftimeOffsetEVeto[45]=-0.504376 ; 
ftimeOffsetEVeto[46]=-0.694716 ; 
ftimeOffsetEVeto[47]=-0.752361 ; 
ftimeOffsetEVeto[48]=-3.39393 ; 
ftimeOffsetEVeto[49]=-3.14132 ; 
ftimeOffsetEVeto[50]=-3.37764 ; 
ftimeOffsetEVeto[51]=-3.67831 ; 
ftimeOffsetEVeto[52]=-3.26801 ; 
ftimeOffsetEVeto[53]=-3.66518 ; 
ftimeOffsetEVeto[54]=-3.8967 ; 
ftimeOffsetEVeto[55]=-3.47346 ; 
ftimeOffsetEVeto[56]=-3.27547 ; 
ftimeOffsetEVeto[57]=-3.25364 ; 
ftimeOffsetEVeto[58]=-3.62896 ; 
ftimeOffsetEVeto[59]=-3.18138 ; 
ftimeOffsetEVeto[60]=-3.32639 ; 
ftimeOffsetEVeto[61]=-3.5682 ; 
ftimeOffsetEVeto[62]=-3.60912 ; 
ftimeOffsetEVeto[63]=-3.63186 ; 
ftimeOffsetEVeto[64]=-3.75248 ; 
ftimeOffsetEVeto[65]=-3.66265 ; 
ftimeOffsetEVeto[66]=-4.0871 ; 
ftimeOffsetEVeto[67]=-4.0664 ; 
ftimeOffsetEVeto[68]=-3.62838 ; 
ftimeOffsetEVeto[69]=-3.95912 ; 
ftimeOffsetEVeto[70]=-4.31528 ; 
ftimeOffsetEVeto[71]=-4.10979 ; 
ftimeOffsetEVeto[72]=-3.75264 ; 
ftimeOffsetEVeto[73]=-3.83103 ; 
ftimeOffsetEVeto[74]=-4.03251 ; 
ftimeOffsetEVeto[75]=-3.77272 ; 
ftimeOffsetEVeto[76]=-3.69096 ; 
ftimeOffsetEVeto[77]=-3.5287 ; 
ftimeOffsetEVeto[78]=-3.91749 ; 
ftimeOffsetEVeto[79]=-3.76101 ; 
ftimeOffsetEVeto[80]=-3.76101 ; 
ftimeOffsetEVeto[81]=-3.76101 ; 
ftimeOffsetEVeto[82]=-3.76101 ; 
ftimeOffsetEVeto[83]=-3.76101 ; 
ftimeOffsetEVeto[84]=-3.76101 ; 
ftimeOffsetEVeto[85]=-3.76101 ; 
ftimeOffsetEVeto[86]=-3.76101 ; 
ftimeOffsetEVeto[87]=-3.76101 ; 
ftimeOffsetEVeto[88]=-3.76101 ; 
ftimeOffsetEVeto[89]=-3.76101 ; 
ftimeOffsetEVeto[90]=-3.76101 ; 
ftimeOffsetEVeto[91]=-3.76101 ; 
ftimeOffsetEVeto[92]=-3.76101 ; 
ftimeOffsetEVeto[93]=-3.76101 ; 
ftimeOffsetEVeto[94]=-3.76101 ; 
ftimeOffsetEVeto[95]=-3.76101 ;
*/
}
void GlobalTimeAnalysis::ApplyTimeCalibHEPVeto() {
/*
ftimeOffsetHEPVeto[0]=33.2947 ; 
ftimeOffsetHEPVeto[1]=33.5556 ; 
ftimeOffsetHEPVeto[2]=34.3929 ; 
ftimeOffsetHEPVeto[3]=34.1724 ; 
ftimeOffsetHEPVeto[4]=34.1269 ; 
ftimeOffsetHEPVeto[5]=34.0993 ; 
ftimeOffsetHEPVeto[6]=34.4157 ; 
ftimeOffsetHEPVeto[7]=34.6173 ; 
ftimeOffsetHEPVeto[8]=34.4037 ; 
ftimeOffsetHEPVeto[9]=34.299 ; 
ftimeOffsetHEPVeto[10]=34.4303 ; 
ftimeOffsetHEPVeto[11]=34.7061 ; 
ftimeOffsetHEPVeto[12]=34.3652 ; 
ftimeOffsetHEPVeto[13]=34.5289 ; 
ftimeOffsetHEPVeto[14]=34.5833 ; 
ftimeOffsetHEPVeto[15]=34.6225 ; 
ftimeOffsetHEPVeto[16]=32.4403 ; 
ftimeOffsetHEPVeto[17]=33.1107 ; 
ftimeOffsetHEPVeto[18]=33.2783 ; 
ftimeOffsetHEPVeto[19]=32.3747 ; 
ftimeOffsetHEPVeto[20]=33.5736 ; 
ftimeOffsetHEPVeto[21]=33.5701 ; 
ftimeOffsetHEPVeto[22]=33.4828 ; 
ftimeOffsetHEPVeto[23]=33.6996 ; 
ftimeOffsetHEPVeto[24]=34.1688 ; 
ftimeOffsetHEPVeto[25]=34.1689 ; 
ftimeOffsetHEPVeto[26]=33.861 ; 
ftimeOffsetHEPVeto[27]=33.9058 ; 
ftimeOffsetHEPVeto[28]=34.2488 ; 
ftimeOffsetHEPVeto[29]=34.1752 ; 
ftimeOffsetHEPVeto[30]=33.6696 ; 
ftimeOffsetHEPVeto[31]=34.0452 ; 
*/
}
void GlobalTimeAnalysis::ApplyTimeCalibSAC() {
 ;
}
void GlobalTimeAnalysis::ApplyTimeCalibECal() {
 ;
}





