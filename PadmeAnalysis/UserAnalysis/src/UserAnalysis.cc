#include "UserAnalysis.hh"
#include "HistoSvc.hh"
#include <iostream>
#include "TLorentzVector.h"


UserAnalysis::UserAnalysis(){
  ;
}

UserAnalysis::~UserAnalysis(){
  ;
}
Bool_t UserAnalysis::Init(PadmeAnalysisEvent *event){
  evt = event;
  InitHistos();
  return true;
}

Bool_t UserAnalysis::InitHistos(){
  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  //  hSvcVal->BookHisto(hname, nBin, min, max);
  
  char name[64];

  hSvcVal->BookHisto("UserAnalysis", 100, 0, 1000);
  hSvcVal->BookHisto("PVetoSAC", 2000,-100, 100);
  hSvcVal->BookHisto("PVetoPulseAll", 300,-50, 250);

  
  hSvcVal->BookHisto("PVetoPulse", 300,-50, 250);
  hSvcVal->BookHisto("PVetoTimeDiff", 2000,-100, 100);
  hSvcVal->BookHisto("PVetoTimeDiffAdjacent", 2000,-100, 100);

  for(int i = 0;i<96;i++) {
    sprintf(name,"PVetoSACdT_%d",i);
    hSvcVal->BookHisto(name, 2000,-100, 100);
  }

  hSvcVal->BookHisto2("PVetoPos_vs_SACE", 100,0.0,100,300,0.0, 300);

  hSvcVal->BookHisto("PVetoECal", 2000,-100, 100);

  

  for(int i = 0;i<96;i++) {
    sprintf(name,"PVetoECaldT_%d",i);
    hSvcVal->BookHisto(name, 2000,-100, 100);
  }

  

  hSvcVal->BookHisto2("PVetoPos_vs_ECalE", 100,0.0,100,400,0.0, 400);

  //ECal histos

  hSvcVal->BookHisto("ECalClEnergy",600,0.0,600.0);
  hSvcVal->BookHisto("ECal2ClEnergy",1000,0.0,1000.0);
  hSvcVal->BookHisto("ECalClCldT",200,-100.0,100.0);
  hSvcVal->BookHisto("ECalClCldT_Ecut",200,-100.0,100.0);

  hSvcVal->BookHisto("ECal2ClEnergyTimeCut",1000,0.0,1000.0);
  hSvcVal->BookHisto("ECal2ClEnergyTimeDistCut",1000,0.0,1000.0);
  hSvcVal->BookHisto("ECal2ClEnergyTimeDist2Cut",1000,0.0,1000.0);
  hSvcVal->BookHisto("ECal_ClCl_dist",1000,0.0,1000.0);
  
  hSvcVal->BookHisto("ECal2ClEnergyTimeCut2",1000,0.0,1000.0);
  hSvcVal->BookHisto("ECal2ClEnergyTimeDistCut2",1000,0.0,1000.0);
  hSvcVal->BookHisto("ECal2ClEnergyTimeDist2Cut2",1000,0.0,1000.0);
  hSvcVal->BookHisto("ECal_ClCl_dist2",1000,0.0,1000.0);

  hSvcVal->BookHisto("ECal_COG_X",300,-300.0,300.0);
  hSvcVal->BookHisto("ECal_COG_Y",300,-300.0,300.0);
  hSvcVal->BookHisto("ECal_COG",300,0.0,300.0);

  hSvcVal->BookHisto("ECal2ClEnergyTimeDistCut2Cog",1000,0.0,1000.0);
  hSvcVal->BookHisto("ECal2ClEnergyTimeDistCut2Cog2",1000,0.0,1000.0);

  hSvcVal->BookHisto2("ECal_COG_vs_m2",300,0.0,300.0,1000,0.0,1000.0 );
  hSvcVal->BookHisto2("ECal_COG_vs_m",300,0.0,300.0,200,0.0,50.0 );

  hSvcVal->BookHisto("ee_e",1000,0.0,1000.0);
  hSvcVal->BookHisto("ee_m2",1000,0.0,1000.0);
  hSvcVal->BookHisto("ee_m",200,0.0,50.0);
  

  


  return true;
}

