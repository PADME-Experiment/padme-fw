#include "TagAndProbe.hh"

#include "TRecoEvent.hh"
#include "TRecoVObject.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "HistoSvc.hh"
#include "TVector3.h"
#include "TProfile.h"
#include "TString.h"
#include "TObjString.h"
#include "TLorentzVector.h"
#include <iostream>

#define BFIELD false

TagAndProbe *TagAndProbe::fInstance = 0;
TagAndProbe *TagAndProbe::GetInstance()
{
  if (fInstance == 0)
  {
    fInstance = new TagAndProbe();
  }
  return fInstance;
}

TagAndProbe::TagAndProbe()
{

  fRecoEvent = NULL;
  fVerbose = 0;
  TString cfgFile = "config/UserAnalysis.conf";
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
  fcfgPath = TString(fCfgParser->GetSingleArg("ECAL", "TPWithMMPath"));
  fUseMM = true;
}
TagAndProbe::~TagAndProbe()
{
  if (fCfgParser)
    delete fCfgParser;
}

Bool_t TagAndProbe::Init(PadmeAnalysisEvent *event, Bool_t HistoMode, TString InputHistofile)
{
  fRecoEvent = event->RecoEvent;
  fECal_hitEvent = event->ECalRecoEvent;
  fECal_clEvent = event->ECalRecoCl;
  fEvent = event;
  fRecoEvent = event->RecoEvent;
  fECal_hitEvent = event->ECalRecoEvent;
  fECal_clEvent = event->ECalRecoCl;
  fGeneralInfo = GeneralInfo::GetInstance();
  fhSvcVal = HistoSvc::GetInstance();
  fMCTruthECal = MCTruthECal::GetInstance();
  fNPoTAnalysis = NPoTAnalysis::GetInstance();
  fECalSel = ECalSel::GetInstance();
  // fMMTrackDevel = MMTrackDevel::GetInstance();
  fMMFindBestTrack = MMFindBestTrack::GetInstance();

  fInputHistofile = InputHistofile;
  fHistoMode = HistoMode;

  fNSlicesE = 1 + (Int_t)(fGeneralInfo->GetEnergyMax() - fGeneralInfo->GetEnergyMin()) / spacing;

  // PRENDE VARIABILI SELEZIONE DA ECALSEL (TAGLI THETA - PHI)
  InitHistos();

  return true;
}

Bool_t TagAndProbe::Process()
{
  TagAndProbeSelection();
  return true;
}

Bool_t TagAndProbe::InitHistos()
{

  double fXMin = -21. * (14 + 0.5);
  double fXMax = 21. * (14 + 0.5);
  double fXW = 21; // mm
  int fNXBins = (fXMax - fXMin) / fXW;
  double fYMin = -21. * (14 + 0.5);
  double fYMax = 21. * (14 + 0.5);
  double fYW = 21; // mm

  int fNYBins = (fYMax - fYMin) / fYW;

  fhSvcVal->CreateList("TagAndProbe");
  fhSvcVal->CreateList("TagAndProbe_sliced");
  fhSvcVal->BookHisto2List("TagAndProbe", "ECal_TP_DEVsE_NOcut_tag", 400, 0, 400, 600, -300, 300);   // da mettere quello in phi
  fhSvcVal->BookHisto2List("TagAndProbe", "ECal_TP_DEVsE_NOcut_probe", 400, 0, 400, 600, -300, 300); // da mettere quello in phi
  fhSvcVal->BookHisto2List("TagAndProbe", "ECal_TP_DEvsE_probe", 400, 0, 400, 600, -300, 300);       // da mettere quello in phi
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_dRvsDE_tag"), 600, -300, 300, 600, -600, 600);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_dRvsdRold"), 600, -600, 600, 600, -600, 600);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_dRvsDE_probe"), 600, -300, 300, 600, -600, 600);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_CatvsDE"), 600, -300, 300, 7, -0.5, 6.5);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_CatvOthersDE"), 600, -300, 300, 7, -0.5, 6.5);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_VvsQ"), 4, -0.5, 3.5, 600, -600, 600);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_VextvsQ"), 4, -0.5, 3.5, 600, -600, 600);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DTHEVsDPHIAbs_probe_nocut"), 600, 0, 2 * TMath::Pi(), 600, 0, 2 * TMath::Pi());
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_Cat1vs2"), 7, -0.5, 6.5, 7, -0.5, 6.5);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_globalCatvsDE"), 600, -300, 300, 8, -1.5, 6.5);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsDE_lvl0_TAG"), 600, -300, 300, 600, -600, 600);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsDE_lvl1_TAG"), 600, -300, 300, 600, -600, 600);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsDE_lvl0_PROBE"), 600, -300, 300, 600, -600, 600);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsDE_lvl1_PROBE"), 600, -300, 300, 600, -600, 600);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_lvl0_TAG"), 50, 0, 1, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_lvl1_TAG"), 50, 0, 1, 600, -300, 300);

  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsDV_4hit_lvl1_TAG"), 200, -5, 5, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_4hit_lvl1_TAG"), 50, 0, 1, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_5hit_lvl1_TAG"), 50, 0, 1, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_6phit_lvl1_TAG"), 50, 0, 1, 600, -300, 300);

  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_3hit_lvl0_TAG"), 50, 0, 1, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_4hit_lvl0_TAG"), 50, 0, 1, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_5phit_lvl0_TAG"), 50, 0, 1, 600, -300, 300);

  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsDV_4hit_lvl1_ov_TAG"), 50, 0, 1, 600, -300, 300);
  // fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_4hit_lvl1_ov_TAG"), 50,0,1,600,-300,300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_5hit_lvl1_ov_TAG"), 50, 0, 1, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_6phit_lvl1_ov_TAG"), 50, 0, 1, 600, -300, 300);

  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_3hit_lvl0_ov_TAG"), 50, 0, 1, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_4hit_lvl0_ov_TAG"), 50, 0, 1, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_5phit_lvl0_ov_TAG"), 50, 0, 1, 600, -300, 300);

  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_lvl0_PROBE"), 50, 0, 1, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_lvl1_PROBE"), 50, 0, 1, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsDV_4hit_lvl1_PROBE"), 200, -5, 5, 600, -300, 300);
  // fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_4hit_lvl1_PROBE"), 50,0,1,600,-300,300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_5hit_lvl1_PROBE"), 50, 0, 1, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_6phit_lvl1_PROBE"), 50, 0, 1, 600, -300, 300);

  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_PurityvsDE_3h_lvl0_TAG"), 600, -300, 300, 50, 0, 1);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_PurityvsDE_4h_lvl0_TAG"), 600, -300, 300, 50, 0, 1);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_PurityvsDE_5h_lvl1_TAG"), 600, -300, 300, 50, 0, 1);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_PurityvsDE_6ph_lvl1_TAG"), 600, -300, 300, 50, 0, 1);

  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_PurityvsDE_3h_lvl0_PROBE"), 600, -300, 300, 50, 0, 1);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_PurityvsDE_4h_lvl0_PROBE"), 600, -300, 300, 50, 0, 1);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_PurityvsDE_5h_lvl1_PROBE"), 600, -300, 300, 50, 0, 1);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_PurityvsDE_6ph_lvl1_PROBE"), 600, -300, 300, 50, 0, 1);

  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_PurityvsDE_TAG_PROBE"), 600, -300, 300, 100, 0, 2);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_QualityvsDE_TAG_PROBE"), 600, -300, 300, 25, -0.5, 24.5);
  for(int vw=0; vw<2; vw++) {
    TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(vw);
    fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_D%svs%s_TAG",viewlabel.Data(),viewlabel.Data()),600,-300,300,600,-300,300);
    fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_D%svs%s_PROBE",viewlabel.Data(),viewlabel.Data()),600,-300,300,600,-300,300);
  }
  
  for(int iProcess = 0; iProcess < fNprocessAvailableTwoClu; iProcess++)
  {
    TString proc = fprocessIDsTwoClu[iProcess];
    fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_QualityvsDE_TAG_PROBE_%s", proc.Data()), 600, -300, 300, 25, -0.5, 24.5);
    fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_QualityvsDE_PROBED_%s", proc.Data()), 600, -300, 300, 25, -0.5, 24.5);
  }
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_QualityNewvsDE_PROBE"), 600, -300, 300, 2, -0.5, 1.5);

  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_QualityvsSlope_TAG"), 200, -0.5, 0.5, 25, -0.5, 24.5);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_QualityvsInter_TAG"),  600, -300, 300, 25, -0.5, 24.5);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_QualityvsSlope_PROBE"), 200, -0.5, 0.5, 25, -0.5, 24.5);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_QualityvsInter_PROBE"),  600, -300, 300, 25, -0.5, 24.5);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_QualityvsRECal_TAG_PROBE"),  300, 0, 300, 25, -0.5, 24.5);
  
  
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_CluPos_X_vs_Y_HIGH_Purity_TAG"), 600, -300, 300, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_CluPos_X_vs_Y_HIGH_Purity_PROBE"), 600, -300, 300, 600, -300, 300);

  fhSvcVal->BookHisto2List("TagAndProbe", "ECal_TP_DEVsE_BESTQTAG", 400, 0, 400, 600, -300, 300);   // da mettere quello in phi
  fhSvcVal->BookHisto2List("TagAndProbe", "ECal_TP_DEVsE_BESTQPROBE", 400, 0, 400, 600, -300, 300); // da mettere quello in phi

  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_PChi2vsNhit_lvl1"), 3, 3.5, 6.5, 50, 0, 1);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_PChi2vsNhit_lvl0"), 3, 2.5, 5.5, 50, 0, 1);
  // fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_dVvsdTheta_fromlvl0_lvl1"),600,-0.05,0.05,600,-5,5);
  // fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_dVvsdTheta_fromlvl0_dRmatched_lvl1_TAG"),300,-0.05,0.05,600,-5,5);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_dRvsdTheta_fromlvl0_lvl1"), 300, -0.05, 0.05, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_dRvsdV_fromlvl0_lvl1"), 600, -5, 5, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_dZ-dZEcalvsDE_TAG"), 600, -300, 300, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_dZ-dZEcalvsDE_PROBE"), 600, -300, 300, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_dZTAGvsdZPROBE"), 600, -300, 300, 600, -300, 300);

  fhSvcVal->BookHisto2List("TagAndProbe", "ECal_TP_DE_ele_vs_DE_pos_TAG", 600, -300, 300, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", "ECal_TP_DEexp_ele_vs_DEexp_pos_TAG", 600, -300, 300, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", "ECal_TP_DEexp_vs_E_pos_TAG", 400, 0, 400, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", "ECal_TP_DEexp_vs_E_ele_TAG", 400, 0, 400, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_P_vs_R_ELE_TAG"), 400, -0.5, 399.5, 350, -0.5, 349.5);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_P_vs_R_POS_TAG"), 400, -0.5, 399.5, 350, -0.5, 349.5);

  for (int iSlice = 0; iSlice < fNSlicesE; iSlice++)
  {
    fhSvcVal->BookHisto2List("TagAndProbe_sliced", Form("ECal_TP_DEvsPhiExp_tag_slice_%i", iSlice), 600, -TMath::Pi(), TMath::Pi(), 600, -300, 300);
    fhSvcVal->BookHisto2List("TagAndProbe_sliced", Form("ECal_TP_DEvsE_probe_slice_%i", iSlice), 400, 0, 400, 600, -300, 300);
    fhSvcVal->BookHisto2List("TagAndProbe_sliced", Form("ECal_TP_DPhivsPhiExp_probe_slice_%i", iSlice), 600, -TMath::Pi(), TMath::Pi(), 600, 0, 2 * TMath::Pi());
    fhSvcVal->BookHisto2List("TagAndProbe_sliced", Form("ECal_TP_DTHEVsDPHIAbs_probe_slice_%i", iSlice), 600, 0, 2 * TMath::Pi(), 600, 0, 2 * TMath::Pi());
    fhSvcVal->BookHisto2List("TagAndProbe_sliced", Form("ECal_TP_dRvsDE_tag_slice_%i", iSlice), 600, -300, 300, 600, -600, 600);
  }

  return true;
}

