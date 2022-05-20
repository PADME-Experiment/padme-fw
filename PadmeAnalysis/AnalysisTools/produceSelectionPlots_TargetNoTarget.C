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

  // 150ns(and 270ns ) of bunch lenght 
  TFile *fT =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/Analysisrun_0030547_20201017_122906_newClasses.root"); 
  TFile *fnoT =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/AnalysisBkgRuns__newClasses.root"); 
 
  std::string xtitle;
  double xmin=-999;
  double xmax= 999;
  
  std::fstream fs;
  fs.open("ggCount_30547.txt", std::fstream::in | std::fstream::out | std::fstream::app);
  time_t now=time(0);
  char* dt =ctime(&now);
  fs << "name          28k      ..."<<dt << std::endl;
  fs.close();

  xtitle = "E(#gamma_{1})+E(#gamma_{2}) [MeV]";
  xmin = 0.;
  xmax = 800.; 
  compare_TargetNoTarget("ECAL_gravTwoPhoton10ns_g1inFRin5Cog", -1, xmin, xmax, xtitle,fT, fnoT, 0, 350, 700);
  compare_TargetNoTarget("ECAL_gravTwoPhoton10ns_g1g2inFRin5Cog", -1, xmin, xmax, xtitle,fT, fnoT, 0, 350, 700);
  compare_TargetNoTarget("ECAL_gravTwoPhoton10ns_g1inFRin20Deg", -1, xmin, xmax, xtitle,fT, fnoT, 0, 350, 700);
  compare_TargetNoTarget("ECAL_gravTwoPhoton10ns_g1g2inFRin20DegThetaCut", -1, xmin, xmax, xtitle,fT, fnoT, 1, 350, 700);
  //compare_TargetNoTarget("", -1, xmin, xmax, xtitle,fT, fnoT, 0, 350, 700);
   

}
