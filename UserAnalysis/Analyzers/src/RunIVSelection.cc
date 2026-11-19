#include "RunIVSelection.hh"
#include "TRecoEvent.hh"
#include "TRecoVObject.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "HistoSvc.hh"
#include <iostream>

RunIVSelection *RunIVSelection::fInstance = 0;
RunIVSelection *RunIVSelection::GetInstance()
{
  if (fInstance == 0)
  {
    fInstance = new RunIVSelection();
  }
  return fInstance;
}

RunIVSelection::RunIVSelection()
{
  fEvent = NULL;
  fRecoEvent = NULL;
  fECal_hitEvent = NULL;
  fECal_clEvent = NULL;
  fVerbose = 0;
  fHistoMode = false;
  TString cfgFile = "config/UserAnalysis.conf";
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
  fApplyDataQuality = false;
  TString cfgvalue = TString(fCfgParser->GetSingleArg("GENERAL", "ApplyDataQuality"));
    if (cfgvalue.CompareTo("1") == 0)
      fApplyDataQuality = true;
    else
      fApplyDataQuality = false;

}

RunIVSelection::~RunIVSelection()
{
  delete fCfgParser;
}

Bool_t RunIVSelection::Init(PadmeAnalysisEvent *event, Bool_t fHistoModeVal, TString InputHistofileVal)
{
  fEvent = event;
  fRecoEvent = event->RecoEvent;
  fECal_hitEvent = event->ECalRecoEvent;
  fECal_clEvent = event->ECalRecoCl;
  fGeneralInfo = GeneralInfo::GetInstance();
  fhSvcVal = HistoSvc::GetInstance();
  fNPoTAnalysis = NPoTAnalysis::GetInstance();
  fMMFindBestTrack = MMFindBestTrack::GetInstance();
  fMCTruthECal = MCTruthECal::GetInstance();
  fMCTruthECalImproved = MCTruthImprovedECal::GetInstance();

  fHistoMode = fHistoModeVal;
  InputHistofile = InputHistofileVal;

  if(fApplyDataQuality){
    TString DQPath = TString(fCfgParser->GetSingleArg("GENERAL", "DQPath"));
    std::ifstream infile(Form("%s/tobecut_%d.txt", DQPath.Data(), fGeneralInfo->GetRunNumberFromDB())); //not sure is handling merged runs correctly, but for now it is ok since we are not merging runs from reco level
    
    TString name;
    int run, bin;
    long long tStart, tStop;
    double sigma;
    std::cout << "******** DATA QUALITY APPLIED: Reading bad time intervals from file: " << Form("%s/tobecut_%d.txt", DQPath.Data(), fGeneralInfo->GetRunNumberFromDB()) << std::endl;
    while (infile >> run >>name >> bin >> tStart >> tStop >> sigma) {
        //std::cout<<run<<bin<<tStart<<tStop<<sigma<<std::endl;
        // If the file contains multiple runs
        if (run != fGeneralInfo->GetRunNumberFromDB()){
            std::cout << "ECalSel: Warning - Run number in bad intervals file (" << run << ") does not match current run number (" << fGeneralInfo->GetRunNumberFromDB() << "). Skipping this entry." << std::endl;
            continue;
        }
        fBadDQIntervals.push_back({tStart, tStop});
    }
  }
  InitHistos();
  return true;
}

