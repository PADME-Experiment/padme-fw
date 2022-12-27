// Written by M. Raggi   20/07/2021 
// Last modified by M. Raggi 12/08/2021
#include "BremsstrahlungAnalysis.hh"

BremsstrahlungAnalysis::BremsstrahlungAnalysis(TString cfgFile, Int_t verbose)
{
  //  Neve=0;
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating BremsstrahlungAnalysis\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  fHS = HistoSvc::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
}

BremsstrahlungAnalysis::~BremsstrahlungAnalysis(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t BremsstrahlungAnalysis::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing BremsstrahlungAnalysis\n");
  fEvent = event;
  Bool_t isMC = false;
  std::cout<<"before "<<fEvent->MCTruthEvent<<std::endl;
  if (fEvent->MCTruthEvent){
    isMC=true;
    std::cout<<"input data are simulatetd "<<std::endl;
  }
  InitHistos(isMC);
  return true;
}

Bool_t BremsstrahlungAnalysis::InitHistos(Bool_t isMC){

  // BremsstrahlungAnalysis directory will contain all histograms related to this analysis
  fHS->CreateList("BremsstrahlungList");

  //Number of clusters
  fHS->BookHistoList("BremsstrahlungList","hNPVetoCluster",100,0,100);
  fHS->BookHistoList("BremsstrahlungList","hNSACCluster",100,0,100);

  //Cluster time
  if(isMC=false)  fHS->BookHistoList("BremsstrahlungList","htPVetoCluster",500,-250,250);
  else if(isMC=true)  fHS->BookHistoList("BremsstrahlungList","htPVetoCluster",500,0,500);
  fHS->BookHistoList("BremsstrahlungList","htSACCluster",500,-250,250);

  //No. hits per cluster
  fHS->BookHistoList("BremsstrahlungList","hNHitsPVetoCluster",15,0,15);
  fHS->BookHistoList("BremsstrahlungList","hNHitsSACCluster",15,0,15);

  //Cluster energy
  fHS->BookHistoList("BremsstrahlungList","hEnergyPVetoCluster",150,0,30);
  fHS->BookHistoList("BremsstrahlungList","hEnergySACCluster",7500,0,1500);

  //Cluster channel
  fHS->BookHistoList("BremsstrahlungList","hChPVetoCluster",96,0,96);
  fHS->BookHistoList("BremsstrahlungList","hChSACCluster",96,0,96);

  //Time correction
  fHS->BookHisto2List("BremsstrahlungList","hTimeCorrectionPerVetoChannel",90,0,90,300,-11,-8);
    
  //Bremsstrahlung plots
  fHS->BookHistoList("BremsstrahlungList","h2nsWindowEPVetoPlusESac",500,0,500);
  fHS->BookHisto2List("BremsstrahlungList","h2nsWindowNPVetoClusterVsNSACCluster",80,0,330,500,0,500);

  fHS->BookHistoList("BremsstrahlungList","hGoodChannels2nsWindowEPVetoPlusESac",500,0,500);
  fHS->BookHisto2List("BremsstrahlungList","hGoodChannels2nsWindowNPVetoClusterVsNSACCluster",51,55,260,500,0,500);

  fHS->BookHistoList("BremsstrahlungList","hPClusECutGoodChannels2nsWindowEPVetoPlusESac",500,0,500);
  fHS->BookHisto2List("BremsstrahlungList","hPClusECutGoodChannels2nsWindowNPVetoClusterVsNSACCluster",51,55,260,500,0,500);

  fHS->BookHistoList("BremsstrahlungList","hSACClusECutPClusECutGoodChannels2nsWindowEPVetoPlusESac",350,150,500);
  fHS->BookHisto2List("BremsstrahlungList","hSACClusECutPClusECutGoodChannels2nsWindowNPVetoClusterVsNSACCluster",51,55,260,350,150,500);

  fHS->BookHisto2List("BremsstrahlungList","hPVetoHitEnergyVsDeltaTPVetoSACCorrect",100,0,10,100,-5,5);
  
  return true;
}

