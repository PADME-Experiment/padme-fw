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

TagAndProbe* TagAndProbe::fInstance = 0;
TagAndProbe* TagAndProbe::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new TagAndProbe(); }
  return fInstance;
}

TagAndProbe::TagAndProbe()
{
  
  fRecoEvent   = NULL;
  fVerbose        = 0;
  TString cfgFile = "config/UserAnalysis.conf";
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
  fcfgPath = TString(fCfgParser->GetSingleArg("ECAL", "TPWithMMPath"));
  fUseMM = true;

}
TagAndProbe::~TagAndProbe()
{ 
  if (fCfgParser) delete fCfgParser;
}

Bool_t TagAndProbe::Init(PadmeAnalysisEvent* event, Bool_t HistoMode, TString InputHistofile){
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
  //fMMTrackDevel = MMTrackDevel::GetInstance();
  fMMFindBestTrack = MMFindBestTrack::GetInstance();

  fInputHistofile = InputHistofile;
  fHistoMode = HistoMode;

  fNSlicesE = 1 + (Int_t)(fGeneralInfo->GetEnergyMax() - fGeneralInfo->GetEnergyMin()) / spacing;
  
  
  //PRENDE VARIABILI SELEZIONE DA ECALSEL (TAGLI THETA - PHI)
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

  int  fNYBins = (fYMax - fYMin) / fYW;

  fhSvcVal->CreateList("TagAndProbe");
	fhSvcVal->BookHisto2List("TagAndProbe", "ECal_TP_DEVsE_NOcut_tag",400, 0, 400, 600,-300, 300); // da mettere quello in phi
	fhSvcVal->BookHisto2List("TagAndProbe", "ECal_TP_DEVsE_NOcut_probe",400, 0, 400, 600,-300, 300); // da mettere quello in phi
  fhSvcVal->BookHisto2List("TagAndProbe", "ECal_TP_DEvsE_probe",400, 0, 400, 600,-300, 300); // da mettere quello in phi
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_dRvsDE_tag"),600, -300, 300,600, -600, 600);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_dRvsdRold"),600, -600, 600,600, -600, 600);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_dRvsDE_probe"),600, -300, 300,600, -600, 600);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_CatvsDE"),600,-300, 300, 6, -0.5, 5.5);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_CatvOthersDE"),600,-300, 300, 6, -0.5, 5.5);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_VvsQ"),4, -0.5, 3.5, 600, -600, 600);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_VextvsQ"),4, -0.5, 3.5, 600, -600, 600);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DTHEVsDPHIAbs_probe_nocut"),600, 0, 2*TMath::Pi(), 600, 0, 2*TMath::Pi());
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_Cat1vs2"),6, -0.5, 5.5, 6, -0.5, 5.5);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_globalCatvsDE"), 600,-300, 300, 7, -1.5, 5.5);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsDE_lvl0"), 600, -300, 300,600, -600, 600);
  fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DRvsDE_lvl1"), 600, -300, 300,600, -600, 600);


  for(int iSlice = 0; iSlice < fNSlicesE; iSlice++) {
    fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DEvsPhiExp_tag_slice_%i", iSlice),600, -TMath::Pi(), TMath::Pi(), 600,-300, 300);
    fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DEvsE_probe_slice_%i", iSlice),400, 0, 400, 600,-300, 300);

    fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_DTHEVsDPHIAbs_probe_slice_%i", iSlice),600, 0, 2*TMath::Pi(), 600, 0, 2*TMath::Pi());
    fhSvcVal->BookHisto2List("TagAndProbe", Form("ECal_TP_dRvsDE_tag_slice_%i", iSlice),600, -300, 300,600, -600, 600);
  }


  return true;
}



