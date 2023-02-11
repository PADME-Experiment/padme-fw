// Written by Beth Long 13/6/22
#include "T0sAnalysis.hh"
#include "TRecoVHit.hh"
#include "VetoEndPoint.hh"
#include "TLorentzVector.h"
#include "PadmeVRecoConfig.hh"

T0sAnalysis::T0sAnalysis(TString cfgFile, Int_t verbose)
{
  //  Neve=0;
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating T0sAnalysis\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  fHS = HistoSvc::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
  fConfig = new PadmeVRecoConfig(fCfgParser,"PadmeReconstructionConfiguration");

  fSwimBrem = fConfig->GetParOrDefault("T0sAnalysis","SwimBrem",1.);
  fVetoEndPoint = VetoEndPoint::GetInstance();
}

T0sAnalysis::~T0sAnalysis(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t T0sAnalysis::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing T0sAnalysis\n");
  fEvent = event;
  InitHistos();
  fSwimmerInit=0;
  return true;
}

Bool_t T0sAnalysis::InitHistos(){
  // T0sAnalysis directory will contain all histograms related to this analysis
  //  fHS->CreateList("PVetoHits");
  //  fHS->CreateList("EVetoHits");

  fHS->CreateList("PVetoSACT0sList/StandardRecoClus");
  fHS->CreateList("EVetoSACT0sList/StandardRecoClus");

  fHS->CreateList("PVetoSACT0sList/StandardRecoHits");
  fHS->CreateList("EVetoSACT0sList/StandardRecoHits");

  fHS->CreateList("PVetoSACT0sList/SwimmerFrontFace");
  fHS->CreateList("EVetoSACT0sList/SwimmerFrontFace");

  fHS->CreateList("PVetoSACT0sList/SwimmerSACSiPM");
  fHS->CreateList("EVetoSACT0sList/SwimmerSACSiPM");

  fHS->CreateList("PVetoSACT0sList/StandardRecoClusWithT0s");
  fHS->CreateList("EVetoSACT0sList/StandardRecoClusWithT0s");
  
  fHS->CreateList("PVetoAdjChaT0sList");
  fHS->CreateList("EVetoAdjChaT0sList");
  
  fHS->CreateList("SwumEnergyToChannel");
  
  //Time difference PVetoClusters wrt SAC
  fHS->BookHistoList("PVetoSACT0sList/StandardRecoClus","hDeltatPVetoSACCluster",4000,-500,500);
  fHS->BookHistoList("PVetoSACT0sList/StandardRecoClus","hDeltatPVetoSACClusterGoodPVetoCh",4000,-500,500);
  fHS->BookHistoList("PVetoSACT0sList/StandardRecoClus","hDeltatPVetoSAC22ClusterGoodPVetoCh",4000,-500,500);

  //Time difference PVetoClusters wrt SAC with Brem T0s
  fHS->BookHistoList("PVetoSACT0sList/StandardRecoClusWithT0s","hDeltatPVetoSACCluster",4000,-500,500);
  fHS->BookHistoList("PVetoSACT0sList/StandardRecoClusWithT0s","hDeltatPVetoSACClusterGoodPVetoCh",4000,-500,500);
  fHS->BookHistoList("PVetoSACT0sList/StandardRecoClusWithT0s","hDeltatPVetoSAC22ClusterGoodPVetoCh",4000,-500,500);

  //Time difference EVetoClusters wrt SAC
  fHS->BookHistoList("EVetoSACT0sList/StandardRecoClus","hDeltatEVetoSACCluster",4000,-500,500);
  fHS->BookHistoList("EVetoSACT0sList/StandardRecoClus","hDeltatEVetoSACClusterGoodEVetoCh",4000,-500,500);
  fHS->BookHistoList("EVetoSACT0sList/StandardRecoClus","hDeltatEVetoSAC22ClusterGoodEVetoCh",4000,-500,500);

  //Time difference EVetoClusters wrt SAC with Brem T0s
  fHS->BookHistoList("EVetoSACT0sList/StandardRecoClusWithT0s","hDeltatEVetoSACCluster",4000,-500,500);
  fHS->BookHistoList("EVetoSACT0sList/StandardRecoClusWithT0s","hDeltatEVetoSACClusterGoodEVetoCh",4000,-500,500);
  fHS->BookHistoList("EVetoSACT0sList/StandardRecoClusWithT0s","hDeltatEVetoSAC22ClusterGoodEVetoCh",4000,-500,500);

  //Time difference PVetoHits wrt SAC
  fHS->BookHistoList("PVetoSACT0sList/StandardRecoHits","hDeltatPVetoHitsSACCluster",4000,-500,500);
  fHS->BookHistoList("PVetoSACT0sList/StandardRecoHits","hDeltatPVetoHitsSACClusterGoodPVetoCh",4000,-500,500);
  fHS->BookHistoList("PVetoSACT0sList/StandardRecoHits","hDeltatPVetoHitsSAC22ClusterGoodPVetoCh",4000,-500,500);
  
  fHS->BookHistoList("EVetoSACT0sList/StandardRecoHits","hDeltatEVetoHitsSACCluster",4000,-500,500);
  fHS->BookHistoList("EVetoSACT0sList/StandardRecoHits","hDeltatEVetoHitsSACClusterGoodEVetoCh",4000,-500,500);
  fHS->BookHistoList("EVetoSACT0sList/StandardRecoHits","hDeltatEVetoHitsSAC22ClusterGoodEVetoCh",4000,-500,500);

  //Hit properties
  // fHS->BookHistoList("PVetoHits","hPVetoHitEnergy",600,0,6);
  // fHS->BookHistoList("EVetoHits","hEVetoHitEnergy",600,0,6);
  // fHS->BookHistoList("PVetoHits","hPVetoHitCh",90,0,90);
  // fHS->BookHistoList("EVetoHits","hEVetoHitCh",96,0,96);
  // fHS->BookHisto2List("PVetoHits","hPVetoHitChVsHitEnergy",90,0,90,600,0,6);
  // fHS->BookHisto2List("EVetoHits","hEVetoHitChVsHitEnergy",96,0,96,600,0,6);
  
  //Time difference wrt previous channel
  fHS->BookHistoList("PVetoAdjChaT0sList","hDeltatPVetoCh_iCh_i--",80,-5,5);
  fHS->BookHistoList("EVetoAdjChaT0sList","hDeltatEVetoCh_iCh_i--",80,-5,5);
  fHS->BookHistoList("PVetoAdjChaT0sList","hDeltatPVetoCh_iCh_i--GoodPVetoCh",80,-5,5);
  fHS->BookHistoList("EVetoAdjChaT0sList","hDeltatEVetoCh_iCh_i--GoodEVetoCh",80,-5,5);

  //Collinear particles channelID->energy
  fHS->BookHisto2List("SwumEnergyToChannel","hSwumEnergyToChannelPVeto",90,0,90,215,0,430);
  fHS->BookHisto2List("SwumEnergyToChannel","hSwumEnergyToChannelEVeto",96,0,96,215,0,430);
  
  //Individual channel times
  for(int ch=0;ch<90;ch++){
    sprintf(name,"hDeltatPVetoSAC22ClusterCh%i",ch);
    fHS->BookHistoList("PVetoSACT0sList/StandardRecoClus",name,450,-45,45);

    sprintf(name,"hDeltatPVetoSAC22ClusterCh%i",ch);
    fHS->BookHistoList("PVetoSACT0sList/StandardRecoClusWithT0s",name,450,-45,45);
  
    sprintf(name,"hDeltatPVetoHitsSAC22Ch%i",ch);
    fHS->BookHistoList("PVetoSACT0sList/StandardRecoHits",name,450,-45,45);

    sprintf(name,"hDeltatSwumPVetoCh%iSACFrontFace",ch);
    fHS->BookHistoList("PVetoSACT0sList/SwimmerFrontFace",name,450,-45,45);

    sprintf(name,"hDeltatSwumPVetoCh%iSACSiPM",ch);
    fHS->BookHistoList("PVetoSACT0sList/SwimmerSACSiPM",name,450,-45,45);
    
    if(ch>0){
      sprintf(name,"hDeltatPVetoCh%iCh%i",ch,ch-1);
      fHS->BookHistoList("PVetoAdjChaT0sList",name,80,-5,5);
    }
  }
  for(int ch=0;ch<96;ch++){
    sprintf(name,"hDeltatEVetoSAC22ClusterCh%i",ch);
    fHS->BookHistoList("EVetoSACT0sList/StandardRecoClus",name,450,-45,45);

    sprintf(name,"hDeltatEVetoSAC22ClusterCh%i",ch);
    fHS->BookHistoList("EVetoSACT0sList/StandardRecoClusWithT0s",name,450,-45,45);

    sprintf(name,"hDeltatEVetoHitsSAC22Ch%i",ch);
    fHS->BookHistoList("EVetoSACT0sList/StandardRecoHits",name,450,-45,45);

    sprintf(name,"hDeltatSwumEVetoCh%iSACFrontFace",ch);
    fHS->BookHistoList("EVetoSACT0sList/SwimmerFrontFace",name,450,-45,45);

    sprintf(name,"hDeltatSwumEVetoCh%iSACSiPM",ch);
    fHS->BookHistoList("EVetoSACT0sList/SwimmerSACSiPM",name,450,-45,45);
    
    if(ch>0){
      sprintf(name,"hDeltatEVetoCh%iCh%i",ch,ch-1);
      fHS->BookHistoList("EVetoAdjChaT0sList",name,80,-5,5);
    }
  }
 
  return true;
}

