#include "SPA_BackgroundCuts.hh"
#include "HistoSvc.hh"
#include "GeneralInfo.hh"
#include "TPVetoMCHit.hh"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TMath.h"
#include "TRandom.h"
#include <fstream>
#include <unistd.h>
#include <iostream>

static SPA_BackgroundCuts ThisAnalyser("SPA_BackgroundCuts");

SPA_BackgroundCuts::SPA_BackgroundCuts(const char *s, int valid, int verb)
    : PadmeVAnalyser(s, valid, verb)
{
  // filecoord = "gamma_gamma_coord_" + std::to_string(getpid()) + ".txt";
  // fcoordout.open(filecoord);
}

SPA_BackgroundCuts::SPA_BackgroundCuts(const char *s) : PadmeVAnalyser(s)
{
  filecoord = "gamma_gamma_coord_" + std::to_string(getpid()) + ".txt";
  fcoordout.open(filecoord);
}

Bool_t SPA_BackgroundCuts::InitHistos()
{

  HistoSvc *hSvcVal = HistoSvc::GetInstance();
  hSvcVal->makeFileDir(GetName());

  // Int_t nGG_scan[11*11*11];

  // Target

  hSvcVal->BookHisto("Target_NPots", 50000, 0.0, 50000.0);
  hSvcVal->BookHisto("Target_X", 50, -25.0, 25.0);
  hSvcVal->BookHisto("Target_Y", 50, -25.0, 25.0);
  hSvcVal->BookHisto2("Target_XY", 50, -25.0, 25.0, 50, -25.0, 25.0);

  // PVeto only histograms for all clusters
  hSvcVal->BookHisto("PVeto_NHits", 1000, 0.0, 1000.0);
  hSvcVal->BookHisto("PVeto_NClusters", 100, 0.0, 100.0);
  hSvcVal->BookHisto("PVeto_Hit_Energy", 100, 0.0, 50.0);
  hSvcVal->BookHisto("PVeto_Cluster_Energy", 500, 0.0, 500.0);
  hSvcVal->BookHisto("PVeto_Cluster_Time", 800, -400.0, 400.0);
  hSvcVal->BookHisto2("PVeto_Cluster_Time_Energy", 800, -400.0, 400.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto("PVeto_Cluster_Z", 86, -447.0, 500.0);
  hSvcVal->BookHisto("PVeto_Cluster_ChID", 100, 0.0, 100.0);
  hSvcVal->BookHisto2("PVeto_Cluster_Time_Z", 800, -400.0, 400.0, 86, -447.0, 500.0);
  hSvcVal->BookHisto2("PVeto_Cluster_Time_ChID", 800, -400.0, 400.0, 100, 0.0, 100.0);
  hSvcVal->BookHisto2("PVeto_Cluster_Energy_Z", 500, 0.0, 500.0, 86, -447.0, 500.0);
  hSvcVal->BookHisto2("PVeto_Cluster_Energy_ChID", 500, 0.0, 500.0, 100, 0.0, 100.0);

  // EVeto only histograms for all clusters
  hSvcVal->BookHisto("EVeto_NHits", 100, 0.0, 100.0);
  hSvcVal->BookHisto("EVeto_NClusters", 100, 0.0, 100.0);
  hSvcVal->BookHisto("EVeto_Hit_Energy", 500, 0.0, 500.0);
  hSvcVal->BookHisto("EVeto_Cluster_Energy", 500, 0.0, 500.0);
  hSvcVal->BookHisto("EVeto_Cluster_Time", 800, -400.0, 400.0);
  hSvcVal->BookHisto2("EVeto_Cluster_Time_Energy", 800, -400.0, 400.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto("EVeto_Cluster_Z", 86, -447.0, 500.0);
  hSvcVal->BookHisto("EVeto_Cluster_ChID", 100, 0.0, 100.0);
  hSvcVal->BookHisto2("EVeto_Cluster_Time_Z", 800, -400.0, 400.0, 86, -447.0, 500.0);
  hSvcVal->BookHisto2("EVeto_Cluster_Time_ChID", 800, -400.0, 400.0, 100, 0.0, 100.0);
  hSvcVal->BookHisto2("EVeto_Cluster_Energy_Z", 500, 0.0, 500.0, 86, -447.0, 500.0);
  hSvcVal->BookHisto2("EVeto_Cluster_Energy_ChID", 500, 0.0, 500.0, 100, 0.0, 100.0);

  // HEPVeto only histograms for all clusters
  hSvcVal->BookHisto("HEPVeto_NHits", 100, 0.0, 100.0);
  hSvcVal->BookHisto("HEPVeto_NClusters", 100, 0.0, 100.0);
  hSvcVal->BookHisto("HEPVeto_Hit_Energy", 500, 0.0, 500.0);
  hSvcVal->BookHisto("HEPVeto_Cluster_Energy", 500, 0.0, 500.0);
  hSvcVal->BookHisto("HEPVeto_Cluster_Time", 800, -400.0, 400.0);
  hSvcVal->BookHisto2("HEPVeto_Cluster_Time_Energy", 800, -400.0, 400.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto("HEPVeto_Cluster_X", 32, 789.0, 949.0);
  hSvcVal->BookHisto("HEPVeto_Cluster_ChID", 16, 0.0, 16.0);
  hSvcVal->BookHisto2("HEPVeto_Cluster_Time_X", 800, -400.0, 400.0, 32, 789.0, 949.0);
  hSvcVal->BookHisto2("HEPVeto_Cluster_Time_ChID", 800, -400.0, 400.0, 16, 0.0, 16.0);
  hSvcVal->BookHisto2("HEPVeto_Cluster_Energy_X", 500, 0.0, 500.0, 32, 789.0, 949.0);
  hSvcVal->BookHisto2("HEPVeto_Cluster_Energy_ChID", 500, 0.0, 500.0, 16, 0.0, 16.0);

  // SAC only histograms for all clusters
  hSvcVal->BookHisto("SAC_NHits", 1000, 0.0, 1000.0);
  hSvcVal->BookHisto("SAC_NClusters", 500, 0.0, 500.0);
  hSvcVal->BookHisto("SAC_Hit_Energy", 500, 0.0, 500.0);
  hSvcVal->BookHisto("SAC_Cluster_Energy", 700, 0.0, 700.0);
  hSvcVal->BookHisto("SAC_Cluster_Time", 800, -400.0, 400.0);
  hSvcVal->BookHisto2("SAC_Cluster_Time_Energy", 800, -400.0, 400.0, 700, 0.0, 700.0);

  // ECal only histograms
  hSvcVal->BookHisto("ECal_NHits", 700, 0.0, 700.0);
  hSvcVal->BookHisto("ECal_NClusters", 100, 0.0, 100.0);
  hSvcVal->BookHisto("ECal_Hit_Energy", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ECal_Cluster_Energy", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ECal_Cluster_Time", 800, -400.0, 400.0);
  hSvcVal->BookHisto2("ECal_Cluster_Time_Energy", 800, -400.0, 400.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto("ECal_Cluster_Radius", 800, -400.0, 400.0);
  hSvcVal->BookHisto("ECal_Cluster_Theta", 200, 0.0, 0.2);
  hSvcVal->BookHisto("ECal_Cluster_Phi", 1000, -50.0, 50.0);
  hSvcVal->BookHisto("ECal_Cluster_X", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("ECal_Cluster_Y", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("ECal_Cluster_Z", 1000, -5000.0, 5000.0);
  hSvcVal->BookHisto2("ECal_Cluster_XY", 1000, -500.0, 500.0, 1000, -500.0, 500.0);
  hSvcVal->BookHisto2("ECal_Cluster_Energy_Theta", 500, 0.0, 500.0, 200, 0.0, 0.2);
  hSvcVal->BookHisto2("ECal_Cluster_Energy_Phi", 500, 0.0, 500.0, 1000, -50.0, 50.0);
  hSvcVal->BookHisto2("ECal_Cluster_Energy_Radius", 500, 0.0, 500.0, 400, 0.0, 400.0);

  // ECal isolation
  hSvcVal->BookHisto("ECal_2clu_DeltaT", 800, -400.0, 400.0);
  hSvcVal->BookHisto("ECal_2clu_DeltaT_closest", 800, -400.0, 400.0);

  // ECal gamma gamma
  hSvcVal->BookHisto("GammaGamma_EnergySum", 500, 0.0, 500.0);
  hSvcVal->BookHisto("GammaGamma_EnergySum_0_pi/3.0", 500, 0.0, 500.0);
  hSvcVal->BookHisto("GammaGamma_EnergySum_pi/3.0_2.0pi/3.0", 500, 0.0, 500.0);
  hSvcVal->BookHisto("GammaGamma_EnergySum_2.0pi/3.0_pi", 500, 0.0, 500.0);
  hSvcVal->BookHisto("GammaGamma_EnergySum_0_-pi/3.0", 500, 0.0, 500.0);
  hSvcVal->BookHisto("GammaGamma_EnergySum_-pi/3.0_-2.0pi/3.0", 500, 0.0, 500.0);
  hSvcVal->BookHisto("GammaGamma_EnergySum_-2.0pi/3.0_-pi", 500, 0.0, 500.0);
  hSvcVal->BookHisto("GammaGamma_SumTheta", 1000, -5.0, 5.0);
  hSvcVal->BookHisto("GammaGamma_DeltaPhi", 200, -10.0, 10.0);
  hSvcVal->BookHisto2("GammaGamma_SumTheta_DeltaPhi", 1000, -5.0, 5.0, 200, -10.0, 10.0);
  hSvcVal->BookHisto("GammaGamma_NClusters", 1000, 0.0, 1000.0);
  hSvcVal->BookHisto("GammaGamma_COGX", 1200, -600.0, 600.0);
  hSvcVal->BookHisto("GammaGamma_COGY", 1200, -600.0, 600.0);
  hSvcVal->BookHisto2("GammaGamma_COG", 1200, -600.0, 600.0, 1200, -600.0, 600.0);
  hSvcVal->BookHisto("GammaGamma_X", 1200, -600.0, 600.0);
  hSvcVal->BookHisto("GammaGamma_Y", 1200, -600.0, 600.0);
  hSvcVal->BookHisto2("GammaGamma_XY", 1200, -600.0, 600.0, 1200, -600.0, 600.0);
  hSvcVal->BookHisto2("GammaGamma_XX", 1200, -600.0, 600.0, 1200, -600.0, 600.0);
  hSvcVal->BookHisto2("GammaGamma_YY", 1200, -600.0, 600.0, 1200, -600.0, 600.0);
  hSvcVal->BookHisto2("GammaGamma_EE", 500, 0.0, 500.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto("GammaGamma_dE", 1200, -600.0, 600.0);
  hSvcVal->BookHisto("GammaGamma_dX", 1200, -600.0, 600.0);
  hSvcVal->BookHisto("GammaGamma_dY", 1200, -600.0, 600.0);
  hSvcVal->BookHisto2("GammaGamma_dXdY", 1200, -600.0, 600.0, 1200, -600.0, 600.0);
  hSvcVal->BookHisto("GammaGamma_EnergySumSel", 500, 0.0, 500.0);
  hSvcVal->BookHisto("GammaGamma_MinvariantSel", 500, 0.0, 500.0);
  hSvcVal->BookHisto("GammaGamma_EnergySumSel-MinvariantSel", 500, -250.0, 250.0);
  hSvcVal->BookHistoProf("GammaGamma_EnergySum_X", 1200, -600.0, 600.0, 0.0, 500.0);

  hSvcVal->BookHistoProf("GammaGamma_EnergySum_Y", 1200, -600.0, 600.0, 0.0, 500.0);

  hSvcVal->BookHistoProf("GammaGamma_EnergySum_Phi", 200, -10.0, 10.0, 0.0, 500.0);
  hSvcVal->BookHistoProf("GammaGamma_EnergySum_E", 500, 0.0, 500.0, 0.0, 500.0);

  hSvcVal->BookHisto2("GammaGamma_ELab_EReco", 500, 0.0, 500.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("GammaGamma_ThetaLab_Radius", 100, 0.0, 4.0, 500, 0.0, 500.0);


  // PVeto+EVeto
  hSvcVal->BookHisto("PVeto_EVeto_DeltaT", 800, -400.0, 400.0);
  hSvcVal->BookHisto("PVeto_EVeto_DeltaT_closest", 800, -400.0, 400.0);
  hSvcVal->BookHisto("Golden_PVeto_EVeto_TimeDiff", 800, -400.0, 400.0);

  // SAC+PVeto for fitting
  hSvcVal->BookHisto("SAC_PVeto_TimeDiff", 800, -400.0, 400.0);
  hSvcVal->BookHisto("SAC_PVeto_TimeDiff_closest", 800, -400.0, 400.0);
  hSvcVal->BookHisto2("SAC_PVeto_Z_vs_TimeDiff", 86, -447.0, 500.0, 800, -400.0, 400.0);
  hSvcVal->BookHisto2("SAC_PVeto_ChID_vs_TimeDiff_detail", 100.0, 0.0, 100.0, 1000, -20.0, 20.0);

  hSvcVal->BookHisto2("SACEnergy_PVetoZ", 700, 0.0, 700.0, 86, -447.0, 500.0);
  hSvcVal->BookHisto2("SACEnergy_PVetoChID", 700, 0.0, 700.0, 100, 0.0, 100.0);
  hSvcVal->BookHisto2("SACEnergy_PVetoZ_inTime", 700, 0.0, 700.0, 86, -447.0, 500.0);
  hSvcVal->BookHisto2("SACEnergy_PVetoZ_inTime_banan", 700, 0.0, 700.0, 86, -447.0, 500.0);
  hSvcVal->BookHisto2("SACEnergy_PVetoChID_inTime", 700, 0.0, 700.0, 100, 0.0, 100.0);
  hSvcVal->BookHisto2("SACEnergy_PVetoChID_inTime_banan", 700, 0.0, 700.0, 100, 0.0, 100.0);
  hSvcVal->BookHisto2("PVetoEnergy_PVetoChID_inTime_banan", 500, 0.0, 500.0, 100, 0.0, 100.0);

  hSvcVal->BookHisto2("PVetoZ_SACEnergy", 86, -447.0, 500.0, 700, 0.0, 700.0);
  hSvcVal->BookHisto2("PVetoZ_SACEnergy_inTime", 86, -447.0, 500.0, 700, 0.0, 700.0);
  hSvcVal->BookHisto2("PVetoZ_SACEnergy_inTime_banan", 86, -447.0, 500.0, 700, 0.0, 700.0);
  hSvcVal->BookHisto2("PVetoChID_SACEnergy", 100, 0.0, 100.0, 700, 0.0, 700.0);
  hSvcVal->BookHisto2("PVetoChID_SACEnergy_inTime", 100, 0.0, 100.0, 700, 0.0, 700.0);
  hSvcVal->BookHisto2("PVetoChID_SACEnergy_inTime_banan", 100, 0.0, 100.0, 700, 0.0, 700.0);
  hSvcVal->BookHisto2("PVetoChID_PVetoEnergy_inTime_banan", 100, 0.0, 100.0, 500, 0.0, 500.0);

  // SAC+HEPVeto for fitting
  hSvcVal->BookHisto("SAC_HEPVeto_TimeDiff", 800, -400.0, 400.0);
  hSvcVal->BookHisto("SAC_HEPVeto_TimeDiff_closest", 800, -400.0, 400.0);
  hSvcVal->BookHisto2("SAC_HEPVeto_X_vs_TimeDiff", 16, 789.0, 949.0, 800, -400.0, 400.0);
  hSvcVal->BookHisto2("SAC_HEPVeto_ChID_vs_TimeDiff_detail", 17, 0.0, 17.0, 1000, -20.0, 20.0);

  hSvcVal->BookHisto2("SACEnergy_HEPVetoX", 700, 0.0, 700.0, 16, 789.0, 949.0);
  hSvcVal->BookHisto2("SACEnergy_HEPVetoChID", 700, 0.0, 700.0, 17, 0.0, 17.0);
  hSvcVal->BookHisto2("SACEnergy_HEPVetoX_inTime", 700, 0.0, 700.0, 16, 789.0, 949.0);
  hSvcVal->BookHisto2("SACEnergy_HEPVetoX_inTime_banan", 700, 0.0, 700.0, 16, 789.0, 949.0);
  hSvcVal->BookHisto2("SACEnergy_HEPVetoChID_inTime", 700, 0.0, 700.0, 17, 0.0, 17.0);
  hSvcVal->BookHisto2("SACEnergy_HEPVetoChID_inTime_banan", 700, 0.0, 700.0, 17, 0.0, 17.0);
  hSvcVal->BookHisto2("HEPVetoEnergy_HEPVetoChID_inTime_banan", 500, 0.0, 500.0, 17, 0.0, 17.0);

  hSvcVal->BookHisto2("HEPVetoX_SACEnergy", 16, 789.0, 949.0, 700, 0.0, 700.0);
  hSvcVal->BookHisto2("HEPVetoX_SACEnergy_inTime", 16, 789.0, 949.0, 700, 0.0, 700.0);
  hSvcVal->BookHisto2("HEPVetoX_SACEnergy_inTime_banan", 16, 789.0, 949.0, 700, 0.0, 700.0);
  hSvcVal->BookHisto2("HEPVetoChID_SACEnergy", 17, 0.0, 17.0, 700, 0.0, 700.0);
  hSvcVal->BookHisto2("HEPVetoChID_SACEnergy_inTime", 17, 0.0, 17.0, 700, 0.0, 700.0);
  hSvcVal->BookHisto2("HEPVetoChID_SACEnergy_inTime_banan", 17, 0.0, 17.0, 700, 0.0, 700.0);
  hSvcVal->BookHisto2("HEPVetoChID_HEPVetoEnergy_inTime_banan", 17, 0.0, 17.0, 500, 0.0, 500.0);

  // SAC+EVeto for checking
  hSvcVal->BookHisto("SAC_EVeto_TimeDiff", 800, -400.0, 400.0);
  hSvcVal->BookHisto("SAC_EVeto_TimeDiff_closest", 800, -400.0, 400.0);
  hSvcVal->BookHisto2("SAC_EVeto_Z_vs_TimeDiff", 86, -447.0, 500.0, 800, -400.0, 400.0);
  hSvcVal->BookHisto2("SAC_EVeto_ChID_vs_TimeDiff_detail", 100, 0.0, 100.0, 1000, -20.0, 20.0);

  hSvcVal->BookHisto2("SACEnergy_EVetoZ", 700, 0.0, 700.0, 86, -447.0, 500.0);
  hSvcVal->BookHisto2("SACEnergy_EVetoChID", 700, 0.0, 700.0, 100, 0.0, 100.0);
  hSvcVal->BookHisto2("EVetoZ_SACEnergy", 86, -447.0, 500.0, 700, 0.0, 700.0);
  hSvcVal->BookHisto2("EVetoChID_SACEnergy", 100, 0.0, 100.0, 700, 0.0, 700.0);

  hSvcVal->BookHisto2("SACEnergy_EVetoZ_inTime", 700, 0.0, 700.0, 86, -447.0, 500.0);
  hSvcVal->BookHisto2("SACEnergy_EVetoChID_inTime", 700, 0.0, 700.0, 100, 0.0, 100.0);
  hSvcVal->BookHisto2("EVetoZ_SACEnergy_inTime", 86, -447.0, 500.0, 700, 0.0, 700.0);
  hSvcVal->BookHisto2("EVetoChID_SACEnergy_inTime", 100, 0.0, 100.0, 700, 0.0, 700.0);

  //ECal+SAC for checking
  hSvcVal->BookHisto("ECal_SAC_DeltaT", 800, -400.0, 400.0);
  hSvcVal->BookHisto("ECal_SAC_EnergySum_1ns", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ECal_SAC_EnergySum_2ns", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ECal_SAC_EnergySum_5ns", 500, 0.0, 500.0);
  
  // ECal+PVeto for checking
  hSvcVal->BookHisto("ECal_PVeto_TimeDiff", 800, -400.0, 400.0);
  hSvcVal->BookHisto("ECal_PVeto_TimeDiff_closest", 800, -400.0, 400.0);
  hSvcVal->BookHisto2("ECal_PVeto_Z_vs_TimeDiff", 86, -447.0, 500.0, 800, -400.0, 400.0);
  hSvcVal->BookHisto2("ECal_PVeto_ChID_vs_TimeDiff_detail", 100, 0.0, 100.0, 1000, -20.0, 20.0);

  hSvcVal->BookHisto2("ECalEnergy_PVetoZ", 500, 0.0, 500.0, 86, -447.0, 500.0);
  hSvcVal->BookHisto2("ECalEnergy_PVetoChID", 500, 0.0, 500.0, 100, 0.0, 100.0);
  hSvcVal->BookHisto2("ECalEnergy_PVetoZ_inTime", 500, 0.0, 500.0, 86, -447.0, 500.0);
  hSvcVal->BookHisto2("ECalEnergy_PVetoChID_random", 500, 0.0, 500.0, 100, 0.0, 100.0);
  hSvcVal->BookHisto2("ECalEnergy_PVetoChID_inTime", 500, 0.0, 500.0, 100, 0.0, 100.0);

  hSvcVal->BookHisto2("PVetoZ_ECalEnergy", 86, -447.0, 500.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("PVetoZ_ECalEnergy_inTime", 86, -447.0, 500.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("PVetoChID_ECalEnergy", 100, 0.0, 100.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("PVetoChID_ECalEnergy_inTime", 100, 0.0, 100.0, 500, 0.0, 500.0);

  hSvcVal->BookHisto2("ECalEnergy_PVetoZ_1clu", 500, 0.0, 500.0, 86, -447.0, 500.0);
  hSvcVal->BookHisto2("ECalEnergy_PVetoZ_inTime_1clu", 500, 0.0, 500.0, 86, -447.0, 500.0);

  hSvcVal->BookHisto2("PVetoZ_ECalEnergy_inTime_Cut", 86, -447.0, 500.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("ECalEnergy_PVetoChID_inTime_Cut", 500, 0.0, 500.0, 100, 0.0, 100.0);

  hSvcVal->BookHisto2("PVetoZ_ECalEnergy_inTime_afterCut", 86, -447.0, 500.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("ECalEnergy_PVetoChID_inTime_afterCut", 500, 0.0, 500.0, 100, 0.0, 100.0);

  // ECal+HEPVeto for checking
  hSvcVal->BookHisto("ECal_HEPVeto_TimeDiff", 800, -400.0, 400.0);
  hSvcVal->BookHisto("ECal_HEPVeto_TimeDiff_closest", 800, -400.0, 400.0);
  hSvcVal->BookHisto2("ECal_HEPVeto_X_vs_TimeDiff", 16, 789.0, 949.0, 800, -400.0, 400.0);
  hSvcVal->BookHisto2("ECal_HEPVeto_ChID_vs_TimeDiff_detail", 17, 0.0, 17.0, 1000, -20.0, 20.0);

  hSvcVal->BookHisto2("ECalEnergy_HEPVetoX", 500, 0.0, 500.0, 16, 789.0, 949.0);
  hSvcVal->BookHisto2("ECalEnergy_HEPVetoChID", 500, 0.0, 500.0, 17, 0.0, 17.0);
  hSvcVal->BookHisto2("ECalEnergy_HEPVetoX_inTime", 500, 0.0, 500.0, 16, 789.0, 949.0);
  hSvcVal->BookHisto2("ECalEnergy_HEPVetoChID_inTime", 500, 0.0, 500.0, 17, 0.0, 17.0);
  hSvcVal->BookHisto2("ECalEnergy_HEPVetoChID_random", 500, 0.0, 500.0, 17, 0.0, 17.0);

  hSvcVal->BookHisto2("HEPVetoX_ECalEnergy", 16, 789.0, 949.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("HEPVetoX_ECalEnergy_inTime", 16, 789.0, 949.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("HEPVetoChID_ECalEnergy", 17, 0.0, 17.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("HEPVetoChID_ECalEnergy_inTime", 17, 0.0, 17.0, 500, 0.0, 500.0);

  hSvcVal->BookHisto2("ECalEnergy_HEPVetoX_1clu", 500, 0.0, 500.0, 16, 789.0, 949.0);
  hSvcVal->BookHisto2("ECalEnergy_HEPVetoX_inTime_1clu", 500, 0.0, 500.0, 16, 789.0, 949.0);

  hSvcVal->BookHisto2("HEPVetoX_ECalEnergy_inTime_Cut", 16, 789.0, 949.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("ECalEnergy_HEPVetoChID_inTime_Cut", 500, 0.0, 500.0, 17, 0.0, 17.0);

  hSvcVal->BookHisto2("HEPVetoX_ECalEnergy_inTime_afterCut", 16, 789.0, 949.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("ECalEnergy_HEPVetoChID_inTime_afterCut", 500, 0.0, 500.0, 17, 0.0, 17.0);

  // ECal+EVeto for checking

  hSvcVal->BookHisto("ECal_EVeto_TimeDiff", 800, -400.0, 400.0);
  hSvcVal->BookHisto("ECal_EVeto_TimeDiff_closest", 800, -400.0, 400.0);
  hSvcVal->BookHisto2("ECal_EVeto_Z_vs_TimeDiff", 86, -447.0, 500.0, 800, -400.0, 400.0);
  hSvcVal->BookHisto2("ECal_EVeto_ChID_vs_TimeDiff_detail", 100, 0.0, 100.0, 1000, -20.0, 20.0);

  hSvcVal->BookHisto2("ECalEnergy_EVetoZ", 500, 0.0, 500.0, 86, -447.0, 500.0);
  hSvcVal->BookHisto2("ECalEnergy_EVetoChID", 500, 0.0, 500.0, 100, 0.0, 100.0);
  hSvcVal->BookHisto2("ECalEnergy_EVetoZ_inTime", 500, 0.0, 500.0, 86, -447.0, 500.0);
  hSvcVal->BookHisto2("ECalEnergy_EVetoChID_inTime", 500, 0.0, 500.0, 100, 0.0, 100.0);
  hSvcVal->BookHisto2("ECalEnergy_EVetoChID_random", 500, 0.0, 500.0, 100, 0.0, 100.0);

  hSvcVal->BookHisto2("EVetoZ_ECalEnergy", 86, -447.0, 500.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("EVetoZ_ECalEnergy_inTime", 86, -447.0, 500.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("EVetoChID_ECalEnergy", 100, 0.0, 100.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("EVetoChID_ECalEnergy_inTime", 100, 0.0, 100.0, 500, 0.0, 500.0);

  // ECal+PVeto+EVeto for checking
  hSvcVal->BookHisto2("ECal_PVeto_EVeto_TimeDiff", 800, -400.0, 400.0, 800, -400.0, 400.0);
  hSvcVal->BookHisto("ECal_PVeto_EVeto_ChIDDiff", 200, -100.0, 100.0);

// ClusterEnergy
  hSvcVal->BookHisto("ClusterEnergy_All", 500, 0.0, 500.0);

  hSvcVal->BookHisto("ClusterEnergy_Ecut30MeV", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut50MeV", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut60MeV", 500, 0.0, 500.0);

  hSvcVal->BookHisto("ClusterEnergy_Ecut30MeV_GeomCut", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut50MeV_GeomCut", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut60MeV_GeomCut", 500, 0.0, 500.0);

  hSvcVal->BookHisto("ClusterEnergy_Ecut30MeV_GeomCut_1nsIsolation", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut50MeV_GeomCut_1nsIsolation", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut60MeV_GeomCut_1nsIsolation", 500, 0.0, 500.0);

  hSvcVal->BookHisto("ClusterEnergy_Ecut30MeV_GeomCut_2nsIsolation", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut50MeV_GeomCut_2nsIsolation", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut60MeV_GeomCut_2nsIsolation", 500, 0.0, 500.0);

  hSvcVal->BookHisto("ClusterEnergy_Ecut30MeV_GeomCut_5nsIsolation", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut50MeV_GeomCut_5nsIsolation", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut60MeV_GeomCut_5nsIsolation", 500, 0.0, 500.0);

  hSvcVal->BookHisto("ClusterEnergy_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTime", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTime", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTime", 500, 0.0, 500.0);

  hSvcVal->BookHisto("ClusterEnergy_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 500, 0.0, 500.0);

  hSvcVal->BookHisto("ClusterEnergy_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 500, 0.0, 500.0);

  hSvcVal->BookHisto("ClusterEnergy_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 500, 0.0, 500.0);

  hSvcVal->BookHisto("ClusterEnergy_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 500, 0.0, 500.0);

  hSvcVal->BookHisto("ClusterEnergy_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ClusterEnergy_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 500, 0.0, 500.0);

  // MissingMass
  hSvcVal->BookHisto("MissingMass_All", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMass_Ecut30MeV", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut50MeV", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut60MeV", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMass_Ecut30MeV_GeomCut", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut50MeV_GeomCut", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut60MeV_GeomCut", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMass_Ecut30MeV_GeomCut_1nsIsolation", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut50MeV_GeomCut_1nsIsolation", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut60MeV_GeomCut_1nsIsolation", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMass_Ecut30MeV_GeomCut_2nsIsolation", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut50MeV_GeomCut_2nsIsolation", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut60MeV_GeomCut_2nsIsolation", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMass_Ecut30MeV_GeomCut_5nsIsolation", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut50MeV_GeomCut_5nsIsolation", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut60MeV_GeomCut_5nsIsolation", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMass_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTime", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTime", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTime", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMass_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMass_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMass_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMass_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMass_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMass_Special", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMass_Special_inChID", 1000, -500.0, 500.0);
  hSvcVal->BookHisto2("MissingMass_Special_inChID_ChIDDiff", 1000, -500.0, 500.0, 200, -100.0, 100.0);

  // MissingMomentum
  hSvcVal->BookHisto("MissingMomentum_All", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMomentum_Ecut30MeV", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut50MeV", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut60MeV", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMomentum_Ecut30MeV_GeomCut", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut50MeV_GeomCut", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut60MeV_GeomCut", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMomentum_Ecut30MeV_GeomCut_1nsIsolation", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut50MeV_GeomCut_1nsIsolation", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut60MeV_GeomCut_1nsIsolation", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMomentum_Ecut30MeV_GeomCut_2nsIsolation", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut50MeV_GeomCut_2nsIsolation", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut60MeV_GeomCut_2nsIsolation", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMomentum_Ecut30MeV_GeomCut_5nsIsolation", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut50MeV_GeomCut_5nsIsolation", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut60MeV_GeomCut_5nsIsolation", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMomentum_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTime", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTime", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTime", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMomentum_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMomentum_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMomentum_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMomentum_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMomentum_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 1000, -500.0, 500.0);

  hSvcVal->BookHisto("MissingMomentum_Special", 1000, -500.0, 500.0);
  hSvcVal->BookHisto("MissingMomentum_Special_inChID", 1000, -500.0, 500.0);
  hSvcVal->BookHisto2("MissingMomentum_Special_inChID_ChIDDiff", 1000, -500.0, 500.0, 200, -100.0, 100.0);

  // MissingMomentum Theta
  hSvcVal->BookHisto("MissingTheta_All", 1000, 0.0, 0.25);

  hSvcVal->BookHisto("MissingTheta_Ecut30MeV", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut50MeV", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut60MeV", 1000, 0.0, 0.25);

  hSvcVal->BookHisto("MissingTheta_Ecut30MeV_GeomCut", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut50MeV_GeomCut", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut60MeV_GeomCut", 1000, 0.0, 0.25);

  hSvcVal->BookHisto("MissingTheta_Ecut30MeV_GeomCut_1nsIsolation", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut50MeV_GeomCut_1nsIsolation", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut60MeV_GeomCut_1nsIsolation", 1000, 0.0, 0.25);

  hSvcVal->BookHisto("MissingTheta_Ecut30MeV_GeomCut_2nsIsolation", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut50MeV_GeomCut_2nsIsolation", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut60MeV_GeomCut_2nsIsolation", 1000, 0.0, 0.25);

  hSvcVal->BookHisto("MissingTheta_Ecut30MeV_GeomCut_5nsIsolation", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut50MeV_GeomCut_5nsIsolation", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut60MeV_GeomCut_5nsIsolation", 1000, 0.0, 0.25);

  hSvcVal->BookHisto("MissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTime", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTime", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTime", 1000, 0.0, 0.25);

  hSvcVal->BookHisto("MissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 1000, 0.0, 0.25);

  hSvcVal->BookHisto("MissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 1000, 0.0, 0.25);

  hSvcVal->BookHisto("MissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 1000, 0.0, 0.25);

  hSvcVal->BookHisto("MissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 1000, 0.0, 0.25);

  hSvcVal->BookHisto("MissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 1000, 0.0, 0.25);
  hSvcVal->BookHisto("MissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 1000, 0.0, 0.25);

  // MissingMass vs. theta
  hSvcVal->BookHisto2("MissingMassTheta_All", 1000, -500.0, 500.0, 1000, 0.0, 0.25);

  hSvcVal->BookHisto2("MissingMassTheta_Ecut30MeV", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut50MeV", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut60MeV", 1000, -500.0, 500.0, 1000, 0.0, 0.25);

  hSvcVal->BookHisto2("MissingMassTheta_Ecut30MeV_GeomCut", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut50MeV_GeomCut", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut60MeV_GeomCut", 1000, -500.0, 500.0, 1000, 0.0, 0.25);

  hSvcVal->BookHisto2("MissingMassTheta_Ecut30MeV_GeomCut_1nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut50MeV_GeomCut_1nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut60MeV_GeomCut_1nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 0.25);

  hSvcVal->BookHisto2("MissingMassTheta_Ecut30MeV_GeomCut_2nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut50MeV_GeomCut_2nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut60MeV_GeomCut_2nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 0.25);

  hSvcVal->BookHisto2("MissingMassTheta_Ecut30MeV_GeomCut_5nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut50MeV_GeomCut_5nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut60MeV_GeomCut_5nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 0.25);

  hSvcVal->BookHisto2("MissingMassTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTime", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTime", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTime", 1000, -500.0, 500.0, 1000, 0.0, 0.25);

  hSvcVal->BookHisto2("MissingMassTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 1000, -500.0, 500.0, 1000, 0.0, 0.25);

  hSvcVal->BookHisto2("MissingMassTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 1000, -500.0, 500.0, 1000, 0.0, 0.25);

  hSvcVal->BookHisto2("MissingMassTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 1000, -500.0, 500.0, 1000, 0.0, 0.25);

  hSvcVal->BookHisto2("MissingMassTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 1000, -500.0, 500.0, 1000, 0.0, 0.25);

  hSvcVal->BookHisto2("MissingMassTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  hSvcVal->BookHisto2("MissingMassTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 1000, -500.0, 500.0, 1000, 0.0, 0.25);

  // MissingMass vs. radius
  hSvcVal->BookHisto2("MissingMassRadius_All", 1000, -500.0, 500.0, 1000, 0.0, 500);

  hSvcVal->BookHisto2("MissingMassRadius_Ecut30MeV", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut50MeV", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut60MeV", 1000, -500.0, 500.0, 1000, 0.0, 500);

  hSvcVal->BookHisto2("MissingMassRadius_Ecut30MeV_GeomCut", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut50MeV_GeomCut", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut60MeV_GeomCut", 1000, -500.0, 500.0, 1000, 0.0, 500);

  hSvcVal->BookHisto2("MissingMassRadius_Ecut30MeV_GeomCut_1nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut50MeV_GeomCut_1nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut60MeV_GeomCut_1nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 500);

  hSvcVal->BookHisto2("MissingMassRadius_Ecut30MeV_GeomCut_2nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut50MeV_GeomCut_2nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut60MeV_GeomCut_2nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 500);

  hSvcVal->BookHisto2("MissingMassRadius_Ecut30MeV_GeomCut_5nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut50MeV_GeomCut_5nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut60MeV_GeomCut_5nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 500);

  hSvcVal->BookHisto2("MissingMassRadius_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTime", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTime", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTime", 1000, -500.0, 500.0, 1000, 0.0, 500);

  hSvcVal->BookHisto2("MissingMassRadius_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 1000, -500.0, 500.0, 1000, 0.0, 500);

  hSvcVal->BookHisto2("MissingMassRadius_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 1000, -500.0, 500.0, 1000, 0.0, 500);

  hSvcVal->BookHisto2("MissingMassRadius_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 1000, -500.0, 500.0, 1000, 0.0, 500);

  hSvcVal->BookHisto2("MissingMassRadius_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 1000, -500.0, 500.0, 1000, 0.0, 500);

  hSvcVal->BookHisto2("MissingMassRadius_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 1000, -500.0, 500.0, 1000, 0.0, 500);
  hSvcVal->BookHisto2("MissingMassRadius_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 1000, -500.0, 500.0, 1000, 0.0, 500);

  // MissingMomentumMissingTheta
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_All", 1000, -500.0, 500.0, 1000, 0.0, 4.0);

  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut30MeV", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut50MeV", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut60MeV", 1000, -500.0, 500.0, 1000, 0.0, 4.0);

  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut30MeV_GeomCut", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut50MeV_GeomCut", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut60MeV_GeomCut", 1000, -500.0, 500.0, 1000, 0.0, 4.0);

  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut30MeV_GeomCut_1nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut50MeV_GeomCut_1nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut60MeV_GeomCut_1nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 4.0);

  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut30MeV_GeomCut_2nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut50MeV_GeomCut_2nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut60MeV_GeomCut_2nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 4.0);

  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut30MeV_GeomCut_5nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut50MeV_GeomCut_5nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut60MeV_GeomCut_5nsIsolation", 1000, -500.0, 500.0, 1000, 0.0, 4.0);

  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTime", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTime", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTime", 1000, -500.0, 500.0, 1000, 0.0, 4.0);

  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", 1000, -500.0, 500.0, 1000, 0.0, 4.0);

  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", 1000, -500.0, 500.0, 1000, 0.0, 4.0);

  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", 1000, -500.0, 500.0, 1000, 0.0, 4.0);

  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", 1000, -500.0, 500.0, 1000, 0.0, 4.0);

  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  hSvcVal->BookHisto2("MissingMomentumMissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", 1000, -500.0, 500.0, 1000, 0.0, 4.0);

  return true;
}

double SPA_BackgroundCuts::ComputeRadius(Double_t PosX1, Double_t PosY1)
{
  Double_t R = sqrt(PosX1 * PosX1 + PosY1 * PosY1);
  return R;
}

double SPA_BackgroundCuts::ComputeTheta(Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma)
{
  Double_t R = sqrt(PosX * PosX + PosY * PosY + PosZ * PosZ);
  double Px = E_gamma * PosX / R;
  double Py = E_gamma * PosY / R;
  double Pz = E_gamma * PosZ / R;
  TLorentzVector Photon(Px, Py, Pz, E_gamma);
  return Photon.Theta();
}

double SPA_BackgroundCuts::ComputeInvariantMass(Double_t E1, Double_t E2, Double_t PosX1, Double_t PosX2, Double_t PosY1, Double_t PosY2)
{
  Double_t Z = 3536;
  Double_t M = 0;
  Double_t R = sqrt((PosX1 - PosX2) * (PosX1 - PosX2) + (PosY1 - PosY2) * (PosY1 - PosY2));
  M = (E1 * E2 * R * R) / (Z * Z);
  return M;
}

double SPA_BackgroundCuts::ComputePhi(Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma)
{
  Double_t R = sqrt(PosX * PosX + PosY * PosY + PosZ * PosZ);
  double Px = E_gamma * PosX / R;
  double Py = E_gamma * PosY / R;
  double Pz = E_gamma * PosZ / R;
  TLorentzVector Photon(Px, Py, Pz, E_gamma);
  return Photon.Phi();
}

double SPA_BackgroundCuts::ComputeMissingMass(double BeamEnergy, Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma)
{
  double BeamPz = sqrt(BeamEnergy * BeamEnergy - 0.511 * 0.511);
  TLorentzVector Positron(0, 0, BeamPz, BeamEnergy);
  TLorentzVector Electron(0, 0, 0, 0.511);
  Double_t R = sqrt(PosX * PosX + PosY * PosY + PosZ * PosZ);
  double Px = E_gamma * PosX / R;
  double Py = E_gamma * PosY / R;
  double Pz = E_gamma * PosZ / R;
  TLorentzVector Photon(Px, Py, Pz, E_gamma);
  TLorentzVector Missing(0, 0, 0, 0);
  Missing = Positron + Electron - Photon;
  return Missing.M2(); //*Missing;
}

double SPA_BackgroundCuts::ComputeMissingMomentum(double BeamEnergy, Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma)
{
  double BeamPz = sqrt(BeamEnergy * BeamEnergy - 0.511 * 0.511);
  TVector3 Positron(0, 0, BeamPz);
  TVector3 Electron(0, 0, 0);
  Double_t R = sqrt(PosX * PosX + PosY * PosY + PosZ * PosZ);
  double Px = E_gamma * PosX / R;
  double Py = E_gamma * PosY / R;
  double Pz = E_gamma * PosZ / R;

  TVector3 Photon(Px, Py, Pz);
  TVector3 Missing(0, 0, 0);
  Missing = Positron + Electron - Photon;
  return Missing.Mag2(); //*Missing;
}

TVector3 SPA_BackgroundCuts::ComputeMissingMomentumVector(double BeamEnergy, Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma)
{
  double BeamPz = sqrt(BeamEnergy * BeamEnergy - 0.511 * 0.511);
  TVector3 Positron(0, 0, BeamPz);
  TVector3 Electron(0, 0, 0);
  Double_t R = sqrt(PosX * PosX + PosY * PosY + PosZ * PosZ);
  double Px = E_gamma * PosX / R;
  double Py = E_gamma * PosY / R;
  double Pz = E_gamma * PosZ / R;

  TVector3 Photon(Px, Py, Pz);
  TVector3 Missing(0, 0, 0);
  Missing = Positron + Electron - Photon;
  return Missing; //*Missing;
}

double SPA_BackgroundCuts::ComputeCOG(Double_t E_gamma1, Double_t Pos1, Double_t E_gamma2, Double_t Pos2)
{
  Double_t COG = 0;
  COG = (E_gamma1 * Pos1 + E_gamma2 * Pos2) / (E_gamma1 + E_gamma2);
  return COG;
}

TLorentzVector SPA_BackgroundCuts::ComputeCMMomentum(Double_t xt, Double_t yt, Double_t zt, Double_t PosX, Double_t PosY, Double_t PosZ)
{
  Double_t fMe = 0.511;
  Double_t fBeamMomentum = 431.2;
  Double_t sqrts = sqrt(2. * fMe * fMe + 2. * fBeamMomentum * fMe);
  Double_t betagamma = fBeamMomentum / sqrts;
  Double_t gamma = sqrt(betagamma * betagamma + 1.);
  Double_t beta = betagamma / gamma;

  TVector3 cluPos;
  cluPos.SetXYZ(PosX, PosY, 2508.31);
  TVector3 targetPos;
  // targetPos.SetXYZ(0,0,-1028);
  targetPos.SetXYZ(xt, yt, zt);
  TVector3 cluMom = cluPos - targetPos;
  cluMom *= (1. / cluMom.Mag());
  TVector3 COGPos;
  COGPos.SetXYZ(2.1, 0.67, 2508.31);
  TVector3 boost = COGPos - targetPos;
  boost *= (beta / boost.Mag());
  Double_t cosa = cluMom.Dot(boost) / (cluMom.Mag() * boost.Mag());

  Double_t Elab = 0.5 * sqrts / sqrt(1. - cosa * cosa + pow(gamma * cosa, 2) - 2. * betagamma * gamma * cosa + pow(betagamma, 2));
  cluMom *= Elab;
  TLorentzVector clu4Mom;
  clu4Mom.SetVectM(cluMom, 0.);
  TLorentzVector clu4MomCM;
  clu4MomCM.SetVectM(clu4Mom.Vect(), 0);
  clu4MomCM.Boost(-boost);
  return clu4MomCM;
}

int SPA_BackgroundCuts::ComputeSector(Double_t x, Double_t y)
{
  int NphiSectors = 6;
  int NradialSubsectors = 3;
  double rMinCalib = 90.0;
  double rMaxCalib = 270.0;
  double phi = atan2(y, x);
  if (phi < 0)
    phi += 2 * M_PI;
  int sphi = int(phi / (2 * M_PI / NphiSectors));
  sphi = std::clamp(sphi, 0, NphiSectors - 1);

  double r = sqrt(x * x + y * y);
  double rC = std::clamp(r, rMinCalib + 1e-9, rMaxCalib - 1e-9);
  int sr = int((rC - rMinCalib) / ((rMaxCalib - rMinCalib) / NradialSubsectors));
  sr = std::clamp(sr, 0, NradialSubsectors - 1);

  return sphi + NphiSectors * sr;
}

double SPA_BackgroundCuts::CalibrateEnergy(Double_t E_gamma, Double_t x, Double_t y)
{
  double coefficients[19] = {1.00651, 1.02013, 1.0266, 1.01829, 1.01393, 1.02115, 1.02356, 1.00694, 1.01086, 1.00473, 1.00284, 1.0056, 1.00919, 0.998449, 0.9914, 1.00386, 0.991517, 0.985795, 0.990386};
  int c = ComputeSector(x, y);
  return coefficients[0] * coefficients[1 + c] * E_gamma;
}

double SPA_BackgroundCuts::ComputeExpectedEnergy(Double_t PosX, Double_t PosY){
  
  Double_t fMe = 0.511;
  Double_t fBeamMomentum = 431.2;
  Double_t sqrts = sqrt(2. * fMe * fMe + 2. * fBeamMomentum * fMe);
  Double_t betagamma = fBeamMomentum / sqrts;
  Double_t gamma = sqrt(betagamma * betagamma + 1.);
  Double_t beta = betagamma / gamma;

  TVector3 cluPos;
  cluPos.SetXYZ(PosX, PosY, 2508.31);
  TVector3 targetPos;
  targetPos.SetXYZ(-14.3,0.35,-1028);
  //targetPos.SetXYZ(xt, yt, zt);
  TVector3 cluMom = cluPos - targetPos;
  cluMom *= (1. / cluMom.Mag());
  TVector3 COGPos;
  COGPos.SetXYZ(2.1, 0.67, 2508.31);
  TVector3 boost = COGPos - targetPos;
  boost *= (beta / boost.Mag());
  Double_t cosa = cluMom.Dot(boost) / (cluMom.Mag() * boost.Mag());

  Double_t Elab = 0.5 * sqrts / sqrt(1. - cosa * cosa + pow(gamma * cosa, 2) - 2. * betagamma * gamma * cosa + pow(betagamma, 2));

  return Elab;
}

Bool_t SPA_BackgroundCuts::Process()
{
  HistoSvc *hSvc = HistoSvc::GetInstance();
  GeneralInfo *fGeneralInfo = GeneralInfo::GetInstance();
  float BeamEnergy = 431.6;
  Double_t Z_1_U = 3536;
  Double_t fSafeSpaceMargin = 3.15;
  Float_t safeE = 30.;
  Float_t eCalib = 1.0;
  Float_t eCalib1;
  Float_t eCalib2;
  Float_t eCalib3;
  Float_t eCalib4;
  Float_t eCalib5;
  Float_t eCalib6;

  Int_t NclusPVeto = 0;
  Int_t NclusEVeto = 0;
  Int_t NclusHEPVeto = 0;
  Int_t NclusSAC = 0;
  Int_t NclusECal = 0;

  TRecoVHit *hitPVeto = NULL;
  TRecoVCluster *cluPVeto = NULL;
  Float_t cluPVetoEnergy = 0;
  Float_t cluPVetoTime = 0;
  Float_t cluPVetoZ = 0;
  Float_t cluPVetoChID = 0;

  TRecoVHit *hitEVeto = NULL;
  TRecoVCluster *cluEVeto = NULL;
  Float_t cluEVetoEnergy = 0;
  Float_t cluEVetoTime = 0;
  Float_t cluEVetoZ = 0;
  Float_t cluEVetoChID = 0;

  TRecoVHit *hitHEPVeto = NULL;
  TRecoVCluster *cluHEPVeto = NULL;
  Float_t cluHEPVetoEnergy = 0;
  Float_t cluHEPVetoTime = 0;
  Float_t cluHEPVetoX = 0;
  Float_t cluHEPVetoChID = 0;

  TRecoVHit *hitSAC = NULL;
  TRecoVCluster *cluSAC = NULL;
  Float_t cluSACEnergy = 0;
  Float_t cluSACTime = 0;

  Float_t deltaTPVetoSAC = 0;
  Float_t deltaTHEPVetoSAC = 0;
  Float_t deltaTEVetoSAC = 0;

  TRecoVHit *hitECal = NULL;
  TRecoVCluster *cluECal = NULL;
  Float_t cluEnergy = 0;
  Float_t cluTime = 0;
  Float_t cluRadius = 0;
  Float_t cluTheta = 0;
  Float_t cluPhi = 0;
  Float_t cluX = 0;
  Float_t cluY = 0;
  Float_t cluChX = 0;
  Float_t cluChY = 0;
  Float_t cluZ = 0;
  Float_t MissingMass = 0;
  Float_t MissingMomentum = 0;
  TVector3 MissingMomentumV(0, 0, 0);
  Float_t MissingTheta = 0;

  TRecoVCluster *cluECal2 = NULL;
  Float_t clu2Energy = 0;
  Float_t clu2Radius = 0;
  Float_t clu2Theta = 0;
  Float_t clu2Phi = 0;
  Float_t clu2X = 0;
  Float_t clu2Y = 0;
  Float_t clu2ChX = 0;
  Float_t clu2ChY = 0;
  Float_t clu2Z = 0;

  Float_t energySum = 0;
  Float_t Minv = 0;
  Float_t COGX = 0;
  Float_t COGY = 0;

  Float_t eSumMClow = 399.7;
  Float_t eSumMChigh = 454.9;
  Float_t eSumDatalow = 382.5;
  Float_t eSumDatahigh = 463.3;

  Float_t sumTheta = 0;
  Float_t deltaPhi = 0;

  Float_t sumThetaMClow = 0.124;
  Float_t sumThetaMChigh = 0.157;
  Float_t sumThetaDatalow = 3.07;
  Float_t sumThetaDatahigh = 3.21;

  Float_t deltaPhiMClow = 2.88;
  Float_t deltaPhiMChigh = 3.38;
  Float_t deltaPhiDatalow = 2.90;
  Float_t deltaPhiDatahigh = 3.35;

  Int_t nGG = 0;

  Float_t deltaTPVetoEVeto = 0;

  Float_t deltaTECal = 0;
  Int_t intime1ns = 0;
  Int_t intime2ns = 0;
  Int_t intime5ns = 0;

  Float_t deltaTECalSAC = 0;
  Float_t energySumECalSAC = 0;
  Int_t intimeECalSAC1ns = 0;
  Int_t intimeECalSAC2ns = 0;
  Int_t intimeECalSAC5ns = 0;

  Float_t intimePVeto1ns = 0;
  Float_t intimePVeto1ns_inBanan = 0;
  Float_t deltaTECalPVeto = 0;

  // Fit parameters for Bremsstrahlung fit using expected positron energy from beam energy - ECal photon energy and Z position of the cluster in the PVeto
  Float_t alineECalCh = -0.00035;
  Float_t blineECalCh = -0.01;
  Float_t cgoreECalCh = 83.64;
  Float_t cdoluECalCh = 58.07;

  Float_t intimeHEPVeto1ns = 0;
  Float_t intimeHEPVeto1ns_inBanan = 0;
  Float_t deltaTECalHEPVeto = 0;

  // Fit parameters for Bremsstrahlung fit using expected positron energy from beam energy - ECal photon energy and Z position of the cluster in the PVeto
  Float_t alineECalHEPCh = -0.00129;
  Float_t blineECalHEPCh = -0.07;
  Float_t cgoreECalHEPCh = 32.65;
  Float_t cdoluECalHEPCh = 15.66;

  Float_t intimeEVeto1ns = 0;
  Float_t deltaTECalEVeto = 0;

  Float_t bestdP = 0;
  Float_t bestdE = 0;
  Float_t dPE = 0;
  Float_t ChIDDiff = 0;

  TRecoVCluster *GoldenCluPVeto = NULL;
  TRecoVCluster *GoldenCluEVeto = NULL;

  // variables for time difference to closest cluster

  Float_t closestECalECal = 9999;
  Float_t closestPVetoEVeto = 9999;
  Float_t closestSACPVeto = 9999;
  Float_t closestSACHEPVeto = 9999;
  Float_t closestSACEVeto = 9999;
  Float_t closestECalPVeto = 9999;
  Float_t closestECalHEPVeto = 9999;
  Float_t closestECalEVeto = 9999;

  Int_t NPots = 0;
  Float_t Xt = 0;
  Float_t Yt = 0;

  if (evt->TargetRecoEvent)
  {
    NPots = evt->TargetRecoBeam->getnPOT();

    // if(NPots < 27000 || NPots>30000) return true;
    Xt = evt->TargetRecoBeam->getX();
    Yt = evt->TargetRecoBeam->getY();
    hSvc->FillHisto("Target_NPots", NPots);
    hSvc->FillHisto("Target_X", Xt);
    hSvc->FillHisto("Target_Y", Yt);
    hSvc->FillHisto2("Target_XY", Xt, Yt);
  }

  if (evt->PVetoRecoEvent)
  {
    // std::cout<<"PVeto  event!"<<std::endl;
    NclusPVeto = evt->PVetoRecoCl->GetNElements();
    Int_t NhitsPVeto = evt->PVetoRecoEvent->GetNHits();

    hSvc->FillHisto("PVeto_NHits", NhitsPVeto);
    hSvc->FillHisto("PVeto_NClusters", NclusPVeto);

    hitPVeto = NULL;
    for (Int_t i = 0; i < NhitsPVeto; i++)
    {
      hitPVeto = evt->PVetoRecoEvent->Hit(i);
      hSvc->FillHisto("PVeto_Hit_Energy", hitPVeto->GetEnergy());
    }

    cluPVeto = NULL;
    for (Int_t i = 0; i < NclusPVeto; i++)
    {
      cluPVeto = evt->PVetoRecoCl->Element(i);
      hSvc->FillHisto("PVeto_Cluster_Energy", cluPVeto->GetEnergy());
      hSvc->FillHisto("PVeto_Cluster_Time", cluPVeto->GetTime());
      hSvc->FillHisto2("PVeto_Cluster_Time_Energy", cluPVeto->GetTime(), cluPVeto->GetEnergy());
      hSvc->FillHisto("PVeto_Cluster_Z", cluPVeto->GetPosition().Z());
      hSvc->FillHisto("PVeto_Cluster_ChID", cluPVeto->GetChannelId());
      hSvc->FillHisto2("PVeto_Cluster_Time_Z", cluPVeto->GetTime(), cluPVeto->GetPosition().Z());
      hSvc->FillHisto2("PVeto_Cluster_Time_ChID", cluPVeto->GetTime(), cluPVeto->GetChannelId());
      hSvc->FillHisto2("PVeto_Cluster_Energy_Z", cluPVeto->GetEnergy(), cluPVeto->GetPosition().Z());
      hSvc->FillHisto2("PVeto_Cluster_Energy_ChID", cluPVeto->GetEnergy(), cluPVeto->GetChannelId());
      closestPVetoEVeto = 9999.;
      if (evt->EVetoRecoEvent)
      {
        NclusEVeto = evt->EVetoRecoCl->GetNElements();
        for (Int_t j = 0; j < NclusEVeto; j++)
        {
          deltaTPVetoEVeto = 0;
          cluEVeto = evt->EVetoRecoCl->Element(j);
          cluEVetoTime = cluEVeto->GetTime();
          deltaTPVetoEVeto = cluPVetoTime - cluEVetoTime;
          hSvc->FillHisto("PVeto_EVeto_DeltaT", deltaTPVetoEVeto);
          if (abs(deltaTPVetoEVeto) < abs(closestPVetoEVeto))
            closestPVetoEVeto = deltaTPVetoEVeto;
        }
        hSvc->FillHisto("PVeto_EVeto_DeltaT_closest", closestPVetoEVeto);
      }
    }
  }

  // else std::cout<<"Not PVeto  event!"<<std::endl;

  if (evt->EVetoRecoEvent)
  {
    // std::cout<<"EVeto  event!"<<std::endl;
    NclusEVeto = evt->EVetoRecoCl->GetNElements();
    Int_t NhitsEVeto = evt->EVetoRecoEvent->GetNHits();

    hSvc->FillHisto("EVeto_NHits", NhitsEVeto);
    hSvc->FillHisto("EVeto_NClusters", NclusEVeto);

    hitEVeto = NULL;
    for (Int_t i = 0; i < NhitsEVeto; i++)
    {
      hitEVeto = evt->EVetoRecoEvent->Hit(i);
      hSvc->FillHisto("EVeto_Hit_Energy", hitEVeto->GetEnergy());
    }

    cluEVeto = NULL;

    for (Int_t i = 0; i < NclusEVeto; i++)
    {
      cluEVeto = evt->EVetoRecoCl->Element(i);
      hSvc->FillHisto("EVeto_Cluster_Energy", cluEVeto->GetEnergy());
      hSvc->FillHisto("EVeto_Cluster_Time", cluEVeto->GetTime());
      hSvc->FillHisto2("EVeto_Cluster_Time_Energy", cluEVeto->GetTime(), cluEVeto->GetEnergy());
      hSvc->FillHisto("EVeto_Cluster_Z", cluEVeto->GetPosition().Z());
      hSvc->FillHisto("EVeto_Cluster_ChID", cluEVeto->GetChannelId());
      hSvc->FillHisto2("EVeto_Cluster_Time_Z", cluEVeto->GetTime(), cluEVeto->GetPosition().Z());
      hSvc->FillHisto2("EVeto_Cluster_Time_ChID", cluEVeto->GetTime(), cluEVeto->GetChannelId());
      hSvc->FillHisto2("EVeto_Cluster_Energy_Z", cluEVeto->GetEnergy(), cluEVeto->GetPosition().Z());
      hSvc->FillHisto2("EVeto_Cluster_Energy_ChID", cluEVeto->GetEnergy(), cluEVeto->GetChannelId());
    }
  }

  if (evt->HEPVetoRecoEvent)
  {
    // std::cout<<"HEPVeto  event!"<<std::endl;
    NclusHEPVeto = evt->HEPVetoRecoCl->GetNElements();
    Int_t NhitsHEPVeto = evt->HEPVetoRecoEvent->GetNHits();

    hSvc->FillHisto("HEPVeto_NHits", NhitsHEPVeto);
    hSvc->FillHisto("HEPVeto_NClusters", NclusHEPVeto);

    hitHEPVeto = NULL;
    for (Int_t i = 0; i < NhitsHEPVeto; i++)
    {
      hitHEPVeto = evt->HEPVetoRecoEvent->Hit(i);
      hSvc->FillHisto("HEPVeto_Hit_Energy", hitHEPVeto->GetEnergy());
    }

    cluHEPVeto = NULL;
    for (Int_t i = 0; i < NclusHEPVeto; i++)
    {
      cluHEPVeto = evt->HEPVetoRecoCl->Element(i);
      hSvc->FillHisto("HEPVeto_Cluster_Energy", cluHEPVeto->GetEnergy());
      hSvc->FillHisto("HEPVeto_Cluster_Time", cluHEPVeto->GetTime());
      hSvc->FillHisto2("HEPVeto_Cluster_Time_Energy", cluHEPVeto->GetTime(), cluHEPVeto->GetEnergy());
      hSvc->FillHisto("HEPVeto_Cluster_X", cluHEPVeto->GetPosition().X());
      hSvc->FillHisto("HEPVeto_Cluster_ChID", cluHEPVeto->GetChannelId());
      hSvc->FillHisto2("HEPVeto_Cluster_Time_X", cluHEPVeto->GetTime(), cluHEPVeto->GetPosition().X());
      hSvc->FillHisto2("HEPVeto_Cluster_Time_ChID", cluHEPVeto->GetTime(), cluHEPVeto->GetChannelId());
      hSvc->FillHisto2("HEPVeto_Cluster_Energy_X", cluHEPVeto->GetEnergy(), cluHEPVeto->GetPosition().X());
      hSvc->FillHisto2("HEPVeto_Cluster_Energy_ChID", cluHEPVeto->GetEnergy(), cluHEPVeto->GetChannelId());
    }
  }

  if (evt->SACRecoEvent)
  {

    NclusSAC = evt->SACRecoCl->GetNElements();
    Int_t NhitsSAC = evt->SACRecoEvent->GetNHits();

    hSvc->FillHisto("SAC_NHits", NhitsSAC);
    hSvc->FillHisto("SAC_NClusters", NclusSAC);

    hitSAC = NULL;
    for (Int_t i = 0; i < NhitsSAC; i++)
    {
      hitSAC = evt->SACRecoEvent->Hit(i);
      hSvc->FillHisto("SAC_Hit_Energy", hitSAC->GetEnergy());
    }

    cluSAC = NULL;
    for (Int_t i = 0; i < NclusSAC; i++)
    {

      cluSAC = evt->SACRecoCl->Element(i);
      cluSACEnergy = cluSAC->GetEnergy();
      cluSACTime = cluSAC->GetTime();

      hSvc->FillHisto("SAC_Cluster_Energy", cluSACEnergy);
      hSvc->FillHisto("SAC_Cluster_Time", cluSACTime);
      hSvc->FillHisto2("SAC_Cluster_Time_Energy", cluSACTime, cluSACEnergy);

      if (evt->PVetoRecoEvent)
      {
        closestSACPVeto = 9999.;
        for (Int_t j = 0; j < NclusPVeto; ++j)
        {
          cluPVeto = evt->PVetoRecoCl->Element(j);
          cluPVetoTime = cluPVeto->GetTime();
          deltaTPVetoSAC = cluPVetoTime - cluSACTime;
          if (abs(deltaTPVetoSAC) < abs(closestSACPVeto))
            closestSACPVeto = deltaTPVetoSAC;
        }
        hSvc->FillHisto("SAC_PVeto_TimeDiff_closest", closestSACPVeto);

        deltaTPVetoSAC = 0;
        for (Int_t j = 0; j < NclusPVeto; ++j)
        {

          cluPVeto = evt->PVetoRecoCl->Element(j);
          cluPVetoEnergy = cluPVeto->GetEnergy();
          cluPVetoTime = cluPVeto->GetTime();
          cluPVetoZ = cluPVeto->GetPosition().Z();
          cluPVetoChID = cluPVeto->GetChannelId();

          deltaTPVetoSAC = cluPVetoTime - cluSACTime;

          hSvc->FillHisto("SAC_PVeto_TimeDiff", deltaTPVetoSAC);
          hSvc->FillHisto2("SAC_PVeto_Z_vs_TimeDiff", cluPVetoZ, deltaTPVetoSAC);
          if (abs(deltaTPVetoSAC) < 20.0)
            hSvc->FillHisto2("SAC_PVeto_ChID_vs_TimeDiff_detail", cluPVetoChID, deltaTPVetoSAC);
          hSvc->FillHisto2("SACEnergy_PVetoZ", cluSACEnergy, cluPVetoZ);
          hSvc->FillHisto2("SACEnergy_PVetoChID", cluSACEnergy, cluPVetoChID);
          hSvc->FillHisto2("PVetoZ_SACEnergy", cluPVetoZ, cluSACEnergy);
          hSvc->FillHisto2("PVetoChID_SACEnergy", cluPVetoChID, cluSACEnergy);

          if (fabs(deltaTPVetoSAC) < 1.0)
          {
            hSvc->FillHisto2("SACEnergy_PVetoZ_inTime", cluSACEnergy, cluPVetoZ);
            hSvc->FillHisto2("SACEnergy_PVetoChID_inTime", cluSACEnergy, cluPVetoChID);
            hSvc->FillHisto2("PVetoZ_SACEnergy_inTime", cluPVetoZ, cluSACEnergy);
            hSvc->FillHisto2("PVetoChID_SACEnergy_inTime", cluPVetoChID, cluSACEnergy);

            if (cluPVetoZ < 500.0)
            {
              hSvc->FillHisto2("SACEnergy_PVetoZ_inTime_banan", cluSACEnergy, cluPVetoZ);
              hSvc->FillHisto2("SACEnergy_PVetoChID_inTime_banan", cluSACEnergy, cluPVetoChID);
              hSvc->FillHisto2("PVetoZ_SACEnergy_inTime_banan", cluPVetoZ, cluSACEnergy);
              hSvc->FillHisto2("PVetoChID_SACEnergy_inTime_banan", cluPVetoChID, cluSACEnergy);

              hSvc->FillHisto2("PVetoEnergy_PVetoChID_inTime_banan", cluPVetoEnergy, cluPVetoChID);
              hSvc->FillHisto2("PVetoChID_PVetoEnergy_inTime_banan", cluPVetoChID, cluPVetoEnergy);
            }
          }
        }
      }

      if (evt->HEPVetoRecoEvent)
      {
        closestSACHEPVeto = 9999.;
        for (Int_t j = 0; j < NclusHEPVeto; ++j)
        {
          cluHEPVeto = evt->HEPVetoRecoCl->Element(j);
          cluHEPVetoTime = cluHEPVeto->GetTime();
          deltaTHEPVetoSAC = cluHEPVetoTime - cluSACTime;
          if (abs(deltaTHEPVetoSAC) < abs(closestSACHEPVeto))
            closestSACHEPVeto = deltaTHEPVetoSAC;
        }
        hSvc->FillHisto("SAC_HEPVeto_TimeDiff_closest", closestSACHEPVeto);
        deltaTHEPVetoSAC = 0;
        for (Int_t j = 0; j < NclusHEPVeto; ++j)
        {

          cluHEPVeto = evt->HEPVetoRecoCl->Element(j);
          cluHEPVetoEnergy = cluHEPVeto->GetEnergy();
          cluHEPVetoTime = cluHEPVeto->GetTime();
          cluHEPVetoX = cluHEPVeto->GetPosition().X();
          cluHEPVetoChID = cluHEPVeto->GetChannelId();

          deltaTHEPVetoSAC = cluHEPVetoTime - cluSACTime;

          hSvc->FillHisto("SAC_HEPVeto_TimeDiff", deltaTHEPVetoSAC);
          hSvc->FillHisto2("SAC_HEPVeto_X_vs_TimeDiff", cluHEPVetoX, deltaTHEPVetoSAC);
          if (abs(deltaTHEPVetoSAC) < 20.0)
            hSvc->FillHisto2("SAC_HEPVeto_ChID_vs_TimeDiff_detail", cluHEPVetoChID, deltaTHEPVetoSAC);
          hSvc->FillHisto2("SACEnergy_HEPVetoX", cluSACEnergy, cluHEPVetoX);
          hSvc->FillHisto2("SACEnergy_HEPVetoChID", cluSACEnergy, cluHEPVetoChID);
          hSvc->FillHisto2("HEPVetoX_SACEnergy", cluHEPVetoX, cluSACEnergy);
          hSvc->FillHisto2("HEPVetoChID_SACEnergy", cluHEPVetoChID, cluSACEnergy);
          if (fabs(deltaTHEPVetoSAC) < 1.0)
          {
            hSvc->FillHisto2("SACEnergy_HEPVetoX_inTime", cluSACEnergy, cluHEPVetoX);
            hSvc->FillHisto2("SACEnergy_HEPVetoChID_inTime", cluSACEnergy, cluHEPVetoChID);
            hSvc->FillHisto2("HEPVetoX_SACEnergy_inTime", cluHEPVetoX, cluSACEnergy);
            hSvc->FillHisto2("HEPVetoChID_SACEnergy_inTime", cluHEPVetoChID, cluSACEnergy);

            // To be opened of we find anything at all...
            //    if(cluHEPVetoX<500.0) {
            //      hSvc->FillHisto2("SACEnergy_HEPVetoX_inTime_banan",cluSACEnergy,cluHEPVetoX);
            //      hSvc->FillHisto2("SACEnergy_HEPVetoChID_inTime_banan",cluSACEnergy,cluHEPVetoChID);
            //      hSvc->FillHisto2("HEPVetoX_SACEnergy_inTime_banan",cluHEPVetoX,cluSACEnergy);
            //      hSvc->FillHisto2("HEPVetoChID_SACEnergy_inTime_banan",cluHEPVetoChID,cluSACEnergy);

            //     hSvc->FillHisto2("HEPVetoEnergy_HEPVetoChID_inTime_banan",cluHEPVetoEnergy,cluHEPVetoChID);
            //     hSvc->FillHisto2("HEPVetoChID_HEPVetoEnergy_inTime_banan",cluHEPVetoChID,cluHEPVetoEnergy);

            //   }
          }
        }
      }

      if (evt->EVetoRecoEvent)
      {
        closestSACEVeto = 9999.;
        for (Int_t j = 0; j < NclusEVeto; ++j)
        {
          cluEVeto = evt->EVetoRecoCl->Element(j);
          cluEVetoTime = cluEVeto->GetTime();
          deltaTEVetoSAC = cluEVetoTime - cluSACTime;
          if (abs(deltaTEVetoSAC) < abs(closestSACEVeto))
            closestSACEVeto = deltaTEVetoSAC;
        }
        hSvc->FillHisto("SAC_EVeto_TimeDiff_closest", closestSACEVeto);
        deltaTEVetoSAC = 0;
        for (Int_t j = 0; j < NclusEVeto; ++j)
        {

          cluEVeto = evt->EVetoRecoCl->Element(j);
          cluEVetoEnergy = cluEVeto->GetEnergy();
          cluEVetoTime = cluEVeto->GetTime();
          cluEVetoZ = cluEVeto->GetPosition().Z();
          cluEVetoChID = cluEVeto->GetChannelId();

          deltaTEVetoSAC = cluEVetoTime - cluSACTime;

          hSvc->FillHisto("SAC_EVeto_TimeDiff", deltaTEVetoSAC);
          hSvc->FillHisto2("SAC_EVeto_Z_vs_TimeDiff", cluEVetoZ, deltaTEVetoSAC);
          if (abs(deltaTEVetoSAC) < 20.0)
            hSvc->FillHisto2("SAC_EVeto_ChID_vs_TimeDiff_detail", cluEVetoChID, deltaTEVetoSAC);
          hSvc->FillHisto2("SACEnergy_EVetoZ", cluSACEnergy, cluEVetoZ);
          hSvc->FillHisto2("SACEnergy_EVetoChID", cluSACEnergy, cluEVetoChID);
          hSvc->FillHisto2("EVetoZ_SACEnergy", cluEVetoZ, cluSACEnergy);
          hSvc->FillHisto2("EVetoChID_SACEnergy", cluEVetoChID, cluSACEnergy);
          // To be opened of we find anything at all...
          if (deltaTEVetoSAC > -1.0 && deltaTEVetoSAC < 3.0)
          {
            hSvc->FillHisto2("SACEnergy_EVetoZ_inTime", cluSACEnergy, cluEVetoZ);
            hSvc->FillHisto2("SACEnergy_EVetoChID_inTime", cluSACEnergy, cluEVetoChID);
            hSvc->FillHisto2("EVetoZ_SACEnergy_inTime", cluEVetoZ, cluSACEnergy);
            hSvc->FillHisto2("EVetoChID_SACEnergy_inTime", cluEVetoChID, cluSACEnergy);

            //   if(cluEVetoZ<500.0) {
            //     hSvc->FillHisto2("SACEnergy_EVetoZ_inTime_banan",cluSACEnergy,cluEVetoZ);
            //     hSvc->FillHisto2("SACEnergy_EVetoChID_inTime_banan",cluSACEnergy,cluEVetoChID);
            //     hSvc->FillHisto2("EVetoZ_SACEnergy_inTime_banan",cluEVetoZ,cluSACEnergy);
            //     hSvc->FillHisto2("EVetoChID_SACEnergy_inTime_banan",cluEVetoChID,cluSACEnergy);

            //     hSvc->FillHisto2("EVetoEnergy_EVetoChID_inTime_banan",cluEVetoEnergy,cluEVetoChID);
            //     hSvc->FillHisto2("EVetoChID_EVetoEnergy_inTime_banan",cluEVetoChID,cluEVetoEnergy);

            //   }
          }
        }
      }
    }
  }

  if (evt->ECalRecoEvent)
  {

    NclusECal = evt->ECalRecoCl->GetNElements();
    Int_t NhitsECal = evt->ECalRecoEvent->GetNHits();

    hSvc->FillHisto("ECal_NHits", NhitsECal);
    hSvc->FillHisto("ECal_NClusters", NclusECal);

    hitECal = NULL;
    for (Int_t i = 0; i < NhitsECal; i++)
    {
      hitECal = evt->ECalRecoEvent->Hit(i);
      hSvc->FillHisto("ECal_Hit_Energy", hitECal->GetEnergy());
    }

    
    // nGG=0;

    for (Int_t i = 0; i < NclusECal; i++)
    {
      cluECal = NULL;

      cluEnergy = 0;
      cluTime = 0;
      cluRadius = 0;
      cluTheta = 0;
      cluPhi = 0;
      cluX = 0;
      cluY = 0;
      cluZ = 0;

      MissingMass = 0;
      MissingMomentum = 0;


      cluECal = evt->ECalRecoCl->Element(i);
      cluEnergy = CalibrateEnergy(cluECal->GetEnergy(), cluX, cluY);
      cluTime = cluECal->GetTime();
      cluX = cluECal->GetPosition().X();
      cluY = cluECal->GetPosition().Y();
      cluChX = (cluECal->GetChannelId()) / 10;
      cluChY = (cluECal->GetChannelId()) % 10;
      //cluZ = cluECal->GetPosition().Z();
      cluZ = 2508.31;
      //std::cout << "CluZ: " << cluZ << std::endl;
      cluRadius = ComputeRadius(cluX, cluY);
      cluTheta = ComputeTheta(cluX, cluY, cluZ, cluEnergy);
      //cluTheta = ComputeCMMomentum(-14.3, 0.35, -1028, cluX, cluY, cluZ).Vect().Theta();
      cluPhi = ComputePhi(cluX, cluY, cluZ, cluEnergy);
      
      MissingMass = ComputeMissingMass(BeamEnergy, cluX, cluY, Z_1_U, cluEnergy);
      MissingMomentum = sqrt(ComputeMissingMomentum(BeamEnergy, cluX, cluY, Z_1_U, cluEnergy));
      MissingMomentumV = ComputeMissingMomentumVector(BeamEnergy, cluX, cluY, Z_1_U, cluEnergy);
      MissingTheta = MissingMomentumV.Theta();

      hSvc->FillHisto("ECal_Cluster_Energy", cluEnergy);
      hSvc->FillHisto("ECal_Cluster_Time", cluTime);
      hSvc->FillHisto2("ECal_Cluster_Time_Energy", cluTime, cluEnergy);
      hSvc->FillHisto("ECal_Cluster_Radius", cluRadius);
      hSvc->FillHisto("ECal_Cluster_Theta", cluTheta);
      hSvc->FillHisto("ECal_Cluster_Phi", cluPhi);
      hSvc->FillHisto("ECal_Cluster_Z", cluZ);
      hSvc->FillHisto2("ECal_Cluster_Energy_Theta", cluEnergy, cluTheta);
      hSvc->FillHisto2("ECal_Cluster_Energy_Phi", cluEnergy, cluPhi);
      hSvc->FillHisto2("ECal_Cluster_Energy_Radius", cluEnergy, cluRadius);

      hSvc->FillHisto("ClusterEnergy_All", cluEnergy);
      hSvc->FillHisto("MissingMass_All", MissingMass);
      hSvc->FillHisto("MissingMomentum_All", MissingMomentum);
      hSvc->FillHisto("MissingTheta_All", MissingTheta);
      hSvc->FillHisto2("MissingMassTheta_All", MissingMass, cluTheta);
      hSvc->FillHisto2("MissingMassRadius_All", MissingMass, cluRadius);
      hSvc->FillHisto2("MissingMomentumMissingTheta_All", MissingMomentum, MissingTheta);

      if (cluEnergy > 30.)
      {
        hSvc->FillHisto("ClusterEnergy_Ecut30MeV", cluEnergy);
        hSvc->FillHisto("MissingMass_Ecut30MeV", MissingMass);
        hSvc->FillHisto("MissingMomentum_Ecut30MeV", MissingMomentum);
        hSvc->FillHisto("MissingTheta_Ecut30MeV", MissingTheta);
        hSvc->FillHisto2("MissingMassTheta_Ecut30MeV", MissingMass, cluTheta);
        hSvc->FillHisto2("MissingMassRadius_Ecut30MeV", MissingMass, cluRadius);
        hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut30MeV", MissingMomentum, MissingTheta);
      }
      if (cluEnergy > 50.)
      {
        hSvc->FillHisto("ClusterEnergy_Ecut50MeV", cluEnergy);
        hSvc->FillHisto("MissingMass_Ecut50MeV", MissingMass);
        hSvc->FillHisto("MissingMomentum_Ecut50MeV", MissingMomentum);
        hSvc->FillHisto("MissingTheta_Ecut50MeV", MissingTheta);
        hSvc->FillHisto2("MissingMassTheta_Ecut50MeV", MissingMass, cluTheta);
        hSvc->FillHisto2("MissingMassRadius_Ecut50MeV", MissingMass, cluRadius);
        hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut50MeV", MissingMomentum, MissingTheta);
      }
      if (cluEnergy > 60.)
      {
        hSvc->FillHisto("ClusterEnergy_Ecut60MeV", cluEnergy);
        hSvc->FillHisto("MissingMass_Ecut60MeV", MissingMass);
        hSvc->FillHisto("MissingMomentum_Ecut60MeV", MissingMomentum);
        hSvc->FillHisto("MissingTheta_Ecut60MeV", MissingTheta);
        hSvc->FillHisto2("MissingMassTheta_Ecut60MeV", MissingMass, cluTheta);
        hSvc->FillHisto2("MissingMassRadius_Ecut60MeV", MissingMass, cluRadius);
        hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut60MeV", MissingMomentum, MissingTheta);
      }
      if (cluRadius > 90. && cluRadius < 270.)
      {
        // if(cluChY>3.&&cluChY<26.){
        if (cluEnergy > 30.)
        {
          hSvc->FillHisto("ClusterEnergy_Ecut30MeV_GeomCut", cluEnergy);
          hSvc->FillHisto("MissingMass_Ecut30MeV_GeomCut", MissingMass);
          hSvc->FillHisto("MissingMomentum_Ecut30MeV_GeomCut", MissingMomentum);
          hSvc->FillHisto("MissingTheta_Ecut30MeV_GeomCut", MissingTheta);
          hSvc->FillHisto2("MissingMassTheta_Ecut30MeV_GeomCut", MissingMass, cluTheta);
          hSvc->FillHisto2("MissingMassRadius_Ecut30MeV_GeomCut", MissingMass, cluRadius);
          hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut30MeV_GeomCut", MissingMomentum, MissingTheta);
        }
        if (cluEnergy > 50.)
        {
          hSvc->FillHisto("ClusterEnergy_Ecut50MeV_GeomCut", cluEnergy);
          hSvc->FillHisto("MissingMass_Ecut50MeV_GeomCut", MissingMass);
          hSvc->FillHisto("MissingMomentum_Ecut50MeV_GeomCut", MissingMomentum);
          hSvc->FillHisto("MissingTheta_Ecut50MeV_GeomCut", MissingTheta);
          hSvc->FillHisto2("MissingMassTheta_Ecut50MeV_GeomCut", MissingMass, cluTheta);
          hSvc->FillHisto2("MissingMassRadius_Ecut50MeV_GeomCut", MissingMass, cluRadius);
          hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut50MeV_GeomCut", MissingMomentum, MissingTheta);
        }
        if (cluEnergy > 60.)
        {
          hSvc->FillHisto("ClusterEnergy_Ecut60MeV_GeomCut", cluEnergy);
          hSvc->FillHisto("MissingMass_Ecut60MeV_GeomCut", MissingMass);
          hSvc->FillHisto("MissingMomentum_Ecut60MeV_GeomCut", MissingMomentum);
          hSvc->FillHisto("MissingTheta_Ecut60MeV_GeomCut", MissingTheta);
          hSvc->FillHisto2("MissingMassTheta_Ecut60MeV_GeomCut", MissingMass, cluTheta);
          hSvc->FillHisto2("MissingMassRadius_Ecut60MeV_GeomCut", MissingMass, cluRadius);
          hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut60MeV_GeomCut", MissingMomentum, MissingTheta);
          //}
        }

        hSvc->FillHisto("ECal_Cluster_X", cluX);
        hSvc->FillHisto("ECal_Cluster_Y", cluY);
        hSvc->FillHisto2("ECal_Cluster_XY", cluX, cluY);

        deltaTECal = 0;
        intime1ns = 0;
        intime2ns = 0;
        intime5ns = 0;

        closestECalECal = 9999.;
        for (Int_t j = 0; j < NclusECal; ++j)
        {
          cluECal2 = evt->ECalRecoCl->Element(j);
          deltaTECal = cluTime - cluECal2->GetTime();
          if (abs(deltaTECal) < abs(closestECalECal))
            closestECalECal = deltaTECal;
        }
        hSvc->FillHisto("ECal_2clu_DeltaT_closest", closestECalECal);

        for (Int_t j = i + 1; j < NclusECal; ++j) // gamma gamma check
        {
          if (j != i)
          {

            cluECal2 = evt->ECalRecoCl->Element(j);
            // clu2Energy = eCalib*(cluECal2->GetEnergy());
            clu2X = cluECal2->GetPosition().X();
            clu2Y = cluECal2->GetPosition().Y();
            //clu2Z = cluECal2->GetPosition().Z();
            clu2Z = 2508.31;
            clu2ChX = (cluECal2->GetChannelId()) / 10;
            clu2ChY = (cluECal2->GetChannelId()) % 10;
            clu2Radius = ComputeRadius(clu2X, clu2Y);
            clu2Theta = ComputeTheta(clu2X, clu2Y, clu2Z, clu2Energy);
            clu2Phi = ComputePhi(clu2X, clu2Y, clu2Z, clu2Energy);
            clu2Energy = CalibrateEnergy(cluECal2->GetEnergy(), clu2X, clu2Y);

            deltaTECal = cluTime - cluECal2->GetTime();
            hSvc->FillHisto("ECal_2clu_DeltaT", deltaTECal);
            if (fabs(deltaTECal) < 1.0)
              intime1ns += 1;
            if (fabs(deltaTECal) < 2.0)
              intime2ns += 1;
            if (fabs(deltaTECal) < 5.0)
            {
              intime5ns += 1;
              if (clu2Energy > 30. && cluEnergy > 30. && clu2Radius > 90.0 && clu2Radius < 270.0)
              {
                // if( clu2ChY>3. && clu2ChY<26.){
                energySum = cluEnergy + clu2Energy;
                hSvc->FillHisto("GammaGamma_EnergySum", energySum);
                // MC
                if (evt->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED))
                {
                  if (energySum > eCalib * eSumMClow && energySum < eCalib * eSumMChigh)
                  {
                    sumTheta = cluTheta + clu2Theta;
                    deltaPhi = cluPhi - clu2Phi;
                    hSvc->FillHisto("GammaGamma_SumTheta", sumTheta);
                    hSvc->FillHisto("GammaGamma_DeltaPhi", abs(deltaPhi));
                    hSvc->FillHisto2("GammaGamma_SumTheta_DeltaPhi", sumTheta, abs(deltaPhi));
                    if (sumTheta > sumThetaMClow && sumTheta < sumThetaMChigh)
                    {
                      if (abs(deltaPhi) > deltaPhiMClow && abs(deltaPhi) < deltaPhiMChigh)
                      {
                        nGG += 1;

                        COGX = ComputeCOG(cluEnergy, cluX, clu2Energy, clu2X);
                        COGY = ComputeCOG(cluEnergy, cluY, clu2Energy, clu2Y);
                        hSvc->FillHisto("GammaGamma_COGX", COGX);
                        hSvc->FillHisto("GammaGamma_COGY", COGY);
                        hSvc->FillHisto2("GammaGamma_COG", COGX, COGY);

                        hSvc->FillHisto("GammaGamma_X", cluX);
                        hSvc->FillHisto("GammaGamma_X", clu2X);
                        hSvc->FillHisto("GammaGamma_Y", cluY);
                        hSvc->FillHisto("GammaGamma_Y", clu2Y);
                        hSvc->FillHisto2("GammaGamma_XY", cluX, cluY);
                        hSvc->FillHisto2("GammaGamma_XY", clu2X, clu2Y);

                        hSvc->FillHisto2("GammaGamma_XX", cluX, clu2X);
                        hSvc->FillHisto2("GammaGamma_YY", cluY, clu2Y);

                        hSvc->FillHisto2("GammaGamma_EE", cluEnergy, clu2Energy);
                        hSvc->FillHisto("GammaGamma_dE", cluEnergy - clu2Energy);
                        // if (abs(cluEnergy - clu2Energy)<148. && abs(cluEnergy - clu2Energy)>115. ){
                        hSvc->FillHisto("GammaGamma_dX", cluX + clu2X);
                        hSvc->FillHisto("GammaGamma_dY", cluY + clu2Y);

                        hSvc->FillHisto2("GammaGamma_dXdY", abs(cluY - clu2Y), abs(cluX - clu2X));

                        
                        //}
                      }
                    }
                  }
                }
                // Data
                if (!(evt->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)))
                {
                  if (energySum > eCalib * eSumDatalow && energySum < eCalib * eSumDatahigh)
                  {
                    cluTheta = 0;
                    clu2Theta = 0;
                    // fcoordout << cluEnergy << " " << cluX << " " << cluY << " " << cluZ << " " << clu2Energy << " " << clu2X << " " << clu2Y << " " << clu2Z << "\n";
                    //  for(Double_t xt=-20.; xt<21.; xt++){
                    //   for(Double_t yt=-20.; yt<21.; yt++){
                    //   for(Double_t zt=-1040.; zt<-999.; zt++){
                    TLorentzVector clu4Mom = ComputeCMMomentum(-14.3, 0.35, -1028, cluX, cluY, cluZ);
                    TLorentzVector clu24Mom = ComputeCMMomentum(-14.3, 0.35, -1028, clu2X, clu2Y, clu2Z);
                    cluTheta = clu4Mom.Vect().Theta();
                    // std::cout<<"Theta1: "<<cluTheta<<std::endl;
                    clu2Theta = clu24Mom.Vect().Theta();

                    sumTheta = cluTheta + clu2Theta;
                    deltaPhi = cluPhi - clu2Phi;
                    hSvc->FillHisto("GammaGamma_SumTheta", sumTheta);
                    hSvc->FillHisto("GammaGamma_DeltaPhi", abs(deltaPhi));
                    hSvc->FillHisto2("GammaGamma_SumTheta_DeltaPhi", sumTheta, abs(deltaPhi));
                    if (sumTheta > sumThetaDatalow && sumTheta < sumThetaDatahigh)
                    {
                      if (abs(deltaPhi) > deltaPhiDatalow && abs(deltaPhi) < deltaPhiDatahigh)
                      {
                        double r = gRandom->Uniform();
                        double PI = TMath::Pi();
                        // if(r>0.5){
                        hSvc->FillHistoProf("GammaGamma_EnergySum_X", cluX, energySum);
                        hSvc->FillHistoProf("GammaGamma_EnergySum_Y", cluY, energySum);
                        hSvc->FillHistoProf("GammaGamma_EnergySum_Phi", cluPhi, energySum);
                        hSvc->FillHistoProf("GammaGamma_EnergySum_E", cluEnergy, energySum);
                        if (cluPhi >= 0 && cluPhi < (PI) / 3.0)
                          hSvc->FillHisto("GammaGamma_EnergySum_0_pi/3.0", energySum);
                        if (cluPhi >= (PI) / 3.0 && cluPhi < 2.0 * (PI) / 3.0)
                          hSvc->FillHisto("GammaGamma_EnergySum_pi/3.0_2.0pi/3.0", energySum);
                        if (cluPhi >= 2.0 * (PI) / 3.0 && cluPhi <= (PI))
                          hSvc->FillHisto("GammaGamma_EnergySum_2.0pi/3.0_pi", energySum);
                        if (cluPhi < 0 && cluPhi > (-(PI) / 3.0))
                          hSvc->FillHisto("GammaGamma_EnergySum_0_-pi/3.0", energySum);
                        if (cluPhi <= (-(PI) / 3.0) && cluPhi > (-2.0 * (PI) / 3.0))
                          hSvc->FillHisto("GammaGamma_EnergySum_-pi/3.0_-2.0pi/3.0", energySum);
                        if (cluPhi <= (-2.0 * (PI) / 3.0) && cluPhi > (-(PI)))
                          hSvc->FillHisto("GammaGamma_EnergySum_-2.0pi/3.0_-pi", energySum);
                        // }
                        // if(r<=0.5){
                        //   if(clu2Phi>=0 && clu2Phi<(PI)/3.0) hSvc->FillHisto("GammaGamma_EnergySum_0_pi/3.0",energySum);
                        //   if(clu2Phi>=(PI)/3.0 && clu2Phi<2.0*(PI)/3.0) hSvc->FillHisto("GammaGamma_EnergySum_pi/3.0_2.0pi/3.0",energySum);
                        //   if(clu2Phi>=2.0*(PI)/3.0 && clu2Phi<=(PI)) hSvc->FillHisto("GammaGamma_EnergySum_2.0pi/3.0_pi",energySum);
                        //   if(clu2Phi<0 && clu2Phi>(-(PI)/3.0)) hSvc->FillHisto("GammaGamma_EnergySum_0_-pi/3.0",energySum);
                        //   if(clu2Phi<=(-(PI)/3.0) && clu2Phi>(-2.0*(PI)/3.0)) hSvc->FillHisto("GammaGamma_EnergySum_-pi/3.0_-2.0pi/3.0",energySum);
                        //   if(clu2Phi<=(-2.0*(PI)/3.0) && clu2Phi>(-(PI))) hSvc->FillHisto("GammaGamma_EnergySum_-2.0pi/3.0_-pi",energySum);
                        // }
                        // int ix = xt + 20;
                        // int iy = yt + 20;
                        // int iz = zt + 1040;
                        // int index = ix * 41 * 41 + iy * 41 + iz;
                        // nGG_scan[index]+=1;
                        nGG += 1;

                        COGX = ComputeCOG(cluEnergy, cluX, clu2Energy, clu2X);
                        COGY = ComputeCOG(cluEnergy, cluY, clu2Energy, clu2Y);
                        hSvc->FillHisto("GammaGamma_COGX", COGX);
                        hSvc->FillHisto("GammaGamma_COGY", COGY);
                        hSvc->FillHisto2("GammaGamma_COG", COGX, COGY);

                        hSvc->FillHisto("GammaGamma_X", cluX);
                        hSvc->FillHisto("GammaGamma_X", clu2X);
                        hSvc->FillHisto("GammaGamma_Y", cluY);
                        hSvc->FillHisto("GammaGamma_Y", clu2Y);

                        hSvc->FillHisto2("GammaGamma_XY", cluX, cluY);
                        hSvc->FillHisto2("GammaGamma_XY", clu2X, clu2Y);

                        hSvc->FillHisto2("GammaGamma_XX", cluX, clu2X);
                        hSvc->FillHisto2("GammaGamma_YY", cluY, clu2Y);

                        hSvc->FillHisto2("GammaGamma_EE", cluEnergy, clu2Energy);
                        hSvc->FillHisto("GammaGamma_dE", cluEnergy - clu2Energy);
                        // if (abs(cluEnergy - clu2Energy)<148. && abs(cluEnergy - clu2Energy)>115. ){
                        hSvc->FillHisto("GammaGamma_dX", cluX + clu2X);
                        hSvc->FillHisto("GammaGamma_dY", cluY + clu2Y);

                        hSvc->FillHisto2("GammaGamma_dXdY", abs(cluY - clu2Y), abs(cluX - clu2X));

                        //}
                        hSvc->FillHisto("GammaGamma_EnergySumSel", energySum);
                        Minv = ComputeInvariantMass(cluEnergy, clu2Energy, cluX, clu2X, cluY, clu2Y);
                        hSvc->FillHisto("GammaGamma_MinvariantSel", Minv);

                        hSvc->FillHisto("GammaGamma_EnergySumSel-MinvariantSel", energySum - Minv);

                        hSvc->FillHisto2("GammaGamma_ELab_EReco", ComputeExpectedEnergy(cluX,cluY),cluEnergy);
                        hSvc->FillHisto2("GammaGamma_ELab_EReco", ComputeExpectedEnergy(clu2X,clu2Y),clu2Energy);

                        hSvc->FillHisto2("GammaGamma_ThetaLab_Radius", cluTheta,cluRadius);
                        hSvc->FillHisto2("GammaGamma_ThetaLab_Radius", clu2Theta,clu2Radius);
                      }
                    }
                    //  }
                    // }
                    //}
                  }
                }
                //}
              }
            }
          }
        }

        hSvc->FillHisto("GammaGamma_NClusters", nGG);

        intimeECalSAC1ns = 0;
        intimeECalSAC2ns = 0;
        intimeECalSAC5ns = 0;
        if (evt->SACRecoEvent) // ECal + SAC check for 3-photon events and others
        {
          for (Int_t j = 0; j < NclusSAC; ++j)
          {
            cluSAC = evt->SACRecoCl->Element(j);
            if (cluSAC->GetEnergy() > 30. && cluEnergy > 30.) 
            {
              deltaTECalSAC = cluTime - cluSAC->GetTime();
              energySumECalSAC = cluEnergy + cluSAC->GetEnergy();
              hSvc->FillHisto("ECal_SAC_DeltaT", deltaTECalSAC);
              if (fabs(deltaTECalSAC) < 1.0)
              {
                intimeECalSAC1ns += 1;
                hSvc->FillHisto("ECal_SAC_EnergySum_1ns", energySumECalSAC);
              }
              if (fabs(deltaTECalSAC) < 2.0)
              {
                intimeECalSAC2ns += 1;
                hSvc->FillHisto("ECal_SAC_EnergySum_2ns", energySumECalSAC);
              }
              if (fabs(deltaTECalSAC) < 5.0)
              {
                intimeECalSAC5ns += 1;
                hSvc->FillHisto("ECal_SAC_EnergySum_5ns", energySumECalSAC);
              }
            }
          }
        }

        intimePVeto1ns = 0;
        intimePVeto1ns_inBanan = 0;

        bestdP = 9999.;
        bestdE = 9999.;

        dPE = 9999.;

        if (evt->PVetoRecoEvent && evt->EVetoRecoEvent) // special check
        {
          if (NclusPVeto != 0 && NclusEVeto != 0)
          {
            for (Int_t j = 0; j < NclusPVeto; ++j)
            {
              cluPVeto = evt->PVetoRecoCl->Element(j);
              cluPVetoTime = cluPVeto->GetTime();
              deltaTECalPVeto = cluPVetoTime - cluTime;
              if (fabs(deltaTECalPVeto) < fabs(bestdP))
              {
                bestdP = deltaTECalPVeto;
                GoldenCluPVeto = cluPVeto;
              }
            }

            for (Int_t j = 0; j < NclusEVeto; ++j)
            {
              cluEVeto = evt->EVetoRecoCl->Element(j);
              cluEVetoTime = cluEVeto->GetTime();
              deltaTECalEVeto = cluEVetoTime - cluTime;
              if (fabs(deltaTECalEVeto) < fabs(bestdE))
              {
                bestdE = deltaTECalEVeto;
                GoldenCluEVeto = cluEVeto;
              }
            }

            hSvc->FillHisto2("ECal_PVeto_EVeto_TimeDiff", bestdP, bestdE);
            dPE = GoldenCluPVeto->GetTime() - GoldenCluEVeto->GetTime();
            hSvc->FillHisto("Golden_PVeto_EVeto_TimeDiff", dPE);
            if (fabs(dPE) < 2.0 && cluEnergy > 40. && (abs(GoldenCluPVeto->GetTime() - cluTime) < 2.0) && (abs(GoldenCluEVeto->GetTime() - cluTime) < 2.0))
            {
              if ((GoldenCluPVeto->GetChannelId()) > (alineECalCh * cluEnergy * cluEnergy + blineECalCh * cluEnergy + cgoreECalCh) ||
                  (GoldenCluPVeto->GetChannelId()) < (alineECalCh * cluEnergy * cluEnergy + blineECalCh * cluEnergy + cdoluECalCh))
                hSvc->FillHisto("MissingMass_Special", MissingMass);
              hSvc->FillHisto("MissingMomentum_Special", MissingMomentum);
              ChIDDiff = GoldenCluPVeto->GetChannelId() - GoldenCluEVeto->GetChannelId();
              hSvc->FillHisto("ECal_PVeto_EVeto_ChIDDiff", ChIDDiff);
              hSvc->FillHisto2("MissingMass_Special_inChID_ChIDDiff", MissingMass, ChIDDiff);
              hSvc->FillHisto2("MissingMomentum_Special_inChID_ChIDDiff", MissingMomentum, ChIDDiff);
              if (ChIDDiff > 20. && ChIDDiff < 40.)
                hSvc->FillHisto("MissingMass_Special_inChID", MissingMass);
              hSvc->FillHisto("MissingMomentum_Special_inChID", MissingMomentum);
            }
          }
        }

        if (evt->PVetoRecoEvent) // ECal + PVeto check
        {
          closestECalPVeto = 9999.;
          for (Int_t j = 0; j < NclusPVeto; ++j)
          {
            cluPVeto = evt->PVetoRecoCl->Element(j);
            cluPVetoTime = cluPVeto->GetTime();
            deltaTECalPVeto = cluPVetoTime - cluTime;
            if (abs(deltaTECalPVeto) < abs(closestECalPVeto))
              closestECalPVeto = deltaTECalPVeto;
          }
          hSvc->FillHisto("ECal_PVeto_TimeDiff_closest", closestECalPVeto);
          deltaTECalPVeto = 0;
          cluPVeto = NULL;
          cluPVetoEnergy = 0;
          cluPVetoTime = 0;
          cluPVetoZ = 0;
          cluPVetoChID = 0;
          for (Int_t j = 0; j < NclusPVeto; ++j)
          {

            cluPVeto = evt->PVetoRecoCl->Element(j);
            cluPVetoEnergy = cluPVeto->GetEnergy();
            cluPVetoTime = cluPVeto->GetTime();
            cluPVetoZ = cluPVeto->GetPosition().Z();
            cluPVetoChID = cluPVeto->GetChannelId();

            deltaTECalPVeto = cluPVetoTime - cluTime;

            hSvc->FillHisto("ECal_PVeto_TimeDiff", deltaTECalPVeto);
            hSvc->FillHisto2("ECal_PVeto_Z_vs_TimeDiff", cluPVetoZ, deltaTECalPVeto);
            if (abs(deltaTECalPVeto) < 20.0)
              hSvc->FillHisto2("ECal_PVeto_ChID_vs_TimeDiff_detail", cluPVetoChID, deltaTECalPVeto);

            hSvc->FillHisto2("ECalEnergy_PVetoZ", cluEnergy, cluPVetoZ);
            hSvc->FillHisto2("ECalEnergy_PVetoChID", cluEnergy, cluPVetoChID);

            hSvc->FillHisto2("PVetoZ_ECalEnergy", cluPVetoZ, cluEnergy);
            hSvc->FillHisto2("PVetoChID_ECalEnergy", cluPVetoChID, cluEnergy);

            if (deltaTECalPVeto > -20. && deltaTECalPVeto < -18.)
              hSvc->FillHisto2("ECalEnergy_PVetoChID_random", cluEnergy, cluPVetoChID);

            if (fabs(deltaTECalPVeto) <= 2.0)
            {
              intimePVeto1ns += 1;
              hSvc->FillHisto2("ECalEnergy_PVetoZ_inTime", cluEnergy, cluPVetoZ);
              hSvc->FillHisto2("ECalEnergy_PVetoChID_inTime", cluEnergy, cluPVetoChID);
              hSvc->FillHisto2("PVetoZ_ECalEnergy_inTime", cluPVetoZ, cluEnergy);
              hSvc->FillHisto2("PVetoChID_ECalEnergy_inTime", cluPVetoChID, cluEnergy);

              if (evt->EVetoRecoEvent)
              {
                for (Int_t k = 0; k < NclusEVeto; ++k)
                {
                  cluEVeto = evt->EVetoRecoCl->Element(k);
                  cluEVetoTime = cluEVeto->GetTime();
                  deltaTECalEVeto = cluEVetoTime - cluTime;
                  hSvc->FillHisto2("ECal_PVeto_EVeto_TimeDiff", deltaTECalPVeto, deltaTECalEVeto);
                }
              }
            }
            // if((fabs(deltaTECalPVeto) <= 5.0) && (BeamEnergy-cluEnergy)< (alineECalCh*cluPVetoChID*cluPVetoChID+blineECalCh*cluPVetoChID+cgoreECalCh) &&
            //    (BeamEnergy-cluEnergy)  > (alineECalCh*cluPVetoChID*cluPVetoChID+blineECalCh*cluPVetoChID+cdoluECalCh)){
            if ((fabs(deltaTECalPVeto) <= 2.0) && (cluPVetoChID) < (alineECalCh * cluEnergy * cluEnergy + blineECalCh * cluEnergy + cgoreECalCh) &&
                (cluPVetoChID) > (alineECalCh * cluEnergy * cluEnergy + blineECalCh * cluEnergy + cdoluECalCh))
            {

              intimePVeto1ns_inBanan += 1;
              hSvc->FillHisto2("PVetoZ_ECalEnergy_inTime_Cut", cluPVetoZ, cluEnergy);
              hSvc->FillHisto2("ECalEnergy_PVetoChID_inTime_Cut", cluEnergy, cluPVetoChID);

              // Old criteria to search for A'->e+e-; new one is with golden clusters a bit above
              //  if(evt->EVetoRecoEvent) {
              //  	for (Int_t k=0; k<NclusEVeto; ++k){
              //  	  cluEVeto = evt->EVetoRecoCl->Element(k);
              //  	  cluEVetoTime = cluEVeto->GetTime();
              //  	  deltaTECalEVeto = cluEVetoTime-cluTime;
              //  	  if(fabs(deltaTECalEVeto)<=5.) hSvc->FillHisto("MissingMass_Special",MissingMass);
              //    }
              //  }
            }
            if ((fabs(deltaTECalPVeto) <= 2.0) && intimePVeto1ns_inBanan == 0)
            {
              hSvc->FillHisto2("PVetoZ_ECalEnergy_inTime_afterCut", cluPVetoZ, cluEnergy);
              hSvc->FillHisto2("ECalEnergy_PVetoChID_inTime_afterCut", cluEnergy, cluPVetoChID);
            }
          }
        }

        intimeHEPVeto1ns = 0;
        intimeHEPVeto1ns_inBanan = 0;
        if (evt->HEPVetoRecoEvent) // ECal + HEPVeto check
        {
          closestECalHEPVeto = 9999.;
          for (Int_t j = 0; j < NclusHEPVeto; ++j)
          {
            cluHEPVeto = evt->HEPVetoRecoCl->Element(j);
            cluHEPVetoTime = cluHEPVeto->GetTime();
            deltaTECalHEPVeto = cluHEPVetoTime - cluTime;
            if (abs(deltaTECalHEPVeto) < abs(closestECalHEPVeto))
              closestECalHEPVeto = deltaTECalHEPVeto;
          }
          hSvc->FillHisto("ECal_HEPVeto_TimeDiff_closest", closestECalHEPVeto);
          deltaTECalHEPVeto = 0;
          cluHEPVeto = NULL;
          cluHEPVetoEnergy = 0;
          cluHEPVetoTime = 0;
          cluHEPVetoX = 0;
          cluHEPVetoChID = 0;
          for (Int_t j = 0; j < NclusHEPVeto; ++j)
          {

            cluHEPVeto = evt->HEPVetoRecoCl->Element(j);
            cluHEPVetoEnergy = cluHEPVeto->GetEnergy();
            cluHEPVetoTime = cluHEPVeto->GetTime();
            cluHEPVetoX = cluHEPVeto->GetPosition().X();
            cluHEPVetoChID = cluHEPVeto->GetChannelId();

            deltaTECalHEPVeto = cluHEPVetoTime - cluTime;

            hSvc->FillHisto("ECal_HEPVeto_TimeDiff", deltaTECalHEPVeto);
            hSvc->FillHisto2("ECal_HEPVeto_X_vs_TimeDiff", cluHEPVetoX, deltaTECalHEPVeto);
            if (abs(deltaTECalHEPVeto) < 20.)
              hSvc->FillHisto2("ECal_HEPVeto_ChID_vs_TimeDiff_detail", cluHEPVetoChID, deltaTECalHEPVeto);

            hSvc->FillHisto2("ECalEnergy_HEPVetoX", cluEnergy, cluHEPVetoX);
            hSvc->FillHisto2("ECalEnergy_HEPVetoChID", cluEnergy, cluHEPVetoChID);

            hSvc->FillHisto2("HEPVetoX_ECalEnergy", cluHEPVetoX, cluEnergy);
            hSvc->FillHisto2("HEPVetoChID_ECalEnergy", cluHEPVetoChID, cluEnergy);

            if (deltaTECalHEPVeto > -20. && deltaTECalPVeto < -18.)
              hSvc->FillHisto2("ECalEnergy_HEPVetoChID_random", cluEnergy, cluHEPVetoChID);

            if (fabs(deltaTECalHEPVeto) < 2.0)
            {
              intimeHEPVeto1ns += 1;
              hSvc->FillHisto2("ECalEnergy_HEPVetoX_inTime", cluEnergy, cluHEPVetoX);
              hSvc->FillHisto2("ECalEnergy_HEPVetoChID_inTime", cluEnergy, cluHEPVetoChID);
              hSvc->FillHisto2("HEPVetoX_ECalEnergy_inTime", cluHEPVetoX, cluEnergy);
              hSvc->FillHisto2("HEPVetoChID_ECalEnergy_inTime", cluHEPVetoChID, cluEnergy);
            }
            // To be done when cut is defined
            if (fabs(deltaTECalHEPVeto) < 2.0 && ((cluHEPVetoChID) < (alineECalHEPCh * cluEnergy * cluEnergy + blineECalHEPCh * cluEnergy + cgoreECalHEPCh) &&
                                                  (cluHEPVetoChID) > (alineECalHEPCh * cluEnergy * cluEnergy + blineECalHEPCh * cluEnergy + cdoluECalHEPCh)))
            {
              intimeHEPVeto1ns_inBanan += 1;
              hSvc->FillHisto2("HEPVetoX_ECalEnergy_inTime_Cut", cluHEPVetoX, cluEnergy);
              hSvc->FillHisto2("ECalEnergy_HEPVetoChID_inTime_Cut", cluEnergy, cluHEPVetoChID);
            }
            if (fabs(deltaTECalHEPVeto) < 5.0 && intimeHEPVeto1ns_inBanan == 0)
            {
              hSvc->FillHisto2("HEPVetoX_ECalEnergy_inTime_afterCut", cluHEPVetoX, cluEnergy);
              hSvc->FillHisto2("ECalEnergy_HEPVetoChID_inTime_afterCut", cluEnergy, cluHEPVetoChID);
            }
          }
        }

        intimeEVeto1ns = 0;
        if (evt->EVetoRecoEvent) // ECal + EVeto check
        {
          closestECalEVeto = 9999.;
          for (Int_t j = 0; j < NclusEVeto; ++j)
          {
            cluEVeto = evt->EVetoRecoCl->Element(j);
            cluEVetoTime = cluEVeto->GetTime();
            deltaTECalEVeto = cluEVetoTime - cluTime;
            if (abs(deltaTECalEVeto) < abs(closestECalEVeto))
              closestECalEVeto = deltaTECalEVeto;
          }
          hSvc->FillHisto("ECal_EVeto_TimeDiff_closest", closestECalEVeto);
          deltaTECalEVeto = 0;
          cluEVeto = NULL;
          cluEVetoEnergy = 0;
          cluEVetoTime = 0;
          cluEVetoZ = 0;
          cluEVetoChID = 0;
          for (Int_t j = 0; j < NclusEVeto; ++j)
          {

            cluEVeto = evt->EVetoRecoCl->Element(j);
            cluEVetoEnergy = cluEVeto->GetEnergy();
            cluEVetoTime = cluEVeto->GetTime();
            cluEVetoZ = cluEVeto->GetPosition().Z();
            cluEVetoChID = cluEVeto->GetChannelId();

            deltaTECalEVeto = cluEVetoTime - cluTime;

            hSvc->FillHisto("ECal_EVeto_TimeDiff", deltaTECalEVeto);
            hSvc->FillHisto2("ECal_EVeto_Z_vs_TimeDiff", cluEVetoZ, deltaTECalEVeto);
            if (abs(deltaTECalEVeto) < 20.0)
              hSvc->FillHisto2("ECal_EVeto_ChID_vs_TimeDiff_detail", cluEVetoChID, deltaTECalEVeto);

            hSvc->FillHisto2("ECalEnergy_EVetoZ", cluEnergy, cluEVetoZ);
            hSvc->FillHisto2("ECalEnergy_EVetoChID", cluEnergy, cluEVetoChID);

            hSvc->FillHisto2("EVetoZ_ECalEnergy", cluEVetoZ, cluEnergy);
            hSvc->FillHisto2("EVetoChID_ECalEnergy", cluEVetoChID, cluEnergy);

            if (deltaTECalEVeto > -20. && deltaTECalEVeto < -18.)
              hSvc->FillHisto2("ECalEnergy_EVetoChID_random", cluEnergy, cluEVetoChID);

            if (deltaTECalEVeto > -5.0 && deltaTECalEVeto < 5.0)
            {
              intimeEVeto1ns += 1;
              hSvc->FillHisto2("ECalEnergy_EVetoZ_inTime", cluEnergy, cluEVetoZ);
              hSvc->FillHisto2("ECalEnergy_EVetoChID_inTime", cluEnergy, cluEVetoChID);
              hSvc->FillHisto2("EVetoZ_ECalEnergy_inTime", cluEVetoZ, cluEnergy);
              hSvc->FillHisto2("EVetoChID_ECalEnergy_inTime", cluEVetoChID, cluEnergy);

              // To be done when cut is defined
              //  if((BeamEnergy-cluEnergy)< (alineECalCh*cluEVetoChID*cluEVetoChID+blineECalCh*cluEVetoChID+cgoreECalCh) &&
              //     (BeamEnergy-cluEnergy)  > (alineECalCh*cluEVetoChID*cluEVetoChID+blineECalCh*cluEVetoChID+cdoluECalCh)){
              //    intimeEVeto1ns_inBanan+=1;
              //    hSvc->FillHisto2("EVetoZ_ECalEnergy_inTime_Cut",cluEVetoZ,cluEnergy);
              //    hSvc->FillHisto2("EVetoChID_ECalEnergy_inTime_Cut",cluEVetoChID,cluEnergy);

              // }
              // else {
              //   hSvc->FillHisto2("EVetoZ_ECalEnergy_inTime_afterCut",cluEVetoZ,cluEnergy);
              //   hSvc->FillHisto2("EVetoChID_ECalEnergy_inTime_afterCut",cluEVetoChID,cluEnergy);
              // }
            }
          }
        }

        if (intime1ns == 0 && cluRadius > 90. && cluRadius < 280.)
        {
          if (cluEnergy > 30.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut30MeV_GeomCut_1nsIsolation", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut30MeV_GeomCut_1nsIsolation", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut30MeV_GeomCut_1nsIsolation", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut30MeV_GeomCut_1nsIsolation", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut30MeV_GeomCut_1nsIsolation", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut30MeV_GeomCut_1nsIsolation", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut30MeV_GeomCut_1nsIsolation", MissingMomentum, MissingTheta);
          }
          if (cluEnergy > 50.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut50MeV_GeomCut_1nsIsolation", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut50MeV_GeomCut_1nsIsolation", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut50MeV_GeomCut_1nsIsolation", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut50MeV_GeomCut_1nsIsolation", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut50MeV_GeomCut_1nsIsolation", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut50MeV_GeomCut_1nsIsolation", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut50MeV_GeomCut_1nsIsolation", MissingMomentum, MissingTheta);
          }
          if (cluEnergy > 60.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut60MeV_GeomCut_1nsIsolation", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut60MeV_GeomCut_1nsIsolation", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut60MeV_GeomCut_1nsIsolation", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut60MeV_GeomCut_1nsIsolation", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut60MeV_GeomCut_1nsIsolation", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut60MeV_GeomCut_1nsIsolation", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut60MeV_GeomCut_1nsIsolation", MissingMomentum, MissingTheta);
          }
        }

        if (intime2ns == 0 && cluRadius > 90. && cluRadius < 280.)
        {
          if (cluEnergy > 30.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut30MeV_GeomCut_2nsIsolation", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut30MeV_GeomCut_2nsIsolation", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut30MeV_GeomCut_2nsIsolation", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut30MeV_GeomCut_2nsIsolation", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut30MeV_GeomCut_2nsIsolation", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut30MeV_GeomCut_2nsIsolation", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut30MeV_GeomCut_2nsIsolation", MissingMomentum, MissingTheta);
          }
          if (cluEnergy > 50.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut50MeV_GeomCut_2nsIsolation", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut50MeV_GeomCut_2nsIsolation", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut50MeV_GeomCut_2nsIsolation", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut50MeV_GeomCut_2nsIsolation", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut50MeV_GeomCut_2nsIsolation", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut50MeV_GeomCut_2nsIsolation", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut50MeV_GeomCut_2nsIsolation", MissingMomentum, MissingTheta);
          }
          if (cluEnergy > 60.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut60MeV_GeomCut_2nsIsolation", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut60MeV_GeomCut_2nsIsolation", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut60MeV_GeomCut_2nsIsolation", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut60MeV_GeomCut_2nsIsolation", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut60MeV_GeomCut_2nsIsolation", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut60MeV_GeomCut_2nsIsolation", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut60MeV_GeomCut_2nsIsolation", MissingMomentum, MissingTheta);
          }
        }

        if (intime5ns == 0 && cluRadius > 90. && cluRadius < 280.)
        {
          if (cluEnergy > 30.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut30MeV_GeomCut_5nsIsolation", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut30MeV_GeomCut_5nsIsolation", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut30MeV_GeomCut_5nsIsolation", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut30MeV_GeomCut_5nsIsolation", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut30MeV_GeomCut_5nsIsolation", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut30MeV_GeomCut_5nsIsolation", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut30MeV_GeomCut_5nsIsolation", MissingMomentum, MissingTheta);
          }
          if (cluEnergy > 50.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut50MeV_GeomCut_5nsIsolation", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut50MeV_GeomCut_5nsIsolation", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut50MeV_GeomCut_5nsIsolation", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut50MeV_GeomCut_5nsIsolation", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut50MeV_GeomCut_5nsIsolation", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut50MeV_GeomCut_5nsIsolation", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut50MeV_GeomCut_5nsIsolation", MissingMomentum, MissingTheta);
          }
          if (cluEnergy > 60.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut60MeV_GeomCut_5nsIsolation", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut60MeV_GeomCut_5nsIsolation", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut60MeV_GeomCut_5nsIsolation", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut60MeV_GeomCut_5nsIsolation", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut60MeV_GeomCut_5nsIsolation", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut60MeV_GeomCut_5nsIsolation", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut60MeV_GeomCut_5nsIsolation", MissingMomentum, MissingTheta);
          }
        }

        if (intime5ns == 0 && cluRadius > 90. && cluRadius < 280. && intimePVeto1ns == 0)
        {
          if (cluEnergy > 30.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTime", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTime", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTime", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTime", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTime", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTime", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTime", MissingMomentum, MissingTheta);
          }
          if (cluEnergy > 50.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTime", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTime", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTime", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTime", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTime", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTime", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTime", MissingMomentum, MissingTheta);
          }
          if (cluEnergy > 60.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTime", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTime", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTime", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTime", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTime", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTime", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTime", MissingMomentum, MissingTheta);
          }
        }

        if (intime5ns == 0 && cluRadius > 90. && cluRadius < 280. && intimePVeto1ns_inBanan == 0)
        {
          if (cluEnergy > 30.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", MissingMomentum, MissingTheta);
          }
          if (cluEnergy > 50.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", MissingMomentum, MissingTheta);
          }
          if (cluEnergy > 60.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit", MissingMomentum, MissingTheta);
          }
        }

        if (intime5ns == 0 && cluRadius > 90. && cluRadius < 280. && intimePVeto1ns_inBanan == 0 && intimeHEPVeto1ns == 0)
        {
          if (cluEnergy > 30.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", MissingMomentum, MissingTheta);
          }
          if (cluEnergy > 50.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", MissingMomentum, MissingTheta);
          }
          if (cluEnergy > 60.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", MissingMomentum, MissingTheta);
          }
        }

        if (intime5ns == 0 && cluRadius > 90. && cluRadius < 280. && intimePVeto1ns_inBanan == 0 && intimeHEPVeto1ns_inBanan == 0)
        {
          if (cluEnergy > 30.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", MissingMomentum, MissingTheta);
          }
          if (cluEnergy > 50.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", MissingMomentum, MissingTheta);
          }
          if (cluEnergy > 60.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", MissingMomentum, MissingTheta);
          }
        }

        // if(intime5ns==0 && cluRadius>90.&&cluRadius<280. && intimePVeto1ns==0 && intimePVeto1ns_inBanan==0 && intimeHEPVeto1ns==0&& intimeHEPVeto1ns_inBanan==0 && intimeEVeto1ns==0 ){
        // 	if(cluEnergy > 30.)   hSvc->FillHisto("MissingMass_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut",MissingMass);
        // 	if(cluEnergy > 50.)   hSvc->FillHisto("MissingMass_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut",MissingMass);
        // 	if(cluEnergy > 60.)   hSvc->FillHisto("MissingMass_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut",MissingMass);

        // }

        if (intime5ns == 0 && cluRadius > 90. && cluRadius < 280. && intimePVeto1ns_inBanan == 0 && intimeHEPVeto1ns == 0 && intimeEVeto1ns == 0)
        {
          if (cluEnergy > 30.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut30MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", MissingMomentum, MissingTheta);
          }
          if (cluEnergy > 50.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut50MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", MissingMomentum, MissingTheta);
          }
          if (cluEnergy > 60.)
          {
            hSvc->FillHisto("ClusterEnergy_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", cluEnergy);
            hSvc->FillHisto("MissingMass_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", MissingMass);
            hSvc->FillHisto("MissingMomentum_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", MissingMomentum);
            hSvc->FillHisto("MissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", MissingTheta);
            hSvc->FillHisto2("MissingMassTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", MissingMass, cluTheta);
            hSvc->FillHisto2("MissingMassRadius_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", MissingMass, cluRadius);
            hSvc->FillHisto2("MissingMomentumMissingTheta_Ecut60MeV_GeomCut_5nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", MissingMomentum, MissingTheta);
          }
        }
        //}
      }
    }
  }

  fResult = true;
  return fResult;
}

Bool_t SPA_BackgroundCuts::Finalize()
{

  std::cerr << "=== FINALIZE ENTERED ===" << std::endl;
  // std::string filename = "scan_output_" + std::to_string(getpid()) + ".txt";
  // std::ofstream fout(filename);

  // if (!fout.is_open()) {
  //   std::cerr << "FAILED TO OPEN OUTPUT FILE: " << filename << std::endl;
  //   return kFALSE;
  // }
  // for (int index = 0; index < 41 * 41 * 41; index++) {

  //   int ix = index / (41 * 41);
  //   int iy = (index / 41) % 41;
  //   int iz = index % 41;

  //   double xt = ix - 20;
  //   double yt = iy - 20;
  //   double zt = iz - 1040;

  //   fout << index << " "
  // 	 << xt << " "
  // 	 << yt << " "
  // 	 << zt << " "
  // 	 << nGG_scan[index] << "\n";
  // }
  // fout.close();
  // fcoordout.close();
  // std::cerr << "WROTE FILE: " << filename << std::endl;
  return true;
}