Double_t TagAndProbe::PurityFunc(Double_t x, Double_t y, Double_t *p)
{

  double sigY = TMath::Exp(-0.5 * (y - p[2]) * (y - p[2]) / (p[3] * p[3]));
  double bkgY = TMath::Exp(-0.5 * (y - p[4]) * (y - p[4]) / (p[5] * p[5]));

  double sigX = TMath::Exp(p[6] * x);
  double bkgX = TMath::Exp(-p[7] * x);

  return p[0] * sigX * sigY / (p[0] * sigX * sigY + p[1] * bkgX * bkgY);
}

Double_t TagAndProbe::PFuncAtECal(Double_t x, Double_t y, Double_t *p)
{
  double r = TMath::Sqrt(x * x + y * y);
  double phi = TMath::ATan2(y, x);
  double cosphi = TMath::Cos(phi);
  double sinphi = TMath::Sin(phi);

  double A0 = p[0];
  double A1 = p[1] * cosphi + p[2] * sinphi;
  double A2 = (r / 400.) * (p[3] + p[4] * cosphi);
  double A3 = (r / 400.) * (r / 400.) * (p[6] + p[7] * cosphi + p[8] * sinphi);
  double A4 = (r / 400.) * (r / 400.) * (r / 400.) * (p[9] + p[10] * cosphi + p[11] * sinphi);

  return A0 + A1 + A2 + A3 + A4;
}

Int_t TagAndProbe::ChargeFinder(Double_t x, Double_t y, Double_t E)
{
  Double_t me = 0.511; // MeV
  Double_t p_exp_ele = 0;
  Double_t p_exp_pos = 0;
  if (fabs(GeneralInfo::GetInstance()->GetBField() - 100.) < 1)
  {
    p_exp_ele = PFuncAtECal(x, y, par_ele_B100G);
    p_exp_pos = PFuncAtECal(x, y, par_pos_B100G);
  }
  Double_t E_exp_ele = TMath::Sqrt(p_exp_ele * p_exp_ele + me * me);
  Double_t E_exp_pos = TMath::Sqrt(p_exp_pos * p_exp_pos + me * me);

  int charge = 0;
  if (fabs(E_exp_ele - E) < fabs(E_exp_pos - E))
    charge = -1;
  else
    charge = 1;

  return charge;
}

