// Written by M. Raggi   20/07/2021 
// Last modified by M. Raggi 12/08/2021
#include "BhabhaAnalysis.hh"
#include "TMath.h"
#include "VetoEndPoint.hh"
#include "TLorentzVector.h"
#include "PadmeVRecoConfig.hh"

BhabhaAnalysis::BhabhaAnalysis(TString cfgFile, Int_t verbose)
{
  //  Neve=0;
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating BhabhaAnalysis\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  fHS = HistoSvc::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
  fConfig = new PadmeVRecoConfig(fCfgParser,"PadmeReconstructionConfiguration");

  fSwimBhabha = fConfig->GetParOrDefault("BhabhaAnalysis","SwimBhabha",1.);
  std::cout<<"fSwimBhabha "<<fSwimBhabha<<std::endl;
  fVetoEndPoint = VetoEndPoint::GetInstance();

}

BhabhaAnalysis::~BhabhaAnalysis(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t BhabhaAnalysis::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing BhabhaAnalysis\n");
  fEvent = event;
  InitHistos();
  return true;
}

Bool_t BhabhaAnalysis::InitHistos(){
  fHS->BookHistoList("TOP","TriggerMaskBhabha",130,0,130);
  
  // BhabhaAnalysis directory will contain all histograms related to this analysis
  fHS->CreateList("PVetoClusters");
  fHS->CreateList("EVetoClusters");
  fHS->CreateList("ClusterTimeList");
  fHS->CreateList("BhabhaList");
  fHS->CreateList("MCBhabha");
  fHS->CreateList("TimeCorrectionList");
  fHS->CreateList("BackgroundList");
  fHS->CreateList("NPoTAnalysis");
  fHS->CreateList("PVetoHits");
  fHS->CreateList("EVetoHits");

  if(fSwimBhabha) fHS->CreateList("MCSwimming");

  //No. PoT used in this analysis
  fHS->BookHistoList("NPoTAnalysis","hNPotBhabha",600,-1000.,59000.);
  
  //Number of clusters
  fHS->BookHistoList("PVetoClusters","hNPVetoCluster",100,0,100);
  fHS->BookHistoList("EVetoClusters","hNEVetoCluster",100,0,100);

  fHS->BookHistoList("PVetoClusters","hGoodClusNPVetoCluster",10,0,10);
  fHS->BookHistoList("EVetoClusters","hGoodClusNEVetoCluster",10,0,10);

  //Cluster time
  fHS->BookHistoList("PVetoClusters","htPVetoCluster",500,-250,250);
  fHS->BookHistoList("EVetoClusters","htEVetoCluster",500,-250,250);

  fHS->BookHistoList("PVetoClusters","hGoodChastPVetoCluster",500,-250,250);
  fHS->BookHistoList("EVetoClusters","hGoodChastEVetoCluster",500,-250,250);

  //Cluster time length
  //  fHS->BookHistoList("PVetoClusters","htlenPVetoCluster",500,-250,250);
  //  fHS->BookHistoList("EVetoClusters","htlenEVetoCluster",500,-250,250);

  //"Luce" = distance between last hit of one cluster (= most upstream channel + nhits) and first hit of next cluster (= most upstream channel)
  //The "ChID" of a cluster is set as the most upstream channel in the reco
  fHS->BookHisto2List("PVetoClusters","hLuceVsDeltaTClusPVeto",360,-180,180,1000,-500,500);
  fHS->BookHisto2List("EVetoClusters","hLuceVsDeltaTClusEVeto",360,-180,180,1000,-500,500);

  fHS->BookHisto2List("PVetoClusters","hVetoChasOver302to3HitsGoodChaLuceVsDeltaTClusPVeto",160,-80,80,1000,-500,500);
  fHS->BookHisto2List("EVetoClusters","hVetoChasOver302to3HitsGoodChaLuceVsDeltaTClusEVeto",160,-80,80,1000,-500,500);

  //Time difference
  //  fHS->BookHistoList("BhabhaList","hDeltatPVetoEVetoCluster10nsfabs",100,-10,10);
  //  fHS->BookHistoList("BhabhaList","h2PlusHitsDeltatPVetoEVetoCluster",10000,-500,500);
  //  fHS->BookHistoList("BhabhaList","h2to3HitsDeltatPVetoEVetoCluster",10000,-500,500);
  //  fHS->BookHistoList("BhabhaList","hEnergyCut2to3HitsDeltatPVetoEVetoCluster",10000,-500,500);

  //No. hits per cluster
  fHS->BookHistoList("PVetoClusters","hNHitsPVetoCluster",15,0,15);
  fHS->BookHistoList("EVetoClusters","hNHitsEVetoCluster",15,0,15);

  fHS->BookHistoList("PVetoClusters","hGoodChasNHitsPVetoCluster",15,0,15);
  fHS->BookHistoList("EVetoClusters","hGoodChasNHitsEVetoCluster",15,0,15);

  fHS->BookHisto2List("PVetoClusters","hPVetoChVsNHitsPVetoCluster",90,0,89,15,0,15);
  fHS->BookHisto2List("EVetoClusters","hEVetoChVsNHitsEVetoCluster",90,0,89,15,0,15);

  fHS->BookHistoList("PVetoClusters","h0029ChNHitsPVetoCluster",15,0,15);
  fHS->BookHistoList("EVetoClusters","h0029ChNHitsEVetoCluster",15,0,15);

  fHS->BookHistoList("PVetoClusters","h3070ChNHitsPVetoCluster",15,0,15);
  fHS->BookHistoList("EVetoClusters","h3070ChNHitsEVetoCluster",15,0,15);

  fHS->BookHistoList("PVetoClusters","h7084ChNHitsPVetoCluster",15,0,15);
  fHS->BookHistoList("EVetoClusters","h7084ChNHitsEVetoCluster",15,0,15);

  fHS->BookHistoList("PVetoClusters","h85UpChNHitsPVetoCluster",15,0,15);
  fHS->BookHistoList("EVetoClusters","h85UpChNHitsEVetoCluster",15,0,15);

  //Energy per hit in cluster
  fHS->BookHistoList("PVetoClusters","hEPerHitPVetoCluster",150,0,15);
  fHS->BookHistoList("EVetoClusters","hEPerHitEVetoCluster",150,0,15);

  fHS->BookHistoList("PVetoClusters","hGoodChasEPerHitPVetoCluster",150,0,15);
  fHS->BookHistoList("EVetoClusters","hGoodChasEPerHitEVetoCluster",150,0,15);

  //Cluster energy
  fHS->BookHistoList("PVetoClusters","hEnergyPVetoCluster",150,0,30);
  fHS->BookHistoList("EVetoClusters","hEnergyEVetoCluster",150,0,30);

  fHS->BookHistoList("PVetoClusters","hGoodChasEnergyPVetoCluster",150,0,30);
  fHS->BookHistoList("EVetoClusters","hGoodChasEnergyEVetoCluster",150,0,30);

  //Cluster channel
  fHS->BookHistoList("PVetoClusters","hChPVetoCluster",90,0,90);
  fHS->BookHistoList("EVetoClusters","hChEVetoCluster",96,0,96);
 
  //Cluster energy
  fHS->BookHistoList("PVetoClusters","hChToEnergyPositron",330,0,330);
  fHS->BookHistoList("EVetoClusters","hChToEnergyElectron",330,0,330);

  //Cluster time difference
  fHS->BookHistoList("ClusterTimeList","hClusTimeDiffPVeto",40,-5,5);
  fHS->BookHistoList("ClusterTimeList","hClusTimeDiffEVeto",40,-5,5);
  fHS->BookHistoList("ClusterTimeList","hGoodChas2to3HitsClusTimeDiffPVeto",40,-5,5);
  fHS->BookHistoList("ClusterTimeList","hGoodChas2to3HitsClusTimeDiffEVeto",40,-5,5);
  fHS->BookHisto2List("ClusterTimeList","hClusTimeDiffPVetoVsPVeto1stCh",90,0,90,40,-5,5);
  fHS->BookHisto2List("ClusterTimeList","hClusTimeDiffEVetoVsEVeto1stCh",96,0,96,40,-5,5);
  fHS->BookHisto2List("ClusterTimeList","hGoodChas2to3HitsClusTimeDiffPVetoVsPVeto1stCh",40,30,70,40,-5,5);
  fHS->BookHisto2List("ClusterTimeList","hGoodChas2to3HitsClusTimeDiffEVetoVsEVeto1stCh",40,30,70,40,-5,5);
  fHS->BookHisto2List("ClusterTimeList","hClusTimeDiffPVetoVsPVeto2ndCh",90,0,90,40,-5,5);
  fHS->BookHisto2List("ClusterTimeList","hClusTimeDiffEVetoVsEVeto2ndCh",96,0,96,40,-5,5);
  fHS->BookHisto2List("ClusterTimeList","hGoodChas2to3HitsClusTimeDiffPVetoVsPVeto2ndCh",40,30,70,40,-5,5);
  fHS->BookHisto2List("ClusterTimeList","hGoodChas2to3HitsClusTimeDiffEVetoVsEVeto2ndCh",40,30,70,40,-5,5);
  fHS->BookHisto2List("ClusterTimeList","hClusTimeDiffPVetoVsPVetoDeltaCh",180,-90,90,40,-5,5);
  fHS->BookHisto2List("ClusterTimeList","hClusTimeDiffEVetoVsEVetoDeltaCh",192,-96,96,40,-5,5);
  fHS->BookHisto2List("ClusterTimeList","hGoodChas2to3HitsClusTimeDiffPVetoVsPVetoDeltaCh",80,-40,40,40,-5,5);
  fHS->BookHisto2List("ClusterTimeList","hGoodChas2to3HitsClusTimeDiffEVetoVsEVetoDeltaCh",80,-40,40,40,-5,5);

  //Bhabha plots
  fHS->BookHistoList("BhabhaList","hGoodClusChaSum",81,60,141);
  fHS->BookHistoList("BhabhaList","h1nsInTimeGoodClusChaSum",81,60,141);
  fHS->BookHisto2List("BhabhaList","hChaSumVsDeltaTuncorrect",81,60,141,200,-10,10);
  fHS->BookHisto2List("BhabhaList","hChaSumVsDeltaTcorrect",81,60,141,200,-10,10);
  fHS->BookHisto2List("BhabhaList","h1nsChaSumVsDeltaTuncorrect",81,60,141,200,-10,10);
  fHS->BookHisto2List("BhabhaList","h1nsChaSumVsDeltaTcorrect",81,60,141,200,-10,10);

  //MC only Bhabha plots
  fHS->BookHisto2List("MCBhabha","hAllNPVetoClusterVsNEVetoCluster",96,0,95,96,0,95);
  fHS->BookHisto2List("MCBhabha","hBhabhaNPVetoClusterVsNEVetoCluster",96,0,95,96,0,95);
  fHS->BookHisto2List("MCBhabha","hAllBoxNPVetoClusterVsNEVetoCluster",96,0,95,96,0,95);
  fHS->BookHisto2List("MCBhabha","hBhabhaBoxNPVetoClusterVsNEVetoCluster",96,0,95,96,0,95);
  
  //MC Swimming plots
  if(fSwimBhabha){
    fHS->BookHistoList("MCSwimming","hPVetoSwumInsideVeto",4,0,4);
    fHS->BookHistoList("MCSwimming","hEVetoSwumInsideVeto",4,0,4);
    fHS->BookHisto2List("MCSwimming","hPVetoSwumInsideVetoVsPVetoClusters",4,0,4,7,0,7);
    fHS->BookHisto2List("MCSwimming","hEVetoSwumInsideVetoVsEVetoClusters",4,0,4,7,0,7);
  
    fHS->BookHistoList("MCSwimming","hPVetoSwumChPVetoRecoChaDiff",181,-89.5,89.5);
    fHS->BookHistoList("MCSwimming","hEVetoSwumChEVetoRecoChaDiff",181,-89.5,89.5);
    fHS->BookHistoList("MCSwimming","hChaSum96to98GoodChasPVetoSwumChPVetoRecoChaDiff",181,-89.5,89.5);
    fHS->BookHistoList("MCSwimming","hChaSum96to98GoodChasEVetoSwumChEVetoRecoChaDiff",181,-89.5,89.5);
    fHS->BookHistoList("MCSwimming","hPVetoSwumTimePVetoRecoTimeDiff",201,-99.5,99.5);
    fHS->BookHistoList("MCSwimming","hEVetoSwumTimeEVetoRecoTimeDiff",201,-99.5,99.5);
    fHS->BookHistoList("MCSwimming","hPVetoSwumTimeEVetoSwumTimeDiff",20,-5,5);
    fHS->BookHistoList("MCSwimming","hPVetoSwumTimeEVetoSwumTimeDiffCorrect",20,-5,5);

    fHS->BookHistoList("MCSwimming","hSwumChaSum",186,0,186);
    fHS->BookHistoList("MCSwimming","hGoodChasSwumChaSum",78,62,140);
    fHS->BookHisto2List("MCSwimming","hPVetoSwumChVsEVetoSwumCh",90,0,90,96,0,96);
    fHS->BookHisto2List("MCSwimming","hChaSumOver90GoodChasPVetoSwumChVsEVetoSwumCh",90,0,90,96,0,96);
    fHS->BookHistoList("MCSwimming","hChaSumOver90GoodChasPVetoSwumTimeEVetoSwumTimeDiffCorrect",20,-5.,5);
    fHS->BookHisto2List("MCSwimming","hChaSum96to98GoodChasPVetoSwumChVsEVetoSwumCh",90,0,90,96,0,96);
    fHS->BookHistoList("MCSwimming","hChaSum96to98GoodChasPVetoSwumTimeEVetoSwumTimeDiffCorrect",20,-5.,5);
    fHS->BookHisto2List("MCSwimming","hVetoSwumChaDiffVsVetoSwumTimeDiff",181,-90,90,20,-5,5);
  }

  //Time correction plots
  fHS->BookHistoList("TimeCorrectionList","hdeltaTuncorrectPVetoEVeto",600,-150,150);
  fHS->BookHistoList("TimeCorrectionList","hdeltaTcorrectPVetoEVeto",600,-150,150);
  fHS->BookHistoList("TimeCorrectionList","hGoodChasdeltaTuncorrectPVetoEVeto",600,-150,150);
  fHS->BookHistoList("TimeCorrectionList","hGoodChasdeltaTcorrectPVetoEVeto",600,-150,150);
  fHS->BookHistoList("TimeCorrectionList","hdeltaTtrajPVetoEVeto",600,-150,150);
  
  fHS->BookHisto2List("TimeCorrectionList","hdeltaChVsdeltaTuncorrect",181,-90,90,600,-150,150);
  fHS->BookHisto2List("TimeCorrectionList","hdeltaChVsdeltaTcorrect",181,-90,90,600,-150,150);
  fHS->BookHisto2List("TimeCorrectionList","hdeltaChGoodChasVsdeltaTuncorrect",101,-50,50,600,-150,150);
  fHS->BookHisto2List("TimeCorrectionList","hdeltaChGoodChasVsdeltaTcorrect",101,-50,50,600,-150,150);
  fHS->BookHisto2List("TimeCorrectionList","hdeltaChVsdeltaTtrajPVetoEVeto",101,-50,50,600,-150,150);
  fHS->BookHisto2List("TimeCorrectionList","hdeltaTuncorrectVsdeltaTcorrectPVetoEVeto",2000,-500,500,600,-150,150);
  fHS->BookHisto2List("TimeCorrectionList","hdeltaTuncorrectGoodChasVsdeltaTcorrectPVetoEVeto",2000,-500,500,600,-150,150);
  //  fHS->BookHisto2List("TimeCorrectionList","hdeltaTtrajVsdeltaTcorrectPVetoEVeto",2000,-500,500,600,-150,150);
  
  //Hit energy
  //With no cuts, the energy histograms are filled in T0s Analysis since there's already a loop over hits there.
  fHS->BookHistoList("EVetoHits","hGoodClusterEVetoHitEnergy",600,0,6);
  fHS->BookHistoList("PVetoHits","hGoodClusterPVetoHitEnergy",600,0,6);
  fHS->BookHistoList("EVetoHits","h10nsInTimeGoodClusterEVetoHitEnergy",600,0,6);
  fHS->BookHistoList("PVetoHits","h10nsInTimeGoodClusterPVetoHitEnergy",600,0,6);
  fHS->BookHistoList("EVetoHits","h1nsInTimeGoodClusterEVetoHitEnergy",600,0,6);
  fHS->BookHistoList("PVetoHits","h1nsInTimeGoodClusterPVetoHitEnergy",600,0,6);
  return true;
}