Bool_t RunIVSelection::InitHistos()
{
  fhSvcVal->CreateList("RunIVSelection");

  ftree = fhSvcVal->BookNtupleList("RunIVSelection","ftree");
  selev = new SelectedEvent();
  ftree->Branch("Ebeam", &selev->Ebeam, "Ebeam/D");
  ftree->Branch("gamma", &selev->gamma, "gamma/D");
  ftree->Branch("ERange", &selev->ERange, "ERange[2]/D");
  ftree->Branch("RRange", &selev->RRange, "RRange[2]/D");
  ftree->Branch("energy", &selev->energy, "energy[2]/D");
  ftree->Branch("thetaCMinfo", &selev->thetaCMinfo, "thetaCMinfo[2]/D");

  ftree->Branch("time", &selev->time, "time[2]/D");
  ftree->Branch("phi", &selev->phi, "phi[2]/D");
  ftree->Branch("thetaCM", &selev->thetaCM, "thetaCM[2]/D");
  ftree->Branch("thetaLab", &selev->thetaLab, "thetaLab[2]/D");
  ftree->Branch("seedenergy", &selev->seedenergy, "seedenergy[2]/D");
  ftree->Branch("seedposition_0","TVector2", &selev->seedposition[0]);
  ftree->Branch("seedposition_1","TVector2", &selev->seedposition[1]);
  ftree->Branch("cluster_Nhits", &selev->cluster_Nhits, "cluster_Nhits[2]/I");

  ftree->Branch("cog", "TVector2", &selev->cog);
  ftree->Branch("xyclu_0","TVector2", &selev->xyclu[0]);
  ftree->Branch("xyclu_1", "TVector2", &selev->xyclu[1]);
  ftree->Branch("labP_0", "TLorentzVector", &selev->labP[0]);
  ftree->Branch("labP_1", "TLorentzVector", &selev->labP[1]);
  
  ftree->Branch("cmP_0", "TLorentzVector", &selev->cmP[0]);
  ftree->Branch("cmP_1", "TLorentzVector", &selev->cmP[1]);

  ftree->Branch("trueLabP_0", "TLorentzVector", &selev->trueLabP[0]);
  ftree->Branch("trueLabP_1", "TLorentzVector", &selev->trueLabP[1]);
  ftree->Branch("trueCMP_0", "TLorentzVector", &selev->trueCMP[0]);
  ftree->Branch("trueCMP_1", "TLorentzVector", &selev->trueCMP[1]);

  ftree->Branch("truePos_0","TVector2", &selev->truePos[0]);
  ftree->Branch("truePos_1","TVector2", &selev->truePos[1]);

  ftree->Branch("processID", &selev->processID, "processID[2]/I");

  ftree->Branch("purity",&selev->purity,"purity[2][2]/D");
  ftree->Branch("purity_combined",&selev->purity_comb,"purity_combined[2]/D");
  ftree->Branch("trackSlope[2][2]", &selev->trackSlope, "trackSlope[2][2]/D");
  ftree->Branch("trackInter[2][2]", &selev->trackInter, "trackInter[2][2]/D");
  ftree->Branch("trackPosAtECal0VX","TVector3",&selev->trackPosAtECal[0][0]);//,"trackPosAtECal[2][2]/D");
  ftree->Branch("trackPosAtECal1VY","TVector3",&selev->trackPosAtECal[0][1]);//,"trackPosAtECal[2][2]/D");
  ftree->Branch("trackPosAtECal0VX","TVector3",&selev->trackPosAtECal[1][0]);//,"trackPosAtECal[2][2]/D");
  ftree->Branch("trackPosAtECal1VY","TVector3",&selev->trackPosAtECal[1][1]);//,"trackPosAtECal[2][2]/D");
  ftree->Branch("trackPosAtTarget0VX","TVector3",&selev->trackPosAtTarget[0][0]);//,"trackPosAtTarget[2][2]/D");
  ftree->Branch("trackPosAtTarget0VY","TVector3",&selev->trackPosAtTarget[0][1]);//,"trackPosAtTarget[2][2]/D");
  ftree->Branch("trackPosAtTarget1VX","TVector3",&selev->trackPosAtTarget[1][0]);//,"trackPosAtTarget[2][2]/D");
  ftree->Branch("trackPosAtTarget1VY","TVector3",&selev->trackPosAtTarget[1][1]);//,"trackPosAtTarget[2][2]/D");



  fhSvcVal->BookHistoList("RunIVSelection", "NPoTLGCorr", 1500, -500, 15500);
  fhSvcVal->BookHistoList("RunIVSelection", "NPoTLG_NO_Corr",1500, -500, 15500);
  fhSvcVal->BookHistoList("RunIVSelection", "EnSignalBhabha", 100, 0., 300.);
  fhSvcVal->BookHisto2List("RunIVSelection", "ECal_TC_DTHEVsDPHIAbs", 600, 0., 2*TMath::Pi(), 600, 0., 2*TMath::Pi());
  fhSvcVal->BookHisto2List("RunIVSelection", "ECal_TC_DTHEVsDPHIAbs_dt", 600, 0., 2*TMath::Pi(), 600, 0., 2*TMath::Pi());
  fhSvcVal->BookHisto2List("RunIVSelection", "ECal_TC_DTHEVsDPHIAbs_dt_Emin", 600, 0., 2*TMath::Pi(), 600, 0., 2*TMath::Pi());
  fhSvcVal->BookHisto2List("RunIVSelection", "ECal_TC_DTHEVsDPHIAbs_dt_Emin_Emax", 600, 0., 2*TMath::Pi(), 600, 0., 2*TMath::Pi());
  fhSvcVal->BookHisto2List("RunIVSelection", "ECal_TC_DTHEVsDPHIAbs_dt_Emin_Emax_DPhi", 600, 0., 2*TMath::Pi(), 600, 0., 2*TMath::Pi());



  // fhSvcVal->BookHistoList("RunIVSelection", "NClusters", 50, 0., 50.);

  return true;
}