Int_t TagAndProbe::TagAndProbeSelection()
{
  // loop over events and fill the struct with the info of interest for the tag and probe selection

  // then fill the histos for the tag and probe efficiency evaluation
  TRecoVCluster *tempClu[2];
  TVector3 cluPos[2], cluPosRel[2];
  double cluTime[2];
  double cluEnergy[2];
  TLorentzVector labMomenta[2];
  TLorentzVector labMomentaCM[2];
  for (int h1 = 0; h1 < fECal_clEvent->GetNElements(); h1++)
  {
    tempClu[0] = fECal_clEvent->Element(h1);
    cluPos[0].SetXYZ(
        tempClu[0]->GetPosition().X(),
        tempClu[0]->GetPosition().Y(), fGeneralInfo->GetCOG().Z());
    cluPosRel[0].SetXYZ(
        tempClu[0]->GetPosition().X() - fGeneralInfo->GetCOG().X(),
        tempClu[0]->GetPosition().Y() - fGeneralInfo->GetCOG().Y(),
        0.);
    cluTime[0] = tempClu[0]->GetTime();
    cluEnergy[0] = tempClu[0]->GetEnergy();
    // calcute expected energy for a two body product coming from the target center and hitting the cluster position
    //  Direction of the observed cluster from the target
    TVector3 cluDir = cluPos[0] - fGeneralInfo->GetTargetPos();
    cluDir *= (1.0 / cluDir.Mag());
    // cos(theta_lab) between photon and boost axis
    double cosTheta = cluDir.Dot(fGeneralInfo->GetBoost()) / fGeneralInfo->GetBeta();
    // Expected photon energy for e+e- -> gamma gamma
    double pg = 0.5 * fGeneralInfo->GetSqrts() / (fGeneralInfo->GetGam() * (1.0 - fGeneralInfo->GetBeta() * cosTheta));
    double probeExpEnergy = fGeneralInfo->GetBeamEnergy() - pg;
    // std::cout << "Expected photon energy TAGAND PROBE h1: " << h1<<" "<<pg << std::endl;
    //  Photon 1 momentum
    TVector3 cluMom = pg * cluDir;
    // Initial-state momentum:
    // P_init = E_init * beta_vec = gamma * sqrt(s) * beta_vec
    TVector3 pInit = fGeneralInfo->GetGam() * fGeneralInfo->GetSqrts() * (fGeneralInfo->GetBoost() * (1 / fGeneralInfo->GetBeta()));
    // Photon 2 momentum from momentum conservation
    TVector3 otherCluMom = pInit - cluMom;
    labMomenta[0].SetVectM(cluMom, 0.); // define a photon-like tlorentzVector
    labMomentaCM[0].SetVectM(labMomenta[0].Vect(), 0);
    labMomentaCM[0].Boost(-fGeneralInfo->GetBoost());

    double Xexph2 = fGeneralInfo->GetTargetPos().X() + (otherCluMom.X() / otherCluMom.Z()) * (fGeneralInfo->GetCOG().Z() - fGeneralInfo->GetTargetPos().Z());
    double Yexph2 = fGeneralInfo->GetTargetPos().Y() + (otherCluMom.Y() / otherCluMom.Z()) * (fGeneralInfo->GetCOG().Z() - fGeneralInfo->GetTargetPos().Z());
    TVector3 ExpPosProbe(Xexph2, Yexph2, fGeneralInfo->GetCOG().Z());
    double PhiExpProbe = TMath::ATan2(Yexph2, Xexph2);

    // Selection cuts for the tag cluster (h1) --> Radious, Minimum amount of hits
    if (!BFIELD)
    {
      if (cluPosRel[0].Perp() < fGeneralInfo->GetRadiusMin())
        continue; // cluster should be within the radius range of the 2gamma cluster pair
      if (cluPosRel[0].Perp() > fGeneralInfo->GetRadiusMax())
        continue; // cluster should be within the radius range of the 2gamma cluster pair
    }

    if (tempClu[0]->GetNHitsInClus() < 3)
      continue;

    if (!BFIELD)
    {
      if (fGeneralInfo->GetBeamEnergy() - pg < fGeneralInfo->GetEnergyMin() || fGeneralInfo->GetBeamEnergy() - pg > fGeneralInfo->GetEnergyMax())
        continue; // energy of the probe cluster should be within the energy range of the 2gamma cluster pair
    }

    Int_t iSlice = (Int_t)(fGeneralInfo->GetBeamEnergy() - pg - fGeneralInfo->GetEnergyMin()) / spacing;
    //MC Truth
    TString processSelected;
    if (fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED))
        {
          if (fMCTruthECal->GetVtxFromCluID(h1) < 0)
          {
            processSelected = "NoVtx";
          }else
          {
            TMCVertex *mcVtx = fEvent->MCTruthEvent->Vertex(fMCTruthECal->GetVtxFromCluID(h1));
            processSelected = mcVtx->GetProcess().Data();
          }
        }

    // std::cout<<"Eprobe: "<<fGeneralInfo->GetBeamEnergy()-pg<<" pg "<<" Edown: "<<Edown<<" Eup: "<<Eup<<" iSlice: "<<iSlice<<" lowerbound:"<<Edown + (spacing * (iSlice))<<" upperbound:"<<Edown + (spacing * (iSlice + 1))<<std::endl;
    // fhSvcVal->FillHisto2List("ECalSel", Form("ECal_TP_DEVsE_noSel"), pg, cluEnergy[0] - pg, 1.);
    // fhSvcVal->FillHisto2List("TagAndProbe", "ECal_TP_DEVsE_NOcut_tag", fGeneralInfo->GetBeamEnergy() - pg, cluEnergy[0] - pg, 1.); // da mettere quello in phi
    // fhSvcVal->FillHisto2List("TagAndProbe_sliced", Form("ECal_TP_DEvsPhiExp_tag_slice_%i", iSlice), PhiExpProbe, cluEnergy[0] - pg, 1.);

    // MAGNETIC FIELD STUFF
    Double_t p_exp_ele = 0;
    Double_t p_exp_pos = 0;
    Double_t me = 0.511; // MeV
    if (fabs(GeneralInfo::GetInstance()->GetBField() - 100.) < 1)
    {
      p_exp_ele = PFuncAtECal(cluPos[0].X(), cluPos[0].Y(), par_ele_B100G);
      p_exp_pos = PFuncAtECal(cluPos[0].X(), cluPos[0].Y(), par_pos_B100G);
    }
    Double_t E_exp_ele = TMath::Sqrt(p_exp_ele * p_exp_ele + me * me);
    Double_t E_exp_pos = TMath::Sqrt(p_exp_pos * p_exp_pos + me * me);
    fhSvcVal->FillHisto2List("TagAndProbe", "ECal_TP_DE_ele_vs_DE_pos_TAG", E_exp_pos - pg, E_exp_ele - pg, 1.);
    fhSvcVal->FillHisto2List("TagAndProbe", "ECal_TP_DEexp_ele_vs_DEexp_pos_TAG", E_exp_pos - cluEnergy[0], E_exp_ele - cluEnergy[0], 1.);
    fhSvcVal->FillHisto2List("TagAndProbe", "ECal_TP_DEexp_vs_E_ele_TAG", cluEnergy[0], E_exp_ele - cluEnergy[0], 1.);
    fhSvcVal->FillHisto2List("TagAndProbe", "ECal_TP_DEexp_vs_E_pos_TAG", cluEnergy[0], E_exp_pos - cluEnergy[0], 1.);

    int quad = -1;
    if (cluPos[0].X() < 0 && cluPos[0].Y() < 0)
      quad = 0;
    else if (cluPos[0].X() < 0 && cluPos[0].Y() > 0)
      quad = 1;
    else if (cluPos[0].X() > 0 && cluPos[0].Y() > 0)
      quad = 2;
    else
      quad = 3;

    int quadOther = -1;

    if (Xexph2 < 0 && Yexph2 < 0)
      quadOther = 0;
    else if (Xexph2 < 0 && Yexph2 > 0)
      quadOther = 1;
    else if (Xexph2 > 0 && Yexph2 > 0)
      quadOther = 2;
    else
      quadOther = 3;

    bool filledquad[4] = {false, false, false, false};
    bool filledquadOther[4] = {false, false, false, false};
    // casi

    // 0  1-in pos   1-non in pos  2 una in una no  2 entrambe in pos 2 nessuna in pos
    // 0  1           2              3              4                 5
    // return 0;

    int quality = -999;
    if (fUseMM)
    { ////// TO BE IMPLEMENTED //provare a uscire con più cluster per vista, aggiungere dZ nel tracklet e in best fit
      vector<MMBestTrack *> trackvect = fMMFindBestTrack->GetVectorTracks();
      // std::cout<<"Number of tracks in the event: "<<trackvect.size()<<std::endl;

      int nTracksInQuad = 0;
      int nTracksInPosInQuad = 0;

      int nTracksInQuadOther = 0;
      int nTracksInPosInQuadOther = 0;

      double purity_TAG = -999;
      double purity_PROBE = -999;
      double pars_3h_lvl0[8] = {7.56008, 58.3746, -1.2901, 7.28396, 16.0513, 105.037, 3.37634, 0.263186};
      double pars_4h_lvl0[8] = {0.0265705, 92.0291, -1.24075, 6.90274, 19.2264, 104.307, 8.19723, 1.84315};
      double pars_5h_lvl1[8] = {18.5897, 34.5641, 0.906092, 11.324, 5.74614, 87.1964, 2.09614, 0.914284};
      double pars_6ph_lvl1[8] = {204.73, 306.454, 1.26521, 11.1982, 4.69843, 99.2033, -0.781994, 4.14681};

      int first_view = -999;
      Double_t dROld = -999;

      double m_best_TAG = -999;
      double c_best_TAG = -999;
      double m_best_PROBE = -999;
      double c_best_PROBE = -999;
      
      for (auto it = begin(trackvect); it != end(trackvect); ++it)
      {
        MMBestTrack *track = *it;
        // std::cout<<"Track extrapolation TAG AND PROBE: clu quad: "<<quad<<" track quad: "<<track->quad<<" track view: "<<track->view<<" tracklet slope:  "<<track->slope<<"tracklet inter: "<<track->inter<<std::endl;

        if (track->quad != quad)
          continue;

        TVector3 extPos = track->BestTrackExtrapolationAtZ(fGeneralInfo->GetCOG().Z());
        double dR = (extPos[1 - track->view] - cluPos[0][1 - track->view]);
        double dV = -999;
        double dTheta = -999;
	
        fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_lvl%d_TAG", track->level), track->pchi2, dR, 1.);

        if (track->level == 1)
        {
          dV = track->inter_lvl0[0] - track->inter_lvl0[1];
          dTheta = TMath::ATan(track->slope_lvl0[0]) - TMath::ATan(track->slope_lvl0[1]);
          // fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_dVvsdTheta_fromlvl0_lvl1"),dTheta,dV);
          fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_dRvsdTheta_fromlvl0_lvl1"), dTheta, dR);
          fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_dRvsdV_fromlvl0_lvl1"), dV, dR);

          if (track->nhit == 4)
            fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsDV_4hit_lvl1_TAG"), dV, dR, 1.);
          if (track->nhit == 4)
            fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_4hit_lvl1_TAG"), track->pchi2, dR, 1.);
          if (track->nhit == 5)
            fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_5hit_lvl1_TAG"), track->pchi2, dR, 1.);
          if (track->nhit > 5)
            fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_6phit_lvl1_TAG"), track->pchi2, dR, 1.);

          fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_PChi2vsNhit_lvl1"), track->nhit, track->pchi2, 1.);
          fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_dZ-dZEcalvsDE_TAG"), cluEnergy[0] - pg, track->pars[4] - (cluTime[0] + 440) * GeneralInfo::GetInstance()->GetMMDriftVelocity()); // TIME OFFSET HARDCODED
        }
        else
        { // track->level == 0
          if (track->nhit == 3)
            fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_3hit_lvl0_TAG"), track->pchi2, dR, 1.);
          if (track->nhit == 4)
            fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_4hit_lvl0_TAG"), track->pchi2, dR, 1.);
          if (track->nhit > 4)
            fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_5phit_lvl0_TAG"), track->pchi2, dR, 1.);

          fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_PChi2vsNhit_lvl0"), track->nhit, track->pchi2, 1.);
        }

        if (track->pchi2 < 0.6)
          continue;
        nTracksInQuad++;

        double purity = -999;
        if (track->level == 0)
        {
          if (track->nhit == 3)
          {
            purity = PurityFunc(track->pchi2, dR, pars_3h_lvl0);
            fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_PurityvsDE_3h_lvl0_TAG"), cluEnergy[0] - pg, purity, 1.);
          }
          if (track->nhit == 4)
          {
            purity = PurityFunc(track->pchi2, dR, pars_4h_lvl0);
            fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_PurityvsDE_4h_lvl0_TAG"), cluEnergy[0] - pg, purity, 1.);
          }
        }
        else
        { // track->level == 1
          if (track->nhit == 5)
          {
            purity = PurityFunc(track->pchi2, dR, pars_5h_lvl1);
            fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_PurityvsDE_5h_lvl1_TAG"), cluEnergy[0] - pg, purity, 1.);
          }
          if (track->nhit > 5)
          {
            purity = PurityFunc(track->pchi2, dR, pars_6ph_lvl1);
            fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_PurityvsDE_6ph_lvl1_TAG"), cluEnergy[0] - pg, purity, 1.);
          }
        }

        if (purity > purity_TAG) {
	  purity_TAG = purity;
	  m_best_TAG = track->slope;
	  c_best_TAG = track->inter;
	}

	if (purity > 0.8)
	{
          fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_CluPos_X_vs_Y_HIGH_Purity_TAG"), cluPos[0].X(), cluPos[0].Y());

	  TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(track->view);    
	  fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_D%svs%s_TAG",viewlabel.Data(),viewlabel.Data()), cluPos[0][1-track->view], dR, 1.);
	  
          int charge = ChargeFinder(cluPos[0].X(), cluPos[0].Y(), cluEnergy[0]);
          if (charge == -1)
            fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_P_vs_R_ELE_TAG"), TMath::Sqrt(cluPos[0].X() * cluPos[0].X() + cluPos[0].Y() * cluPos[0].Y()), cluEnergy[0]);
          else
            fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_P_vs_R_POS_TAG"), TMath::Sqrt(cluPos[0].X() * cluPos[0].X() + cluPos[0].Y() * cluPos[0].Y()), cluEnergy[0]);
        }

        fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsDE_lvl%d_TAG", track->level), cluEnergy[0] - pg, dR, 1.);

        if (nTracksInQuad == 2)
        {
          fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_dRvsdRold"), dROld, dR, 1.);
        }

        if (fabs(dR + 2.41) < (20))
        { // TO BE CHANGED !!!!!

          // if(track->level == 1) fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_dVvsdTheta_fromlvl0_dRmatched_lvl1_TAG"),dTheta,dV);
          nTracksInPosInQuad++;
          if (nTracksInPosInQuad == 1)
            first_view = track->view;
        }
        dROld = dR;
        if (nTracksInPosInQuad > 0 && first_view != track->view)
        {
          if (track->level == 1)
          {
            if (track->nhit == 4)
              fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsDV_4hit_lvl1_ov_TAG"), dV, dR, 1.);
            // if(track->nhit == 4) fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_4hit_lvl1_ov_TAG"), track->pchi2, dR, 1.);
            if (track->nhit == 5)
              fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_5hit_lvl1_ov_TAG"), track->pchi2, dR, 1.);
            if (track->nhit > 5)
              fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_6phit_lvl1_ov_TAG"), track->pchi2, dR, 1.);
          }
          else
          { // track->level == 0
            if (track->nhit == 3)
              fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_3hit_lvl0_ov_TAG"), track->pchi2, dR, 1.);
            if (track->nhit == 4)
              fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_4hit_lvl0_ov_TAG"), track->pchi2, dR, 1.);
            if (track->nhit > 4)
              fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_5phit_lvl0_ov_TAG"), track->pchi2, dR, 1.);
          }
        }
      }

      for (auto it2 = begin(trackvect); it2 != end(trackvect); ++it2)
      {
        MMBestTrack *track2 = *it2;
        if (track2->quad != quadOther)
          continue;

        TVector3 extPos2 = track2->BestTrackExtrapolationAtZ(fGeneralInfo->GetCOG().Z());
        double dR2 = (extPos2[1 - track2->view] - ExpPosProbe[1 - track2->view]);

        fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_lvl%d_PROBE", track2->level), track2->pchi2, dR2, 1.);

        if (track2->level == 1)
        {
          double dV = track2->inter_lvl0[0] - track2->inter_lvl0[1];
          double dTheta = TMath::ATan(track2->slope_lvl0[0]) - TMath::ATan(track2->slope_lvl0[1]);
          // fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_dVvsdTheta_fromlvl0_lvl1"),dTheta,dV);

          if (track2->nhit == 4 && track2->pchi2 > 0.6)
            fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsDV_4hit_lvl1_PROBE"), dV, dR2, 1.);
          // if(track2->nhit == 4) fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_4hit_lvl1_PROBE"), track2->pchi2, dR2, 1.);
          if (track2->nhit == 5)
            fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_5hit_lvl1_PROBE"), track2->pchi2, dR2, 1.);
          if (track2->nhit > 5)
            fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsPChi2_6phit_lvl1_PROBE"), track2->pchi2, dR2, 1.);

          fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_dZ-dZEcalvsDE_PROBE"), cluEnergy[0] - pg, track2->pars[4] - (cluTime[0] + 440) * GeneralInfo::GetInstance()->GetMMDriftVelocity()); // TIME OFFSET HARDCODED
        }

        if (track2->pchi2 < 0.6)
          continue;

        double purity = -999;
        if (track2->level == 0)
        {
          if (track2->nhit == 3)
          {
            purity = PurityFunc(track2->pchi2, dR2, pars_3h_lvl0);
            fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_PurityvsDE_3h_lvl0_PROBE"), cluEnergy[0] - pg, purity, 1.);
          }
          if (track2->nhit == 4)
          {
            purity = PurityFunc(track2->pchi2, dR2, pars_4h_lvl0);
            fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_PurityvsDE_4h_lvl0_PROBE"), cluEnergy[0] - pg, purity, 1.);
          }
        }
        else
        { // track2->level == 1
          if (track2->nhit == 5)
          {
            purity = PurityFunc(track2->pchi2, dR2, pars_5h_lvl1);
            fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_PurityvsDE_5h_lvl1_PROBE"), cluEnergy[0] - pg, purity, 1.);
          }
          if (track2->nhit > 5)
          {
            purity = PurityFunc(track2->pchi2, dR2, pars_6ph_lvl1);
            fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_PurityvsDE_6ph_lvl1_PROBE"), cluEnergy[0] - pg, purity, 1.);
          }
        }

        if (purity > purity_PROBE)
	{
          purity_PROBE = purity;
	  m_best_PROBE = track2->slope;
	  c_best_PROBE = track2->inter;
	}
	if (purity > 0.8)
	{
	  fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_CluPos_X_vs_Y_HIGH_Purity_PROBE"), ExpPosProbe.X(), ExpPosProbe.Y());
	  TString viewlabel = GeneralInfo::GetInstance()->GetMMViewLabel(track2->view);    
	  fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_D%svs%s_PROBE",viewlabel.Data(),viewlabel.Data()), cluPos[0][1-track2->view], dR2, 1.);
	}
	
        nTracksInQuadOther++;

        if (nTracksInPosInQuad > 0)
        {
          fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsDE_lvl%d_PROBE", track2->level), cluEnergy[0] - pg, dR2, 1.);
        }

        if (fabs(dR2 + 5) < (33))
        { // TO BE CHANGED !!!!!
          nTracksInPosInQuadOther++;
        }
      }
      
      if (purity_TAG > 0.1 && purity_PROBE > 0.1)
        fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_PurityvsDE_TAG_PROBE"), cluEnergy[0] - pg, purity_TAG + purity_PROBE, 1.);
      //        purity_TAG < 0      --> quality_bin_TAG = 0 (no track considered)
      //    0 < purity_TAG < 0.02   --> quality_bin_TAG = 1 (bad purity)
      // 0.02 < purity_TAG < 0.2    --> quality_bin_TAG = 2
      //  0.2 < purity_TAG < 0.6    --> quality_bin_TAG = 3
      //  0.6 < purity_TAG < 1      --> quality_bin_TAG = 4
      //        purity_PROBE < 0      --> quality_bin_PROBE = 0 (no track considered)
      //    0 < purity_PROBE < 0.02   --> quality_bin_PROBE = 1 (bad purity)
      // 0.02 < purity_PROBE < 0.2    --> quality_bin_PROBE = 2
      //  0.2 < purity_PROBE < 0.6    --> quality_bin_PROBE = 3
      //  0.6 < purity_PROBE < 1      --> quality_bin_PROBE = 4
      quality = QualityBin(purity_TAG, 0) + 5 * QualityBin(purity_PROBE, 1);
      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_QualityvsDE_TAG_PROBE"), cluEnergy[0] - pg, quality, 1.);

      if(fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED))fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_QualityvsDE_TAG_PROBE_%s", processSelected.Data()), cluEnergy[0] - pg, quality, 1.);

      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_QualityvsSlope_TAG"), m_best_TAG, quality, 1.);
      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_QualityvsInter_TAG"), c_best_TAG, quality, 1.);
      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_QualityvsSlope_PROBE"), m_best_PROBE, quality, 1.);
      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_QualityvsInter_PROBE"), c_best_PROBE, quality, 1.);
      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_QualityvsRECal_TAG_PROBE"), cluPosRel[0].Perp(), quality, 1.);
      
      if (QualityBin(purity_TAG, 0) == 4)
        fhSvcVal->FillHisto2List("TagAndProbe", "ECal_TP_DEVsE_BESTQTAG", fGeneralInfo->GetBeamEnergy() - pg, cluEnergy[0] - pg, 1.); // da mettere quello in phi
      int qualitynew = 0;
      if (quality > 19)
      {
        qualitynew = 1;
        fhSvcVal->FillHisto2List("TagAndProbe", "ECal_TP_DEVsE_BESTQPROBE", fGeneralInfo->GetBeamEnergy() - pg, cluEnergy[0] - pg, 1.); // da mettere quello in phi
      }
      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_QualityNewvsDE_PROBE"), cluEnergy[0] - pg, qualitynew, 1.);

      if(purity_PROBE < 0.8) continue; // skip events with a high purity probe track, to avoid biasing the efficiency measurement
      fhSvcVal->FillHisto2List("TagAndProbe", "ECal_TP_DEVsE_NOcut_tag", fGeneralInfo->GetBeamEnergy() - pg, cluEnergy[0] - pg, 1.); // da mettere quello in phi
      fhSvcVal->FillHisto2List("TagAndProbe_sliced", Form("ECal_TP_DEvsPhiExp_tag_slice_%i", iSlice), PhiExpProbe, cluEnergy[0] - pg, 1.);

      // casi

      // 0  1-in pos   1-non in pos  2 una in una no  2 entrambe in pos 2 nessuna in pos
      // 0  1           2              3              4                 5

      int Category = -1;
      if (nTracksInQuad == 0)
        Category = 0;
      else if (nTracksInQuad == 1 && nTracksInPosInQuad == 1)
        Category = 1;
      else if (nTracksInQuad == 1 && nTracksInPosInQuad == 0)
        Category = 2;
      else if (nTracksInQuad == 2 && nTracksInPosInQuad == 1)
        Category = 3;
      else if (nTracksInQuad == 2 && nTracksInPosInQuad == 2)
        Category = 4;
      else if (nTracksInQuad == 2 && nTracksInPosInQuad == 0)
        Category = 5;

      int CategoryOther = -1;
      if (nTracksInQuadOther == 0)
        CategoryOther = 0;
      else if (nTracksInQuadOther == 1 && nTracksInPosInQuadOther == 1)
        CategoryOther = 1;
      else if (nTracksInQuadOther == 1 && nTracksInPosInQuadOther == 0)
        CategoryOther = 2;
      else if (nTracksInQuadOther == 2 && nTracksInPosInQuadOther == 1)
        CategoryOther = 3;
      else if (nTracksInQuadOther == 2 && nTracksInPosInQuadOther == 2)
        CategoryOther = 4;
      else if (nTracksInQuadOther == 2 && nTracksInPosInQuadOther == 0)
        CategoryOther = 5;

      int globalCat = -1; // no track reconstructed
      if (nTracksInPosInQuad == 2 && nTracksInPosInQuadOther == 2)
        globalCat = 6; // both tag and probe have two track in Position
      else if (nTracksInPosInQuad > 0 && nTracksInPosInQuadOther > 0)
        globalCat = 2; // both tag and probe have a track in Position
      else if (nTracksInPosInQuad > 0 && nTracksInPosInQuadOther == 0)
        globalCat = 1; // only tag has the track in Position
      else if (nTracksInPosInQuad == 0 && nTracksInPosInQuadOther > 0)
        globalCat = 3; // only probe has the track in Position
      else if ((nTracksInPosInQuad == 0 && nTracksInPosInQuadOther == 0) && (nTracksInQuad > 0 || nTracksInQuadOther > 0))
        globalCat = 0; // both do not have tracks in position but there are recontructed tracks in either the tag or probe quadrant

      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_Cat1vs2"), Category, CategoryOther, 1.);
      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_CatvsDE"), cluEnergy[0] - pg, Category, 1.);
      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_CatvOthersDE"), cluEnergy[0] - pg, CategoryOther, 1.);
      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_globalCatvsDE"), cluEnergy[0] - pg, globalCat, 1.);

      for (auto it = begin(trackvect); it != end(trackvect); ++it)
      {
        MMBestTrack *track = *it;
        // std::cout<<"Track extrapolation TAG AND PROBE: clu quad: "<<quad<<" track quad: "<<track->quad<<" track view: "<<track->view<<" tracklet slope:  "<<track->slope<<"tracklet inter: "<<track->inter<<std::endl;

        if (track->quad != quad)
          continue;
        if (filledquad[quad])
          continue; // fill only one track per quadrant
        filledquad[quad] = true;
        TVector3 extPos = track->BestTrackExtrapolationAtZ(fGeneralInfo->GetCOG().Z());
        double dR = (extPos[1 - track->view] - cluPos[0][1 - track->view]);
        fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_VvsQ"), quad, cluPos[0][1 - track->view], 1);
        fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_VextvsQ"), quad, extPos[1 - track->view], 1);
        fhSvcVal->FillHisto2List("TagAndProbe_sliced", Form("ECal_TP_dRvsDE_tag_slice_%i", iSlice), cluEnergy[0] - pg, dR, 1.);
        fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_dRvsDE_tag"), cluEnergy[0] - pg, dR, 1.);

        for (auto it2 = begin(trackvect); it2 != end(trackvect); ++it2)
        {
          MMBestTrack *track2 = *it2;
          if (track2->quad != quadOther)
            continue;
          if (filledquadOther[quadOther])
            continue; // fill only one track per quadrant
          filledquadOther[quadOther] = true;
          TVector3 extPos2 = track2->BestTrackExtrapolationAtZ(fGeneralInfo->GetCOG().Z());
          double dR2 = (extPos2[1 - track2->view] - ExpPosProbe[1 - track2->view]);
          fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_dRvsDE_probe"), cluEnergy[0] - pg, dR2, 1.);
        }
      }
    }

    if (fabs(cluEnergy[0] - pg) > tpHigh)
      continue; // tag stringent energy selection
    // Enforce track - cluster association --> save
    // funzione che loopa dal cluster sulle tracce, chiedo Q poi guardo distanza traccia cluster X e Y (o che minimizza R?)

    if(fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED) && processSelected.Data()!="NoVtx") continue;

    for (int h2 = 0; h2 < fECal_clEvent->GetNElements(); h2++)
    {
      if (h1 == h2)
        continue;
      tempClu[1] = fECal_clEvent->Element(h2);
      cluPos[1].SetXYZ(
          tempClu[1]->GetPosition().X(),
          tempClu[1]->GetPosition().Y(), fGeneralInfo->GetCOG().Z());
      cluPosRel[1].SetXYZ(
          tempClu[1]->GetPosition().X() - fGeneralInfo->GetCOG().X(),
          tempClu[1]->GetPosition().Y() - fGeneralInfo->GetCOG().Y(),
          0.);
      cluTime[1] = tempClu[1]->GetTime();
      cluEnergy[1] = tempClu[1]->GetEnergy();
      double dt = cluTime[0] - cluTime[1];
      double dr = (cluPos[0] - cluPos[1]).Mag();

      if (fabs(dt) > fECalSel->GetMaxTimeDistance())
        continue; // time coincidence cut
      if (dr < fECalSel->GetMinGGDistance())
        continue; // avoid split clusters

      TVector3 cluDir2 = cluPos[1] - fGeneralInfo->GetTargetPos();
      cluDir2 *= (1.0 / cluDir2.Mag());
      // cos(theta_lab) between photon and boost axis
      double cosTheta2 = cluDir2.Dot(fGeneralInfo->GetBoost()) / fGeneralInfo->GetBeta();
      // Expected photon energy for e+e- -> gamma gamma
      double pg2 = 0.5 * fGeneralInfo->GetSqrts() / (fGeneralInfo->GetGam() * (1.0 - fGeneralInfo->GetBeta() * cosTheta2));
      // apply tag and probe selection and fill histos
      TVector3 cluMom1 = pg2 * cluDir2;
      labMomenta[1].SetVectM(cluMom1, 0.); // define a photon-like tlorentzVector
      labMomentaCM[1].SetVectM(labMomenta[1].Vect(), 0);
      labMomentaCM[1].Boost(-fGeneralInfo->GetBoost());
      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DTHEVsDPHIAbs_probe_nocut"),
                               fabs(labMomentaCM[0].Vect().Phi() - labMomentaCM[1].Vect().Phi()),
                               labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta(), 1.);
      fhSvcVal->FillHisto2List("TagAndProbe", "ECal_TP_DEVsE_NOcut_probe", fGeneralInfo->GetBeamEnergy() - pg, cluEnergy[1] - pg, 1.); // da mettere quello in phi
      if (fabs(labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta() - fMeanDTheta) > fSigmaDTheta * fSigmaCut)
        continue; // tag and probe selection on theta
      fhSvcVal->FillHisto2List("TagAndProbe_sliced", Form("ECal_TP_DTHEVsDPHIAbs_probe_slice_%i", iSlice), fabs(labMomentaCM[0].Vect().Phi() - labMomentaCM[1].Vect().Phi()), labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta(), 1.);

      if (fabs(fabs(labMomentaCM[0].Vect().Phi() - labMomentaCM[1].Vect().Phi())-fMeanDPhi) >  fSigmaDPhi * fSigmaCut)
        continue; // tag and probe selection on phi

      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_QualityvsDE_PROBED_%s", processSelected.Data()), cluEnergy[0] - pg, quality, 1.);
      fhSvcVal->FillHisto2List("TagAndProbe_sliced", Form("ECal_TP_DPhivsPhiExp_probe_slice_%i", iSlice), PhiExpProbe, fabs(labMomentaCM[0].Vect().Phi() - labMomentaCM[1].Vect().Phi()), 1.);

      fhSvcVal->FillHisto2List("TagAndProbe_sliced", Form("ECal_TP_DEvsE_probe_slice_%i", iSlice), fGeneralInfo->GetBeamEnergy() - pg, cluEnergy[1] - pg2, 1.);
      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DEvsE_probe"), fGeneralInfo->GetBeamEnergy() - pg, cluEnergy[1] - pg2, 1.);
    }
  }

  return 0;
}