Bool_t BremsstrahlungAnalysis::Process(){
  //  std::cout<<"processing brem"<<std::endl;
  //Check if is MC or data
  Bool_t isMC = false;
  if (fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) {
    isMC=true;
    //    std::cout<<"input data are simulatetd "<<std::endl;
  }
  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();

  //Cut on physics trigger Data Only
  if( !(trigMask & (1 << 0)) && isMC==false) return false;

  //Number of PVeto clusters
  Int_t NPVetoCluster = fEvent->PVetoRecoCl->GetNElements();
  fHS->FillHistoList("BremsstrahlungList","hNPVetoCluster",NPVetoCluster);

  //Number of PVeto hits
  Int_t NPVetoHit = fEvent->PVetoRecoEvent->GetNHits();
  fHS->FillHistoList("BremsstrahlungList","hNPVetoCluster",NPVetoCluster);
  
  //Number of SAC clusters
  if(!fEvent->SACRecoCl) return 0;
  Int_t NSACCluster = fEvent->SACRecoCl->GetNElements();
  fHS->FillHistoList("BremsstrahlungList","hNSACCluster",NSACCluster);

  //time
  double tPVeto;
  double tSAC;

  //channel
  int chPVeto;
  int chSAC;

  //no hits
  int NHitsPVeto;
  int NHitsSAC;

  //energy
  double enPVeto;
  double enSAC;

  //energy sum
  double enPositron;
  double enSum;

  double timecorrection;//-3.67; //3.67 comes from the difference between the raw peak time (used for FedeO's T0s) and the derivative peak time (used for Beth's T0s)

  for(int ii = 0; ii<NSACCluster; ii++){

    //import SAC variables
    tSAC     =  fEvent->SACRecoCl->Element(ii)->GetTime();
    chSAC    =  fEvent->SACRecoCl->Element(ii)->GetChannelId();
    NHitsSAC =  fEvent->SACRecoCl->Element(ii)->GetNHitsInClus();
    enSAC    =  fEvent->SACRecoCl->Element(ii)->GetEnergy();

    //histograms of raw variables
    fHS->FillHistoList("BremsstrahlungList","htSACCluster",tSAC);
    fHS->FillHistoList("BremsstrahlungList","hNHitsSACCluster",NHitsSAC);
    fHS->FillHistoList("BremsstrahlungList","hEnergySACCluster",enSAC);
    fHS->FillHistoList("BremsstrahlungList","hChSACCluster",chSAC);

    for(int jj = 0; jj<NPVetoCluster;jj++){

      //import PVeto variables
      tPVeto     =  fEvent->PVetoRecoCl->Element(jj)->GetTime();
      chPVeto    =  fEvent->PVetoRecoCl->Element(jj)->GetChannelId();
      NHitsPVeto =  fEvent->PVetoRecoCl->Element(jj)->GetNHitsInClus();
      enPVeto    =  fEvent->PVetoRecoCl->Element(jj)->GetEnergy();

      timecorrection = 0.03594*(chPVeto)-11.52;//bring DeltaT(PVeto-SAC) for Bremsstrahlung to 0, as it would be when they're produced
      fHS->FillHisto2List("BremsstrahlungList","hTimeCorrectionPerVetoChannel",chPVeto,timecorrection);
      fHS->FillHisto2List("BremsstrahlungList","hPVetoHitEnergyVsDeltaTPVetoSACCorrect",enPVeto,(tPVeto-tSAC-timecorrection));
      
      //histograms of raw variables
      //      fHS->FillHistoList("BremsstrahlungList","htPVetoCluster",tPVeto);
      fHS->FillHistoList("BremsstrahlungList","hNHitsPVetoCluster",NHitsPVeto);
      fHS->FillHistoList("BremsstrahlungList","hEnergyPVetoCluster",enPVeto);
      fHS->FillHistoList("BremsstrahlungList","hChPVetoCluster",chPVeto);

      //within 2ns?
      if(!(std::fabs(tPVeto-tSAC-timecorrection)<2)) continue;

      //positron energy as function of channel, from Mauro MonteCarlo 22/4/21
      enPositron = 20.15+1.094*chPVeto+0.0328*chPVeto*chPVeto;
      enSum = enSAC+enPositron;
      fHS->FillHistoList("BremsstrahlungList","h2nsWindowEPVetoPlusESac",enSum);
      fHS->FillHisto2List("BremsstrahlungList","h2nsWindowNPVetoClusterVsNSACCluster",enPositron,enSAC);

      //good PVeto channels?
      if(!(chPVeto>19&&chPVeto<71)) continue;
      fHS->FillHistoList("BremsstrahlungList","hGoodChannels2nsWindowEPVetoPlusESac",enSum);
      fHS->FillHisto2List("BremsstrahlungList","hGoodChannels2nsWindowNPVetoClusterVsNSACCluster",enPositron,enSAC);

      //minimum PVeto cluster energy
      if(!(enPVeto>1.6)) continue;
      fHS->FillHistoList("BremsstrahlungList","hPClusECutGoodChannels2nsWindowEPVetoPlusESac",enSum);
      fHS->FillHisto2List("BremsstrahlungList","hPClusECutGoodChannels2nsWindowNPVetoClusterVsNSACCluster",enPositron,enSAC);

      //minimum SAC cluster energy
      if(!(enSAC>150)) continue;
      fHS->FillHistoList("BremsstrahlungList","hSACClusECutPClusECutGoodChannels2nsWindowEPVetoPlusESac",enSum);
      fHS->FillHisto2List("BremsstrahlungList","hSACClusECutPClusECutGoodChannels2nsWindowNPVetoClusterVsNSACCluster",enPositron,enSAC);

    }
  }
  
  double trajectorycorrection;
  double tHitPVeto;
  int chHitPVeto;
  double enHitPVeto;
  
  for(int ii = 0; ii<NSACCluster; ii++){
    
    //import SAC variables
    tSAC     =  fEvent->SACRecoCl->Element(ii)->GetTime();
    chSAC    =  fEvent->SACRecoCl->Element(ii)->GetChannelId();
    NHitsSAC =  fEvent->SACRecoCl->Element(ii)->GetNHitsInClus();
    enSAC    =  fEvent->SACRecoCl->Element(ii)->GetEnergy();

    //histograms of raw variables
    fHS->FillHistoList("BremsstrahlungList","htSACCluster",tSAC);
    fHS->FillHistoList("BremsstrahlungList","hNHitsSACCluster",NHitsSAC);
    fHS->FillHistoList("BremsstrahlungList","hEnergySACCluster",enSAC);
    fHS->FillHistoList("BremsstrahlungList","hChSACCluster",chSAC);

    for(int jj = 0; jj<NPVetoHit;jj++){

      //import PVeto variables
      tHitPVeto     =  fEvent->PVetoRecoEvent->Hit(jj)->GetTime();
      chHitPVeto    =  fEvent->PVetoRecoEvent->Hit(jj)->GetChannelId();
      enHitPVeto    =  fEvent->PVetoRecoEvent->Hit(jj)->GetEnergy();

      //from MC: Thursday 18th August 2022
      //trajectorycorrection = -11.435+0.034498*chHitPVeto;

      //analytically using geometry & excel fit (PathDifferenceinPVetoChannel.exe), still Thursday 18th August 2022
      //trajectorycorrection = -10.351+0.036443*chHitPVeto;
      
      //within 2ns?
      if(!(std::fabs(tHitPVeto-tSAC-trajectorycorrection)<2)) continue;

    }
  }

  
  return true;
}

Bool_t BremsstrahlungAnalysis::Finalize()
{
 // TGraph* nPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vNPoT[0]);
 // fHS->SaveTGraphList("GGAnalysis","NPotVsTime",nPotVsTime);
 
 if (fVerbose) printf("---> Finalizing BremsstrahlungAnalysis\n");
  return true;
}