Double_t RunIVSelection::NPoTLGCorr()
{
  Double_t NPotCorr = fNPoTAnalysis->GetNPoTLGCorr();
  Double_t NPotNoCorr = fNPoTAnalysis->GetNPoTLG();
  fhSvcVal->FillHistoList("RunIVSelection", "NPoTLGCorr", NPotCorr, 1.);
  fhSvcVal->FillHistoList("RunIVSelection", "NPoTLG_NO_Corr", NPotNoCorr, 1.);
  
  return NPotCorr;
}

Bool_t RunIVSelection::NSignalBhabha()
{

  for (Int_t iV = 0; iV < fEvent->MCTruthEvent->GetNVertices(); iV++)
  {
    TMCVertex *mcVtx = fEvent->MCTruthEvent->Vertex(iV);
    if (mcVtx->GetProcess() == "Bhabha")
    {
      TMCParticle *mcOPart = mcVtx->ParticleOut(0);
      Double_t pcleE = mcOPart->GetEnergy();
      
        fhSvcVal->FillHistoList("RunIVSelection", "EnSignalBhabha", pcleE, 1.);
    }
  }
  return true;
}
std::vector<std::pair<Int_t, Int_t>> RunIVSelection::GetCluCouples(){
  int NClu = fECal_clEvent->GetNElements();
  int r = 2;

  std::pair<Int_t, Int_t> idxs_couple;
    
  std::vector<std::pair<Int_t, Int_t>> couples;
  if(NClu<2) return couples;
  std::vector<bool> v(NClu);
  std::fill(v.end() - r, v.end(), true);

  do {
    idxs_couple.first=-1;
    idxs_couple.second=-1;
    for (int i = 0; i < NClu; ++i) {
      if (v[i]) {
	
	if(idxs_couple.first==-1) idxs_couple.first =i;
	else idxs_couple.second =i;
	
      }
    }
    couples.push_back(idxs_couple);
  } while (std::next_permutation(v.begin(), v.end()));
  
  return couples; 
}