Bool_t TagAndProbe::TagProbeEff_macro()
{
  fileIn = TFile::Open(fInputHistofile.Data());
  fileIn->Print();

  std::cout << "TagAndProbe::TagProbeEff_macro ERROR: cannot open " << fInputHistofile.Data() << std::endl;

  if (!fileIn || fileIn->IsZombie())
  {
    return false;
  }

  EofTag = dynamic_cast<TH2D *>(fileIn->Get("TagAndProbe/ECal_TP_DEVsE_NOcut_tag"));
  EofProbe = dynamic_cast<TH2D *>(fileIn->Get("TagAndProbe/ECal_TP_DEvsE_probe"));

  if (!EofTag || !EofProbe)
  {
    std::cerr << "TagAndProbe::TagProbeEff_macro ERROR: tag/probe histograms not found." << std::endl;
    // return false;
  }

  TagSlicevsPhi.clear();
  DPhiSlicevsPhi.clear();

  for (Int_t iSlice = 0; iSlice < fNSlicesE; ++iSlice)
  {
    TH2D *tagPhi = dynamic_cast<TH2D *>(fileIn->Get(Form("TagAndProbe_sliced/ECal_TP_DEvsPhiExp_tag_slice_%d", iSlice))); // FOLDER DEVE DIVENTARE SLICED
    TH2D *dPhiProbe = dynamic_cast<TH2D *>(fileIn->Get(Form("TagAndProbe_sliced/ECal_TP_DPhivsPhiExp_probe_slice_%d", iSlice)));
    TH2D *probeE = dynamic_cast<TH2D *>(fileIn->Get(Form("TagAndProbe_sliced/ECal_TP_DEvsE_probe_slice_%d", iSlice)));
    if (!tagPhi || !dPhiProbe || !probeE)
    {
      std::cerr << "TagAndProbe::TagProbeEff_macro ERROR: missing histogram for energy slice " << iSlice << std::endl;
      return false;
    }

    TagSlicevsPhi.push_back(tagPhi);
    DPhiSlicevsPhi.push_back(dPhiProbe);
    EProbeSlicevsE.push_back(probeE);
  }

  if (!FitTagProbeEff())
    return false;
  if (!FitTagProbeEffvsPhi())
    return false;

  return true;
}
// Bool_t TagAndProbe::TagProbeEffwithMM_macro()
// {
//   fileIn = TFile::Open(fInputHistofile.Data(), "READ");

