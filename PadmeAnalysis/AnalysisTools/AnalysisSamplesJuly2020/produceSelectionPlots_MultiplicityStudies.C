#include <fstream>
#include "TGraph2D.h"
#include "TTree.h"
#include "TFile.h"
#include "MyCompare.C"
#include "PadmeUtils.h"
#include "PadmeStyle.h"
#include "PadmeLabels.h"


void produceSelectionPlots_MultiplicityStudies()
{

#ifdef __CINT__
  gROOT->LoadMacro("PadmeLabels.C");
  gROOT->LoadMacro("PadmeUtils.C");
  gROOT->LoadMacro("PadmeStyle.C");
#endif
  
  SetPadmeStyle();
  /*
  // target in file
  TFile *fT5  =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030211_20200722_19182.root");
  TFile *fT10 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030205_20200722_081640.root"); 
  TFile *fT15 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030201_20200721_224335.root");
  TFile *fT20 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030203_20200722_031723.root"); 
  TFile *fT25 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030209_20200722_143634.root");
   
  // target out file
  TFile *fnoT5 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030220_20200723_054920.root");
  TFile *fnoT10 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030207_20200722_115800.root"); 
  TFile *fnoT15 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030202_20200722_010224.root");
  TFile *fnoT20 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030204_20200722_054045.root"); 
  TFile *fnoT25 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030210_20200722_173655.root");
  */

  //after cut on target pot
 // target in file
  TFile *fT5  =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030211_20200722_19182_npotThr1800.root");
  TFile *fT10 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030205_20200722_081640_npotThr5000.root"); 
  TFile *fT15 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030201_20200721_224335_npotThr5000.root");
  TFile *fT20 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030203_20200722_031723_npotThr5000.root"); 
  TFile *fT25 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030209_20200722_143634_npotThr5000.root");
   
  // target out file
  TFile *fnoT5 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030220_20200723_054920_npotThr0.root");
  TFile *fnoT10 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030207_20200722_115800_npotThr0.root"); 
  TFile *fnoT15 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030202_20200722_010224_npotThr0.root");
  TFile *fnoT20 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030204_20200722_054045_npotThr0.root"); 
  TFile *fnoT25 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030210_20200722_173655_npotThr0.root");

  std::fstream fs;
  fs.open("ggCount.txt", std::fstream::in | std::fstream::out | std::fstream::app);
  fs << "name          5k        10k     15k      20k      25k" << std::endl;
  fs.close();

   
  std::string xtitle;
  double xmin=-999;
  double xmax= 999;
    
  xtitle = "Number of ECal hits";
  xmin = 0.;
  xmax = 400.;
  MyCompare("SS2g_nhits",-1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  xtitle = "Cluster size";
  xmin = -999;
  xmax =  999;
  MyCompare("SS2g_clSize",-1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  
  xtitle = "E_{hit} [MeV]";
  xmin = 0.;
  xmax = 3500.;
  MyCompare("ECal_HitEnergy", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  xtitle = "E_{cl} [MeV]";
  MyCompare("ECal_ClusterEnergy",  -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  xtitle = "E_{hit}^{tot} [MeV]";
  MyCompare("ECal_EHitTot", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  xtitle = "E_{cl}^{tot} [MeV]";
  MyCompare("ECal_EClTot", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
    
  xtitle = "N hits in cl";
  xmin = 0.;
  xmax = 50.;
  MyCompare( "ECal_NHitsInClus",-1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  xtitle = "N cl";
  xmin = 0.;
  xmax = 80.;
  MyCompare("ECal_NCluster",-1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);


  xtitle = "nPOT";
  xmin = 0.;
  xmax = 3.;
  MyCompare("nPOT",-1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  xtitle = "POT/Bunch";
  xmin = 0.;
  xmax = 35000.;
  MyCompare("NposInBunch_beam",-1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);

  xtitle = "E(#gamma_{1})+E(#gamma_{2}) [MeV]";
  xmin = 0.;
  xmax = 1200.; 
  MyCompare("ECAL_twoPhotonInTime10ns", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_twoPhotonInTime10ns", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0, 300, 700);
  MyCompare("ECAL_gravTwoPhoton10ns_InDeltaPhi20Degree2Method",  -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0, 300, 700);
  MyCompare("ECAL_gravTwoPhoton10ns_InDeltaPhi20Degree2Method_DeltaTheta", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0, 300, 700);
  MyCompare("ECAL_gravTwoPhoton10ns1cm", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  xmin = 300.;
  xmax = 700.;
  MyCompare("ECAL_twoPhotonInTime10nsUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0, 300, 700);
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0, 300, 700);
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0, 300, 700);
  MyCompare("ECAL_gravTwoPhoton10ns1cmUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_gravTwoPhoton10ns1cm20DegreeUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_gravTwoPhoton10ns1cm20DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  xmin = 0.;
  xmax = 1200.;
  MyCompare("ECAL_twoPhotonInTime10nsEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0, 300, 700);
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0, 300, 700);
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0, 300, 700);
  MyCompare("ECAL_gravTwoPhoton10ns1cmEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_gravTwoPhoton10ns1cm20DegreeEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_gravTwoPhoton10ns1cm20DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);  //14
  
  std::string ggcog5[5]={"ECAL_gravTwoPhoton10ns5cm","ECAL_gravTwoPhoton10ns5cmUnderPeak", "ECAL_gravTwoPhoton10ns5cmUnderPeakEThr90", "ECAL_gravTwoPhoton10ns5cmUnderPeakEThr90_FRY","ECAL_gravTwoPhoton10ns5cmEThr90"};
  for(int i= 0; i<5; i++) { 
    MyCompare(ggcog5[i], -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0, 300, 700);
  }
  xtitle = "E(#gamma_{1})+E(#gamma_{2}) [MeV]";
  xmin = 0.;
  xmax = 900.;
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange70_280", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange90_260", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange110_240", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange130_220", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange150_200", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange160_190", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange70_280", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange90_280", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange110_280", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange130_280", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange150_280", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange160_280", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange70_280UnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange90_260UnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange110_240UnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange130_220UnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange150_200UnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange160_190UnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange70_280", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange90_280UnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange110_280UnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange130_280UnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange150_280UnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_twoPhotonInTime10ns20DegreeDeltaThetaInRadiusRange160_280UnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns1cmInRadiusRange70_280", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_twoPhotonInTime10ns1cmInRadiusRange90_280", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_twoPhotonInTime10ns1cmInRadiusRange110_280", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns1cmInRadiusRange130_280", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_twoPhotonInTime10ns1cmInRadiusRange150_280", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns1cmInRadiusRange160_280", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_twoPhotonInTime10ns1cmInRadiusRange70_280", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns1cmInRadiusRange90_280", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns1cmInRadiusRange110_280", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_twoPhotonInTime10ns1cmInRadiusRange130_280", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns1cmInRadiusRange150_280", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0); 
  MyCompare("ECAL_twoPhotonInTime10ns1cmInRadiusRange160_280", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);

  xtitle = "#Delta t (#gamma_{1})+E(#gamma_{2}) [ns]";
  xmin = -10;
  xmax = 10.;
  MyCompare("ECAL_dTime_10ns", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0, -10, 10);
  MyCompare("ECAL_dTime_10ns_UnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0, -10, 10);
  MyCompare("ECAL_dTime_10ns20DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0, -10, 10);
  MyCompare("ECAL_dTime_10ns20DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0, -10, 10);
  MyCompare("ECAL_dTime_10ns5DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_dTime_10ns5DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_dTime_10ns1CoG", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_dTime_10ns1CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_dTime_10ns3CoG", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_dTime_10ns3CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_dTime_10ns5CoG", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0, -10, 10);
  MyCompare("ECAL_dTime_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0, -10, 10);

  xmin = -350.;
  xmax =  350.;
  xtitle = "X [mm]";
  MyCompare("ECAL_Xgg_10ns", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns20Degree", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns20DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns5DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns20DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns5DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns20DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns20DegreeDeltaThetaUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns5DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns20DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns5DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns1CoG", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns1CoGUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns1CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns1CoGEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns1CoG20Degree", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns1CoG20DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns3CoG", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns3CoGUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns3CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns3CoGUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns3CoGEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns5CoG", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns5CoGUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns5CoGUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Xgg_10ns5CoGEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);

  xmin = -350.;
  xmax =  350.;
  xtitle = "Y [mm]";
  MyCompare("ECAL_Ygg_10ns", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns20Degree", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns20DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns5DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns20DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns5DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns20DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns20DegreeDeltaThetaUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns5DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns20DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns5DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns1CoG", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns1CoGUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns1CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns1CoGEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns1CoG20Degree", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns1CoG20DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns3CoG", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns3CoGUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns3CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns3CoGUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns3CoGEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns5CoG", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns5CoGUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns5CoGUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Ygg_10ns5CoGEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);

  xmin = 0.;
  xmax =  350.;
  xtitle = "R [mm]";
  MyCompare("ECal_HitRadius", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_RggAllClusters", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns20Degree", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns20DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns1CoG", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns1CoGEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns1CoG20Degree", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns1CoG20DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns5DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns3CoG", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns3CoGUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns3CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns3CoGUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns3CoGEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns5CoG", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns5CoGUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns5CoGUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns5CoGEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10nsUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns20DegreeUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns20DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns5DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns20DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns20DegreeDeltaThetaUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns5DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns5DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns1CoGUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns1CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns1CoG20DegreeUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns1CoG20DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10nsEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns20DegreeEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns20DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns1CoG20DegreeEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Rgg_10ns1CoG20DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  //MyCompare("ECAL_HigherRgg_10ns5CoG", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  //MyCompare("ECAL_LessRgg_10ns5CoG", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  //MyCompare("ECAL_HigherRgg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  //MyCompare("ECAL_LessRgg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);

  xmin = -0.5;
  xmax = 600.;
  xtitle = "E [MeV]";
  MyCompare("ECAL_Egg_10ns", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns20Degree", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns20DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns5DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns20DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns5DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns20DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns20DegreeDeltaThetaUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns5DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns20DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns5DegreeDeltaThetaEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns1CoG", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns1CoGUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns1CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns1CoGEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns1CoG20Degree", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns1CoG20DegreeDeltaTheta", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns1CoG20DegreeDeltaThetaUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns3CoG", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns3CoGUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns3CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns3CoGUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns3CoGEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns5CoG", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns5CoGUnderPeak", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns5CoGUnderPeakEThr90_FRY", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Egg_10ns5CoGEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  //MyCompare("ECAL_HigherEgg_10ns5CoG", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  //MyCompare("ECAL_LessEgg_10ns5CoG", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  //MyCompare("ECAL_HigherEgg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  //MyCompare("ECAL_LessEgg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  
  xmin =-80; 
  xmax = 250;
  xtitle = "t [ns]";
  MyCompare("ECal_HitTime", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECal_ClusterTime", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_tgg_10ns20DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_tgg_10ns5DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_tgg_10ns1CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_tgg_10ns3CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_tgg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  
  xmin =-300; 
  xmax =  300;
  xtitle = "xCoG [mm]";
  MyCompare("ECal_xCoG_10ns", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  xtitle = "yCoG [mm]";
  MyCompare("ECal_yCoG_10ns", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  xtitle = "xCoG [mm]";
  MyCompare("ECAL_xCoGgg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  xtitle = "yCoG [mm]";
  MyCompare("ECAL_yCoGgg_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  
  xmin =-0.; 
  xmax = 0.2;
  xtitle = "#Theta";
  MyCompare("ECAL_Theta_10nsUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_ThetaG1_10nsUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_ThetaG2_10nsUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  xtitle = "#Delta #Theta";
  MyCompare("ECAL_DeltaTheta_10ns", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0, 0.08, 0.12);
  MyCompare("ECAL_DeltaTheta_10nsDeltaPhi20", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_DeltaTheta_10nsUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0, 0.08, 0.12);
  xmin =-4; 
  xmax = 4;
  xtitle = "#Phi";
  MyCompare("ECAL_Phi_10ns", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Phi_10nsUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_PhiG1_10nsUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_PhiG2_10nsUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Phi_10nsDeltaTheta", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_Phi_10ns20DegreeDeltaThetaUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  xmin =-0.1; 
  xmax = 4.2;
  xtitle = "#Delta #Phi";
  MyCompare("ECAL_DeltaPhi_10ns", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_DeltaPhi_10nsDeltaTheta", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  
  xmin =-0.08; 
  xmax = 0.08;
  xtitle = "#Delta #Theta";
  MyCompare("ECal_DeltaThetaFuncAllCl", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0, -0.01, 0.015);
  MyCompare("ECal_DeltaThetaFuncAllCl_ethr50MeV", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECal_DeltaThetaFuncAllClThr1GeV", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECal_DeltaThetaFuncNmaxCl8", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECal_DeltaThetaFuncNmaxCl4", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECal_DeltaThetaFuncFR_xHiger120", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECal_DeltaThetaFuncFR_xHiger150", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECal_DeltaThetaFuncFR_xHiger150_EthrLess1GeV", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECal_DeltaThetaFuncFR_xLess120", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECal_DeltaThetaFuncAllClThr400MeV1200MeV", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);

  xmin =0.; 
  xmax = 70;
  xtitle = "SAC_{nCl}^{afterCut}";
  MyCompare("SAC_nClAfterCut", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  xmin =0.; 
  xmax = 700;
  xtitle = "E_{#gamma ecal}+E_{#gamma sac} [MeV]";
  MyCompare("AnnihilationGammaEcalGammaSac_5ns", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("AnnihilationGammaEcalGammaSac_5nsEthr50", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  xtitle = "E_{#gamma sac} [MeV]";
  MyCompare("AnnihilationSAConephoton_5ns", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("AnnihilationSAConephoton_5nsEThr50", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("AnnihilationSAConephoton_10ns", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("AnnihilationSAConephoton_10nsEThr50", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);

  xmin =-200; 
  xmax = 1000;
  xtitle = "Missing Mass [MeV^2]";
  MyCompare("MissingMass_AllECALclu", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("MissingMass_AllECALclu_ThrEne50MeV", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("MissingMass_NoClInTime20ns_ThrEne50MeV", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("MissingMass_NoClInTime10ns_ThrEne50MeV", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("MissingMass_NoClInTime20ns_ThrEne50MeV_NoCoincidencePVeto", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("MissingMass_NoClInTime20ns_ThrEne50MeV_NoCoincidencePVeto_NoCoincidenceSAC", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("MissingMass_NoClInTime10ns_ThrEne50MeV_NoCoincidencePVeto", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("MissingMass_NoClInTime10ns_ThrEne50MeV_NoCoincidencePVeto_NoCoincidenceSAC", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,0);
  MyCompare("ECAL_MissingMassAnnihilation_10ns5CoGUnderPeakEThr90", -1, xmin, xmax, xtitle,fT5, fT10, fT15, fT20, fT25,fnoT5, fnoT10, fnoT15, fnoT20, fnoT25,1);

  
}
