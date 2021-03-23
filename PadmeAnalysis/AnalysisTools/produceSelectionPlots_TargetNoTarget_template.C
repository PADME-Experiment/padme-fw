#include <fstream>
#include "TGraph2D.h"
#include "TTree.h"
#include "TFile.h"
#include "compare_TargetNoTarget.C"
#include "PadmeUtils.h"
#include "PadmeStyle.h"
#include "PadmeLabels.h"
#include <ctime>

double ExtractnPOT(TFile* fTargetIn){
  TH1D* hDN = (TH1D*)fTargetIn->Get("AnnMet_nPOT");
  double nPOTdata = hDN->GetBinContent(2);
  return nPOTdata;
}

double ExtractnPOTratioToReject(TFile* fTargetIn){
  TH1D* hDN = (TH1D*)fTargetIn->Get("SAC_ClusterTime");
  int bin=hDN->FindBin(-110);
  Double_t integral=hDN->Integral(1, bin);
  Double_t ratio=integral/(hDN->GetEntries());
  return ratio;
}


void produceSelectionPlots_TargetNoTarget_template()
{

#ifdef __CINT__
  gROOT->LoadMacro("PadmeLabels.C");
  gROOT->LoadMacro("PadmeUtils.C");
  gROOT->LoadMacro("PadmeStyle.C");
#endif
  
  SetPadmeStyle();


  TFile *fT =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/runsAnalysis/Analysis_CutOnEcalClTimeM110_effextractedFromSameRun_run_0030601_20201110_050805.root"); 
  TFile *fnoT =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/AnalysisBkgRuns_NewVersion_cunOnTime110dueToHighSpread_newRmiddle170_CutOnPOT_effFrom30547run_accSmeared_sortedByEne1.root"); 


  std::string xtitle;
  double xmin=-999;
  double xmax= 999;
  
  std::fstream fs;
   std::string filetitle="ggCount_run_0030601_20201110_050805_NewVersion_cunOnTime110_newRmiddle170_CutOnPOT_EffExtractedUsingThisRun.txt";
   fs.open(filetitle.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
   time_t now=time(0);
  char* dt =ctime(&now);
  fs<<" " << std::endl;
  fs << "myTime "<<dt <<" nPOT " << ExtractnPOT(fT) << " ratio of clusters rejected " << ExtractnPOTratioToReject(fT) << std::endl;
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
  compare_TargetNoTarget("ECAL_gravTwoPhoton10ns_WAccEffPlusSysR1R2inFR_g1g2inFRin5Cog", -1, xmin, xmax, xtitle,fT, fnoT, 1, 350, 700, filetitle);   

}


