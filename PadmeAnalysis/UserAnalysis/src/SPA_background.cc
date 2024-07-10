#include "SPA_background.hh"
#include "HistoSvc.hh"
#include "GeneralInfo.hh"
#include "TLorentzVector.h"
#include <iostream>

static SPA_background ThisAnalyser("SPA_background");

SPA_background::SPA_background(const char *s,int valid, int verb)
  : PadmeVAnalyser(s,valid,verb)
{;}

SPA_background::SPA_background(const char *s):
  PadmeVAnalyser(s)
{;}

Bool_t SPA_background::InitHistos(){

  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  hSvcVal->makeFileDir(GetName());
  
  //Number of hits in each detector
  hSvcVal->BookHisto(this->GetName()+"_ECal_NHits",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_NHits",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_PVeto_NHits",1000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_EVeto_NHits",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_HEPVeto_NHits",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_SAC_NHits",100,0.0,100.0);

  //Number of clusters in each detector
  hSvcVal->BookHisto(this->GetName()+"_ECal_NClusters",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_NClusters",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_PVeto_NClusters",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_EVeto_NClusters",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_SAC_NClusters",100,0.0,100.0);

  
  hSvcVal->BookHisto(this->GetName()+"_ECal_2Clusters_Energysum",750,0.0,750.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2gamma_Zwhen0Mmiss",1000,-4000.0,4000.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_2Clusters_COG",100,-50.0,50.0,100,-50.0,50.0);
  
  hSvcVal->BookHisto(this->GetName()+"_ECal_2gamma_Z",1000,-4000.0,4000.0);
   //--------------------------------------------------MissingMass---------------------------------------------------------------------------
  //ECal
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_All",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_After2gamma",1000,-500.0,500.0);

  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_oneCluster",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_oneCluster_GGcut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_Energy_oneCluster",500,0.0,500.0);  
  hSvcVal->BookHisto(this->GetName()+"_ECal_Theta_oneCluster",100,0.0,0.1);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_Energy_Mmiss",500,0.0,500.0,1000,-500.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_Energy_MissingTheta",500,0.0,500,200,0.0,0.2);

  
  hSvcVal->BookHisto(this->GetName()+"_ECal_Theta_MissingCluster",1000,0.0,1.0);

  
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_After2gamma3gamma",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_After2gamma3gammaBrems",1000,-500.0,500.0);

  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_2gamma_1stgamma",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_2gamma_2ndgamma",1000,-500.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_MissingMass_2gamma",1000,-500.0,500.0,1000,-500.0,500.0);

  //ECalML
  hSvcVal->BookHisto(this->GetName()+"_ECalML_MissingMass_All",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_MissingMass_After2gamma",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_MissingMass_After2gamma3gamma",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_MissingMass_After2gamma3gammaBremsstrahlung",1000,-500.0,500.0);


  //---------------------PVeto---------------------------------
  //Time difference
  hSvcVal->BookHisto(this->GetName()+"_ECalML_PVeto_TimeDiff",800,-100.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_PVeto_TimeDiff_single",800,-100.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_PVeto_TimeDiff",800,-100.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_PVeto_TimeDiff_single",800,-100.0,100.0);

  //PVeto coordinates
  hSvcVal->BookHisto(this->GetName()+"_PVeto_z",20000,-1000.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_PVeto_x",10000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_PVeto_y",10000,0.0,1000.0);

  //PVeto Energy vs position
  //hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_EnergyvsX",500,0.0,500.0,100,0.0,100.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_EnergyvsZ",500,0.0,500.0,20000,-1000.0,1000.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_EnergyvsZ_inTime",500,0.0,500.0,20000,-1000.0,1000.0);

  //Energy vs Energy
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_PVeto_EnergyvsEnergy",500,0.0,500.0,500,0.0,500.0);
    
  

  //---------------------EVeto---------------------------------

  
  
  return true;
}


double SPA_background::ComputeInvariantMass(Double_t E1, Double_t E2, Double_t PosX1, Double_t PosX2, Double_t PosY1, Double_t PosY2)
{
  Double_t M=0;
  Double_t Z  = 3534;
  Double_t R = sqrt( (PosX1-PosX2)*(PosX1-PosX2) + (PosY1-PosY2)*(PosY1-PosY2) ); 
  M = sqrt((E1*E2*R*R)/(Z*Z)); 
  return M;
}

double SPA_background::ComputeRadius(Double_t PosX1, Double_t PosY1)
{
  Double_t R = sqrt( PosX1*PosX1 + PosY1*PosY1 );  
  return R;
}

double SPA_background::ComputeCOG(Double_t E_gamma1, Double_t E_gamma2, Double_t Pos1, Double_t Pos2){
  Double_t ETot=0;
  Double_t COG=0;
  
  ETot=E_gamma1+E_gamma2;
  COG= E_gamma1*Pos1+E_gamma2*Pos2;

  COG=COG/ETot;
  return COG;

}

double SPA_background::ComputeMissingMass(double BeamEnergy, Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma){
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

double SPA_background::ComputeZfor0MissingMass(double BeamEnergy, Double_t PosX1, Double_t PosX2, Double_t PosY1, Double_t PosY2, Double_t E1, Double_t E2){
  Double_t Z=0;
  Double_t R= sqrt((PosX1-PosX2)*(PosX1-PosX2)+(PosY1-PosY2)*(PosY1-PosY2)); 
  Z = -sqrt(E1*E2*R*R)/sqrt(2*0.5111*BeamEnergy);  
  return Z;
}

double SPA_background::ComputeDistance(Double_t PosX1, Double_t PosY1, Double_t PosX2, Double_t PosY2)
{
  Double_t D = sqrt(( PosX1-PosX2)*(PosX1-PosX2) + (PosY1-PosY2)*(PosY1-PosY2) );  
  return D;
}

double SPA_background::ComputeTheta(Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma){
 Double_t R = sqrt(PosX*PosX+PosY*PosY+PosZ*PosZ);
 double Px = E_gamma*PosX/R;
 double Py = E_gamma*PosY/R;
 double Pz = E_gamma*PosZ/R;
 TLorentzVector Photon   (Px,Py,Pz,E_gamma);
 return Photon.Theta();
}

double SPA_background::ComputePhi(Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma){
 Double_t R = sqrt(PosX*PosX+PosY*PosY+PosZ*PosZ);
 double Px = E_gamma*PosX/R;
 double Py = E_gamma*PosY/R;
 double Pz = E_gamma*PosZ/R;
 TLorentzVector Photon   (Px,Py,Pz,E_gamma);
 return Photon.Phi();
}

Bool_t SPA_background::IsGG(TRecoVCluster* clu, TRecoVCluster* clu2){

  
  GeneralInfo* fGeneralInfo = GeneralInfo::GetInstance();

  Double_t time1 = clu->GetTime();
  Double_t time2 = clu2->GetTime();
  
  Double_t clu1Energy=clu->GetEnergy();
  Double_t clu2Energy=clu2->GetEnergy();
  TVector3 posECal=clu->GetPosition();
  TVector3 posECal2=clu2->GetPosition();
  Double_t InvMass=ComputeInvariantMass(clu1Energy,clu2Energy,posECal.X(), posECal2.X(),posECal.Y(), posECal2.Y());
  
  Double_t Theta1=ComputeTheta((clu->GetPosition().X()),(clu->GetPosition().Y()),(clu->GetPosition().Z()),(clu->GetEnergy()));
  Double_t Phi1=ComputePhi((clu->GetPosition().X()),(clu->GetPosition().Y()),(clu->GetPosition().Z()),(clu->GetEnergy()));
  Double_t Theta2=ComputeTheta((clu2->GetPosition().X()),(clu2->GetPosition().Y()),(clu2->GetPosition().Z()),(clu2->GetEnergy()));
  Double_t Phi2=ComputePhi((clu2->GetPosition().X()),(clu2->GetPosition().Y()),(clu2->GetPosition().Z()),(clu2->GetEnergy()));

  Double_t RadiusClu=ComputeRadius(clu->GetPosition().X(),clu->GetPosition().Y() );
  Double_t RadiusClu2=ComputeRadius(clu2->GetPosition().X(),clu2->GetPosition().Y() );

  Double_t fSafeEnergyFactor = 0.7;
  Double_t fSafeSpaceMargin = 3.15;
  
  //Limit only intime clusters
  if((abs(time1-time2))<4.3){
    //Limit the distance between the clusters
    if(abs((ComputeDistance((clu->GetPosition().X()),(clu->GetPosition().Y()),(clu2->GetPosition().X()),(clu2->GetPosition().Y()))))>60){
      //Limit the minimum and maximum energy
      if((clu->GetEnergy()<(fGeneralInfo->GetEnergyMax()/fSafeEnergyFactor))&&(clu2->GetEnergy()<(fGeneralInfo->GetEnergyMax()/fSafeEnergyFactor))&&(clu->GetEnergy()>(fGeneralInfo->GetEnergyMin()*fSafeEnergyFactor))&&(clu2->GetEnergy()>(fGeneralInfo->GetEnergyMin()*fSafeEnergyFactor))){
	//Limit the minimum and maximum radius
	if((RadiusClu>(fGeneralInfo->GetRadiusMin()-fSafeSpaceMargin))&&(RadiusClu2>(fGeneralInfo->GetRadiusMin()-fSafeSpaceMargin))&&(RadiusClu<(fGeneralInfo->GetRadiusMax()))&&(RadiusClu2<(fGeneralInfo->GetRadiusMax()))){
	   //Limit the sum of the polar angles
	  if((Theta1+Theta2)>0.12&&(Theta1+Theta2)<0.165){
	    //Limit the difference in the azimuthal angles
	    if(abs(Phi2-Phi1)>2.8&&abs(Phi2-Phi1)<3.4){
	      //Limit invariant mass
	      if((abs(InvMass))>17. && (abs(InvMass))<23.) {
		return true;
	      }//end of invariant mass limit
	      else {return false;}
	    }//end of phi limit
	    else {return false;}
	  }//end of theta limit
	  else {return false;}
	}//end of minimum and maximum radius limit
	else {return false;}
      }//end of minimum and maximum energy limit
      else {return false;}
    }//end of distance limit
    else {return false;} 
  }//end of time limit
 else {return false;}
}

Bool_t SPA_background::IsGGG(TRecoVCluster* clu, TRecoVCluster* clu2, TRecoVCluster* clu3){
  return false;
}

Bool_t SPA_background::IsBrems(TRecoVCluster* clu, TRecoVCluster* clu2){

  Double_t time1 = clu->GetTime();
  Double_t time2 = clu2->GetTime();
  
  if((abs(time1-time2))<15){
    return true;
  }
  else {
    return false;
  }
}

Bool_t SPA_background::Process(){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
    
  GeneralInfo* fGeneralInfo = GeneralInfo::GetInstance();
  
  //float BeamEnergy=fGeneralInfo->GetBeamEnergy();
  float BeamEnergy=423.5;
    
  int nGG = 0;
  int nGGG = 0;
  int nBrems = 0;
  
  //Begin 2 and 3 gamma selection for ECal
  if(evt->ECalRecoEvent){
    hSvc->FillHisto(this->GetName()+"_ECal_NHits",evt->ECalRecoEvent->GetNHits());
    
    Int_t NclusECal = evt->ECalRecoCl->GetNElements();
    hSvc->FillHisto(this->GetName()+"_ECal_NClusters",NclusECal);

    TRecoVCluster* clu=NULL;
    TRecoVCluster* clu2=NULL;
    TRecoVCluster* clu3=NULL;
    TRecoVCluster* cluP=NULL;
    TRecoVCluster* cluP2=NULL;
    TRecoVCluster* cluPgood=NULL;

    Double_t MissingMass=0;
    Double_t MissingMass2=0;
    Double_t COGX=0;
    Double_t COGY=0;
    Double_t Z=0;

    Double_t deltat=0;
    Double_t Z_1 = 3480;

    
    
    //Loop over the clusters for the first time
    if(NclusECal==1){
      clu    = evt->ECalRecoCl->Element(0);
      MissingMass = ComputeMissingMass(BeamEnergy,(clu->GetPosition().X()),(clu->GetPosition().Y()),Z_1,(clu->GetEnergy()));
      hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_oneCluster",MissingMass);
      hSvc->FillHisto(this->GetName()+"_ECal_Energy_oneCluster",clu->GetEnergy());
      hSvc->FillHisto2(this->GetName()+"_ECal_Energy_Mmiss",clu->GetEnergy(),MissingMass);
      hSvc->FillHisto(this->GetName()+"_ECal_Theta_oneCluster",ComputeTheta((clu->GetPosition().X()),(clu->GetPosition().Y()),Z_1,(clu->GetEnergy())));
      hSvc->FillHisto2(this->GetName()+"_ECal_Energy_MissingTheta",(clu->GetEnergy()),(ComputeTheta((clu->GetPosition().X()),(clu->GetPosition().Y()),Z_1,(clu->GetEnergy()))));
	
      hSvc->FillHisto(this->GetName()+"_ECal_Theta_MissingCluster",0.14-(ComputeTheta((clu->GetPosition().X()),(clu->GetPosition().Y()),Z_1,(clu->GetEnergy()))));

      if ((clu->GetEnergy())>300&&(clu->GetEnergy())<380){
	if((0.14-(ComputeTheta((clu->GetPosition().X()),(clu->GetPosition().Y()),Z_1,(clu->GetEnergy()))))>0.111&&(0.14-(ComputeTheta((clu->GetPosition().X()),(clu->GetPosition().Y()),Z_1,(clu->GetEnergy()))))<0.120){
	  nGG+=1;
	}
      }
      if(nGG==0) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_oneCluster_GGcut",MissingMass);
    }
    
    for (Int_t i=0; i<NclusECal; ++i){


      MissingMass=0;
      MissingMass2=0;
      COGX=0;
      COGY=0;
      Z=0;
      
      
      clu    = evt->ECalRecoCl->Element(i);
      
      Double_t Z_1 = 3480;
      MissingMass = ComputeMissingMass(BeamEnergy,(clu->GetPosition().X()),(clu->GetPosition().Y()),Z_1,(clu->GetEnergy()));
      hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_All",MissingMass);
     
      for (Int_t j=0; j<NclusECal; ++j){

     	if(j!=i) {
	  clu2 = evt->ECalRecoCl->Element(j);
	}
	
	if(clu&&clu2) {//Existence of 2 clusters
	  if(IsGG(clu,clu2)) {
	    hSvc->FillHisto(this->GetName()+"_ECal_2Clusters_Energysum",(clu->GetEnergy()+clu2->GetEnergy()));
	    COGX=ComputeCOG(clu->GetEnergy(),clu2->GetEnergy(),clu->GetPosition().X(),clu2->GetPosition().X());
	    COGY=ComputeCOG(clu->GetEnergy(),clu2->GetEnergy(),clu->GetPosition().Y(),clu2->GetPosition().Y());
	    hSvc->FillHisto2(this->GetName()+"_ECal_2Clusters_COG",COGX,COGY);
	    Z=ComputeZfor0MissingMass(BeamEnergy,(clu->GetPosition().X()),(clu2->GetPosition().X()),(clu->GetPosition().Y()),(clu2->GetPosition().Y()),(clu->GetEnergy()),(clu2->GetEnergy()));
	    
	    hSvc->FillHisto(this->GetName()+"_ECal_2gamma_Zwhen0Mmiss",Z);
	    hSvc->FillHisto(this->GetName()+"_ECal_2gamma_Z",(clu->GetPosition().Z()));
	    hSvc->FillHisto(this->GetName()+"_ECal_2gamma_Z",(clu2->GetPosition().Z()));
	    MissingMass2 = ComputeMissingMass(BeamEnergy,(clu2->GetPosition().X()),(clu2->GetPosition().Y()),Z_1,(clu2->GetEnergy()));
	    hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_2gamma_1stgamma",MissingMass);
	    hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_2gamma_2ndgamma",MissingMass2);
	    hSvc->FillHisto2(this->GetName()+"_ECal_MissingMass_2gamma",MissingMass,MissingMass2);
	    nGG++;
	  }
	  
   
	}//End of existence of 2 clusters

	//Third loop over clusters for GGG
	for(Int_t k=0; k<NclusECal; ++k){
	    
	    if(k!=i&&k!=j) {
	      clu3 = evt->ECalRecoCl->Element(k);
	    }
	    
	    if(clu&&clu2&&clu3) {
	      if(IsGGG(clu,clu2,clu3)) nGGG++;
	    }
	    
	}//End of third loop over clusters
      }//End of second loop over clusters

    // if(nGG==0){
    //   hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_After2gamma",MissingMass);
    // }
    if (nGG==0&&nGGG==0){
      hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_After2gamma3gamma",MissingMass);
    }

 
      hSvc->FillHisto(this->GetName()+"_PVeto_NHits",evt->PVetoRecoEvent->GetNHits());
      Int_t NclusPVeto = evt->PVetoRecoCl->GetNElements();
      
      for (Int_t j=0; j<NclusPVeto; ++j){
	cluP    = evt->PVetoRecoCl->Element(j);
	//time1 = clu->GetTime();
	//time3 = cluP->GetTime();
	hSvc->FillHisto(this->GetName()+"_PVeto_z", cluP->GetPosition().Z());
	hSvc->FillHisto(this->GetName()+"_PVeto_x", cluP->GetPosition().X());
	hSvc->FillHisto(this->GetName()+"_PVeto_y", cluP->GetPosition().Y());
	deltat=(cluP->GetTime()-clu->GetTime());
	cluPgood = cluP;
	for (Int_t k=0; k<NclusPVeto; ++k){
	  cluP2    = evt->PVetoRecoCl->Element(k);
	  if(abs(cluP2->GetTime()-clu->GetTime())<abs(deltat)) {
	    deltat=cluP2->GetTime()-clu->GetTime();
	    cluPgood = cluP2;
	  }
	  
	}
	
	if(nGG==0){
	  
	  hSvc->FillHisto(this->GetName()+"_ECal_PVeto_TimeDiff",deltat);
	  //hSvc->FillHisto(this->GetName()+"_PVeto_z", cluP->GetPosition().Z());
	  hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_EnergyvsZ",clu->GetEnergy(),cluPgood->GetPosition().Z());
	  if(abs(deltat)<2){

	    hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_EnergyvsZ_inTime",clu->GetEnergy(),cluPgood->GetPosition().Z());
	  }
	  

	  //hSvc->FillHisto(this->GetName()+"_PVeto_x", cluP->GetPosition().X());
	}

	if(clu&&cluP) {
	  if(IsBrems(clu,cluP)) nBrems++;
	}
	
      }
      if(nGG==0&&nGGG==0&&nBrems==0){
	hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_After2gamma3gammaBrems",MissingMass);
      }
    
    
    }//End of first loop over clusters

    
  }//End of 2 and 3 gamma selection for ECal


  //Begin 2 and 3 gamma selection for ECalML
  if(evt->ECalMLRecoEvent){
    hSvc->FillHisto(this->GetName()+"_ECalML_NHits",evt->ECalMLRecoEvent->GetNHits());

    Int_t NclusECalML = evt->ECalMLRecoCl->GetNElements();
    hSvc->FillHisto(this->GetName()+"_ECalML_NClusters",NclusECalML);
    
    TRecoVCluster* cluML=NULL;
    TRecoVCluster* clu2ML=NULL;
    TRecoVCluster* clu3ML=NULL;
  }//End 2 and 3 gamma selection for ECalML
  
  fResult = true;
  return fResult;
}
