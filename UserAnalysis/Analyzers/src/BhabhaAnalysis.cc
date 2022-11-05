// Written by M. Raggi   20/07/2021 
// Last modified by M. Raggi 12/08/2021
#include "BhabhaAnalysis.hh"
#include "TMath.h"
#include "VetoEndPoint.hh"

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
  fHS->CreateList("BhabhaList");
  fHS->CreateList("MCBhabha");
  fHS->CreateList("TimeCorrectionList");
  fHS->CreateList("BackgroundList");

  //No. PoT used in this analysis
  fHS->BookHistoList("NPoTAnalysis","hNPotBhabha",600,-1000.,59000.);
  
  //Number of clusters
  fHS->BookHistoList("PVetoClusters","hNPVetoCluster",100,0,100);
  fHS->BookHistoList("EVetoClusters","hNEVetoCluster",100,0,100);

  fHS->BookHistoList("PVetoClusters","hVetoChasOver302to3HitsGoodChaNPVetoCluster",10,0,10);
  fHS->BookHistoList("EVetoClusters","hVetoChasOver302to3HitsGoodChaNEVetoCluster",10,0,10);

  //Cluster time
  fHS->BookHistoList("PVetoClusters","htPVetoCluster",500,-250,250);
  fHS->BookHistoList("EVetoClusters","htEVetoCluster",500,-250,250);

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

  fHS->BookHisto2List("PVetoClusters","hPVetoChVshNHitsPVetoCluster",90,0,89,15,0,15);
  fHS->BookHisto2List("EVetoClusters","hEVetoChVshNHitsEVetoCluster",90,0,89,15,0,15);

  //Energy per hit in cluster
  fHS->BookHistoList("PVetoClusters","hEPerHitPVetoCluster",75,0,15);
  fHS->BookHistoList("EVetoClusters","hEPerHitEVetoCluster",75,0,15);

  //Cluster energy
  fHS->BookHistoList("PVetoClusters","hEnergyPVetoCluster",150,0,30);
  fHS->BookHistoList("EVetoClusters","hEnergyEVetoCluster",150,0,30);

  //Cluster channel
  fHS->BookHistoList("PVetoClusters","hChPVetoCluster",96,0,96);
  fHS->BookHistoList("EVetoClusters","hChEVetoCluster",96,0,96);
 
  //Cluster energy
  fHS->BookHistoList("PVetoClusters","hChToEnergyPositron",330,0,330);
  fHS->BookHistoList("EVetoClusters","hChToEnergyElectron",330,0,330);

  //Particle energy sum assuming Bremsstrahlung
  fHS->BookHistoList("BhabhaList","hChToEnergySum",200,0,750);
  fHS->BookHistoList("BhabhaList","hChToEnergySum10nsWindow",200,0,750);
  fHS->BookHistoList("BhabhaList","hChToEnergySum10nsWindowGood",200,0,750);
  fHS->BookHistoList("BhabhaList","hChToEnergySum10nsWindowGoodMoreEnergeticElectron",200,0,750);

  //Bhabha plots
  fHS->BookHisto2List("BhabhaList","h10nsWindowNPVetoClusterVsNEVetoCluster",96,0,95,96,0,95);
  fHS->BookHisto2List("BhabhaList","hTop10nsWindowNPVetoClusterVsNEVetoCluster",96,0,95,96,0,95);
  fHS->BookHisto2List("BhabhaList","hMiddle10nsWindowNPVetoClusterVsNEVetoCluster",96,0,95,96,0,95);
  fHS->BookHisto2List("BhabhaList","hBottom10nsWindowNPVetoClusterVsNEVetoCluster",96,0,95,96,0,95);
  //  fHS->BookHisto2List("BhabhaList","h10nsWindowNPVetoClusterVsNEVetoClusterMax10kPoT",96,0,95,96,0,95);
  //  fHS->BookHisto2List("BhabhaList","h10nsWindowNPVetoClusterVsNEVetoClusterMin6.5kMax10kPoT",96,0,95,96,0,95);
  fHS->BookHisto2List("BhabhaList","h10nsWindowNPVetoClusterVsNEVetoClusterDeltaTWeight",96,0,95,96,0,95);
  // fHS->BookHisto2List("BhabhaList","h2to3Hits10nsWindowNPVetoClusterVsNEVetoCluster",96,0,95,96,0,95);
  // fHS->BookHisto2List("BhabhaList","hEnergyCut2to3Hits10nsWindowNPVetoClusterVsNEVetoCluster",96,0,95,96,0,95);
  // fHS->BookHisto2List("BhabhaList","hGoodChasEnergyCut2to3Hits10nsWindowNPVetoClusterVsNEVetoCluster",96,0,95,96,0,95);
  // fHS->BookHisto2List("BhabhaList","hLeadingElectronGoodChasEnergyCut2to3Hits10nsWindowNPVetoClusterVsNEVetoCluster",96,0,95,96,0,95);
  fHS->BookHisto2List("BhabhaList","hChaSumVsDeltaTuncorrect",180,0,179,2000,-500,500);
  fHS->BookHisto2List("BhabhaList","hChaSumVsDeltaTcorrect",180,0,179,2000,-500,500);
  fHS->BookHisto2List("BhabhaList","hGoodChaSumVsDeltaTuncorrect",40,59.5,140.5,2000,-500,500);
  fHS->BookHisto2List("BhabhaList","hGoodChaSumVsDeltaTcorrect",40,59.5,140.5,2000,-500,500);
  fHS->BookHisto2List("BhabhaList","h2to3HitsGoodChaSumVsDeltaTuncorrect",40,59.5,140.5,2000,-500,500);
  fHS->BookHisto2List("BhabhaList","h2to3HitsGoodChaSumVsDeltaTcorrect",40,59.5,140.5,2000,-500,500);
  fHS->BookHisto2List("BhabhaList","hVetoChasOver302to3HitsGoodChaSumVsDeltaTuncorrect",40,59.5,140.5,2000,-500,500);
  fHS->BookHisto2List("BhabhaList","hVetoChasOver302to3HitsGoodChaSumVsDeltaTcorrect",40,59.5,140.5,2000,-500,500);

  //MC only Bhabha plots
  fHS->BookHisto2List("MCBhabha","hAllNPVetoClusterVsNEVetoCluster",96,0,95,96,0,95);
  fHS->BookHisto2List("MCBhabha","hBhabhaNPVetoClusterVsNEVetoCluster",96,0,95,96,0,95);
  fHS->BookHisto2List("MCBhabha","hAllBoxNPVetoClusterVsNEVetoCluster",96,0,95,96,0,95);
  fHS->BookHisto2List("MCBhabha","hBhabhaBoxNPVetoClusterVsNEVetoCluster",96,0,95,96,0,95);
  
  //Time correction plots
  fHS->BookHistoList("TimeCorrectionList","hdeltaTuncorrectPVetoEVeto",2000,-500,500);
  fHS->BookHistoList("TimeCorrectionList","hdeltaTcorrectPVetoEVeto",2000,-500,500);
  fHS->BookHistoList("TimeCorrectionList","hGoodChasdeltaTuncorrectPVetoEVeto",2000,-500,500);
  fHS->BookHistoList("TimeCorrectionList","hGoodChasdeltaTcorrectPVetoEVeto",2000,-500,500);
  fHS->BookHistoList("TimeCorrectionList","h2to3HitsGoodChasdeltaTuncorrectPVetoEVeto",2000,-500,500);
  fHS->BookHistoList("TimeCorrectionList","h2to3HitsGoodChasdeltaTcorrectPVetoEVeto",2000,-500,500);
  fHS->BookHistoList("TimeCorrectionList","hVetoChasOver302to3HitsGoodChasdeltaTuncorrectPVetoEVeto",2000,-500,500);
  fHS->BookHistoList("TimeCorrectionList","hVetoChasOver302to3HitsGoodChasdeltaTcorrectPVetoEVeto",2000,-500,500);
  fHS->BookHistoList("TimeCorrectionList","hChaSumOver90VetoChasOver302to3HitsGoodChasdeltaTuncorrectPVetoEVeto",2000,-500,500);
  fHS->BookHistoList("TimeCorrectionList","hChaSumOver90VetoChasOver302to3HitsGoodChasdeltaTcorrectPVetoEVeto",2000,-500,500);
  fHS->BookHistoList("TimeCorrectionList","hdeltaTtrajPVetoEVeto",2000,-500,500);
  
  fHS->BookHisto2List("TimeCorrectionList","hdeltaChVsdeltaTuncorrect",180,-90,90,2000,-500,500);
  fHS->BookHisto2List("TimeCorrectionList","hTopdeltaChVsdeltaTuncorrect",180,-90,90,2000,-500,500);
  fHS->BookHisto2List("TimeCorrectionList","hMiddledeltaChVsdeltaTuncorrect",180,-90,90,2000,-500,500);
  fHS->BookHisto2List("TimeCorrectionList","hBottomdeltaChVsdeltaTuncorrect",180,-90,90,2000,-500,500);
  fHS->BookHisto2List("TimeCorrectionList","hdeltaChVsdeltaTcorrect",180,-90,90,2000,-500,500);
  fHS->BookHisto2List("TimeCorrectionList","hdeltaChGoodChasVsdeltaTuncorrect",101,-50,50,2000,-500,500);
  fHS->BookHisto2List("TimeCorrectionList","hTopdeltaChGoodChasVsdeltaTuncorrect",180,-90,90,2000,-500,500);
  fHS->BookHisto2List("TimeCorrectionList","hMiddledeltaChGoodChasVsdeltaTuncorrect",180,-90,90,2000,-500,500);
  fHS->BookHisto2List("TimeCorrectionList","hBottomdeltaChGoodChasVsdeltaTuncorrect",180,-90,90,2000,-500,500);
  fHS->BookHisto2List("TimeCorrectionList","hdeltaChGoodChasVsdeltaTcorrect",101,-50,50,2000,-500,500);
  fHS->BookHisto2List("TimeCorrectionList","hdeltaChVsdeltaTtrajPVetoEVeto",101,-50,50,2000,-500,500);
  fHS->BookHisto2List("TimeCorrectionList","hdeltaTuncorrectVsdeltaTcorrectPVetoEVeto",2000,-500,500,2000,-500,500);
  fHS->BookHisto2List("TimeCorrectionList","hdeltaTuncorrectGoodChasVsdeltaTcorrectPVetoEVeto",2000,-500,500,2000,-500,500);
  //  fHS->BookHisto2List("TimeCorrectionList","hdeltaTtrajVsdeltaTcorrectPVetoEVeto",2000,-500,500,2000,-500,500);

  //Background studies
  fHS->BookHistoList("BackgroundList","hOutofTime5nsChaSum100VetoChasOver302to3HitsGoodChasPVetoCh",90,-0.5,89.5);
  fHS->BookHistoList("BackgroundList","hOutofTime5nsChaSum100VetoChasOver302to3HitsGoodChasEVetoCh",90,-0.5,89.5);
  fHS->BookHistoList("BackgroundList","hOutofTime5nsChaSum99VetoChasOver302to3HitsGoodChasPVetoCh",90,-0.5,89.5);
  fHS->BookHistoList("BackgroundList","hOutofTime5nsChaSum99VetoChasOver302to3HitsGoodChasEVetoCh",90,-0.5,89.5);

  fHS->BookHistoList("BackgroundList","hInTime5nsChaSum100VetoChasOver302to3HitsGoodChasPVetoCh",90,-0.5,89.5);
  fHS->BookHistoList("BackgroundList","hInTime5nsChaSum100VetoChasOver302to3HitsGoodChasEVetoCh",90,-0.5,89.5);
  fHS->BookHistoList("BackgroundList","hInTime5nsChaSum99VetoChasOver302to3HitsGoodChasPVetoCh",90,-0.5,89.5);
  fHS->BookHistoList("BackgroundList","hInTime5nsChaSum99VetoChasOver302to3HitsGoodChasEVetoCh",90,-0.5,89.5);

  return true;
}

