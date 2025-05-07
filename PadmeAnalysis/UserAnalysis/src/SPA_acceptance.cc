#include "SPA_acceptance.hh"
#include "HistoSvc.hh"
#include "GeneralInfo.hh"
#include "TPVetoMCHit.hh"
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
  hSvcVal->BookHisto(this->GetName()+"_SAC_NHits",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_HEPVeto_NHits",100,0.0,100.0);

  
  hSvcVal->BookHisto(this->GetName()+"_ECal_NClusters",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_SAC_NClusters",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_HEPVeto_NClusters",100,0.0,100.0);

  
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
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_deltaT_nocenter",1000,-500.0,500.0);
  
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

  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_05ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_05ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_05ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_05ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_05ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_05ns_BremCut",1000,-500.0,500.0);

  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_1ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_1ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_1ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_1ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_1ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_1ns",1000,-500.0,500.0);

  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_1ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_1ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_1ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_1ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_1ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_1ns_BremCut",1000,-500.0,500.0);

  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_2ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_2ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_2ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_2ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_2ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_2ns",1000,-500.0,500.0);

  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_2ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_2ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_2ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_2ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_2ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_2ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_BkgCluRadius",1000,0.0,1000.0);

  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_2ns_BremCut_Geom",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_2ns_BremCut_Geom",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_2ns_BremCut_Geom",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_2ns_BremCut_Geom",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_2ns_BremCut_Geom",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_2ns_BremCut_Geom",1000,-500.0,500.0);

  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_5ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_5ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_5ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_5ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_5ns",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_5ns",1000,-500.0,500.0);

  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_5ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_5ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_5ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_5ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_5ns_BremCut",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_5ns_BremCut",1000,-500.0,500.0);
  
  hSvcVal->BookHisto2(this->GetName()+"_ECal_MissingMass_ThetaGamma",500,0.0,500,200,0.0,0.2);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_checkEvsRadiusafterCuts",500,0.0,500,1000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_PVeto_DeltaT_theRest",1200,-600.0,600.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_PVeto_DeltaT_theRest_Z",1200,-600.0,600.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_Energy_theRest",500,0.0,500.0,500,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_Energy_theRest_inTime",500,0.0,500.0,500,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_EnergyPos_theRest_inTime",500,0.0,500.0,2000,-1000.0,1000.0);
		      
  hSvcVal->BookHisto(this->GetName()+"_ECal_PVeto_TimeDiff",1200,-600.0,600.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_TimeDiff_vs_Z",1200,-600.0,600.0,2000,-1000.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_PVeto_ChID",100,0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_PVeto_z",2000,-1000.0,1000.0);
  hSvcVal->BookHisto2(this->GetName()+"_PVeto_z_vs_ChID",2000,-1000.0,1000.0,90,0.,90.);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_EnergyvsZ",500,0.0,500.0,2000,-1000.0,1000.0);
  hSvcVal->BookHisto2(this->GetName()+"_PVeto_EnergyvsZ",500,0.0,500.0,2000,-1000.0,1000.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_EnergyvsChID",500,0.0,500.0,90,0.0,90.0);
  hSvcVal->BookHisto2(this->GetName()+"_PVeto_ZvsE",86,-447.0,500.0,500,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_EnergyvsZ_inTime",500,0.0,500.0,2000,-1000.0,1000.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_EnergyvsZ_1clu",500,0.0,500.0,2000,-1000.0,1000.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_EnergyvsZ_inTime_1clu",500,0.0,500.0,2000,-1000.0,1000.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_ZvsEnergy",2000,-1000.0,1000.0,500,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_ZvsEnergy_inTime",2000,-1000.0,1000.0,500,0.0,500.0);
  
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_ZvsGammaEnergy_beforeCut",2000,-1000.0,1000.0,500,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_PVeto_ZvsEnergy_beforeCut",2000,-1000.0,1000.0,500,0.0,500.0);  
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_ChIDvsGammaEnergy_beforeCut",90,0.0,90.0,500,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_PVeto_ChIDvsEnergy_beforeCut",90,0.0,90.0,500,0.0,500.0);
 
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_ZvsGammaEnergy_Cut",2000,-1000.0,1000.0,500,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_ChIDvsGammaEnergy_Cut",90,0.0,90.0,500,0.0,500.0); 
  hSvcVal->BookHisto2(this->GetName()+"_PVeto_ZvsEnergy_Cut",2000,-1000.0,1000.0,500,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_PVeto_ChIDvsEnergy_Cut",90,0.0,90.0,500,0.0,500.0);

  
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_ChIDvsGammaEnergy_beforeCut",90,0.0,90.0,500,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_PVeto_ChIDvsEnergy_beforeCut",90,0.0,90.0,500,0.0,500.0);
  
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_ZvsGammaEnergy_afterCut",2000,-1000.0,1000.0,500,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_ZvsEnergy_1clu",2000,-1000.0,1000.0,500,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_ZvsEnergy_inTime_1clu",2000,-1000.0,1000.0,500,0.0,500.0);


  hSvcVal->BookHisto(this->GetName()+"_SAC_PVeto_TimeDiff",1200,-600.0,600.0);
  hSvcVal->BookHisto2(this->GetName()+"_SAC_PVeto_TimeDiff_vs_Z",1200,-600.0,600.0,2000,-1000.0,1000.0);
  hSvcVal->BookHisto2(this->GetName()+"_SAC_PVeto_EnergyvsZ",500,0.0,500.0,2000,-1000.0,1000.0);
  hSvcVal->BookHisto2(this->GetName()+"_SAC_PVeto_EnergyvsChID",500,0.0,500.0,90,0.0,90.0);
  hSvcVal->BookHisto2(this->GetName()+"_SAC_PVeto_EnergyvsZ_inTime",500,0.0,500.0,2000,-1000.0,1000.0);
  hSvcVal->BookHisto2(this->GetName()+"_SAC_PVeto_EnergyvsZ_inTime_banan",500,0.0,500.0,2000,-1000.0,1000.0);
  hSvcVal->BookHisto2(this->GetName()+"_SAC_PVeto_EnergyvsChID_inTime",500,0.0,500.0,90,0.0,90.0);
  hSvcVal->BookHisto2(this->GetName()+"_SAC_PVeto_EnergyFromVetovsChID_inTime_banan",500,0.0,500.0,90,0.0,90.0);
  hSvcVal->BookHisto2(this->GetName()+"_SAC_PVeto_EnergyvsChID_inTime_banan",500,0.0,500.0,90,0.0,90.0);

  
  hSvcVal->BookHisto2(this->GetName()+"_SAC_PVeto_ZvsEnergy",2000,-1000.0,1000.0,500,0.0,500.0); 
  hSvcVal->BookHisto2(this->GetName()+"_SAC_PVeto_ChIDvsEnergy",90,0.0,90.0,500,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_SAC_PVeto_ZvsEnergy_inTime",2000,-1000.0,1000.0,500,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_SAC_PVeto_ZvsEnergy_inTime_banan",2000,-1000.0,1000.0,500,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_SAC_PVeto_ChIDvsEnergy_inTime",90,0.0,90.0,500,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_SAC_PVeto_ChIDvsEnergy_inTime_banan",90,0.0,90.0,500,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_SAC_PVeto_ChIDvsEnergyFromVeto_inTime_banan",90,0.0,90.0,500,0.0,500.0);
  
  hSvcVal->BookHisto2(this->GetName()+"_SAC_PVeto_ZvsEnergy_inTime_banan_rebin",86,-447.0,500.0,500,0.0,500.0);

  hSvcVal->BookHisto2(this->GetName()+"_PVeto_Energy_vs_Z",1000,0.0,500.0,2000,-1000.0,1000.0);

  hSvcVal->BookHisto(this->GetName()+"_PVeto_Energy",500,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_PVeto_Energy_ChID",500,0.0,500.0,100,0.0,100.0);
  
  hSvcVal->BookHisto2(this->GetName()+ "_PVetoEHit_PVeto_Z_inTime",500,0.0,500.0,2000,-1000.0,1000.0);
  
  hSvcVal->BookHisto(this->GetName()+"_ECal_HEPVeto_TimeDiff",1200,-600.0,600.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_HEPVeto_ZvsGammaEnergy_beforeCut",16,789.0,949.0,500,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_HEPVeto_Energy_beforeCut",500,0.0,500.0,500,0.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_HEPVeto_x",16,789.0,949.0);
  hSvcVal->BookHisto(this->GetName()+"_HEPVeto_y",2000,-1000.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_HEPVeto_z",2000,-1000.0,1000.0);
  return true;
}

double SPA_acceptance::ComputeRadius(Double_t PosX1, Double_t PosY1)
{
  Double_t R = sqrt( PosX1*PosX1 + PosY1*PosY1 );  
  return R;
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

  Double_t fSafeSpaceMargin = 3.15;
      
  Int_t NclusECal = evt->ECalRecoCl->GetNElements();
  Int_t NclusPVeto = evt->PVetoRecoCl->GetNElements();
  Int_t NclusHEPVeto = evt->HEPVetoRecoCl->GetNElements();

  if(evt->HEPVetoRecoEvent){
    hSvc->FillHisto(this->GetName()+"_HEPVeto_NHits",evt->HEPVetoRecoEvent->GetNHits());

    hSvc->FillHisto(this->GetName()+"_HEPVeto_NClusters",NclusHEPVeto);
  }
  
  if(evt->ECalRecoEvent){
    hSvc->FillHisto(this->GetName()+"_ECal_NHits",evt->ECalRecoEvent->GetNHits());

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
    //Int_t cluBrem=0;
    Float_t cluPVetoZ=0;
    Float_t cluPVetoE=0;
    Float_t cluPVetoEECal=0;
    Float_t cluPVetoChID=0;

    //Fit parameters for Bremsstrahlung fit using expected positron energy from beam energy - ECal photon energy and Z position of the cluster in the PVeto
    Float_t alineECalZ=0.000266708;
    Float_t blineECalZ=0.328642;
    Float_t cgoreECalZ=232.682;
    Float_t cdoluECalZ=72.682;

    //Fit parameters for Bremsstrahlung fit using expected positron energy from beam energy - ECal photon energy and Channel ID of the cluster in the PVeto
    // Float_t alineECalCh=0.000266708;
    // Float_t blineECalCh=0.328642;
    // Float_t cgoreECalCh=232.682;
    // Float_t cdoluECalCh=72.682;

    //Fit parameters for Bremsstrahlung fit using positron energy from PVeto  and Z position of the cluster in the PVeto
    // Float_t alineEZ=0.000266708;
    // Float_t blineEZ=0.328642;
    // Float_t cgoreEZ=232.682;
    // Float_t cdoluEZ=72.682;

    //Fit parameters for Bremsstrahlung fit using positron energy from PVeto and ChannelID of the cluster in the PVeto
    // Float_t alineECh=0.000266708;
    // Float_t blineECh=0.328642;
    // Float_t cgoreECh=232.682;
    // Float_t cdoluECh=72.682;


      

    Double_t fSafeSpaceMargin = 3.15;
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
	  if(ComputeRadius(cluU->GetPosition().X(),cluU->GetPosition().Y())>(fGeneralInfo->GetRadiusMin()-fSafeSpaceMargin)&&ComputeRadius(clu2->GetPosition().X(),clu2->GetPosition().Y())>(fGeneralInfo->GetRadiusMin()-fSafeSpaceMargin)) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_deltaT_nocenter",cluU->GetTime()-clu2->GetTime());
	  if (clu2->GetEnergy()>safeE){
	    if(fabs(cluU->GetTime()-clu2->GetTime())<deltaT05) intime05+=1;
	    if(fabs(cluU->GetTime()-clu2->GetTime())<deltaT1) intime1+=1;		  
	    if(fabs(cluU->GetTime()-clu2->GetTime())<deltaT2) intime2+=1;	
	    if(fabs(cluU->GetTime()-clu2->GetTime())<deltaT5) intime5+=1;
	  }
	}
      }
    

      Int_t cluBrem=0;
      if (evt->PVetoRecoEvent)   {
	TRecoVCluster* cluPVetoR=NULL;
	for (Int_t i=0; i<NclusPVeto; ++i){
	  cluPVetoR    = evt->PVetoRecoCl->Element(i);
	  cluPVetoZ = cluPVetoR->GetPosition().Z();
	  cluPVetoE = cluPVetoR->GetEnergy();
	  cluPVetoEECal = BeamEnergy - cluU->GetEnergy();
	  cluPVetoChID = cluPVetoR->GetChannelId();
	  //if(cluPVetoE>0.01) cluBrem+=1;

	  //Select intime clusters in ECal and PVeto for vetoing
	  if(fabs(cluPVetoR->GetTime()-cluU->GetTime()) < 5.){
	    //cluBrem+=1;

	    //See energy vs. position only after time cut. GammaEnergy histos have the expected positron energy from beam energy - ECal photon energy.
	    hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_ZvsGammaEnergy_beforeCut",cluPVetoZ,cluPVetoEECal);	    
	    hSvc->FillHisto2(this->GetName()+"_PVeto_ZvsEnergy_beforeCut",cluPVetoZ,cluPVetoE);
	    hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_ChIDvsGammaEnergy_beforeCut",cluPVetoChID,cluPVetoEECal);
	    hSvc->FillHisto2(this->GetName()+"_PVeto_ChIDvsEnergy_beforeCut",cluPVetoChID,cluPVetoE);

	    //Veto using expected energy from BeamE-ECalE and Z position
	    if(cluPVetoEECal < (alineECalZ*cluPVetoZ*cluPVetoZ+blineECalZ*cluPVetoZ+cgoreECalZ) &&
	       cluPVetoEECal > (alineECalZ*cluPVetoZ*cluPVetoZ+blineECalZ*cluPVetoZ+cdoluECalZ)){
	      hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_ZvsGammaEnergy_Cut",cluPVetoZ,cluPVetoEECal);
	      cluBrem+=1;
	    }
	    //Veto using expected energy from BeamE-ECalE and Channel ID
	    // if(cluPVetoEECal < (alineECalCh*cluPVetoChID*cluPVetoChID+blineECalCh*cluPVetoChID+cgoreECalCh) &&
	    //    cluPVetoEECal > (alineECalCh*cluPVetoChID*cluPVetoChID+blineECalCh*cluPVetoChID+cdoluECalCh)){
	    //   hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_ChIDvsGammaEnergy_Cut",cluPVetoChID,cluPVetoEECal);
	    //   cluBrem+=1;
	    // }
	    //Veto using energy from PVeto and Z position
	    // if(cluPVetoE < (alineEZ*cluPVetoZ*cluPVetoZ+blineEZ*cluPVetoZ+cgoreEZ) &&
	    //    cluPVetoE > (alineEZ*cluPVetoZ*cluPVetoZ+blineEZ*cluPVetoZ+cdoluEZ)){
	    //   hSvc->FillHisto2(this->GetName()+"_PVeto_ZvsEnergy_Cut",cluPVetoZ,cluPVetoE);
	    //   cluBrem+=1;
	    // }
	    
	    //Veto using energy from PVeto and Channel ID    
	    // if(cluPVetoE < (alineECh*cluPVetoChID*cluPVetoChID+blineECh*cluPVetoChID+cgoreECh) &&
	    //    cluPVetoE > (alineECh*cluPVetoChID*cluPVetoChID+blineECh*cluPVetoChID+cdoluECh)){
	    //   hSvc->FillHisto2(this->GetName()+"_PVeto_ChIDvsEnergy_Cut",cluPVetoChID,cluPVetoE);
	    //   cluBrem+=1;
	    // }
	  }
	}
      }
      
      if (evt->HEPVetoRecoEvent)   {
	TRecoVCluster* cluHEPVeto=NULL;
	for (Int_t i=0; i<NclusHEPVeto; ++i){
	  cluHEPVeto = evt->HEPVetoRecoCl->Element(i);
	  hSvc->FillHisto(this->GetName()+"_ECal_HEPVeto_TimeDiff",cluU->GetTime()-cluHEPVeto->GetTime());
	  if(fabs(cluHEPVeto->GetTime()-cluU->GetTime()) < 2.){
	    cluBrem+=1;
	    hSvc->FillHisto(this->GetName()+"_HEPVeto_x",cluHEPVeto->GetPosition().X());
	    hSvc->FillHisto(this->GetName()+"_HEPVeto_y",cluHEPVeto->GetPosition().Y());
	    hSvc->FillHisto(this->GetName()+"_HEPVeto_z",cluHEPVeto->GetPosition().Z());
	    hSvc->FillHisto2(this->GetName()+"_ECal_HEPVeto_ZvsGammaEnergy_beforeCut",cluHEPVeto->GetPosition().X(), BeamEnergy-cluU->GetEnergy());
	    hSvc->FillHisto2(this->GetName()+"_ECal_HEPVeto_Energy_beforeCut",cluHEPVeto->GetEnergy(), cluU->GetEnergy());
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

      if(intime5==0&&cluBrem==0){
	if(cluU->GetEnergy()>10.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_5ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>20.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_5ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>30.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_5ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>40.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_5ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>50.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_5ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>60.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_5ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));	
      }
      
      if(intime2==0){
	if (evt->PVetoRecoEvent)   {
	  TRecoVCluster* cluPVetoR=NULL;
	  for (Int_t i=0; i<NclusPVeto; ++i){
	    cluPVetoR    = evt->PVetoRecoCl->Element(i);
	    //cluPVetoZ = cluPVetoR->GetPosition().Z();
	    //cluPVetoE = cluPVetoR->GetEnergy();
	    //hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_ZvsGammaEnergy_beforeCut",cluPVetoR->GetPosition().Z(),cluU->GetEnergy());
	  }
	}
	if(cluU->GetEnergy()>10.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_2ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>20.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_2ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>30.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_2ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>40.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_2ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>50.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_2ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>60.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_2ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));	
      }

      if(intime2==0&&cluBrem==0){
	if (evt->PVetoRecoEvent)   {
	  TRecoVCluster* cluPVetoR=NULL;
	  for (Int_t i=0; i<NclusPVeto; ++i){
	    cluPVetoR    = evt->PVetoRecoCl->Element(i);
	    if(fabs(cluPVetoR->GetTime()-cluU->GetTime()) < 2.){
	      //cluPVetoZ = cluPVetoR->GetPosition().Z();
	      //cluPVetoE = cluPVetoR->GetEnergy();
	      hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_ZvsGammaEnergy_afterCut",cluPVetoR->GetPosition().Z(),BeamEnergy - cluU->GetEnergy());
	    }
	  }
	}
	if(cluU->GetEnergy()>10.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_2ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>20.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_2ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>30.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_2ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>40.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_2ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>50.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_2ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>60.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_2ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));

	
	
	Double_t RadiusClu=ComputeRadius(cluU->GetPosition().X(),cluU->GetPosition().Y() );
	//if (ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy()))>200.)  hSvc->FillHisto(this->GetName()+"_ECal_BkgCluRadius",RadiusClu);
	//if((RadiusClu>(fGeneralInfo->GetRadiusMin()-fSafeSpaceMargin))&&(RadiusClu<(fGeneralInfo->GetRadiusMax()))){
	if(RadiusClu>75.){

	  if(cluU->GetEnergy()>10.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_2ns_BremCut_Geom",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	  if(cluU->GetEnergy()>20.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_2ns_BremCut_Geom",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	  if(cluU->GetEnergy()>30.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_2ns_BremCut_Geom",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	  if(cluU->GetEnergy()>40.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_2ns_BremCut_Geom",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	  if(cluU->GetEnergy()>50.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_2ns_BremCut_Geom",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	  if(cluU->GetEnergy()>60.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_2ns_BremCut_Geom",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));

	  if (evt->PVetoRecoEvent)   {
	    TRecoVCluster* cluPVetoR=NULL;
	    for (Int_t i=0; i<NclusPVeto; ++i){
	      cluPVetoR    = evt->PVetoRecoCl->Element(i);
	      hSvc->FillHisto(this->GetName()+"_ECal_PVeto_DeltaT_theRest",fabs(cluPVetoR->GetTime()-cluU->GetTime()));
	      
	      if(cluU->GetEnergy()<100.) hSvc->FillHisto(this->GetName()+"_ECal_PVeto_DeltaT_theRest_Z",fabs(cluPVetoR->GetTime()-cluU->GetTime()));
	      
	      if(fabs(cluPVetoR->GetTime()-cluU->GetTime())<=1){
		
		  hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_EnergyPos_theRest_inTime",cluU->GetEnergy(),cluPVetoR->GetPosition().Z());
		  hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_Energy_theRest_inTime",cluU->GetEnergy(),cluPVetoR->GetEnergy());
	      }
	      hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_Energy_theRest",cluU->GetEnergy(),cluPVetoR->GetEnergy());
	    }
	  }
	  hSvc->FillHisto2(this->GetName()+"_ECal_checkEvsRadiusafterCuts",cluU->GetEnergy(),ComputeRadius(cluU->GetPosition().X(),cluU->GetPosition().Y()));
	  
	}
      }
      
      if(intime1==0){
	if(cluU->GetEnergy()>10.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_1ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>20.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_1ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>30.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_1ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>40.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_1ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>50.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_1ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>60.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_1ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));	
      }

      if(intime1==0&&cluBrem==0){
	if(cluU->GetEnergy()>10.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_1ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>20.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_1ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>30.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_1ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>40.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_1ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>50.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_1ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>60.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_1ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));	
      }
      
      if(intime05==0){
	if(cluU->GetEnergy()>10.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_05ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>20.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_05ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>30.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_05ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>40.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_05ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>50.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_05ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>60.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_05ns",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));	
      }

      if(intime05==0&&cluBrem==0){
	if(cluU->GetEnergy()>10.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_10MeV_inTime_05ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>20.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_20MeV_inTime_05ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>30.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_30MeV_inTime_05ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>40.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_40MeV_inTime_05ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>50.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_50MeV_inTime_05ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));
	if(cluU->GetEnergy()>60.) hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_Ecut_60MeV_inTime_05ns_BremCut",ComputeMissingMass(BeamEnergy,(cluU->GetPosition().X()),(cluU->GetPosition().Y()),Z_1_U,(cluU->GetEnergy())));	
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

    TRecoVCluster* cluPVeto=NULL;
    TRecoVHit* hit = NULL;
    std::vector<Int_t> hitVInCl;
    Float_t pEnergy=0;
    TRecoVObject* fPVeto_hitEvent=evt->PVetoRecoEvent;
    
    for (Int_t i=0; i<NclusECal; ++i){
      cluU    = evt->ECalRecoCl->Element(i);
      for (Int_t j=0; j<NclusPVeto; ++j){
        cluPVeto    = evt->PVetoRecoCl->Element(j);
	hSvc->FillHisto(this->GetName()+"_ECal_PVeto_TimeDiff", (cluPVeto->GetTime()-cluU->GetTime()));
	hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_TimeDiff_vs_Z", (cluPVeto->GetTime()-cluU->GetTime()),cluPVeto->GetPosition().Z());
	hSvc->FillHisto(this->GetName()+"_PVeto_z",cluPVeto->GetPosition().Z());
	hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_EnergyvsZ",cluU->GetEnergy(),cluPVeto->GetPosition().Z());
	hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_ZvsEnergy",cluPVeto->GetPosition().Z(),cluU->GetEnergy());
	if((cluPVeto->GetTime()-cluU->GetTime())>0.&&(cluPVeto->GetTime()-cluU->GetTime())<1.){
	  hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_EnergyvsZ_inTime",cluU->GetEnergy(),cluPVeto->GetPosition().Z());
	  hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_ZvsEnergy_inTime",cluPVeto->GetPosition().Z(),cluU->GetEnergy());
	  
	  hSvc->FillHisto2(this->GetName()+"_PVeto_ZvsE",cluPVeto->GetPosition().Z(),BeamEnergy-(cluU->GetEnergy()));
	}
	if(NclusECal==1){
	  hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_EnergyvsZ_1clu",cluU->GetEnergy(),cluPVeto->GetPosition().Z());
	  hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_ZvsEnergy_1clu",cluPVeto->GetPosition().Z(),cluU->GetEnergy());
	  if((cluPVeto->GetTime()-cluU->GetTime())>0.&&(cluPVeto->GetTime()-cluU->GetTime())<1.){
	    hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_EnergyvsZ_inTime_1clu",cluU->GetEnergy(),cluPVeto->GetPosition().Z());
	    hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_ZvsEnergy_inTime_1clu",cluPVeto->GetPosition().Z(),cluU->GetEnergy());
	  }
	  // }
	  // Int_t NHitsInClu= cluPVeto->GetNHitsInClus();
	  // hitVInCl = cluPVeto->GetHitVecInClus();
	  // for (Int_t ih=0; ih<NHitsInClu; ++ih)// {
	  // hit = fPVeto_hitEvent->Hit(hitVInCl[ih]);
	  // //if(hit->GetTrackID()!=0){
	  //   pEnergy=hit->GetTrackEnergy();
	  //   //std::cout<<"Cluster number: "<<j<<" Hit number: "<<ih<<" Track Energy: "<<pEnergy<<" Track ID: "<<hit->GetTrackID()<<" Channel: "<<hit->GetChannelId()<<std::endl;
	  //   if((cluPVeto->GetTime()-cluU->GetTime())>-5.&&(cluPVeto->GetTime()-cluU->GetTime())<7.){
	  //     //std::cout<<"INTIIIIIIIIIIIIIIIIIIIIIME"<<std::endl;
	  //     hSvc->FillHisto2(this->GetName()+"_PVetoEHit_PVeto_Z_inTime",pEnergy,cluPVeto->GetPosition().Z());
	  //   }
	  // }
	  
	}
      }
    }
    //std::cout<<"End of event! New event."<<std::endl;
  }

  
  if(evt->SACRecoEvent){
    hSvc->FillHisto(this->GetName()+"_SAC_NHits",evt->SACRecoEvent->GetNHits());
    Int_t NclusSAC = evt->SACRecoCl->GetNElements();
    hSvc->FillHisto(this->GetName()+"_SAC_NClusters",NclusSAC);
    
    TRecoVCluster* cluPVeto2=NULL;
    TRecoVCluster* cluSAC=NULL;
    
    for (Int_t i=0; i<NclusPVeto; ++i){
      cluPVeto2    = evt->PVetoRecoCl->Element(i);
      hSvc->FillHisto(this->GetName()+"_PVeto_ChID", cluPVeto2->GetChannelId());
      hSvc->FillHisto2(this->GetName()+"_PVeto_Energy_vs_Z", cluPVeto2->GetEnergy(),cluPVeto2->GetPosition().Z());
      
      hSvc->FillHisto2(this->GetName()+"_PVeto_z_vs_ChID",cluPVeto2->GetPosition().Z(),cluPVeto2->GetChannelId());
      
      for (Int_t j=0; j<NclusSAC; ++j){
	cluSAC    = evt->SACRecoCl->Element(j);
	hSvc->FillHisto(this->GetName()+"_SAC_PVeto_TimeDiff", (cluPVeto2->GetTime()-cluSAC->GetTime()));
	hSvc->FillHisto2(this->GetName()+"_SAC_PVeto_TimeDiff_vs_Z", (cluPVeto2->GetTime()-cluSAC->GetTime()),cluPVeto2->GetPosition().Z());
	hSvc->FillHisto2(this->GetName()+"_SAC_PVeto_EnergyvsZ",cluSAC->GetEnergy(),cluPVeto2->GetPosition().Z());
	hSvc->FillHisto2(this->GetName()+"_SAC_PVeto_EnergyvsChID",cluSAC->GetEnergy(),cluPVeto2->GetChannelId());
	hSvc->FillHisto2(this->GetName()+"_SAC_PVeto_ZvsEnergy",cluPVeto2->GetPosition().Z(),cluSAC->GetEnergy());
	hSvc->FillHisto2(this->GetName()+"_SAC_PVeto_ChIDvsEnergy",cluPVeto2->GetChannelId(),cluSAC->GetEnergy());
	if((cluPVeto2->GetTime()-cluSAC->GetTime())>-1.&&(cluPVeto2->GetTime()-cluSAC->GetTime())<1.){
	  hSvc->FillHisto2(this->GetName()+"_SAC_PVeto_EnergyvsZ_inTime",cluSAC->GetEnergy(),cluPVeto2->GetPosition().Z());
	  hSvc->FillHisto2(this->GetName()+"_SAC_PVeto_ZvsEnergy_inTime",cluPVeto2->GetPosition().Z(),cluSAC->GetEnergy());
	  
	  hSvc->FillHisto2(this->GetName()+"_SAC_PVeto_EnergyvsChID_inTime",cluSAC->GetEnergy(),cluPVeto2->GetChannelId());
	  hSvc->FillHisto2(this->GetName()+"_SAC_PVeto_ChIDvsEnergy_inTime",cluPVeto2->GetChannelId(),cluSAC->GetEnergy());
	  
	  if(cluPVeto2->GetPosition().Z()<500){
	    hSvc->FillHisto2(this->GetName()+"_SAC_PVeto_EnergyvsZ_inTime_banan",cluSAC->GetEnergy(),cluPVeto2->GetPosition().Z());
	    hSvc->FillHisto2(this->GetName()+"_SAC_PVeto_ZvsEnergy_inTime_banan",cluPVeto2->GetPosition().Z(),BeamEnergy-(cluSAC->GetEnergy()));
	    hSvc->FillHisto2(this->GetName()+"_SAC_PVeto_EnergyvsChID_inTime_banan",cluSAC->GetEnergy(),cluPVeto2->GetChannelId());
	    hSvc->FillHisto2(this->GetName()+"_SAC_PVeto_ChIDvsEnergy_inTime_banan",cluPVeto2->GetChannelId(),BeamEnergy-(cluSAC->GetEnergy()));
	    hSvc->FillHisto2(this->GetName()+"_SAC_PVeto_EnergyFromVetovsChID_inTime_banan",cluPVeto2->GetEnergy(),cluPVeto2->GetChannelId());
	    hSvc->FillHisto2(this->GetName()+"_SAC_PVeto_ChIDvsEnergyFromVeto_inTime_banan",cluPVeto2->GetChannelId(),cluPVeto2->GetEnergy());

	    
	    hSvc->FillHisto2(this->GetName()+"_SAC_PVeto_ZvsEnergy_inTime_banan_rebin",cluPVeto2->GetPosition().Z(),BeamEnergy-(cluSAC->GetEnergy()));
	    
	  }
	}
      }
    }
    
  }
  
  if(evt->PVetoRecoEvent){
    TRecoVCluster* cluPVeto3=NULL;
    for (Int_t i=0; i<NclusPVeto; ++i){
      cluPVeto3    = evt->PVetoRecoCl->Element(i);
      hSvc->FillHisto(this->GetName()+"_PVeto_Energy", cluPVeto3->GetEnergy());
      hSvc->FillHisto2(this->GetName()+"_PVeto_Energy_ChID", cluPVeto3->GetEnergy(),cluPVeto3->GetChannelId());
    }
  }
  

  
  fResult = true;
  return fResult;
}