Bool_t RunIVSelection::SelectTwoClusters(){
  std::vector<std::pair<Int_t, Int_t>> couples = GetCluCouples();
  TRecoVCluster *tempClu[2];

  for (std::vector<std::pair<Int_t, Int_t>>::iterator clupairs = couples.begin(); clupairs != couples.end(); ++clupairs) {
    tempClu[0] = fECal_clEvent->Element(clupairs->first);
    tempClu[1] = fECal_clEvent->Element(clupairs->second);

    if(tempClu[0]->GetEnergy() < tempClu[1]->GetEnergy()){
        tempClu[0] = fECal_clEvent->Element(clupairs->second);
        tempClu[1] = fECal_clEvent->Element(clupairs->first);
    }
    double cluTime[2];
    cluTime[0] = tempClu[0]->GetTime()-tempClu[1]->GetTime();
    cluTime[1] = tempClu[1]->GetTime()-tempClu[0]->GetTime();
    double cluEnergy[2];
    cluEnergy[0] = tempClu[0]->GetEnergy();
    cluEnergy[1] = tempClu[1]->GetEnergy();
   
    TVector3 cluPos[2];
    cluPos[0].SetXYZ(tempClu[0]->GetPosition().X(),tempClu[0]->GetPosition().Y(), fGeneralInfo->GetCOG().Z());
    cluPos[1].SetXYZ(tempClu[1]->GetPosition().X(),tempClu[1]->GetPosition().Y(), fGeneralInfo->GetCOG().Z());

    TVector3 cluPosRel[2];
    cluPosRel[0] = cluPos[0]-fGeneralInfo->GetCOG();
    cluPosRel[1] = cluPos[1]-fGeneralInfo->GetCOG();
    TLorentzVector labMomenta[2], labMomentaCM[2]; // momenta in the lab and CM frames
    //evaluate kinematics and momenta   
    for(int i=0; i<2; i++){
        TVector3 cluMom = cluPos[i] - fGeneralInfo->GetTargetPos();
        cluMom *= (cluEnergy[i] / cluMom.Mag());
        // laboratory and cm momenta
        labMomenta[i].SetVectM(cluMom, 0.); 
        labMomentaCM[i].SetVectM(labMomenta[i].Vect(), 0);
        labMomentaCM[i].Boost(-fGeneralInfo->GetBoost());
      }
    TVector2 cog((cluEnergy[0] * cluPos[0] + cluEnergy[1] * cluPos[1]).X() / (cluEnergy[0] + cluEnergy[1]),(cluEnergy[0] * cluPos[0] + cluEnergy[1] * cluPos[1]).Y() / (cluEnergy[0] + cluEnergy[1]));
    fhSvcVal->FillHisto2List("RunIVSelection", Form("ECal_TC_DTHEVsDPHIAbs"),fabs(labMomentaCM[0].Vect().Phi() - labMomentaCM[1].Vect().Phi()), labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta(), 1.);
    //for MC events looks for MCTruth information in the improved version
    TString processSelected[2];
    bool processFound[2] = {false, false};
    Int_t ecalvtxid[2] = {-1, -1};
    if(fisMC){
      for(int i=0; i<2; i++){
        int clusid = i==0 ? clupairs->first : clupairs->second;
        ecalvtxid[i] = fMCTruthECalImproved->GetECalVtxFromCluID(clusid);
        if(fMCTruthECalImproved->GetOriginalVtxFromECalVtxID(ecalvtxid[i])!= -1){
          processSelected[i] = fEvent->MCTruthEvent->Vertex(fMCTruthECalImproved->GetOriginalVtxFromECalVtxID(ecalvtxid[i]))->GetProcess();
          processFound[i] = true;
        }
      }
    }
    //Time coincidence cut
    if(fabs(cluTime[0]-cluTime[1]) > fMaxTimeDistance) continue;
    fhSvcVal->FillHisto2List("RunIVSelection", Form("ECal_TC_DTHEVsDPHIAbs_dt"),fabs(labMomentaCM[0].Vect().Phi() - labMomentaCM[1].Vect().Phi()), labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta(), 1.);

    //Minumum energy cut 
    if (cluEnergy[0] < fGeneralInfo->GetEnergyMin() * fSafeEnergyFactor ||  cluEnergy[1] < fGeneralInfo->GetEnergyMin() * fSafeEnergyFactor){
      continue;
      }
    fhSvcVal->FillHisto2List("RunIVSelection", Form("ECal_TC_DTHEVsDPHIAbs_dt_Emin"),fabs(labMomentaCM[0].Vect().Phi() - labMomentaCM[1].Vect().Phi()), labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta(), 1.);
    //Maximum energy cut
    if (cluEnergy[0] > fGeneralInfo->GetEnergyMax() / fSafeEnergyFactor ||  cluEnergy[1] > fGeneralInfo->GetEnergyMax() / fSafeEnergyFactor){
      continue;
      }
    fhSvcVal->FillHisto2List("RunIVSelection", Form("ECal_TC_DTHEVsDPHIAbs_dt_Emin_Emax"),fabs(labMomentaCM[0].Vect().Phi() - labMomentaCM[1].Vect().Phi()), labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta(), 1.);
    Double_t DeltaPhiAbs = fabs(labMomentaCM[0].Vect().Phi() - labMomentaCM[1].Vect().Phi());
    Double_t DeltaTheta = labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta();
    if (!(DeltaPhiAbs > fMeanDPhi - fSigmaCut * fSigmaDPhi && DeltaPhiAbs < fMeanDPhi + fSigmaCut * fSigmaDPhi)){
      continue;
    }
    fhSvcVal->FillHisto2List("RunIVSelection", Form("ECal_TC_DTHEVsDPHIAbs_dt_Emin_Emax_DPhi"),fabs(labMomentaCM[0].Vect().Phi() - labMomentaCM[1].Vect().Phi()), labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta(), 1.);
    *selev = SelectedEvent{};
    MMBestTrack3D *asstracks[2];
    asstracks[0] = fMMFindBestTrack->GetBestTrackFromCluPos(cluPos[0], fGeneralInfo->GetCOG().Z());
    asstracks[1] = fMMFindBestTrack->GetBestTrackFromCluPos(cluPos[1], fGeneralInfo->GetCOG().Z());
    selev->thetaCMinfo[0] = fMeanDTheta;
    selev->thetaCMinfo[1] = fSigmaDTheta;
    selev->Ebeam = fGeneralInfo->GetBeamEnergy();
    selev->gamma = fGeneralInfo->GetGam();
    selev->ERange[0] = fGeneralInfo->GetEnergyMin();
    selev->ERange[1] = fGeneralInfo->GetEnergyMax();
    selev->RRange[0] = fGeneralInfo->GetRadiusMin();
    selev->RRange[1] = fGeneralInfo->GetRadiusMax();
    selev->xyclu[0].Set(cluPos[0].X(), cluPos[0].Y());
    selev->xyclu[1].Set(cluPos[1].X(), cluPos[1].Y());

    selev->phi[0] = labMomentaCM[0].Vect().Phi();
    selev->phi[1] = labMomentaCM[1].Vect().Phi();
    selev->thetaCM[0] = labMomentaCM[0].Vect().Theta();
    selev->thetaCM[1] = labMomentaCM[1].Vect().Theta();
    selev->thetaLab[0] = labMomenta[0].Vect().Theta();
    selev->thetaLab[1] = labMomenta[1].Vect().Theta();
    selev->energy[0] = cluEnergy[0];
    selev->time[0] = cluTime[0];
    selev->energy[1] = cluEnergy[1];
    selev->time[1] = cluTime[1];
    selev->cog.Set(cog.X(), cog.Y());
    selev->seedposition[0].Set( fECal_hitEvent->Hit( tempClu[0]->GetSeed())->GetPosition().X(), fECal_hitEvent->Hit( tempClu[0]->GetSeed())->GetPosition().Y());
    selev->seedposition[1].Set( fECal_hitEvent->Hit( tempClu[1]->GetSeed())->GetPosition().X(), fECal_hitEvent->Hit( tempClu[1]->GetSeed())->GetPosition().Y());
    selev->seedenergy[0] = fECal_hitEvent->Hit( tempClu[0]->GetSeed())->GetEnergy();
    selev->seedenergy[1] = fECal_hitEvent->Hit( tempClu[1]->GetSeed())->GetEnergy();
    selev->cluster_Nhits[0]= tempClu[0]->GetNHitsInClus();
    selev->cluster_Nhits[1]= tempClu[1]->GetNHitsInClus();
    selev->labP[0] = labMomenta[0];  
    selev->labP[1] = labMomenta[1];  
    selev->cmP[0] = labMomentaCM[0];  
    selev->cmP[1] = labMomentaCM[1]; 
    selev->purity_comb[0] =  asstracks[0]->purity;
    selev->purity_comb[1] =  asstracks[1]->purity;
    for(int vw=0; vw<2; vw++) {
      selev->purity[0][vw] = asstracks[0]->tracks[vw] ? asstracks[0]->tracks[vw]->purity : -999;
      selev->purity[1][vw] = asstracks[1]->tracks[vw] ? asstracks[1]->tracks[vw]->purity : -999;
      
      selev->trackPosAtECal[0][vw] = asstracks[0]->tracks[vw] ? asstracks[0]->tracks[vw]->BestTrackExtrapolationAtZ(fGeneralInfo->GetCOG().Z()) : TVector3(0, 0, 0);
      selev->trackPosAtTarget[0][vw] = asstracks[0]->tracks[vw] ? asstracks[0]->tracks[vw]->BestTrackExtrapolationAtZ(fGeneralInfo->GetTargetPos().Z()) : TVector3(0, 0, 0);
      selev->trackPosAtECal[1][vw] = asstracks[1]->tracks[vw] ? asstracks[1]->tracks[vw]->BestTrackExtrapolationAtZ(fGeneralInfo->GetCOG().Z()) : TVector3(0, 0, 0);
      selev->trackPosAtTarget[1][vw] = asstracks[1]->tracks[vw] ? asstracks[1]->tracks[vw]->BestTrackExtrapolationAtZ(fGeneralInfo->GetTargetPos().Z()) : TVector3(0, 0, 0);
      selev->trackSlope[0][vw] = asstracks[0]->tracks[vw] ? asstracks[0]->tracks[vw]->slope : -999;
      selev->trackSlope[1][vw] = asstracks[1]->tracks[vw] ? asstracks[1]->tracks[vw]->slope : -999;
      selev->trackInter[0][vw] = asstracks[0]->tracks[vw] ? asstracks[0]->tracks[vw]->inter : -999;
      selev->trackInter[1][vw] = asstracks[1]->tracks[vw] ? asstracks[1]->tracks[vw]->inter : -999;
    }
    selev->processID[0]=0;
    selev->processID[1]= 0;
    //ADD MCTruth association 
    if(fisMC){  
      for(int i =0; i<2; i++){
        if(processFound[i]){
          if(processSelected[i] =="eBrem"){
            selev->processID[i] = 1;
            }else if(processSelected[i] =="eIoni"){ 
            selev->processID[i] = 2;
            }else if(processSelected[i]=="annihil"){
            selev->processID[i] = 3;
            }else if (processSelected[i]=="Bhabha"){
            selev->processID[i] = 4;  
            }else if (processSelected[i]=="Babayaga"){
            selev->processID[i] = 5;
            }else if(processSelected[i]=="BabayagaGG"){
            selev->processID[i] = 6;
            }
          TMCVertex *mcVtx = fEvent->MCTruthEvent->Vertex(ecalvtxid[i]);
          TMCParticle* pcleOut = mcVtx->ParticleOut(0); //for ecal type vertices the first particle is the one going to the ecal
          selev->truePos[i].Set(mcVtx->GetPosition().X(), mcVtx->GetPosition().Y());
          selev->trueLabP[i].SetVectM(pcleOut->GetMomentum(), 0.);
          selev->trueCMP[i].SetVectM(pcleOut->GetMomentum(), 0.);
          selev->trueCMP[i].Boost(-fGeneralInfo->GetBoost());
          //aggiungo la proiettata?
          }
      }
    }

    fhSvcVal->FillNtupleList("RunIVSelection","ftree");
    // GetBestTrackFromCluPos returns a newly allocated MMBestTrack3D: we own it
    delete asstracks[0];
    delete asstracks[1];


  } //closes loop on cluster couples
  return true;
}