Bool_t BhabhaAnalysis::Process(){

  Bool_t isMC = false;
  if (fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) {
    isMC=true;
    //    std::cout<<"input data are simulatetd "<<std::endl;
  }
  
  //Cut on physics trigger Data Only
  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  fHS->FillHistoList("TOP","TriggerMaskBhabha",trigMask);

  if( !(trigMask & (1 << 0)) && isMC==false) return false;
  
  fNPoT = fEvent->TargetRecoBeam->getnPOT();

  //MCTruth: momentum of e+ or e- resulting from Bhabha
  TVector3 momentum;
  
  Int_t charge;

  TVector3 VertexPos;
  Double_t VertexTime=-1000;
  
  if(isMC){
    for(Int_t iV = 0; iV<fEvent->MCTruthEvent->GetNVertices(); iV++) {
      mcVtx = fEvent->MCTruthEvent->Vertex(iV);
      if(mcVtx->GetProcess() == "eIoni"){
	VertexPos=mcVtx->GetPosition();
	VertexTime = mcVtx->GetTime();
	//	VertexPos.Print();
	for(Int_t iO = 0; iO<mcVtx->GetNParticleOut(); iO++) {
	  mcOutPart = mcVtx->ParticleOut(iO);
	  
	  if(mcOutPart->GetPDGCode()==11)
	    charge = 1;
	  if(mcOutPart->GetPDGCode()==-11)
	    charge = -1;

	  momentum = mcOutPart->GetMomentum();
	  fVetoEndPoint->ParticleSwim(momentum,VertexPos,VertexTime,charge);
	  NSwum++;
	}
      }
    }
  }

  //  if(fNPoT>10000) return 0;

  fHS->FillHistoList("NPoTAnalysis","hNPotBhabha",fNPoT);

  //Number of PVeto clusters
  Int_t NPVetoCluster = fEvent->PVetoRecoCl->GetNElements();
  fHS->FillHistoList("PVetoClusters","hNPVetoCluster",NPVetoCluster);

  //Number of EVeto clusters
  Int_t NEVetoCluster = fEvent->EVetoRecoCl->GetNElements();
  fHS->FillHistoList("EVetoClusters","hNEVetoCluster",NEVetoCluster);

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

  //temporary variables for the loop
  double tempEClus;
  double tempT;
  int tempNHit;
  int tempCh;

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
      if((tempCh>30&&tempCh<71&&(tempNHit==2||tempNHit==3))&&(temp2Ch>30&&temp2Ch<71&&(temp2NHit==2||temp2NHit==3))) fHS->FillHisto2List("EVetoClusters","hVetoChasOver302to3HitsGoodChaLuceVsDeltaTClusEVeto",LuceEVeto,temp2T-tempT);
      if(LuceEVeto>100)      std::cout<<"ii "<<ii<<" jj"<<jj<<" tempCh "<<tempCh<<" temp2Ch "<<temp2Ch<<" tempNHit "<<tempNHit<<" temp2NHit "<<temp2NHit<<" LuceEVeto "<<LuceEVeto<<std::endl;

    }

    fHS->FillHisto2List("EVetoClusters","hEVetoChVshNHitsEVetoCluster",tempCh,tempNHit);
    //good cluster cuts:
    //>=2 hits
    if(tempNHit<2) continue;
    //energy per hit > 0.7
    if(!(tempEClus/tempNHit)>0.7) continue;

    //Counter of EVeto clusters that pass
    NEVetoGoodClusters++;

    if(tempCh>30&&tempCh<71&&(tempNHit==2||tempNHit==3)) npassEVeto++;
    
    //fill vectors with parameters of good hits
    NHitsEVetoGood.push_back(tempNHit);
    enEVetoGood.push_back(tempEClus);
    tEVetoGood.push_back(tempT);
    chEVetoGood.push_back(tempCh);
  }

  fHS->FillHistoList("EVetoClusters","hVetoChasOver302to3HitsGoodChaNEVetoCluster", npassEVeto);

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

      //two good clusters
      if((tempCh>30&&tempCh<71&&(tempNHit==2||tempNHit==3))&&(temp2Ch>30&&temp2Ch<71&&(temp2NHit==2||temp2NHit==3))&&(tempEClus/tempNHit)>0.7&&(temp2EClus/temp2NHit)>0.7)
	fHS->FillHisto2List("PVetoClusters","hVetoChasOver302to3HitsGoodChaLuceVsDeltaTClusPVeto",LucePVeto,temp2T-tempT);
    }


    fHS->FillHisto2List("PVetoClusters","hPVetoChVshNHitsPVetoCluster",tempCh,tempNHit);
    if(isMC){
      for(int ii = 0; ii<NEVetoCluster; ii++){
	tempEvCh =  fEvent->EVetoRecoCl->Element(ii)->GetChannelId();
	fHS->FillHisto2List("MCBhabha","hAllNPVetoClusterVsNEVetoCluster",tempCh,tempEvCh);
      }
    }
    //good cluster cuts:
    //>=2 hits
    if(tempNHit<2) continue;
    //energy per hit > 0.7
    if(!(tempEClus/tempNHit)>0.7) continue;

    //Counter of PVeto clusters that pass
    NPVetoGoodClusters++;

    //fill vectors with parameters of good hits
    NHitsPVetoGood.push_back(tempNHit);
    enPVetoGood.push_back(tempEClus);
    tPVetoGood.push_back(tempT);
    chPVetoGood.push_back(tempCh);

    if(tempCh>30&&tempCh<71&&(tempNHit==2||tempNHit==3)) npassPVeto++;

  }
  fHS->FillHistoList("PVetoClusters","hVetoChasOver302to3HitsGoodChaNPVetoCluster", npassPVeto);

  //time
  double tPVeto;
  double tEVeto;

  //variables to calculate time difference corrected for different trajectories into vetoes:
  //PVeto channel - EVeto channel
  int deltaCh;

  //Time difference caused by trajectory difference
  double deltaTtraj;
  
  //Corrected time difference
  double deltaTcorrect;
  
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

  if(NEVetoGoodClusters!=NHitsEVetoGood.size()||NPVetoGoodClusters!=NHitsPVetoGood.size()){
    std::cout<<"Mismatched cluster sizes in BhabhaAnalysis. Check vector pushback and counter increase"<<std::endl;
    return 0;
  }

  //loop over good clusters
  for(int ii = 0; ii<NEVetoGoodClusters; ii++){
    //import EVeto variables
    tEVeto     =  tEVetoGood[ii];
    chEVeto    =  chEVetoGood[ii];
    NHitsEVeto =  NHitsEVetoGood[ii];
    enEVeto    =  enEVetoGood[ii];

    //   if(chEVeto>51&&chEVeto<56) continue;
 
    for(int jj = 0; jj<NPVetoGoodClusters; jj++){
      
      //      std::cout<<"NHitsEVetoGood.size() "<<NHitsEVetoGood.size()<<" ii "<<ii<<" NHitsPVetoGood.size() "<<NHitsPVetoGood.size()<<" jj "<<jj<<std::endl;

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
      //      fHS->FillHisto2List("TimeCorrectionList","hdeltaTtrajVsdeltaTcorrectPVetoEVeto",deltaTtraj,deltaTcorrect);
      fHS->FillHisto2List("TimeCorrectionList","hdeltaTuncorrectVsdeltaTcorrectPVetoEVeto",tPVeto-tEVeto,deltaTcorrect);

      fHS->FillHisto2List("BhabhaList","hChaSumVsDeltaTuncorrect",chPVeto+chEVeto,tPVeto-tEVeto);
      fHS->FillHisto2List("BhabhaList","hChaSumVsDeltaTcorrect",chPVeto+chEVeto,deltaTcorrect);

      if((chPVeto>19&&chEVeto>19&&chPVeto<71&&chEVeto<71)){
	fHS->FillHisto2List("BhabhaList","hGoodChaSumVsDeltaTuncorrect",chPVeto+chEVeto,tPVeto-tEVeto);
	fHS->FillHisto2List("BhabhaList","hGoodChaSumVsDeltaTcorrect",chPVeto+chEVeto,deltaTcorrect);

	fHS->FillHistoList("TimeCorrectionList","hGoodChasdeltaTuncorrectPVetoEVeto",tPVeto-tEVeto);
	fHS->FillHistoList("TimeCorrectionList","hGoodChasdeltaTcorrectPVetoEVeto",deltaTcorrect);

	fHS->FillHisto2List("TimeCorrectionList","hdeltaChGoodChasVsdeltaTuncorrect",chPVeto-chEVeto,tPVeto-tEVeto);
	fHS->FillHisto2List("TimeCorrectionList","hdeltaChGoodChasVsdeltaTcorrect",chPVeto-chEVeto,deltaTcorrect);
	fHS->FillHisto2List("TimeCorrectionList","hdeltaTuncorrectGoodChasVsdeltaTcorrectPVetoEVeto",tPVeto-tEVeto,deltaTcorrect);

	if((NHitsPVeto==2||NHitsPVeto==3)&&(NHitsEVeto==2||NHitsEVeto==3)){
	  fHS->FillHisto2List("BhabhaList","h2to3HitsGoodChaSumVsDeltaTuncorrect",chPVeto+chEVeto,tPVeto-tEVeto);
	  fHS->FillHisto2List("BhabhaList","h2to3HitsGoodChaSumVsDeltaTcorrect",chPVeto+chEVeto,deltaTcorrect);
	  fHS->FillHistoList("TimeCorrectionList","h2to3HitsGoodChasdeltaTuncorrectPVetoEVeto",tPVeto-tEVeto);
	  fHS->FillHistoList("TimeCorrectionList","h2to3HitsGoodChasdeltaTcorrectPVetoEVeto",deltaTcorrect);
	  if(chPVeto>29&&chEVeto>29){
	    //	    std::cout<<"NHitsEVetoGood.size() "<<NHitsEVetoGood.size()<<" ii "<<ii<<" NHitsPVetoGood.size() "<<NHitsPVetoGood.size()<<" jj "<<jj<<std::endl;
	    fHS->FillHistoList("TimeCorrectionList","hVetoChasOver302to3HitsGoodChasdeltaTuncorrectPVetoEVeto",tPVeto-tEVeto);
	    fHS->FillHistoList("TimeCorrectionList","hVetoChasOver302to3HitsGoodChasdeltaTcorrectPVetoEVeto",deltaTcorrect);
	    fHS->FillHisto2List("BhabhaList","hVetoChasOver302to3HitsGoodChaSumVsDeltaTuncorrect",chPVeto+chEVeto,tPVeto-tEVeto);
	    fHS->FillHisto2List("BhabhaList","hVetoChasOver302to3HitsGoodChaSumVsDeltaTcorrect",chPVeto+chEVeto,deltaTcorrect);

	    if(fabs(deltaTcorrect)>5){
	      if(chPVeto+chEVeto==99){
		fHS->FillHistoList("BackgroundList","hOutofTime5nsChaSum99VetoChasOver302to3HitsGoodChasPVetoCh",chPVeto);
		fHS->FillHistoList("BackgroundList","hOutofTime5nsChaSum99VetoChasOver302to3HitsGoodChasEVetoCh",chEVeto);
	      }
	      if(chPVeto+chEVeto==100){
		fHS->FillHistoList("BackgroundList","hOutofTime5nsChaSum100VetoChasOver302to3HitsGoodChasPVetoCh",chPVeto);
		fHS->FillHistoList("BackgroundList","hOutofTime5nsChaSum100VetoChasOver302to3HitsGoodChasEVetoCh",chEVeto);
	      }
	    }
	    else{
	      if(chPVeto+chEVeto==99){
		fHS->FillHistoList("BackgroundList","hInTime5nsChaSum99VetoChasOver302to3HitsGoodChasPVetoCh",chPVeto);
		fHS->FillHistoList("BackgroundList","hInTime5nsChaSum99VetoChasOver302to3HitsGoodChasEVetoCh",chEVeto);
	      }
	      if(chPVeto+chEVeto==100){
		fHS->FillHistoList("BackgroundList","hInTime5nsChaSum100VetoChasOver302to3HitsGoodChasPVetoCh",chPVeto);
		fHS->FillHistoList("BackgroundList","hInTime5nsChaSum100VetoChasOver302to3HitsGoodChasEVetoCh",chEVeto);
	      }
	    }

	    if(isMC){
	      if(std::fabs(deltaTcorrect)<2){
		//		fEvent->MCTruthEvent->Print("");
	      }
	    }
	    if(chPVeto+chEVeto>90){
	      fHS->FillHistoList("TimeCorrectionList","hChaSumOver90VetoChasOver302to3HitsGoodChasdeltaTuncorrectPVetoEVeto",tPVeto-tEVeto);
	      fHS->FillHistoList("TimeCorrectionList","hChaSumOver90VetoChasOver302to3HitsGoodChasdeltaTcorrectPVetoEVeto",deltaTcorrect);
	    }
	    //per entrambe le particelle:
	    //prendere variabili cinematiche
	    //passarle per VetEndPoint -> per fare swim
	    //Swim(momentum, position, time, charge)
	    //finisce dentro il veto? Se si dimmi il finger, senno dimmi -1
	    //posso farmi dire il tempo?
	  }
	}
      }

      /*      //within 10ns?
      if(!(std::fabs(tPVeto-tEVeto)<10)) continue;
      fHS->FillHisto2List("BhabhaList","h10nsWindowNPVetoClusterVsNEVetoCluster",chPVeto,chEVeto);
      if((tPVeto-tEVeto)>(0.03*(chPVeto-chEVeto)+1.15)){
	fHS->FillHisto2List("BhabhaList","hTop10nsWindowNPVetoClusterVsNEVetoCluster",chPVeto,chEVeto);
	fHS->FillHisto2List("TimeCorrectionList","hTopdeltaChVsdeltaTuncorrect",chPVeto-chEVeto,tPVeto-tEVeto);
	if(chPVeto>19&&chEVeto>19&&chPVeto<71&&chEVeto<71) fHS->FillHisto2List("TimeCorrectionList","hTopdeltaChGoodChasVsdeltaTuncorrect",chPVeto-chEVeto,tPVeto-tEVeto);
      }
      if((tPVeto-tEVeto)<(0.03*(chPVeto-chEVeto)+1.15)&&(tPVeto-tEVeto)>(0.03*(chPVeto-chEVeto)-1.3)){
	fHS->FillHisto2List("BhabhaList","hMiddle10nsWindowNPVetoClusterVsNEVetoCluster",chPVeto,chEVeto);
	fHS->FillHisto2List("TimeCorrectionList","hMiddledeltaChVsdeltaTuncorrect",chPVeto-chEVeto,tPVeto-tEVeto);
	if(chPVeto>19&&chEVeto>19&&chPVeto<71&&chEVeto<71) fHS->FillHisto2List("TimeCorrectionList","hMiddledeltaChGoodChasVsdeltaTuncorrect",chPVeto-chEVeto,tPVeto-tEVeto);
      }
      if((tPVeto-tEVeto)<(0.03*(chPVeto-chEVeto)-1.3)){
	fHS->FillHisto2List("BhabhaList","hBottom10nsWindowNPVetoClusterVsNEVetoCluster",chPVeto,chEVeto);
	fHS->FillHisto2List("TimeCorrectionList","hBottomdeltaChVsdeltaTuncorrect",chPVeto-chEVeto,tPVeto-tEVeto);
	if(chPVeto>19&&chEVeto>19&&chPVeto<71&&chEVeto<71) fHS->FillHisto2List("TimeCorrectionList","hBottomdeltaChGoodChasVsdeltaTuncorrect",chPVeto-chEVeto,tPVeto-tEVeto);
      }
      // if(fNPoT<10000){
      // 	fHS->FillHistoList("BhabhaList","h10nsWindowNPVetoClusterVsNEVetoClusterMax10kPoT",chPVeto,chEVeto);
      // 	if(fNPoT>6500)      fHS->FillHistoList("BhabhaList","h10nsWindowNPVetoClusterVsNEVetoClusterMin6.5kMax10kPoT",chPVeto,chEVeto);
      // }

      fHS->FillHisto2List("BhabhaList","h10nsWindowNPVetoClusterVsNEVetoClusterDeltaTWeight",chPVeto,chEVeto,tPVeto-tEVeto+100);
      fHS->FillHistoList("BhabhaList","hChToEnergySum10nsWindow",enBremElectron+enBremPositron);
      
      //either 2 or 3 hits?
      if((NHitsPVeto<2&&NHitsEVeto<2)) continue;
      if(!(NHitsPVeto<4&&NHitsEVeto<4)) continue;
            
      //      fHS->FillHisto2List("BhabhaList","h2to3Hits10nsWindowNPVetoClusterVsNEVetoCluster",chPVeto,chEVeto);
      //      fHS->FillHistoList("TimeCorrectionList","hdeltaTcorrect2to3HitsPVetoEVeto",deltaTcorrect);
      
      //minimum energy?
      if(!(enPVeto>1.6&&enEVeto>1.6)) continue;
      // fHS->FillHisto2List("BhabhaList","hEnergyCut2to3Hits10nsWindowNPVetoClusterVsNEVetoCluster",chPVeto,chEVeto);
      // fHS->FillHistoList("TimeCorrectionList","hdeltaTcorrectEnergyCut2to3HitsPVetoEVeto",deltaTcorrect);
      
      //19<ch<71?
      if(!(chPVeto>19&&chEVeto>19&&chPVeto<71&&chEVeto<71)) continue;
      fHS->FillHistoList("BhabhaList","hChToEnergySum10nsWindowGood",enBremElectron+enBremPositron);
      
      
      // fHS->FillHisto2List("BhabhaList","hGoodChasEnergyCut2to3Hits10nsWindowNPVetoClusterVsNEVetoCluster",chPVeto,chEVeto);
      // fHS->FillHistoList("TimeCorrectionList","hdeltaTcorrectGoodChasEnergyCut2to3HitsPVetoEVeto",deltaTcorrect);

      //Electron more energetic than positron?
      if(!(chEVeto>chPVeto)) continue;
      fHS->FillHistoList("BhabhaList","hChToEnergySum10nsWindowGoodMoreEnergeticElectron",enBremElectron+enBremPositron);
      // fHS->FillHisto2List("BhabhaList","hLeadingElectronGoodChasEnergyCut2to3Hits10nsWindowNPVetoClusterVsNEVetoCluster",chPVeto,chEVeto);
      // fHS->FillHistoList("TimeCorrectionList","hdeltaTcorrectMoreEnergeticElectronGoodChasEnergyCut2to3HitsPVetoEVeto",deltaTcorrect);*/
    }
  }

  //then do double loop to find couples:
  //3. do deltaTcorrection using plot from june
  //4. using deltaTcorrected look for the clusters in time

  return true;
}

Bool_t BhabhaAnalysis::Finalize()
{
  // TGraph* nPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vNPoT[0]);
  // fHS->SaveTGraphList("GGAnalysis","NPotVsTime",nPotVsTime);
  if(fVetoEndPoint)
    fVetoEndPoint->SaveHistos();
  else std::cout<<"fVetoEndPoint doesn't exist in BhabhaAnalysis::Finalize()"<<std::endl;

  std::cout<<"NSwum "<<NSwum<<std::endl;
  
  if (fVerbose) printf("---> Finalizing BhabhaAnalysis\n");
  return true;
}

