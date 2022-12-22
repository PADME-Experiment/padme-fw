// Written by Beth Long 13/6/22
#include "T0sAnalysis.hh"
#include "TRecoVHit.hh"
#include "VetoEndPoint.hh"
#include "TLorentzVector.h"

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
  return true;
}

Bool_t T0sAnalysis::InitHistos(){
  // T0sAnalysis directory will contain all histograms related to this analysis
  fHS->CreateList("PVetoSACT0sList/StandardRecoClus");
  fHS->CreateList("EVetoSACT0sList/StandardRecoClus");

  fHS->CreateList("PVetoSACT0sList/StandardRecoHits");
  fHS->CreateList("EVetoSACT0sList/StandardRecoHits");

  fHS->CreateList("PVetoSACT0sList/SwimmerFrontFace");
  fHS->CreateList("EVetoSACT0sList/SwimmerFrontFace");

  fHS->CreateList("PVetoSACT0sList/SwimmerSACCentre");
  fHS->CreateList("EVetoSACT0sList/SwimmerSACCentre");

  fHS->CreateList("PVetoAdjChaT0sList");
  fHS->CreateList("EVetoAdjChaT0sList");

  fHS->CreateList("SwumEnergyToChannel");
  
  //Time difference PVetoClusters wrt SAC
  fHS->BookHistoList("PVetoSACT0sList/StandardRecoClus","hDeltatPVetoSACCluster",4000,-500,500);
  fHS->BookHistoList("PVetoSACT0sList/StandardRecoClus","hDeltatPVetoSACClusterGoodPVetoCh",4000,-500,500);
  fHS->BookHistoList("PVetoSACT0sList/StandardRecoClus","hDeltatPVetoSAC22ClusterGoodPVetoCh",4000,-500,500);

  fHS->BookHistoList("EVetoSACT0sList/StandardRecoClus","hDeltatEVetoSACCluster",4000,-500,500);
  fHS->BookHistoList("EVetoSACT0sList/StandardRecoClus","hDeltatEVetoSACClusterGoodEVetoCh",4000,-500,500);
  fHS->BookHistoList("EVetoSACT0sList/StandardRecoClus","hDeltatEVetoSAC22ClusterGoodEVetoCh",4000,-500,500);

  //Time difference PVetoHits wrt SAC
  fHS->BookHistoList("PVetoSACT0sList/StandardRecoHits","hDeltatPVetoSACCluster",4000,-500,500);
  fHS->BookHistoList("PVetoSACT0sList/StandardRecoHits","hDeltatPVetoSACClusterGoodPVetoCh",4000,-500,500);
  fHS->BookHistoList("PVetoSACT0sList/StandardRecoHits","hDeltatPVetoSAC22ClusterGoodPVetoCh",4000,-500,500);

  fHS->BookHistoList("EVetoSACT0sList/StandardRecoHits","hDeltatEVetoSACCluster",4000,-500,500);
  fHS->BookHistoList("EVetoSACT0sList/StandardRecoHits","hDeltatEVetoSACClusterGoodEVetoCh",4000,-500,500);
  fHS->BookHistoList("EVetoSACT0sList/StandardRecoHits","hDeltatEVetoSAC22ClusterGoodEVetoCh",4000,-500,500);

  //Time difference wrt previous channel
  fHS->BookHistoList("PVetoAdjChaT0sList","hDeltatPVetoCh_iCh_i--",80,-5,5);
  fHS->BookHistoList("EVetoAdjChaT0sList","hDeltatEVetoCh_iCh_i--",80,-5,5);
  fHS->BookHistoList("PVetoAdjChaT0sList","hDeltatPVetoCh_iCh_i--GoodPVetoCh",80,-5,5);
  fHS->BookHistoList("EVetoAdjChaT0sList","hDeltatEVetoCh_iCh_i--GoodPVetoCh",80,-5,5);

  //Collinear particles channelID->energy
  fHS->BookHisto2List("SwumEnergyToChannel","hSwumEnergyToChannelPVeto",90,0,90,215,0,430);
  fHS->BookHisto2List("SwumEnergyToChannel","hSwumEnergyToChannelEVeto",96,0,96,215,0,430);
  
  //Individual channel times
  for(int ch=0;ch<90;ch++){
    sprintf(name,"hDeltatPVetoSAC22ClusterCh%i",ch);
    fHS->BookHistoList("PVetoSACT0sList/StandardRecoClus",name,1000,-15,40);
    sprintf(name,"hDeltatPVetoSAC22ClusterCh%i",ch);
    fHS->BookHistoList("PVetoSACT0sList/StandardRecoHits",name,100,-15,-6);
    sprintf(name,"hDeltatSwumPVetoCh%iSACFrontFace",ch);
    fHS->BookHistoList("PVetoSACT0sList/SwimmerFrontFace",name,100,-15,-6);
    sprintf(name,"hDeltatSwumPVetoCh%iSACCentre",ch);
    fHS->BookHistoList("PVetoSACT0sList/SwimmerSACCentre",name,100,-15,-6);
    
    if(ch>0){
      sprintf(name,"hDeltatPVetoCh%iCh%i",ch,ch-1);
      //      std::cout<<"name "<<name<<" ch "<<ch<<" ch-1 "<<ch-1<<std::endl;
      fHS->BookHistoList("PVetoAdjChaT0sList",name,80,-5,5);
    }
  }
  for(int ch=0;ch<96;ch++){
    sprintf(name,"hDeltatEVetoSAC22ClusterCh%i",ch);
    fHS->BookHistoList("EVetoSACT0sList/StandardRecoClus",name,100,-15,-6);
    sprintf(name,"hDeltatSwumEVetoCh%iSACFrontFace",ch);
    fHS->BookHistoList("EVetoSACT0sList/SwimmerFrontFace",name,100,-15,-6);
    sprintf(name,"hDeltatSwumEVetoCh%iSACCentre",ch);
    fHS->BookHistoList("EVetoSACT0sList/SwimmerSACCentre",name,100,-15,-6);

    
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
  
  //time
  double tPVeto;
  double tEVeto;
  double tSAC;

  //channel
  int chPVeto;
  int chEVeto;
  int chSAC;

  //no hits in cluster
  int NClusterHitsPVeto;
  int NClusterHitsEVeto;
  int NClusterHitsSAC;

  //energy
  double enPVeto;
  double enEVeto;
  double enSAC;

  //energy sum
  double enPositron;
  double enSum;

  double timecorrection = -3.67;

  //Vetos-SAC difference
  for(int ii = 0; ii<NSACCluster; ii++){
    if(!fEvent->SACRecoCl) break;
    //import SAC variables
    tSAC     =  fEvent->SACRecoCl->Element(ii)->GetTime();
    chSAC    =  fEvent->SACRecoCl->Element(ii)->GetChannelId();
    NClusterHitsSAC =  fEvent->SACRecoCl->Element(ii)->GetNHitsInClus();
    enSAC    =  fEvent->SACRecoCl->Element(ii)->GetEnergy();

    // if(chSAC!=22) continue;
    
    for(int ii = 0; ii<NPVetoCluster;ii++){

      //import PVeto variables
      tPVeto     =  fEvent->PVetoRecoCl->Element(ii)->GetTime();
      chPVeto    =  fEvent->PVetoRecoCl->Element(ii)->GetChannelId();
      NClusterHitsPVeto =  fEvent->PVetoRecoCl->Element(ii)->GetNHitsInClus();
      enPVeto    =  fEvent->PVetoRecoCl->Element(ii)->GetEnergy();

      //      if(NClusterHitsPVeto==1) std::cout<<NClusterHitsPVeto<<std::endl;

      //time difference
      fHS->FillHistoList("PVetoSACT0sList/StandardRecoClus","hDeltatPVetoSACCluster",tPVeto-tSAC);
      if(chPVeto>19&&chPVeto<71) fHS->FillHistoList("PVetoSACT0sList/StandardRecoClus","hDeltatPVetoSACClusterGoodPVetoCh",tPVeto-tSAC);
      if(chPVeto>19&&chPVeto<71&&chSAC==22) fHS->FillHistoList("PVetoSACT0sList/StandardRecoClus","hDeltatPVetoSAC22ClusterGoodPVetoCh",tPVeto-tSAC);

      sprintf(name,"hDeltatPVetoSAC22ClusterCh%i",chPVeto);
      if(chSAC==22&&chPVeto<90)	fHS->FillHistoList("PVetoSACT0sList/StandardRecoClus",name,tPVeto-tSAC);
    }

     for(int ii = 0; ii<NPVetoHit;ii++){

      //import PVeto variables
      tPVeto     =  fEvent->PVetoRecoEvent->Hit(ii)->GetTime();
      chPVeto    =  fEvent->PVetoRecoEvent->Hit(ii)->GetChannelId();
      enPVeto    =  fEvent->PVetoRecoEvent->Hit(ii)->GetEnergy();

      //      if(NClusterHitsPVeto==1) std::cout<<NClusterHitsPVeto<<std::endl;

      //time difference
      fHS->FillHistoList("PVetoSACT0sList/StandardRecoHits","hDeltatPVetoSACCluster",tPVeto-tSAC);
      if(chPVeto>19&&chPVeto<71) fHS->FillHistoList("PVetoSACT0sList/StandardRecoHits","hDeltatPVetoSACClusterGoodPVetoCh",tPVeto-tSAC);
      if(chPVeto>19&&chPVeto<71&&chSAC==22) fHS->FillHistoList("PVetoSACT0sList/StandardRecoHits","hDeltatPVetoSAC22ClusterGoodPVetoCh",tPVeto-tSAC);

      sprintf(name,"hDeltatPVetoSAC22ClusterCh%i",chPVeto);
      if(chSAC==22&&chPVeto<90)	fHS->FillHistoList("PVetoSACT0sList/StandardRecoHits",name,tPVeto-tSAC);
    }

    for(int ii = 0; ii<NEVetoCluster;ii++){

      //import EVeto variables
      tEVeto     =  fEvent->EVetoRecoCl->Element(ii)->GetTime();
      chEVeto    =  fEvent->EVetoRecoCl->Element(ii)->GetChannelId();
      NClusterHitsEVeto =  fEvent->EVetoRecoCl->Element(ii)->GetNHitsInClus();
      enEVeto    =  fEvent->EVetoRecoCl->Element(ii)->GetEnergy();

      //time difference
      fHS->FillHistoList("EVetoSACT0sList/StandardRecoClus","hDeltatEVetoSACCluster",tEVeto-tSAC);
      if(chEVeto>19&&chEVeto<71) fHS->FillHistoList("EVetoSACT0sList/StandardRecoClus","hDeltatEVetoSACClusterGoodEVetoCh",tEVeto-tSAC);
      if(chEVeto>19&&chEVeto<71&&chSAC==22) fHS->FillHistoList("EVetoSACT0sList/StandardRecoClus","hDeltatEVetoSAC22ClusterGoodEVetoCh",tEVeto-tSAC);

      sprintf(name,"hDeltatEVetoSAC22ClusterCh%i",chEVeto);
      if(chSAC==22&&chEVeto<90)	fHS->FillHistoList("EVetoSACT0sList/StandardRecoClus",name,tEVeto-tSAC);
    }
  }

  double tPVeto_jj;
  double tPVeto_kk;
  double tPVeto_jjminus;
  double Deltat;
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

  TRecoVHit * myhit;

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
	//	myhit = fEvent->PVetoRecoEvent->Hit(PVetoHitIndex_jj);
	PVetoHitIndex_kk = PVetoClusterHitIndices[kk];

	if(isMC==false){
	  bdPVeto_jj=fEvent->PVetoRecoEvent->Hit(PVetoHitIndex_jj)->getBDid();
	  //continue;	
	  elchPVeto_jj=fEvent->PVetoRecoEvent->Hit(PVetoHitIndex_jj)->getCHid();
	  if(elchPVeto_jj!=0||elchPVeto_kk!=0){
	    // std::cout<<"bdjj "<<bdPVeto_jj<<" bdkk "<<bdPVeto_jj<<std::endl;
	    // std::cout<<"elchjj "<<elchPVeto_jj<<" elchkk "<<elchPVeto_kk<<std::endl;
	  }
	  chPVeto_jj = (bdPVeto_jj-10)*32+elchPVeto_jj;
	  
	  bdPVeto_kk=fEvent->PVetoRecoEvent->Hit(PVetoHitIndex_kk)->getBDid();
	  elchPVeto_kk=fEvent->PVetoRecoEvent->Hit(PVetoHitIndex_kk)->getCHid();
	  chPVeto_kk = (bdPVeto_kk-10)*32+elchPVeto_kk;
	}

	else if(isMC==true){
	  chPVeto_jj = fEvent->PVetoRecoEvent->Hit(PVetoHitIndex_jj)->GetChannelId();
	  chPVeto_kk = fEvent->PVetoRecoEvent->Hit(PVetoHitIndex_kk)->GetChannelId();
	}
	
	tPVeto_jj=fEvent->PVetoRecoEvent->Hit(PVetoHitIndex_jj)->GetTime();
	tPVeto_kk=fEvent->PVetoRecoEvent->Hit(PVetoHitIndex_kk)->GetTime();
	//      tPVeto_jjminus=fEvent->PVetoRecoEvent->Hit(PVetoHitIndex_jjminus)->GetTime();

	enPVeto_jj = fEvent->PVetoRecoEvent->Hit(PVetoHitIndex_jj)->GetEnergy();
	enPVeto_kk = fEvent->PVetoRecoEvent->Hit(PVetoHitIndex_kk)->GetEnergy();
	enPVeto_jjminus = fEvent->PVetoRecoEvent->Hit(PVetoHitIndex_jjminus)->GetEnergy();

	//time difference
	//      Deltat = tPVeto_jj-tPVeto_jjminus;
	Deltat = tPVeto_jj-tPVeto_kk;

	fHS->FillHistoList("PVetoAdjChaT0sList","hDeltatPVetoCh_iCh_i--",Deltat);
    
	if(chPVeto>19&&chPVeto<71) fHS->FillHistoList("PVetoAdjChaT0sList","hDeltatPVetoCh_iCh_i--GoodPVetoCh",Deltat);

	//if(kk==1)	std::cout<<"chjj "<<chPVeto_jj<<" chkk "<<chPVeto_kk<<std::endl;
	if(chPVeto_jj-chPVeto_kk==1){
	  //	  std::cout<<"chjj "<<chPVeto_jj<<" chkk "<<chPVeto_kk<<std::endl;
	  sprintf(name,"hDeltatPVetoCh%iCh%i",chPVeto_jj,chPVeto_kk);
	  fHS->FillHistoList("PVetoAdjChaT0sList",name,Deltat);
	}
	else if(chPVeto_jj-chPVeto_kk==-1){
	  sprintf(name,"hDeltatPVetoCh%iCh%i",chPVeto_kk,chPVeto_jj);
	  fHS->FillHistoList("PVetoAdjChaT0sList",name,Deltat);
	}
	// else  std::cout<<"cluster "<<iPClus<<" chPVeto_jj "<<chPVeto_jj<<" chPVeto_jjminus "<<chPVeto_jjminus<<std::endl;
      }
    }
  }

  //find Delta(ToF) for Bremsstrahlung positrons in PVeto vs photons in SAC (central channel, 22)

  if(fEvent->RecoEvent->GetEventNumber()==0){
    for(int ii =0; ii<5000; ii++){
  
      double energy = myRNG->Uniform(0,430);
      TVector3 momentum = TVector3(0,0,energy);//assume particles emmitted collinearly with beam;
      TLorentzVector FourMomentum = TLorentzVector(momentum,energy);
      TVector3 VertexPos = TVector3(0,0,-1028);//mm
      double VertexTime = 0;
      int pcharge = 1;
      int echarge = -1;

      fVetoEndPoint->ParticleSwim(FourMomentum,VertexPos,VertexTime,pcharge);
      int PVetoSwimmingChannel=fVetoEndPoint->GetEndFinger();
      double PVetoSwimmingTime=fVetoEndPoint->GetEndTime();

      fVetoEndPoint->ParticleSwim(FourMomentum,VertexPos,VertexTime,echarge);
      int EVetoSwimmingChannel=fVetoEndPoint->GetEndFinger();
      double EVetoSwimmingTime=fVetoEndPoint->GetEndTime();

      double targetsacdistance = 370.;//cm, from SACGeometry 19/12/22
      double halfsaclength = 7;//cm, from SACGeometry 19/12/22
      double sacrefractiveindex = 1.85;
      double c_ms = 2.998e8;//m/s
      double c_cmns = c_ms*1e-9*1e2;//cm/ns
      double timetosacface = targetsacdistance/c_cmns;//ns
      double timetosaccentre = timetosacface+halfsaclength*1.85/c_cmns;//ns

      // std::cout<<"PVetoCh "<<PVetoSwimmingChannel<<" PVetoCh "<<PVetoSwimmingTime<<" time to sac "<<timetosacface<<" deltaT "<<PVetoSwimmingTime-timetosacface<<" to center "<<PVetoSwimmingTime-timetosaccentre<<std::endl;
      // std::cout<<"EVetoCh "<<EVetoSwimmingChannel<<" EVetoCh "<<EVetoSwimmingTime<<" time to sac "<<timetosacface<<" deltaT "<<EVetoSwimmingTime-timetosacface<<" to center "<<EVetoSwimmingTime-timetosaccentre<<std::endl;
  
      if(PVetoSwimmingChannel>-1&&PVetoSwimmingChannel<90){
	sprintf(name,"hDeltatSwumPVetoCh%iSACFrontFace",PVetoSwimmingChannel);
	fHS->FillHistoList("PVetoSACT0sList/SwimmerFrontFace",name,PVetoSwimmingTime-timetosacface);
	sprintf(name,"hDeltatSwumPVetoCh%iSACCentre",PVetoSwimmingChannel);
	fHS->FillHistoList("PVetoSACT0sList/SwimmerSACCentre",name,PVetoSwimmingTime-timetosaccentre);
      }

      if(EVetoSwimmingChannel>-1&&EVetoSwimmingChannel<96){
	sprintf(name,"hDeltatSwumEVetoCh%iSACFrontFace",EVetoSwimmingChannel);
	fHS->FillHistoList("EVetoSACT0sList/SwimmerFrontFace",name,EVetoSwimmingTime-timetosacface);
	sprintf(name,"hDeltatSwumEVetoCh%iSACCentre",EVetoSwimmingChannel);
	fHS->FillHistoList("EVetoSACT0sList/SwimmerSACCentre",name,EVetoSwimmingTime-timetosaccentre);
      }
  
      fHS->FillHistoList("SwumEnergyToChannel","hSwumEnergyToChannelPVeto",PVetoSwimmingChannel,energy);
      fHS->FillHistoList("SwumEnergyToChannel","hSwumEnergyToChannelEVeto",EVetoSwimmingChannel,energy);  
    }
  }
  return true;
}

Bool_t T0sAnalysis::Finalize()
{
 // TGraph* nPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vNPoT[0]);
 // fHS->SaveTGraphList("GGAnalysis","NPotVsTime",nPotVsTime);
 
 if (fVerbose) printf("---> Finalizing T0sAnalysis\n");
  return true;
}

