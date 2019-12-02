#include "PadmeUtils.h"
#include "PadmeStyle.h"
#include "PadmeLabels.h"

#include "compareSideBySide.C"
#include "compare.C"


using namespace std;

void produceSelectionPlots(TFile* fData=_file0, TFile* fMC=_file1)
{
  
  #ifdef __CINT__
    gROOT->LoadMacro("PadmeLabels.C");
    gROOT->LoadMacro("PadmeUtils.C");
    gROOT->LoadMacro("PadmeStyle.C");
  #endif


    
  compareSideBySide("SS2gEvsR_passDt", fData, fMC);
  compareSideBySide("SS2gEvsR_passDtDphi", fData, fMC);
  compareSideBySide("SS2gEvsR_passDtDphiCog", fData, fMC);
  compareSideBySide("SS2gEvsR_passDtDphiCogFR", fData, fMC);
  compareSideBySide("SS2gEvsR_passDtDphiCogDsume", fData, fMC);   

  compareSideBySide("SS2gEvsR_sClu_FRphi0", fData, fMC);
  compareSideBySide("SS2gEvsR_sClu_FRphi45", fData, fMC);
  compareSideBySide("SS2gEvsR_sClu_FRphi90", fData, fMC);
  compareSideBySide("SS2gEvsR_sClu_FRphi135", fData, fMC);
  compareSideBySide("SS2gEvsR_sClu_FRphi180", fData, fMC);   
  compareSideBySide("SS2gEvsR_sClu_FRphi225", fData, fMC);   
  compareSideBySide("SS2gEvsR_sClu_FRphi270", fData, fMC);   
  compareSideBySide("SS2gEvsR_sClu_FRphi315", fData, fMC);   


  compareSideBySide("SS2gXY_inSR", fData, fMC);
  compareSideBySide("SS2gEwXY_inSR", fData, fMC);
  compareSideBySide("SS2gDtVsSumE_inSR", fData, fMC);
  compareSideBySide("SS2gDphiVsSumE_inSR", fData, fMC);
  compareSideBySide("SS2gXcogVsSumE_inSR", fData, fMC);
  compareSideBySide("SS2gYcogVsSumE_inSR", fData, fMC);
  compareSideBySide("SS2gEVsE_inSR", fData, fMC);
  compareSideBySide("SS2gEVsE_passDt", fData, fMC);
  compareSideBySide("SS2gEVsE_passDtDphi", fData, fMC);
  compareSideBySide("SS2gEVsE_passDtDphiCog", fData, fMC);
  compareSideBySide("SS2gEVsE_passDtDphiCog", fData, fMC);
  compareSideBySide("SS2gEVsE_passDtDphiCogFR", fData, fMC);
  compareSideBySide("SS2gEVsE_passDtDphiCogDsume", fData, fMC);


  compareSideBySide("SS2gXYEw_passDt", fData, fMC);
  compareSideBySide("SS2gXYEw_passDtDphi", fData, fMC);
  compareSideBySide("SS2gXYEw_passDtDphiCog", fData, fMC);
  compareSideBySide("SS2gXYEw_passDtDphiCogFR", fData, fMC);
  compareSideBySide("SS2gXYEw_passDtDphiCogDsume", fData, fMC);

  compareSideBySide("SS2gXY_passDt", fData, fMC);
  compareSideBySide("SS2gXY_passDtDphi", fData, fMC);
  compareSideBySide("SS2gXY_passDtDphiCog", fData, fMC);
  compareSideBySide("SS2gXY_passDtDphiCogFR", fData, fMC);
  compareSideBySide("SS2gXY_passDtDphiCogDsume", fData, fMC);

  
  //return;

  SetPadmeStyle();


  std::string xtitle;
  double xmin=-999;
  double xmax= 999;

  

  xtitle = "Number of ECal hits";
  xmin = 0.;
  xmax = 400.;
  compare("SS2g_nhits",                    -1, xmin, xmax, xtitle, fData, fMC);
  xtitle = "Cluster size";
  xmin = -999;
  xmax =  999;
  compare("SS2g_clSize",                    -1, xmin, xmax, xtitle, fData, fMC);

  xtitle = "Number of positrons/bunch";
  xmin = 0.;
  xmax = 30000.;
  compare("NposInBunch_beam",                1, xmin, xmax, xtitle, fData, fMC);
  xtitle = "Number of positrons/bunch - SR";
  xmin = 0.;
  xmax = 30000.;
  compare("NposInBunch_beam_passDtDphiCogDsume",  -1, xmin, xmax, xtitle, fData, fMC);


  
  //return;

  xtitle = "E(#gamma_{1})+E(#gamma_{2}) [MeV]";
  xmin = 0.;
  xmax = 1200.;
  compare("SS2gSumE_passDt",                    -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gSumE_passDtDphi",                -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gSumE_passDtDphiCog",             -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gSumE_passDtDphiCogFR",           -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gSumE_passDtDphiCogDsume",        -1, 400, 600, xtitle, fData, fMC);
  compare("SS2gSumE_inSR_Dt10Dphi160",          -1, xmin, xmax, xtitle, fData, fMC);


  xtitle = "E(#gamma_{1})+E(#gamma_{2}) [MeV] (phi=0)";
  compare("SS2gSumE_passDtDphiCogFR0",           -1, xmin, xmax, xtitle, fData, fMC);
  xtitle = "E(#gamma_{1})+E(#gamma_{2}) [MeV] (phi=45)";
  compare("SS2gSumE_passDtDphiCogFR45",           -1, xmin, xmax, xtitle, fData, fMC);
  xtitle = "E(#gamma_{1})+E(#gamma_{2}) [MeV] (phi=90)";
  compare("SS2gSumE_passDtDphiCogFR90",           -1, xmin, xmax, xtitle, fData, fMC);
  xtitle = "E(#gamma_{1})+E(#gamma_{2}) [MeV] (phi=135)";
  compare("SS2gSumE_passDtDphiCogFR135",           -1, xmin, xmax, xtitle, fData, fMC);
 
  xmin = 180.;
  xmax = 700.;
  xtitle = "E(#gamma_{1})+E(#gamma_{2}) [MeV] (Dt<10)";
  compare("SS2gSumE_inSR_Dt10", -1, xmin, xmax, xtitle, fData, fMC);
  xtitle = "E(#gamma_{1})+E(#gamma_{2}) [MeV] (Dt<10, Cog<30)";
  compare("SS2gSumE_inSR_Dt10Cog30", -1, xmin, xmax, xtitle, fData, fMC);
  xtitle = "E(#gamma_{1})+E(#gamma_{2}) [MeV] (Dt<10, Dphi>160)";
  compare("SS2gSumE_inSR_Dt10Dphi160", -1, xmin, xmax, xtitle, fData, fMC);
  xtitle = "E(#gamma_{1})+E(#gamma_{2}) [MeV] (Dphi>160, Cog<30)";
  compare("SS2gSumE_inSR_Dphi160Cog30", -1, xmin, xmax, xtitle, fData, fMC);


  //compare("SS2gSumE_passDtDphiCogDsume",        0.38, 400., 600, xtitle, fData, fMC);
  //compare("SS2gSumE_passDtDphiCogDsume",        0.2144, 400., 600, xtitle, fData, fMC);

  xtitle = "R(#gamma_{1,2}) [mm]";
  xmin = -999;
  xmax =  999;
  compare("SS2gR_passDt",                    -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gR_passDtDphi",                -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gR_passDtDphiCog",             -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gR_passDtDphiCogFR",           -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gR_passDtDphiCogDsume",        -1, xmin, xmax, xtitle, fData, fMC);
  //compare("SS2gSumE_passDtDphiCogDsume",        0.38, 400., 600, xtitle, fData, fMC);
  //compare("SS2gSumE_passDtDphiCogDsume",        0.2144, 400., 600, xtitle, fData, fMC);
  
  xtitle = "#phi(#gamma_{1,2}) [rad]";
  xmin = -999;
  xmax =  999;
  compare("SS2gPhi_passDt",                    -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gPhi_passDtDphi",                -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gPhi_passDtDphiCog",             -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gPhi_passDtDphiCogFR",           -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gPhi_passDtDphiCogDsume",        -1, xmin, xmax, xtitle, fData, fMC);


  xtitle = "#Deltat [ns]";
  xmin = -15.;
  xmax =  15.;
  compare("SS2gDt_passDtDphi",                -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gDt_passDtDphiCog",             -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gDt_passDtDphiCogFR",           -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gDt_passDtDphiCogDsume",        -1, xmin, xmax, xtitle, fData, fMC);

  //  return;

  xmin = -150.;
  xmax =  150.;
  xtitle = "#Deltat (hit-hit, ECal) [ns]";
  compare("TimeSpreadInECal",                 -1, xmin, xmax, xtitle, fData, fMC);
  xmin = -15.;
  xmax =  15.;
  xtitle = "#Deltat (in ECal clusters) [ns]";
  compare("TimeSpreadInECalClus",             -1, xmin, xmax, xtitle, fData, fMC);

  xmin = -100.;
  xmax =  400.;
  xtitle = "T(#gamma_{1}) (E>50MeV)[ns]";
  compare("SS2g_ClTime",                      -1, xmin, xmax, xtitle, fData, fMC);
  xtitle = "T(#gamma_{1}) (#gamma#gamma candidate)[ns]";
  compare("SS2g_ClTime_passDtDphiCogDsume",   -1, xmin, xmax, xtitle, fData, fMC);

  xmin=50.;
  xmax=500.;
  xtitle = "E(#gamma) [MeV]";
  compare("SS2gE_passDtDphiCogDsume",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gE_passDtDphiCogFR",      -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gE_passDtDphiCog",        -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gE_passDtDphi",           -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gE_passDt",               -1, xmin, xmax, xtitle, fData, fMC);

  xtitle = "#Delta#phi [deg]";
  xmax=180.2;
  compare("SS2gDphi_passDt",   -1, xmin, xmax, xtitle, fData, fMC);
  xmin=129.8;
  compare("SS2gDphi_passDtDphi",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gDphi_passDtDphiCog",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gDphi_passDtDphiCogFR",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gDphi_passDtDphiCogDsume",   -1, xmin, xmax, xtitle, fData, fMC);

  xmin = -250.;
  xmax =  250.;
  xtitle = "X centroid [mm]";
  compare("SS2gXcog_passDt",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gXcog_passDtDphi",   -1, xmin, xmax, xtitle, fData, fMC);
  xmin = -40.;
  xmax =  40.;
  compare("SS2gXcog_passDtDphiCogFR",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gXcog_passDtDphiCogDsume",   -1, xmin, xmax, xtitle, fData, fMC);


  xmin = -250.;
  xmax =  250.;
  xtitle = "Y centroid [mm]";
  compare("SS2gYcog_passDt",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gYcog_passDtDphi",   -1, xmin, xmax, xtitle, fData, fMC);
  xmin = -40.;
  xmax =  40.;
  compare("SS2gYcog_passDtDphiCogFR",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gYcog_passDtDphiCogDsume",   -1, xmin, xmax, xtitle, fData, fMC);


  xmin = -350.;
  xmax =  350.;
  xtitle = "X [mm]";
  compare("SS2gX_passDt",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gX_passDtDphi",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gX_passDtDphiCog",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gX_passDtDphiCogFR",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gX_passDtDphiCogDsume",   -1, xmin, xmax, xtitle, fData, fMC);


  xtitle = "Y [mm]";
  compare("SS2gY_passDt",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gY_passDtDphi",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gY_passDtDphiCog",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gY_passDtDphiCogFR",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gY_passDtDphiCogDsume",   -1, xmin, xmax, xtitle, fData, fMC);

  xmin = -350.;
  xmax =  350.;
  xtitle = "X (energy weigthed) [mm]";
  compare("SS2gXEw_passDt",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gXEw_passDtDphi",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gXEw_passDtDphiCog",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gXEw_passDtDphiCogFR",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gXEw_passDtDphiCogDsume",   -1, xmin, xmax, xtitle, fData, fMC);


  xtitle = "Y (energy weigthed) [mm]";
  compare("SS2gYEw_passDt",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gYEw_passDtDphi",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gYEw_passDtDphiCog",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gYEw_passDtDphiCogFR",   -1, xmin, xmax, xtitle, fData, fMC);
  compare("SS2gYEw_passDtDphiCogDsume",   -1, xmin, xmax, xtitle, fData, fMC);


}