//   if (!fileIn || fileIn->IsZombie()) {
//     std::cerr << "TagAndProbe::TagProbeEffwithMM_macro ERROR: cannot open " << fInputHistofile.Data() << std::endl;
//     return false;
//   }

//   EofTag = dynamic_cast<TH2D*>(fileIn->Get("TagAndProbe/ECal_TP_DEvsE_NOcut_tag"));
//   EofProbe = dynamic_cast<TH2D*>(fileIn->Get("TagAndProbe/ECal_TP_DEvsE_probe"));

//   if (!EofTag || !EofProbe) {
//     std::cerr << "TagAndProbe::TagProbeEffwithMM_macro ERROR: tag/probe histograms not found." << std::endl;
//     return false;
//   }

//   TagSlicevsPhi.clear();
//   DPhiSlicevsPhi.clear();

//   for (Int_t iSlice = 0; iSlice < fNSlicesE; ++iSlice) {
//     TH2D* tagPhi = dynamic_cast<TH2D*>(fileIn->Get(Form("TagAndProbe_sliced/ECal_TP_DEvsPhiExp_tag_slice_%d", iSlice))); // FOLDER DEVE DIVENTARE SLICED
//     TH2D* dPhiProbe = dynamic_cast<TH2D*>(fileIn->Get(Form("TagAndProbe_sliced/ECal_TP_DPhivsPhiExp_probe_slice_%d", iSlice)));

