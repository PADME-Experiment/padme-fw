#include "PadmeUtils.h"
#include "PadmeStyle.h"
#include "PadmeLabels.h"

#include "compare_NoTarget.C"
#include "MakeDifference_NoTarget.C"
#include "compare.C"

using namespace std;

void produceSelectionPlotsOnlyFourCuts_NoTarget(TFile* fData=_file0, TFile* fMC=_file1)
{
  
  #ifdef __CINT__
    gROOT->LoadMacro("PadmeLabels.C");
    gROOT->LoadMacro("PadmeUtils.C");
    gROOT->LoadMacro("PadmeStyle.C");
  #endif
    // new TCanvas("c1","c1",800,300);

  std::string ggdpdt[9]={"ECAL_gravTwoPhoton10ns_InDeltaPhi20Degree2Method_DeltaTheta","ECAL_twoPhotonInTime10ns20DegreeDeltaThetaUnderPeak","ECAL_twoPhotonInTime10ns20DegreeDeltaThetaUnderPeakEThr90","ECAL_twoPhotonInTime10ns20DegreeDeltaThetaUnderPeakEThr90_FRY","ECAL_twoPhotonInTime10ns20DegreeDeltaThetaEThr90", "ECAL_gravTwoPhoton10ns_InDeltaPhi5Degree2Method_DeltaTheta", "ECAL_twoPhotonInTime10ns5DegreeDeltaThetaUnderPeak","ECAL_twoPhotonInTime10ns5DegreeDeltaThetaUnderPeakEThr90","ECAL_twoPhotonInTime10ns5DegreeDeltaThetaEThr90"};
  
  std::string ggcog[9]={"ECAL_gravTwoPhoton10ns3cm","ECAL_gravTwoPhoton10ns3cmUnderPeak", "ECAL_gravTwoPhoton10ns3cmUnderPeakEThr90", "ECAL_gravTwoPhoton10ns3cmUnderPeakEThr90_FRY","ECAL_gravTwoPhoton10ns3cmEThr90", "ECAL_gravTwoPhoton10ns1cm","ECAL_gravTwoPhoton10ns1cmUnderPeak","ECAL_gravTwoPhoton10ns1cmUnderPeakEThr90","ECAL_gravTwoPhoton10ns1cmEThr90"};
  std::string ggcog5[5]={"ECAL_gravTwoPhoton10ns5cm","ECAL_gravTwoPhoton10ns5cmUnderPeak", "ECAL_gravTwoPhoton10ns5cmUnderPeakEThr90", "ECAL_gravTwoPhoton10ns5cmUnderPeakEThr90_FRY","ECAL_gravTwoPhoton10ns5cmEThr90"};
  
  
  std::string ggdpdtFR[6]={"ECAL_twoPhotonInTime10ns20DegreeDeltaThetaOut4Cry134_216","ECAL_twoPhotonInTime10ns20DegreeDeltaThetaOut4Cry134_216UnderPeak","ECAL_twoPhotonInTime10ns20DegreeDeltaThetaOut4Cry134_216EThr90", "ECAL_twoPhotonInTime10ns5DegreeDeltaThetaOut4Cry134_216","ECAL_twoPhotonInTime10ns5DegreeDeltaThetaOut4Cry134_216UnderPeak","ECAL_twoPhotonInTime10ns5DegreeDeltaThetaOut4Cry134_216EThr90"};
  
  std::string ggcogFR[6]={"ECAL_twoPhotonInTime10ns1CoGOut4Cry134_216","ECAL_twoPhotonInTime10ns1CoGOut4Cry134_216UnderPeak","ECAL_twoPhotonInTime10ns1CoGOut4Cry134_216EThr90","ECAL_twoPhotonInTime10ns3CoGOut4Cry134_216", "ECAL_twoPhotonInTime10ns3CoGOut4Cry134_216UnderPeak","ECAL_twoPhotonInTime10ns3CoGOut4Cry134_216EThr90"};

std::string ggInTimecog[10]={"ECAL_gravTwoPhoton10ns3cmUnderPeakEThr90_timeRange1", 	"ECAL_gravTwoPhoton10ns3cmUnderPeakEThr90_timeRange2",
"ECAL_gravTwoPhoton10ns3cmUnderPeakEThr90_timeRange3", "ECAL_gravTwoPhoton10ns3cmUnderPeakEThr90_timeRange4" ,
"ECAL_gravTwoPhoton10ns3cmUnderPeakEThr90_timeRange5", "ECAL_gravTwoPhoton10ns1cmUnderPeakEThr90_timeRange1", 	"ECAL_gravTwoPhoton10ns1cmUnderPeakEThr90_timeRange2",
"ECAL_gravTwoPhoton10ns1cmUnderPeakEThr90_timeRange3", "ECAL_gravTwoPhoton10ns1cmUnderPeakEThr90_timeRange4" ,
"ECAL_gravTwoPhoton10ns1cmUnderPeakEThr90_timeRange5"};

std::string ggInTimedp[10]={"ECAL_twoPhotonInTime10ns20DegreeDeltaThetaUnderPeakEThr90_timeRange1", "ECAL_twoPhotonInTime10ns20DegreeDeltaThetaUnderPeakEThr90_timeRange2",
"ECAL_twoPhotonInTime10ns20DegreeDeltaThetaUnderPeakEThr90_timeRange3",	"ECAL_twoPhotonInTime10ns20DegreeDeltaThetaUnderPeakEThr90_timeRange4",
	"ECAL_twoPhotonInTime10ns20DegreeDeltaThetaUnderPeakEThr90_timeRange5",  "ECAL_twoPhotonInTime10ns5DegreeDeltaThetaUnderPeakEThr90_timeRange1", "ECAL_twoPhotonInTime10ns5DegreeDeltaThetaUnderPeakEThr90_timeRange2",
  "ECAL_twoPhotonInTime10ns5DegreeDeltaThetaUnderPeakEThr90_timeRange3",	"ECAL_twoPhotonInTime10ns5DegreeDeltaThetaUnderPeakEThr90_timeRange4",
  	"ECAL_twoPhotonInTime10ns5DegreeDeltaThetaUnderPeakEThr90_timeRange5"};



  int lastSet=0;
  Bool_t MakeDifference=true;
 
  std::string xtitle;
  double xmin=-999;
  double xmax= 999;
  
  SetPadmeStyle();  
  Bool_t dTimeNormalization=true;
  
  xtitle = "Number of ECal hits";
  xmin = 0.;
  xmax = 400.;
  compare_NoTarget("SS2g_nhits",                    -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  xtitle = "Cluster size";
  xmin = -999;
  xmax =  999;
  compare_NoTarget("SS2g_clSize",                    -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  
  xtitle = "E_{hit} [MeV]";
  xmin = 0.;
  xmax = 2500.;
  compare_NoTarget("ECal_HitEnergy", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  xtitle = "E_{cl} [MeV]";
  compare_NoTarget("ECal_ClusterEnergy",  -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  xtitle = "E_{hit^{tot} [MeV]";
  compare_NoTarget("ECal_EHitTot", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  xtitle = "E_{cl}^{tot} [MeV]";
  compare_NoTarget("ECal_EClTot", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
    
  xtitle = "N hits in cl";
  xmin = 0.;
  xmax = 50.;
  compare_NoTarget( "ECal_NHitsInClus",                    -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  xtitle = "N cl";
  xmin = 0.;
  xmax = 80.;
  compare_NoTarget("ECal_NCluster",                    -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  
  xtitle = "E(#gamma_{1})+E(#gamma_{2}) [MeV]";
  xmin = 0.;
  xmax = 900.;
  for(int i= 0; i<9; i++) { 
    compare_NoTarget(ggdpdt[i], -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
    MakeDifference_NoTarget(ggdpdt[i], -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  }
 for(int i= 0; i<6; i++) { 
    compare_NoTarget(ggdpdtFR[i], -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
    MakeDifference_NoTarget(ggdpdtFR[i], -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  }
  for(int i= 0; i<9; i++) { 
    compare_NoTarget(ggcog[i], -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
    MakeDifference_NoTarget(ggcog[i], -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  }
  for(int i= 0; i<5; i++) { 
    compare_NoTarget(ggcog5[i], -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
     MakeDifference_NoTarget(ggcog5[i], -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  }/*
  for(int i= 0; i<6; i++) {  
    compare_NoTarget(ggcogFR[i], -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
    MakeDifference_NoTarget(ggcogFR[i], -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
    }*/
  /*
  //new histos in range time
 for(int i= 0; i<10; i++) {  
    compare_NoTarget(ggInTimedp[i], -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
    MakeDifference_NoTarget(ggInTimedp[i], -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
}
 for(int i= 0; i<10; i++) {  
    compare_NoTarget(ggInTimecog[i], -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
    MakeDifference_NoTarget(ggInTimecog[i], -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  }
  */
  compare_NoTarget("ECAL_gravTwoPhoton10ns5cm", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  MakeDifference_NoTarget("ECAL_gravTwoPhoton10ns5cm", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  xtitle = "#Delta t (#gamma_{1})+E(#gamma_{2}) [ns]";
  xmin = -10;
  xmax = 10.;
  compare_NoTarget("ECAL_dTime_10ns", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  MakeDifference_NoTarget("ECAL_dTime_10ns", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  compare_NoTarget("ECAL_dTime_10ns_UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  MakeDifference_NoTarget("ECAL_dTime_10ns_UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  compare_NoTarget("ECAL_dTime_10ns20DegreeDeltaTheta", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  MakeDifference_NoTarget("ECAL_dTime_10ns20DegreeDeltaTheta", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  compare_NoTarget("ECAL_dTime_10ns20DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  MakeDifference_NoTarget("ECAL_dTime_10ns20DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  compare_NoTarget("ECAL_dTime_10ns5DegreeDeltaTheta", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  MakeDifference_NoTarget("ECAL_dTime_10ns5DegreeDeltaTheta", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  compare_NoTarget("ECAL_dTime_10ns5DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  MakeDifference_NoTarget("ECAL_dTime_10ns5DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  compare_NoTarget("ECAL_dTime_10ns1CoG", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  MakeDifference_NoTarget("ECAL_dTime_10ns1CoG", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  compare_NoTarget("ECAL_dTime_10ns1CoGUnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  MakeDifference_NoTarget("ECAL_dTime_10ns1CoGUnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  compare_NoTarget("ECAL_dTime_10ns3CoG", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  MakeDifference_NoTarget("ECAL_dTime_10ns3CoG", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  compare_NoTarget("ECAL_dTime_10ns3CoGUnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  MakeDifference_NoTarget("ECAL_dTime_10ns3CoGUnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  
  xtitle = "E(#gamma_{1})+E(#gamma_{2}) [MeV]";
  xmin = 0.;
  xmax = 900.;
  
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange70_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange70_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange90_260", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange90_260", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
   
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange110_240", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange110_240", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange130_220", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange130_220", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange150_200", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange150_200", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange160_190", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange160_190", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  // compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange70_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange90_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange90_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange110_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange110_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange130_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange130_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
   
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange150_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange150_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
   
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange160_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange160_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange70_280UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange70_280UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange90_260UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange90_260UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange110_240UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange110_240UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange130_220UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange130_220UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
   
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange150_200UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange150_200UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
   
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange160_190UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange160_190UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  // compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange70_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange90_280UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange90_280UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange110_280UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange110_280UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange130_280UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange130_280UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange150_280UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange150_280UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
    
  compare_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange160_280UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange160_280UnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
   compare_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange70_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
   MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange70_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
    
  compare_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange90_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange90_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
   
  compare_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange110_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange110_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  compare_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange130_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange130_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
   
  compare_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange150_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange150_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  compare_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange160_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange160_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  // compare_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange70_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  compare_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange90_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange90_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  compare_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange110_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange110_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  compare_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange130_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange130_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  compare_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange150_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange150_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization); 
  
  compare_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange160_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  //MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange160_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization);
  
  Int_t End=1;
  MakeDifference_NoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange160_280", -1, xmin, xmax, xtitle, fData, fMC, dTimeNormalization, End); 
 
 
}

