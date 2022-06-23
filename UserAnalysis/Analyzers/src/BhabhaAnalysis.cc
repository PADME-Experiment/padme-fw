// Written by M. Raggi   20/07/2021 
// Last modified by M. Raggi 12/08/2021
#include "BhabhaAnalysis.hh"

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

  //Number of clusters
  fHS->BookHistoList("PVetoClusters","hNPVetoCluster",100,0,100);
  fHS->BookHistoList("EVetoClusters","hNEVetoCluster",100,0,100);

  //Cluster time
  fHS->BookHistoList("PVetoClusters","htPVetoCluster",500,-250,250);
  fHS->BookHistoList("EVetoClusters","htEVetoCluster",500,-250,250);

  //Time difference
  fHS->BookHistoList("BhabhaList","hDeltatPVetoEVetoCluster",1000,-500,500);
  fHS->BookHistoList("BhabhaList","h2PlusHitsDeltatPVetoEVetoCluster",1000,-500,500);
  fHS->BookHistoList("BhabhaList","h2to3HitsDeltatPVetoEVetoCluster",1000,-500,500);
  fHS->BookHistoList("BhabhaList","hEnergyCut2to3HitsDeltatPVetoEVetoCluster",1000,-500,500);

  //No. hits per cluster
  fHS->BookHistoList("PVetoClusters","hNHitsPVetoCluster",15,0,15);
  fHS->BookHistoList("EVetoClusters","hNHitsEVetoCluster",15,0,15);

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
  fHS->BookHisto2List("BhabhaList","hDiffEBeamChToEnergySumVsDeltaChPVetoEVeto",192,-96,96,200,-320,430);

  fHS->BookHistoList("BhabhaList","hChToEnergySum5nsWindow",200,0,750);
  fHS->BookHisto2List("BhabhaList","hDiffEBeamChToEnergySumVsDeltaChPVetoEVeto5nsWindow",192,-96,96,200,-320,430);

  fHS->BookHistoList("BhabhaList","hChToEnergySum5nsWindowGood",200,0,750);
  fHS->BookHisto2List("BhabhaList","hDiffEBeamChToEnergySumVsDeltaChPVetoEVeto5nsWindowGood",192,-96,96,200,-320,430);

  fHS->BookHistoList("BhabhaList","hChToEnergySum5nsWindowGoodMoreEnergeticElectron",200,0,750);
  fHS->BookHisto2List("BhabhaList","hDiffEBeamChToEnergySumVsDeltaChPVetoEVeto5nsWindowGoodMoreEnergeticElectron",192,-96,96,200,-320,430);

  //Bhabha plots
  fHS->BookHisto2List("BhabhaList","h5nsWindowNPVetoClusterVsNEVetoCluster",96,0,95,96,0,95);
  fHS->BookHisto2List("BhabhaList","h2to3Hits5nsWindowNPVetoClusterVsNEVetoCluster",96,0,95,96,0,95);
  fHS->BookHisto2List("BhabhaList","hEnergyCut2to3Hits5nsWindowNPVetoClusterVsNEVetoCluster",96,0,95,96,0,95);
  //fHS->BookHisto2List("BhabhaList","h0.6xHitsEnergyCut2to3Hits5nsWindowNPVetoClusterVsNEVetoCluster",96,0,95,96,0,95);

  return true;
}

