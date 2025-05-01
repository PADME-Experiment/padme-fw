#include "SPA_acceptance.hh"
#include "HistoSvc.hh"
#include "GeneralInfo.hh"
#include "TLorentzVector.h"
#include <iostream>

static SPA_acceptance ThisAnalyser("SPA_acceptance");

SPA_acceptance::SPA_acceptance(const char *s,int valid, int verb)
  : PadmeVAnalyser(s,valid,verb)
{;}

SPA_acceptance::SPA_acceptance(const char *s):
  PadmeVAnalyser(s)
{;}

Bool_t SPA_acceptance::InitHistos(){

  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  hSvcVal->makeFileDir(GetName());
  
  hSvcVal->BookHisto(this->GetName()+"_ECal_NHits",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_NClusters",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_NClustersAbove10MeV",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_NClustersAbove20MeV",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_NClustersAbove30MeV",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_NClustersAbove40MeV",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_NClustersAbove50MeV",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_NClustersAbove60MeV",100,0.0,100.0);

  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_All",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Signal",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut",1000,-500.0,500.0);
  
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_deltaT",1000,-500.0,500.0);
  
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV",1000,-500.0,500.0);

  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_05ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_05ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_05ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_05ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_05ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_05ns",1000,-500.0,500.0);

  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_1ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_1ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_1ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_1ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_1ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_1ns",1000,-500.0,500.0);

  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_2ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_2ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_2ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_2ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_2ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_2ns",1000,-500.0,500.0);

  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_5ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_5ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_5ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_5ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_5ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_5ns",1000,-500.0,500.0);
  
  hSvcVal->BookHisto2(this->GetName()+"_ECal_MissingMass_ThetaGamma",500,0.0,500,200,0.0,0.2);

  return true;
}

double SPA_acceptance::ComputeTheta(Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma){
 Double_t R = sqrt(PosX*PosX+PosY*PosY+PosZ*PosZ);
 double Px = E_gamma*PosX/R;
 double Py = E_gamma*PosY/R;
 double Pz = E_gamma*PosZ/R;
 TLorentzVector Photon   (Px,Py,Pz,E_gamma);
 return Photon.Theta();
}

double SPA_acceptance::ComputeMissingMass(double BeamEnergy, Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma){
 double BeamPz = sqrt(BeamEnergy*BeamEnergy-0.511*0.511);
 TLorentzVector Positron (0,0,BeamPz,BeamEnergy);
 TLorentzVector Electron (0,0,         0,    0.511);
 Double_t R = sqrt(PosX*PosX+PosY*PosY+PosZ*PosZ);
 double Px = E_gamma*PosX/R;
 double Py = E_gamma*PosY/R;
 double Pz = E_gamma*PosZ/R;
 TLorentzVector Photon   (Px,Py,Pz,E_gamma);
 TLorentzVector Missing  (0,0,0,0); 
 Missing  = Positron + Electron - Photon;
 return    Missing.M2();//*Missing;
}


Bool_t SPA_acceptance::Process(){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  GeneralInfo* fGeneralInfo = GeneralInfo::GetInstance();
  float BeamEnergy=423.5;
  
 if(evt->ECalRecoEvent){
    hSvc->FillHisto(this->GetName()+"_ECal_NHits",evt->ECalRecoEvent->GetNHits());
    
    Int_t NclusECal = evt->ECalRecoCl->GetNElements();
    hSvc->FillHisto(this->GetName()+"_ECal_NClusters",NclusECal);

    //if(evt->MCTruthEvent){

      //if(NclusECal==1){
	TMCVertex* mcVtx;
	TRecoVCluster* cluU=NULL;
	Double_t MissingMassU=0;
	//Int_t NclusECal = evt->ECalRecoCl->GetNElements();

	
	Double_t Z_1_U = 3480;
	
	Int_t n_g10=0;
	Int_t n_g20=0;
	Int_t n_g30=0;
	Int_t n_g40=0;
	Int_t n_g50=0;
	Int_t n_g60=0;

	for(Int_t i=0; i<NclusECal; i++){
	  cluU    = evt->ECalRecoCl->Element(i);
	  if(cluU->GetEnergy()>10.) n_g10+=1;
	  if(cluU->GetEnergy()>20.) n_g20+=1;
	  if(cluU->GetEnergy()>30.) n_g30+=1;
	  if(cluU->GetEnergy()>40.) n_g40+=1;
	  if(cluU->GetEnergy()>50.) n_g50+=1;
	  if(cluU->GetEnergy()>60.) n_g60+=1;
	}

	if(n_g10==1){
	  for(Int_t i=0; i<NclusECal; i++){
	    cluU    = evt->ECalRecoCl->Element(i);
	    if(cluU->GetEnergy()>10.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));	      
	  }
	}
	
	if(n_g20==1){
	  for(Int_t i=0; i<NclusECal; i++){
	    cluU    = evt->ECalRecoCl->Element(i);
	    if(cluU->GetEnergy()>20.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));	      
	  }
	}
	
	if(n_g30==1){
	  for(Int_t i=0; i<NclusECal; i++){
	    cluU    = evt->ECalRecoCl->Element(i);
	    if(cluU->GetEnergy()>30.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));	      
	  }
	}
	
	if(n_g40==1){
	  for(Int_t i=0; i<NclusECal; i++){
	    cluU    = evt->ECalRecoCl->Element(i);
	    if(cluU->GetEnergy()>40.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));	      
	  }
	}
	
	if(n_g50==1){
	  for(Int_t i=0; i<NclusECal; i++){
	    cluU    = evt->ECalRecoCl->Element(i);
	    if(cluU->GetEnergy()>50.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));	      
	  }
	}
	
	if(n_g60==1){
	  for(Int_t i=0; i<NclusECal; i++){
	    cluU    = evt->ECalRecoCl->Element(i);
	    if(cluU->GetEnergy()>60.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));	      
	  }
	}

	//-------------------Delta t---------------------------------------------

    TRecoVCluster* clu2=NULL;
    Int_t intime05=0;
    Int_t intime1=0;
    Int_t intime2=0;
    Int_t intime5=0;
    Float_t deltaT05=0.5;
    Float_t deltaT1=1.;
    Float_t deltaT2=2.;
    Float_t deltaT5=5.;

    Float_t safeE=30.;
    
    cluU=NULL;
       
    for (Int_t i=0; i<NclusECal; ++i){
      intime05=0;
      intime1=0;
      intime2=0;
      intime5=0;
      cluU    = evt->ECalRecoCl->Element(i);
      
      for (Int_t j=0; j<NclusECal; ++j){
	if(j!=i) {
	  clu2 = evt->ECalRecoCl->Element(j);
	  hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_deltaT",cluU->GetTime()-clu2->GetTime());
	  if (clu2->GetEnergy()>safeE){
	    if(fabs(cluU->GetTime()-clu2->GetTime())<deltaT05) intime05+=1;
	    if(fabs(cluU->GetTime()-clu2->GetTime())<deltaT1) intime1+=1;		  
	    if(fabs(cluU->GetTime()-clu2->GetTime())<deltaT2) intime2+=1;	
	    if(fabs(cluU->GetTime()-clu2->GetTime())<deltaT5) intime5+=1;
	  }
	}
      }

      if(intime5==0){
	if(cluU->GetEnergy()>10.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_5ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>20.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_5ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>30.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_5ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>40.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_5ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>50.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_5ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>60.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_5ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	
      }
      
      if(intime2==0){
	if(cluU->GetEnergy()>10.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_2ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>20.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_2ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>30.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_2ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>40.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_2ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>50.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_2ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>60.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_2ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));	
      }
      
      if(intime1==0){
	if(cluU->GetEnergy()>10.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_1ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>20.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_1ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>30.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_1ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>40.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_1ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>50.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_1ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>60.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_1ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));	
      }
      
      if(intime05==0){
	if(cluU->GetEnergy()>10.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_05ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>20.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_05ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>30.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_05ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>40.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_05ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>50.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_05ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>60.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_05ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));	
      }
      
      
    }
    
    //---------------------------Ecut--------------------------------
    
    Int_t NClusAbove10=0;
    Int_t NClusAbove20=0;
    Int_t NClusAbove30=0;
    Int_t NClusAbove40=0;
    Int_t NClusAbove50=0;
    Int_t NClusAbove60=0;
   
    for (Int_t i=0; i<NclusECal; ++i){
      cluU    = evt->ECalRecoCl->Element(i);
      if (cluU->GetEnergy()>10) NClusAbove10+=1;
      if (cluU->GetEnergy()>20) NClusAbove20+=1;
      if (cluU->GetEnergy()>30) NClusAbove30+=1;
      if (cluU->GetEnergy()>40) NClusAbove40+=1;
      if (cluU->GetEnergy()>50) NClusAbove50+=1;
      if (cluU->GetEnergy()>60) NClusAbove60+=1;
    }
    
    hSvc->FillHisto(this->GetName()+"_ECal_NClustersAbove10MeV",NClusAbove10);
    hSvc->FillHisto(this->GetName()+"_ECal_NClustersAbove20MeV",NClusAbove20);
    hSvc->FillHisto(this->GetName()+"_ECal_NClustersAbove30MeV",NClusAbove30);
    hSvc->FillHisto(this->GetName()+"_ECal_NClustersAbove40MeV",NClusAbove40);
    hSvc->FillHisto(this->GetName()+"_ECal_NClustersAbove50MeV",NClusAbove50);
    hSvc->FillHisto(this->GetName()+"_ECal_NClustersAbove60MeV",NClusAbove60);
    
	//MissingMassU = ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy()));
	//	hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_All",MissingMassU);
	
	// if(cluU->GetEnergy()>10.) {

	//   hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV",MissingMassU);
	// }
	
	// if(cluU->GetEnergy()>20.) {
	//   hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV",MissingMassU);
	// }
	
	// if(cluU->GetEnergy()>30.){
	//   hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV",MissingMassU);

	// }

	// if(cluU->GetEnergy()>40.) {
	//   hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV",MissingMassU);
	// }
	
	// if(cluU->GetEnergy()>50.){
	//   hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV",MissingMassU);
	// }
	// if(cluU->GetEnergy()>60.) {
	//   hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV",MissingMassU);
	// }
	

	// if(evt->MCTruthEvent->GetNVertices()==1) {
	//   mcVtx = evt->MCTruthEvent->Vertex(0);
	//   TMCParticle* mcOPart = mcVtx->ParticleOut(0);
	//   TMCParticle* mcOPart2 = mcVtx->ParticleOut(1);
	//   Double_t DeltaE;
	//   if(mcOPart->GetPDGCode() != 22) DeltaE=mcOPart2->GetEnergy()-cluU->GetEnergy();	  
	//   else DeltaE=mcOPart->GetEnergy()-cluU->GetEnergy(); 
	//   if (DeltaE<0.1) {
	//     hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Signal",MissingMassU);
	//     hSvc->FillHisto2(this->GetName()+"_ECal_MissingMass_ThetaGamma",MissingMassU,ComputeTheta((cluU->GetPosition().X()),(cluU->GetPosition().Y()),(cluU->GetPosition().Z()),(cluU->GetEnergy())));
	//     if(cluU->GetEnergy()>60.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut",MissingMassU);
	//   }

	// }
	
	//   }
      
      
    // }
 }
  
  fResult = true;
  return fResult;
}
