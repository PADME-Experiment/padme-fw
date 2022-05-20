#include <fstream>
#include "TGraph2D.h"
#include "TTree.h"
#include "TFile.h"
#include "compare_20kPOTTargetNoTarget.C"
#include "PadmeUtils.h"
#include "PadmeStyle.h"
#include "PadmeLabels.h"
#include <ctime>


void produceSelectionPlots_20kPOTOnTargetNoTarget()
{

#ifdef __CINT__
  gROOT->LoadMacro("PadmeLabels.C");
  gROOT->LoadMacro("PadmeUtils.C");
  gROOT->LoadMacro("PadmeStyle.C");
#endif
  
  SetPadmeStyle();

  // 150ns(and 270ns ) of bunch lenght 
  TFile *fT_150 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030203_20200722_031723_npotThr5000.root"); 
  TFile *fnoT_150 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030204_20200722_054045_npotThr0.root"); //150ns bunch lenght no target
 
  std::string xtitle;
  double xmin=-999;
  double xmax= 999;
  
  std::fstream fs;
  fs.open("ggCount_20k150ns.txt", std::fstream::in | std::fstream::out | std::fstream::app);
  time_t now=time(0);
  char* dt =ctime(&now);
  fs << "name          20k      ..."<<dt << std::endl;
  fs.close();

  xtitle = "Number of ECal hits";
  xmin = 0.;
  xmax = 400.;
  compare_20kPOTTargetNoTarget("SS2g_nhits",-1, xmin, xmax, xtitle,fT_150, fnoT_150);
  xtitle = "Cluster size";
  xmin = -999;
  xmax =  999;
  compare_20kPOTTargetNoTarget("SS2g_clSize",-1, xmin, xmax, xtitle,fT_150, fnoT_150);
  
  xtitle = "E_{hit} [MeV]";
  xmin = 0.;
  xmax = 2500.;
  compare_20kPOTTargetNoTarget("ECal_HitEnergy", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  xtitle = "E_{cl} [MeV]";
  compare_20kPOTTargetNoTarget("ECal_ClusterEnergy",  -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  xtitle = "E_{hit}^{tot} [MeV]";
  compare_20kPOTTargetNoTarget("ECal_EHitTot", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  xtitle = "E_{cl}^{tot} [MeV]";
  compare_20kPOTTargetNoTarget("ECal_EClTot", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
    
  xtitle = "N hits in cl";
  xmin = 0.;
  xmax = 50.;
  compare_20kPOTTargetNoTarget( "ECal_NHitsInClus",-1, xmin, xmax, xtitle,fT_150, fnoT_150);
  xtitle = "N cl";
  xmin = 0.;
  xmax = 80.;
  compare_20kPOTTargetNoTarget("ECal_NCluster",-1, xmin, xmax, xtitle,fT_150, fnoT_150);
  xtitle = "nPOT";
  xmin = 0.;
  xmax = 3.;
  compare_20kPOTTargetNoTarget("nPOT",-1, xmin, xmax, xtitle,fT_150, fnoT_150);
  xtitle = "POT/Bunch";
  xmin = 0.;
  xmax = 35000.;
  compare_20kPOTTargetNoTarget("NposInBunch_beam",-1, xmin, xmax, xtitle,fT_150, fnoT_150);

  xtitle = "E(#gamma_{1})+E(#gamma_{2}) [MeV]";
  xmin = 0.;
  xmax = 1200.; 
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700);
  compare_20kPOTTargetNoTarget("ECAL_gravTwoPhoton10ns_InDeltaPhi20Degree2Method", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700);
  compare_20kPOTTargetNoTarget("ECAL_gravTwoPhoton10ns_InDeltaPhi20Degree2Method_DeltaTheta", -1, xmin, xmax, xtitle,fT_150, fnoT_150 , 0, 350, 700);
  compare_20kPOTTargetNoTarget("ECAL_gravTwoPhoton10ns1cm", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  xmin = 300.;
  xmax = 700.;
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10nsUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700);
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700);
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700);
  compare_20kPOTTargetNoTarget("ECAL_gravTwoPhoton10ns1cmUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_gravTwoPhoton10ns1cm20DegreeUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_gravTwoPhoton10ns1cm20DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  xmin = 0.;
  xmax = 1200.;
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10nsEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700);
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700);
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700);
  compare_20kPOTTargetNoTarget("ECAL_gravTwoPhoton10ns1cmEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_gravTwoPhoton10ns1cm20DegreeEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_gravTwoPhoton10ns1cm20DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);  //14
  
  std::string ggcog5[5]={"ECAL_gravTwoPhoton10ns5cm","ECAL_gravTwoPhoton10ns5cmUnderPeak", "ECAL_gravTwoPhoton10ns5cmUnderPeakEThr90", "ECAL_gravTwoPhoton10ns5cmUnderPeakEThr90_FRY","ECAL_gravTwoPhoton10ns5cmEThr90"};
  for(int i= 0; i<5; i++) { 
    compare_20kPOTTargetNoTarget(ggcog5[i], -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700);
  }
  xtitle = "E(#gamma_{1})+E(#gamma_{2}) [MeV]";
  xmin = 0.;
  xmax = 900.;
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange70_280", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700);
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange90_260", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700);
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange110_240", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700); 
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange130_220", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700); 
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange150_200", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700); 
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange160_190", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700); 
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange70_280", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700); 
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange90_280", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700); 
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange110_280", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700); 
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange130_280", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700);
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange150_280", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700);
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange160_280", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700); 
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange70_280UnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700); 
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange90_260UnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700); 
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange110_240UnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700); 
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange130_220UnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700);
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange150_200UnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700);
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange160_190UnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700);
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange70_280", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700); 
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange90_280UnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700); 
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange110_280UnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700); 
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange130_280UnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700); 
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange150_280UnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700);
  compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange160_280UnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, 350, 700); 
 //compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange70_280", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
 //compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange90_280", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
 //compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange110_280", -1, xmin, xmax, xtitle,fT_150, fnoT_150); 
 //compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange130_280", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
 //compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange150_280", -1, xmin, xmax, xtitle,fT_150, fnoT_150); 
 //compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange160_280", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
 //compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange70_280", -1, xmin, xmax, xtitle,fT_150, fnoT_150); 
 //compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange90_280", -1, xmin, xmax, xtitle,fT_150, fnoT_150); 
 //compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange110_280", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
 //compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange130_280", -1, xmin, xmax, xtitle,fT_150, fnoT_150); 
 //compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange150_280", -1, xmin, xmax, xtitle,fT_150, fnoT_150); 
 //compare_20kPOTTargetNoTarget("ECAL_twoPhotonInTime10ns1cmInRadiusRange160_280", -1, xmin, xmax, xtitle,fT_150, fnoT_150);

  xtitle = "#Delta t (#gamma_{1})+E(#gamma_{2}) [ns]";
  xmin = -10;
  xmax = 10.;
  compare_20kPOTTargetNoTarget("ECAL_dTime_10ns", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_dTime_10ns_UnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_dTime_10ns20DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_dTime_10ns20DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_dTime_10ns5DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_dTime_10ns5DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_dTime_10ns1CoG", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_dTime_10ns1CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_dTime_10ns3CoG", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_dTime_10ns3CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_dTime_10ns5CoG", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_dTime_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);

  xmin = -350.;
  xmax =  350.;
  xtitle = "X [mm]";
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns20Degree", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns20DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns5DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns20DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns5DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns20DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns20DegreeDeltaThetaUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns5DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns20DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns5DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns1CoG", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns1CoGUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns1CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns1CoGEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns1CoG20Degree", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns1CoG20DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns3CoG", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns3CoGUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns3CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns3CoGUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns3CoGEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns5CoG", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns5CoGUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns5CoGUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Xgg_10ns5CoGEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);

  xmin = -350.;
  xmax =  350.;
  xtitle = "Y [mm]";
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns20Degree", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns20DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns5DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns20DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns5DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns20DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns20DegreeDeltaThetaUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns5DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns20DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns5DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns1CoG", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns1CoGUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns1CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns1CoGEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns1CoG20Degree", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns1CoG20DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns3CoG", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns3CoGUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns3CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns3CoGUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns3CoGEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns5CoG", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns5CoGUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns5CoGUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Ygg_10ns5CoGEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);

  xmin = 0.;
  xmax =  350.;
  xtitle = "R [mm]";
  compare_20kPOTTargetNoTarget("ECal_HitRadius", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_RggAllClusters", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns20Degree", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns20DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns1CoG", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns1CoGEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns1CoG20Degree", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns1CoG20DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns5DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns3CoG", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns3CoGUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns3CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns3CoGUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns3CoGEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns5CoG", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns5CoGUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns5CoGUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns5CoGEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10nsUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns20DegreeUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns20DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns5DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns20DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns20DegreeDeltaThetaUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns5DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns5DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns1CoGUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns1CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns1CoG20DegreeUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns1CoG20DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10nsEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns20DegreeEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns20DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns1CoG20DegreeEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Rgg_10ns1CoG20DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  // compare_20kPOTTargetNoTarget("ECAL_HigherRgg_10ns5CoG", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  // compare_20kPOTTargetNoTarget("ECAL_LessRgg_10ns5CoG", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  // compare_20kPOTTargetNoTarget("ECAL_HigherRgg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  //compare_20kPOTTargetNoTarget("ECAL_LessRgg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);

  xmin = -0.5;
  xmax = 600.;
  xtitle = "E [MeV]";
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns20Degree", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns20DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns5DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns20DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns5DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns20DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns20DegreeDeltaThetaUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns5DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns20DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns5DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns1CoG", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns1CoGUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns1CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns1CoGEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns1CoG20Degree", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns1CoG20DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns1CoG20DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns3CoG", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns3CoGUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns3CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns3CoGUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns3CoGEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns5CoG", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns5CoGUnderPeak", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns5CoGUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Egg_10ns5CoGEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  //compare_20kPOTTargetNoTarget("ECAL_HigherEgg_10ns5CoG", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  //compare_20kPOTTargetNoTarget("ECAL_LessEgg_10ns5CoG", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  //compare_20kPOTTargetNoTarget("ECAL_HigherEgg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  //compare_20kPOTTargetNoTarget("ECAL_LessEgg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  
  xmin =-80; 
  xmax = 250;
  xtitle = "t [ns]";
  compare_20kPOTTargetNoTarget("ECal_HitTime", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECal_ClusterTime", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_tgg_10ns20DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_tgg_10ns5DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_tgg_10ns1CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_tgg_10ns3CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_tgg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  
  xmin =-300; 
  xmax =  300;
  xtitle = "xCoG [mm]";
  compare_20kPOTTargetNoTarget("ECal_xCoG_10ns", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  xtitle = "yCoG [mm]";
  compare_20kPOTTargetNoTarget("ECal_yCoG_10ns", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  xtitle = "xCoG [mm]";
  compare_20kPOTTargetNoTarget("ECAL_xCoGgg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  xtitle = "yCoG [mm]";
  compare_20kPOTTargetNoTarget("ECAL_yCoGgg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);

  xmin =-0.; 
  xmax = 0.2;
  xtitle = "#Theta";
  compare_20kPOTTargetNoTarget("ECAL_Theta_10nsUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_ThetaG1_10nsUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_ThetaG2_10nsUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  xtitle = "#Delta #Theta";
  compare_20kPOTTargetNoTarget("ECAL_DeltaTheta_10ns", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_DeltaTheta_10nsDeltaPhi20", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_DeltaTheta_10nsUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  xmin =-4; 
  xmax = 4;
  xtitle = "#Phi";
  compare_20kPOTTargetNoTarget("ECAL_Phi_10ns", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Phi_10nsUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_PhiG1_10nsUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_PhiG2_10nsUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Phi_10nsDeltaTheta", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_Phi_10ns20DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  xmin =-0.1; 
  xmax = 4.2;
  xtitle = "#Delta #Phi";
  compare_20kPOTTargetNoTarget("ECAL_DeltaPhi_10ns", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_DeltaPhi_10nsDeltaTheta", -1, xmin, xmax, xtitle,fT_150, fnoT_150);

  xmin =-0.08; 
  xmax = 0.08;
  xtitle = "#Delta #Theta";
  compare_20kPOTTargetNoTarget("ECal_DeltaThetaFuncAllCl", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, -0.01, 0.015);
  compare_20kPOTTargetNoTarget("ECal_DeltaThetaFuncAllCl_ethr50MeV", -1, xmin, xmax, xtitle,fT_150, fnoT_150, 0, -0.01, 0.015);
  compare_20kPOTTargetNoTarget("ECal_DeltaThetaFuncAllClThr1GeV", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECal_DeltaThetaFuncNmaxCl8", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECal_DeltaThetaFuncNmaxCl4", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECal_DeltaThetaFuncFR_xHiger120", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECal_DeltaThetaFuncFR_xHiger150", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECal_DeltaThetaFuncFR_xHiger150_EthrLess1GeV", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECal_DeltaThetaFuncFR_xLess120", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECal_DeltaThetaFuncAllClThr400MeV1200MeV", -1, xmin, xmax, xtitle,fT_150, fnoT_150);

  xmin =-200; 
  xmax = 700;
  xtitle = "Missing Mass [MeV^2]";
  compare_20kPOTTargetNoTarget("MissingMass_AllECALclu", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("MissingMass_AllECALclu_ThrEne50MeV", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("MissingMass_NoClInTime20ns_ThrEne50MeV", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("MissingMass_NoClInTime10ns_ThrEne50MeV", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("MissingMass_NoClInTime20ns_ThrEne50MeV_NoCoincidencePVeto", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("MissingMass_NoClInTime20ns_ThrEne50MeV_NoCoincidencePVeto_NoCoincidenceSAC", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("MissingMass_NoClInTime10ns_ThrEne50MeV_NoCoincidencePVeto", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("MissingMass_NoClInTime10ns_ThrEne50MeV_NoCoincidencePVeto_NoCoincidenceSAC", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  compare_20kPOTTargetNoTarget("ECAL_MissingMassAnnihilation_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT_150, fnoT_150);
  

}