void CalibrateTimePVeto(double *timeOffset) {
 timeOffset[0]=30.897200;
 timeOffset[1]=31.035610;
 timeOffset[2]=31.174020;
 timeOffset[3]=31.312430;
 timeOffset[4]=31.450840;
 timeOffset[5]=31.589250;
 timeOffset[6]=31.727660;
 timeOffset[7]=31.866070;
 timeOffset[8]=32.004480;
 timeOffset[9]=32.142890;
 timeOffset[10]=32.281300;
 timeOffset[11]=32.419710;
 timeOffset[12]=32.558120;
 timeOffset[13]=32.696530;
 timeOffset[14]=32.834940;
 timeOffset[15]=32.973350;
 timeOffset[16]=32.890820;
 timeOffset[17]=33.211898;
 timeOffset[18]=33.289182;
 timeOffset[19]=33.341203;
 timeOffset[20]=33.200115;
 timeOffset[21]=33.374934;
 timeOffset[22]=33.556819;
 timeOffset[23]=34.080630;
 timeOffset[24]=34.092354;
 timeOffset[25]=34.340731;
 timeOffset[26]=34.554038;
 timeOffset[27]=34.790529;
 timeOffset[28]=34.952806;
 timeOffset[29]=34.755125;
 timeOffset[30]=35.022980;
 timeOffset[31]=35.627999;
 timeOffset[32]=35.237881;
 timeOffset[33]=35.488854;
 timeOffset[34]=35.949793;
 timeOffset[35]=35.702472;
 timeOffset[36]=36.053466;
 timeOffset[37]=36.381224;
 timeOffset[38]=36.274407;
 timeOffset[39]=36.322261;
 timeOffset[40]=36.515453;
 timeOffset[41]=36.620632;
 timeOffset[42]=37.164659;
 timeOffset[43]=37.262615;
 timeOffset[44]=37.164278;
 timeOffset[45]=37.357804;
 timeOffset[46]=37.568731;
 timeOffset[47]=37.653306;
 timeOffset[48]=37.391866;
 timeOffset[49]=37.402487;
 timeOffset[50]=37.518689;
 timeOffset[51]=38.012656;
 timeOffset[52]=37.698736;
 timeOffset[53]=37.623118;
 timeOffset[54]=37.750863;
 timeOffset[55]=38.151170;
 timeOffset[56]=38.318374;
 timeOffset[57]=38.662572;
 timeOffset[58]=38.361165;
 timeOffset[59]=39.032582;
 timeOffset[60]=38.645761;
 timeOffset[61]=39.325416;
 timeOffset[62]=39.289367;
 timeOffset[63]=39.868047;
 timeOffset[64]=39.735753;
 timeOffset[65]=40.005376;
 timeOffset[66]=40.303381;
 timeOffset[67]=40.511240;
 timeOffset[68]=40.402011;
 timeOffset[69]=40.200387;
 timeOffset[70]=40.008423;
 timeOffset[71]=39.851931;
 timeOffset[72]=40.862720;
 timeOffset[73]=41.071373;
 timeOffset[74]=41.139540;
 timeOffset[75]=41.277950;
 timeOffset[76]=41.416360;
 timeOffset[77]=41.554770;
 timeOffset[78]=41.693180;
 timeOffset[79]=41.831590;
 timeOffset[80]=41.970000;
 timeOffset[81]=42.108410;
 timeOffset[82]=42.246820;
 timeOffset[83]=42.385230;
 timeOffset[84]=42.523640;
 timeOffset[85]=42.662050;
 timeOffset[86]=42.800460;
 timeOffset[87]=42.938870;
 timeOffset[88]=43.077280;
 timeOffset[89]=43.215690;
 timeOffset[90]=0.000000;
 timeOffset[91]=0.000000;
 timeOffset[92]=0.000000;
 timeOffset[93]=0.000000;
 timeOffset[94]=0.000000;
 timeOffset[95]=0.000000;

}

#define ABS(x)  ((x) > 0 ? (x):(-x))