Bool_t BhabhaAnalysis::Process(){
  
  //Check if is MC or data
  Bool_t isMC = false;
  if (fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) {
    isMC=true;
    //    std::cout<<"input data are simulatetd "<<std::endl;
  }
  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  fHS->FillHistoList("TOP","TriggerMaskBhabha",trigMask);

  //Cut on physics trigger Data Only
  if( !(trigMask & (1 << 0)) && isMC==false) return false;

  //Number of PVeto clusters
  Int_t NPVetoCluster = fEvent->PVetoRecoCl->GetNElements();
  fHS->FillHistoList("PVetoClusters","hNPVetoCluster",NPVetoCluster);

  //Number of EVeto clusters
  Int_t NEVetoCluster = fEvent->EVetoRecoCl->GetNElements();
  fHS->FillHistoList("EVetoClusters","hNEVetoCluster",NEVetoCluster);

  //Containers for variables of "good" clusters
  //time
  std::vector<double> tPVetoGood;
  std::vector<double> tEVetoGood;

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

    //good cluster cuts:
    //>=2 hits
    if(tempNHit<2) continue;
    //energy per hit > 0.7
    if(!(tempEClus/tempNHit)>0.7) continue;

    //fill vectors with parameters of good hits
    NHitsEVetoGood.push_back(tempNHit);
    enEVetoGood.push_back(tempEClus);
    tEVetoGood.push_back(tempT);
    chEVetoGood.push_back(tempCh);
  }

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

    //good cluster cuts:
    //>=2 hits
    if(tempNHit<2) continue;
    //energy per hit > 0.7
    if(!(tempEClus/tempNHit)>0.7) continue;

    //fill vectors with parameters of good hits
    NHitsPVetoGood.push_back(tempNHit);
    enPVetoGood.push_back(tempEClus);
    tPVetoGood.push_back(tempT);
    chPVetoGood.push_back(tempCh);
  }

  //time
  double tPVeto;
  double tEVeto;

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

  //loop over good clusters
  for(int ii = 0; ii<NHitsEVetoGood.size(); ii++){

    //import EVeto variables
    tEVeto     =  tEVetoGood[ii];
    chEVeto    =  chEVetoGood[ii];
    NHitsEVeto =  NHitsEVetoGood[ii];
    enEVeto    =  enEVetoGood[ii];

    for(int ii = 0; ii<NHitsPVetoGood.size(); ii++){
      
      //import PVeto variables
      tPVeto     =  tPVetoGood[ii];
      chPVeto    =  chPVetoGood[ii];
      NHitsPVeto =  NHitsPVetoGood[ii];
      enPVeto    =  enPVetoGood[ii];

      //positron energy as function of channel, assuming no angle to beam (eg Brem), from Mauro MonteCarlo 22/4/21
      enBremPositron = 20.15+1.094*chPVeto+0.0328*chPVeto*chPVeto;
      enBremElectron = 20.15+1.094*chEVeto+0.0328*chEVeto*chEVeto;

      //histograms of raw variables
      fHS->FillHistoList("BhabhaList","hDeltatPVetoEVetoCluster",tPVeto-tEVeto);
      fHS->FillHistoList("PVetoClusters","hChToEnergyPositron",enBremPositron);
      fHS->FillHistoList("EVetoClusters","hChToEnergyElectron",enBremElectron);

      //sum of e+ e- energies as reconstructed using Bremsstrahlung relation
      fHS->FillHistoList("BhabhaList","hChToEnergySum",enBremElectron+enBremPositron);
      fHS->FillHisto2List("BhabhaList","hDiffEBeamChToEnergySumVsDeltaChPVetoEVeto",chPVeto-chEVeto,430-(enBremElectron+enBremPositron));

      if(NHitsPVeto>1&&NHitsEVeto>1){
	fHS->FillHistoList("BhabhaList","h2PlusHitsDeltatPVetoEVetoCluster",tPVeto-tEVeto);
	if(NHitsPVeto<4&&NHitsEVeto<4){
	  fHS->FillHistoList("BhabhaList","h2to3HitsDeltatPVetoEVetoCluster",tPVeto-tEVeto);
	  if(enPVeto>1.6&&enEVeto>1.6){
	    fHS->FillHistoList("BhabhaList","hEnergyCut2to3HitsDeltatPVetoEVetoCluster",tPVeto-tEVeto);		
	  }
	}
      }

      //within 5ns?
      if(!(std::fabs(tPVeto-tEVeto)<5)) continue;
      fHS->FillHisto2List("BhabhaList","h5nsWindowNPVetoClusterVsNEVetoCluster",chPVeto,chEVeto);
      fHS->FillHistoList("BhabhaList","hChToEnergySum5nsWindow",enBremElectron+enBremPositron);
      fHS->FillHisto2List("BhabhaList","hDiffEBeamChToEnergySumVsDeltaChPVetoEVeto5nsWindow",chPVeto-chEVeto,430-(enBremElectron+enBremPositron));
  
      //either 2 or 3 hits?
      if(!(NHitsPVeto<4&&NHitsEVeto<4)) continue;
      fHS->FillHisto2List("BhabhaList","h2to3Hits5nsWindowNPVetoClusterVsNEVetoCluster",chPVeto,chEVeto);

      //minimum energy?
      if(!(enPVeto>1.6&&enEVeto>1.6)) continue;
      fHS->FillHisto2List("BhabhaList","hEnergyCut2to3Hits5nsWindowNPVetoClusterVsNEVetoCluster",chPVeto,chEVeto);

      //19<ch<71?
      if(!(chPVeto>19&&chEVeto>19&&chPVeto<71&&chEVeto<71)) continue;
      fHS->FillHistoList("BhabhaList","hChToEnergySum5nsWindowGood",enBremElectron+enBremPositron);
      fHS->FillHisto2List("BhabhaList","hDiffEBeamChToEnergySumVsDeltaChPVetoEVeto5nsWindowGood",chPVeto-chEVeto,430-(enBremElectron+enBremPositron));

      //Electron more energetic than positron?
      if(!(chEVeto>chPVeto)) continue;
      fHS->FillHistoList("BhabhaList","hChToEnergySum5nsWindowGoodMoreEnergeticElectron",enBremElectron+enBremPositron);
      fHS->FillHisto2List("BhabhaList","hDiffEBeamChToEnergySumVsDeltaChPVetoEVeto5nsWindowGoodMoreEnergeticElectron",chPVeto-chEVeto,430-(enBremElectron+enBremPositron));
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
 
 if (fVerbose) printf("---> Finalizing BhabhaAnalysis\n");
  return true;
}