//     if (!tagPhi || !dPhiProbe) {
//       std::cerr << "TagAndProbe::TagProbeEffwithMM_macro ERROR: missing phi histogram for energy slice " << iSlice << std::endl;
//       return false;
//     }

//     TagSlicevsPhi.push_back(tagPhi);
//     DPhiSlicevsPhi.push_back(dPhiProbe);
//   }

//   if (!FitTagProbeEff()) return false;
//   if (!FitTagProbeEffvsPhi()) return false;

//   return true;
// }
Bool_t TagAndProbe::FitTagProbeEff()
{
  TObjArray *tx = fInputHistofile.Tokenize("/");
  TString InputHistofileName = ((TObjString *)(tx->At(tx->GetLast())))->String();

  if (!EofTag || !EofProbe)
  {
    std::cerr << "TagAndProbe::FitTagProbeEff ERROR: input histograms are not initialized." << std::endl;
    return false;
  }

  const Double_t eMin = fGeneralInfo->GetEnergyMin();

  TString dataType = fGeneralInfo->isMC() ? "MC" : "DATA";
  TString outputName = Form("%sFitsliceOut_%s_%s", fcfgPath.Data(), dataType.Data(), InputHistofileName.Data());

  TFile *outputFile = TFile::Open(outputName.Data(), "RECREATE");

  if (!outputFile || outputFile->IsZombie())
  {
    std::cerr << "TagAndProbe::FitTagProbeEff ERROR: cannot create " << outputName << std::endl;
    return false;
  }

  std::cout << "TagAndProbe::FitTagProbeEff: creating output file " << outputName << std::endl;

  TGraphErrors *efficiencyGraph = new TGraphErrors();
  efficiencyGraph->SetName("FitEffGraphE");
  efficiencyGraph->SetTitle("Tag-and-Probe efficiency;E_{probe}^{exp} [MeV];Efficiency");

  Double_t sumW = 0.;
  Double_t sumWE = 0.;

  for (Int_t iSlice = 0; iSlice < fNSlicesE; ++iSlice)
  {
    const Double_t eLow = eMin + iSlice * spacing;
    const Double_t eHigh = eLow + spacing;
    const Double_t energy = 0.5 * (eLow + eHigh);

    if (iSlice >= static_cast<Int_t>(TagSlicevsPhi.size()) || !TagSlicevsPhi[iSlice])
    {
      std::cerr << "TagAndProbe::FitTagProbeEff ERROR: missing tag histogram for slice " << iSlice << std::endl;
      continue;
    }

    TH1D *tag = TagSlicevsPhi[iSlice]->ProjectionY(Form("FitProYTag_%d", iSlice));

    if (!tag)
    {
      std::cerr << "TagAndProbe::FitTagProbeEff WARNING: could not project tag histogram for slice " << iSlice << std::endl;
      continue;
    }

    const Double_t tagIntegral = tag->Integral();

    if (tagIntegral <= 0.)
    {
      std::cout << "Slice " << iSlice << "  E = " << energy << "  Ntag = 0 -> skipping slice" << std::endl;
      delete tag;
      continue;
    }

    TF1 exponential(Form("exponential_%d", iSlice), "expo", -90., 60.);
    exponential.SetParameters(5., -1.e-2);

    tag->Fit(&exponential, "QNR");

    TF1 expGaus(Form("expGaus_%d", iSlice), Form("expo+gausn(2)/(0.5*(1.+TMath::Erf((%f-[3])/([4]*TMath::Sqrt(2.))))-0.5*(1.+TMath::Erf((%f-[3])/([4]*TMath::Sqrt(2.)))))", tpHigh, tpLow), -90., 60.);

    expGaus.SetParameter(0, exponential.GetParameter(0));
    expGaus.SetParameter(1, exponential.GetParameter(1));
    expGaus.SetParameter(2, tagIntegral);
    expGaus.SetParameter(3, 0.);
    expGaus.SetParameter(4, 5.);

    expGaus.SetParLimits(0, -10., 10.);
    expGaus.SetParLimits(1, -0.1, 0.1);
    expGaus.SetParLimits(2, 0., tagIntegral);
    expGaus.SetParLimits(3, -10., 10.);
    expGaus.SetParLimits(4, 0., 15.);

    TFitResultPtr fitResult = tag->Fit(&expGaus, "QRS");

    if (!fitResult.Get() || fitResult->Status() != 0)
    {
      std::cerr << "TagAndProbe::FitTagProbeEff WARNING: tag fit failed for slice " << iSlice << std::endl;
      delete tag;
      continue;
    }

    const Double_t nTag = expGaus.GetParameter(2);
    const Double_t nTagErr = expGaus.GetParError(2);
    Double_t nTagNoFitErr;
    const Double_t nTagNoFit = tag->IntegralAndError(tag->FindBin(tpLow), tag->FindBin(tpHigh), nTagNoFitErr);

    if (nTag <= 0.)
    {
      std::cerr << "TagAndProbe::FitTagProbeEff WARNING: fitted Ntag <= 0 for slice " << iSlice << std::endl;
      delete tag;
      continue;
    }

    if (iSlice >= static_cast<Int_t>(EProbeSlicevsE.size()) || !EProbeSlicevsE[iSlice])
    {
      std::cerr << "TagAndProbe::FitTagProbeEff ERROR: missing probe histogram for slice " << iSlice << std::endl;
      delete tag;
      continue;
    }

    TH1D *probe = EProbeSlicevsE[iSlice]->ProjectionY(Form("FitProYProbe_%d", iSlice));

    if (!probe)
    {
      std::cerr << "TagAndProbe::FitTagProbeEff WARNING: could not project probe histogram for slice " << iSlice << std::endl;
      delete tag;
      continue;
    }

    Double_t nProbeErr = 0.;
    const Double_t nProbe = probe->IntegralAndError(1, probe->GetNbinsX(), nProbeErr);

    if (nProbe <= 0.)
    {
      std::cout << "Slice " << iSlice << "  E range = [" << eLow << ", " << eHigh << ")  Nprobe = 0 -> skipping efficiency calculation" << std::endl;
      delete tag;
      delete probe;
      continue;
    }

    if (iSlice >= static_cast<Int_t>(DPhiSlicevsPhi.size()) || !DPhiSlicevsPhi[iSlice])
    {
      std::cerr << "TagAndProbe::FitTagProbeEff ERROR: missing DPhi histogram for slice " << iSlice << std::endl;
      delete tag;
      delete probe;
      continue;
    }

    TH1D *probeDPhi = DPhiSlicevsPhi[iSlice]->ProjectionY(Form("FitProYProbeDPhi_%d", iSlice));

    if (!probeDPhi)
    {
      std::cerr << "TagAndProbe::FitTagProbeEff WARNING: could not project DPhi histogram for slice " << iSlice << std::endl;
      delete tag;
      delete probe;
      continue;
    }

    Double_t nDPhiErr = 0.;
    const Double_t nDPhi = probeDPhi->IntegralAndError(1, probeDPhi->GetNbinsX(), nDPhiErr);

    std::cout << "Slice " << iSlice
              << "  E range = [" << eLow << ", " << eHigh << ")"
              << "  N(tag hist) = " << tagIntegral
              << "  N(probe hist) = " << nProbe
              << "  N(DPhi) = " << nDPhi
              << "  ratio = " << (nProbe > 0. ? nDPhi / nProbe : 0.)
              << std::endl;

    const Double_t efficiency = nProbe / nTag;
    const Double_t efficiencyError = efficiency * TMath::Sqrt(TMath::Power(nProbeErr / nProbe, 2) + TMath::Power(nTagErr / nTag, 2));

    std::cout << "Slice " << iSlice
              << "  E = " << energy
              << "  Ntag = " << nTag << " +/- " << nTagErr
              << "  NtagNoFit = " << nTagNoFit << " +/- " << nTagNoFitErr
              << "  Nprobe = " << nProbe << " +/- " << nProbeErr
              << "  Eff = " << efficiency << " +/- " << efficiencyError
              << std::endl;

    const Int_t point = efficiencyGraph->GetN();
    efficiencyGraph->SetPoint(point, energy, efficiency);
    efficiencyGraph->SetPointError(point, 0., efficiencyError);

    if (efficiencyError > 0.)
    {
      const Double_t weight = 1. / TMath::Power(efficiencyError, 2);
      sumW += weight;
      sumWE += efficiency * weight;
    }

    outputFile->cd();
    tag->Write();
    probe->Write();
    probeDPhi->Write();
    expGaus.Write();

    delete tag;
    delete probe;
    delete probeDPhi;
  }

  Double_t integratedEfficiency = 0.;
  Double_t integratedError = 0.;

  if (sumW > 0.)
  {
    integratedEfficiency = sumWE / sumW;
    integratedError = 1. / TMath::Sqrt(sumW);
  }

  std::cout << "Integrated efficiency = " << integratedEfficiency << " +/- " << integratedError << std::endl;

  outputFile->cd();
  efficiencyGraph->Write();

  outputFile->Close();

  delete outputFile;
  delete efficiencyGraph;
  delete tx;

  return true;
}

