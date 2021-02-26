#include <fstream>
#include "TGraph2D.h"
#include "TTree.h"
#include "TFile.h"
#include "compare_TargetNoTarget.C"
#include "PadmeUtils.h"
#include "PadmeStyle.h"
#include "PadmeLabels.h"
#include <ctime>


void produceSelectionPlots_TargetNoTarget()
{

#ifdef __CINT__
  gROOT->LoadMacro("PadmeLabels.C");
  gROOT->LoadMacro("PadmeUtils.C");
  gROOT->LoadMacro("PadmeStyle.C");
#endif
  
  SetPadmeStyle();

  
  //TFile *fT =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/Analysisrun_0030547_20201017_122906_newClasses.root");
  //  TFile *fT =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/Analysisrun_0030547_20201017_122906_SS2Reco.root"); 
  ///TFile *fT =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/Analysisrun_0030547_20201017_122906_NewVersion_cunOnTime110dueToHighSpread_newRmiddle170_CutOnPOT_newWeightAnnihilation_SSReco.root"); 

  //TFile *fnoT =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/AnalysisBkgRuns__newClasses.root"); 
  //  TFile *fnoT =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/AnalysisBkgRuns.root"); 
  ///TFile *fnoT =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/AnalysisBkgRuns_NewVersion_cunOnTime110dueToHighSpread_newRmiddle170_CutOnPOT_newWeightAnnihilation_SSReco.root"); 

  TFile *fT =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/Analysisrun_0030201_20200721_224335_all_newWheight.root"); //15k pot/bunch
  TFile *fnoT =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/Analysisrun_0030202_20200722_010224_all_newWheight.root"); 

  std::string xtitle;
  double xmin=-999;
  double xmax= 999;
  
  std::fstream fs;
  std::string filetitle="ggCount_30202_newWheight.txt";
  // std::string filetitle="ggCount_30547_NewVersion_cunOnTime110dueToHighSpread_newRmiddle170_CutOnPOT_newWeightAnnihilation.txt";
  fs.open(filetitle.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
  //fs.open("ggCount_30547.txt", std::fstream::in | std::fstream::out | std::fstream::app);
  time_t now=time(0);
  char* dt =ctime(&now);
  //fs << "name          15k      ..."<<dt << std::endl;
  fs << "name          28k      ..."<<dt << std::endl;
  fs.close();

  xtitle = "E(#gamma_{1})+E(#gamma_{2}) [MeV]";
  xmin = 0.;
  xmax = 800.; 
  compare_TargetNoTarget("ECAL_gravTwoPhoton10ns_g1inFRin5Cog", -1, xmin, xmax, xtitle,fT, fnoT, 0, 350, 700, filetitle);
  compare_TargetNoTarget("ECAL_gravTwoPhoton10ns_WEffR1inFR_g1inFRin5Cog", -1, xmin, xmax, xtitle,fT, fnoT, 0, 350, 700, filetitle);
  compare_TargetNoTarget("ECAL_gravTwoPhoton10ns_WEffPlusSysR1inFR_g1inFRin5Cog", -1, xmin, xmax, xtitle,fT, fnoT, 0, 350, 700, filetitle);
  compare_TargetNoTarget("ECAL_gravTwoPhoton10ns_WAccR1inFR_g1inFRin5Cog", -1, xmin, xmax, xtitle,fT, fnoT, 0, 350, 700, filetitle);
  compare_TargetNoTarget("ECAL_gravTwoPhoton10ns_WAccEffR1inFR_g1inFRin5Cog", -1, xmin, xmax, xtitle,fT, fnoT, 0, 350, 700, filetitle);
  compare_TargetNoTarget("ECAL_gravTwoPhoton10ns_WAccEffPlusSysR1inFR_g1inFRin5Cog", -1, xmin, xmax, xtitle,fT, fnoT, 0, 350, 700, filetitle);

  compare_TargetNoTarget("ECAL_gravTwoPhoton10ns_g1g2inFRin5Cog", -1, xmin, xmax, xtitle,fT, fnoT, 0, 350, 700, filetitle);
  compare_TargetNoTarget("ECAL_gravTwoPhoton10ns_WEffR1R2inFR_g1g2inFRin5Cog", -1, xmin, xmax, xtitle,fT, fnoT, 0, 350, 700, filetitle);
  compare_TargetNoTarget("ECAL_gravTwoPhoton10ns_WEffPlusSysR1R2inFR_g1g2inFRin5Cog", -1, xmin, xmax, xtitle,fT, fnoT, 0, 350, 700, filetitle);
  compare_TargetNoTarget("ECAL_gravTwoPhoton10ns_WAccR1R2inFR_g1g2inFRin5Cog", -1, xmin, xmax, xtitle,fT, fnoT, 0, 350, 700, filetitle);
  compare_TargetNoTarget("ECAL_gravTwoPhoton10ns_WAccEffR1R2inFR_g1g2inFRin5Cog", -1, xmin, xmax, xtitle,fT, fnoT, 0, 350, 700, filetitle);
  compare_TargetNoTarget("ECAL_gravTwoPhoton10ns_WAccEffPlusSysR1R2inFR_g1g2inFRin5Cog", -1, xmin, xmax, xtitle,fT, fnoT, 0, 350, 700, filetitle);

  compare_TargetNoTarget("ECAL_gravTwoPhoton10ns_g1inFRin20Deg", -1, xmin, xmax, xtitle,fT, fnoT, 0, 350, 700, filetitle);
  compare_TargetNoTarget("ECAL_gravTwoPhoton10ns_g1g2inFRin20DegThetaCut", -1, xmin, xmax, xtitle,fT, fnoT, 1, 350, 700, filetitle);

  
  

  //compare_TargetNoTarget("", -1, xmin, xmax, xtitle,fT, fnoT, 0, 350, 700);
   

}