Bool_t T0sAnalysis::Process(){
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

  //Number of EVeto clusters
  Int_t NEVetoCluster = fEvent->EVetoRecoCl->GetNElements();

  //Number of SAC clusters
  Int_t NSACCluster;
  if((fEvent->SACRecoCl)!=NULL)
    NSACCluster = fEvent->SACRecoCl->GetNElements();
  else NSACCluster=0;

  //Number of PVeto hits per event
  Int_t NPVetoHit = fEvent->PVetoRecoEvent->GetNHits();

  //Number of EVeto hits per event
  Int_t NEVetoHit = fEvent->EVetoRecoEvent->GetNHits();
  
  //time
  double tPVeto;
  double tHitPVeto;
  double tEVeto;
  double tHitEVeto;
  double tSAC;

  //channel
  int chPVeto;
  int chHitPVeto;
  int chEVeto;
  int chHitEVeto;
  int chSAC;

  //no hits in cluster
  int NClusterHitsPVeto;
  int NClusterHitsEVeto;
  int NClusterHitsSAC;

  //energy
  double enPVeto;
  double enHitPVeto;
  double enEVeto;
  double enHitEVeto;
  double enSAC;

  //energy sum
  double enPositron;
  double enSum;

  double timecorrection;

  //Vetos-SAC difference
  for(int ii = 0; ii<NSACCluster; ii++){
    if(!fEvent->SACRecoCl) break;

    //import SAC variables
    tSAC     =  fEvent->SACRecoCl->Element(ii)->GetTime();
    chSAC    =  fEvent->SACRecoCl->Element(ii)->GetChannelId();
    NClusterHitsSAC =  fEvent->SACRecoCl->Element(ii)->GetNHitsInClus();
    enSAC    =  fEvent->SACRecoCl->Element(ii)->GetEnergy();

    for(int jj = 0; jj<NPVetoCluster;jj++){

      //import PVeto variables
      tPVeto     =  fEvent->PVetoRecoCl->Element(jj)->GetTime();
      chPVeto    =  fEvent->PVetoRecoCl->Element(jj)->GetChannelId();
      NClusterHitsPVeto =  fEvent->PVetoRecoCl->Element(jj)->GetNHitsInClus();
      enPVeto    =  fEvent->PVetoRecoCl->Element(jj)->GetEnergy();

      timecorrection = 0.03594*(chPVeto)-11.52-0.37;//bring DeltaT(PVeto-SAC) for Bremsstrahlung to 0, as it would be when they're produced - 0.37ns = difference in propogation time in vetoes vs SAC
      
      //time difference
      fHS->FillHistoList("PVetoSACT0sList/StandardRecoClus","hDeltatPVetoSACCluster",tPVeto-tSAC);
      fHS->FillHistoList("PVetoSACT0sList/StandardRecoClusWithT0s","hDeltatPVetoSACCluster",tPVeto-tSAC-timecorrection);

      if(chPVeto>19&&chPVeto<71){
	fHS->FillHistoList("PVetoSACT0sList/StandardRecoClus","hDeltatPVetoSACClusterGoodPVetoCh",tPVeto-tSAC);
	fHS->FillHistoList("PVetoSACT0sList/StandardRecoClusWithT0s","hDeltatPVetoSACClusterGoodPVetoCh",tPVeto-tSAC-timecorrection);
      }
      
      if(chPVeto>19&&chPVeto<71&&chSAC==22){
	fHS->FillHistoList("PVetoSACT0sList/StandardRecoClus","hDeltatPVetoSAC22ClusterGoodPVetoCh",tPVeto-tSAC);
	fHS->FillHistoList("PVetoSACT0sList/StandardRecoClusWithT0s","hDeltatPVetoSAC22ClusterGoodPVetoCh",tPVeto-tSAC-timecorrection);
      }
      
      sprintf(name,"hDeltatPVetoSAC22ClusterCh%i",chPVeto);
      if(chSAC==22&&chPVeto<90){
	fHS->FillHistoList("PVetoSACT0sList/StandardRecoClus",name,tPVeto-tSAC);
	fHS->FillHistoList("PVetoSACT0sList/StandardRecoClusWithT0s",name,tPVeto-tSAC-timecorrection);
	//	std::cout<<"NPVetoCluster "<<NPVetoCluster<<" jj "<<jj<<" PVetoClusterTime "<<tPVeto<<" SACClusterTime "<<tSAC<<std::endl;
      }
    }
      
    for(int kk = 0; kk<NPVetoHit;kk++){
      
      //import PVeto variables
      tHitPVeto     =  fEvent->PVetoRecoEvent->Hit(kk)->GetTime();
      chHitPVeto    =  fEvent->PVetoRecoEvent->Hit(kk)->GetChannelId();
      enHitPVeto    =  fEvent->PVetoRecoEvent->Hit(kk)->GetEnergy();

      // fHS->FillHistoList("PVetoHits","hPVetoHitEnergy",enHitPVeto);
      // fHS->FillHistoList("PVetoHits","hPVetoHitCh",chHitPVeto);
      // fHS->FillHistoList("PVetoHits","hPVetoHitChVsHitEnergy",chHitPVeto,enHitPVeto);

      fHS->FillHistoList("PVetoSACT0sList/StandardRecoHits","hDeltatPVetoHitsSACCluster",tHitPVeto-tSAC);

      if(chPVeto>19&&chPVeto<71)
	fHS->FillHistoList("PVetoSACT0sList/StandardRecoHits","hDeltatPVetoHitsSACClusterGoodPVetoCh",tHitPVeto-tSAC);

      if(chPVeto>19&&chPVeto<71&&chSAC==22)
	fHS->FillHistoList("PVetoSACT0sList/StandardRecoHits","hDeltatPVetoHitsSAC22ClusterGoodPVetoCh",tHitPVeto-tSAC);
      
      //time difference
      sprintf(name,"hDeltatPVetoHitsSAC22Ch%i",chHitPVeto);
      if(chSAC==22&&chPVeto<90)	fHS->FillHistoList("PVetoSACT0sList/StandardRecoHits",name,tHitPVeto-tSAC);
    }

    for(int jj = 0; jj<NEVetoCluster;jj++){

      //import EVeto variables
      tEVeto     =  fEvent->EVetoRecoCl->Element(jj)->GetTime();
      chEVeto    =  fEvent->EVetoRecoCl->Element(jj)->GetChannelId();
      NClusterHitsEVeto =  fEvent->EVetoRecoCl->Element(jj)->GetNHitsInClus();
      enEVeto    =  fEvent->EVetoRecoCl->Element(jj)->GetEnergy();

      timecorrection = 0.03594*(chEVeto)-11.52-0.37;//bring DeltaT(EVeto-SAC) for Bremsstrahlung to 0, as it would be when they're produced - 0.37ns = difference in propogation time in vetoes vs SAC
      
      //time difference
      fHS->FillHistoList("EVetoSACT0sList/StandardRecoClus","hDeltatEVetoSACCluster",tEVeto-tSAC);
      fHS->FillHistoList("EVetoSACT0sList/StandardRecoClusWithT0s","hDeltatEVetoSACCluster",tEVeto-tSAC-timecorrection);

      if(chEVeto>19&&chEVeto<71){
	fHS->FillHistoList("EVetoSACT0sList/StandardRecoClus","hDeltatEVetoSACClusterGoodEVetoCh",tEVeto-tSAC);
	fHS->FillHistoList("EVetoSACT0sList/StandardRecoClusWithT0s","hDeltatEVetoSACClusterGoodEVetoCh",tEVeto-tSAC-timecorrection);
      }
      
      if(chEVeto>19&&chEVeto<71&&chSAC==22){
	fHS->FillHistoList("EVetoSACT0sList/StandardRecoClus","hDeltatEVetoSAC22ClusterGoodEVetoCh",tEVeto-tSAC);
	fHS->FillHistoList("EVetoSACT0sList/StandardRecoClusWithT0s","hDeltatEVetoSAC22ClusterGoodEVetoCh",tEVeto-tSAC-timecorrection);
      }
      
      sprintf(name,"hDeltatEVetoSAC22ClusterCh%i",chEVeto);
      if(chSAC==22&&chEVeto<90){
	fHS->FillHistoList("EVetoSACT0sList/StandardRecoClus",name,tEVeto-tSAC);
	fHS->FillHistoList("EVetoSACT0sList/StandardRecoClusWithT0s",name,tEVeto-tSAC-timecorrection);
	//	std::cout<<"NEVetoCluster "<<NEVetoCluster<<" jj "<<jj<<" EVetoClusterTime "<<tEVeto<<" SACClusterTime "<<tSAC<<std::endl;
      }
    }
    
    for(int mm = 0; mm<NEVetoHit;mm++){
   
      //import EVeto variables
      tHitEVeto     =  fEvent->EVetoRecoEvent->Hit(mm)->GetTime();
      chHitEVeto    =  fEvent->EVetoRecoEvent->Hit(mm)->GetChannelId();
      enHitEVeto    =  fEvent->EVetoRecoEvent->Hit(mm)->GetEnergy();

      //      std::cout<<"chhiteveto "<<chHitEVeto<<" chsac "<<chSAC<<std::endl;

      // fHS->FillHistoList("EVetoHits","hEVetoHitEnergy",enHitEVeto);
      // fHS->FillHistoList("EVetoHits","hEVetoHitCh",chHitEVeto);
      // fHS->FillHistoList("EVetoHits","hEVetoHitChVsHitEnergy",chHitEVeto,enHitEVeto);

      fHS->FillHistoList("EVetoSACT0sList/StandardRecoHits","hDeltatEVetoHitsSACCluster",tHitEVeto-tSAC);

      if(chEVeto>19&&chEVeto<71)
	fHS->FillHistoList("EVetoSACT0sList/StandardRecoHits","hDeltatEVetoHitsSACClusterGoodEVetoCh",tHitEVeto-tSAC);

      if(chEVeto>19&&chEVeto<71&&chSAC==22)
	fHS->FillHistoList("EVetoSACT0sList/StandardRecoHits","hDeltatEVetoHitsSAC22ClusterGoodEVetoCh",tHitEVeto-tSAC);

      //time difference
      sprintf(name,"hDeltatEVetoHitsSAC22Ch%i",chHitEVeto);
      if(chSAC==22&&chEVeto<90){
	fHS->FillHistoList("EVetoSACT0sList/StandardRecoHits",name,tHitEVeto-tSAC);
      }
    }

  }

  double tPVeto_jj;
  double tPVeto_kk;
  double tPVeto_jjminus;
  double DeltatPVeto;
  int bdPVeto_jj;
  int bdPVeto_kk;
  int bdPVeto_jjminus;
  int elchPVeto_jj;
  int elchPVeto_kk;
  int elchPVeto_jjminus;
  int chPVeto_jj;
  int chPVeto_kk;
  int chPVeto_jjminus;
  double enPVeto_jj;
  double enPVeto_kk;
  double enPVeto_jjminus;
  std::vector<Int_t> PVetoClusterHitIndices; //Hit indices in particular PVeto cluster
  TRecoVHit* PVetoHit;
  Int_t PVetoHitIndex_jj;
  Int_t PVetoHitIndex_kk;
  Int_t PVetoHitIndex_jjminus;

  //PVeto ch_i-ch_i-- difference
  for(int iPClus = 0; iPClus<NPVetoCluster;iPClus++){
    PVetoClusterHitIndices.clear();

    NClusterHitsPVeto =  fEvent->PVetoRecoCl->Element(iPClus)->GetNHitsInClus();    
    PVetoClusterHitIndices = fEvent->PVetoRecoCl->Element(iPClus)->GetHitVecInClus();

    if(NClusterHitsPVeto!=PVetoClusterHitIndices.size()) std::cout<<"NClusterHitsPVeto "<<NClusterHitsPVeto<<" PVetoClusterHitIndices.size() "<<PVetoClusterHitIndices.size()<<std::endl;

    for(int jj = 0; jj<NClusterHitsPVeto; jj++){
      for(int kk = 1; kk<NClusterHitsPVeto; kk++){
	//import PVeto variables
	if(kk==jj) continue;

	PVetoHitIndex_jj = PVetoClusterHitIndices[jj];
	PVetoHitIndex_kk = PVetoClusterHitIndices[kk];

	chPVeto_jj = fEvent->PVetoRecoEvent->Hit(PVetoHitIndex_jj)->GetChannelId();
	chPVeto_kk = fEvent->PVetoRecoEvent->Hit(PVetoHitIndex_kk)->GetChannelId();
	
	tPVeto_jj=fEvent->PVetoRecoEvent->Hit(PVetoHitIndex_jj)->GetTime();
	tPVeto_kk=fEvent->PVetoRecoEvent->Hit(PVetoHitIndex_kk)->GetTime();

	enPVeto_jj = fEvent->PVetoRecoEvent->Hit(PVetoHitIndex_jj)->GetEnergy();
	enPVeto_kk = fEvent->PVetoRecoEvent->Hit(PVetoHitIndex_kk)->GetEnergy();
	enPVeto_jjminus = fEvent->PVetoRecoEvent->Hit(PVetoHitIndex_jjminus)->GetEnergy();

	//time difference
	//      DeltatPVeto = tPVeto_jj-tPVeto_jjminus;
	DeltatPVeto = tPVeto_jj-tPVeto_kk;

	fHS->FillHistoList("PVetoAdjChaT0sList","hDeltatPVetoCh_iCh_i--",DeltatPVeto);
    
	if(chPVeto>19&&chPVeto<71) fHS->FillHistoList("PVetoAdjChaT0sList","hDeltatPVetoCh_iCh_i--GoodPVetoCh",DeltatPVeto);

	if(chPVeto_jj-chPVeto_kk==1){
	  sprintf(name,"hDeltatPVetoCh%iCh%i",chPVeto_jj,chPVeto_kk);
	  fHS->FillHistoList("PVetoAdjChaT0sList",name,DeltatPVeto);
	}
	else if(chPVeto_jj-chPVeto_kk==-1){
	  sprintf(name,"hDeltatPVetoCh%iCh%i",chPVeto_kk,chPVeto_jj);
	  fHS->FillHistoList("PVetoAdjChaT0sList",name,DeltatPVeto);
	}
      }
    }
  }

  double tEVeto_jj;
  double tEVeto_kk;
  double tEVeto_jjminus;
  double DeltatEVeto;
  int bdEVeto_jj;
  int bdEVeto_kk;
  int bdEVeto_jjminus;
  int elchEVeto_jj;
  int elchEVeto_kk;
  int elchEVeto_jjminus;
  int chEVeto_jj;
  int chEVeto_kk;
  int chEVeto_jjminus;
  double enEVeto_jj;
  double enEVeto_kk;
  double enEVeto_jjminus;
  std::vector<Int_t> EVetoClusterHitIndices; //Hit indices in particular EVeto cluster
  TRecoVHit* EVetoHit;
  Int_t EVetoHitIndex_jj;
  Int_t EVetoHitIndex_kk;
  Int_t EVetoHitIndex_jjminus;

  //EVeto ch_i-ch_i-- difference
  for(int iPClus = 0; iPClus<NEVetoCluster;iPClus++){
    EVetoClusterHitIndices.clear();

    NClusterHitsEVeto =  fEvent->EVetoRecoCl->Element(iPClus)->GetNHitsInClus();    
    EVetoClusterHitIndices = fEvent->EVetoRecoCl->Element(iPClus)->GetHitVecInClus();

    if(NClusterHitsEVeto!=EVetoClusterHitIndices.size()) std::cout<<"NClusterHitsEVeto "<<NClusterHitsEVeto<<" EVetoClusterHitIndices.size() "<<EVetoClusterHitIndices.size()<<std::endl;

    for(int jj = 0; jj<NClusterHitsEVeto; jj++){
      for(int kk = 1; kk<NClusterHitsEVeto; kk++){
	//import EVeto variables
	if(kk==jj) continue;

	EVetoHitIndex_jj = EVetoClusterHitIndices[jj];
	EVetoHitIndex_kk = EVetoClusterHitIndices[kk];

	chEVeto_jj = fEvent->EVetoRecoEvent->Hit(EVetoHitIndex_jj)->GetChannelId();
	chEVeto_kk = fEvent->EVetoRecoEvent->Hit(EVetoHitIndex_kk)->GetChannelId();
	
	tEVeto_jj=fEvent->EVetoRecoEvent->Hit(EVetoHitIndex_jj)->GetTime();
	tEVeto_kk=fEvent->EVetoRecoEvent->Hit(EVetoHitIndex_kk)->GetTime();

	enEVeto_jj = fEvent->EVetoRecoEvent->Hit(EVetoHitIndex_jj)->GetEnergy();
	enEVeto_kk = fEvent->EVetoRecoEvent->Hit(EVetoHitIndex_kk)->GetEnergy();
	enEVeto_jjminus = fEvent->EVetoRecoEvent->Hit(EVetoHitIndex_jjminus)->GetEnergy();

	//time difference
	//      DeltatEVeto = tEVeto_jj-tEVeto_jjminus;
	DeltatEVeto = tEVeto_jj-tEVeto_kk;

	fHS->FillHistoList("EVetoAdjChaT0sList","hDeltatEVetoCh_iCh_i--",DeltatEVeto);
    
	if(chEVeto>19&&chEVeto<71) fHS->FillHistoList("EVetoAdjChaT0sList","hDeltatEVetoCh_iCh_i--GoodEVetoCh",DeltatEVeto);

	if(chEVeto_jj-chEVeto_kk==1){
	  sprintf(name,"hDeltatEVetoCh%iCh%i",chEVeto_jj,chEVeto_kk);
	  fHS->FillHistoList("EVetoAdjChaT0sList",name,DeltatEVeto);
	}
	else if(chEVeto_jj-chEVeto_kk==-1){
	  sprintf(name,"hDeltatEVetoCh%iCh%i",chEVeto_kk,chEVeto_jj);
	  fHS->FillHistoList("EVetoAdjChaT0sList",name,DeltatEVeto);
	}
	// else  std::cout<<"cluster "<<iPClus<<" chEVeto_jj "<<chEVeto_jj<<" chEVeto_jjminus "<<chEVeto_jjminus<<std::endl;
      }
    }
  }

  //find Delta(ToF) for Bremsstrahlung positrons in PVeto vs photons in SAC (central channel, 22)

  //  if(fSwimmerInit==0){
  //    fSwimmerInit=1;
  if(isMC&&fSwimBrem){//&&fEvent->MCTruthEvent->GetEventNumber()<50){
    for(Int_t iV = 0; iV<fEvent->MCTruthEvent->GetNVertices(); iV++) {
        mcVtx = fEvent->MCTruthEvent->Vertex(iV);
	if(mcVtx->GetProcess() == "eBrem"){
	  TVector3 VertexPos = mcVtx->GetPosition();
	  double VertexTime = mcVtx->GetTime();
	  for(Int_t iOut = 0; iOut<mcVtx->GetNParticleOut(); iOut++) {
	    mcOutPart = mcVtx->ParticleOut(iOut);
	    if(mcOutPart->GetPDGCode()==-11){
	      double energy = mcOutPart->GetEnergy();
	      TVector3 momentum = mcOutPart->GetMomentum();
	      TLorentzVector FourMomentum = TLorentzVector(momentum,energy);
	      int pcharge = 1;
	      int echarge = -1;
	      
	      fVetoEndPoint->ParticleSwim(FourMomentum,VertexPos,VertexTime,pcharge);
	      int PVetoSwimmingChannel=fVetoEndPoint->GetEndFinger();
	      double PVetoSwimmingTime=fVetoEndPoint->GetEndTime();
	      
	      fVetoEndPoint->ParticleSwim(FourMomentum,VertexPos,VertexTime,echarge);
	      int EVetoSwimmingChannel=fVetoEndPoint->GetEndFinger();
	      double EVetoSwimmingTime=fVetoEndPoint->GetEndTime();
	      
	      double targetsacdistance = 402.8;//cm, from SACGeometry and TargetGeometry 22/12/22
	      double saclength = 14;//cm, from SACGeometry 19/12/22
	      double sacrefractiveindex = 1.85;
	      double c_ms = 2.998e8;//m/s
	      double c_cmns = c_ms*1e-9*1e2;//cm/ns
	      double timetosacface = targetsacdistance/c_cmns;//ns
	      double timetosacsipm = timetosacface+saclength*1.85/c_cmns;//ns 
	      
	      // std::cout<<"PVetoCh "<<PVetoSwimmingChannel<<" PVetoCh "<<PVetoSwimmingTime<<" time to sac "<<timetosacface<<" deltaT "<<PVetoSwimmingTime-timetosacface<<" to center "<<PVetoSwimmingTime-timetosacSiPM<<std::endl;
	      // std::cout<<"EVetoCh "<<EVetoSwimmingChannel<<" EVetoCh "<<EVetoSwimmingTime<<" time to sac "<<timetosacface<<" deltaT "<<EVetoSwimmingTime-timetosacface<<" to center "<<EVetoSwimmingTime-timetosacSiPM<<std::endl;
	      
	      if(PVetoSwimmingChannel>-1&&PVetoSwimmingChannel<90){
		sprintf(name,"hDeltatSwumPVetoCh%iSACFrontFace",PVetoSwimmingChannel);
		fHS->FillHistoList("PVetoSACT0sList/SwimmerFrontFace",name,PVetoSwimmingTime-timetosacface);
		sprintf(name,"hDeltatSwumPVetoCh%iSACSiPM",PVetoSwimmingChannel);
		fHS->FillHistoList("PVetoSACT0sList/SwimmerSACSiPM",name,PVetoSwimmingTime-timetosacsipm);
	      }
	      
	      if(EVetoSwimmingChannel>-1&&EVetoSwimmingChannel<96){
		sprintf(name,"hDeltatSwumEVetoCh%iSACFrontFace",EVetoSwimmingChannel);
		fHS->FillHistoList("EVetoSACT0sList/SwimmerFrontFace",name,EVetoSwimmingTime-timetosacface);
		sprintf(name,"hDeltatSwumEVetoCh%iSACSiPM",EVetoSwimmingChannel);
		fHS->FillHistoList("EVetoSACT0sList/SwimmerSACSiPM",name,EVetoSwimmingTime-timetosacsipm);
	      }
	      
	      fHS->FillHistoList("SwumEnergyToChannel","hSwumEnergyToChannelPVeto",PVetoSwimmingChannel,energy);
	      fHS->FillHistoList("SwumEnergyToChannel","hSwumEnergyToChannelEVeto",EVetoSwimmingChannel,energy);  
	    }
	  }
	}
    }
  }//end if MC
  return true;
}

Bool_t T0sAnalysis::Finalize()
{
 // TGraph* nPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vNPoT[0]);
 // fHS->SaveTGraphList("GGAnalysis","NPotVsTime",nPotVsTime);
 
 if (fVerbose) printf("---> Finalizing T0sAnalysis\n");
  return true;
}