Int_t TagAndProbe::TagAndProbeSelection(){
  // loop over events and fill the struct with the info of interest for the tag and probe selection
  
  // then fill the histos for the tag and probe efficiency evaluation
  TRecoVCluster *tempClu[2];
  TVector3 cluPos[2], cluPosRel[2];
  double cluTime[2];
  double cluEnergy[2];
  TLorentzVector labMomenta[2];
  TLorentzVector labMomentaCM[2];
  for(int h1=0; h1<fECal_clEvent->GetNElements(); h1++){
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
    //calcute expected energy for a two body product coming from the target center and hitting the cluster position
    // Direction of the observed cluster from the target
    TVector3 cluDir = cluPos[0] - fGeneralInfo->GetTargetPos();
    cluDir *= (1.0 / cluDir.Mag());
    // cos(theta_lab) between photon and boost axis
    double cosTheta = cluDir.Dot(fGeneralInfo->GetBoost()) / fGeneralInfo->GetBeta();
    // Expected photon energy for e+e- -> gamma gamma
    double pg = 0.5 * fGeneralInfo->GetSqrts()/ (fGeneralInfo->GetGam() * (1.0 - fGeneralInfo->GetBeta() * cosTheta));
    double probeExpEnergy = fGeneralInfo->GetBeamEnergy() - pg;
    //std::cout << "Expected photon energy TAGAND PROBE h1: " << h1<<" "<<pg << std::endl;
    // Photon 1 momentum
    TVector3 cluMom = pg * cluDir;
    // Initial-state momentum:
    // P_init = E_init * beta_vec = gamma * sqrt(s) * beta_vec
    TVector3 pInit = fGeneralInfo->GetGam() * fGeneralInfo->GetSqrts() * (fGeneralInfo->GetBoost()*(1/fGeneralInfo->GetBeta()));
    // Photon 2 momentum from momentum conservation
    TVector3 otherCluMom = pInit - cluMom;
    labMomenta[0].SetVectM(cluMom, 0.); // define a photon-like tlorentzVector
    labMomentaCM[0].SetVectM(labMomenta[0].Vect(), 0);
    labMomentaCM[0].Boost(-fGeneralInfo->GetBoost());

    double Xexph2 = fGeneralInfo->GetTargetPos().X() + (otherCluMom.X() / otherCluMom.Z()) * (fGeneralInfo->GetCOG().Z() - fGeneralInfo->GetTargetPos().Z());
    double Yexph2 = fGeneralInfo->GetTargetPos().Y() + (otherCluMom.Y() / otherCluMom.Z()) * (fGeneralInfo->GetCOG().Z() - fGeneralInfo->GetTargetPos().Z());
    TVector3 ExpPosProbe(Xexph2, Yexph2, fGeneralInfo->GetCOG().Z());
    double PhiExpProbe = TMath::ATan2(Yexph2, Xexph2);

    //Selection cuts for the tag cluster (h1) --> Radious, Minimum amount of hits
    if (cluPosRel[0].Perp() < fGeneralInfo->GetRadiusMin())
      continue; // cluster should be within the radius range of the 2gamma cluster pair
    if (cluPosRel[0].Perp() > fGeneralInfo->GetRadiusMax())
      continue; // cluster should be within the radius range of the 2gamma cluster pair
    if (tempClu[0]->GetNHitsInClus() < 3)
      continue;
    
    if(fGeneralInfo->GetBeamEnergy()-pg < fGeneralInfo->GetEnergyMin() || fGeneralInfo->GetBeamEnergy()-pg > fGeneralInfo->GetEnergyMax())
      continue; // energy of the probe cluster should be within the energy range of the 2gamma cluster pair
    

    Int_t iSlice = (Int_t)(fGeneralInfo->GetBeamEnergy() - pg - fGeneralInfo->GetEnergyMin()) / spacing;

    
      //std::cout<<"Eprobe: "<<fGeneralInfo->GetBeamEnergy()-pg<<" pg "<<" Edown: "<<Edown<<" Eup: "<<Eup<<" iSlice: "<<iSlice<<" lowerbound:"<<Edown + (spacing * (iSlice))<<" upperbound:"<<Edown + (spacing * (iSlice + 1))<<std::endl;
        //fhSvcVal->FillHisto2List("ECalSel", Form("ECal_TP_DEVsE_noSel"), pg, cluEnergy[0] - pg, 1.);
    fhSvcVal->FillHisto2List("TagAndProbe", "ECal_TP_DEVsE_NOcut_tag", fGeneralInfo->GetBeamEnergy() - pg, cluEnergy[0] - pg, 1.); // da mettere quello in phi
    fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DEvsPhiExp_tag_slice_%i", iSlice), PhiExpProbe, cluEnergy[0] - pg, 1.);
    int quad = -1;
    if (cluPos[0].X() < 0 && cluPos[0].Y() < 0) quad = 0;
    else if (cluPos[0].X() < 0 && cluPos[0].Y() > 0) quad = 1;
    else if (cluPos[0].X() > 0 && cluPos[0].Y() > 0) quad = 2;
    else quad = 3;
    
    int quadOther = -1;

    if (Xexph2 < 0 && Yexph2 < 0) quadOther = 0;
    else if (Xexph2 < 0 && Yexph2 > 0) quadOther = 1;
    else if (Xexph2 > 0 && Yexph2 > 0) quadOther = 2;
    else quadOther = 3;

    bool filledquad[4] = {false, false, false, false};
    bool filledquadOther[4] = {false, false, false, false};
    //casi

    // 0  1-in pos   1-non in pos  2 una in una no  2 entrambe in pos 2 nessuna in pos
    // 0  1           2              3              4                 5
    //return 0;
    if(fUseMM){
      vector<MMBestTrack*> trackvect =fMMFindBestTrack->GetVectorTracks();
      //std::cout<<"Number of tracks in the event: "<<trackvect.size()<<std::endl;

      int nTracksInQuad = 0;
      int nTracksInPosInQuad = 0;

      int nTracksInQuadOther = 0;
      int nTracksInPosInQuadOther = 0;

      Double_t dROld = -999;
      for (auto it = begin (trackvect); it != end (trackvect); ++it) {
        MMBestTrack* track = *it;
        //std::cout<<"Track extrapolation TAG AND PROBE: clu quad: "<<quad<<" track quad: "<<track->quad<<" track view: "<<track->view<<" tracklet slope:  "<<track->slope<<"tracklet inter: "<<track->inter<<std::endl;

        if(track->quad != quad) continue;
        nTracksInQuad++;
              
        TVector3 extPos = track->BestTrackExtrapolationAtZ(fGeneralInfo->GetCOG().Z());
        double dR = (extPos[1-track->view] - cluPos[0][1-track->view]);
        fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DRvsDE_lvl%d",track->level), cluEnergy[0] - pg, dR, 1.);

        if(nTracksInQuad==2){
          fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_dRvsdRold"), dROld, dR, 1.);
        }
        if(fabs(dR+2.41) < (20)) { //TO BE CHANGED !!!!!
          nTracksInPosInQuad++;
        }
        dROld= dR;
      }
      for (auto it2 = begin (trackvect); it2 != end (trackvect); ++it2) {
        MMBestTrack* track2 = *it2;
        if(track2->quad != quadOther) continue;
      
        nTracksInQuadOther++;
        TVector3 extPos2 = track2->BestTrackExtrapolationAtZ(fGeneralInfo->GetCOG().Z());
        double dR2 = (extPos2[1-track2->view] - ExpPosProbe[1-track2->view]);
        if(fabs(dR2+5) < (33)) { //TO BE CHANGED !!!!!
          nTracksInPosInQuadOther++;
        }
      }
    //casi

    // 0  1-in pos   1-non in pos  2 una in una no  2 entrambe in pos 2 nessuna in pos
    // 0  1           2              3              4                 5

      int Category =-1;
      if(nTracksInQuad == 0) Category = 0;
      else if(nTracksInQuad == 1 && nTracksInPosInQuad == 1) Category = 1;
      else if(nTracksInQuad == 1 && nTracksInPosInQuad == 0) Category = 2;
      else if(nTracksInQuad == 2 && nTracksInPosInQuad == 1) Category = 3;
      else if(nTracksInQuad == 2 && nTracksInPosInQuad == 2) Category = 4;
      else if(nTracksInQuad == 2 && nTracksInPosInQuad == 0) Category = 5;

      int CategoryOther =-1;
      if(nTracksInQuadOther == 0) CategoryOther = 0;
      else if(nTracksInQuadOther == 1 && nTracksInPosInQuadOther == 1) CategoryOther = 1;
      else if(nTracksInQuadOther == 1 && nTracksInPosInQuadOther == 0) CategoryOther = 2;
      else if(nTracksInQuadOther == 2 && nTracksInPosInQuadOther == 1) CategoryOther = 3;
      else if(nTracksInQuadOther == 2 && nTracksInPosInQuadOther == 2) CategoryOther = 4;
      else if(nTracksInQuadOther == 2 && nTracksInPosInQuadOther == 0) CategoryOther = 5;

      int globalCat=-1; //no track reconstructed
      if(nTracksInPosInQuad>0 && nTracksInPosInQuadOther>0) globalCat=2; // both tag and probe have a track in Position
      else if(nTracksInPosInQuad>0 && nTracksInPosInQuadOther==0) globalCat=1; //only tag has the track in Position
      else if(nTracksInPosInQuad==0 && nTracksInPosInQuadOther>0) globalCat=3; //only probe has the track in Position
      else if((nTracksInPosInQuad==0 && nTracksInPosInQuadOther==0)  && (nTracksInQuad>0 || nTracksInQuadOther>0)) globalCat=0; //both do not have tracks in position but there are recontructed tracks in either the tag or probe quadrant
      
      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_Cat1vs2"),Category, CategoryOther, 1.);
      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_CatvsDE"),cluEnergy[0] - pg, Category, 1.);
      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_CatvOthersDE"),cluEnergy[0] - pg, CategoryOther, 1.);
      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_globalCatvsDE"),cluEnergy[0] - pg, globalCat, 1.);


      for (auto it = begin (trackvect); it != end (trackvect); ++it) {
        MMBestTrack* track = *it;
        //std::cout<<"Track extrapolation TAG AND PROBE: clu quad: "<<quad<<" track quad: "<<track->quad<<" track view: "<<track->view<<" tracklet slope:  "<<track->slope<<"tracklet inter: "<<track->inter<<std::endl;

        if(track->quad != quad) continue;
        if(filledquad[quad]) continue; //fill only one track per quadrant
        filledquad[quad] = true;
        TVector3 extPos = track->BestTrackExtrapolationAtZ(fGeneralInfo->GetCOG().Z());
        double dR = (extPos[1-track->view] - cluPos[0][1-track->view]);
        fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_VvsQ"), quad, cluPos[0][1-track->view], 1);
        fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_VextvsQ"), quad, extPos[1-track->view], 1);
        fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_dRvsDE_tag_slice_%i", iSlice), cluEnergy[0] - pg, dR, 1.);
        fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_dRvsDE_tag"), cluEnergy[0] - pg, dR, 1.);

        for (auto it2 = begin (trackvect); it2 != end (trackvect); ++it2) {
          MMBestTrack* track2 = *it2;
          if(track2->quad != quadOther) continue;
          if(filledquadOther[quadOther]) continue; //fill only one track per quadrant
          filledquadOther[quadOther] = true;
          TVector3 extPos2 = track2->BestTrackExtrapolationAtZ(fGeneralInfo->GetCOG().Z());
          double dR2 = (extPos2[1-track2->view] - ExpPosProbe[1-track2->view]);
          fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_dRvsDE_probe"), cluEnergy[0] - pg, dR2, 1.);


        }
      }
    }

    if(fabs(cluEnergy[0]-pg)> tpHigh) continue; //tag stringent energy selection
    //Enforce track - cluster association --> save 
    //funzione che loopa dal cluster sulle tracce, chiedo Q poi guardo distanza traccia cluster X e Y (o che minimizza R?)

    for(int h2=0; h2<fECal_clEvent->GetNElements(); h2++){
      if(h1==h2) continue;
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


      if(fabs(dt) > fECalSel->GetMaxTimeDistance()) continue; // time coincidence cut
      if(dr < fECalSel->GetMinGGDistance()) continue; // avoid split clusters 

      TVector3 cluDir2 = cluPos[1] - fGeneralInfo->GetTargetPos();
      cluDir2 *= (1.0 / cluDir2.Mag());
      // cos(theta_lab) between photon and boost axis
      double cosTheta2 = cluDir2.Dot(fGeneralInfo->GetBoost()) / fGeneralInfo->GetBeta();
      // Expected photon energy for e+e- -> gamma gamma
      double pg2 = 0.5 * fGeneralInfo->GetSqrts()/ (fGeneralInfo->GetGam() * (1.0 - fGeneralInfo->GetBeta() * cosTheta2));
        // apply tag and probe selection and fill histos
      TVector3 cluMom1 = pg2 * cluDir2;
      labMomenta[1].SetVectM(cluMom1, 0.); // define a photon-like tlorentzVector
      labMomentaCM[1].SetVectM(labMomenta[1].Vect(), 0);
      labMomentaCM[1].Boost(-fGeneralInfo->GetBoost());
      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DTHEVsDPHIAbs_probe_nocut"),
                                 fabs(labMomentaCM[0].Vect().Phi() - labMomentaCM[1].Vect().Phi()),
                                 labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta(), 1.);
      fhSvcVal->FillHisto2List("TagAndProbe", "ECal_TP_DEVsE_NOcut_probe", fGeneralInfo->GetBeamEnergy() - pg, cluEnergy[1] - pg, 1.); // da mettere quello in phi
      if(fabs(labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta() - fMeanDTheta) > fSigmaDTheta * fSigmaCut) continue; // tag and probe selection on theta
      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DTHEVsDPHIAbs_probe_slice_%i", iSlice), fabs(labMomentaCM[0].Vect().Phi() - labMomentaCM[1].Vect().Phi()), labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta(), 1.);
      //fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DPhivsPhiExp_probe_slice_%i", iSlice), PhiExpProbe, fabs(labMomentaCM[0].Vect().Phi() - labMomentaCM[1].Vect().Phi()), 1.);
  
    
      if(fabs(labMomentaCM[0].Vect().Phi() - labMomentaCM[1].Vect().Phi()) > fMeanDPhi + fSigmaDPhi * fSigmaCut) continue; // tag and probe selection on phi
  
      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DEvsE_probe_slice_%i", iSlice), fGeneralInfo->GetBeamEnergy() - pg, cluEnergy[1] - pg2, 1.);
      fhSvcVal->FillHisto2List("TagAndProbe", Form("ECal_TP_DEvsE_probe"), fGeneralInfo->GetBeamEnergy() - pg, cluEnergy[1] - pg2, 1.);
    
    }
  }

  return 0;
} 

Bool_t TagAndProbe::Finalize(){
  
  if (fHistoMode)
  {
    TObjArray *tx = fInputHistofile.Tokenize("/");
    TString InputHistofileName = ((TObjString *)(tx->At(tx->GetLast())))->String();

    TObjArray *txRun = InputHistofileName.Tokenize(".");
    fNRun = ((TObjString *)(txRun->At(0)))->String() + "." + ((TObjString *)(txRun->At(1)))->String();
    // fNRun = ((TObjString *)(txRun->At(0)))->String(); //OCCHIO
  }

  return true;
}
