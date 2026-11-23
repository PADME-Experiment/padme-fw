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
  TString cfgvalue = TString(fCfgParser->GetSingleArg("GENERAL", "UseMM"));
  fUseMM = false;
  if (cfgvalue == "1")
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
  fMCTruthImprovedECal = MCTruthImprovedECal::GetInstance();
  fNPoTAnalysis = NPoTAnalysis::GetInstance();
  fECalSel = ECalSel::GetInstance();
  // fMMTrackDevel = MMTrackDevel::GetInstance();
  fMMFindBestTrack = MMFindBestTrack::GetInstance();
  fMeanDPhi = fECalSel->GetMeanDPhi();
  fSigmaDPhi = fECalSel->GetSigmaDPhi();
  fMeanDTheta = fECalSel->GetMeanDTheta();
  fSigmaDTheta = fECalSel->GetSigmaDTheta();
  fSigmaCut = fECalSel->GetSigmaCut();
  fInputHistofile = InputHistofile;
  fHistoMode = HistoMode;

  InitHistos();
  return true;
}

Bool_t TagAndProbe::Process()
{
  fisMC = fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED);
  TagAndProbeSelection();
  return true;
}

Bool_t TagAndProbe::InitHistos()
{

  fhSvcVal->CreateList("TagAndProbe");
  ftree = fhSvcVal->BookNtupleList("TagAndProbe", "ftree");
  tpEvent = new TagAndProbeEvent();
  ftree->Branch("RunID", &tpEvent->RunID, "RunID/I");
  ftree->Branch("isMC", &tpEvent->isMC, "isMC/I");
  ftree->Branch("EnergyRange[2]", &tpEvent->EnergyRange, "EnergyRange[2]/D");
  ftree->Branch("RadiusRange[2]", &tpEvent->RadiusRange, "RadiusRange[2]/D");
  ftree->Branch("cog", "TVector3", &tpEvent->cog);
  ftree->Branch("targ", "TVector3", &tpEvent->targ);
  ftree->Branch("HasProbe", &tpEvent->HasProbe, "HasProbe/I");
  ftree->Branch("cluEnergy", &tpEvent->cluEnergy, "cluEnergy[2]/D");
  ftree->Branch("Eexp", &tpEvent->Eexp, "Eexp[2]/D");
  ftree->Branch("cluTime", &tpEvent->cluTime, "cluTime[2]/D");
  ftree->Branch("Hit_ix", &tpEvent->Hit_ix, "Hit_ix[25]/I");
  ftree->Branch("Hit_iy", &tpEvent->Hit_iy, "Hit_iy[25]/I");
  ftree->Branch("Hit_E", &tpEvent->Hit_E, "Hit_E[25]/D");
  ftree->Branch("phiCM", &tpEvent->phiCM, "phiCM[2]/D");
  ftree->Branch("thetaCM", &tpEvent->thetaCM, "thetaCM[2]/D");
  ftree->Branch("Ebeam", &tpEvent->Ebeam, "Ebeam/D");
  ftree->Branch("cluProcess", &tpEvent->cluProcess, "cluProcess[2]/I");
  ftree->Branch("cluPosTag", "TVector3", &tpEvent->cluPos[0]);
  ftree->Branch("cluPosProbe", "TVector3", &tpEvent->cluPos[1]);
  ftree->Branch("cluPosExp", "TVector3", &tpEvent->cluPosExp);
  ftree->Branch("truePosTag", "TVector3", &tpEvent->truePos[0]);
  ftree->Branch("truePosProbe", "TVector3", &tpEvent->truePos[1]);
  ftree->Branch("labPTag", "TLorentzVector", &tpEvent->labP[0]);
  ftree->Branch("labPProbe", "TLorentzVector", &tpEvent->labP[1]);
  ftree->Branch("cmPTag", "TLorentzVector", &tpEvent->cmP[0]);
  ftree->Branch("cmPProbe", "TLorentzVector", &tpEvent->cmP[1]);
  ftree->Branch("trueLabPTag", "TLorentzVector", &tpEvent->trueLabP[0]);
  ftree->Branch("trueCMPTag", "TLorentzVector", &tpEvent->trueCMP[0]);
  ftree->Branch("trueLabPProbe", "TLorentzVector", &tpEvent->trueLabP[1]);
  ftree->Branch("trueCMPProbe", "TLorentzVector", &tpEvent->trueCMP[1]);
  ftree->Branch("purity", &tpEvent->purity, "purity[2][2]/D");
  ftree->Branch("purity_combined", &tpEvent->purity_combined, "purity_combined[2]/D");
  ftree->Branch("trackSlope[2][2]", &tpEvent->trackSlope, "trackSlope[2][2]/D");
  ftree->Branch("trackInter[2][2]", &tpEvent->trackInter, "trackInter[2][2]/D");
  ftree->Branch("trackPosAtECalTagVX", "TVector3", &tpEvent->trackPosAtECal[0][0]);       //,"trackPosAtECal[2][2]/D");
  ftree->Branch("trackPosAtECalTagVY", "TVector3", &tpEvent->trackPosAtECal[0][1]);       //,"trackPosAtECal[2][2]/D");
  ftree->Branch("trackPosAtECalProbeVX", "TVector3", &tpEvent->trackPosAtECal[1][0]);     //,"trackPosAtECal[2][2]/D");
  ftree->Branch("trackPosAtECalProbeVY", "TVector3", &tpEvent->trackPosAtECal[1][1]);     //,"trackPosAtECal[2][2]/D");
  ftree->Branch("trackPosAtTargetTagVX", "TVector3", &tpEvent->trackPosAtTarget[0][0]);   //,"trackPosAtTarget[2][2]/D");
  ftree->Branch("trackPosAtTargetTagVY", "TVector3", &tpEvent->trackPosAtTarget[0][1]);   //,"trackPosAtTarget[2][2]/D");
  ftree->Branch("trackPosAtTargetProbeVX", "TVector3", &tpEvent->trackPosAtTarget[1][0]); //,"trackPosAtTarget[2][2]/D");
  ftree->Branch("trackPosAtTargetProbeVY", "TVector3", &tpEvent->trackPosAtTarget[1][1]); //,"trackPosAtTarget[2][2]/D");

  fhSvcVal->BookHisto2List("TagAndProbe", "ECal_TP_DEVsE_NOcut_tag", 400, 0, 400, 600, -300, 300);   // da mettere quello in phi
  fhSvcVal->BookHisto2List("TagAndProbe", "ECal_TP_DEVsE_NOcut_probe", 400, 0, 400, 600, -300, 300); // da mettere quello in phi
  fhSvcVal->BookHisto2List("TagAndProbe", "ECal_TP_DEvsE_probe", 400, 0, 400, 600, -300, 300);       // da mettere quello in phi
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DTHEVsDPHIAbs_probe_nocut"), 600, 0, 2 * TMath::Pi(), 600, 0, 2 * TMath::Pi());
  fhSvcVal->BookHisto2List("TagAndProbe", "ECal_TP_DE_ele_vs_DE_pos_TAG", 600, -300, 300, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", "ECal_TP_DEexp_ele_vs_DEexp_pos_TAG", 600, -300, 300, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", "ECal_TP_DEexp_vs_E_pos_TAG", 400, 0, 400, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", "ECal_TP_DEexp_vs_E_ele_TAG", 400, 0, 400, 600, -300, 300);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_P_vs_R_ELE_TAG"), 400, -0.5, 399.5, 350, -0.5, 349.5);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_P_vs_R_POS_TAG"), 400, -0.5, 399.5, 350, -0.5, 349.5);

  return true;
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

    // MC Truth
    *tpEvent = TagAndProbeEvent{};
    tpEvent->cluProcess[0] = 0;
    TString processSelected = "";
    Int_t ecalvtxid_tag = -1;
    if (fisMC)
    {
      ecalvtxid_tag = fMCTruthImprovedECal->GetECalVtxFromCluID(h1);
      if (fMCTruthImprovedECal->GetOriginalVtxFromECalVtxID(ecalvtxid_tag) != -1)
      {
        processSelected = fEvent->MCTruthEvent->Vertex(fMCTruthImprovedECal->GetOriginalVtxFromECalVtxID(ecalvtxid_tag))->GetProcess();
        tpEvent->cluProcess[0] = fMCTruthImprovedECal->AssignProcessID(processSelected);
        TMCVertex *mcVtx = fEvent->MCTruthEvent->Vertex(ecalvtxid_tag);
        TMCParticle *pcleOut = mcVtx->ParticleOut(0); // for ecal type vertices the first particle is the one going to the ecal
        tpEvent->truePos[0].SetXYZ(mcVtx->GetPosition().X(), mcVtx->GetPosition().Y(), fGeneralInfo->GetCOG().Z());
        tpEvent->trueLabP[0].SetVectM(pcleOut->GetMomentum(), 0.);
        tpEvent->trueCMP[0].SetVectM(pcleOut->GetMomentum(), 0.);
        tpEvent->trueCMP[0].Boost(-fGeneralInfo->GetBoost());
      }
    }

    MMBestTrack3D *best_track_TAG = nullptr;
    MMBestTrack3D *best_track_PROBE = nullptr;
  
    best_track_TAG = fMMFindBestTrack->GetBestTrackFromCluPos(cluPos[0], fGeneralInfo->GetCOG().Z());
    best_track_PROBE = fMMFindBestTrack->GetBestTrackFromCluPos(ExpPosProbe, fGeneralInfo->GetCOG().Z());  
    
    for (int i = 0; i < tempClu[0]->GetNHitsInClus(); i++)
    {
      int hitel = tempClu[0]->GetHitVecInClus().at(i);
      TRecoVHit *hit = fECal_hitEvent->Hit(hitel);
      tpEvent->Hit_ix[i] = hit->GetChannelId() / 100;
      tpEvent->Hit_iy[i] = hit->GetChannelId() % 100;
      tpEvent->Hit_E[i] = hit->GetEnergy();
    }
    tpEvent->cluEnergy[0] = cluEnergy[0];
    tpEvent->cluPos[0] = cluPos[0];
    tpEvent->cluTime[0] = cluTime[0];
    tpEvent->cmP[0] = labMomentaCM[0];
    tpEvent->labP[0] = labMomenta[0];
    tpEvent->thetaCM[0] = labMomentaCM[0].Vect().Theta();
    tpEvent->phiCM[0] = labMomentaCM[0].Vect().Phi();
    tpEvent->RunID = fGeneralInfo->GetRunNumberFromDB();
    tpEvent->isMC = fGeneralInfo->isMC();
    tpEvent->Ebeam = fGeneralInfo->GetBeamEnergy();
    tpEvent->EnergyRange[0] = fGeneralInfo->GetEnergyMin();
    tpEvent->EnergyRange[1] = fGeneralInfo->GetEnergyMax();
    tpEvent->RadiusRange[0] = fGeneralInfo->GetRadiusMin();
    tpEvent->RadiusRange[1] = fGeneralInfo->GetRadiusMax();
    tpEvent->HasProbe = -1;
    tpEvent->cog = fGeneralInfo->GetCOG();
    tpEvent->targ = fGeneralInfo->GetTargetPos();
    tpEvent->Eexp[1] = probeExpEnergy;
    tpEvent->Eexp[0] = pg;
    tpEvent->cluPosExp = ExpPosProbe;
    tpEvent->purity_combined[0] = best_track_TAG ? best_track_TAG->purity : -999;
    tpEvent->purity_combined[1] = best_track_PROBE ? best_track_PROBE->purity : -999;
    for (int vw = 0; vw < 2; vw++)
    {
      tpEvent->purity[0][vw] = best_track_TAG->tracks[vw] ? best_track_TAG->tracks[vw]->purity : -999; // sbagliato va sistemato
      tpEvent->purity[1][vw] = best_track_PROBE->tracks[vw] ? best_track_PROBE->tracks[vw]->purity : -999;
      tpEvent->trackPosAtECal[0][vw] = best_track_TAG->tracks[vw] ? best_track_TAG->tracks[vw]->BestTrackExtrapolationAtZ(fGeneralInfo->GetCOG().Z()) : TVector3(0, 0, 0);
      tpEvent->trackPosAtTarget[0][vw] = best_track_TAG->tracks[vw] ? best_track_TAG->tracks[vw]->BestTrackExtrapolationAtZ(fGeneralInfo->GetTargetPos().Z()) : TVector3(0, 0, 0);
      tpEvent->trackPosAtECal[1][vw] = best_track_PROBE->tracks[vw] ? best_track_PROBE->tracks[vw]->BestTrackExtrapolationAtZ(fGeneralInfo->GetCOG().Z()) : TVector3(0, 0, 0);
      tpEvent->trackPosAtTarget[1][vw] = best_track_PROBE->tracks[vw] ? best_track_PROBE->tracks[vw]->BestTrackExtrapolationAtZ(fGeneralInfo->GetTargetPos().Z()) : TVector3(0, 0, 0);
      tpEvent->trackSlope[0][vw] = best_track_TAG->tracks[vw] ? best_track_TAG->tracks[vw]->slope : -999;
      tpEvent->trackSlope[1][vw] = best_track_PROBE->tracks[vw] ? best_track_PROBE->tracks[vw]->slope : -999;
      tpEvent->trackInter[0][vw] = best_track_TAG->tracks[vw] ? best_track_TAG->tracks[vw]->inter : -999;
      tpEvent->trackInter[1][vw] = best_track_PROBE->tracks[vw] ? best_track_PROBE->tracks[vw]->inter : -999;
    }

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

      if (fabs(fabs(labMomentaCM[0].Vect().Phi() - labMomentaCM[1].Vect().Phi()) - fMeanDPhi) > fSigmaDPhi * fSigmaCut)
        continue; // tag and probe selection on phi
      TString processSelectedProbe = "";
      Int_t ecalvtxid_probe = -1;
      tpEvent->cluProcess[1] = 0;
      if (fisMC)
      {
        ecalvtxid_probe = fMCTruthImprovedECal->GetECalVtxFromCluID(h2);
        if (fMCTruthImprovedECal->GetOriginalVtxFromECalVtxID(ecalvtxid_probe) != -1)
        {
          processSelectedProbe = fEvent->MCTruthEvent->Vertex(fMCTruthImprovedECal->GetOriginalVtxFromECalVtxID(ecalvtxid_probe))->GetProcess();
          tpEvent->cluProcess[1] = fMCTruthImprovedECal->AssignProcessID(processSelectedProbe);
          TMCVertex *mcVtx = fEvent->MCTruthEvent->Vertex(ecalvtxid_probe);
          TMCParticle *pcleOut = mcVtx->ParticleOut(0); // for ecal type vertices the first particle is the one going to the ecal
          tpEvent->truePos[1].SetXYZ(mcVtx->GetPosition().X(), mcVtx->GetPosition().Y(), fGeneralInfo->GetCOG().Z());
          tpEvent->trueLabP[1].SetVectM(pcleOut->GetMomentum(), 0.);
          tpEvent->trueCMP[1].SetVectM(pcleOut->GetMomentum(), 0.);
          tpEvent->trueCMP[1].Boost(-fGeneralInfo->GetBoost());
        }
      }
      tpEvent->HasProbe = 1;
      tpEvent->cluEnergy[1] = cluEnergy[1];
      tpEvent->cluPos[1] = cluPos[1];
      tpEvent->cluTime[1] = cluTime[1];
      tpEvent->cmP[1] = labMomentaCM[1];
      tpEvent->labP[1] = labMomenta[1];
      tpEvent->thetaCM[1] = labMomentaCM[1].Vect().Theta();
      tpEvent->phiCM[1] = labMomentaCM[1].Vect().Phi();
      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DEvsE_probe"), fGeneralInfo->GetBeamEnergy() - pg, cluEnergy[1] - pg2, 1.);
    }
    fhSvcVal->FillNtupleList("TagAndProbe", "ftree");

  }
  return 0;

}


Bool_t TagAndProbe::Finalize()
{
  return true;
}
