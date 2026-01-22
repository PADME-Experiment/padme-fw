#include "DarkPhoton.hh"
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

static DarkPhoton ThisAnalyser("DarkPhoton");

DarkPhoton::DarkPhoton(const char *s, int valid, int verb)
    : PadmeVAnalyser(s, valid, verb)
{
  // filecoord = "gamma_gamma_coord_" + std::to_string(getpid()) + ".txt";
  // fcoordout.open(filecoord);
}

DarkPhoton::DarkPhoton(const char *s) : PadmeVAnalyser(s)
{
  filecoord = "gamma_gamma_coord_" + std::to_string(getpid()) + ".txt";
  fcoordout.open(filecoord);

  fProcessTarget = false;
  fProcessPVeto = true;
  fPVetoEVetoCheck = false;
  fProcessEVeto = false;
  fProcessHEPVeto =  false;
  fProcessSAC = false;
  fSACPVetoBrems = false;
  fSACHEPVetoBrems = false;
  fSACEVetoCheck = false;
  fECalPVetoBrems = true;
  fECalHEPVetoBrems = true;
  fECalEVetoCheck = false;
  fECalSACCheck = true;
  fDarkPhotonHunt = true;
  fUBosonDecayHunt = false;
  fProcessGG = false;

  //for rejection
  PVetoIsolationWindow = 1.0;
  HEPVetoIsolationWindow = 2.0;
  fECalIsolationWindow = 2.0;
  SACPVetoIsolationWindow = 2.0;

  //for selection
  SACPVetoTimeWindow = 1.0;
  SACHEPVetoTimeWindow = 1.0;
  SACEVetoTimeWindow = 1.0;
  ECalPVetoInTimeWindow = 0.5;
  ECalHEPVetoInTimeWindow = 1.0;
  ECalEVetoInTimeWindow = 1.0;
  MaxTimeDiffGG = 1.0;
  ECalSACInTimeWindow = 5.0;

  fECalMinEnergy = 50.0;

  ShiftECalSac=3.2-3.11;
  ShiftECalPVeto=3.6-3.3;
  shiftMomentumsacPVeto=80.0;

  DebugMin = 0.0;
  DebugMax = 500.0;

}


Bool_t DarkPhoton::InitHistos()
{

  HistoSvc *hSvcVal = HistoSvc::GetInstance();

  hSvcVal->makeFileDir(GetName());

  if(fProcessTarget) BookTargetHistos();
  if (fProcessPVeto)
    BookPVetoHistos();
  if (fPVetoEVetoCheck)
    BookPVetoEVetoCheckHistos();
  if (fProcessEVeto)
    BookEVetoHistos();
  if (fProcessHEPVeto)
    BookHEPVetoHistos();
  if (fProcessSAC)
    BookSACHistos();
  if (fSACPVetoBrems)
    BookSACPVetoBremsHistos();
  if (fSACHEPVetoBrems)
    BookSACHEPVetoBremsHistos();
  if (fSACEVetoCheck)
    BookSACEVetoCheckHistos();
  if (fECalPVetoBrems)
    BookECalPVetoBremsHistos();
  if (fECalHEPVetoBrems)
    BookECalHEPVetoBremsHistos();
  if (fECalEVetoCheck)
    BookECalEVetoCheckHistos();
  //if (fECalSACCheck)
    BookECalSACCheckHistos();
  if (fDarkPhotonHunt) {
    BookDarkPhotonHistos();
    BookDarkPhotonDebugHistos();
  }
  if (fUBosonDecayHunt)
    BookUBosonDecayHistos();
  if (fProcessGG)
    BookGGHistos();

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
  hSvcVal->BookHistoProf("ECal_Cluster_Energy_Radius", 500, 0.0, 500.0, 0.0, 500.0);
 
  // ECal isolation
  hSvcVal->BookHisto("ECal_2clu_DeltaT", 800, -400.0, 400.0);
  hSvcVal->BookHisto("ECal_2clu_DeltaT_closest", 800, -400.0, 400.0);

  return true;
}
 

void DarkPhoton::BookTargetHistos(){
  HistoSvc *hSvcVal = HistoSvc::GetInstance();

  //hSvcVal->makeFileDir("Target");

  hSvcVal->BookHisto("Target_NPots", 50000, 0.0, 50000.0);
  hSvcVal->BookHisto("Target_X", 50, -25.0, 25.0);
  hSvcVal->BookHisto("Target_Y", 50, -25.0, 25.0);
  hSvcVal->BookHisto2("Target_XY", 50, -25.0, 25.0, 50, -25.0, 25.0);


}