Bool_t RunIVSelection::Process()
{
  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();

  // if(NEvent%10000==0) cout<<"ECalSel NEvent "<<NEvent<<endl;// -->si resetta ogni circa 500 evt per il MC

  fSafeEnergyFactor = 0.6; // Safety factor used for the energy min and max cuts
  fSafeSpaceMargin = 31.5; // 1.5 cells in mm, safety margin used for the radius min and max cut
  fMaxTimeDistance = 5.; // ns, two-cluster time distance
  fMeanDPhi = 3.143; //rad
  fSigmaDPhi = 0.1105; //rad 
  fMeanDTheta = 3.143; //rad
  fSigmaDTheta = 0.1105; //rad
  fSigmaCut = 5.;
  long long t = (long long)fEvent->RecoEvent->GetEventTime().GetSec();

  bool reject = false;
  if(fApplyDataQuality){
    for (const auto &interval : fBadDQIntervals) {
        if (t >= interval.tStart && t < interval.tStop) {
            reject = true;
            break;
        }
    }

    if (reject)
        { std::cout << "ECalSel: Event rejected due to bad time interval. Event time: " << t << std::endl;
          return false;
        }
  }
  fisMC = fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED);
  if (fisMC)
    NSignalBhabha();

  if (fisMC || trigMask & (1 << 0))
  {
    NPoTLGCorr();
    SelectTwoClusters();
  }
  // per-event selection goes here
  return true;
}



Bool_t RunIVSelection::Finalize()
{
  return true;
}
