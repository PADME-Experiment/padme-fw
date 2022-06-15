#include "PadmeUtils.h"
#include "PadmeStyle.h"
#include "PadmeLabels.h"

#include "compareSideBySide.C"
#include "compare.C"


using namespace std;

void produceSelectionPlotsOnlyFourCuts(TFile* fData=_file0, TFile* fMC=_file1)
{
  
  #ifdef __CINT__
    gROOT->LoadMacro("PadmeLabels.C");
    gROOT->LoadMacro("PadmeUtils.C");
    gROOT->LoadMacro("PadmeStyle.C");
  #endif
    // new TCanvas("c1","c1",800,300);

  std::string Rggdpdt[8]={"ECAL_Rgg_10ns20DegreeDeltaTheta", "ECAL_Rgg_10ns20DegreeDeltaThetaUnderPeak","ECAL_Rgg_10ns20DegreeDeltaThetaUnderPeakEThr90", "ECAL_Rgg_10ns20DegreeDeltaThetaEThr90", "ECAL_Rgg_10ns5DegreeDeltaTheta", "ECAL_Rgg_10ns5DegreeDeltaThetaUnderPeak","ECAL_Rgg_10ns5DegreeDeltaThetaUnderPeakEThr90", "ECAL_Rgg_10ns5DegreeDeltaThetaEThr90"};
  
  std::string Rggcog[8]={"ECAL_Rgg_10ns3CoG","ECAL_Rgg_10ns3CoGUnderPeak","ECAL_Rgg_10ns3CoGUnderPeakEThr90","ECAL_Rgg_10ns3CoGEThr90","ECAL_Rgg_10ns1CoG","ECAL_Rgg_10ns1CoGUnderPeak","ECAL_Rgg_10ns1CoGUnderPeakEThr90","ECAL_Rgg_10ns1CoGEThr90"};
  
  std::string Xggdpdt[8]={"ECAL_Xgg_10ns20DegreeDeltaTheta","ECAL_Xgg_10ns20DegreeDeltaThetaUnderPeak","ECAL_Xgg_10ns20DegreeDeltaThetaUnderPeakEThr90","ECAL_Xgg_10ns20DegreeDeltaThetaEThr90","ECAL_Xgg_10ns5DegreeDeltaTheta","ECAL_Xgg_10ns5DegreeDeltaThetaUnderPeak", 
  "ECAL_Xgg_10ns5DegreeDeltaThetaUnderPeakEThr90", "ECAL_Xgg_10ns5DegreeDeltaThetaEThr90"};
  
  std::string Xggcog[8]={"ECAL_Xgg_10ns1CoG","ECAL_Xgg_10ns1CoGUnderPeak","ECAL_Xgg_10ns1CoGUnderPeakEThr90","ECAL_Xgg_10ns1CoGEThr90","ECAL_Xgg_10ns3CoG","ECAL_Xgg_10ns3CoGUnderPeak","ECAL_Xgg_10ns3CoGUnderPeakETHr90","ECAL_Xgg_10ns3CoGEThr90"};
  //std::string Xggcog[6]={"ECAL_Xgg_10ns3CoG","ECAL_Xgg_10ns3CoGUnderPeak","ECAL_Xgg_10ns3CoGEThr90","ECAL_Xgg_10ns1CoG","ECAL_Xgg_10ns1CoGUnderPeak","ECAL_Xgg_10ns1CoGEThr90"};
  
  
  std::string Yggdpdt[8]={"ECAL_Ygg_10ns20DegreeDeltaTheta","ECAL_Ygg_10ns20DegreeDeltaThetaUnderPeak","ECAL_Ygg_10ns20DegreeDeltaThetaUnderPeakEThr90","ECAL_Ygg_10ns20DegreeDeltaThetaEThr90","ECAL_Ygg_10ns5DegreeDeltaTheta","ECAL_Ygg_10ns5DegreeDeltaThetaUnderPeak",
  "ECAL_Ygg_10ns5DegreeDeltaThetaUnderPeakEThr90", "ECAL_Ygg_10ns5DegreeDeltaThetaEThr90"};
  
  std::string Yggcog[8]={"ECAL_Ygg_10ns3CoG","ECAL_Ygg_10ns3CoGUnderPeak","ECAL_Ygg_10ns3CoGUnderPeakEThr90","ECAL_Ygg_10ns3CoGEThr90","ECAL_Ygg_10ns1CoG","ECAL_Ygg_10ns1CoGUnderPeak","ECAL_Ygg_10ns1CoGUnderPeakEThr90","ECAL_Ygg_10ns1CoGEThr90"};
  
  
  std::string Eggdpdt[8]={"ECAL_Egg_10ns20DegreeDeltaTheta","ECAL_Egg_10ns20DegreeDeltaThetaUnderPeak","ECAL_Egg_10ns20DegreeDeltaThetaUnderPeakEThr90","ECAL_Egg_10ns20DegreeDeltaThetaEThr90","ECAL_Egg_10ns5DegreeDeltaTheta","ECAL_Egg_10ns5DegreeDeltaThetaUnderPeak",
  "ECAL_Egg_10ns5DegreeDeltaThetaUnderPeakEThr90", "ECAL_Egg_10ns5DegreeDeltaThetaEThr90"};
  
  std::string Eggcog[8]={"ECAL_Egg_10ns3CoG","ECAL_Egg_10ns3CoGUnderPeak","ECAL_Egg_10ns3CoGUnderPeakEThr90","ECAL_Egg_10ns3CoGEThr90","ECAL_Egg_10ns1CoG","ECAL_Egg_10ns1CoGUnderPeak","ECAL_Egg_10ns1CoGUnderPeakEThr90","ECAL_Egg_10ns1CoGEThr90"};
  
  std::string ggdpdt[9]={"ECAL_gravTwoPhoton10ns_InDeltaPhi20Degree2Method_DeltaTheta","ECAL_twoPhotonInTime10ns20DegreeDeltaThetaUnderPeak","ECAL_twoPhotonInTime10ns20DegreeDeltaThetaUnderPeakEThr90","ECAL_twoPhotonInTime10ns20DegreeDeltaThetaUnderPeakEThr90_FRY","ECAL_twoPhotonInTime10ns20DegreeDeltaThetaEThr90", "ECAL_gravTwoPhoton10ns_InDeltaPhi5Degree2Method_DeltaTheta", "ECAL_twoPhotonInTime10ns5DegreeDeltaThetaUnderPeak","ECAL_twoPhotonInTime10ns5DegreeDeltaThetaUnderPeakEThr90","ECAL_twoPhotonInTime10ns5DegreeDeltaThetaEThr90"};
  
  std::string ggcog[9]={"ECAL_gravTwoPhoton10ns3cm","ECAL_gravTwoPhoton10ns3cmUnderPeak", "ECAL_gravTwoPhoton10ns3cmUnderPeakEThr90", "ECAL_gravTwoPhoton10ns3cmUnderPeakEThr90_FRY","ECAL_gravTwoPhoton10ns3cmEThr90", "ECAL_gravTwoPhoton10ns1cm","ECAL_gravTwoPhoton10ns1cmUnderPeak","ECAL_gravTwoPhoton10ns1cmUnderPeakEThr90","ECAL_gravTwoPhoton10ns1cmEThr90"};
  
  
  std::string ggdpdtFR[6]={"ECAL_twoPhotonInTime10ns20DegreeDeltaThetaOut4Cry134_216","ECAL_twoPhotonInTime10ns20DegreeDeltaThetaOut4Cry134_216UnderPeak","ECAL_twoPhotonInTime10ns20DegreeDeltaThetaOut4Cry134_216EThr90", "ECAL_twoPhotonInTime10ns5DegreeDeltaThetaOut4Cry134_216","ECAL_twoPhotonInTime10ns5DegreeDeltaThetaOut4Cry134_216UnderPeak","ECAL_twoPhotonInTime10ns5DegreeDeltaThetaOut4Cry134_216EThr90"};
  
  std::string ggcogFR[6]={"ECAL_twoPhotonInTime10ns1CoGOut4Cry134_216","ECAL_twoPhotonInTime10ns1CoGOut4Cry134_216UnderPeak","ECAL_twoPhotonInTime10ns1CoGOut4Cry134_216EThr90","ECAL_twoPhotonInTime10ns3CoGOut4Cry134_216", "ECAL_twoPhotonInTime10ns3CoGOut4Cry134_216UnderPeak","ECAL_twoPhotonInTime10ns3CoGOut4Cry134_216EThr90"};
  

std::string time[6]={ "ECal_HitTime",	"ECal_ClusterTime","ECAL_tgg_10ns20DegreeDeltaThetaUnderPeakEThr90", "ECAL_tgg_10ns5DegreeDeltaThetaUnderPeakEThr90", "ECAL_tgg_10ns1CoGUnderPeakEThr90", 	"ECAL_tgg_10ns3CoGUnderPeakEThr90"};

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


  //2d histos
  
  std::string xedpdt[9]={"ECAL_XggVsEgg_10ns20DegreeDeltaTheta","ECAL_XggVsEgg_10ns20DegreeDeltaThetaUnderPeak","ECAL_XggVsEgg_10ns20DegreeDeltaThetaUnderPeakEThr90","ECAL_XggVsEgg_10ns20DegreeDeltaThetaUnderPeakEThr90_FRY","ECAL_XggVsEgg_10ns20DegreeDeltaThetaEThr90","ECAL_XggVsEgg_10ns5DegreeDeltaTheta", "ECAL_XggVsEgg_10ns5DegreeDeltaThetaUnderPeak","ECAL_XggVsEgg_10ns5DegreeDeltaThetaUnderPeakEThr90", "ECAL_XggVsEgg_10ns5DegreeDeltaThetaEThr90"};

  std::string yedpdt[9]={"ECAL_YggVsEgg_10ns20DegreeDeltaTheta","ECAL_YggVsEgg_10ns20DegreeDeltaThetaUnderPeak","ECAL_YggVsEgg_10ns20DegreeDeltaThetaUnderPeakEThr90","ECAL_YggVsEgg_10ns20DegreeDeltaThetaUnderPeakEThr90_FRY","ECAL_YggVsEgg_10ns20DegreeDeltaThetaEThr90","ECAL_YggVsEgg_10ns5DegreeDeltaTheta", "ECAL_YggVsEgg_10ns5DegreeDeltaThetaUnderPeak","ECAL_YggVsEgg_10ns5DegreeDeltaThetaUnderPeakEThr90", "ECAL_YggVsEgg_10ns5DegreeDeltaThetaEThr90"};
  
  std::string xecog[9]={"ECAL_XggVsEgg_10ns3CoG","ECAL_XggVsEgg_10ns3CoGUnderPeak","ECAL_XggVsEgg_10ns3CoGUnderPeakEThr90","ECAL_XggVsEgg_10ns3CoGUnderPeakEThr90_FRY","ECAL_XggVsEgg_10ns3CoGEThr90","ECAL_XggVsEgg_10ns1CoG","ECAL_XggVsEgg_10ns1CoGUnderPeak","ECAL_XggVsEgg_10ns1CoGUnderPeakEThr90","ECAL_XggVsEgg_10ns1CoGEThr90"};
  
  std::string yecog[9]={"ECAL_YggVsEgg_10ns3CoG","ECAL_YggVsEgg_10ns3CoGUnderPeak","ECAL_YggVsEgg_10ns3CoGUnderPeakEThr90","ECAL_YggVsEgg_10ns3CoGUnderPeakEThr90_FRY","ECAL_YggVsEgg_10ns3CoGEThr90","ECAL_YggVsEgg_10ns1CoG","ECAL_YggVsEgg_10ns1CoGUnderPeak","ECAL_YggVsEgg_10ns1CoGUnderPeakEThr90","ECAL_YggVsEgg_10ns1CoGEThr90"};
  
  std::string redpdt[9]={"ECAL_RggVsEgg_10ns20DegreeDeltaTheta","ECAL_RggVsEgg_10ns20DegreeDeltaThetaUnderPeak","ECAL_RggVsEgg_10ns20DegreeDeltaThetaUnderPeakEThr90","ECAL_RggVsEgg_10ns20DegreeDeltaThetaUnderPeakEThr90_FRY","ECAL_RggVsEgg_10ns20DegreeDeltaThetaEThr90","ECAL_RggVsEgg_10ns5DegreeDeltaTheta", "ECAL_RggVsEgg_10ns5DegreeDeltaThetaUnderPeak","ECAL_RggVsEgg_10ns5DegreeDeltaThetaUnderPeakEThr90", "ECAL_RggVsEgg_10ns5DegreeDeltaThetaEThr90"};
  
  std::string recog[9]={"ECAL_RggVsEgg_10ns3CoG","ECAL_RggVsEgg_10ns3CoGUnderPeak","ECAL_RggVsEgg_10ns3CoGUnderPeakEThr90","ECAL_RggVsEgg_10ns3CoGUnderPeakEThr90_FRY","ECAL_RggVsEgg_10ns3CoGEThr90", "ECAL_RggVsEgg_10ns1CoG","ECAL_RggVsEgg_10ns1CoGUnderPeak","ECAL_RggVsEgg_10ns1CoGUnderPeakEThr90","ECAL_RggVsEgg_10ns1CoGEThr90"};
  
 std::string mapdpdt[9]={"MAPgg_10ns20DegreeDeltaTheta" ,  "MAPgg_10ns20DegreeDeltaThetaUnderPeak" , "MAPgg_10ns20DegreeDeltaThetaUnderPeakEThr90" ,  "MAPgg_10ns20DegreeDeltaThetaUnderPeakEThr90_FRY" ,   "MAPgg_10ns20DegreeDeltaThetaEThr90"  ,"MAPgg_10ns5DegreeDeltaTheta" ,  "MAPgg_10ns5DegreeDeltaThetaUnderPeak" , "MAPgg_10ns5DegreeDeltaThetaUnderPeakEThr90", "MAPgg_10ns5DegreeDeltaThetaEThr90" };
 /*
 std::string mapWdpdt[8]={"MAPggW_10ns20DegreeDeltaTheta", "MAPggW_10ns5DegreeDeltaTheta", "MAPggW_10ns20DegreeDeltaThetaUnderPeak" ,"MAPggW_10ns20DegreeDeltaThetaUnderPeakEThr90" , "MAPggW_10ns5DegreeDeltaThetaUnderPeak" , "MAPggW_10ns5DegreeDeltaThetaUnderPeakEThr90", "MAPggW_10ns20DegreeDeltaThetaEThr90", "MAPggW_10ns5DegreeDeltaThetaEThr90" };
*/
 std::string mapcog[9]={  "MAPgg_10ns3CoG" , "MAPgg_10ns3CoGUnderPeak", "MAPgg_10ns3CoGUnderPeakEThr90" ,"MAPgg_10ns3CoGUnderPeakEThr90_FRY" ,  "MAPgg_10ns3CoGEThr90", "MAPgg_10ns1CoG" , "MAPgg_10ns1CoGUnderPeak" , "MAPgg_10ns1CoGUnderPeakEThr90", "MAPgg_10ns1CoGEThr90" };
/*
 std::string mapWcog[8]={ "MAPggW_10ns1CoG" ,"MAPggW_10ns3CoG" , "MAPggW_10ns1CoGUnderPeak", "MAPggW_10ns1CoGUnderPeakEThr90" , "MAPgg_10ns3CoGUnderPeak" , "MAPggW_10ns3CoGUnderPeak" , "MAPgg_10ns3CoGUnderPeakEThr90" , "MAPggW_10ns3CoGUnderPeakEThr90" , "MAPgg_10ns1CoGEThr90" , "MAPggW_10ns1CoGEThr90" , "MAPgg_10ns3CoGEThr90" , "MAPggW_10ns3CoGEThr90"};
*/

  int lastSet=0;
  Bool_t MakeDifference=true;
 
  
  compareSideBySide("MAPgg_10ns1CoGUnderPeakEThr90", fData, fMC);
  //compareSideBySide("ECAL_RggVsEgg_10ns1CoG",fData,fMC);
  
  std::string xtitle;
  double xmin=-999;
  double xmax= 999;
  
  SetPadmeStyle();  
 
  
  xtitle = "E(#gamma_{1})+E(#gamma_{2}) [MeV]";
  xmin = 0.;
  xmax = 900.;
  compare("ECAL_gravTwoPhoton10ns1cm", -1, xmin, xmax, xtitle, fData, fMC); 
 

  xmin = -120.;
  xmax =  60.;
  xtitle = "t [ns]";
  compare("ECAL_tgg_10ns1CoGUnderPeakEThr90", -1, xmin, xmax, xtitle, fData, fMC); 
    
}

