#include "SinglePhotonAnalysis.hh"
#include "HistoSvc.hh"
#include "GeneralInfo.hh"
#include "TLorentzVector.h"
#include <iostream>

using namespace std;

static SinglePhotonAnalysis ThisAnalyser("SinglePhotonAnalysis");

SinglePhotonAnalysis::SinglePhotonAnalysis(const char *s,int valid, int verb)
  : PadmeVAnalyser(s,valid,verb)
{;}

SinglePhotonAnalysis::SinglePhotonAnalysis(const char *s):
  PadmeVAnalyser(s)
{;}

Bool_t SinglePhotonAnalysis::InitHistos(){
  HistoSvc* hSvcVal =  HistoSvc::GetInstance();

  hSvcVal->makeFileDir(GetName());


  //Number of hits in each detector
  hSvcVal->BookHisto(this->GetName()+"_ECal_NHits",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_NHits",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_PVeto_NHits",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_EVeto_NHits",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_HEPVeto_NHits",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_SAC_NHits",100,0.0,100.0);

  //Number of clusters in each detector
  hSvcVal->BookHisto(this->GetName()+"_ECal_NClusters",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_NClusters",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_PVeto_NClusters",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_EVeto_NClusters",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_HEPVeto_NClusters",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_SAC_NClusters",100,0.0,100.0);

  //-----------------------------------------------2 gamma events-------------------------------------------------

  //2 gamma events Time difference
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2Clusters_TimeDiff",800,-100.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2Clusters_TimeDiff_2gamma",800,-100.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2Clusters_TimeDiff",800,-100.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2Clusters_TimeDiff_2gamma",800,-100.0,100.0);

  //Energy vs Energy for 2gamma events
  hSvcVal->BookHisto2(this->GetName()+"_ECal_2Clusters_EnergyvsEnergy",500,0.0,500.0,500,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_2Clusters_EnergyvsEnergy",500,0.0,500.0,500,0.0,500.0);

  //Energy sum of two clusters ECal
  hSvcVal->BookHisto(this->GetName()+"_ECal_2Clusters_Energysum",750,0.0,750.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2Clusters_Energysum_afterTimeLimitation",750,0.0,750.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2Clusters_Energysum_afterTimeDistanceLimitation",750,0.0,750.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2Clusters_Energysum_afterTimeDistanceMinMaxEnergyLimitation",750,0.0,750.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2Clusters_Energysum_afterTimeDistanceMinMaxEnergyMinMaxRadiusLimitation",750,0.0,750.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2Clusters_Energysum_afterTimeDistanceEminmaxRminmaxThetaLimitation",750,0.0,750.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2Clusters_Energysum_afterTimeDistanceEminmaxRminmaxThetaPhiLimitation",750,0.0,750.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2Clusters_Energysum_afterTimeDistanceEminmaxRminmaxThetaPhiInvMassLimitation",750,0.0,750.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2Clusters_Energysum_flatRegion",750,0.0,750.0);
  //hSvcVal->BookHisto(this->GetName()+"_ECal_2Clusters_Energysum_afterTimeEnergyEminmaxRminmaxDistanceThetaPhiLimitation",750,0.0,750.0);

  //ECal Time vs Energy sum for gamma gamma
  hSvcVal->BookHisto2(this->GetName()+"_ECal_2gamma_EnergyvsTime",1000,-500.0,500.0,350,0.0,700.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_2gamma_EnergyvsTime_timecut",1000,-500.0,500.0,350,0.0,700.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_2gamma_EnergyvsTime_timeDistancecut",1000,-500.0,500.0,350,0.0,700.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_2gamma_EnergyvsTime_timeDistanceMinMaxEnergycut",1000,-500.0,500.0,350,0.0,700.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_2gamma_EnergyvsTime_timeDistanceMinMaxEnergyMinMaxRadiuscut",1000,-500.0,500.0,350,0.0,700.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_2gamma_EnergyvsTime_timeDistanceEminmaxRminmaxThetacut",1000,-500.0,500.0,350,0.0,700.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_2gamma_EnergyvsTime_timeDistanceEminmaxRminmaxThetaPhicut",1000,-500.0,500.0,350,0.0,700.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_2gamma_EnergyvsTime_timeDistanceEminmaxRminmaxThetaPhiInvMasscut",1000,-500.0,500.0,350,0.0,700.0);
  //hSvcVal->BookHisto2(this->GetName()+"_ECal_2gamma_EnergyvsTime_timeEnergyEminmaxRminmaxDistanceThetaPhicut",1000,-500.0,500.0,350,0.0,700.0);

  //ECal angles  
  hSvcVal->BookHisto(this->GetName()+"_ECal_2gamma_sumTheta",1000,-1.0,1.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2gamma_sumTheta_afterTimeCut",1000,-1.0,1.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2gamma_DPhi",200,-10.0,10.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2gamma_DPhi_afterTimeCut",200,-10.0,10.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2gamma_DeltaPhi",200,-10.0,10.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2gamma_DeltaPhi_afterTimeCut",2000,-10.0,10.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_2gamma_sumThetavsDeltaPhi",1000,-1.0,1.0,100,0.0,10.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_2gamma_ThetavsTheta",1000,-1.0,1.0,1000,-1.0,1.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_2gamma_PhivsPhi",200,-10.0,10.0,200,-10.0,10.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_2gamma_sumThetavsDeltaPhi_afterTimeCut",1000,-1.0,1.0,100,0.0,10.0);

  //ECal profile histos for Time vs. Energy sum
  hSvcVal->BookHistoProf(this->GetName()+"_ECal_2gamma_sumThetavsDeltaPhi_afterallCuts_1ns",280,-140.0,140.0,350.0,500.0);
  hSvcVal->BookHistoProf(this->GetName()+"_ECal_2gamma_sumThetavsDeltaPhi_afterallCuts_2ns",140,-140.0,140.0,350.0,500.0);
  hSvcVal->BookHistoProf(this->GetName()+"_ECal_2gamma_sumThetavsDeltaPhi_afterallCuts_5ns",56,-140.0,140.0,350.0,500.0);
  hSvcVal->BookHistoProf(this->GetName()+"_ECal_2gamma_sumThetavsDeltaPhi_afterallCuts_10ns",28,-140.0,140.0,350.0,500.0);

  //Energy sum of two clusters ECalML
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2Clusters_Energysum",750,0.0,750.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2Clusters_Energysum_afterTimeLimitation",750,0.0,750.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2Clusters_Energysum_afterTimeDistanceLimitation",750,0.0,750.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2Clusters_Energysum_afterTimeDistanceMinMaxEnergyLimitation",750,0.0,750.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2Clusters_Energysum_afterTimeDistanceMinMaxEnergyMinMaxRadiusLimitation",750,0.0,750.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2Clusters_Energysum_afterTimeDistanceMinMaxEnergyMinMaxRadiusThetaLimitation",750,0.0,750.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2Clusters_Energysum_afterTimeDistanceMinMaxEnergyMinMaxRadiusThetaPhiLimitation",750,0.0,750.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2Clusters_Energysum_afterTimeDistanceMinMaxEnergyMinMaxRadiusThetaPhiInvMassLimitation",750,0.0,750.0);
  
  //ECalML Time vs Energy sum for gamma gamma
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_2gamma_EnergyvsTime",1000,-500.0,500.0,350,0.0,700.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_2gamma_EnergyvsTime_timecut",1000,-500.0,500.0,350,0.0,700.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_2gamma_EnergyvsTime_timeDistancecut",1000,-500.0,500.0,350,0.0,700.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_2gamma_EnergyvsTime_timeDistanceMinMaxEnergycut",1000,-500.0,500.0,350,0.0,700.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_2gamma_EnergyvsTime_timeDistanceMinMaxEnergyMinMaxRadiuscut",1000,-500.0,500.0,350,0.0,700.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_2gamma_EnergyvsTime_timeDistanceEminmaxRminmaxThetacut",1000,-500.0,500.0,350,0.0,700.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_2gamma_EnergyvsTime_timeDistanceEminmaxRminmaxThetaPhicut",1000,-500.0,500.0,350,0.0,700.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_2gamma_EnergyvsTime_timeDistanceEminmaxRminmaxThetaPhiInvMasscut",1000,-500.0,500.0,350,0.0,700.0);

  //ECalML angles
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2gamma_sumTheta",1000,-1.0,1.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2gamma_sumTheta_afterTimeCut",1000,-1.0,1.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2gamma_DPhi",200,-10.0,10.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2gamma_DPhi_afterTimeCut",200,-10.0,10.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2gamma_DeltaPhi",200,-10.0,10.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2gamma_DeltaPhi_afterTimeCut",2000,-10.0,10.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_2gamma_sumThetavsDeltaPhi",1000,-1.0,1.0,100,0.0,10.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_2gamma_ThetavsTheta",1000,-1.0,1.0,1000,-1.0,1.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_2gamma_PhivsPhi",200,-10.0,10.0,200,-10.0,10.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_2gamma_sumThetavsDeltaPhi_afterTimeCut",1000,-1.0,1.0,100,0.0,10.0);

  //ECalML profile histos for Time vs. Energy sum
  hSvcVal->BookHistoProf(this->GetName()+"_ECalML_2gamma_sumThetavsDeltaPhi_afterallCuts_1ns",280,-140.0,140.0,350.0,500.0);
  hSvcVal->BookHistoProf(this->GetName()+"_ECalML_2gamma_sumThetavsDeltaPhi_afterallCuts_2ns",140,-140.0,140.0,350.0,500.0);
  hSvcVal->BookHistoProf(this->GetName()+"_ECalML_2gamma_sumThetavsDeltaPhi_afterallCuts_5ns",56,-140.0,140.0,350.0,500.0);
  hSvcVal->BookHistoProf(this->GetName()+"_ECalML_2gamma_sumThetavsDeltaPhi_afterallCuts_10ns",28,-140.0,140.0,350.0,500.0);

  //Invariant mass for 2 gamma events
  hSvcVal->BookHisto(this->GetName()+"_ECal_2Clusters_InvariantMass",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2Clusters_InvariantMass",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2Clusters_InvariantMass_afterTimeAndEnergyLimitation",100,0.0,100.0);
  
   
  //Energy vs Invariant Mass
  hSvcVal->BookHisto2(this->GetName()+"_ECal_2Clusters_EnergyvsInvMass_afterTimeLimitation",750,0.0,750.0,100,0.0,100.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_2Clusters_EnergyvsInvMass",750,0.0,750.0,100,0.0,100.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_2Clusters_EnergyvsInvMass_afterTimeLimitation",750,0.0,750.0,100,0.0,100.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_2Clusters_EnergyvsInvMass",750,0.0,750.0,100,0.0,100.0);

  //ECal TimeDifference small peaks analysis
  hSvcVal->BookHisto(this->GetName()+"_ECal_2gamma_RightPeakClusterDistance",1000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2gamma_LeftPeakClusterDistance",1000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2gamma_MainPeakClusterDistance",1000,0.0,1000.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_2gamma_TimeDiffvsClusterDistance",100,-100.0,100.0,1000.0,0.0,1000.0);

  //ECalML TimeDifference small peaks analysis
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2gamma_RightPeakClusterDistance",1000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2gamma_LeftPeakClusterDistance",1000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_2gamma_MainPeakClusterDistance",1000,0.0,1000.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_2gamma_TimeDiffvsClusterDistance",100,-100.0,100.0,1000.0,0.0,1000.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_smallpeaks_EnergyvsEnergy",500,0.0,500.0,500.0,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_smallpeaks_position",1200,-600.0,600.0,1200.0,-600.0,600.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_smallpeaks_radius",600,0.0,600.0);

  //--------------------------------------------------3gammaEvents-------------------------------------------------------------------------

  //Time difference
  hSvcVal->BookHisto(this->GetName()+"_ECal_3Clusters_TimeDiff",1000,-500.,500.);
  hSvcVal->BookHisto(this->GetName()+"_ECal_3Clusters_TimeDiffMax",1000,0.,1000.);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_3Clusters_TimeDiff",1000,-500.,500.);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_3Clusters_TimeDiffMax",1000,0.,1000.);

  //Energy sum
  hSvcVal->BookHisto(this->GetName()+"_ECal_3Clusters_Energysum",1000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_3Clusters_Energysum",1000,0.0,1000.0);
    
  hSvcVal->BookHisto(this->GetName()+"_ECal_3Clusters_Energysum_InTime",1000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_3Clusters_Energysum_InTime",1000,0.0,1000.0);
  
  hSvcVal->BookHisto(this->GetName()+"_ECal_3Clusters_Energysum_InTimeDistance",1000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_3Clusters_Energysum_InTimeDistance",1000,0.0,1000.0);
  
  hSvcVal->BookHisto(this->GetName()+"_ECal_3Clusters_Energysum_InTimeDistanceEnergy",1000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_3Clusters_Energysum_InTimeDistanceEnergy",1000,0.0,1000.0);
  
  hSvcVal->BookHisto(this->GetName()+"_ECal_3Clusters_Energysum_InTimeDistanceEnergyCOG",1000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_3Clusters_Energysum_InTimeDistanceEnergyCOG",1000,0.0,1000.0);

  //Split Clusters 
  hSvcVal->BookHisto(this->GetName()+"_ECal_SplitClusters_EnergysumOfALL",1000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_SplitClusters_EnergysumOf2",1000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_SplitClusters_EnergysumOfALL",1000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_SplitClusters_EnergysumOf2",1000,0.0,1000.0);
  
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_splitClusters_position",1200,-600.0,600.0,1200.0,-600.0,600.0); 

  //COG ECal
  hSvcVal->BookHisto(this->GetName()+"_ECal_3Clusters_COG_X_all",300,-150.,150.);
  hSvcVal->BookHisto(this->GetName()+"_ECal_3Clusters_COG_X_InTime",300,-150.,150.); 
  hSvcVal->BookHisto(this->GetName()+"_ECal_3Clusters_COG_X_InTimeDistance",300,-150.,150.); 
  hSvcVal->BookHisto(this->GetName()+"_ECal_3Clusters_COG_X_InTimeDistanceEnergy",300,-150.,150.); 
  hSvcVal->BookHisto(this->GetName()+"_ECal_3Clusters_COG_Y_all",300,-150.,150.);
  hSvcVal->BookHisto(this->GetName()+"_ECal_3Clusters_COG_Y_InTime",300,-150.,150.); 
  hSvcVal->BookHisto(this->GetName()+"_ECal_3Clusters_COG_Y_InTimeDistance",300,-150.,150.); 
  hSvcVal->BookHisto(this->GetName()+"_ECal_3Clusters_COG_Y_InTimeDistanceEnergy",300,-150.,150.);
  
  hSvcVal->BookHisto2(this->GetName()+"_ECal_3Clusters_COG_XY_all",300,-150.,150.,300,-150.,150.);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_3Clusters_COG_XY_InTime",300,-150.,150.,300,-150.,150.); 
  hSvcVal->BookHisto2(this->GetName()+"_ECal_3Clusters_COG_XY_InTimeDistance",300,-150.,150.,300,-150.,150.); 
  hSvcVal->BookHisto2(this->GetName()+"_ECal_3Clusters_COG_XY_InTimeDistanceEnergy",300,-150.,150.,300,-150.,150.);
  
  //COG ECalML
  hSvcVal->BookHisto(this->GetName()+"_ECalML_3Clusters_COG_X_all",300,-150.,150.);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_3Clusters_COG_X_InTime",300,-150.,150.); 
  hSvcVal->BookHisto(this->GetName()+"_ECalML_3Clusters_COG_X_InTimeDistance",300,-150.,150.); 
  hSvcVal->BookHisto(this->GetName()+"_ECalML_3Clusters_COG_X_InTimeDistanceEnergy",300,-150.,150.); 
  hSvcVal->BookHisto(this->GetName()+"_ECalML_3Clusters_COG_Y_all",300,-150.,150.);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_3Clusters_COG_Y_InTime",300,-150.,150.); 
  hSvcVal->BookHisto(this->GetName()+"_ECalML_3Clusters_COG_Y_InTimeDistance",300,-150.,150.); 
  hSvcVal->BookHisto(this->GetName()+"_ECalML_3Clusters_COG_Y_InTimeDistanceEnergy",300,-150.,150.); 
  
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_3Clusters_COG_XY_all",300,-150.,150.,300,-150.,150.);
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_3Clusters_COG_XY_InTime",300,-150.,150.,300,-150.,150.); 
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_3Clusters_COG_XY_InTimeDistance",300,-150.,150.,300,-150.,150.); 
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_3Clusters_COG_XY_InTimeDistanceEnergy",300,-150.,150.,300,-150.,150.);

  //Invariant Mass
  hSvcVal->BookHisto(this->GetName()+"_ECal_3Clusters_InvariantMass",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_3Clusters_InvariantMass",100,0.0,100.0);
  
  //--------------------------------------------------MissingMass---------------------------------------------------------------------------
  //ECal
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_AllCLusters",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_InTimeClusters",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_InTimeDistanceClusters",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_InTimeDistanceEnergyClusters",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_InTimeDistanceEnergyRadiusClusters",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_InTimeDistanceEnergyRadiusThetaClusters",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_InTimeDistanceEnergyRadiusThetaPhiClusters",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_InTimeDistanceEnergyRadiusThetaPhiInvMassClusters",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_NoInTimeEnergyInvMassClusters",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_MissingMass_NoInTimeEnergyInvMassClustersPVeto",1000,-500.0,500.0);

  //ECalML
  hSvcVal->BookHisto(this->GetName()+"_ECalML_MissingMass_AllCLusters",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_MissingMass_InTimeClusters",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_MissingMass_InTimeDistanceClusters",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_MissingMass_InTimeDistanceEnergyClusters",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_MissingMass_InTimeDistanceEnergyRadiusClusters",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_MissingMass_InTimeDistanceEnergyRadiusThetaClusters",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_MissingMass_InTimeDistanceEnergyRadiusThetaPhiClusters",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_MissingMass_InTimeDistanceEnergyRadiusThetaPhiInvMassClusters",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_MissingMass_NoInTimeEnergyInvMassClusters",1000,-500.0,500.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_MissingMass_NoInTimeEnergyInvMassClustersPVeto",1000,-500.0,500.0);


  //-------------------------------------------------PVeto and ECal events--------------------------------------------------------------

  //Time difference
  hSvcVal->BookHisto(this->GetName()+"_ECalML_PVeto_TimeDiff",800,-100.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECalML_PVeto_TimeDiff_single",800,-100.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_PVeto_TimeDiff",800,-100.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_PVeto_TimeDiff_single",800,-100.0,100.0);

  //PVeto coordinates
  hSvcVal->BookHisto(this->GetName()+"PVeto_z",100,0.0,100.0);
  hSvcVal->BookHisto(this->GetName()+"PVeto_x",100,0.0,100.0);

  //PVeto Energy vs position
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_EnergyvsX",500,0.0,500.0,100,0.0,100.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_PVeto_EnergyvsZ",500,0.0,500.0,100,0.0,100.0);

  //Energy vs Energy
  hSvcVal->BookHisto2(this->GetName()+"_ECalML_PVeto_EnergyvsEnergy",500,0.0,500.0,500,0.0,500.0);
    

  //--------------------------------------------------Single clusters---------------------------------------------------------------

  //Energy vs Radius
  hSvcVal->BookHisto2(this->GetName()+"_SingleClustersML_EnergyvsRadius",500,0.0,500.0,1000,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_SingleClusters_EnergyvsRadius",500,0.0,500.0,1000,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_SingleClusters_EnergyvsRadius_2gamma",500,0.0,500.0,1000,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_SingleClustersML_EnergyvsRadius_after2gamma",500,0.0,500.0,1000,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_SingleClusters_EnergyvsRadius_after2gamma",500,0.0,500.0,1000,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_SingleClustersML_EnergyvsRadius_after2gammaPVeto",500,0.0,500.0,1000,0.0,500.0);
  hSvcVal->BookHisto2(this->GetName()+"_SingleClusters_EnergyvsRadius_after2gammaPVeto",500,0.0,500.0,1000,0.0,500.0);

  //Energy
  hSvcVal->BookHisto(this->GetName()+"_SingleClustersECalML_Energy",2000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_SingleClustersECal_Energy",2000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_SingleClustersECalML_Energy_after2gamma",2000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_SingleClustersECal_Energy_after2gamma",2000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_SingleClustersECalML_Energy_after2gammaPVeto",2000,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_SingleClustersECal_Energy_after2gammaPVeto",2000,0.0,1000.0); 
  
  return true;
}


double SinglePhotonAnalysis::ComputeInvariantMass(Double_t E1, Double_t E2, Double_t PosX1, Double_t PosX2, Double_t PosY1, Double_t PosY2)
{
  Double_t M=0;
  Double_t Z  = 3534;
  Double_t R = sqrt( (PosX1-PosX2)*(PosX1-PosX2) + (PosY1-PosY2)*(PosY1-PosY2) ); 
  M = sqrt((E1*E2*R*R)/(Z*Z)); 
  return M;
}

double SinglePhotonAnalysis::ComputeRadius(Double_t PosX1, Double_t PosY1)
{
  Double_t R = sqrt( PosX1*PosX1 + PosY1*PosY1 );  
  return R;
}

double SinglePhotonAnalysis::ComputeDistance(Double_t PosX1, Double_t PosY1, Double_t PosX2, Double_t PosY2)
{
  Double_t D = sqrt(( PosX1-PosX2)*(PosX1-PosX2) + (PosY1-PosY2)*(PosY1-PosY2) );  
  return D;
}

double SinglePhotonAnalysis::ComputeMissingMass(double BeamEnergy, Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma){
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
 return    Missing*Missing;
}

double SinglePhotonAnalysis::ComputeTheta(Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma){
 Double_t R = sqrt(PosX*PosX+PosY*PosY+PosZ*PosZ);
 double Px = E_gamma*PosX/R;
 double Py = E_gamma*PosY/R;
 double Pz = E_gamma*PosZ/R;
 TLorentzVector Photon   (Px,Py,Pz,E_gamma);
 return Photon.Theta();
}

double SinglePhotonAnalysis::ComputePhi(Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma){
 Double_t R = sqrt(PosX*PosX+PosY*PosY+PosZ*PosZ);
 double Px = E_gamma*PosX/R;
 double Py = E_gamma*PosY/R;
 double Pz = E_gamma*PosZ/R;
 TLorentzVector Photon   (Px,Py,Pz,E_gamma);
 return Photon.Phi();
}

double SinglePhotonAnalysis::ComputeDPhi(Double_t PosX1, Double_t PosY1, Double_t PosZ1, Double_t E_gamma1,Double_t PosX2, Double_t PosY2, Double_t PosZ2, Double_t E_gamma2){
 Double_t R1 = sqrt(PosX1*PosX1+PosY1*PosY1+PosZ1*PosZ1);
 Double_t R2 = sqrt(PosX2*PosX2+PosY2*PosY2+PosZ2*PosZ2);
 double Px1 = E_gamma1*PosX1/R1;
 double Px2 = E_gamma2*PosX2/R2;
 double Py1 = E_gamma1*PosY1/R1;
 double Py2 = E_gamma2*PosY2/R2;
 double Pz1 = E_gamma1*PosZ1/R1;
 double Pz2 = E_gamma2*PosZ2/R2;

 TVector3 Momentum1 = TVector3(Px1,Py1,Pz1);
 TVector3 Momentum2 = TVector3(Px2,Py2,Pz2);

 Momentum1.SetZ(0.);
 Momentum2.SetZ(0.);

 double scalarP = Momentum1.Dot(Momentum2);
 double cosphi = scalarP/Momentum1.Perp()/Momentum2.Perp();
 double dPhi = acos(cosphi);

 return dPhi;

}

double SinglePhotonAnalysis::ComputeCOG_GGG(Double_t E_gamma1, Double_t E_gamma2, Double_t E_gamma3, Double_t Pos1, Double_t Pos2, Double_t Pos3){
  Double_t ETot=0;
  Double_t COG=0;
  
  ETot=E_gamma1+E_gamma2+E_gamma3;
  COG= E_gamma1*Pos1+E_gamma2*Pos2+E_gamma3* Pos3;

  COG=COG/ETot;
  return COG;

}

Bool_t SinglePhotonAnalysis::Process(){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  GeneralInfo* fGeneralInfo = GeneralInfo::GetInstance();
  fGeneralInfo->Process();

  //std::cout<<"tuk"<<std::endl;
  if(evt->PVetoRecoEvent || evt->ECalRecoEvent || evt->ECalMLRecoEvent || evt->HEPVetoRecoEvent || evt->SACRecoEvent || evt->EVetoRecoEvent){
    // if(evt->ECalRecoEvent || evt->ECalMLRecoEvent){
    hSvc->FillHisto(this->GetName()+"_PVeto_NHits",evt->PVetoRecoEvent->GetNHits());
    hSvc->FillHisto(this->GetName()+"_ECal_NHits",evt->ECalRecoEvent->GetNHits());
    hSvc->FillHisto(this->GetName()+"_ECalML_NHits",evt->ECalMLRecoEvent->GetNHits());
    hSvc->FillHisto(this->GetName()+"_EVeto_NHits",evt->EVetoRecoEvent->GetNHits());
    //hSvc->FillHisto(this->GetName()+"_HEPVeto_NHits",evt->HEPVetoRecoEvent->GetNHits());
    hSvc->FillHisto(this->GetName()+"_SAC_NHits",evt->SACRecoEvent->GetNHits());

    Int_t NclusECal = evt->ECalRecoCl->GetNElements();
    Int_t NclusECalML = evt->ECalMLRecoCl->GetNElements();
    Int_t NclusPVeto = evt->PVetoRecoCl->GetNElements();
    Int_t NclusEVeto = evt->EVetoRecoCl->GetNElements();
    //Int_t NclusHEPVeto = evt->HEPVetoRecoCl->GetNElements();
    Int_t NclusSAC = evt->SACRecoCl->GetNElements();

    hSvc->FillHisto(this->GetName()+"_ECal_NClusters",NclusECal);
    hSvc->FillHisto(this->GetName()+"_ECalML_NClusters",NclusECalML);
    hSvc->FillHisto(this->GetName()+"_PVeto_NClusters",NclusPVeto);
    hSvc->FillHisto(this->GetName()+"_EVeto_NClusters",NclusEVeto);
    //hSvc->FillHisto(this->GetName()+"_HEPVeto_NClusters",NclusHEPVeto);
    hSvc->FillHisto(this->GetName()+"_SAC_NClusters",NclusSAC);
  
    TRecoVCluster* clu=NULL;
    TRecoVCluster* clu2=NULL;
    TRecoVCluster* clu3=NULL;
    TRecoVCluster* cluML=NULL;
    TRecoVCluster* clu2ML=NULL;
    TRecoVCluster* clu3ML=NULL;
    TRecoVCluster* cluPVeto=NULL;
    TRecoVCluster* cluEVeto=NULL;
    TRecoVCluster* cluHEPVeto=NULL;
    TRecoVCluster* cluSAC=NULL;
 
    Double_t time1=0;
    Double_t time2=0;
    Double_t time3=0;
    Double_t time4=0;
    Double_t time5=0;
    Double_t time6=0;

    Double_t time1_3g=0;
    Double_t time2_3g=0;
    Double_t time3_3g=0;

    Double_t time1_3gML=0;
    Double_t time2_3gML=0;
    Double_t time3_3gML=0;  

    Double_t energy1_3g=0;
    Double_t energy2_3g=0;
    Double_t energy3_3g=0;

    Double_t energy1_3gML=0;
    Double_t energy2_3gML=0;
    Double_t energy3_3gML=0;

    Double_t fSafeEnergyFactor = 0.7;
    Double_t fSafeSpaceMargin = 3.15;

    Double_t RadiusClu=0;
    Double_t RadiusClu2=0;  
    Double_t RadiusClu3=0;    
    Double_t RadiusCluML=0;
    Double_t RadiusCluML2=0;
    Double_t RadiusCluML3=0;
      
    TVector3 posECal;
    TVector3 posECal2;
    TVector3 posECalML;
    
    Double_t Theta1=0;
    Double_t Theta2=0;
    Double_t Phi1=0;
    Double_t Phi2=0;
    Double_t DPhi=0;
    
    Double_t ThetaML1=0;
    Double_t ThetaML2=0;
    Double_t PhiML1=0;
    Double_t PhiML2=0;
    Double_t DPhiML=0;
    Double_t InvMass=0;
    Double_t InvMassML=0;
    
    Double_t MissingMass=0;
    Double_t MissingMassML=0;

    int nclusInTimeML = 0;
    int nPosInTimeML = 0;
    int nHEPPosInTimeML = 0;
    int nElecInTimeML = 0;
    int nSACInTimeML = 0;
    
    int nclusInTime = 0;
    int nPosInTime = 0;
    int nHEPPosInTime = 0;
    int nElecInTime = 0;
    int nSACInTime = 0;

    float clu1Energy=0;
    float clu2Energy=0;

    Double_t COG_GGG_X;
    Double_t COG_GGG_Y;
    Double_t COG_GGG_X_ML;
    Double_t COG_GGG_Y_ML;
    
    
    float BeamEnergy=fGeneralInfo->GetBeamEnergy();

    Double_t deltaE=1.014;//calibration constant for run 2; 1.07 for run3

    //-----------------------------------------ECal-----------------------------------------
    for (Int_t i=0; i<NclusECal; ++i){
      time1 = 0;
      time2 = 0;
      nclusInTime = 0;     
      clu    = evt->ECalRecoCl->Element(i);
      MissingMass = ComputeMissingMass(fGeneralInfo->GetBeamEnergy(),(clu->GetPosition().X()),(clu->GetPosition().Y()),(clu->GetPosition().Z()),(clu->GetEnergy()));
      hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_AllCLusters",MissingMass);
      
      for (Int_t j=0; j<NclusECal; ++j){

     	if(j!=i) {
	  clu2 = evt->ECalRecoCl->Element(j);
	}
	clu1Energy=0;
	clu2Energy=0;
	InvMass=0;
	
     	if(clu&&clu2) {
	  clu1Energy=clu->GetEnergy();
	  clu2Energy=clu2->GetEnergy();
	  hSvc->FillHisto(this->GetName()+"_ECal_2Clusters_Energysum",clu1Energy+clu2Energy);
	  posECal=clu->GetPosition();
	  posECal2=clu2->GetPosition();
	  InvMass=ComputeInvariantMass(clu1Energy,clu2Energy,posECal.X(), posECal2.X(),posECal.Y(), posECal2.Y());
	  hSvc->FillHisto(this->GetName()+"_ECal_2Clusters_InvariantMass",abs(InvMass));
	  time1 = clu->GetTime();
	  time2 = clu2->GetTime();
	  hSvc->FillHisto(this->GetName()+"_ECal_2Clusters_TimeDiff", time2-time1);
	  
	  Theta1=ComputeTheta((clu->GetPosition().X()),(clu->GetPosition().Y()),(clu->GetPosition().Z()),(clu->GetEnergy()));
	  Phi1=ComputePhi((clu->GetPosition().X()),(clu->GetPosition().Y()),(clu->GetPosition().Z()),(clu->GetEnergy()));
	  Theta2=ComputeTheta((clu2->GetPosition().X()),(clu2->GetPosition().Y()),(clu2->GetPosition().Z()),(clu2->GetEnergy()));
	  Phi2=ComputePhi((clu2->GetPosition().X()),(clu2->GetPosition().Y()),(clu2->GetPosition().Z()),(clu2->GetEnergy()));
	  DPhi=ComputeDPhi((clu->GetPosition().X()),(clu->GetPosition().Y()),(clu->GetPosition().Z()),(clu->GetEnergy()),(clu2->GetPosition().X()),(clu2->GetPosition().Y()),(clu2->GetPosition().Z()),(clu2->GetEnergy()));

	  RadiusClu=ComputeRadius(clu->GetPosition().X(),clu->GetPosition().Y() );
	  RadiusClu2=ComputeRadius(clu2->GetPosition().X(),clu2->GetPosition().Y() );
	  
	  hSvc->FillHisto(this->GetName()+"_ECal_2gamma_sumTheta",(Theta1+Theta2));
	  hSvc->FillHisto(this->GetName()+"_ECal_2gamma_DPhi",DPhi);
	  hSvc->FillHisto(this->GetName()+"_ECal_2gamma_DeltaPhi",Phi2-Phi1);
	  hSvc->FillHisto2(this->GetName()+"_ECal_2gamma_sumThetavsDeltaPhi",(Theta1+Theta2),abs(Phi2-Phi1));
	  hSvc->FillHisto2(this->GetName()+"_ECal_2gamma_ThetavsTheta",Theta1,Theta2);
	  hSvc->FillHisto2(this->GetName()+"_ECal_2gamma_PhivsPhi",Phi1,Phi2);
	  hSvc->FillHisto2(this->GetName()+"_ECal_2gamma_EnergyvsTime",((time2+time1)/2),(clu->GetEnergy()+clu2->GetEnergy()));	  
	  hSvc->FillHisto2(this->GetName()+"_ECal_2Clusters_EnergyvsInvMass",clu1Energy+clu2Energy,InvMass);

	  //Investigate the little peaks in time diff
	  //Right peak
	  if(time2-time1>4.7&&time2-time1<10.7){
	    hSvc->FillHisto(this->GetName()+"_ECal_2gamma_RightPeakClusterDistance",ComputeDistance((clu->GetPosition().X()),(clu->GetPosition().Y()),(clu2->GetPosition().X()),(clu2->GetPosition().Y())));
	  }

	  //Left peak
	  if(time2-time1>-8.9&&time2-time1<-2.9){
	    hSvc->FillHisto(this->GetName()+"_ECal_2gamma_LeftPeakClusterDistance",ComputeDistance((clu->GetPosition().X()),(clu->GetPosition().Y()),(clu2->GetPosition().X()),(clu2->GetPosition().Y())));
	  }
	    
	  //Main peak
	  if((abs(time2-time1))<4.3){
	    hSvc->FillHisto(this->GetName()+"_ECal_2gamma_MainPeakClusterDistance",ComputeDistance((clu->GetPosition().X()),(clu->GetPosition().Y()),(clu2->GetPosition().X()),(clu2->GetPosition().Y())));
	  }

	  hSvc->FillHisto2(this->GetName()+"_ECal_2gamma_TimeDiffvsClusterDistance",(time2-time1),ComputeDistance((clu->GetPosition().X()),(clu->GetPosition().Y()),(clu2->GetPosition().X()),(clu2->GetPosition().Y())));

	  //Limit only intime clusters
	  if((abs(time2-time1))<4.3){

	    hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_InTimeClusters",MissingMass);
	    hSvc->FillHisto2(this->GetName()+"_ECal_2Clusters_EnergyvsEnergy",clu->GetEnergy(),clu2->GetEnergy());
	    hSvc->FillHisto(this->GetName()+"_ECal_2Clusters_Energysum_afterTimeLimitation",clu1Energy+clu2Energy);
	    hSvc->FillHisto2(this->GetName()+"_ECal_2gamma_EnergyvsTime_timecut",((time2+time1)/2),(clu->GetEnergy()+clu2->GetEnergy()));
	    hSvc->FillHisto2(this->GetName()+"_ECal_2Clusters_EnergyvsInvMass_afterTimeLimitation",clu1Energy+clu2Energy,InvMass);

	    hSvc->FillHisto(this->GetName()+"_ECal_2gamma_sumTheta_afterTimeCut",(Theta1+Theta2));
	    hSvc->FillHisto(this->GetName()+"_ECal_2gamma_DPhi_afterTimeCut",DPhi);
	    hSvc->FillHisto(this->GetName()+"_ECal_2gamma_DeltaPhi_afterTimeCut",Phi2-Phi1);
	    hSvc->FillHisto2(this->GetName()+"_ECal_2gamma_sumThetavsDeltaPhi_afterTimeCut",(Theta1+Theta2),abs(Phi2-Phi1));

	    //Limit the distance between the clusters
	    if(abs((ComputeDistance((clu->GetPosition().X()),(clu->GetPosition().Y()),(clu2->GetPosition().X()),(clu2->GetPosition().Y()))))>60){
	      
	      hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_InTimeDistanceClusters",MissingMass);
	      hSvc->FillHisto(this->GetName()+"_ECal_2Clusters_Energysum_afterTimeDistanceLimitation",(clu->GetEnergy()+clu2->GetEnergy()));
	      hSvc->FillHisto2(this->GetName()+"_ECal_2gamma_EnergyvsTime_timeDistancecut",((time2+time1)/2),(clu->GetEnergy()+clu2->GetEnergy()));


	      //Limit the minimum and maximum energy
	      if((clu->GetEnergy()<(fGeneralInfo->GetEnergyMax()/fSafeEnergyFactor))&&(clu2->GetEnergy()<(fGeneralInfo->GetEnergyMax()/fSafeEnergyFactor))&&(clu->GetEnergy()>(fGeneralInfo->GetEnergyMin()*fSafeEnergyFactor))&&(clu2->GetEnergy()>(fGeneralInfo->GetEnergyMin()*fSafeEnergyFactor))){

		hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_InTimeDistanceEnergyClusters",MissingMass);
		hSvc->FillHisto(this->GetName()+"_ECal_2Clusters_Energysum_afterTimeDistanceMinMaxEnergyLimitation",(clu->GetEnergy()+clu2->GetEnergy()));
		hSvc->FillHisto2(this->GetName()+"_ECal_2gamma_EnergyvsTime_timeDistanceMinMaxEnergycut",((time2+time1)/2),(clu->GetEnergy()+clu2->GetEnergy()));

		//Limit the minimum and maximum radius
		if((RadiusClu>(fGeneralInfo->GetRadiusMin()-fSafeSpaceMargin))&&(RadiusClu2>(fGeneralInfo->GetRadiusMin()-fSafeSpaceMargin))&&(RadiusClu<(fGeneralInfo->GetRadiusMax()))&&(RadiusClu2<(fGeneralInfo->GetRadiusMax()))){
		  
		  hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_InTimeDistanceEnergyRadiusClusters",MissingMass);
		  hSvc->FillHisto(this->GetName()+"_ECal_2Clusters_Energysum_afterTimeDistanceMinMaxEnergyMinMaxRadiusLimitation",(clu->GetEnergy()+clu2->GetEnergy()));
		  hSvc->FillHisto2(this->GetName()+"_ECal_2gamma_EnergyvsTime_timeDistanceMinMaxEnergyMinMaxRadiuscut",((time2+time1)/2),(clu->GetEnergy()+clu2->GetEnergy()));
	    
		
		  //Limit the sum of the polar angles
		  if((Theta1+Theta2)>0.12&&(Theta1+Theta2)<0.165){
		    hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_InTimeDistanceEnergyRadiusThetaClusters",MissingMass);
		    hSvc->FillHisto(this->GetName()+"_ECal_2Clusters_Energysum_afterTimeDistanceEminmaxRminmaxThetaLimitation",(clu->GetEnergy()+clu2->GetEnergy()));
		    hSvc->FillHisto2(this->GetName()+"_ECal_2gamma_EnergyvsTime_timeDistanceEminmaxRminmaxThetacut",((time2+time1)/2),(clu->GetEnergy()+clu2->GetEnergy()));

		    //Limit the difference in the azimuthal angles
		    if(abs(Phi2-Phi1)>2.8&&abs(Phi2-Phi1)<3.4){
		      hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_InTimeDistanceEnergyRadiusThetaPhiClusters",MissingMass);
		      hSvc->FillHisto(this->GetName()+"_ECal_2Clusters_Energysum_afterTimeDistanceEminmaxRminmaxThetaPhiLimitation",(clu->GetEnergy()+clu2->GetEnergy()));
		      hSvc->FillHisto2(this->GetName()+"_ECal_2gamma_EnergyvsTime_timeDistanceEminmaxRminmaxThetaPhicut",((time2+time1)/2),(clu->GetEnergy()+clu2->GetEnergy()));
		  

		      //Limit invariant mass
		      if((abs(InvMass))>17. && (abs(InvMass))<23.) {
			
			hSvc->FillHistoProf(this->GetName()+"_ECal_2gamma_sumThetavsDeltaPhi_afterallCuts_1ns",((time2+time1)/2),(clu->GetEnergy()+clu2->GetEnergy()));
			hSvc->FillHistoProf(this->GetName()+"_ECal_2gamma_sumThetavsDeltaPhi_afterallCuts_2ns",((time2+time1)/2),(clu->GetEnergy()+clu2->GetEnergy()));
			hSvc->FillHistoProf(this->GetName()+"_ECal_2gamma_sumThetavsDeltaPhi_afterallCuts_5ns",((time2+time1)/2),(clu->GetEnergy()+clu2->GetEnergy()));
			hSvc->FillHistoProf(this->GetName()+"_ECal_2gamma_sumThetavsDeltaPhi_afterallCuts_10ns",((time2+time1)/2),(clu->GetEnergy()+clu2->GetEnergy()));

			//plot the energy sum only in the flat region
			if(((time2+time1)/2)>-100.&&((time2+time1)/2)<-50.){
			  hSvc->FillHisto(this->GetName()+"_ECal_2Clusters_Energysum_flatRegion",(clu->GetEnergy()+clu2->GetEnergy()));
			}//end of flat region
			
			hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_InTimeDistanceEnergyRadiusThetaPhiInvMassClusters",MissingMass);
		
			hSvc->FillHisto(this->GetName()+"_ECal_2Clusters_TimeDiff_2gamma", time2-time1);
		
			hSvc->FillHisto(this->GetName()+"_ECal_2Clusters_Energysum_afterTimeDistanceEminmaxRminmaxThetaPhiInvMassLimitation",(clu->GetEnergy()+clu2->GetEnergy()));
			hSvc->FillHisto2(this->GetName()+"_ECal_2gamma_EnergyvsTime_timeDistanceEminmaxRminmaxThetaPhiInvMasscut",((time2+time1)/2),(clu->GetEnergy()+clu2->GetEnergy()));
			nclusInTime++;
			
		      }//end of invariant mass limit
		    }//end of phi limit
		  }//end of theta limit
		}//end of minimum and maximum radius limit
	      }//end of minimum and maximum energy limit
	    }//end of distance limit
	  }//end of time limit




	  //3gamma events
	  for(Int_t k=0; k<NclusECal; ++k){
	    
	    if(k!=i&&k!=j) {
	      clu3 = evt->ECalRecoCl->Element(k);
	    }


	    if(clu&&clu2&&clu3) {

	      time1_3g = clu->GetTime();
	      time2_3g = clu2->GetTime();
	      time3_3g = clu3->GetTime();
	      
	      energy1_3g=clu->GetEnergy();
	      energy2_3g=clu2->GetEnergy();
	      energy3_3g=clu3->GetEnergy();

	      //clu1 is good, clu2 and clu3 are 1 split
	      if (((abs(time2_3g-time1_3g)<4)&&(abs(time3_3g-time2_3g)>5&&(abs(time3_3g-time2_3g)<10)))){
		if((abs((ComputeDistance((clu->GetPosition().X()),(clu->GetPosition().Y()),(clu2->GetPosition().X()),(clu2->GetPosition().Y()))))>60)&&(abs((ComputeDistance((clu->GetPosition().X()),(clu->GetPosition().Y()),(clu3->GetPosition().X()),(clu3->GetPosition().Y()))))>60)){
		  if((abs((ComputeDistance((clu3->GetPosition().X()),(clu3->GetPosition().Y()),(clu2->GetPosition().X()),(clu2->GetPosition().Y()))))<20)){
		    hSvc->FillHisto(this->GetName()+"_ECal_SplitClusters_EnergysumOfALL",(energy1_3g+energy2_3g+energy3_3g));
		    //if(energy2_3g>energy3_3g){
		    if(abs(time1_3g-time3_3g)>abs(time1_3g-time2_3g)){
		      hSvc->FillHisto(this->GetName()+"_ECal_SplitClusters_EnergysumOf2",(energy1_3g+energy2_3g));
		    }
		    else {
		      hSvc->FillHisto(this->GetName()+"_ECal_SplitClusters_EnergysumOf2",(energy1_3g+energy3_3g));
		    }
		  }
		}
	      }
	      
	      //clu2 is good, clu1 and clu3 are 1 split
	      if(((abs(time3_3g-time2_3g)<4)&&(abs(time3_3g-time1_3g)>5&&(abs(time3_3g-time1_3g)<10)))){
		if((abs((ComputeDistance((clu->GetPosition().X()),(clu->GetPosition().Y()),(clu2->GetPosition().X()),(clu2->GetPosition().Y()))))>60)&&(abs((ComputeDistance((clu2->GetPosition().X()),(clu2->GetPosition().Y()),(clu3->GetPosition().X()),(clu3->GetPosition().Y()))))>60)){
		  if((abs((ComputeDistance((clu3->GetPosition().X()),(clu3->GetPosition().Y()),(clu->GetPosition().X()),(clu->GetPosition().Y()))))<20)){
		    hSvc->FillHisto(this->GetName()+"_ECal_SplitClusters_EnergysumOfALL",(energy1_3g+energy2_3g+energy3_3g));
		    //if(energy1_3g>energy3_3g){
		    if(abs(time2_3g-time3_3g)>abs(time1_3g-time2_3g)){
		      hSvc->FillHisto(this->GetName()+"_ECal_SplitClusters_EnergysumOf2",(energy1_3g+energy2_3g));
		    }
		    else {
		      hSvc->FillHisto(this->GetName()+"_ECal_SplitClusters_EnergysumOf2",(energy2_3g+energy3_3g));
		    }
		  }
		}	
	      }
	      
	      //clu3 is good, clu1 and clu2 are 1 split
	      if(((abs(time3_3g-time1_3g)<4)&&(abs(time2_3g-time1_3g)>5&&(abs(time2_3g-time1_3g)<10)))){
		if((abs((ComputeDistance((clu3->GetPosition().X()),(clu3->GetPosition().Y()),(clu2->GetPosition().X()),(clu2->GetPosition().Y()))))>60)&&(abs((ComputeDistance((clu->GetPosition().X()),(clu->GetPosition().Y()),(clu3->GetPosition().X()),(clu3->GetPosition().Y()))))>60)){
		  if((abs((ComputeDistance((clu2->GetPosition().X()),(clu2->GetPosition().Y()),(clu->GetPosition().X()),(clu->GetPosition().Y()))))<20)){
		    hSvc->FillHisto(this->GetName()+"_ECal_SplitClusters_EnergysumOfALL",(energy1_3g+energy2_3g+energy3_3g));
		    //if(energy1_3g>energy2_3g){
		    if(abs(time2_3g-time3_3g)>abs(time1_3g-time3_3g)){
		      hSvc->FillHisto(this->GetName()+"_ECal_SplitClusters_EnergysumOf2",(energy1_3g+energy3_3g));
		    }
		    else {
		      hSvc->FillHisto(this->GetName()+"_ECal_SplitClusters_EnergysumOf2",(energy2_3g+energy3_3g));
		    }
		  }
		}
	      }
	      
	      hSvc->FillHisto(this->GetName()+"_ECal_3Clusters_Energysum",(energy1_3g+energy2_3g+energy3_3g));
	      RadiusClu3=ComputeRadius(clu3->GetPosition().X(),clu3->GetPosition().Y() );
	      COG_GGG_X=ComputeCOG_GGG(energy1_3g,energy2_3g,energy3_3g,clu->GetPosition().X(),clu2->GetPosition().X(),clu3->GetPosition().X());
	      COG_GGG_Y=ComputeCOG_GGG(energy1_3g,energy2_3g,energy3_3g,clu->GetPosition().Y(),clu2->GetPosition().Y(),clu3->GetPosition().Y());

	      hSvc->FillHisto(this->GetName()+"_ECal_3Clusters_TimeDiff",(((time1_3g+time2_3g)/2)-(time3_3g)));
	      hSvc->FillHisto(this->GetName()+"_ECal_3Clusters_TimeDiff",(((time1_3g+time3_3g)/2)-(time2_3g)));
	      hSvc->FillHisto(this->GetName()+"_ECal_3Clusters_TimeDiff",(((time2_3g+time3_3g)/2)-(time1_3g)));

	      if ((((time1_3g+time2_3g)/2)-(time3_3g))>(((time1_3g+time3_3g)/2)-(time2_3g))&&(((time1_3g+time2_3g)/2)-(time3_3g))>(((time2_3g+time3_3g)/2)-(time1_3g))) hSvc->FillHisto(this->GetName()+"_ECal_3Clusters_TimeDiffMax",abs(((time1_3g+time2_3g)/2)-(time3_3g)));
	      if ((((time1_3g+time3_3g)/2)-(time2_3g))>(((time1_3g+time2_3g)/2)-(time3_3g))&&(((time1_3g+time3_3g)/2)-(time2_3g))>(((time2_3g+time3_3g)/2)-(time1_3g))) hSvc->FillHisto(this->GetName()+"_ECal_3Clusters_TimeDiffMax",abs(((time1_3g+time3_3g)/2)-(time2_3g)));
	      if ((((time2_3g+time3_3g)/2)-(time1_3g))>(((time1_3g+time2_3g)/2)-(time3_3g))&&(((time2_3g+time3_3g)/2)-(time1_3g))>(((time1_3g+time3_3g)/2)-(time2_3g))) hSvc->FillHisto(this->GetName()+"_ECal_3Clusters_TimeDiffMax",abs(((time2_3g+time3_3g)/2)-(time1_3g)));
			      
	      hSvc->FillHisto(this->GetName()+"_ECal_3Clusters_COG_X_all",COG_GGG_X);
	      hSvc->FillHisto(this->GetName()+"_ECal_3Clusters_COG_Y_all",COG_GGG_Y);
	      hSvc->FillHisto2(this->GetName()+"_ECal_3Clusters_COG_XY_all",COG_GGG_X,COG_GGG_Y);
	      
	      //Time limit
	      if((abs(time2_3g-time1_3g)<5)&&(abs(time3_3g-time1_3g)<5)&&(abs(time3_3g-time2_3g)<5)){
		hSvc->FillHisto(this->GetName()+"_ECal_3Clusters_Energysum_InTime",(energy1_3g+energy2_3g+energy3_3g));
		hSvc->FillHisto(this->GetName()+"_ECal_3Clusters_COG_X_InTime",COG_GGG_X);
		hSvc->FillHisto(this->GetName()+"_ECal_3Clusters_COG_Y_InTime",COG_GGG_Y);
		hSvc->FillHisto2(this->GetName()+"_ECal_3Clusters_COG_XY_InTime",COG_GGG_X,COG_GGG_Y);
		
		//Limit the distance between the clusters
		if((abs((ComputeDistance((clu->GetPosition().X()),(clu->GetPosition().Y()),(clu2->GetPosition().X()),(clu2->GetPosition().Y()))))>60)&&(abs((ComputeDistance((clu->GetPosition().X()),(clu->GetPosition().Y()),(clu3->GetPosition().X()),(clu3->GetPosition().Y()))))>60)&&(abs((ComputeDistance((clu2->GetPosition().X()),(clu2->GetPosition().Y()),(clu3->GetPosition().X()),(clu3->GetPosition().Y()))))>60)){
		  hSvc->FillHisto(this->GetName()+"_ECal_3Clusters_Energysum_InTimeDistance",(energy1_3g+energy2_3g+energy3_3g));

		  hSvc->FillHisto(this->GetName()+"_ECal_3Clusters_COG_X_InTimeDistance",COG_GGG_X);
		  hSvc->FillHisto(this->GetName()+"_ECal_3Clusters_COG_Y_InTimeDistance",COG_GGG_Y);
		  hSvc->FillHisto2(this->GetName()+"_ECal_3Clusters_COG_XY_InTimeDistance",COG_GGG_X,COG_GGG_Y);
		  
		  //Limit the minimum and maximum energy
		  if((energy1_3g>50)&&(energy2_3g>50)&&(energy3_3g>50)){
		    hSvc->FillHisto(this->GetName()+"_ECal_3Clusters_Energysum_InTimeDistanceEnergy",(energy1_3g+energy2_3g+energy3_3g));

		    hSvc->FillHisto(this->GetName()+"_ECal_3Clusters_COG_X_InTimeDistanceEnergy",COG_GGG_X);
		    hSvc->FillHisto(this->GetName()+"_ECal_3Clusters_COG_Y_InTimeDistanceEnergy",COG_GGG_Y);
		    hSvc->FillHisto2(this->GetName()+"_ECal_3Clusters_COG_XY_InTimeDistanceEnergy",COG_GGG_X,COG_GGG_Y);

		    //COG Limit
		    if(abs(COG_GGG_X)<50 && abs(COG_GGG_Y)<50){
		      hSvc->FillHisto(this->GetName()+"_ECal_3Clusters_Energysum_InTimeDistanceEnergyCOG",(energy1_3g+energy2_3g+energy3_3g));
		    }//end of COG limit		    
		  }//end of energy limit
		}//end of distance limit
	      }//end of time limit
	    }//end of existence of 3 different clusters

	  }//end of third loop through clusters for 3 gamma

	  
     	}//end of existence of two different clusters
      }//end of second loop through all clusters which searches for 2-gamma
      
      // Single clusters after 2gamma events in ECal ere filtered
      if(nclusInTime == 0){
	hSvc->FillHisto(this->GetName()+"_SingleClustersECal_Energy_after2gamma",clu->GetEnergy());
	posECal=clu->GetPosition();
	RadiusClu=ComputeRadius(posECal.X(),posECal.Y());
	hSvc->FillHisto2(this->GetName()+"_SingleClusters_EnergyvsRadius_after2gamma",clu->GetEnergy(),RadiusClu);
	hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_NoInTimeEnergyInvMassClusters",(BeamEnergy-(clu->GetEnergy())));
      }

      
      time3 = 0;
      nPosInTime = 0;
      
      for (Int_t j=0; j<NclusPVeto; ++j){
        cluPVeto    = evt->PVetoRecoCl->Element(j);
        if(clu&&cluPVeto) {
          time1 = clu->GetTime();
          time3 = cluPVeto->GetTime();
          hSvc->FillHisto(this->GetName()+"_ECal_PVeto_TimeDiff", time3-time1);
          hSvc->FillHisto(this->GetName()+"PVeto_z",cluPVeto->GetPosition().Z());
          hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_EnergyvsZ",clu->GetEnergy(),cluPVeto->GetPosition().Z());
          hSvc->FillHisto(this->GetName()+"PVeto_x",cluPVeto->GetPosition().X());
          hSvc->FillHisto2(this->GetName()+"_ECal_PVeto_EnergyvsX",clu->GetEnergy(),cluPVeto->GetPosition().X());

	  //Time limit for ECal+PVeto
	  if ((abs(time3-time1))<10.){
	    nPosInTime ++;
	  }//End of time limit
        }
      }
      
      //Single clusters after 2gamma and PVeto events in ECal ere filtered
      if(nclusInTime == 0 && nPosInTime == 0){
        hSvc->FillHisto(this->GetName()+"_SingleClustersECal_Energy_after2gammaPVeto",clu->GetEnergy());
        posECal=clu->GetPosition();
        RadiusClu=ComputeRadius(posECal.X(),posECal.Y());
        hSvc->FillHisto2(this->GetName()+"_SingleClusters_EnergyvsRadius_after2gammaPVeto",clu->GetEnergy(),RadiusClu);
        hSvc->FillHisto(this->GetName()+"_ECal_MissingMass_NoInTimeEnergyInvMassClustersPVeto",ComputeMissingMass(fGeneralInfo->GetBeamEnergy(),(clu->GetPosition().X()),(clu->GetPosition().Y()),(clu->GetPosition().Z()),(clu->GetEnergy())));
      }
    }//end of first loop through all ECal clusters
 
    

    //---------------------------------ECalML-------------------------------------------------------------
    
    for (Int_t i=0; i<NclusECalML; ++i){
      
      time1 = 0;
      time2 = 0;
      nclusInTimeML = 0;
      cluML    = evt->ECalMLRecoCl->Element(i);
      MissingMassML = ComputeMissingMass(fGeneralInfo->GetBeamEnergy(),(cluML->GetPosition().X()),(cluML->GetPosition().Y()),(cluML->GetPosition().Z()),(cluML->GetEnergy()*deltaE));
      hSvc->FillHisto(this->GetName()+"_ECalML_MissingMass_AllCLusters",MissingMassML);
      
      posECalML=cluML->GetPosition();
      
      for (Int_t j=0; j<NclusECalML; ++j){
	if(j!=i) {
	  clu2ML = evt->ECalMLRecoCl->Element(j);
	}

	InvMassML=0;
		
	if(cluML&&clu2ML) {
	  
	  hSvc->FillHisto(this->GetName()+"_ECalML_2Clusters_Energysum",(cluML->GetEnergy()*deltaE)+(clu2ML->GetEnergy()*deltaE));
	  
	  InvMassML=ComputeInvariantMass((cluML->GetEnergy()*deltaE),(clu2ML->GetEnergy()*deltaE),(cluML->GetPosition().X()),(clu2ML->GetPosition().X()),(cluML->GetPosition().Y()),(clu2ML->GetPosition().Y()));
	  hSvc->FillHisto(this->GetName()+"_ECalML_2Clusters_InvariantMass",abs(InvMassML));

	  time1 = cluML->GetTime();
	  time2 = clu2ML->GetTime();
	  hSvc->FillHisto(this->GetName()+"_ECalML_2Clusters_TimeDiff", time2-time1);

	  ThetaML1=ComputeTheta((cluML->GetPosition().X()),(cluML->GetPosition().Y()),(cluML->GetPosition().Z()),(cluML->GetEnergy()*deltaE));
	  PhiML1=ComputePhi((cluML->GetPosition().X()),(cluML->GetPosition().Y()),(cluML->GetPosition().Z()),(cluML->GetEnergy()*deltaE));
	  ThetaML2=ComputeTheta((clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y()),(clu2ML->GetPosition().Z()),(clu2ML->GetEnergy()*deltaE));
	  PhiML2=ComputePhi((clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y()),(clu2ML->GetPosition().Z()),(clu2ML->GetEnergy()*deltaE));
	  DPhiML=ComputeDPhi((cluML->GetPosition().X()),(cluML->GetPosition().Y()),(cluML->GetPosition().Z()),(cluML->GetEnergy()*deltaE),(clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y()),(clu2ML->GetPosition().Z()),(clu2ML->GetEnergy()*deltaE));

	  RadiusCluML=ComputeRadius(cluML->GetPosition().X(),cluML->GetPosition().Y() );
	  RadiusCluML2=ComputeRadius(clu2ML->GetPosition().X(),clu2ML->GetPosition().Y() );

	  hSvc->FillHisto(this->GetName()+"_ECalML_2gamma_sumTheta",(ThetaML1+ThetaML2));
	  hSvc->FillHisto(this->GetName()+"_ECalML_2gamma_DPhi",DPhiML);
	  hSvc->FillHisto(this->GetName()+"_ECalML_2gamma_DeltaPhi",PhiML2-PhiML1);
	  hSvc->FillHisto2(this->GetName()+"_ECalML_2gamma_sumThetavsDeltaPhi",(ThetaML1+ThetaML2),abs(PhiML2-PhiML1));
	  hSvc->FillHisto2(this->GetName()+"_ECalML_2gamma_ThetavsTheta",ThetaML1,ThetaML2);
	  hSvc->FillHisto2(this->GetName()+"_ECalML_2gamma_PhivsPhi",PhiML1,PhiML2);	  
	  
	  hSvc->FillHisto2(this->GetName()+"_ECalML_2gamma_EnergyvsTime",((time2+time1)/2),(cluML->GetEnergy()*deltaE+clu2ML->GetEnergy()*deltaE));

	  //Investigate the little peaks in time diff
	  //Right peak
	  if(time2-time1>3.8&&time2-time1<9.6){
	    hSvc->FillHisto(this->GetName()+"_ECalML_2gamma_RightPeakClusterDistance",ComputeDistance((cluML->GetPosition().X()),(cluML->GetPosition().Y()),(clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y())));
	  }

	  //Left peak
	  if(time2-time1>-9.7&&time2-time1<-3.7){
	    hSvc->FillHisto(this->GetName()+"_ECalML_2gamma_LeftPeakClusterDistance",ComputeDistance((cluML->GetPosition().X()),(cluML->GetPosition().Y()),(clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y())));
	  }

	   //LeftandRightPeak
	  if(((time2-time1>3.8&&time2-time1<9.6)||(time2-time1>-9.7&&time2-time1<-3.7))&&((ComputeDistance((cluML->GetPosition().X()),(cluML->GetPosition().Y()),(clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y())))<40)){
	    hSvc->FillHisto2(this->GetName()+"_ECalML_smallpeaks_EnergyvsEnergy",cluML->GetEnergy()*deltaE,clu2ML->GetEnergy()*deltaE);
	    hSvc->FillHisto2(this->GetName()+"_ECalML_smallpeaks_position",(cluML->GetPosition().X()),(cluML->GetPosition().Y()));
	    hSvc->FillHisto2(this->GetName()+"_ECalML_smallpeaks_position",(clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y()));
	    hSvc->FillHisto(this->GetName()+"_ECalML_smallpeaks_radius",RadiusCluML);
	    hSvc->FillHisto(this->GetName()+"_ECalML_smallpeaks_radius",RadiusCluML2);
	  }
	    
	  //Main peak
	  if((abs(time2-time1))<3.9){
	    hSvc->FillHisto(this->GetName()+"_ECalML_2gamma_MainPeakClusterDistance",ComputeDistance((cluML->GetPosition().X()),(cluML->GetPosition().Y()),(clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y())));
	  }

	  hSvc->FillHisto2(this->GetName()+"_ECalML_2gamma_TimeDiffvsClusterDistance",(time2-time1),ComputeDistance((cluML->GetPosition().X()),(cluML->GetPosition().Y()),(clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y())));
	  
	    
	  //Limit only intime clusters
	  if((abs(time2-time1))<3.9){

	    hSvc->FillHisto(this->GetName()+"_ECalML_MissingMass_InTimeClusters",MissingMassML);
	    hSvc->FillHisto2(this->GetName()+"_ECalML_2Clusters_EnergyvsEnergy",(cluML->GetEnergy()*deltaE),(clu2ML->GetEnergy()*deltaE));
	    hSvc->FillHisto(this->GetName()+"_ECalML_2Clusters_Energysum_afterTimeLimitation",(cluML->GetEnergy()*deltaE+clu2ML->GetEnergy()*deltaE));
	    hSvc->FillHisto2(this->GetName()+"_ECalML_2gamma_EnergyvsTime_timecut",((time2+time1)/2),(cluML->GetEnergy()*deltaE+clu2ML->GetEnergy()*deltaE));
	    hSvc->FillHisto2(this->GetName()+"_ECal_2Clusters_EnergyvsInvMass_afterTimeLimitation",(cluML->GetEnergy()*deltaE+clu2ML->GetEnergy()*deltaE),InvMassML);

	    hSvc->FillHisto(this->GetName()+"_ECalML_2gamma_sumTheta_afterTimeCut",(ThetaML1+ThetaML2));
	    hSvc->FillHisto(this->GetName()+"_ECalML_2gamma_DPhi_afterTimeCut",DPhiML);
	    hSvc->FillHisto(this->GetName()+"_ECalML_2gamma_DeltaPhi_afterTimeCut",PhiML2-PhiML1);
	    hSvc->FillHisto2(this->GetName()+"_ECalML_2gamma_sumThetavsDeltaPhi_afterTimeCut",(ThetaML1+ThetaML2),abs(PhiML2-PhiML1));


	    //Limit the distance between the clusters
	    if(abs(ComputeDistance((cluML->GetPosition().X()),(cluML->GetPosition().Y()),(clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y())))>40){
	      hSvc->FillHisto(this->GetName()+"_ECalML_MissingMass_InTimeDistanceClusters",MissingMassML);
	      hSvc->FillHisto(this->GetName()+"_ECalML_2Clusters_Energysum_afterTimeDistanceLimitation",(cluML->GetEnergy()*deltaE+clu2ML->GetEnergy()*deltaE));
	      hSvc->FillHisto2(this->GetName()+"_ECalML_2gamma_EnergyvsTime_timeDistancecut",((time2+time1)/2),(cluML->GetEnergy()*deltaE+clu2ML->GetEnergy()*deltaE));
	    
	      //Limit the minimum and maximum energy
	      if((cluML->GetEnergy()*deltaE<(fGeneralInfo->GetEnergyMax()/fSafeEnergyFactor))&&(clu2ML->GetEnergy()*deltaE<(fGeneralInfo->GetEnergyMax()/fSafeEnergyFactor))&&(cluML->GetEnergy()*deltaE>(fGeneralInfo->GetEnergyMin()*fSafeEnergyFactor))&&(clu2ML->GetEnergy()*deltaE>(fGeneralInfo->GetEnergyMin()*fSafeEnergyFactor))){

		hSvc->FillHisto(this->GetName()+"_ECalML_MissingMass_InTimeDistanceEnergyClusters",MissingMassML);
		hSvc->FillHisto(this->GetName()+"_ECalML_2Clusters_Energysum_afterTimeDistanceMinMaxEnergyLimitation",(cluML->GetEnergy()*deltaE+clu2ML->GetEnergy()*deltaE));
		hSvc->FillHisto2(this->GetName()+"_ECalML_2gamma_EnergyvsTime_timeDistanceMinMaxEnergycut",((time2+time1)/2),(cluML->GetEnergy()*deltaE+clu2ML->GetEnergy()*deltaE));

		//Limit the minimum and maximum radius
		if((RadiusCluML>(fGeneralInfo->GetRadiusMin() - fSafeSpaceMargin))&&(RadiusCluML2>(fGeneralInfo->GetRadiusMin() - fSafeSpaceMargin))&&(RadiusCluML<(fGeneralInfo->GetRadiusMax()))&&(RadiusCluML2<(fGeneralInfo->GetRadiusMax()))){
		  hSvc->FillHisto(this->GetName()+"_ECalML_MissingMass_InTimeDistanceEnergyRadiusClusters",MissingMassML);
		  hSvc->FillHisto(this->GetName()+"_ECalML_2Clusters_Energysum_afterTimeDistanceMinMaxEnergyMinMaxRadiusLimitation",(cluML->GetEnergy()*deltaE+clu2ML->GetEnergy()*deltaE));
		  hSvc->FillHisto2(this->GetName()+"_ECalML_2gamma_EnergyvsTime_timeDistanceMinMaxEnergyMinMaxRadiuscut",((time2+time1)/2),(cluML->GetEnergy()*deltaE+clu2ML->GetEnergy()*deltaE));
	    
		  //Limit the sum of the polar angles
		  if((ThetaML1+ThetaML2)>0.12&&(ThetaML1+ThetaML2)<0.165){
		    hSvc->FillHisto(this->GetName()+"_ECalML_2Clusters_Energysum_afterTimeDistanceMinMaxEnergyMinMaxRadiusThetaLimitation",(cluML->GetEnergy()*deltaE+clu2ML->GetEnergy()*deltaE));
		    hSvc->FillHisto2(this->GetName()+"_ECalML_2gamma_EnergyvsTime_timeDistanceEminmaxRminmaxThetacut",((time2+time1)/2),(cluML->GetEnergy()*deltaE+clu2ML->GetEnergy()*deltaE));
		    hSvc->FillHisto(this->GetName()+"_ECalML_MissingMass_InTimeDistanceEnergyRadiusThetaClusters",MissingMassML);
		     

		    //Limit the difference in the azimuthal angles
		    if(abs(PhiML2-PhiML1)>2.8&&abs(PhiML2-PhiML1)<3.4){
		      hSvc->FillHisto(this->GetName()+"_ECalML_2Clusters_Energysum_afterTimeDistanceMinMaxEnergyMinMaxRadiusThetaPhiLimitation",(cluML->GetEnergy()*deltaE+clu2ML->GetEnergy()*deltaE));
		      hSvc->FillHisto2(this->GetName()+"_ECalML_2gamma_EnergyvsTime_timeDistanceEminmaxRminmaxThetaPhicut",((time2+time1)/2),(cluML->GetEnergy()*deltaE+clu2ML->GetEnergy()*deltaE));
		      hSvc->FillHisto(this->GetName()+"_ECalML_MissingMass_InTimeDistanceEnergyRadiusThetaPhiClusters",MissingMassML);
		     
		      if((abs(InvMass))>17. && (abs(InvMass))<23.) {
		      
			hSvc->FillHisto(this->GetName()+"_ECalML_2Clusters_TimeDiff_2gamma", time2-time1);
		      
			hSvc->FillHistoProf(this->GetName()+"_ECalML_2gamma_sumThetavsDeltaPhi_afterallCuts_1ns",((time2+time1)/2),(cluML->GetEnergy()*deltaE+clu2ML->GetEnergy()*deltaE));
			hSvc->FillHistoProf(this->GetName()+"_ECalML_2gamma_sumThetavsDeltaPhi_afterallCuts_2ns",((time2+time1)/2),(cluML->GetEnergy()*deltaE+clu2ML->GetEnergy()*deltaE));
			hSvc->FillHistoProf(this->GetName()+"_ECalML_2gamma_sumThetavsDeltaPhi_afterallCuts_5ns",((time2+time1)/2),(cluML->GetEnergy()*deltaE+clu2ML->GetEnergy()*deltaE));
			hSvc->FillHistoProf(this->GetName()+"_ECalML_2gamma_sumThetavsDeltaPhi_afterallCuts_10ns",((time2+time1)/2),(cluML->GetEnergy()*deltaE+clu2ML->GetEnergy()*deltaE));

			hSvc->FillHisto(this->GetName()+"_ECalML_MissingMass_InTimeDistanceEnergyRadiusThetaPhiInvMassClusters",MissingMassML);
			hSvc->FillHisto(this->GetName()+"_ECalML_2Clusters_Energysum_afterTimeDistanceMinMaxEnergyMinMaxRadiusThetaPhiInvMassLimitation",(cluML->GetEnergy()*deltaE+clu2ML->GetEnergy()*deltaE));
			hSvc->FillHisto2(this->GetName()+"_ECalML_2gamma_EnergyvsTime_timeDistanceEminmaxRminmaxThetaPhiInvMasscut",((time2+time1)/2),(cluML->GetEnergy()*deltaE+clu2ML->GetEnergy()*deltaE));

			nclusInTimeML++;
			
		      }//end of invariant mass limit
		    }//end of phi limit
		  }//end of theta limit
		}//end of minimum and maximum radius limit
	      }//end of minimum and maximum energy limit
	    }//end of distance limit
	  }//end of time limit
	  
	  //3gamma events
	  for(Int_t k=0; k<NclusECalML; ++k){
	    if(k!=i&&k!=j) {
	      clu3ML = evt->ECalMLRecoCl->Element(k);
	    }


	    if(cluML&&clu2ML&&clu3ML) {

	      time1_3gML = cluML->GetTime();
	      time2_3gML = clu2ML->GetTime();
	      time3_3gML = clu3ML->GetTime();
	      
	      energy1_3gML=cluML->GetEnergy();
	      energy2_3gML=clu2ML->GetEnergy();
	      energy3_3gML=clu3ML->GetEnergy();

	      //clu1 is good, clu2 and clu3 are 1 split
	      if (((abs(time2_3gML-time1_3gML)<4)&&(abs(time3_3gML-time2_3gML)>5&&(abs(time3_3gML-time2_3gML)<10)))){
		if((abs((ComputeDistance((cluML->GetPosition().X()),(cluML->GetPosition().Y()),(clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y()))))>60)&&(abs((ComputeDistance((cluML->GetPosition().X()),(cluML->GetPosition().Y()),(clu3ML->GetPosition().X()),(clu3ML->GetPosition().Y()))))>60)){
		  if((abs((ComputeDistance((clu3ML->GetPosition().X()),(clu3ML->GetPosition().Y()),(clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y()))))<20)){
		    hSvc->FillHisto(this->GetName()+"_ECalML_SplitClusters_EnergysumOfALL",(energy1_3gML+energy2_3gML+energy3_3gML));
		    //if(energy2_3gML>energy3_3gML){
		    if(abs(time1_3gML-time3_3gML)>abs(time1_3gML-time2_3gML)){
		      hSvc->FillHisto(this->GetName()+"_ECalML_SplitClusters_EnergysumOf2",(energy1_3gML+energy2_3gML));
		      if((energy1_3gML+energy2_3gML)>340&&(energy1_3gML+energy2_3gML)<440){
			hSvc->FillHisto2(this->GetName()+"_ECalML_splitClusters_position",(clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y()));
			hSvc->FillHisto2(this->GetName()+"_ECalML_splitClusters_position",(clu3ML->GetPosition().X()),(clu3ML->GetPosition().Y()));
		      }
		    }
		    else {
		      hSvc->FillHisto(this->GetName()+"_ECalML_SplitClusters_EnergysumOf2",(energy1_3gML+energy3_3gML));
		      if((energy1_3gML+energy3_3gML)>340&&(energy1_3gML+energy3_3gML)<440){
			hSvc->FillHisto2(this->GetName()+"_ECalML_splitClusters_position",(clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y()));
			hSvc->FillHisto2(this->GetName()+"_ECalML_splitClusters_position",(clu3ML->GetPosition().X()),(clu3ML->GetPosition().Y()));
		      }
		    }
		  }
		}
	      }
	      
	      //clu2 is good, clu1 and clu3 are 1 split
	      if(((abs(time3_3gML-time2_3gML)<4)&&(abs(time3_3gML-time1_3gML)>5&&(abs(time3_3gML-time1_3gML)<10)))){
		if((abs((ComputeDistance((cluML->GetPosition().X()),(cluML->GetPosition().Y()),(clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y()))))>60)&&(abs((ComputeDistance((clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y()),(clu3ML->GetPosition().X()),(clu3ML->GetPosition().Y()))))>60)){
		  if((abs((ComputeDistance((clu3ML->GetPosition().X()),(clu3ML->GetPosition().Y()),(cluML->GetPosition().X()),(cluML->GetPosition().Y()))))<20)){
		    hSvc->FillHisto(this->GetName()+"_ECalML_SplitClusters_EnergysumOfALL",(energy1_3gML+energy2_3gML+energy3_3gML));
		    // if(energy1_3gML>energy3_3gML){
		    if(abs(time2_3gML-time3_3gML)>abs(time1_3gML-time2_3gML)){
		      hSvc->FillHisto(this->GetName()+"_ECalML_SplitClusters_EnergysumOf2",(energy1_3gML+energy2_3gML));
		      if((energy1_3gML+energy2_3gML)>340&&(energy1_3gML+energy2_3gML)<440){
			hSvc->FillHisto2(this->GetName()+"_ECalML_splitClusters_position",(cluML->GetPosition().X()),(cluML->GetPosition().Y()));
			hSvc->FillHisto2(this->GetName()+"_ECalML_splitClusters_position",(clu3ML->GetPosition().X()),(clu3ML->GetPosition().Y()));
		      }
		    }
		    else {
		      hSvc->FillHisto(this->GetName()+"_ECalML_SplitClusters_EnergysumOf2",(energy2_3gML+energy3_3gML));
		      if((energy2_3gML+energy3_3gML)>340&&(energy2_3gML+energy3_3gML)<440){
			hSvc->FillHisto2(this->GetName()+"_ECalML_splitClusters_position",(cluML->GetPosition().X()),(cluML->GetPosition().Y()));
			hSvc->FillHisto2(this->GetName()+"_ECalML_splitClusters_position",(clu3ML->GetPosition().X()),(clu3ML->GetPosition().Y()));
		      }
		    }
		  }
		}
		
	      }
	      
	      //clu3 is good, clu1 and clu2 are 1 split
	      if(((abs(time3_3gML-time1_3gML)<4)&&(abs(time2_3gML-time1_3gML)>5&&(abs(time2_3gML-time1_3gML)<10)))){
		if((abs((ComputeDistance((clu3ML->GetPosition().X()),(clu3ML->GetPosition().Y()),(clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y()))))>60)&&(abs((ComputeDistance((cluML->GetPosition().X()),(cluML->GetPosition().Y()),(clu3ML->GetPosition().X()),(clu3ML->GetPosition().Y()))))>60)){
		  if((abs((ComputeDistance((clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y()),(cluML->GetPosition().X()),(cluML->GetPosition().Y()))))<20)){
		    hSvc->FillHisto(this->GetName()+"_ECalML_SplitClusters_EnergysumOfALL",(energy1_3gML+energy2_3gML+energy3_3gML));
		    //if(energy1_3gML>energy2_3gML){
		    if(abs(time2_3gML-time3_3gML)>abs(time1_3gML-time3_3gML)){
		      hSvc->FillHisto(this->GetName()+"_ECalML_SplitClusters_EnergysumOf2",(energy1_3gML+energy3_3gML));
		       if((energy1_3gML+energy3_3gML)>340&&(energy1_3gML+energy3_3gML)<440){
			hSvc->FillHisto2(this->GetName()+"_ECalML_splitClusters_position",(clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y()));
			hSvc->FillHisto2(this->GetName()+"_ECalML_splitClusters_position",(cluML->GetPosition().X()),(cluML->GetPosition().Y()));
		      }
		    }
		    else {
		      hSvc->FillHisto(this->GetName()+"_ECalML_SplitClusters_EnergysumOf2",(energy2_3gML+energy3_3gML));
		       if((energy2_3gML+energy3_3gML)>340&&(energy2_3gML+energy3_3gML)<440){
			hSvc->FillHisto2(this->GetName()+"_ECalML_splitClusters_position",(clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y()));
			hSvc->FillHisto2(this->GetName()+"_ECalML_splitClusters_position",(cluML->GetPosition().X()),(cluML->GetPosition().Y()));
		      }
		    }
		  }
		}
	      }
	      
	      hSvc->FillHisto(this->GetName()+"_ECalML_3Clusters_Energysum",(energy1_3gML+energy2_3gML+energy3_3gML));

	      RadiusCluML3=ComputeRadius(clu3ML->GetPosition().X(),clu3ML->GetPosition().Y() );
	      COG_GGG_X_ML=ComputeCOG_GGG(energy1_3gML,energy2_3gML,energy3_3gML,cluML->GetPosition().X(),clu2ML->GetPosition().X(),clu3ML->GetPosition().X());
	      COG_GGG_Y_ML=ComputeCOG_GGG(energy1_3gML,energy2_3gML,energy3_3gML,cluML->GetPosition().Y(),clu2ML->GetPosition().Y(),clu3ML->GetPosition().Y());

	      hSvc->FillHisto(this->GetName()+"_ECalML_3Clusters_TimeDiff",(((time1_3gML+time2_3gML)/2)-(time3_3gML)));
	      hSvc->FillHisto(this->GetName()+"_ECalML_3Clusters_TimeDiff",(((time1_3gML+time3_3gML)/2)-(time2_3gML)));
	      hSvc->FillHisto(this->GetName()+"_ECalML_3Clusters_TimeDiff",(((time2_3gML+time3_3gML)/2)-(time1_3gML)));

	      if ((((time1_3gML+time2_3gML)/2)-(time3_3gML))>(((time1_3gML+time3_3gML)/2)-(time2_3gML))&&(((time1_3gML+time2_3gML)/2)-(time3_3gML))>(((time2_3gML+time3_3gML)/2)-(time1_3gML))) hSvc->FillHisto(this->GetName()+"_ECalML_3Clusters_TimeDiffMax",abs(((time1_3gML+time2_3gML)/2)-(time3_3gML)));
	      if ((((time1_3gML+time3_3gML)/2)-(time2_3gML))>(((time1_3gML+time2_3gML)/2)-(time3_3gML))&&(((time1_3gML+time3_3gML)/2)-(time2_3gML))>(((time2_3gML+time3_3gML)/2)-(time1_3gML))) hSvc->FillHisto(this->GetName()+"_ECalML_3Clusters_TimeDiffMax",abs(((time1_3gML+time3_3gML)/2)-(time2_3gML)));
	      if ((((time2_3gML+time3_3gML)/2)-(time1_3gML))>(((time1_3gML+time2_3gML)/2)-(time3_3gML))&&(((time2_3gML+time3_3gML)/2)-(time1_3gML))>(((time1_3gML+time3_3gML)/2)-(time2_3gML))) hSvc->FillHisto(this->GetName()+"_ECalML_3Clusters_TimeDiffMax",abs(((time2_3gML+time3_3gML)/2)-(time1_3gML)));

	      hSvc->FillHisto(this->GetName()+"_ECalML_3Clusters_COG_X_all",COG_GGG_X_ML);
	      hSvc->FillHisto(this->GetName()+"_ECalML_3Clusters_COG_Y_all",COG_GGG_Y_ML);
	      hSvc->FillHisto2(this->GetName()+"_ECalML_3Clusters_COG_XY_all",COG_GGG_X_ML,COG_GGG_Y_ML);
	      
	      //Time limit
	      if((abs(time2_3gML-time1_3gML)<5)&&(abs(time3_3gML-time1_3gML)<5)&&(abs(time3_3gML-time2_3gML)<5)){
		hSvc->FillHisto(this->GetName()+"_ECalML_3Clusters_Energysum_InTime",(energy1_3gML+energy2_3gML+energy3_3gML));
		hSvc->FillHisto(this->GetName()+"_ECalML_3Clusters_COG_X_InTime",COG_GGG_X_ML);
		hSvc->FillHisto(this->GetName()+"_ECalML_3Clusters_COG_Y_InTime",COG_GGG_Y_ML);
		hSvc->FillHisto2(this->GetName()+"_ECalML_3Clusters_COG_XY_InTime",COG_GGG_X_ML,COG_GGG_Y_ML);

		//Limit the distance between the clusters
		if((abs((ComputeDistance((cluML->GetPosition().X()),(cluML->GetPosition().Y()),(clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y()))))>60)&&(abs((ComputeDistance((cluML->GetPosition().X()),(cluML->GetPosition().Y()),(clu3ML->GetPosition().X()),(clu3ML->GetPosition().Y()))))>60)&&(abs((ComputeDistance((clu2ML->GetPosition().X()),(clu2ML->GetPosition().Y()),(clu3ML->GetPosition().X()),(clu3ML->GetPosition().Y()))))>60)){
		  hSvc->FillHisto(this->GetName()+"_ECalML_3Clusters_Energysum_InTimeDistance",(energy1_3gML+energy2_3gML+energy3_3gML));

		  hSvc->FillHisto(this->GetName()+"_ECalML_3Clusters_COG_X_InTimeDistance",COG_GGG_X_ML);
		  hSvc->FillHisto(this->GetName()+"_ECalML_3Clusters_COG_Y_InTimeDistance",COG_GGG_Y_ML);
		  hSvc->FillHisto2(this->GetName()+"_ECalML_3Clusters_COG_XY_InTimeDistance",COG_GGG_X_ML,COG_GGG_Y_ML);

		  //Limit the minimum and maximum energy
		  if((energy1_3gML>50)&&(energy2_3gML>50)&&(energy3_3gML>50)){
		    hSvc->FillHisto(this->GetName()+"_ECalML_3Clusters_Energysum_InTimeDistanceEnergy",(energy1_3gML+energy2_3gML+energy3_3gML));

		    hSvc->FillHisto(this->GetName()+"_ECalML_3Clusters_COG_X_InTimeDistanceEnergy",COG_GGG_X_ML);
		    hSvc->FillHisto(this->GetName()+"_ECalML_3Clusters_COG_Y_InTimeDistanceEnergy",COG_GGG_Y_ML);
		    hSvc->FillHisto2(this->GetName()+"_ECalML_3Clusters_COG_XY_InTimeDistanceEnergy",COG_GGG_X_ML,COG_GGG_Y_ML);

		    //COG Limit
		    if(abs(COG_GGG_X_ML)<50 && abs(COG_GGG_Y_ML)<50){
		      hSvc->FillHisto(this->GetName()+"_ECalML_3Clusters_Energysum_InTimeDistanceEnergyCOG",(energy1_3gML+energy2_3gML+energy3_3gML));
		    }//end of COG limit
		  }//end of energy limit
		}//end of distance limit
	      }//end of time limit
	    }//end of existence of 3 different clusters
	  }//end of third loop through clusters for 3 gamma
	}//end of existence of 2 clusters
      }//end of second loop through all ECal clusters in search for 2 gamma
    
      //Single clusters after 2gamma events in ECalML ere filtered
      if(nclusInTimeML == 0){
	hSvc->FillHisto(this->GetName()+"_SingleClustersECalML_Energy_after2gamma",cluML->GetEnergy());
	posECalML=cluML->GetPosition();
	RadiusCluML=ComputeRadius(posECalML.X(),posECalML.Y());
	hSvc->FillHisto2(this->GetName()+"_SingleClustersML_EnergyvsRadius_after2gamma",cluML->GetEnergy(),RadiusCluML);
      }
	
      time3 = 0;	
      nPosInTimeML = 0;
      for (Int_t j=0; j<NclusPVeto; ++j){
	cluPVeto    = evt->PVetoRecoCl->Element(j);
	if(cluML&&cluPVeto) {
	  time1 = cluML->GetTime();
	  time3 = cluPVeto->GetTime();
	  hSvc->FillHisto(this->GetName()+"_ECalML_PVeto_TimeDiff", time3-time1);
	  hSvc->FillHisto2(this->GetName()+"_ECalML_PVeto_EnergyvsEnergy",cluML->GetEnergy(),cluPVeto->GetPosition().Z());

	  //Time limit for ECalML+PVeto
	  if ((abs(time3-time1))<10.){
	    nPosInTimeML ++;
	  }//End of time limit
	}
      }
    
      //Single clusters after 2gamma and PVeto events in ECalML ere filtered
      if(nclusInTimeML == 0 && nPosInTimeML == 0){
	hSvc->FillHisto(this->GetName()+"_SingleClustersECalML_Energy_after2gammaPVeto",cluML->GetEnergy());
	
	RadiusCluML=ComputeRadius(posECalML.X(),posECalML.Y());
	hSvc->FillHisto2(this->GetName()+"_SingleClustersML_EnergyvsRadius_after2gammaPVeto",cluML->GetEnergy(),RadiusCluML);
      }
   
    }//end of loop through all ECalML clusters
             
  fResult = true;
  return fResult;
    
  }
  
  else {
    return true;
  }
  
}