Bool_t TagAndProbe::FitTagProbeEffvsPhi()
{
  TObjArray *tx = fInputHistofile.Tokenize("/");
  TString InputHistofileName = ((TObjString *)(tx->At(tx->GetLast())))->String();
  if (TagSlicevsPhi.size() != static_cast<size_t>(fNSlicesE) || DPhiSlicevsPhi.size() != static_cast<size_t>(fNSlicesE))
  {
    std::cerr << "TagAndProbe::FitTagProbeEffvsPhi ERROR: missing phi histograms." << std::endl;
    return false;
  }

  const Int_t nPhiSlices = 6;
  const Double_t phiMin = -TMath::Pi();
  const Double_t phiWidth = 2. * TMath::Pi() / nPhiSlices;
  const Double_t eMin = fGeneralInfo->GetEnergyMin();

  TString dataType = fGeneralInfo->isMC() ? "MC" : "DATA";
  TString outputName = Form("%sPhi_FitsliceOut_%s_%s", fcfgPath.Data(), dataType.Data(), InputHistofileName.Data());
  std::cout << "TagAndProbe::FitTagProbeEffvsPhi: creating output file " << outputName << std::endl;

  TFile *outputFile = TFile::Open(outputName.Data(), "RECREATE");

  if (!outputFile || outputFile->IsZombie())
  {
    std::cerr << "TagAndProbe::FitTagProbeEffvsPhi ERROR: cannot create " << outputName << std::endl;
    return false;
  }

  TGraphErrors *gEffvsE[nPhiSlices];

  for (Int_t iPhi = 0; iPhi < nPhiSlices; ++iPhi)
  {
    gEffvsE[iPhi] = new TGraphErrors();
    gEffvsE[iPhi]->SetName(Form("gEffvsE_PhiFixed_%d", iPhi));
    gEffvsE[iPhi]->SetTitle(Form("Tag-and-Probe efficiency;E_{probe}^{exp} [MeV];Efficiency (#phi %.2f to %.2f rad)", phiMin + iPhi * phiWidth, phiMin + (iPhi + 1) * phiWidth));
  }

  for (Int_t iSlice = 0; iSlice < fNSlicesE; ++iSlice)
  {
    const Double_t energy = eMin + (iSlice + 0.5) * spacing;

    for (Int_t iPhi = 0; iPhi < nPhiSlices; ++iPhi)
    {
      const Double_t phiLow = phiMin + iPhi * phiWidth;
      const Double_t phiHigh = phiLow + phiWidth;

      TH2D *tagPhi = TagSlicevsPhi[iSlice];
      tagPhi->GetXaxis()->SetRangeUser(phiLow, phiHigh);

      TH1D *tag = dynamic_cast<TH1D *>(tagPhi->ProjectionY(Form("Tag_%d_%d", iSlice, iPhi)));

      if (!tag || tag->Integral() == 0.)
        continue;

      TF1 exponential(Form("exponential_%d_%d", iSlice, iPhi), "expo", -60., 60.);
      exponential.SetParameters(5., -1.e-2);
      tag->Fit(&exponential, "QNR");

      TF1 expGaus(Form("expGaus_%d_%d", iSlice, iPhi), Form("expo+gausn(2)/(0.5*(1.+TMath::Erf((%f-[3])/([4]*TMath::Sqrt(2.))))-0.5*(1.+TMath::Erf((%f-[3])/([4]*TMath::Sqrt(2.)))))", tpHigh, tpLow), -50., 50.);

      expGaus.SetParameter(0, exponential.GetParameter(0));
      expGaus.SetParameter(1, exponential.GetParameter(1));
      expGaus.SetParameter(2, tag->Integral());
      expGaus.SetParameter(3, 0.);
      expGaus.SetParameter(4, 5.);

      expGaus.SetParLimits(0, -10., 10.);
      expGaus.SetParLimits(1, -0.1, 0.1);
      expGaus.SetParLimits(2, 0., tag->Integral());
      expGaus.SetParLimits(3, -10., 10.);
      expGaus.SetParLimits(4, 0., 15.);

      TFitResultPtr fitResult = tag->Fit(&expGaus, "QRS");

      if (!fitResult.Get() || fitResult->Status() != 0)
        continue;

      const Double_t nTagFit = expGaus.GetParameter(2);
      const Double_t nTagErrFit = expGaus.GetParError(2);
      Double_t nTagErr;
      const Double_t nTag = tag->IntegralAndError(tag->FindBin(tpLow), tag->FindBin(tpHigh), nTagErr);
      if (nTag <= 0.)
        continue;

      TH2D *probePhi = DPhiSlicevsPhi[iSlice];
      probePhi->GetXaxis()->SetRangeUser(phiLow, phiHigh);

      TH1D *probe = dynamic_cast<TH1D *>(probePhi->ProjectionY(Form("ProbeDPhi_%d_%d", iSlice, iPhi)));

      if (!probe)
        continue;

      const Int_t binLow = probe->FindBin(fMeanDPhi - fSigmaCut * fSigmaDPhi);
      const Int_t binHigh = probe->FindBin(fMeanDPhi + fSigmaCut * fSigmaDPhi);

      Double_t nProbeErr = 0.;
      const Double_t nProbe = probe->IntegralAndError(binLow, binHigh, nProbeErr);

      const Double_t efficiency = nProbe / nTag;
      const Double_t efficiencyError = nProbe > 0. ? efficiency * TMath::Sqrt(TMath::Power(nProbeErr / nProbe, 2) + TMath::Power(nTagErr / nTag, 2)) : 0.;

      std::cout << "E slice " << iSlice << ", Phi slice " << iPhi << "  Ntag = " << nTag << " +/- " << nTagErr << " NTagFit = " << nTagFit << " +/- " << nTagErrFit << "  Nprobe = " << nProbe << " +/- " << nProbeErr << "  Eff = " << efficiency << " +/- " << efficiencyError << std::endl;

      const Int_t point = gEffvsE[iPhi]->GetN();
      gEffvsE[iPhi]->SetPoint(point, energy, efficiency);
      gEffvsE[iPhi]->SetPointError(point, 0., efficiencyError);

      outputFile->cd();
      tag->Write();
      probe->Write();
      expGaus.Write();

      delete tag;
      delete probe;
    }
  }

  outputFile->cd();

  for (Int_t iPhi = 0; iPhi < nPhiSlices; ++iPhi)
  {
    gEffvsE[iPhi]->Write();
    delete gEffvsE[iPhi];
  }

  outputFile->Close();
  delete outputFile;

  return true;
}

Bool_t TagAndProbe::Finalize()
{

  if (fHistoMode)
  {
    fSigmaCut = 3;
    TagProbeEff_macro();
    // TObjArray *txRun = InputHistofileName.Tokenize(".");
    // fNRun = ((TObjString *)(txRun->At(0)))->String() + "." + ((TObjString *)(txRun->At(1)))->String();
    // // fNRun = ((TObjString *)(txRun->At(0)))->String(); //OCCHIO
  }

  return true;
}