void DarkPhoton::BookPVetoHistos(){

  HistoSvc *hSvcVal = HistoSvc::GetInstance();
  //hSvcVal->makeFileDir("PVeto");

  hSvcVal->BookHisto("PVeto_NHits", 1000, 0.0, 1000.0);
  hSvcVal->BookHisto("PVeto_NClusters", 100, 0.0, 100.0);
  hSvcVal->BookHisto("PVeto_Hit_Energy", 100, 0.0, 50.0);
  hSvcVal->BookHisto("PVeto_Hit_ChID", 100, 0.0, 100.0);
  hSvcVal->BookHisto("PVeto_Cluster_Energy", 500, 0.0, 500.0);
  hSvcVal->BookHisto("PVeto_Cluster_Time", 800, -400.0, 400.0);
  hSvcVal->BookHisto2("PVeto_Cluster_Time_Energy", 800, -400.0, 400.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto("PVeto_Cluster_Z", 86, -447.0, 500.0);
  hSvcVal->BookHisto("PVeto_Cluster_ChID", 100, 0.0, 100.0);
  hSvcVal->BookHisto2("PVeto_Cluster_Time_Z", 800, -400.0, 400.0, 86, -447.0, 500.0);
  hSvcVal->BookHisto2("PVeto_Cluster_Time_ChID", 800, -400.0, 400.0, 100, 0.0, 100.0);
  hSvcVal->BookHisto2("PVeto_Cluster_Energy_Z", 500, 0.0, 500.0, 86, -447.0, 500.0);
  hSvcVal->BookHisto2("PVeto_Cluster_Energy_ChID", 500, 0.0, 500.0, 100, 0.0, 100.0);

}

void DarkPhoton::BookPVetoEVetoCheckHistos(){

  HistoSvc *hSvcVal = HistoSvc::GetInstance();
  //hSvcVal->makeFileDir("PVetoEVetoCheck");

  hSvcVal->BookHisto("PVeto_EVeto_DeltaT", 800, -400.0, 400.0);
  hSvcVal->BookHisto("PVeto_EVeto_DeltaT_closest", 800, -400.0, 400.0);
  hSvcVal->BookHisto("Golden_PVeto_EVeto_TimeDiff", 800, -400.0, 400.0);


}

void DarkPhoton::BookEVetoHistos(){

  HistoSvc *hSvcVal = HistoSvc::GetInstance();
  //hSvcVal->makeFileDir("EVeto");

  hSvcVal->BookHisto("EVeto_NHits", 1000, 0.0, 1000.0);
  hSvcVal->BookHisto("EVeto_NClusters", 100, 0.0, 100.0);
  hSvcVal->BookHisto("EVeto_Hit_Energy", 100, 0.0, 50.0);
  hSvcVal->BookHisto("EVeto_Cluster_Energy", 500, 0.0, 500.0);
  hSvcVal->BookHisto("EVeto_Cluster_Time", 800, -400.0, 400.0);
  hSvcVal->BookHisto2("EVeto_Cluster_Time_Energy", 800, -400.0, 400.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto("EVeto_Cluster_Z", 86, -447.0, 500.0);
  hSvcVal->BookHisto("EVeto_Cluster_ChID", 100, 0.0, 100.0);
  hSvcVal->BookHisto2("EVeto_Cluster_Time_Z", 800, -400.0, 400.0, 86, -447.0, 500.0);
  hSvcVal->BookHisto2("EVeto_Cluster_Time_ChID", 800, -400.0, 400.0, 100, 0.0, 100.0);
  hSvcVal->BookHisto2("EVeto_Cluster_Energy_Z", 500, 0.0, 500.0, 86, -447.0, 500.0);
  hSvcVal->BookHisto2("EVeto_Cluster_Energy_ChID", 500, 0.0, 500.0, 100, 0.0, 100.0);

}

void DarkPhoton::BookHEPVetoHistos(){

  HistoSvc *hSvcVal = HistoSvc::GetInstance();
  //hSvcVal->makeFileDir("HEPVeto");

  hSvcVal->BookHisto("HEPVeto_NHits", 1000, 0.0, 1000.0);
  hSvcVal->BookHisto("HEPVeto_NClusters", 100, 0.0, 100.0);
  hSvcVal->BookHisto("HEPVeto_Hit_Energy", 100, 0.0, 50.0);
  hSvcVal->BookHisto("HEPVeto_Cluster_Energy", 500, 0.0, 500.0);
  hSvcVal->BookHisto("HEPVeto_Cluster_Time", 800, -400.0, 400.0);
  hSvcVal->BookHisto2("HEPVeto_Cluster_Time_Energy", 800, -400.0, 400.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto("HEPVeto_Cluster_X", 86, -447.0, 500.0);
  hSvcVal->BookHisto("HEPVeto_Cluster_ChID", 100, 0.0, 100.0);
  hSvcVal->BookHisto2("HEPVeto_Cluster_Time_X", 800, -400.0, 400.0, 86, -447.0, 500.0);
  hSvcVal->BookHisto2("HEPVeto_Cluster_Time_ChID", 800, -400.0, 400.0, 100, 0.0, 100.0);
  hSvcVal->BookHisto2("HEPVeto_Cluster_Energy_X", 500, 0.0, 500.0, 86, -447.0, 500.0);
  hSvcVal->BookHisto2("HEPVeto_Cluster_Energy_ChID", 500, 0.0, 500.0, 100, 0.0, 100.0);

}

void DarkPhoton::BookSACHistos()
{

  HistoSvc *hSvcVal = HistoSvc::GetInstance();
  //hSvcVal->makeFileDir("SAC");

  hSvcVal->BookHisto("SAC_NHits", 1000, 0.0, 1000.0);
  hSvcVal->BookHisto("SAC_NClusters", 500, 0.0, 500.0);
  hSvcVal->BookHisto("SAC_Hit_Energy", 500, 0.0, 500.0);
  hSvcVal->BookHisto("SAC_Cluster_Energy", 700, 0.0, 700.0);
  hSvcVal->BookHisto("SAC_Cluster_Time", 800, -400.0, 400.0);
  hSvcVal->BookHisto2("SAC_Cluster_Time_Energy", 800, -400.0, 400.0, 700, 0.0, 700.0);
}

void DarkPhoton::BookSACPVetoBremsHistos()
{

  HistoSvc *hSvcVal = HistoSvc::GetInstance();
  //hSvcVal->makeFileDir("SACPVetoBrems");

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

  hSvcVal->BookHisto("SAC_PVeto_dE_closest", 400, -200.0, 200.0);

}

void DarkPhoton::BookSACHEPVetoBremsHistos()
{

  HistoSvc *hSvcVal = HistoSvc::GetInstance();
  //hSvcVal->makeFileDir("SACHEPVetoBrems");

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

  hSvcVal->BookHisto("SAC_HEPVeto_dE_closest", 400, -200.0, 200.0);
}

void DarkPhoton::BookSACEVetoCheckHistos()
{

  HistoSvc *hSvcVal = HistoSvc::GetInstance();
  //hSvcVal->makeFileDir("SACEVetoCheck");

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
}

void DarkPhoton::BookECalPVetoBremsHistos()
{

  HistoSvc *hSvcVal = HistoSvc::GetInstance();
  //hSvcVal->makeFileDir("ECalPVetoBrems");

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

  hSvcVal->BookHisto("ECal_PVeto_dE_closest", 400, -200.0, 200.0);
  hSvcVal->BookHisto("ECal_PVeto_dE", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ECal_PVeto_dE_inTime", 500, 0.0, 500.0);
  hSvcVal->BookHisto2("ECal_PVeto_TimeDiff_dE", 800, -400.0, 400.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("ECal_PVeto_TimeDiff_dE_inTime", 800, -400.0, 400.0, 500, 0.0, 500.0);

  hSvcVal->BookHisto2("PVetoEnergy_ECalEnergy", 500, 0.0, 500.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("PVetoEnergy_ECalEnergy_inTime", 500, 0.0, 500.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("PVetoEnergy_ECalEnergy_inTime_dE", 500, 0.0, 500.0, 500, 0.0, 500.0);

  hSvcVal->BookHisto2("ECalEnergy_PVetoChID_inTime_dE", 500, 0.0, 500.0, 100, 0.0, 100.0);
  hSvcVal->BookHisto2("ECalEnergy_PVetoChID_inTime_dE_closest", 500, 0.0, 500.0, 100, 0.0, 100.0);
  hSvcVal->BookHisto2("PVetoEnergy_ECalEnergy_inTime_dE_closest", 500, 0.0, 500.0, 500, 0.0, 500.0);

  hSvcVal->BookHisto2("Clusters_Above_Banan_position", 1000, -500.0, 500.0, 1000, -500.0, 500.0);
  hSvcVal->BookHisto2("Clusters_Banan_position", 1000, -500.0, 500.0, 1000, -500.0, 500.0);
  hSvcVal->BookHisto2("Clusters_Below_Banan_position", 1000, -500.0, 500.0, 1000, -500.0, 500.0);

hSvcVal->BookHisto2("ECalEnergy_PVetoChID_inTime_horizontal", 500, 0.0, 500.0, 100, 0.0, 100.0);

hSvcVal->BookHisto2("ECalEnergy_PVetoChID_inTime_horizontal_out", 500, 0.0, 500.0, 100, 0.0, 100.0);


}

void DarkPhoton::BookECalHEPVetoBremsHistos()
{

  HistoSvc *hSvcVal = HistoSvc::GetInstance();
  //hSvcVal->makeFileDir("ECalHEPVetoBrems");

  hSvcVal->BookHisto("ECal_HEPVeto_TimeDiff", 800, -400.0, 400.0);
  hSvcVal->BookHisto("ECal_HEPVeto_TimeDiff_closest", 800, -400.0, 400.0);
  hSvcVal->BookHisto2("ECal_HEPVeto_X_vs_TimeDiff", 16, 789.0, 949.0, 800, -400.0, 400.0);
  hSvcVal->BookHisto2("ECal_HEPVeto_ChID_vs_TimeDiff_detail", 100, 0.0, 100.0, 1000, -20.0, 20.0);

  hSvcVal->BookHisto2("ECalEnergy_HEPVetoX", 500, 0.0, 500.0, 16, 789.0, 949.0);
  hSvcVal->BookHisto2("ECalEnergy_HEPVetoChID", 500, 0.0, 500.0, 100, 0.0, 100.0);
  hSvcVal->BookHisto2("ECalEnergy_HEPVetoX_inTime", 500, 0.0, 500.0, 16, 789.0, 949.0);
  hSvcVal->BookHisto2("ECalEnergy_HEPVetoChID_inTime", 500, 0.0, 500.0, 100, 0.0, 100.0);
  hSvcVal->BookHisto2("ECalEnergy_HEPVetoChID_random", 500, 0.0, 500.0, 100, 0.0, 100.0);

  hSvcVal->BookHisto2("HEPVetoX_ECalEnergy", 16, 789.0, 949.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("HEPVetoX_ECalEnergy_inTime", 16, 789.0, 949.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("HEPVetoChID_ECalEnergy", 100, 0.0, 100.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("HEPVetoChID_ECalEnergy_inTime", 100, 0.0, 100.0, 500, 0.0, 500.0);

  hSvcVal->BookHisto2("ECalEnergy_HEPVetoX_1clu", 500, 0.0, 500.0, 16, 789.0, 949.0);
  hSvcVal->BookHisto2("ECalEnergy_HEPVetoX_inTime_1clu", 500, 0.0, 500.0, 16, 789.0, 949.0);

  hSvcVal->BookHisto2("HEPVetoX_ECalEnergy_inTime_Cut", 16, 789.0, 949.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("ECalEnergy_HEPVetoChID_inTime_Cut", 500, 0.0, 500.0, 100, 0.0, 100.0);

  hSvcVal->BookHisto2("HEPVetoX_ECalEnergy_inTime_afterCut", 16, 789.0, 949.0, 500, 0.0, 500.0);
  hSvcVal->BookHisto2("ECalEnergy_HEPVetoChID_inTime_afterCut", 500, 0.0, 500.0, 100, 0.0, 100.0);

  hSvcVal->BookHisto("ECal_HEPVeto_dE_closest", 400, -200.0, 200.0);


  hSvcVal->BookHisto2("Clusters_Above_Banan_position_HEP", 1000, -500.0, 500.0, 1000, -500.0, 500.0);
  hSvcVal->BookHisto2("Clusters_Banan_position_HEP", 1000, -500.0, 500.0, 1000, -500.0, 500.0);
  hSvcVal->BookHisto2("Clusters_Below_Banan_position_HEP", 1000, -500.0, 500.0, 1000, -500.0, 500.0);

hSvcVal->BookHisto2("ECalEnergy_HEPVetoChID_inTime_horizontal", 500, 0.0, 500.0, 100, 0.0, 100.0);

}

void DarkPhoton::BookECalEVetoCheckHistos()
{
  HistoSvc *hSvcVal = HistoSvc::GetInstance();
  //hSvcVal->makeFileDir("ECalEVetoCheck");


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


}

void DarkPhoton::BookECalSACCheckHistos()
{

  HistoSvc *hSvcVal = HistoSvc::GetInstance();
  //hSvcVal->makeFileDir("ECalSACCheck");

  // hSvcVal->BookHisto("ECal_SAC_EnergySum_1ns", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ECal_SAC_EnergySum_inTime_noECalGG_noEPbrem", 500, 0.0, 500.0);
  hSvcVal->BookHisto("ECal_SAC_EnergySum_inTime_noECalGG_noEPbrem_noSPbrem", 500, 0.0, 500.0);

  hSvcVal->BookHisto("ECal_SAC_DeltaT", 800, -400.0, 400.0);
  hSvcVal->BookHisto("ECal_SAC_DeltaT_inTime", 100, -20.0, 20.0);
  hSvcVal->BookHisto("ECal_SAC_DeltaT_inTime_noECalGG", 100, -20.0, 20.0);
  hSvcVal->BookHisto("ECal_SAC_DeltaT_inTime_noECalGG_noEPbrem", 100, -20.0, 20.0);
  hSvcVal->BookHisto("ECal_SAC_DeltaT_inTime_noECalGG_noEPbrem_noSPbrem", 100, -20.0, 20.0);
}

void DarkPhoton::BookDarkPhotonHistos()
{
  HistoSvc *hSvcVal = HistoSvc::GetInstance();

  std::vector<std::string> suffixes = {
    "All",
    "Ecut30MeV", "Ecut50MeV", "Ecut60MeV",
    "Ecut30MeV_GeomCut", "Ecut50MeV_GeomCut", "Ecut60MeV_GeomCut",
    "Ecut30MeV_GeomCut_1nsIsolation", "Ecut50MeV_GeomCut_1nsIsolation", "Ecut60MeV_GeomCut_1nsIsolation",
    "Ecut30MeV_GeomCut_2nsIsolation", "Ecut50MeV_GeomCut_2nsIsolation", "Ecut60MeV_GeomCut_2nsIsolation",
    "Ecut30MeV_GeomCut_5nsIsolation", "Ecut50MeV_GeomCut_5nsIsolation", "Ecut60MeV_GeomCut_5nsIsolation",
    "Ecut30MeV_GeomCut_2nsIsolation_PVetoCutTime", "Ecut50MeV_GeomCut_2nsIsolation_PVetoCutTime", "Ecut60MeV_GeomCut_2nsIsolation_PVetoCutTime",
    "Ecut30MeV_GeomCut_2nsIsolation_PVetoCutTimeFit", "Ecut50MeV_GeomCut_2nsIsolation_PVetoCutTimeFit", "Ecut60MeV_GeomCut_2nsIsolation_PVetoCutTimeFit",
    "Ecut30MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", "Ecut50MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTime", "Ecut60MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTime",
    "Ecut30MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", "Ecut50MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit", "Ecut60MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit",
   // "Ecut30MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", "Ecut50MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut", "Ecut60MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut",
   // "Ecut30MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", "Ecut50MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut", "Ecut60MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut",
  };

  for (const auto& suffix : suffixes) {
    hSvcVal->BookHisto("ClusterEnergy_" + suffix, 500, 0.0, 500.0);
  }

  for (const auto& suffix : suffixes) {
    hSvcVal->BookHisto("MissingMass_" + suffix, 1000, -500.0, 500.0);
  }

  for (const auto& suffix : suffixes) {
    hSvcVal->BookHisto("MissingMomentum_" + suffix, 1000, -500.0, 500.0);
  }

  for (const auto& suffix : suffixes) {
    hSvcVal->BookHisto("MissingTheta_" + suffix, 1000, 0.0, 0.25);
  }

  for (const auto& suffix : suffixes) {
    hSvcVal->BookHisto2("MissingMassTheta_" + suffix, 1000, -500.0, 500.0, 1000, 0.0, 0.25);
  }

  for (const auto& suffix : suffixes) {
    hSvcVal->BookHisto2("MissingMassRadius_" + suffix, 1000, -500.0, 500.0, 1000, 0.0, 500.0);
  }

  for (const auto& suffix : suffixes) {
    hSvcVal->BookHisto2("MissingMomentumMissingTheta_" + suffix, 1000, -500.0, 500.0, 1000, 0.0, 4.0);
  }

  for (const auto& suffix : suffixes) {
    hSvcVal->BookHisto2("ClusterEnergy_Radius_" + suffix, 500, 0.0, 500.0, 400, 0.0, 400.0);
  }

  // Special histograms
  //hSvcVal->BookHisto("MissingMass_Special", 1000, -500.0, 500.0);
  //hSvcVal->BookHisto("MissingMass_Special_inChID", 1000, -500.0, 500.0);
  //hSvcVal->BookHisto2("MissingMass_Special_inChID_ChIDDiff", 1000, -500.0, 500.0, 200, -100.0, 100.0);
  //hSvcVal->BookHisto("MissingMomentum_Special", 1000, -500.0, 500.0);
  //hSvcVal->BookHisto("MissingMomentum_Special_inChID", 1000, -500.0, 500.0);
  //hSvcVal->BookHisto2("MissingMomentum_Special_inChID_ChIDDiff", 1000, -500.0, 500.0, 200, -100.0, 100.0);
}

void DarkPhoton::BookDarkPhotonDebugHistos()
{
  HistoSvc *hSvcVal = HistoSvc::GetInstance();

  std::vector<std::string> suffixes = {
      "All",
      "Ecut30MeV",
      "Ecut50MeV",
      "Ecut60MeV",
      "Ecut30MeV_GeomCut",
      "Ecut50MeV_GeomCut",
      "Ecut60MeV_GeomCut",
      "Ecut30MeV_GeomCut_1nsIsolation",
      "Ecut50MeV_GeomCut_1nsIsolation",
      "Ecut60MeV_GeomCut_1nsIsolation",
      "Ecut30MeV_GeomCut_2nsIsolation",
      "Ecut50MeV_GeomCut_2nsIsolation",
      "Ecut60MeV_GeomCut_2nsIsolation",
      "Ecut30MeV_GeomCut_5nsIsolation",
      "Ecut50MeV_GeomCut_5nsIsolation",
      "Ecut60MeV_GeomCut_5nsIsolation",
      "Ecut30MeV_GeomCut_2nsIsolation_PVetoCutTime",
      "Ecut50MeV_GeomCut_2nsIsolation_PVetoCutTime",
      "Ecut60MeV_GeomCut_2nsIsolation_PVetoCutTime",
      "Ecut30MeV_GeomCut_2nsIsolation_PVetoCutTimeFit",
      "Ecut50MeV_GeomCut_2nsIsolation_PVetoCutTimeFit",
      "Ecut60MeV_GeomCut_2nsIsolation_PVetoCutTimeFit",
      "Ecut30MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTime",
      "Ecut50MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTime",
      "Ecut60MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTime",
      "Ecut30MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit",
      "Ecut50MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit",
      "Ecut60MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit",
     // "Ecut30MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut",
      //"Ecut50MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut",
      //"Ecut60MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit_EVetoCut",
      //"Ecut30MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut",
      //"Ecut50MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut",
      //"Ecut60MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTime_EVetoCut",
  };

  for (const auto &suffix : suffixes)
  {
    hSvcVal->BookHisto("Debug_ClusterEnergy_" + suffix, 500, 0.0, 500.0);
  }

  for (const auto &suffix : suffixes)
  {
    hSvcVal->BookHisto("Debug_ClusterRadius_" + suffix, 500, 0.0, 500.0);
  }

  for (const auto &suffix : suffixes)
  {
    hSvcVal->BookHisto("Debug_ClusterTime_" + suffix, 1000, -500.0, 500.0);
  }

  for (const auto &suffix : suffixes)
  {
    hSvcVal->BookHisto2("Debug_ClusterPosition_" + suffix, 700, -350.0, 350.0, 700, -350.0, 350.0);
  }

  for (const auto &suffix : suffixes)
  {
    hSvcVal->BookHisto("Debug_PVetoTimeDiff_" + suffix, 1000, -500.0, 500.0);
  }

  for (const auto &suffix : suffixes)
  {
    hSvcVal->BookHisto2("Debug_ECalEnergy_PVetoChID_inTime_" + suffix, 500, 0.0, 500.0, 100, 0.0, 100.0);
  }

  for (const auto &suffix : suffixes)
  {
    hSvcVal->BookHisto("Debug_HEPVetoTimeDiff_" + suffix, 1000, -500.0, 500.0);
  }

  for (const auto &suffix : suffixes)
  {
    hSvcVal->BookHisto2("Debug_ECalEnergy_HEPVetoChID_inTime_" + suffix, 500, 0.0, 500.0, 20, 0.0, 20.0);
  }
}

void DarkPhoton::BookUBosonDecayHistos()
{

  HistoSvc *hSvcVal = HistoSvc::GetInstance();
 // hSvcVal->makeFileDir("UBosonDecay");

  hSvcVal->BookHisto2("ECal_PVeto_EVeto_TimeDiff", 800, -400.0, 400.0, 800, -400.0, 400.0);
  hSvcVal->BookHisto("ECal_PVeto_EVeto_ChIDDiff", 200, -100.0, 100.0);


}

void DarkPhoton::BookGGHistos()
{

  HistoSvc *hSvcVal = HistoSvc::GetInstance();
  //hSvcVal->makeFileDir("GG");

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


}


double DarkPhoton::ComputeRadius(Double_t PosX1, Double_t PosY1)
{
  Double_t R = sqrt(PosX1 * PosX1 + PosY1 * PosY1);
  return R;
}

double DarkPhoton::ComputeTheta(Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma)
{
  Double_t R = sqrt(PosX * PosX + PosY * PosY + PosZ * PosZ);
  double Px = E_gamma * PosX / R;
  double Py = E_gamma * PosY / R;
  double Pz = E_gamma * PosZ / R;
  TLorentzVector Photon(Px, Py, Pz, E_gamma);
  return Photon.Theta();
}

double DarkPhoton::ComputeInvariantMass(Double_t E1, Double_t E2, Double_t PosX1, Double_t PosX2, Double_t PosY1, Double_t PosY2)
{
  Double_t Z = 3536;
  Double_t M = 0;
  Double_t R = sqrt((PosX1 - PosX2) * (PosX1 - PosX2) + (PosY1 - PosY2) * (PosY1 - PosY2));
  M = (E1 * E2 * R * R) / (Z * Z);
  return M;
}

double DarkPhoton::ComputePhi(Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma)
{
  Double_t R = sqrt(PosX * PosX + PosY * PosY + PosZ * PosZ);
  double Px = E_gamma * PosX / R;
  double Py = E_gamma * PosY / R;
  double Pz = E_gamma * PosZ / R;
  TLorentzVector Photon(Px, Py, Pz, E_gamma);
  return Photon.Phi();
}

double DarkPhoton::ComputeMissingMass(double BeamEnergy, Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma)
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

double DarkPhoton::ComputeMissingMomentum(double BeamEnergy, Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma)
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

TVector3 DarkPhoton::ComputeMissingMomentumVector(double BeamEnergy, Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma)
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

double DarkPhoton::ComputeCOG(Double_t E_gamma1, Double_t Pos1, Double_t E_gamma2, Double_t Pos2)
{
  Double_t COG = 0;
  COG = (E_gamma1 * Pos1 + E_gamma2 * Pos2) / (E_gamma1 + E_gamma2);
  return COG;
}

TLorentzVector DarkPhoton::ComputeCMMomentum(Double_t xt, Double_t yt, Double_t zt, Double_t PosX, Double_t PosY, Double_t PosZ)
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

int DarkPhoton::ComputeSector(Double_t x, Double_t y)
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

double DarkPhoton::CalibrateEnergy(Double_t E_gamma, Double_t x, Double_t y)
{
  //double coefficients[19] = {1.00651, 1.02013, 1.0266, 1.01829, 1.01393, 1.02115, 1.02356, 1.00694, 1.01086, 1.00473, 1.00284, 1.0056, 1.00919, 0.998449, 0.9914, 1.00386, 0.991517, 0.985795, 0.990386};
  int c = ComputeSector(x, y);
  return calibrationCoefficients[0] * calibrationCoefficients[1 + c] * E_gamma;
}

double DarkPhoton::ComputeExpectedEnergy(Double_t PosX, Double_t PosY)
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
  targetPos.SetXYZ(-14.3, 0.35, -1028);
  // targetPos.SetXYZ(xt, yt, zt);
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

Double_t DarkPhoton::ComputeMomentumPositron(Double_t z,Double_t x){
  //return ((z+631.5)*(z+631.5)+x*x)/2/x*0.3*0.3986;
  return ((z+608.3)*(z+608.3)+x*x)/2/x*0.3*0.389;
}

void DarkPhoton::ProcessTarget()
{

  if (!evt->TargetRecoEvent)
    return;

  HistoSvc *hSvc = HistoSvc::GetInstance();
  Int_t NPots = evt->TargetRecoBeam->getnPOT();
  Float_t Xt = evt->TargetRecoBeam->getX();
  Float_t Yt = evt->TargetRecoBeam->getY();

  hSvc->FillHisto("Target_NPots", NPots);
  hSvc->FillHisto("Target_X", Xt);
  hSvc->FillHisto("Target_Y", Yt);
  hSvc->FillHisto2("Target_XY", Xt, Yt);
}

void DarkPhoton::ProcessPVeto()
{

  if (!evt->PVetoRecoEvent)
    return;

  HistoSvc *hSvc = HistoSvc::GetInstance();
  Int_t NclusPVeto = evt->PVetoRecoCl->GetNElements();
  Int_t NhitsPVeto = evt->PVetoRecoEvent->GetNHits();

  hSvc->FillHisto("PVeto_NHits", NhitsPVeto);
  hSvc->FillHisto("PVeto_NClusters", NclusPVeto);

  for (Int_t i = 0; i < NhitsPVeto; i++)
  {
    TRecoVHit *hitPVeto = evt->PVetoRecoEvent->Hit(i);
    hSvc->FillHisto("PVeto_Hit_Energy", hitPVeto->GetEnergy());
    hSvc->FillHisto("PVeto_Hit_ChID", hitPVeto->GetChannelId());
  }

  for (Int_t i = 0; i < NclusPVeto; i++)
  {

    TRecoVCluster *cluPVeto = evt->PVetoRecoCl->Element(i);
    Float_t cluPVetoTime = cluPVeto->GetTime();
    Float_t cluPVetoX = cluPVeto->GetPosition().X();
    Float_t cluPVetoZ = cluPVeto->GetPosition().Z();
    Float_t cluPVetoChID = cluPVeto->GetChannelId();
    Float_t cluPVetoEnergy = ComputeMomentumPositron(cluPVetoZ,cluPVetoX);

    hSvc->FillHisto("PVeto_Cluster_Energy", cluPVetoEnergy);
    hSvc->FillHisto("PVeto_Cluster_Time", cluPVetoTime);
    hSvc->FillHisto2("PVeto_Cluster_Time_Energy", cluPVetoTime, cluPVetoEnergy);
    hSvc->FillHisto("PVeto_Cluster_Z", cluPVetoZ);
    hSvc->FillHisto("PVeto_Cluster_ChID", cluPVetoChID);
    hSvc->FillHisto2("PVeto_Cluster_Time_Z", cluPVetoTime, cluPVetoZ);
    hSvc->FillHisto2("PVeto_Cluster_Time_ChID", cluPVetoTime, cluPVetoChID);
    hSvc->FillHisto2("PVeto_Cluster_Energy_Z", cluPVetoEnergy, cluPVetoZ);
    hSvc->FillHisto2("PVeto_Cluster_Energy_ChID", cluPVetoEnergy, cluPVetoChID);
  }

  if (fPVetoEVetoCheck)
    PVetoEVetoCheck();
}

void DarkPhoton::PVetoEVetoCheck()
{
  if (!(evt->PVetoRecoEvent && evt->EVetoRecoEvent))
    return;

  HistoSvc *hSvc = HistoSvc::GetInstance();
  Int_t NclusPVeto = evt->PVetoRecoCl->GetNElements();
  Int_t NclusEVeto = evt->EVetoRecoCl->GetNElements();

  for (Int_t i = 0; i < NclusPVeto; i++)
  {
    TRecoVCluster *cluPVeto = evt->PVetoRecoCl->Element(i);
    Float_t closestPVetoEVeto = 9999.;
    for (Int_t j = 0; j < NclusEVeto; j++)
    {
      TRecoVCluster *cluEVeto = evt->EVetoRecoCl->Element(j);
      Float_t deltaTPVetoEVeto = cluPVeto->GetTime() - cluEVeto->GetTime();
      hSvc->FillHisto("PVeto_EVeto_DeltaT", deltaTPVetoEVeto);
      if (abs(deltaTPVetoEVeto) < abs(closestPVetoEVeto))
        closestPVetoEVeto = deltaTPVetoEVeto;
    }
    hSvc->FillHisto("PVeto_EVeto_DeltaT_closest", closestPVetoEVeto);
  }
}

void DarkPhoton::ProcessEVeto()
{

  if (!evt->EVetoRecoEvent)
    return;

  HistoSvc *hSvc = HistoSvc::GetInstance();
  Int_t NclusEVeto = evt->EVetoRecoCl->GetNElements();
  Int_t NhitsEVeto = evt->EVetoRecoEvent->GetNHits();

  hSvc->FillHisto("EVeto_NHits", NhitsEVeto);
  hSvc->FillHisto("EVeto_NClusters", NclusEVeto);

  for (Int_t i = 0; i < NhitsEVeto; i++)
  {
    TRecoVHit *hitEVeto = evt->EVetoRecoEvent->Hit(i);
    hSvc->FillHisto("EVeto_Hit_Energy", hitEVeto->GetEnergy());
  }

  for (Int_t i = 0; i < NclusEVeto; i++)
  {

    TRecoVCluster *cluEVeto = evt->EVetoRecoCl->Element(i);
    Float_t cluEVetoEnergy = cluEVeto->GetEnergy();
    Float_t cluEVetoTime = cluEVeto->GetTime();
    Float_t cluEVetoZ = cluEVeto->GetPosition().Z();
    Float_t cluEVetoChID = cluEVeto->GetChannelId();

    hSvc->FillHisto("EVeto_Cluster_Energy", cluEVetoEnergy);
    hSvc->FillHisto("EVeto_Cluster_Time", cluEVetoTime);
    hSvc->FillHisto2("EVeto_Cluster_Time_Energy", cluEVetoTime, cluEVetoEnergy);
    hSvc->FillHisto("EVeto_Cluster_Z", cluEVetoZ);
    hSvc->FillHisto("EVeto_Cluster_ChID", cluEVetoChID);
    hSvc->FillHisto2("EVeto_Cluster_Time_Z", cluEVetoTime, cluEVetoZ);
    hSvc->FillHisto2("EVeto_Cluster_Time_ChID", cluEVetoTime, cluEVetoChID);
    hSvc->FillHisto2("EVeto_Cluster_Energy_Z", cluEVetoEnergy, cluEVetoZ);
    hSvc->FillHisto2("EVeto_Cluster_Energy_ChID", cluEVetoEnergy, cluEVetoChID);
  }
}

void DarkPhoton::ProcessHEPVeto()
{

  if (!evt->HEPVetoRecoEvent)
    return;

  HistoSvc *hSvc = HistoSvc::GetInstance();
  Int_t NclusHEPVeto = evt->HEPVetoRecoCl->GetNElements();
  Int_t NhitsHEPVeto = evt->HEPVetoRecoEvent->GetNHits();

  hSvc->FillHisto("HEPVeto_NHits", NhitsHEPVeto);
  hSvc->FillHisto("HEPVeto_NClusters", NclusHEPVeto);

  for (Int_t i = 0; i < NhitsHEPVeto; i++)
  {
    TRecoVHit *hitHEPVeto = evt->HEPVetoRecoEvent->Hit(i);
    hSvc->FillHisto("HEPVeto_Hit_Energy", hitHEPVeto->GetEnergy());
  }

  for (Int_t i = 0; i < NclusHEPVeto; i++)
  {

    TRecoVCluster *cluHEPVeto = evt->HEPVetoRecoCl->Element(i);
    Float_t cluHEPVetoTime = cluHEPVeto->GetTime();
    Float_t cluHEPVetoX = cluHEPVeto->GetPosition().X();
    Float_t cluHEPVetoZ = cluHEPVeto->GetPosition().Z();
    Float_t cluHEPVetoChID = cluHEPVeto->GetChannelId();
    Float_t cluHEPVetoEnergy = ComputeMomentumPositron(cluHEPVetoZ,cluHEPVetoX);

    hSvc->FillHisto("HEPVeto_Cluster_Energy", cluHEPVetoEnergy);
    hSvc->FillHisto("HEPVeto_Cluster_Time", cluHEPVetoTime);
    hSvc->FillHisto2("HEPVeto_Cluster_Time_Energy", cluHEPVetoTime, cluHEPVetoEnergy);
    hSvc->FillHisto("HEPVeto_Cluster_X", cluHEPVetoX);
    hSvc->FillHisto("HEPVeto_Cluster_ChID", cluHEPVetoChID);
    hSvc->FillHisto2("HEPVeto_Cluster_Time_X", cluHEPVetoTime, cluHEPVetoX);
    hSvc->FillHisto2("HEPVeto_Cluster_Time_ChID", cluHEPVetoTime, cluHEPVetoChID);
    hSvc->FillHisto2("HEPVeto_Cluster_Energy_X", cluHEPVetoEnergy, cluHEPVetoX);
    hSvc->FillHisto2("HEPVeto_Cluster_Energy_ChID", cluHEPVetoEnergy, cluHEPVetoChID);
  }
}

void DarkPhoton::ProcessSAC()
{
  if (!evt->SACRecoEvent)
    return;

  HistoSvc *hSvc = HistoSvc::GetInstance();
  Int_t NclusSAC = evt->SACRecoCl->GetNElements();
  Int_t NhitsSAC = evt->SACRecoEvent->GetNHits();

  hSvc->FillHisto("SAC_NHits", NhitsSAC);
  hSvc->FillHisto("SAC_NClusters", NclusSAC);

  for (Int_t i = 0; i < NhitsSAC; i++)
  {
    TRecoVHit *hitSAC = evt->SACRecoEvent->Hit(i);
    hSvc->FillHisto("SAC_Hit_Energy", hitSAC->GetEnergy());
  }

  for (Int_t i = 0; i < NclusSAC; i++)
  {

    TRecoVCluster *cluSAC = evt->SACRecoCl->Element(i);
    Float_t cluSACEnergy = cluSAC->GetEnergy();
    Float_t cluSACTime = cluSAC->GetTime();

    hSvc->FillHisto("SAC_Cluster_Energy", cluSACEnergy);
    hSvc->FillHisto("SAC_Cluster_Time", cluSACTime);
    hSvc->FillHisto2("SAC_Cluster_Time_Energy", cluSACTime, cluSACEnergy);
  }
}

void DarkPhoton::SACPVetoBrems(){

  if (!(evt->SACRecoEvent && evt->PVetoRecoEvent))
    return;

  HistoSvc *hSvc = HistoSvc::GetInstance();
  Int_t NclusPVeto = evt->PVetoRecoCl->GetNElements();
  Int_t NclusSAC = evt->SACRecoCl->GetNElements();

  for (Int_t i = 0; i < NclusSAC; i++)
  {

    TRecoVCluster *cluSAC = evt->SACRecoCl->Element(i);
    Float_t closestSACPVeto = 9999.;
    Float_t cluSACEnergy = cluSAC->GetEnergy();

    TRecoVCluster *closestCluPVeto = nullptr;

    for (Int_t j = 0; j < NclusPVeto; ++j)
    {

      TRecoVCluster *cluPVeto = evt->PVetoRecoCl->Element(j);

      Float_t deltaTPVetoSAC = cluPVeto->GetTime() - cluSAC->GetTime();
      Float_t cluPVetoX = cluPVeto->GetPosition().X();
      Float_t cluPVetoZ = cluPVeto->GetPosition().Z();
      Float_t cluPVetoChID = cluPVeto->GetChannelId();
      Float_t cluPVetoEnergy = ComputeMomentumPositron(cluPVetoZ,cluPVetoX);

      if (abs(deltaTPVetoSAC) < abs(closestSACPVeto)){
        closestSACPVeto = deltaTPVetoSAC;
        closestCluPVeto = cluPVeto;
      }

      hSvc->FillHisto("SAC_PVeto_TimeDiff", deltaTPVetoSAC);
      hSvc->FillHisto2("SAC_PVeto_Z_vs_TimeDiff", cluPVetoZ, deltaTPVetoSAC);
      if (abs(deltaTPVetoSAC) < 20.0)
        hSvc->FillHisto2("SAC_PVeto_ChID_vs_TimeDiff_detail", cluPVetoChID, deltaTPVetoSAC);
      hSvc->FillHisto2("SACEnergy_PVetoZ", cluSACEnergy, cluPVetoZ);
      hSvc->FillHisto2("SACEnergy_PVetoChID", cluSACEnergy, cluPVetoChID);
      hSvc->FillHisto2("PVetoZ_SACEnergy", cluPVetoZ, cluSACEnergy);
      hSvc->FillHisto2("PVetoChID_SACEnergy", cluPVetoChID, cluSACEnergy);

      if (fabs(deltaTPVetoSAC) < SACPVetoTimeWindow)
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

    hSvc->FillHisto("SAC_PVeto_TimeDiff_closest", closestSACPVeto);
    Float_t dE = ComputeMomentumPositron(closestCluPVeto->GetPosition().Z(),closestCluPVeto->GetPosition().X()) + cluSACEnergy - BeamEnergy - shiftMomentumsacPVeto;
    hSvc->FillHisto("SAC_PVeto_dE_closest", dE);
  }
}

void DarkPhoton::SACHEPVetoBrems(){
  if (!(evt->SACRecoEvent && evt->HEPVetoRecoEvent))
    return;

  HistoSvc *hSvc = HistoSvc::GetInstance();
  Int_t NclusHEPVeto = evt->HEPVetoRecoCl->GetNElements();
  Int_t NclusSAC = evt->SACRecoCl->GetNElements();

  for (Int_t i = 0; i < NclusSAC; i++)
  {

    TRecoVCluster *cluSAC = evt->SACRecoCl->Element(i);
    Float_t cluSACEnergy = cluSAC->GetEnergy();
    Float_t closestSACHEPVeto = 9999.;

    TRecoVCluster *closestCluHEPVeto = nullptr;

    for (Int_t j = 0; j < NclusHEPVeto; ++j)
    {

      TRecoVCluster *cluHEPVeto = evt->HEPVetoRecoCl->Element(j);

      Float_t deltaTHEPVetoSAC = cluHEPVeto->GetTime() - cluSAC->GetTime();
      Float_t cluHEPVetoX = cluHEPVeto->GetPosition().X();
      Float_t cluHEPVetoZ = cluHEPVeto->GetPosition().Z();
      Float_t cluHEPVetoChID = cluHEPVeto->GetChannelId();
      Float_t cluHEPVetoEnergy = ComputeMomentumPositron(cluHEPVetoZ,cluHEPVetoX);

      if (abs(deltaTHEPVetoSAC) < abs(closestSACHEPVeto)){
        closestSACHEPVeto = deltaTHEPVetoSAC;
        closestCluHEPVeto = cluHEPVeto;
      }

      hSvc->FillHisto("SAC_HEPVeto_TimeDiff", deltaTHEPVetoSAC);
      hSvc->FillHisto2("SAC_HEPVeto_X_vs_TimeDiff", cluHEPVetoX, deltaTHEPVetoSAC);
      if (abs(deltaTHEPVetoSAC) < 20.0)
        hSvc->FillHisto2("SAC_HEPVeto_ChID_vs_TimeDiff_detail", cluHEPVetoChID, deltaTHEPVetoSAC);
      hSvc->FillHisto2("SACEnergy_HEPVetoX", cluSACEnergy, cluHEPVetoX);
      hSvc->FillHisto2("SACEnergy_HEPVetoChID", cluSACEnergy, cluHEPVetoChID);
      hSvc->FillHisto2("HEPVetoX_SACEnergy", cluHEPVetoX, cluSACEnergy);
      hSvc->FillHisto2("HEPVetoChID_SACEnergy", cluHEPVetoChID, cluSACEnergy);

      if (fabs(deltaTHEPVetoSAC) < SACHEPVetoTimeWindow)
      {
        hSvc->FillHisto2("SACEnergy_HEPVetoX_inTime", cluSACEnergy, cluHEPVetoX);
        hSvc->FillHisto2("SACEnergy_HEPVetoChID_inTime", cluSACEnergy, cluHEPVetoChID);
        hSvc->FillHisto2("HEPVetoX_SACEnergy_inTime", cluHEPVetoX, cluSACEnergy);
        hSvc->FillHisto2("HEPVetoChID_SACEnergy_inTime", cluHEPVetoChID, cluSACEnergy);

      }
    }

    hSvc->FillHisto("SAC_HEPVeto_TimeDiff_closest", closestSACHEPVeto);
    Float_t dE = ComputeMomentumPositron(closestCluHEPVeto->GetPosition().Z(),closestCluHEPVeto->GetPosition().X()) + cluSACEnergy - BeamEnergy;
    hSvc->FillHisto("SAC_HEPVeto_dE_closest", dE);
  }
}

void DarkPhoton::SACEVetoCheck(){

  if (!(evt->SACRecoEvent && evt->EVetoRecoEvent))
    return;

  HistoSvc *hSvc = HistoSvc::GetInstance();
  Int_t NclusEVeto = evt->EVetoRecoCl->GetNElements();
  Int_t NclusSAC = evt->SACRecoCl->GetNElements();

  for (Int_t i = 0; i < NclusSAC; i++)
  {

    TRecoVCluster *cluSAC = evt->SACRecoCl->Element(i);
    Float_t cluSACEnergy = cluSAC->GetEnergy();
    Float_t closestSACEVeto = 9999.;

    for (Int_t j = 0; j < NclusEVeto; ++j)
    {

      TRecoVCluster *cluEVeto = evt->EVetoRecoCl->Element(j);

      Float_t deltaTEVetoSAC = cluEVeto->GetTime() - cluSAC->GetTime();
      Float_t cluEVetoEnergy = cluEVeto->GetEnergy();
      Float_t cluEVetoZ = cluEVeto->GetPosition().Z();
      Float_t cluEVetoChID = cluEVeto->GetChannelId();

      if (abs(deltaTEVetoSAC) < abs(closestSACEVeto))
        closestSACEVeto = deltaTEVetoSAC;

      hSvc->FillHisto("SAC_EVeto_TimeDiff", deltaTEVetoSAC);
      hSvc->FillHisto2("SAC_EVeto_Z_vs_TimeDiff", cluEVetoZ, deltaTEVetoSAC);
      if (abs(deltaTEVetoSAC) < 20.0)
        hSvc->FillHisto2("SAC_EVeto_ChID_vs_TimeDiff_detail", cluEVetoChID, deltaTEVetoSAC);
      hSvc->FillHisto2("SACEnergy_EVetoZ", cluSACEnergy, cluEVetoZ);
      hSvc->FillHisto2("SACEnergy_EVetoChID", cluSACEnergy, cluEVetoChID);
      hSvc->FillHisto2("EVetoZ_SACEnergy", cluEVetoZ, cluSACEnergy);
      hSvc->FillHisto2("EVetoChID_SACEnergy", cluEVetoChID, cluSACEnergy);

      if (fabs(deltaTEVetoSAC) < SACEVetoTimeWindow)
      {
        hSvc->FillHisto2("SACEnergy_EVetoZ_inTime", cluSACEnergy, cluEVetoZ);
        hSvc->FillHisto2("SACEnergy_EVetoChID_inTime", cluSACEnergy, cluEVetoChID);
        hSvc->FillHisto2("EVetoZ_SACEnergy_inTime", cluEVetoZ, cluSACEnergy);
        hSvc->FillHisto2("EVetoChID_SACEnergy_inTime", cluEVetoChID, cluSACEnergy);
      }
    }

    hSvc->FillHisto("SAC_EVeto_TimeDiff_closest", closestSACEVeto);
  }
}

void DarkPhoton::ProcessECal(){

  if (!(evt->ECalRecoEvent && evt->PVetoRecoEvent && evt->TargetRecoEvent && evt->EVetoRecoEvent && evt->HEPVetoRecoEvent && evt->SACRecoEvent))
    return;

  HistoSvc *hSvc = HistoSvc::GetInstance();
  Int_t NclusECal = evt->ECalRecoCl->GetNElements();
  Int_t NhitsECal = evt->ECalRecoEvent->GetNHits();

  hSvc->FillHisto("ECal_NHits", NhitsECal);
  hSvc->FillHisto("ECal_NClusters", NclusECal);

  for (Int_t i = 0; i < NhitsECal; i++)
  {
    TRecoVHit *hitECal = evt->ECalRecoEvent->Hit(i);
    hSvc->FillHisto("ECal_Hit_Energy", hitECal->GetEnergy());
  }

  ecalAllClusters.clear();
  ecalAllClusters.reserve(NclusECal);

  for (Int_t i = 0; i < NclusECal; i++)
  {
    TRecoVCluster *cluECal = evt->ECalRecoCl->Element(i);

    ECalClusterInfo cluInfo;
    cluInfo.x = cluECal->GetPosition().X();
    cluInfo.y = cluECal->GetPosition().Y();
    cluInfo.z = 2508.31;
    cluInfo.chX = (cluECal->GetChannelId()) / 10;
    cluInfo.chY = (cluECal->GetChannelId()) % 10;
    cluInfo.time = cluECal->GetTime();
    cluInfo.energy = cluECal->GetEnergy();
    //cluInfo.energy = CalibrateEnergy(cluECal->GetEnergy(), cluInfo.x, cluInfo.y);

    cluInfo.radius = ComputeRadius(cluInfo.x, cluInfo.y);
    cluInfo.theta = ComputeTheta(cluInfo.x, cluInfo.y, cluInfo.z, cluInfo.energy);
    cluInfo.phi = ComputePhi(cluInfo.x, cluInfo.y, cluInfo.z, cluInfo.energy);
    cluInfo.missingMass = ComputeMissingMass(BeamEnergy, cluInfo.x, cluInfo.y, Z_1_U, cluInfo.energy);
    cluInfo.missingMomentum = sqrt(ComputeMissingMomentum(BeamEnergy, cluInfo.x, cluInfo.y, Z_1_U, cluInfo.energy));
    cluInfo.missingMomentumV = ComputeMissingMomentumVector(BeamEnergy, cluInfo.x, cluInfo.y, Z_1_U, cluInfo.energy);
    cluInfo.missingTheta = cluInfo.missingMomentumV.Theta();

    ecalAllClusters.push_back(cluInfo);

    FillECalBasicHistos(cluInfo);
  }
}

void DarkPhoton::ECalPVetoBrems()
{
  if (!(evt->PVetoRecoEvent && evt->ECalRecoEvent))
    return;

  HistoSvc *hSvc = HistoSvc::GetInstance();
  Int_t NclusECal = evt->ECalRecoCl->GetNElements();
  Int_t NclusPVeto = evt->PVetoRecoCl->GetNElements();

  for (Int_t i = 0; i < NclusECal; i++)
  {

    TRecoVCluster *cluECal = evt->ECalRecoCl->Element(i);
    Float_t closestECalPVeto = 9999.;
    TRecoVCluster *closestCluPVeto = nullptr;
    Float_t closestX = 9999.;
    Float_t closestZ = 9999.;

    Float_t shiftedTime = cluECal->GetTime() + EcalShiftTime;
    if(shiftedTime> maxTimebunch){     
      Double_t covered = maxTimebunch-cluECal->GetTime();
      Double_t missingT = EcalShiftTime-covered;
      shiftedTime = minTimeBunch + missingT;
    }

    if(cluECal->GetEnergy() < 30.) continue;

    for (Int_t j = 0; j < NclusPVeto; ++j)
    {

      TRecoVCluster *cluPVeto = evt->PVetoRecoCl->Element(j);

      //Float_t deltaTPVetoECal = cluPVeto->GetTime() - cluECal->GetTime();
      //Float_t deltaTPVetoECal = cluPVeto->GetTime() - shiftedTime;
      //Float_t deltaTPVetoECal = cluECal->GetTime() - cluPVeto->GetTime() - ShiftECalPVeto;
      Float_t deltaTPVetoECal = cluECal->GetTime() - (cluPVeto->GetTime());//-PVetoTimeCalib[cluPVeto->GetChannelId()]) - ShiftECalPVeto;
      Float_t cluPVetoX = cluPVeto->GetPosition().X();
      Float_t cluPVetoZ = cluPVeto->GetPosition().Z();
      Float_t cluPVetoChID = cluPVeto->GetChannelId();
      Float_t cluPVetoEnergy = ComputeMomentumPositron(cluPVetoZ,cluPVetoX);

      Float_t dE = cluPVetoEnergy + cluECal->GetEnergy();// - BeamEnergy;

      if (abs(deltaTPVetoECal) < abs(closestECalPVeto)){
        closestECalPVeto = deltaTPVetoECal;
        closestCluPVeto = cluPVeto;
        closestX = cluPVetoX;
        closestZ = cluPVetoZ;
      }

      hSvc->FillHisto("ECal_PVeto_TimeDiff", deltaTPVetoECal);
      hSvc->FillHisto("ECal_PVeto_dE", dE);
      hSvc->FillHisto2("ECal_PVeto_TimeDiff_dE", deltaTPVetoECal, dE);

      hSvc->FillHisto2("ECal_PVeto_Z_vs_TimeDiff", cluPVetoZ, deltaTPVetoECal);
      if (abs(deltaTPVetoECal) < 20.0)
        hSvc->FillHisto2("ECal_PVeto_ChID_vs_TimeDiff_detail", cluPVetoChID, deltaTPVetoECal);
      hSvc->FillHisto2("ECalEnergy_PVetoZ", cluECal->GetEnergy(), cluPVetoZ);
      hSvc->FillHisto2("ECalEnergy_PVetoChID", cluECal->GetEnergy(), cluPVetoChID);
      hSvc->FillHisto2("PVetoZ_ECalEnergy", cluPVetoZ, cluECal->GetEnergy());
      hSvc->FillHisto2("PVetoChID_ECalEnergy", cluPVetoChID, cluECal->GetEnergy());
      hSvc->FillHisto2("PVetoEnergy_ECalEnergy", cluPVetoEnergy, cluECal->GetEnergy());

      if (deltaTPVetoECal > -20. && deltaTPVetoECal < -18.)
        hSvc->FillHisto2("ECalEnergy_PVetoChID_random", cluECal->GetEnergy(), cluPVetoChID);

      if (fabs(deltaTPVetoECal) < ECalPVetoInTimeWindow)
      {
        hSvc->FillHisto("ECal_PVeto_dE_inTime", dE);
        hSvc->FillHisto2("ECal_PVeto_TimeDiff_dE_inTime", deltaTPVetoECal, dE);
        hSvc->FillHisto2("ECalEnergy_PVetoZ_inTime", cluECal->GetEnergy(), cluPVetoZ);
        hSvc->FillHisto2("ECalEnergy_PVetoChID_inTime", cluECal->GetEnergy(), cluPVetoChID);
        hSvc->FillHisto2("PVetoZ_ECalEnergy_inTime", cluPVetoZ, cluECal->GetEnergy());
        hSvc->FillHisto2("PVetoChID_ECalEnergy_inTime", cluPVetoChID, cluECal->GetEnergy());
        hSvc->FillHisto2("PVetoEnergy_ECalEnergy_inTime", cluPVetoEnergy, cluECal->GetEnergy());
      if (fabs(cluECal->GetPosition().Y()) < 75.)
        {
          hSvc->FillHisto2("ECalEnergy_PVetoChID_inTime_horizontal", cluECal->GetEnergy(), cluPVetoChID);
        
           }

        if (fabs(cluECal->GetPosition().Y()) > 95.)
        {
          hSvc->FillHisto2("ECalEnergy_PVetoChID_inTime_horizontal_out", cluECal->GetEnergy(), cluPVetoChID);
        
           }
        
        if(fabs(dE-BeamEnergy) < 50.0){
          hSvc->FillHisto2("ECalEnergy_PVetoChID_inTime_dE", cluECal->GetEnergy(), cluPVetoChID);
          hSvc->FillHisto2("PVetoEnergy_ECalEnergy_inTime_dE", cluPVetoEnergy, cluECal->GetEnergy());

        }
      }

      if ((fabs(deltaTPVetoECal) <= ECalPVetoInTimeWindow) && (cluPVetoChID) < (alineECalCh * cluECal->GetEnergy() * cluECal->GetEnergy() + blineECalCh * cluECal->GetEnergy() + cgoreECalCh) &&
          (cluPVetoChID) > (alineECalCh * cluECal->GetEnergy() * cluECal->GetEnergy() + blineECalCh * cluECal->GetEnergy() + cdoluECalCh))
      {
        hSvc->FillHisto2("PVetoZ_ECalEnergy_inTime_Cut", cluPVetoZ, cluECal->GetEnergy());
        hSvc->FillHisto2("ECalEnergy_PVetoChID_inTime_Cut", cluECal->GetEnergy(), cluPVetoChID);
        hSvc->FillHisto2("Clusters_Banan_position", cluECal->GetPosition().X(), cluECal->GetPosition().Y());
  
      
      }

      if ((fabs(deltaTPVetoECal) <= ECalPVetoInTimeWindow) && (cluPVetoChID) > (alineECalCh * cluECal->GetEnergy() * cluECal->GetEnergy() + blineECalCh * cluECal->GetEnergy() + cgoreECalCh) )
      {
        hSvc->FillHisto2("Clusters_Above_Banan_position", cluECal->GetPosition().X(), cluECal->GetPosition().Y());
      }

      if ((fabs(deltaTPVetoECal) <= ECalPVetoInTimeWindow) && (cluPVetoChID) < (alineECalCh * cluECal->GetEnergy() * cluECal->GetEnergy() + blineECalCh * cluECal->GetEnergy() + cdoluECalCh) && cluECal->GetEnergy()<100.)
      {
        hSvc->FillHisto2("Clusters_Below_Banan_position", cluECal->GetPosition().X(), cluECal->GetPosition().Y());
      }
    }
    
    hSvc->FillHisto("ECal_PVeto_TimeDiff_closest", closestECalPVeto);
    //Float_t closestX = closestCluPVeto->GetPosition().X();
    //Float_t closestZ = closestCluPVeto->GetPosition().Z();
    Float_t dE_closest = ComputeMomentumPositron(closestZ,closestX) + cluECal->GetEnergy() - BeamEnergy;
    if (closestECalPVeto < ECalPVetoInTimeWindow) { 
      hSvc->FillHisto("ECal_PVeto_dE_closest", dE_closest);
      hSvc->FillHisto2("ECalEnergy_PVetoChID_inTime_dE_closest", cluECal->GetEnergy(),closestCluPVeto->GetChannelId());
      hSvc->FillHisto2("PVetoEnergy_ECalEnergy_inTime_dE_closest",ComputeMomentumPositron(closestZ,closestX) , cluECal->GetEnergy());
    }
  }
}

void DarkPhoton::ECalHEPVetoBrems()
{
  if (!(evt->HEPVetoRecoEvent && evt->ECalRecoEvent))
    return;

  HistoSvc *hSvc = HistoSvc::GetInstance();
  Int_t NclusECal = evt->ECalRecoCl->GetNElements();
  Int_t NclusHEPVeto = evt->HEPVetoRecoCl->GetNElements();
  for (Int_t i = 0; i < NclusECal; i++)
  {

    TRecoVCluster *cluECal = evt->ECalRecoCl->Element(i);
    Float_t closestECalHEPVeto = 9999.;
    TRecoVCluster *closestCluHEPVeto = nullptr;

    for (Int_t j = 0; j < NclusHEPVeto; ++j)
    {

      TRecoVCluster *cluHEPVeto = evt->HEPVetoRecoCl->Element(j);

      Float_t deltaTHEPVetoECal = cluHEPVeto->GetTime() - cluECal->GetTime();
      Float_t cluHEPVetoX = cluHEPVeto->GetPosition().X();
      Float_t cluHEPVetoZ = cluHEPVeto->GetPosition().Z();
      Float_t cluHEPVetoChID = cluHEPVeto->GetChannelId();
      Float_t cluHEPVetoEnergy = ComputeMomentumPositron(cluHEPVetoZ,cluHEPVetoX);

      if (abs(deltaTHEPVetoECal) < abs(closestECalHEPVeto)){
        closestECalHEPVeto = deltaTHEPVetoECal;
        closestCluHEPVeto = cluHEPVeto;
      }

      hSvc->FillHisto("ECal_HEPVeto_TimeDiff", deltaTHEPVetoECal);
      hSvc->FillHisto2("ECal_HEPVeto_X_vs_TimeDiff", cluHEPVetoX, deltaTHEPVetoECal);
      if (abs(deltaTHEPVetoECal) < 20.0)
        hSvc->FillHisto2("ECal_HEPVeto_ChID_vs_TimeDiff_detail", cluHEPVetoChID, deltaTHEPVetoECal);
      hSvc->FillHisto2("ECalEnergy_HEPVetoX", cluECal->GetEnergy(), cluHEPVetoX);
      hSvc->FillHisto2("ECalEnergy_HEPVetoChID", cluECal->GetEnergy(), cluHEPVetoChID);
      hSvc->FillHisto2("HEPVetoX_ECalEnergy", cluHEPVetoX, cluECal->GetEnergy());
      hSvc->FillHisto2("HEPVetoChID_ECalEnergy", cluHEPVetoChID, cluECal->GetEnergy());

      if (fabs(deltaTHEPVetoECal) < ECalHEPVetoInTimeWindow)
      {
        hSvc->FillHisto2("ECalEnergy_HEPVetoX_inTime", cluECal->GetEnergy(), cluHEPVetoX);
        hSvc->FillHisto2("ECalEnergy_HEPVetoChID_inTime", cluECal->GetEnergy(), cluHEPVetoChID);
        hSvc->FillHisto2("HEPVetoX_ECalEnergy_inTime", cluHEPVetoX, cluECal->GetEnergy());
        hSvc->FillHisto2("HEPVetoChID_ECalEnergy_inTime", cluHEPVetoChID, cluECal->GetEnergy());
        if (fabs(cluECal->GetPosition().Y()) < 75.)
        {
          hSvc->FillHisto2("ECalEnergy_HEPVetoChID_inTime_horizontal", cluECal->GetEnergy(), cluHEPVetoChID);
        
        }
      
      }

      if (fabs(deltaTHEPVetoECal) <= ECalHEPVetoInTimeWindow && (cluHEPVetoChID) < (alineECalHEPCh * cluECal->GetEnergy() * cluECal->GetEnergy() + blineECalHEPCh * cluECal->GetEnergy() + cgoreECalHEPCh) &&
          (cluHEPVetoChID) > (alineECalHEPCh * cluECal->GetEnergy() * cluECal->GetEnergy() + blineECalHEPCh * cluECal->GetEnergy() + cdoluECalHEPCh))
      {
        hSvc->FillHisto2("HEPVetoX_ECalEnergy_inTime_Cut", cluHEPVetoX, cluECal->GetEnergy());
        hSvc->FillHisto2("ECalEnergy_HEPVetoChID_inTime_Cut", cluECal->GetEnergy(), cluHEPVetoChID);
        hSvc->FillHisto2("Clusters_Banan_position_HEP", cluECal->GetPosition().X(), cluECal->GetPosition().Y());
     
      }

      if ((fabs(deltaTHEPVetoECal) <= ECalHEPVetoInTimeWindow) && (cluHEPVetoChID) > (alineECalHEPCh * cluECal->GetEnergy() * cluECal->GetEnergy() + blineECalHEPCh * cluECal->GetEnergy() + cgoreECalHEPCh) )
      {
        hSvc->FillHisto2("Clusters_Above_Banan_position_HEP", cluECal->GetPosition().X(), cluECal->GetPosition().Y());
      }

      if ((fabs(deltaTHEPVetoECal) <= ECalHEPVetoInTimeWindow) && (cluHEPVetoChID) < (alineECalHEPCh * cluECal->GetEnergy() * cluECal->GetEnergy() + blineECalHEPCh * cluECal->GetEnergy() + cdoluECalHEPCh) && cluECal->GetEnergy()<100.)
      {
        hSvc->FillHisto2("Clusters_Below_Banan_position_HEP", cluECal->GetPosition().X(), cluECal->GetPosition().Y());
      }
    }
    hSvc->FillHisto("ECal_HEPVeto_TimeDiff_closest", closestECalHEPVeto);
    Float_t dE = ComputeMomentumPositron(closestCluHEPVeto->GetPosition().Z(),closestCluHEPVeto->GetPosition().X()) + cluECal->GetEnergy() - BeamEnergy;
    hSvc->FillHisto("ECal_HEPVeto_dE_closest", dE);
  }
}

void DarkPhoton::ECalEVetoCheck()
{
  if (!(evt->EVetoRecoEvent && evt->ECalRecoEvent))
    return;

  HistoSvc *hSvc = HistoSvc::GetInstance();
  Int_t NclusECal = evt->ECalRecoCl->GetNElements();
  Int_t NclusEVeto = evt->EVetoRecoCl->GetNElements();

  for (Int_t i = 0; i < NclusECal; i++)
  {

    TRecoVCluster *cluECal = evt->ECalRecoCl->Element(i);
    Float_t closestECalEVeto = 9999.;

    for (Int_t j = 0; j < NclusEVeto; ++j)
    {

      TRecoVCluster *cluEVeto = evt->EVetoRecoCl->Element(j);

      Float_t deltaTEVetoECal = cluEVeto->GetTime() - cluECal->GetTime();
      Float_t cluEVetoEnergy = cluEVeto->GetEnergy();
      Float_t cluEVetoZ = cluEVeto->GetPosition().Z();
      Float_t cluEVetoChID = cluEVeto->GetChannelId();

      if (abs(deltaTEVetoECal) < abs(closestECalEVeto))
        closestECalEVeto = deltaTEVetoECal;

      hSvc->FillHisto("ECal_EVeto_TimeDiff", deltaTEVetoECal);
      hSvc->FillHisto2("ECal_EVeto_Z_vs_TimeDiff", cluEVetoZ, deltaTEVetoECal);
      if (abs(deltaTEVetoECal) < 20.0)
        hSvc->FillHisto2("ECal_EVeto_ChID_vs_TimeDiff_detail", cluEVetoChID, deltaTEVetoECal);
      hSvc->FillHisto2("ECalEnergy_EVetoZ", cluECal->GetEnergy(), cluEVetoZ);
      hSvc->FillHisto2("ECalEnergy_EVetoChID", cluECal->GetEnergy(), cluEVetoChID);

      hSvc->FillHisto2("EVetoZ_ECalEnergy", cluEVetoZ, cluECal->GetEnergy());
      hSvc->FillHisto2("EVetoChID_ECalEnergy", cluEVetoChID, cluECal->GetEnergy());

      if (deltaTEVetoECal > -20. && deltaTEVetoECal < -18.)
        hSvc->FillHisto2("ECalEnergy_EVetoChID_random", cluECal->GetEnergy(), cluEVetoChID);

      if (fabs(deltaTEVetoECal) < ECalEVetoInTimeWindow)
      {
        hSvc->FillHisto2("ECalEnergy_EVetoZ_inTime", cluECal->GetEnergy(), cluEVetoZ);
        hSvc->FillHisto2("ECalEnergy_EVetoChID_inTime", cluECal->GetEnergy(), cluEVetoChID);
        hSvc->FillHisto2("EVetoZ_ECalEnergy_inTime", cluEVetoZ, cluECal->GetEnergy());
        hSvc->FillHisto2("EVetoChID_ECalEnergy_inTime", cluEVetoChID, cluECal->GetEnergy());
      }
      hSvc->FillHisto("ECal_EVeto_TimeDiff_closest", closestECalEVeto);
    }
  }
}

void DarkPhoton::ECalSACCheck(){

  if (!(evt->SACRecoEvent && evt->ECalRecoEvent))
    return;

  HistoSvc *hSvc = HistoSvc::GetInstance();
  Int_t NclusECal = evt->ECalRecoCl->GetNElements();
  Int_t NclusSAC = evt->SACRecoCl->GetNElements();

  for (const auto &clu : ecalAllClusters)
  {

    for (Int_t j = 0; j < NclusSAC; ++j)
    {
      TRecoVCluster *cluSAC = evt->SACRecoCl->Element(j);
      Float_t deltaTSACECal = cluSAC->GetTime() - clu.time;

      if (fabs(deltaTSACECal) < ECalSACInTimeWindow)
      {
        hSvc->FillHisto("ECal_SAC_DeltaT_inTime", deltaTSACECal);

        if (!(IsECalIsolated(clu, 2.0)))
          continue;

        hSvc->FillHisto("ECal_SAC_DeltaT_inTime_noECalGG", deltaTSACECal);
        

        if (!(IsPVetoBremsFiltered(clu, PVetoIsolationWindow)))
          continue;


        hSvc->FillHisto("ECal_SAC_DeltaT_inTime_noECalGG_noEPbrem", deltaTSACECal);
        hSvc->FillHisto("ECal_SAC_EnergySum_inTime_noECalGG_noEPbrem", clu.energy + cluSAC->GetEnergy());

        if (!(IsSACPVetoBremsFiltered(cluSAC, SACPVetoIsolationWindow)))
          continue;

        hSvc->FillHisto("ECal_SAC_DeltaT_inTime_noECalGG_noEPbrem_noSPbrem", deltaTSACECal);
        if (fabs(deltaTSACECal) < 1.0) hSvc->FillHisto("ECal_SAC_EnergySum_inTime_noECalGG_noEPbrem_noSPbrem", clu.energy + cluSAC->GetEnergy());
      }
    }
  }
    
}

void DarkPhoton::FillECalBasicHistos(const ECalClusterInfo &clu)
{
  HistoSvc *hSvc = HistoSvc::GetInstance();
  Int_t NclusECal = evt->ECalRecoCl->GetNElements();
  Int_t NclusPVeto = evt->PVetoRecoCl->GetNElements();
  Int_t NclusEVeto = evt->EVetoRecoCl->GetNElements();
  Int_t NclusHEPVeto = evt->HEPVetoRecoCl->GetNElements();
  Int_t NclusSAC = evt->SACRecoCl->GetNElements();

  hSvc->FillHisto("ECal_Cluster_Energy", clu.energy);
  hSvc->FillHisto("ECal_Cluster_Time", clu.time);
  hSvc->FillHisto2("ECal_Cluster_Time_Energy", clu.time, clu.energy);
  hSvc->FillHisto("ECal_Cluster_Radius", clu.radius);
  hSvc->FillHisto("ECal_Cluster_Theta", clu.theta);
  hSvc->FillHisto("ECal_Cluster_Phi", clu.phi);
  hSvc->FillHisto("ECal_Cluster_Z", clu.z);
  hSvc->FillHisto2("ECal_Cluster_Energy_Theta", clu.energy, clu.theta);
  hSvc->FillHisto2("ECal_Cluster_Energy_Phi", clu.energy, clu.phi);
  hSvc->FillHisto2("ECal_Cluster_Energy_Radius", clu.energy, clu.radius);
  hSvc->FillHistoProf("ECal_Cluster_Energy_Radius", clu.energy, clu.radius);

  // if (IsInGeometricCut(clu))
  //{
  hSvc->FillHisto("ECal_Cluster_X", clu.x);
  hSvc->FillHisto("ECal_Cluster_Y", clu.y);
  hSvc->FillHisto2("ECal_Cluster_XY", clu.x, clu.y);
  //}

  for (Int_t i = 0; i < NclusECal; i++)
  {
    TRecoVCluster *cluECal = evt->ECalRecoCl->Element(i);
    Float_t deltaTECalECal = clu.time - cluECal->GetTime();
    hSvc->FillHisto("ECal_2clu_DeltaT", deltaTECalECal);
  }

  for (Int_t i = 0; i < NclusSAC; i++)
  {
    TRecoVCluster *cluSAC = evt->SACRecoCl->Element(i);
    Float_t deltaTECalSAC = clu.time - cluSAC->GetTime();
    hSvc->FillHisto("ECal_SAC_DeltaT", deltaTECalSAC);
  }
}

bool DarkPhoton::IsGG(const ECalClusterInfo &clu)
{

  if (ecalAllClusters.size() < 2)
    return false;

  HistoSvc *hSvc = HistoSvc::GetInstance();

  ecalGGClusters.clear();

  // Find the index of clu in ecalAllClusters
  size_t cluIndex = 0;
  bool found = false;
  for (size_t i = 0; i < ecalAllClusters.size(); i++)
  {
    if (&ecalAllClusters[i] == &clu)
    {
      cluIndex = i;
      found = true;
      break;
    }
  }

  if (!found)
    return false;

  // Start from cluIndex + 1 to the end
  for (size_t i = cluIndex + 1; i < ecalAllClusters.size(); i++)
  {
    const auto &clui = ecalAllClusters[i];

    if (fabs(clu.time - clui.time) < MaxTimeDiffGG)
    {
      if (clu.energy > MinEnergy && clui.energy > MinEnergy && IsInGeometricCut(clu) && IsInGeometricCut(clui))
      {
        if (clu.energy + clui.energy > eSumDatalow && clu.energy + clui.energy < eSumDatahigh)
        {
          if (abs(clu.phi - clui.phi) > deltaPhiDatalow && abs(clu.phi - clui.phi) < deltaPhiDatahigh)
          {

            double CMthetaclu = ComputeCMMomentum(ftargetx, ftargety, ftargetz, clu.x, clu.y, clu.z).Vect().Theta();
            double CMthetaclui = ComputeCMMomentum(ftargetx, ftargety, ftargetz, clui.x, clui.y, clui.z).Vect().Theta();

            if (CMthetaclu + CMthetaclui > sumThetaDatalow && CMthetaclu + CMthetaclui < sumThetaDatahigh)
            {
              ECalGGClusterPair ggClu;

              ggClu.x1 = clu.x;
              ggClu.y1 = clu.y;
              ggClu.z1 = clu.z;
              ggClu.x2 = clui.x;
              ggClu.y2 = clui.y;
              ggClu.z2 = clui.z;

              ggClu.energy1 = clu.energy;
              ggClu.energy2 = clui.energy;

              ggClu.time1 = clu.time;
              ggClu.time2 = clui.time;

              ggClu.COGX = ComputeCOG(clu.energy, clu.x, clui.energy, clui.x);
              ggClu.COGY = ComputeCOG(clu.energy, clu.y, clui.energy, clui.y);

              ggClu.energySum = clu.energy + clui.energy;
              ggClu.CMtheta1 = ComputeTheta(clu.x, clu.y, clu.z, clu.energy);
              ggClu.CMtheta2 = ComputeTheta(clui.x, clui.y, clui.z, clui.energy);
              ggClu.radius1 = ComputeRadius(clu.x, clu.y);
              ggClu.radius2 = ComputeRadius(clui.x, clui.y);
              ggClu.phi1 = ComputePhi(clu.x, clu.y, clu.z, clu.energy);
              ggClu.phi2 = ComputePhi(clui.x, clui.y, clui.z, clui.energy);
              ggClu.invMass = ComputeInvariantMass(clu.energy, clui.energy, clu.x, clui.x, clu.y, clui.y);

              ecalGGClusters.push_back(ggClu);
              return true;
            }
          }
        }
      }
    }
  }
  return false;
}

void DarkPhoton::ProcessGG()
{
  if (ecalGGClusters.size() < 1)
    return;

  HistoSvc *hSvc = HistoSvc::GetInstance();
  int nGG = ecalGGClusters.size();

  for (size_t i = 0; i < ecalGGClusters.size(); i++)
  {
    const auto &clupair = ecalGGClusters[i];

    hSvc->FillHisto("GammaGamma_COGX", clupair.COGX);
    hSvc->FillHisto("GammaGamma_COGY", clupair.COGY);
    hSvc->FillHisto2("GammaGamma_COG", clupair.COGX, clupair.COGY);

    hSvc->FillHisto("GammaGamma_X", clupair.x1);
    hSvc->FillHisto("GammaGamma_X", clupair.x2);
    hSvc->FillHisto("GammaGamma_Y", clupair.y1);
    hSvc->FillHisto("GammaGamma_Y", clupair.y2);
    hSvc->FillHisto2("GammaGamma_XY", clupair.x1, clupair.y1);
    hSvc->FillHisto2("GammaGamma_XY", clupair.x2, clupair.y2);

    hSvc->FillHisto2("GammaGamma_XX", clupair.x1, clupair.x2);
    hSvc->FillHisto2("GammaGamma_YY", clupair.y1, clupair.y2);

    hSvc->FillHisto2("GammaGamma_EE", clupair.energy1, clupair.energy2);
    hSvc->FillHisto("GammaGamma_dE", clupair.energy1 - clupair.energy2);
    // if (abs(clupair.energy1 - clupair.energy2)<148. && abs(clupair.energy1 - clupair.energy2)>115. ){
    hSvc->FillHisto("GammaGamma_dX", clupair.x1 + clupair.x2);
    hSvc->FillHisto("GammaGamma_dY", clupair.y1 + clupair.y2);

    hSvc->FillHisto2("GammaGamma_dXdY", abs(clupair.y1 - clupair.y2), abs(clupair.x1 - clupair.x2));

    hSvc->FillHistoProf("GammaGamma_EnergySum_X", clupair.x1, clupair.energySum);
    hSvc->FillHistoProf("GammaGamma_EnergySum_Y", clupair.y1, clupair.energySum);
    hSvc->FillHistoProf("GammaGamma_EnergySum_Phi", clupair.phi1, clupair.energySum);
    hSvc->FillHistoProf("GammaGamma_EnergySum_E", clupair.energy1, clupair.energySum);

    if (clupair.phi1 >= 0 && clupair.phi1 < (PI) / 3.0)
      hSvc->FillHisto("GammaGamma_EnergySum_0_pi/3.0", clupair.energySum);
    if (clupair.phi1 >= (PI) / 3.0 && clupair.phi1 < 2.0 * (PI) / 3.0)
      hSvc->FillHisto("GammaGamma_EnergySum_pi/3.0_2.0pi/3.0", clupair.energySum);
    if (clupair.phi1 >= 2.0 * (PI) / 3.0 && clupair.phi1 <= (PI))
      hSvc->FillHisto("GammaGamma_EnergySum_2.0pi/3.0_pi", clupair.energySum);
    if (clupair.phi1 < 0 && clupair.phi1 > (-(PI) / 3.0))
      hSvc->FillHisto("GammaGamma_EnergySum_0_-pi/3.0", clupair.energySum);
    if (clupair.phi1 <= (-(PI) / 3.0) && clupair.phi1 > (-2.0 * (PI) / 3.0))
      hSvc->FillHisto("GammaGamma_EnergySum_-pi/3.0_-2.0pi/3.0", clupair.energySum);
    if (clupair.phi1 <= (-2.0 * (PI) / 3.0) && clupair.phi1 > (-(PI)))
      hSvc->FillHisto("GammaGamma_EnergySum_-2.0pi/3.0_-pi", clupair.energySum);

    hSvc->FillHisto("GammaGamma_EnergySumSel", clupair.energySum);
    hSvc->FillHisto("GammaGamma_MinvariantSel", clupair.invMass);

    hSvc->FillHisto("GammaGamma_EnergySumSel-MinvariantSel", clupair.energySum - clupair.invMass);

    hSvc->FillHisto2("GammaGamma_ELab_EReco", ComputeExpectedEnergy(clupair.x1, clupair.y1), clupair.energy1);
    hSvc->FillHisto2("GammaGamma_ELab_EReco", ComputeExpectedEnergy(clupair.x2, clupair.y2), clupair.energy2);

    hSvc->FillHisto2("GammaGamma_ThetaLab_Radius", clupair.CMtheta1, clupair.radius1);
    hSvc->FillHisto2("GammaGamma_ThetaLab_Radius", clupair.CMtheta2, clupair.radius2);
  }
}

void DarkPhoton::DarkPhotonHunt() {
 if (!(evt->ECalRecoEvent && evt->PVetoRecoEvent && evt->TargetRecoEvent && evt->EVetoRecoEvent && evt->HEPVetoRecoEvent && evt->SACRecoEvent))
    return;
    //std::cout<<"Warning: Missing Reco Events in DarkPhotonHunt"<<std::endl;

  HistoSvc *hSvc = HistoSvc::GetInstance();


  for (const auto &clu : ecalAllClusters)
  {
    bool isIsolated1ns = IsECalIsolated(clu, 1.0);
    bool isIsolated2ns = IsECalIsolated(clu, 2.0);
    bool isIsolated5ns = IsECalIsolated(clu, 5.0);

    FillMissingMassHistos(clu, "All");
    FillMissingMassDebugHistos(clu, "All");

    if (clu.energy > 30.) FillMissingMassHistos(clu, "Ecut30MeV");
    if (clu.energy > 30.) FillMissingMassDebugHistos(clu, "Ecut30MeV");
    if (clu.energy > 50.) FillMissingMassHistos(clu, "Ecut50MeV");
    if (clu.energy > 50.) FillMissingMassDebugHistos(clu, "Ecut50MeV");
    if (clu.energy > 60.) FillMissingMassHistos(clu, "Ecut60MeV");
    if (clu.energy > 60.) FillMissingMassDebugHistos(clu, "Ecut60MeV");

    if(!IsInGeometricCut(clu)) continue;
    
    if (clu.energy > 30.) FillMissingMassHistos(clu, "Ecut30MeV_GeomCut");
    if (clu.energy > 30.) FillMissingMassDebugHistos(clu, "Ecut30MeV_GeomCut");
    if (clu.energy > 50.) FillMissingMassHistos(clu, "Ecut50MeV_GeomCut");
    if (clu.energy > 50.) FillMissingMassDebugHistos(clu, "Ecut50MeV_GeomCut");
    if (clu.energy > 60.) FillMissingMassHistos(clu, "Ecut60MeV_GeomCut");
    if (clu.energy > 60.) FillMissingMassDebugHistos(clu, "Ecut60MeV_GeomCut");

    if(isIsolated1ns){
      if (clu.energy > 30.) FillMissingMassHistos(clu, "Ecut30MeV_GeomCut_1nsIsolation");
      if (clu.energy > 30.) FillMissingMassDebugHistos(clu, "Ecut30MeV_GeomCut_1nsIsolation");
      if (clu.energy > 50.) FillMissingMassHistos(clu, "Ecut50MeV_GeomCut_1nsIsolation");
      if (clu.energy > 50.) FillMissingMassDebugHistos(clu, "Ecut50MeV_GeomCut_1nsIsolation");
      if (clu.energy > 60.) FillMissingMassHistos(clu, "Ecut60MeV_GeomCut_1nsIsolation");
      if (clu.energy > 60.) FillMissingMassDebugHistos(clu, "Ecut60MeV_GeomCut_1nsIsolation");
    }

    if(isIsolated2ns){
      if (clu.energy > 30.) FillMissingMassHistos(clu, "Ecut30MeV_GeomCut_2nsIsolation");
      if (clu.energy > 30.) FillMissingMassDebugHistos(clu, "Ecut30MeV_GeomCut_2nsIsolation");
      if (clu.energy > 50.) FillMissingMassHistos(clu, "Ecut50MeV_GeomCut_2nsIsolation");
      if (clu.energy > 50.) FillMissingMassDebugHistos(clu, "Ecut50MeV_GeomCut_2nsIsolation");
      if (clu.energy > 60.) FillMissingMassHistos(clu, "Ecut60MeV_GeomCut_2nsIsolation");
      if (clu.energy > 60.) FillMissingMassDebugHistos(clu, "Ecut60MeV_GeomCut_2nsIsolation");
    }

    if(isIsolated5ns){
      if (clu.energy > 30.) FillMissingMassHistos(clu, "Ecut30MeV_GeomCut_5nsIsolation");
      if (clu.energy > 30.) FillMissingMassDebugHistos(clu, "Ecut30MeV_GeomCut_5nsIsolation");
      if (clu.energy > 50.) FillMissingMassHistos(clu, "Ecut50MeV_GeomCut_5nsIsolation");
      if (clu.energy > 50.) FillMissingMassDebugHistos(clu, "Ecut50MeV_GeomCut_5nsIsolation");
      if (clu.energy > 60.) FillMissingMassHistos(clu, "Ecut60MeV_GeomCut_5nsIsolation");
      if (clu.energy > 60.) FillMissingMassDebugHistos(clu, "Ecut60MeV_GeomCut_5nsIsolation");
    }

    if(!(IsECalIsolated(clu, fECalIsolationWindow))) continue;
    

    if(IsPVetoIsolated(clu, PVetoIsolationWindow)){
      if (clu.energy > 30.) FillMissingMassHistos(clu, "Ecut30MeV_GeomCut_2nsIsolation_PVetoCutTime");
      if (clu.energy > 30.) FillMissingMassDebugHistos(clu, "Ecut30MeV_GeomCut_2nsIsolation_PVetoCutTime");
      if (clu.energy > 50.) FillMissingMassHistos(clu, "Ecut50MeV_GeomCut_2nsIsolation_PVetoCutTime");
      if (clu.energy > 50.) FillMissingMassDebugHistos(clu, "Ecut50MeV_GeomCut_2nsIsolation_PVetoCutTime");
      if (clu.energy > 60.) FillMissingMassHistos(clu, "Ecut60MeV_GeomCut_2nsIsolation_PVetoCutTime");
      if (clu.energy > 60.) FillMissingMassDebugHistos(clu, "Ecut60MeV_GeomCut_2nsIsolation_PVetoCutTime");
    }

    if(IsInGeometricCut(clu) && isIsolated2ns && IsPVetoBremsFiltered(clu, PVetoIsolationWindow)){
      if (clu.energy > 30.) FillMissingMassHistos(clu, "Ecut30MeV_GeomCut_2nsIsolation_PVetoCutTimeFit");
      if (clu.energy > 30.) FillMissingMassDebugHistos(clu, "Ecut30MeV_GeomCut_2nsIsolation_PVetoCutTimeFit");
      if (clu.energy > 50.) FillMissingMassHistos(clu, "Ecut50MeV_GeomCut_2nsIsolation_PVetoCutTimeFit");
      if (clu.energy > 50.) FillMissingMassDebugHistos(clu, "Ecut50MeV_GeomCut_2nsIsolation_PVetoCutTimeFit");
      if (clu.energy > 60.) FillMissingMassHistos(clu, "Ecut60MeV_GeomCut_2nsIsolation_PVetoCutTimeFit");
      if (clu.energy > 60.) FillMissingMassDebugHistos(clu, "Ecut60MeV_GeomCut_2nsIsolation_PVetoCutTimeFit");
    }

    if(IsInGeometricCut(clu) && isIsolated2ns && IsPVetoBremsFiltered(clu, PVetoIsolationWindow) && IsHEPVetoIsolated(clu, HEPVetoIsolationWindow)){
      if (clu.energy > 30.) FillMissingMassHistos(clu, "Ecut30MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTime");
      if (clu.energy > 30.) FillMissingMassDebugHistos(clu, "Ecut30MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTime");
      if (clu.energy > 50.) FillMissingMassHistos(clu, "Ecut50MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTime");
      if (clu.energy > 50.) FillMissingMassDebugHistos(clu, "Ecut50MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTime");
      if (clu.energy > 60.) FillMissingMassHistos(clu, "Ecut60MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTime");
      if (clu.energy > 60.) FillMissingMassDebugHistos(clu, "Ecut60MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTime");
    }

    if(IsInGeometricCut(clu) && isIsolated2ns && IsPVetoBremsFiltered(clu, PVetoIsolationWindow) && IsHEPVetoBremsFiltered(clu, HEPVetoIsolationWindow)){
      if (clu.energy > 30.) FillMissingMassHistos(clu, "Ecut30MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit");
      if (clu.energy > 30.) FillMissingMassDebugHistos(clu, "Ecut30MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit");
      if (clu.energy > 50.) FillMissingMassHistos(clu, "Ecut50MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit");
      if (clu.energy > 50.) FillMissingMassDebugHistos(clu, "Ecut50MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit");
      if (clu.energy > 60.) FillMissingMassHistos(clu, "Ecut60MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit");
      if (clu.energy > 60.) FillMissingMassDebugHistos(clu, "Ecut60MeV_GeomCut_2nsIsolation_PVetoCutTimeFit_HEPVetoCutTimeFit");
    }


  }

}

void DarkPhoton::UBosonDecayHunt() {}

bool DarkPhoton::Preselection(){
  Bool_t passed = true;
  if (!(evt->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED))){
    if (evt->TargetRecoBeam->getnPOT()<13000) passed = false;
  }
  return passed;
}

Bool_t DarkPhoton::Process()
{
  HistoSvc *hSvc = HistoSvc::GetInstance();
  GeneralInfo *fGeneralInfo = GeneralInfo::GetInstance();

  //std::cout << "DarkPhoton Analysis Process started "<< std::endl;
  if(!Preselection()){
    fResult = false;
    return fResult;
  }

  if (fProcessTarget)
    ProcessTarget();

  if (fProcessPVeto)
    ProcessPVeto();

  if (fPVetoEVetoCheck)
    PVetoEVetoCheck();

  if (fProcessEVeto)
    ProcessEVeto();

  if (fProcessHEPVeto)
    ProcessHEPVeto();

  if (fProcessSAC)
    ProcessSAC();

  if (fSACPVetoBrems)
    SACPVetoBrems();

  if (fSACHEPVetoBrems)
    SACHEPVetoBrems();

  if (fSACEVetoCheck)
    SACEVetoCheck();



  ProcessECal();

if (fECalPVetoBrems)
    ECalPVetoBrems();

  if (fECalHEPVetoBrems)
    ECalHEPVetoBrems();

  if (fECalEVetoCheck)
    ECalEVetoCheck();

    if(fECalSACCheck)
    ECalSACCheck();

  if (fDarkPhotonHunt)
    DarkPhotonHunt();

  if (fUBosonDecayHunt)
    UBosonDecayHunt();

  if (fProcessGG)
  {
    for (const auto &clu : ecalAllClusters)
    {
      bool isGG = IsGG(clu);
    }
    ProcessGG();
  }
  fResult = true;
  return fResult;
}


Bool_t DarkPhoton::IsInGeometricCut(const ECalClusterInfo &clu)
{
  return (clu.radius > RADIUS_MIN && clu.radius < RADIUS_MAX);
}

bool DarkPhoton::IsECalIsolated(const ECalClusterInfo &clu, Float_t isolationWindow)
{
  for (const auto &otherClu : ecalAllClusters)
  {
    if (&clu == &otherClu)
      continue;
    if (fabs(clu.time - otherClu.time) < isolationWindow)
    {
      return false;
    }
  }
  return true;
}

bool DarkPhoton::IsPVetoIsolated(const ECalClusterInfo &clu, Float_t isolationWindow)
{
  if (!(evt->PVetoRecoEvent))
    return false;
  Int_t NclusPVeto = evt->PVetoRecoCl->GetNElements();

  for (Int_t j = 0; j < NclusPVeto; ++j)
  {

    TRecoVCluster *cluPVeto = evt->PVetoRecoCl->Element(j);

    Float_t deltaTPVetoECal = cluPVeto->GetTime() - clu.time;

    if (fabs(deltaTPVetoECal) < isolationWindow)
    {
      return false;
    }
  }

  return true;
}

bool DarkPhoton::IsPVetoBremsFiltered(const ECalClusterInfo &clu, Float_t isolationWindow)
{
  if (!(evt->PVetoRecoEvent))
    return false;
  Int_t NclusPVeto = evt->PVetoRecoCl->GetNElements();

  TRecoVCluster *closestCluPVeto = nullptr;
  Float_t closestECalPVeto = 9999.;
  Float_t closestcluPVetoChID = 9999.;

  for (Int_t j = 0; j < NclusPVeto; ++j)
  {
    TRecoVCluster *cluPVeto = evt->PVetoRecoCl->Element(j);
    Float_t cluPVetoChID = cluPVeto->GetChannelId();
    Float_t deltaTPVetoECal = clu.time - cluPVeto->GetTime();// - ShiftECalPVeto;

    if (abs(deltaTPVetoECal) < abs(closestECalPVeto))
    {
      closestECalPVeto = deltaTPVetoECal;
      closestCluPVeto = cluPVeto;
      closestcluPVetoChID = cluPVetoChID;
    }
    //}
    // Float_t closestcluPVetoChID = closestCluPVeto->GetChannelId();
    if ((cluPVetoChID <= 80) && (fabs(deltaTPVetoECal) <= isolationWindow) && (cluPVetoChID) < (alineECalCh * clu.energy * clu.energy + blineECalCh * clu.energy + cgoreECalCh) &&
        (cluPVetoChID) > (alineECalCh * clu.energy * clu.energy + blineECalCh * clu.energy + cdoluECalCh))
    {
      return false;
    }
  }
  return true;
}

bool DarkPhoton::IsSACPVetoBremsFiltered(TRecoVCluster* clu, Float_t isolationWindow)
{
  if (!(evt->PVetoRecoEvent))
    return false;
  Int_t NclusPVeto = evt->PVetoRecoCl->GetNElements();

  TRecoVCluster *closestCluPVeto = nullptr;
  Float_t closestSACPVeto = 9999.;
  Float_t closestcluPVetoChID = 9999.;

  for (Int_t j = 0; j < NclusPVeto; ++j)
  {
    TRecoVCluster *cluPVeto = evt->PVetoRecoCl->Element(j);
    Float_t cluPVetoChID = cluPVeto->GetChannelId();
    Float_t deltaTPVetoSAC = clu->GetTime() - cluPVeto->GetTime();

    if (abs(deltaTPVetoSAC) < abs(closestSACPVeto))
    {
      closestSACPVeto = deltaTPVetoSAC;
      closestCluPVeto = cluPVeto;
      closestcluPVetoChID = cluPVetoChID;
    }
    // }
    // Float_t closestcluPVetoChID = closestCluPVeto->GetChannelId();
    if ((cluPVetoChID <= 80) && (fabs(deltaTPVetoSAC) <= isolationWindow) && (cluPVetoChID) < (alineSACCh * clu->GetEnergy() * clu->GetEnergy() + blineSACCh * clu->GetEnergy() + cgoreSACCh) &&
        (cluPVetoChID) > (alineSACCh * clu->GetEnergy() * clu->GetEnergy() + blineSACCh * clu->GetEnergy() + cdoluSACCh))
    {
      return false;
    }
  }
  return true;
}

bool DarkPhoton::IsHEPVetoIsolated(const ECalClusterInfo &clu, Float_t isolationWindow)
{
  if (!(evt->HEPVetoRecoEvent))
    return false;
  Int_t NclusHEPVeto = evt->HEPVetoRecoCl->GetNElements();

  for (Int_t j = 0; j < NclusHEPVeto; ++j)
  {

    TRecoVCluster *cluHEPVeto = evt->HEPVetoRecoCl->Element(j);

    Float_t deltaTHEPVetoECal = cluHEPVeto->GetTime() - clu.time;

    if (fabs(deltaTHEPVetoECal) < isolationWindow)
    {
      return false;
    }
  }
  return true;
}

bool DarkPhoton::IsHEPVetoBremsFiltered(const ECalClusterInfo &clu, Float_t isolationWindow) {
  if (!(evt->HEPVetoRecoEvent))
    return false;
  Int_t NclusHEPVeto = evt->HEPVetoRecoCl->GetNElements();

  for (Int_t j = 0; j < NclusHEPVeto; ++j)
  {
    TRecoVCluster *cluHEPVeto = evt->HEPVetoRecoCl->Element(j);
    Float_t cluHEPVetoChID = cluHEPVeto->GetChannelId();
    Float_t deltaTHEPVetoECal = cluHEPVeto->GetTime() - clu.time;

    if ((fabs(deltaTHEPVetoECal) <= isolationWindow) && (cluHEPVetoChID) < (alineECalHEPCh * clu.energy * clu.energy + blineECalHEPCh * clu.energy + cgoreECalHEPCh) &&
        (cluHEPVetoChID) > (alineECalHEPCh * clu.energy * clu.energy + blineECalHEPCh * clu.energy + cdoluECalHEPCh))
    {
      return false;
    }
  }
  return true;
}

void DarkPhoton::FillMissingMassHistos(const ECalClusterInfo &clu, const std::string &suffix)
{
  HistoSvc *hSvc = HistoSvc::GetInstance();

  std::string baseName = suffix.empty() ? "" : "_" + suffix;

  hSvc->FillHisto("ClusterEnergy" + baseName, clu.energy);
  hSvc->FillHisto("MissingMass" + baseName, clu.missingMass);
  hSvc->FillHisto("MissingMomentum" + baseName, clu.missingMomentum);
  hSvc->FillHisto("MissingTheta" + baseName, clu.missingTheta);
  hSvc->FillHisto2("MissingMassTheta" + baseName, clu.missingMass, clu.theta);
  hSvc->FillHisto2("MissingMassRadius" + baseName, clu.missingMass, clu.radius);
  hSvc->FillHisto2("MissingMomentumMissingTheta" + baseName, clu.missingMomentum, clu.missingTheta);
  hSvc->FillHisto2("ClusterEnergy_Radius" + baseName, clu.energy, clu.radius);
}

void DarkPhoton::FillMissingMassDebugHistos(const ECalClusterInfo &clu, const std::string &suffix)
{
  HistoSvc *hSvc = HistoSvc::GetInstance();
  Int_t NclusPVeto = evt->PVetoRecoCl->GetNElements();
  Int_t NclusHEPVeto = evt->HEPVetoRecoCl->GetNElements();

  std::string baseName = suffix.empty() ? "" : "_" + suffix;
  if (clu.missingMass > DebugMin && clu.missingMass < DebugMax)
  {
    hSvc->FillHisto("Debug_ClusterEnergy" + baseName, clu.energy);
    hSvc->FillHisto("Debug_ClusterRadius" + baseName, clu.radius);
    hSvc->FillHisto("Debug_ClusterTime" + baseName, clu.time);
    hSvc->FillHisto2("Debug_ClusterPosition" + baseName, clu.x, clu.y);

    for (Int_t j = 0; j < NclusPVeto; ++j)
    {
      TRecoVCluster *cluPVeto = evt->PVetoRecoCl->Element(j);
      Float_t deltaTPVetoECal = cluPVeto->GetTime() - clu.time;

      hSvc->FillHisto("Debug_PVetoTimeDiff" + baseName, deltaTPVetoECal);

      if (fabs(deltaTPVetoECal) < PVetoIsolationWindow)
      {
        hSvc->FillHisto2("Debug_ECalEnergy_PVetoChID_inTime" + baseName, clu.energy, cluPVeto->GetChannelId());
      }
    }

    for (Int_t j = 0; j < NclusHEPVeto; ++j)
    {
      TRecoVCluster *cluHEPVeto = evt->HEPVetoRecoCl->Element(j);
      Float_t deltaTHEPVetoECal = cluHEPVeto->GetTime() - clu.time;

      hSvc->FillHisto("Debug_HEPVetoTimeDiff" + baseName, deltaTHEPVetoECal);

      if (fabs(deltaTHEPVetoECal) < HEPVetoIsolationWindow)
      {
        hSvc->FillHisto2("Debug_ECalEnergy_HEPVetoChID_inTime" + baseName, clu.energy, cluHEPVeto->GetChannelId());
      }
    }
  }
}

Bool_t DarkPhoton::Finalize()
{

  std::cerr << "=== FINALIZE ENTERED ===" << std::endl;

  return true;
}