Bool_t BhabhaAnalysis::Process(){
  if (fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) {
    isMC=true;
    //    std::cout<<"Bhabha input data are simulated "<<std::endl;
  }

  //Cut on physics trigger Data Only
  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  fHS->FillHistoList("TOP","TriggerMaskBhabha",trigMask);

  if( !(trigMask & (1 << 0)) && isMC==false) return false;
  
  if(fEvent->TargetRecoBeam)  fNPoT = fEvent->TargetRecoBeam->getnPOT();
  TLorentzVector FourMomentum; //MCTruth: four momentum of e+ or e- resulting from Bhabha
  TVector3 momentum;           //MCTruth: momentum of e+ or e- resulting from Bhabha
  Double_t energy;             //MCTruth: energy of e+ or e- resulting from Bhabha
  Int_t charge;

  TVector3 VertexPos;
  Double_t VertexTime=-1000;

  //variables reconstructed from Swimming in VetoEndPoint
  std::vector<Int_t> PVetoSwimmingChannels;
  std::vector<Int_t> EVetoSwimmingChannels;

  PVetoSwimmingChannels.clear();
  EVetoSwimmingChannels.clear();  
  
  std::vector<Double_t> PVetoSwimmingTime;
  std::vector<Double_t> EVetoSwimmingTime;

  PVetoSwimmingTime.clear();
  EVetoSwimmingTime.clear();  
  
  //variables reconstructed from standard reco which pass selections
  std::vector<Int_t> PVetoRecoPassChannels;
  std::vector<Int_t> EVetoRecoPassChannels;
  
  PVetoRecoPassChannels.clear();
  EVetoRecoPassChannels.clear();

  std::vector<Double_t> PVetoRecoPassTime;
  std::vector<Double_t> EVetoRecoPassTime;

  PVetoRecoPassTime.clear();
  EVetoRecoPassTime.clear();

  int nBhabha=0;
  int nBhabhaPos=0;
  int nBhabhaEle=0;
  int nBhabhaPosInVeto=0;
  int nBhabhaEleInVeto=0;
  
  bool fixedE=0;
  
  //if using MC produced at single energy from target
  if(isMC&&fSwimBhabha&&fixedE){
    energy = 200;
    double theta = 0;//-20e-3;//rad
    double phi = 0;//20e-3;//rad
    double px = (energy*TMath::Tan(theta))/(TMath::Sqrt(1+TMath::Tan(theta)*TMath::Tan(theta)));
    double py = (energy*TMath::Tan(phi))/(TMath::Sqrt(1+TMath::Tan(phi)*TMath::Tan(phi)));
    double pz = TMath::Sqrt(energy*energy-px*px);
    momentum = TVector3(px,py,pz);
    std::cout<<"px "<<px<<" py "<<py<<" pz "<<pz<<std::endl;
    FourMomentum = TLorentzVector(momentum,energy);
    VertexPos = TVector3(0,0,-1028);//mm
    VertexTime = 0;
    charge = 1;
    fVetoEndPoint->ParticleSwim(FourMomentum,VertexPos,VertexTime,charge);
    PVetoSwimmingChannels.push_back(fVetoEndPoint->GetEndFinger());
    PVetoSwimmingTime.push_back(fVetoEndPoint->GetEndTime());
    if(fVetoEndPoint->GetEndFinger()>-100) nBhabhaPosInVeto++;
  }
  else if(isMC&&fSwimBhabha&&!fixedE){//&&fEvent->MCTruthEvent->GetEventNumber()<50){
    for(Int_t iV = 0; iV<fEvent->MCTruthEvent->GetNVertices(); iV++) {
      mcVtx = fEvent->MCTruthEvent->Vertex(iV);
      //if(iV==1) std::cout<<fEvent->MCTruthEvent->GetNVertices()<<" vertices, 0 and 1 are "<<fEvent->MCTruthEvent->Vertex(0)->GetProcess()<<" "<<fEvent->MCTruthEvent->Vertex(1)->GetProcess()<<std::endl;
      if(mcVtx->GetProcess() == "eIoni"||mcVtx->GetProcess() == "Bhabha"){
	VertexPos=mcVtx->GetPosition();
	VertexTime = mcVtx->GetTime();
	nBhabha++;
	NTotBhabha++;
	fHS->FillHistoList("VetoEndPointList","hNTotBhabha_NMatched_NRecoBhabha_NSwimBhabha",0);
	for(Int_t iOut = 0; iOut<mcVtx->GetNParticleOut(); iOut++) {
	  mcOutPart = mcVtx->ParticleOut(iOut);

	  //PDGCode: matter = +ve, antimatter = -ve
	  if(mcOutPart->GetPDGCode()==11)
	    charge = -1;
	  else if(mcOutPart->GetPDGCode()==-11)
	    charge = 1;
	  else{
	    if(mcVtx->GetProcess() == "eIoni"||mcVtx->GetProcess() == "Bhabha")
	      std::cout<<"weirdly, PDGCode = "<<mcOutPart->GetPDGCode()<<std::endl;
	  }
	  momentum = mcOutPart->GetMomentum();
	  energy = mcOutPart->GetEnergy();
	  FourMomentum = TLorentzVector(momentum,energy);
	  fVetoEndPoint->ParticleSwim(FourMomentum,VertexPos,VertexTime,charge);
	  if(charge==1){
	    nBhabhaPos++;
	    NTotBhabhaPos++;
	    PVetoSwimmingChannels.push_back(fVetoEndPoint->GetEndFinger());
	    PVetoSwimmingTime.push_back(fVetoEndPoint->GetEndTime());
	    if(fVetoEndPoint->GetEndFinger()>-100) nBhabhaPosInVeto++;
	  }
	  else if(charge==-1){
	    nBhabhaEle++;
	    NTotBhabhaEle++;
	    EVetoSwimmingChannels.push_back(fVetoEndPoint->GetEndFinger());
	    EVetoSwimmingTime.push_back(fVetoEndPoint->GetEndTime());
	    if(fVetoEndPoint->GetEndFinger()>-100) nBhabhaEleInVeto++;
	  }
	  NSwum++;
	}
	if(mcVtx->GetProcess()=="eIoni"){
	  for(Int_t iOut = 0; iOut<mcVtx->GetNParticleOut(); iOut++) {
	    mcOutPart = mcVtx->ParticleOut(iOut);
	    momentum = mcOutPart->GetMomentum();
	    energy = mcOutPart->GetEnergy();
	    FourMomentum = TLorentzVector(momentum,energy);
	  }
	}
      }//end if(mcVtx->GetProcess() == "eIoni"||mcVtx->GetProcess() == "Bhabha"){
    }
    fHS->FillHistoList("MCSwimming","hPVetoSwumInsideVeto",nBhabhaPosInVeto);
    fHS->FillHistoList("MCSwimming","hEVetoSwumInsideVeto",nBhabhaEleInVeto);
  } //end if MC

  //  if(fNPoT>10000) return 0;
  
  fHS->FillHistoList("NPoTAnalysis","hNPotBhabha",fNPoT);
  
  //Number of PVeto clusters
  Int_t NPVetoCluster = fEvent->PVetoRecoCl->GetNElements();
  fHS->FillHistoList("PVetoClusters","hNPVetoCluster",NPVetoCluster);

  //Number of EVeto clusters
  Int_t NEVetoCluster = fEvent->EVetoRecoCl->GetNElements();
  fHS->FillHistoList("EVetoClusters","hNEVetoCluster",NEVetoCluster);

  if(isMC&&fSwimBhabha){
    fHS->FillHistoList("MCSwimming","hPVetoSwumInsideVetoVsPVetoClusters",nBhabhaPosInVeto,NPVetoCluster);
    fHS->FillHistoList("MCSwimming","hEVetoSwumInsideVetoVsEVetoClusters",nBhabhaPosInVeto,NEVetoCluster);
  }

  //Variables for number of clusters pass initial "good" cluster selection
  Int_t NPVetoGoodClusters=0;
  Int_t NEVetoGoodClusters=0;
  
  //Containers for variables of "good" clusters
  //time
  std::vector<double> tPVetoGood;
  std::vector<double> tEVetoGood;
  
  //counters for clusters that pass channel selections
  int npassPVeto=0;
  int npassEVeto=0;
  
  //channel
  std::vector<int> chPVetoGood;
  std::vector<int> chEVetoGood;
  
  //no hits
  std::vector<int> NHitsPVetoGood;
  std::vector<int> NHitsEVetoGood;
  
  //energy
  std::vector<double> enPVetoGood;
  std::vector<double> enEVetoGood;

  //containers for variables of *hits* in good clusters
  std::vector<std::vector<double> >  chPVetoHitGoodClus;
  std::vector<std::vector<double> >  chEVetoHitGoodClus;

  std::vector<std::vector<double> >  tPVetoHitGoodClus;
  std::vector<std::vector<double> >  tEVetoHitGoodClus;

  std::vector<std::vector<double> >  enPVetoHitGoodClus;
  std::vector<std::vector<double> >  enEVetoHitGoodClus;

  //temporary variables for the loop
  double tempEClus;
  double tempT;
  int tempNHit;
  int tempCh;

  double tempEHit =0;
  double tempTHit =0;
  double tempChHit=0;
  double indexHit =0;

  //temporary vectors for the loop
  std::vector<double>  chtempPVetoHitGoodClus;
  std::vector<double>  chtempEVetoHitGoodClus;
  			 
  std::vector<double>  ttempPVetoHitGoodClus;	
  std::vector<double>  ttempEVetoHitGoodClus;	
  			 
  std::vector<double>  entempPVetoHitGoodClus;
  std::vector<double>  entempEVetoHitGoodClus;

  std::vector<Int_t>  indexPVetoHitGoodClus;
  std::vector<int>  indexEVetoHitGoodClus;

  //variables associated to the a second cluster in the loop for ii>0
  double temp2EClus;
  double temp2T;
  int temp2NHit;
  int temp2Ch;

  //Box selection criteria
  Bool_t TopBoundary;
  Bool_t BottomBoundary;
  Bool_t LeftBoundary;
  Bool_t RightBoundary;

  //"Luce" = distance between last hit of one cluster (= most upstream channel + nhits) and first hit of next cluster (= most upstream channel)
  //The "ChID" of a cluster is set as the most upstream channel in the reco
  Int_t LucePVeto=0;
  Int_t LuceEVeto=0;
  
  //All EVeto Clusters
  for(int ii = 0; ii<NEVetoCluster; ii++){
    tempNHit  = fEvent->EVetoRecoCl->Element(ii)->GetNHitsInClus();
    tempEClus = fEvent->EVetoRecoCl->Element(ii)->GetEnergy(); 
    tempT  =  fEvent->EVetoRecoCl->Element(ii)->GetTime();
    tempCh =  fEvent->EVetoRecoCl->Element(ii)->GetChannelId();
    
    fHS->FillHistoList("EVetoClusters","hNHitsEVetoCluster",tempNHit);
    fHS->FillHistoList("EVetoClusters","hEPerHitEVetoCluster",tempEClus/tempNHit);
    fHS->FillHistoList("EVetoClusters","htEVetoCluster",tempT);
    fHS->FillHistoList("EVetoClusters","hChEVetoCluster",tempCh);
    fHS->FillHistoList("EVetoClusters","hEnergyEVetoCluster",tempEClus);
    
    for(int jj=ii+1;jj<NEVetoCluster;jj++){
      temp2NHit  = fEvent->EVetoRecoCl->Element(jj)->GetNHitsInClus();
      temp2EClus = fEvent->EVetoRecoCl->Element(jj)->GetEnergy(); 
      temp2T  =  fEvent->EVetoRecoCl->Element(jj)->GetTime();
      temp2Ch =  fEvent->EVetoRecoCl->Element(jj)->GetChannelId();
      
      if(temp2Ch>tempCh) LuceEVeto = temp2Ch-(tempCh+tempNHit);
      else if(tempCh>temp2Ch) LuceEVeto = tempCh-(temp2Ch+temp2NHit);
      else LuceEVeto = 0;
      
      fHS->FillHisto2List("EVetoClusters","hLuceVsDeltaTClusEVeto",LuceEVeto,temp2T-tempT);
      fHS->FillHistoList("ClusterTimeList","hClusTimeDiffEVeto",temp2T-tempT);
      fHS->FillHisto2List("ClusterTimeList","hClusTimeDiffEVetoVsEVeto1stCh",tempCh,temp2T-tempT);
      fHS->FillHisto2List("ClusterTimeList","hClusTimeDiffEVetoVsEVeto2ndCh",temp2Ch,temp2T-tempT);
      fHS->FillHisto2List("ClusterTimeList","hClusTimeDiffEVetoVsEVetoDeltaCh",temp2Ch-tempCh,temp2T-tempT);
      
      //two good clusters
      if((tempCh>30&&tempCh<71&&(tempNHit==2||tempNHit==3))&&(temp2Ch>30&&temp2Ch<71&&(temp2NHit==2||temp2NHit==3))){
	fHS->FillHisto2List("EVetoClusters","hVetoChasOver302to3HitsGoodChaLuceVsDeltaTClusEVeto",LuceEVeto,temp2T-tempT);
	if(LuceEVeto>100)      std::cout<<"ii "<<ii<<" jj"<<jj<<" tempCh "<<tempCh<<" temp2Ch "<<temp2Ch<<" tempNHit "<<tempNHit<<" temp2NHit "<<temp2NHit<<" LuceEVeto "<<LuceEVeto<<std::endl;
	fHS->FillHistoList("ClusterTimeList","hGoodChas2to3HitsClusTimeDiffEVeto",temp2T-tempT);
	fHS->FillHisto2List("ClusterTimeList","hGoodChas2to3HitsClusTimeDiffEVetoVsEVeto1stCh",tempCh,temp2T-tempT);
	fHS->FillHisto2List("ClusterTimeList","hGoodChas2to3HitsClusTimeDiffEVetoVsEVeto2ndCh",temp2Ch,temp2T-tempT);
	fHS->FillHisto2List("ClusterTimeList","hGoodChas2to3HitsClusTimeDiffEVetoVsEVetoDeltaCh",tempCh-temp2Ch,temp2T-tempT);
      }
    }//end jj

    if(tempCh<20)                  fHS->FillHistoList("EVetoClusters","h0029ChNHitsEVetoCluster",tempNHit);
    else if(tempCh>19&&tempCh<71)  fHS->FillHistoList("EVetoClusters","h3070ChNHitsEVetoCluster",tempNHit);
    else if(tempCh>70&&tempCh<85)  fHS->FillHistoList("EVetoClusters","h7084ChNHitsEVetoCluster",tempNHit);
    else if(tempCh>84)             fHS->FillHistoList("EVetoClusters","h85UpChNHitsEVetoCluster",tempNHit);

    if(isMC&&fSwimBhabha){
      for(int swumev=0;swumev<EVetoSwimmingChannels.size();swumev++){
	if(EVetoSwimmingChannels[swumev]==-100) continue;
	fHS->FillHistoList("MCSwimming","hEVetoSwumChEVetoRecoChaDiff",EVetoSwimmingChannels[swumev]-tempCh);
	fHS->FillHistoList("MCSwimming","hEVetoSwumTimeEVetoRecoTimeDiff",EVetoSwimmingTime[swumev]-tempT);
	if(fabs(tempCh-EVetoSwimmingChannels[swumev])<=1&&fabs(tempT-EVetoSwimmingTime[swumev])<5) nswimandtrutheveto++;// std::cout<<"yay for eveto"<<std::endl;
      }
    }//end if isMC
    
    fHS->FillHisto2List("EVetoClusters","hEVetoChVsNHitsEVetoCluster",tempCh,tempNHit);

    //good cluster quality cuts, eveto:
    if(tempCh<30||tempCh>70) continue;
    //    if(tempNHit<2||tempNHit>3) continue; //8/2 turned off: not well reproduced in data/mc and makes very little difference to signal/background ratio
    //    if(!(tempEClus/tempNHit)>0.7) continue;
    
    fHS->FillHistoList("EVetoClusters","hGoodChasNHitsEVetoCluster",tempNHit);
    fHS->FillHistoList("EVetoClusters","hGoodChasEPerHitEVetoCluster",tempEClus/tempNHit);
    fHS->FillHistoList("EVetoClusters","hGoodChastEVetoCluster",tempT);
    fHS->FillHistoList("EVetoClusters","hGoodChasEnergyEVetoCluster",tempEClus);

    //Counter of EVeto clusters that pass
    NEVetoGoodClusters++;
    npassEVeto++;
    
    //fill vectors with parameters of good clusters
    NHitsEVetoGood.push_back(tempNHit);
    enEVetoGood.push_back(tempEClus);
    tEVetoGood.push_back(tempT);
    chEVetoGood.push_back(tempCh);
      
    indexEVetoHitGoodClus = fEvent->EVetoRecoCl->Element(ii)->GetHitVecInClus();
    if(tempNHit!=indexEVetoHitGoodClus.size()) std::cout<<"size mismatch EVeto"<<std::endl;
    for(int kk=0;kk<indexEVetoHitGoodClus.size();kk++){    
      indexHit = indexEVetoHitGoodClus[kk];
      tempChHit = fEvent->EVetoRecoEvent->Hit(indexHit)->GetChannelId();
      tempTHit = fEvent->EVetoRecoEvent->Hit(indexHit)->GetTime();
      tempEHit = fEvent->EVetoRecoEvent->Hit(indexHit)->GetEnergy();

      chtempEVetoHitGoodClus.push_back(tempChHit);
      ttempEVetoHitGoodClus.push_back(tempTHit);
      entempEVetoHitGoodClus.push_back(tempEHit);
      fHS->FillHistoList("EVetoHits","hGoodClusterEVetoHitEnergy",tempEHit);
    }
    chEVetoHitGoodClus.push_back(chtempEVetoHitGoodClus);
    tEVetoHitGoodClus.push_back(ttempEVetoHitGoodClus);
    enEVetoHitGoodClus.push_back(entempEVetoHitGoodClus);

  }//end EVetoClusters
  
  fHS->FillHistoList("EVetoClusters","hGoodClusNEVetoCluster", NEVetoGoodClusters);
  
  int tempEvCh;
  
  //All PVeto Clusters
  for(int ii = 0; ii<NPVetoCluster; ii++){
    tempNHit  = fEvent->PVetoRecoCl->Element(ii)->GetNHitsInClus();
    tempEClus = fEvent->PVetoRecoCl->Element(ii)->GetEnergy(); 
    tempT  =  fEvent->PVetoRecoCl->Element(ii)->GetTime();
    tempCh =  fEvent->PVetoRecoCl->Element(ii)->GetChannelId();

    fHS->FillHistoList("PVetoClusters","hNHitsPVetoCluster",tempNHit);
    fHS->FillHistoList("PVetoClusters","hEPerHitPVetoCluster",tempEClus/tempNHit);
    fHS->FillHistoList("PVetoClusters","htPVetoCluster",tempT);
    fHS->FillHistoList("PVetoClusters","hChPVetoCluster",tempCh);
    fHS->FillHistoList("PVetoClusters","hEnergyPVetoCluster",tempEClus);
    
    for(int jj=ii+1;jj<NPVetoCluster;jj++){
      temp2NHit  = fEvent->PVetoRecoCl->Element(jj)->GetNHitsInClus();
      temp2EClus = fEvent->PVetoRecoCl->Element(jj)->GetEnergy(); 
      temp2T  =  fEvent->PVetoRecoCl->Element(jj)->GetTime();
      temp2Ch =  fEvent->PVetoRecoCl->Element(jj)->GetChannelId();

      if(temp2Ch>tempCh) LucePVeto = temp2Ch-(tempCh+tempNHit);
      else if(tempCh>temp2Ch) LucePVeto = tempCh-(temp2Ch+temp2NHit);
      else LucePVeto=0;
      
      fHS->FillHisto2List("PVetoClusters","hLuceVsDeltaTClusPVeto",LucePVeto,temp2T-tempT);
      fHS->FillHistoList("ClusterTimeList","hClusTimeDiffPVeto",temp2T-tempT);
      fHS->FillHisto2List("ClusterTimeList","hClusTimeDiffPVetoVsPVeto1stCh",tempCh,temp2T-tempT);
      fHS->FillHisto2List("ClusterTimeList","hClusTimeDiffPVetoVsPVeto2ndCh",temp2Ch,temp2T-tempT);
      fHS->FillHisto2List("ClusterTimeList","hClusTimeDiffPVetoVsPVetoDeltaCh",tempCh-temp2Ch,temp2T-tempT);
      
      //two good clusters
      if((tempCh>30&&tempCh<71&&(tempNHit==2||tempNHit==3))&&(temp2Ch>30&&temp2Ch<71&&(temp2NHit==2||temp2NHit==3))&&(tempEClus/tempNHit)>0.7&&(temp2EClus/temp2NHit)>0.7){
	fHS->FillHisto2List("PVetoClusters","hVetoChasOver302to3HitsGoodChaLuceVsDeltaTClusPVeto",LucePVeto,temp2T-tempT);
	fHS->FillHistoList("ClusterTimeList","hGoodChas2to3HitsClusTimeDiffPVeto",temp2T-tempT);
	fHS->FillHisto2List("ClusterTimeList","hGoodChas2to3HitsClusTimeDiffPVetoVsPVeto1stCh",tempCh,temp2T-tempT);
	fHS->FillHisto2List("ClusterTimeList","hGoodChas2to3HitsClusTimeDiffPVetoVsPVeto2ndCh",temp2Ch,temp2T-tempT);
	fHS->FillHisto2List("ClusterTimeList","hGoodChas2to3HitsClusTimeDiffPVetoVsPVetoDeltaCh",temp2Ch-tempCh,temp2T-tempT);
      }
    }//end jj
    fHS->FillHisto2List("PVetoClusters","hPVetoChVsNHitsPVetoCluster",tempCh,tempNHit);
    if(tempCh<20)                  fHS->FillHistoList("PVetoClusters","h0029ChNHitsPVetoCluster",tempNHit);
    else if(tempCh>19&&tempCh<71)  fHS->FillHistoList("PVetoClusters","h3070ChNHitsPVetoCluster",tempNHit);
    else if(tempCh>70&&tempCh<85)  fHS->FillHistoList("PVetoClusters","h7084ChNHitsPVetoCluster",tempNHit);
    else if(tempCh>84)             fHS->FillHistoList("PVetoClusters","h85UpChNHitsPVetoCluster",tempNHit);

    if(isMC){
      for(int ii = 0; ii<NEVetoCluster; ii++){
	tempEvCh =  fEvent->EVetoRecoCl->Element(ii)->GetChannelId();
	fHS->FillHisto2List("MCBhabha","hAllNPVetoClusterVsNEVetoCluster",tempCh,tempEvCh);
      }
    }
  
    if(isMC&&fSwimBhabha){
      for(int swumev=0;swumev<PVetoSwimmingChannels.size();swumev++){
	if(PVetoSwimmingChannels[swumev]<0) continue;
	//	std::cout<<"Event No "<<fEvent->RecoEvent->GetEventNumber()<<" no. PVetoSwum "<<PVetoSwimmingChannels.size()<<" PVetoSwimmingChannels[swumev] "<<PVetoSwimmingChannels[swumev]<<" tempCh "<<tempCh<<std::endl;
	fHS->FillHistoList("MCSwimming","hPVetoSwumChPVetoRecoChaDiff",PVetoSwimmingChannels[swumev]-tempCh);
	fHS->FillHistoList("MCSwimming","hPVetoSwumTimePVetoRecoTimeDiff",PVetoSwimmingTime[swumev]-tempT);
	if(fabs(tempCh-PVetoSwimmingChannels[swumev])<=1&&fabs(tempT-PVetoSwimmingTime[swumev])<5) nswimandtruthpveto++;// std::cout<<"yay for pveto"<<std::endl;
      }
    }
    
    //good cluster quality cuts, pveto:
    if(tempCh<30||tempCh>70) continue;
    //if(tempNHit<2||tempNHit>3) continue; //8/2 turned off: not well reproduced in data/mc and makes very little difference to signal/background ratio
    //    if(!(tempEClus/tempNHit)>0.7) continue;
    
    fHS->FillHistoList("PVetoClusters","hGoodChasNHitsPVetoCluster",tempNHit);
    fHS->FillHistoList("PVetoClusters","hGoodChasEPerHitPVetoCluster",tempEClus/tempNHit);
    fHS->FillHistoList("PVetoClusters","hGoodChastPVetoCluster",tempT);
    fHS->FillHistoList("PVetoClusters","hGoodChasEnergyPVetoCluster",tempEClus);

    //Counter of PVeto clusters that pass
    NPVetoGoodClusters++;
    npassPVeto++;

    //fill vectors with parameters of good clusters
    NHitsPVetoGood.push_back(tempNHit);
    enPVetoGood.push_back(tempEClus);
    tPVetoGood.push_back(tempT);
    chPVetoGood.push_back(tempCh);

    indexPVetoHitGoodClus = fEvent->PVetoRecoCl->Element(ii)->GetHitVecInClus();
    if(tempNHit!=indexPVetoHitGoodClus.size()) std::cout<<"size mismatch PVeto"<<std::endl;
    for(int kk=0;kk<indexPVetoHitGoodClus.size();kk++){    
      indexHit = indexPVetoHitGoodClus[kk];
      tempChHit = fEvent->PVetoRecoEvent->Hit(indexHit)->GetChannelId();
      tempTHit = fEvent->PVetoRecoEvent->Hit(indexHit)->GetTime();
      tempEHit = fEvent->PVetoRecoEvent->Hit(indexHit)->GetEnergy();

      chtempPVetoHitGoodClus.push_back(tempChHit);
      ttempPVetoHitGoodClus.push_back(tempTHit);
      entempPVetoHitGoodClus.push_back(tempEHit);

      fHS->FillHistoList("PVetoHits","hGoodClusterPVetoHitEnergy",tempEHit);
    }
    chPVetoHitGoodClus.push_back(chtempPVetoHitGoodClus);
    tPVetoHitGoodClus.push_back(ttempPVetoHitGoodClus);
    enPVetoHitGoodClus.push_back(entempPVetoHitGoodClus);

  }//end PVetoCluster
  fHS->FillHistoList("PVetoClusters","hGoodClusNPVetoCluster", NPVetoGoodClusters);
  
  //time
  double tPVeto;
  double tEVeto;
  
  //variables to calculate time difference corrected for different trajectories into vetoes:
  //PVeto channel - EVeto channel
  int deltaCh;
  int deltaChSwim;

  //Time difference caused by trajectory difference
  double deltaTtraj;
  double deltaTtrajSwim;
  
  //Corrected time difference
  double deltaTcorrect;
  double deltaTcorrectSwim;
  
  //channel
  int chPVeto;
  int chEVeto;

  //no hits
  int NHitsPVeto;
  int NHitsEVeto;

  //cluster energy
  double enPVeto;
  double enEVeto;

  //particle energy assuming emission colinear with beam
  double enBremPositron;
  double enBremElectron;

  int MaxParticles;
  
  if(isMC&&fSwimBhabha){
    //not all events will have the same number of e+ as e-
    MaxParticles = TMath::Max(nBhabhaPos,nBhabhaEle);
    for(int ii =0; ii<MaxParticles; ii++){
      if(PVetoSwimmingChannels[ii]<0||EVetoSwimmingChannels[ii]<0) continue;
      deltaChSwim = PVetoSwimmingChannels[ii]-EVetoSwimmingChannels[ii];
      deltaTtrajSwim = TMath::Power(deltaChSwim,3)*-6.5e-7+TMath::Power(deltaChSwim,2)*3.2e-6+deltaChSwim*0.04-0.07;
      deltaTcorrectSwim = PVetoSwimmingTime[ii]-EVetoSwimmingTime[ii]-deltaTtrajSwim;
	
      fHS->FillHistoList("MCSwimming","hPVetoSwumTimeEVetoSwumTimeDiff",PVetoSwimmingTime[ii]-EVetoSwimmingTime[ii]);
      fHS->FillHistoList("MCSwimming","hPVetoSwumTimeEVetoSwumTimeDiffCorrect",deltaTcorrectSwim);
      fHS->FillHisto2List("MCSwimming","hPVetoSwumChVsEVetoSwumCh",PVetoSwimmingChannels[ii],EVetoSwimmingChannels[ii]);
      fHS->FillHisto2List("MCSwimming","hVetoSwumChaDiffVsVetoSwumTimeDiff",PVetoSwimmingChannels[ii]-EVetoSwimmingChannels[ii],PVetoSwimmingTime[ii]-EVetoSwimmingTime[ii]);
      fHS->FillHistoList("MCSwimming","hSwumChaSum",PVetoSwimmingChannels[ii]+EVetoSwimmingChannels[ii]);
      
      if(PVetoSwimmingChannels[ii]<31||EVetoSwimmingChannels[ii]<31||PVetoSwimmingChannels[ii]>70||EVetoSwimmingChannels[ii]>70) continue;
      fHS->FillHistoList("MCSwimming","hGoodChasSwumChaSum",PVetoSwimmingChannels[ii]+EVetoSwimmingChannels[ii]);
      if((PVetoSwimmingChannels[ii]+EVetoSwimmingChannels[ii])<91) continue;
      fHS->FillHistoList("MCSwimming","hChaSumOver90GoodChasPVetoSwumTimeEVetoSwumTimeDiffCorrect",deltaTcorrectSwim);
      fHS->FillHisto2List("MCSwimming","hChaSumOver90GoodChasPVetoSwumChVsEVetoSwumCh",PVetoSwimmingChannels[ii],EVetoSwimmingChannels[ii]);
      if((PVetoSwimmingChannels[ii]+EVetoSwimmingChannels[ii])<96||(PVetoSwimmingChannels[ii]+EVetoSwimmingChannels[ii])>98) continue;
      fHS->FillHistoList("MCSwimming","hChaSum96to98GoodChasPVetoSwumTimeEVetoSwumTimeDiffCorrect",deltaTcorrectSwim);
      fHS->FillHisto2List("MCSwimming","hChaSum96to98GoodChasPVetoSwumChVsEVetoSwumCh",PVetoSwimmingChannels[ii],EVetoSwimmingChannels[ii]);
      NSwimBhabha++;
      fHS->FillHistoList("VetoEndPointList","hNTotBhabha_NMatched_NRecoBhabha_NSwimBhabha",3);
    }
  }//end if MC
  
  if(NEVetoGoodClusters!=NHitsEVetoGood.size()||NPVetoGoodClusters!=NHitsPVetoGood.size()){
    std::cout<<"Mismatched cluster sizes in BhabhaAnalysis. Check vector pushback and counter increase"<<std::endl;
    return 0;
  }

  double EnEVetoHit;
  double EnPVetoHit;

  //loop over good clusters
  for(int ii = 0; ii<NEVetoGoodClusters; ii++){
    //import EVeto variables
    tEVeto     =  tEVetoGood[ii];
    chEVeto    =  chEVetoGood[ii];
    NHitsEVeto =  NHitsEVetoGood[ii];
    enEVeto    =  enEVetoGood[ii];
   
    for(int jj = 0; jj<NPVetoGoodClusters; jj++){      
      //import PVeto variables
      tPVeto     =  tPVetoGood[jj];
      chPVeto    =  chPVetoGood[jj];
      NHitsPVeto =  NHitsPVetoGood[jj];
      enPVeto    =  enPVetoGood[jj];

      //histograms of raw variables
      //time difference corrected for trajectory
      deltaCh = chPVeto-chEVeto;
      deltaTtraj = TMath::Power(deltaCh,3)*-6.5e-7+TMath::Power(deltaCh,2)*3.2e-6+deltaCh*0.04-0.07;
      deltaTcorrect = tPVeto-tEVeto-deltaTtraj;
      
      fHS->FillHistoList("TimeCorrectionList","hdeltaTtrajPVetoEVeto",deltaTtraj);
      fHS->FillHistoList("TimeCorrectionList","hdeltaTuncorrectPVetoEVeto",tPVeto-tEVeto);
      fHS->FillHistoList("TimeCorrectionList","hdeltaTcorrectPVetoEVeto",deltaTcorrect);

      fHS->FillHisto2List("TimeCorrectionList","hdeltaChVsdeltaTtrajPVetoEVeto",chPVeto-chEVeto,deltaTtraj);
      fHS->FillHisto2List("TimeCorrectionList","hdeltaChVsdeltaTuncorrect",chPVeto-chEVeto,tPVeto-tEVeto);
      fHS->FillHisto2List("TimeCorrectionList","hdeltaChVsdeltaTcorrect",chPVeto-chEVeto,deltaTcorrect);
      fHS->FillHisto2List("TimeCorrectionList","hdeltaTuncorrectVsdeltaTcorrectPVetoEVeto",tPVeto-tEVeto,deltaTcorrect);
      
      fHS->FillHistoList("BhabhaList","hGoodClusChaSum",chPVeto+chEVeto);
      fHS->FillHisto2List("BhabhaList","hChaSumVsDeltaTuncorrect",chPVeto+chEVeto,tPVeto-tEVeto);
      fHS->FillHisto2List("BhabhaList","hChaSumVsDeltaTcorrect",chPVeto+chEVeto,deltaTcorrect);

      if(!(fabs(deltaTcorrect)<10)) continue;
      //      std::cout<<"good deltat. enPVetoHitGoodClus[jj].size() "<<enPVetoHitGoodClus[jj].size()<<" enEVetoHitGoodClus[ii].size() "<<enEVetoHitGoodClus[ii].size()<<std::endl;
      for(int kk=0; kk<enPVetoHitGoodClus[jj].size(); kk++){
	fHS->FillHistoList("PVetoHits","h10nsInTimeGoodClusterPVetoHitEnergy",enPVetoHitGoodClus[jj][kk]);
      }
      for(int kk=0; kk<enEVetoHitGoodClus[ii].size(); kk++){
	fHS->FillHistoList("EVetoHits","h10nsInTimeGoodClusterEVetoHitEnergy",enEVetoHitGoodClus[ii][kk]);
      }
      
      if(!(fabs(deltaTcorrect)<1)) continue;

      fHS->FillHistoList("BhabhaList","h1nsInTimeGoodClusChaSum",chPVeto+chEVeto);
      fHS->FillHisto2List("BhabhaList","h1nsChaSumVsDeltaTuncorrect",chPVeto+chEVeto,deltaTcorrect);
      fHS->FillHisto2List("BhabhaList","h1nsChaSumVsDeltaTcorrect",chPVeto+chEVeto,deltaTcorrect);

      for(int kk=0; kk<enPVetoHitGoodClus[jj].size(); kk++){
	fHS->FillHistoList("PVetoHits","h1nsInTimeGoodClusterPVetoHitEnergy",enPVetoHitGoodClus[jj][kk]);
      }
      for(int kk=0; kk<enEVetoHitGoodClus[ii].size(); kk++){
	fHS->FillHistoList("EVetoHits","h1nsInTimeGoodClusterEVetoHitEnergy",enEVetoHitGoodClus[ii][kk]);
      }


      NRecoBhabha++;
      fHS->FillHistoList("VetoEndPointList","hNTotBhabha_NMatched_NRecoBhabha_NSwimBhabha",2);
      if(isMC&&fSwimBhabha){
	for(int ii =0; ii<MaxParticles; ii++){
	  if(PVetoSwimmingChannels[ii]<0||EVetoSwimmingChannels[ii]<0) continue;
	  if(fabs(PVetoSwimmingChannels[ii]-chPVeto)<2&&fabs(EVetoSwimmingChannels[ii]-chEVeto)<2){
	    NMatched++;
	    fHS->FillHistoList("VetoEndPointList","hNTotBhabha_NMatched_NRecoBhabha_NSwimBhabha",1);
	  }//end if match
	}//end bhabha loop
      }//end if MC
    }//end good pveto clusters
  }//end good eveto clusters
  return true;
}

Bool_t BhabhaAnalysis::Finalize()
{
  // TGraph* nPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vNPoT[0]);
  // fHS->SaveTGraphList("GGAnalysis","NPotVsTime",nPotVsTime);
  if(fVetoEndPoint->DoesVetoEndPointHistosExist())
    fVetoEndPoint->SaveHistos();
  else std::cout<<"fVetoEndPoint doesn't exist in BhabhaAnalysis::Finalize()"<<std::endl;

  std::cout<<"NTotBhabha "<<NTotBhabha<<" NSwum "<<NSwum<<" NMatched "<<NMatched<<" NRecoBhabha "<<NRecoBhabha<<" NSwimBhabha "<<NSwimBhabha<<std::endl;
  std::cout<<"nswimandtrutheveto "<<nswimandtrutheveto<<" nswimandtruthpveto "<<nswimandtruthpveto<<std::endl;
  if (fVerbose) printf("---> Finalizing BhabhaAnalysis\n");
  return true;
}