Bool_t UserAnalysis::Process(){
  static double timeOffsetPVeto[96];
  CalibrateTimePVeto(timeOffsetPVeto);
  
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  //  std::cout << evt->PVetoRecoEvent->GetNHits() << std::endl;
  hSvc->FillHisto("UserAnalysis",  evt->PVetoRecoEvent->GetNHits() ,1);
  char name[64];

  for(int ipv = 0;ipv <  evt->PVetoRecoEvent->GetNHits(); ipv++) {
    double tPv = evt->PVetoRecoEvent->Hit(ipv)->GetTime();
    int chPV = evt->PVetoRecoEvent->Hit(ipv)->GetChannelId();

    //Correct the PVeto timing:
    //tPv = tPv - (30.9219 + 0.137262*chPV);
    tPv = tPv - timeOffsetPVeto[chPV];
    evt->PVetoRecoEvent->Hit(ipv)->SetTime(tPv);
  }
  
  /*
  for(int ipv = 0;ipv <  evt->PVetoRecoEvent->GetNHits(); ipv++) {
    double tPv1 = evt->PVetoRecoEvent->Hit(ipv)->GetTime();
    int chPV = evt->PVetoRecoEvent->Hit(ipv)->GetChannelId();
    double ePv = evt->PVetoRecoEvent->Hit(ipv)->GetEnergy();
    if(ePv < 10.) continue;
    if(ePv > 50.) continue;
   for(int ipv2 = ipv+1;ipv2 <  evt->PVetoRecoEvent->GetNHits(); ipv2++) {
      double tPv2 = evt->PVetoRecoEvent->Hit(ipv2)->GetTime();
      int chPV2 = evt->PVetoRecoEvent->Hit(ipv2)->GetChannelId();
      double ePv2 = evt->PVetoRecoEvent->Hit(ipv2)->GetEnergy();
      if(ePv2 < 10.) continue;
      if(ePv2 > 50.) continue;
      hSvc->FillHisto("PVetoTimeDiff",tPv1-tPv2,1);
      
      if( ABS (chPV - chPV2 ) == 1 ) {
	hSvc->FillHisto("PVetoTimeDiffAdjacent",tPv1-tPv2,1);
      }
    }
  }
  
  */
  for(int ipv = 0;ipv <  evt->PVetoRecoEvent->GetNHits(); ipv++) {
    int chPV = evt->PVetoRecoEvent->Hit(ipv)->GetChannelId();
    double tPv = evt->PVetoRecoEvent->Hit(ipv)->GetTime();
    
    //Correct the PVeto timing:
    //    tPv = tPv - (30.9219 + 0.137262*chPV);
    // tPv = tPv - timeOffsetPVeto[chPV];
    // evt->PVetoRecoEvent->Hit(ipv)->SetTime(tPv);

    double ePv = evt->PVetoRecoEvent->Hit(ipv)->GetEnergy();
    hSvc->FillHisto("PVetoPulseAll",ePv,1);
    if(ePv < 10.) continue;
    if(ePv > 50.) continue;

    // if(chPV < 10) continue;
    // if(chPV > 70) continue;


    for(int ical = 0;ical <  evt->ECalRecoCl->GetNElements(); ical++) {
      double eECal =  evt->ECalRecoCl->Element(ical)->GetEnergy();
      double tECal =  evt->ECalRecoCl->Element(ical)->GetTime();
      if(eECal < 50.) continue;

      sprintf(name,"PVetoECaldT_%d",chPV);
      hSvc->FillHisto(name, tPv - tECal,1);
      hSvc->FillHisto("PVetoECal", tPv - tECal,1);

      if(tPv - tECal > -26 && tPv - tECal < -22.) {
	hSvc->FillHisto2("PVetoPos_vs_ECalE",chPV,eECal,1);
      }
    }

    continue;


    
    for(int isac = 0; isac < evt->SACRecoEvent->GetNHits(); isac++){
      double tSAC = evt->SACRecoEvent->Hit(isac)->GetTime();
      int chSAC = evt->SACRecoEvent->Hit(isac)->GetChannelId();
      double eSAC = evt->SACRecoEvent->Hit(isac)->GetEnergy();
      if(eSAC < 100) continue;
      //      std::cout << chSAC << "  SAC energy: " << eSAC << std::endl; 
      if(chSAC == 21) {
	sprintf(name,"PVetoSACdT_%d",chPV);
	hSvc->FillHisto(name, tPv - tSAC,1);
	hSvc->FillHisto("PVetoSAC", tPv - tSAC,1);
	//std::cout << "SAC energy: " << eSAC << std::endl; 

	if(  tPv - tSAC > -1. && tPv - tSAC < 1. ) {
	  hSvc->FillHisto("PVetoPulse",ePv,1);
	  hSvc->FillHisto2("PVetoPos_vs_SACE",chPV,eSAC,1);
	}
      }
      
    }
    
  }

  //  return true;

  //Calo-Calo checks
  for(int ical = 0;ical <  evt->ECalRecoCl->GetNElements(); ical++) {
    double eECal =  evt->ECalRecoCl->Element(ical)->GetEnergy();
    double tECal =  evt->ECalRecoCl->Element(ical)->GetTime();
    TVector3 pos1 =  evt->ECalRecoCl->Element(ical)->GetPosition();
    hSvc->FillHisto("ECalClEnergy",eECal,1);
    if(eECal < 20.) continue;
    for(int ical2 = ical + 1;ical2 <  evt->ECalRecoCl->GetNElements(); ical2++) {
      double eECal2 =  evt->ECalRecoCl->Element(ical2)->GetEnergy();
      double tECal2 =  evt->ECalRecoCl->Element(ical2)->GetTime();
      TVector3 pos2 =  evt->ECalRecoCl->Element(ical2)->GetPosition();
      hSvc->FillHisto("ECalClCldT",tECal - tECal2,1);
      if(eECal2 < 20.) continue;
      double etot = eECal + eECal2;
      hSvc->FillHisto("ECal2ClEnergy",eECal + eECal2,1);
      hSvc->FillHisto("ECalClCldT_Ecut",tECal - tECal2,1);
      TVector3 dist = pos2 - pos1;

      double cog_x = (pos1.X()*eECal + pos2.X()*eECal2)/( etot  );
      double cog_y = (pos1.Y()*eECal + pos2.Y()*eECal2)/etot;

      double cog = sqrt(cog_x*cog_x + cog_y*cog_y);

      TVector3 posTarget(0,0,-1030+100);
      
      TVector3 Dir1 = pos1 - posTarget;
      TVector3 Dir2 = pos2 - posTarget;

      TLorentzVector g1(eECal*Dir1.X()/Dir1.Mag(),eECal*Dir1.Y()/Dir1.Mag(),eECal*Dir1.Z()/Dir1.Mag(),eECal);
      TLorentzVector g2(eECal2*Dir2.X()/Dir2.Mag(),eECal2*Dir2.Y()/Dir2.Mag(),eECal2*Dir2.Z()/Dir2.Mag(),eECal2);

      TLorentzVector ee = g1+g2;
      

      if(tECal - tECal2 > -2.  && tECal - tECal2 < 2.) {
	hSvc->FillHisto("ECal2ClEnergyTimeCut",eECal + eECal2,1);
    
	hSvc->FillHisto("ECal_ClCl_dist",dist.Mag(),1);
	if(dist.Mag() > 300.) {
	  hSvc->FillHisto("ECal2ClEnergyTimeDistCut",eECal + eECal2,1);
	}
	if(dist.Mag() > 200.) {
	  hSvc->FillHisto("ECal2ClEnergyTimeDist2Cut",eECal + eECal2,1);
	}
		
      }

      if(tECal - tECal2 > -1.  && tECal - tECal2 < 1.) {
	hSvc->FillHisto("ECal2ClEnergyTimeCut2",eECal + eECal2,1);
	
	hSvc->FillHisto("ECal_ClCl_dist2",dist.Mag(),1);
	if(dist.Mag() > 300.) {
	  hSvc->FillHisto("ECal2ClEnergyTimeDistCut2",eECal + eECal2,1);
	  hSvc->FillHisto("ECal_COG_X",cog_x);
	  hSvc->FillHisto("ECal_COG_Y",cog_y);
	  hSvc->FillHisto("ECal_COG",cog);

	  hSvc->FillHisto2("ECal_COG_vs_m2",cog,ee.Mag2());
	  if(ee.Mag2()>0) {
	    hSvc->FillHisto2("ECal_COG_vs_m",cog,ee.Mag());
	  }

	  if (cog < 50) {
	    hSvc->FillHisto("ECal2ClEnergyTimeDistCut2Cog",eECal + eECal2,1);

	  }
	  if (cog < 40) {
	    hSvc->FillHisto("ECal2ClEnergyTimeDistCut2Cog2",eECal + eECal2,1);
	    hSvc->FillHisto("ee_e",ee.E(),1);
	    hSvc->FillHisto("ee_m2",ee.Mag2(),1);
	    if(ee.Mag2()>0) {
	      hSvc->FillHisto("ee_m",ee.Mag(),1);
	    }
	    
	    
	  }

	  



	}

	if(dist.Mag() > 200.) {
	  hSvc->FillHisto("ECal2ClEnergyTimeDist2Cut2",eECal + eECal2,1);
	}

      }
         
    }
  }

  return true;
}


