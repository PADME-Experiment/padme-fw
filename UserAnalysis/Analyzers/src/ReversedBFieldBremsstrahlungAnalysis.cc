// Written by M. Raggi   20/07/2021 
// Last modified by M. Raggi 12/08/2021
#include "ReversedBFieldBremsstrahlungAnalysis.hh"

ReversedBFieldBremsstrahlungAnalysis::ReversedBFieldBremsstrahlungAnalysis(TString cfgFile, Int_t verbose)
{
  //  Neve=0;
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating ReversedBFieldBremsstrahlungAnalysis\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  fHS = HistoSvc::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
}

ReversedBFieldBremsstrahlungAnalysis::~ReversedBFieldBremsstrahlungAnalysis(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t ReversedBFieldBremsstrahlungAnalysis::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing ReversedBFieldBremsstrahlungAnalysis\n");
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

Bool_t ReversedBFieldBremsstrahlungAnalysis::InitHistos(Bool_t isMC){

  // ReversedBFieldReversedBFieldBremsstrahlungAnalysis directory will contain all histograms related to this analysis
  fHS->CreateList("ReversedBFieldBremsstrahlungList");

  //Number of clusters
  fHS->BookHistoList("ReversedBFieldBremsstrahlungList","hNEVetoCluster",100,0,100);
  fHS->BookHistoList("ReversedBFieldBremsstrahlungList","hNSACCluster",100,0,100);

  //Cluster time
  if(isMC==false)  fHS->BookHistoList("ReversedBFieldBremsstrahlungList","htEVetoCluster",500,-250,250);
  else if(isMC==true)  fHS->BookHistoList("ReversedBFieldBremsstrahlungList","htEVetoCluster",750,0,750);
  fHS->BookHistoList("ReversedBFieldBremsstrahlungList","htSACCluster",500,-250,250);

  //No. hits per cluster
  fHS->BookHistoList("ReversedBFieldBremsstrahlungList","hNHitsEVetoCluster",15,0,15);
  fHS->BookHistoList("ReversedBFieldBremsstrahlungList","hNHitsSACCluster",15,0,15);

  //Cluster energy
  fHS->BookHistoList("ReversedBFieldBremsstrahlungList","hEnergyEVetoCluster",150,0,30);
  fHS->BookHistoList("ReversedBFieldBremsstrahlungList","h2nsWindowEnergyEVetoCluster",150,0,30);
  fHS->BookHistoList("ReversedBFieldBremsstrahlungList","hEnergySACCluster",7500,0,1500);

  //Cluster channel
  fHS->BookHistoList("ReversedBFieldBremsstrahlungList","hChEVetoCluster",96,0,96);
  fHS->BookHistoList("ReversedBFieldBremsstrahlungList","hChSACCluster",96,0,96);

  //Time correction
  fHS->BookHisto2List("ReversedBFieldBremsstrahlungList","hTimeCorrectionPerVetoChannel",90,0,90,300,-11,-8);
    
  //ReversedBFieldBremsstrahlung plots
  fHS->BookHistoList("ReversedBFieldBremsstrahlungList","h2nsWindowEEVetoPlusESac",750,0,750);
  fHS->BookHisto2List("ReversedBFieldBremsstrahlungList","h2nsWindowEnEVetoClusterVsEnSACCluster;enEVeto;enSAC",80,0,330,750,0,750);
  fHS->BookHisto2List("ReversedBFieldBremsstrahlungList","h2nsWindowEnSumVsDeltaT;EVetoEnergy + SACEnergy;DeltaT(EVeto-SAC)",750,0,750,400,-2,2);

  fHS->BookHistoList("ReversedBFieldBremsstrahlungList","hGoodChannels2nsWindowEEVetoPlusESac",750,0,750);
  fHS->BookHisto2List("ReversedBFieldBremsstrahlungList","hGoodChannels2nsWindowEnEVetoClusterVsEnSACCluster",51,55,260,750,0,750);
  fHS->BookHisto2List("ReversedBFieldBremsstrahlungList","hGoodChannel2nsWindowEnSumVsDeltaT;EVetoEnergy + SACEnergy;DeltaT(EVeto-SAC)",750,0,750,400,-2,2);

  fHS->BookHistoList("ReversedBFieldBremsstrahlungList","hPClusECutGoodChannels2nsWindowEEVetoPlusESac",750,0,750);
  fHS->BookHisto2List("ReversedBFieldBremsstrahlungList","hPClusECutGoodChannels2nsWindowEnEVetoClusterVsEnSACCluster",51,55,260,750,0,750);
  fHS->BookHisto2List("ReversedBFieldBremsstrahlungList","hPClusECutGoodChannel2nsWindowEnSumVsDeltaT;EVetoEnergy + SACEnergy;DeltaT(EVeto-SAC)",750,0,750,400,-2,2);

  fHS->BookHistoList("ReversedBFieldBremsstrahlungList","hSACClusECutPClusECutGoodChannels2nsWindowEEVetoPlusESac",750,0,750);
  fHS->BookHisto2List("ReversedBFieldBremsstrahlungList","hSACClusECutPClusECutGoodChannels2nsWindowEnEVetoClusterVsEnSACCluster",51,55,260,750,0,750);
  fHS->BookHisto2List("ReversedBFieldBremsstrahlungList","hSACClusECutPClusECutGoodChannel2nsWindowEnSumVsDeltaT;EVetoEnergy + SACEnergy;DeltaT(EVeto-SAC)",750,0,750,400,-2,2);

  fHS->BookHisto2List("ReversedBFieldBremsstrahlungList","hEVetoHitEnergyVsDeltaTEVetoSACCorrect",1000,0,10,1000,-5,5);
  fHS->BookHisto2List("ReversedBFieldBremsstrahlungList","hGoodChannelEVetoHitEnergyVsDeltaTEVetoSACCorrect",1000,0,10,1000,-5,5);
  
  return true;
}

Bool_t ReversedBFieldBremsstrahlungAnalysis::Process(){
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

  //Number of EVeto clusters
  Int_t NEVetoCluster = fEvent->EVetoRecoCl->GetNElements();
  fHS->FillHistoList("ReversedBFieldBremsstrahlungList","hNEVetoCluster",NEVetoCluster);

  //Number of EVeto hits
  Int_t NEVetoHit = fEvent->EVetoRecoEvent->GetNHits();
  fHS->FillHistoList("ReversedBFieldBremsstrahlungList","hNEVetoCluster",NEVetoCluster);
  
  //Number of SAC clusters
  if(!fEvent->SACRecoCl) return 0;
  Int_t NSACCluster = fEvent->SACRecoCl->GetNElements();
  fHS->FillHistoList("ReversedBFieldBremsstrahlungList","hNSACCluster",NSACCluster);

  //time
  double tEVeto;
  double tSAC;

  //channel
  int chEVeto;
  int chSAC;

  //no hits
  int NHitsEVeto;
  int NHitsSAC;

  //energy
  double enEVeto;
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
    fHS->FillHistoList("ReversedBFieldBremsstrahlungList","htSACCluster",tSAC);
    fHS->FillHistoList("ReversedBFieldBremsstrahlungList","hNHitsSACCluster",NHitsSAC);
    fHS->FillHistoList("ReversedBFieldBremsstrahlungList","hEnergySACCluster",enSAC);
    fHS->FillHistoList("ReversedBFieldBremsstrahlungList","hChSACCluster",chSAC);

    for(int jj = 0; jj<NEVetoCluster;jj++){

      //import EVeto variables
      tEVeto     =  fEvent->EVetoRecoCl->Element(jj)->GetTime();
      chEVeto    =  fEvent->EVetoRecoCl->Element(jj)->GetChannelId();
      NHitsEVeto =  fEvent->EVetoRecoCl->Element(jj)->GetNHitsInClus();
      enEVeto    =  fEvent->EVetoRecoCl->Element(jj)->GetEnergy();

      //histograms of raw variables
      //      fHS->FillHistoList("ReversedBFieldBremsstrahlungList","htEVetoCluster",tEVeto);
      fHS->FillHistoList("ReversedBFieldBremsstrahlungList","hNHitsEVetoCluster",NHitsEVeto);
      fHS->FillHistoList("ReversedBFieldBremsstrahlungList","hEnergyEVetoCluster",enEVeto);
      fHS->FillHistoList("ReversedBFieldBremsstrahlungList","hChEVetoCluster",chEVeto);

      timecorrection = 0.03594*(chEVeto)-11.52;//-0.37;//bring DeltaT(EVeto-SAC) for ReversedBFieldBremsstrahlung to 0, as it would be when they're produced - 0.37ns = difference in propogation time in vetoes vs SAC
      //within 2ns?
      if(!(std::fabs(tEVeto-tSAC-timecorrection)<2)) continue;

      fHS->FillHistoList("ReversedBFieldBremsstrahlungList","h2nsWindowEnergyEVetoCluster",enEVeto);
      
      //positron energy as function of channel, from Mauro MonteCarlo 22/4/21
      //enPositron = 20.15+1.094*chEVeto+0.0328*chEVeto*chEVeto;
      
      //from ReversedBFieldBremsstrahlung Swimmer on FullMC @ 2k PoT/bunch 14/1/23
      enPositron = 20.122+1.09974*chEVeto+0.034348*chEVeto*chEVeto;
      
      enSum = enSAC+enPositron;
      fHS->FillHistoList("ReversedBFieldBremsstrahlungList","h2nsWindowEEVetoPlusESac",enSum);
      fHS->FillHisto2List("ReversedBFieldBremsstrahlungList","h2nsWindowEnEVetoClusterVsEnSACCluster;enEVeto;enSAC",enPositron,enSAC);
      fHS->FillHisto2List("ReversedBFieldBremsstrahlungList","h2nsWindowEnSumVsDeltaT;EVetoEnergy + SACEnergy;DeltaT(EVeto-SAC)",enSum,tEVeto-tSAC-timecorrection);

      //good EVeto channels?
      if(!(chEVeto>19&&chEVeto<71)) continue;
      fHS->FillHistoList("ReversedBFieldBremsstrahlungList","hGoodChannels2nsWindowEEVetoPlusESac",enSum);
      fHS->FillHisto2List("ReversedBFieldBremsstrahlungList","hGoodChannels2nsWindowEnEVetoClusterVsEnSACCluster",enPositron,enSAC);
      fHS->FillHisto2List("ReversedBFieldBremsstrahlungList","hGoodChannel2nsWindowEnSumVsDeltaT;EVetoEnergy + SACEnergy;DeltaT(EVeto-SAC)",enSum,tEVeto-tSAC-timecorrection);

      //minimum EVeto cluster energy
      if(!(enEVeto>1.6)) continue;
      fHS->FillHistoList("ReversedBFieldBremsstrahlungList","hPClusECutGoodChannels2nsWindowEEVetoPlusESac",enSum);
      fHS->FillHisto2List("ReversedBFieldBremsstrahlungList","hPClusECutGoodChannels2nsWindowEnEVetoClusterVsEnSACCluster",enPositron,enSAC);
      fHS->FillHisto2List("ReversedBFieldBremsstrahlungList","hPClusECutGoodChannel2nsWindowEnSumVsDeltaT;EVetoEnergy + SACEnergy;DeltaT(EVeto-SAC)",enSum,tEVeto-tSAC-timecorrection);

      //minimum SAC cluster energy
      if(!(enSAC>150)) continue;
      fHS->FillHistoList("ReversedBFieldBremsstrahlungList","hSACClusECutPClusECutGoodChannels2nsWindowEEVetoPlusESac",enSum);
      fHS->FillHisto2List("ReversedBFieldBremsstrahlungList","hSACClusECutPClusECutGoodChannels2nsWindowEnEVetoClusterVsEnSACCluster",enPositron,enSAC);
      fHS->FillHisto2List("ReversedBFieldBremsstrahlungList","hSACClusECutPClusECutGoodChannel2nsWindowEnSumVsDeltaT;EVetoEnergy + SACEnergy;DeltaT(EVeto-SAC)",enSum,tEVeto-tSAC-timecorrection);
    }
  }
  
  double trajectorycorrection;
  double tHitEVeto;
  int chHitEVeto;
  double enHitEVeto;
  
  for(int ii = 0; ii<NSACCluster; ii++){
    
    //import SAC variables
    tSAC     =  fEvent->SACRecoCl->Element(ii)->GetTime();
    chSAC    =  fEvent->SACRecoCl->Element(ii)->GetChannelId();
    NHitsSAC =  fEvent->SACRecoCl->Element(ii)->GetNHitsInClus();
    enSAC    =  fEvent->SACRecoCl->Element(ii)->GetEnergy();

    if(chSAC!=22) continue;
    
    //histograms of raw variables
    fHS->FillHistoList("ReversedBFieldBremsstrahlungList","htSACCluster",tSAC);
    fHS->FillHistoList("ReversedBFieldBremsstrahlungList","hNHitsSACCluster",NHitsSAC);
    fHS->FillHistoList("ReversedBFieldBremsstrahlungList","hEnergySACCluster",enSAC);
    fHS->FillHistoList("ReversedBFieldBremsstrahlungList","hChSACCluster",chSAC);

    for(int jj = 0; jj<NEVetoHit;jj++){

      //import EVeto variables
      tHitEVeto     =  fEvent->EVetoRecoEvent->Hit(jj)->GetTime();
      chHitEVeto    =  fEvent->EVetoRecoEvent->Hit(jj)->GetChannelId();
      enHitEVeto    =  fEvent->EVetoRecoEvent->Hit(jj)->GetEnergy();

      //from MC: Thursday 18th August 2022
      //trajectorycorrection = -11.435+0.034498*chHitEVeto;

      //analytically using geometry & excel fit (PathDifferenceinEVetoChannel.exe), still Thursday 18th August 2022
      //trajectorycorrection = -10.351+0.036443*chHitEVeto;

      timecorrection = 0.03594*(chHitEVeto)-11.52-0.37;//bring DeltaT(EVeto-SAC) for ReversedBFieldBremsstrahlung to 0, as it would be when they're produced - 0.37ns = difference in propogation time in vetoes vs SAC
      fHS->FillHisto2List("ReversedBFieldBremsstrahlungList","hTimeCorrectionPerVetoChannel",chHitEVeto,timecorrection);
      fHS->FillHisto2List("ReversedBFieldBremsstrahlungList","hEVetoHitEnergyVsDeltaTEVetoSACCorrect",enHitEVeto,(tHitEVeto-tSAC-timecorrection));
      if(chHitEVeto>19&&chHitEVeto<71)      fHS->FillHisto2List("ReversedBFieldBremsstrahlungList","hGoodChannelEVetoHitEnergyVsDeltaTEVetoSACCorrect",enHitEVeto,(tHitEVeto-tSAC-timecorrection));
      
      //within 2ns?
      if(!(std::fabs(tHitEVeto-tSAC-trajectorycorrection)<2)) continue;

    }
  }

  
  return true;
}

Bool_t ReversedBFieldBremsstrahlungAnalysis::Finalize()
{
 // TGraph* nPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vNPoT[0]);
 // fHS->SaveTGraphList("GGAnalysis","NPotVsTime",nPotVsTime);
 
 if (fVerbose) printf("---> Finalizing ReversedBFieldBremsstrahlungAnalysis\n");
  return true;
}

