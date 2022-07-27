// Written by E. Leonardi 
// Last modified by M. Raggi 2/08/2021
#include "ETagAnalysis.hh"

ETagAnalysis::ETagAnalysis(TString cfgFile, Int_t verbose)
{
  Neve=0;
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating ETagAnalysis\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  fHS = HistoSvc::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
}

ETagAnalysis::~ETagAnalysis(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t ETagAnalysis::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing ETagAnalysis\n");
  fEvent = event;
  InitHistos();
  return true;
}

Bool_t ETagAnalysis::InitHistos(){
  // ETagAnalysis directory will contain all histograms related to this analysis
  fHS->CreateList("ETagAnalysis");
  fHS->BookHistoList("ETagAnalysis","NPoT",600,-1000.,59000.);
  fHS->BookHistoList("ETagAnalysis","NPoTNoPhys",600,-1000.,59000.);
  fHS->BookHistoList("ETagAnalysis","NPoTPhys",600,-1000.,59000.);
  fHS->BookHistoList("ETagAnalysis","NPoTPhys5K",600,-1000.,59000.);

  fHS->BookHistoList("ETagAnalysis","XPos",200,-25.,25.);
  fHS->BookHistoList("ETagAnalysis","YPos",200,-25.,25.);

  fHS->BookHistoList("ETagAnalysis","LeftRightTimeDiff",200,-25.,25.);
  fHS->BookHistoList("ETagAnalysis","LeftRightTimeSum",200,-500.,0.);
  fHS->BookHistoList("ETagAnalysis","LeftECalTimeDiff",200,-50.,50.);
  fHS->BookHistoList("ETagAnalysis","RightECalTimeDiff",200,-50.,50.);
  fHS->BookHistoList("ETagAnalysis","LeftECalTimeDiffECalEnergyCut",200,-50.,50.);
  fHS->BookHistoList("ETagAnalysis","RightECalTimeDiffECalEnergyCut",200,-50.,50.);
  return true;
}

Bool_t ETagAnalysis::Process(){

  fNPoT = fEvent->TargetRecoBeam->getnPOT();
  fXPos = fEvent->TargetRecoBeam->getX();
  fYPos = fEvent->TargetRecoBeam->getY();
  TotPoT+=fNPoT;

  fHS->FillHistoList("ETagAnalysis","NPoT",fNPoT);

  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  if( !(trigMask & (1 << 0)) ){
    fHS->FillHistoList("ETagAnalysis","NPoTNoPhys",fNPoT);
    TotPoTNoPhys += fNPoT;
  }
  
  if( (trigMask & (1 << 0)) ) {

    std::vector<float> leftClusterTimes, rightClusterTimes;
    Int_t NClusters =fEvent->ETagRecoCl->GetNElements();

    Int_t NClustersECAL = fEvent->ECalRecoCl->GetNElements();
    float totE = 0.;
    std::vector<float> ECALtimes;
    for (int icl = 0; icl < NClustersECAL; icl++) {
      ECALtimes.push_back(fEvent->ECalRecoCl->Element(icl)->GetTime());
      totE += fEvent->ECalRecoCl->Element(icl)->GetEnergy();
    }
    for(int icl = 0; icl < NClusters; icl++) {
      if (fEvent->ETagRecoCl->Element(icl)->GetChannelId() < 0) {
        leftClusterTimes.push_back(fEvent->ETagRecoCl->Element(icl)->GetTime());
      }
      else {
        rightClusterTimes.push_back(fEvent->ETagRecoCl->Element(icl)->GetTime());
      }
    }
    for (auto timeL : leftClusterTimes) {
      for (auto timeE : ECALtimes) {
        fHS->FillHistoList("ETagAnalysis", "LeftECalTimeDiff", timeL - timeE);
        if (totE > 250 && totE < 350)
          fHS->FillHistoList("ETagAnalysis", "LeftECalTimeDiffECalEnergyCut", timeL - timeE);
      }
    }
    for (auto timeR : rightClusterTimes) {
      for (auto timeE : ECALtimes) {
        fHS->FillHistoList("ETagAnalysis", "RightECalTimeDiff", timeR - timeE);
        if (totE > 250 && totE < 350)
          fHS->FillHistoList("ETagAnalysis", "RightECalTimeDiffECalEnergyCut", timeR - timeE);
      }
    }
    // if (NClusters == 2 && leftClusterTimes.size() == rightClusterTimes.size()) {
      for (auto timeL : leftClusterTimes) {
        for (auto timeR: rightClusterTimes) {
          fHS->FillHistoList("ETagAnalysis", "LeftRightTimeDiff", timeL - timeR);
          fHS->FillHistoList("ETagAnalysis", "LeftRightTimeSum", timeL + timeR);
        }
      }
    // }
    for (auto time : leftClusterTimes) {
      std::cout << "left cluster time: " << time << std::endl;
    }
    for (auto time : rightClusterTimes) {
      std::cout << "right cluster time: " << time << std::endl;
    }
    // for (size_t i = 0; i < leftClusters.size(); i++) {
    //   if (i == 1) break;
    //   // icl : leftClusters) {
    //   std::cout << "left cluster time: " << fEvent->ECalRecoCl->Element(leftClusters[i])->GetChannelId() << std::endl;
    // }
    // for (size_t i = 0; i < rightClusters.size(); i++) {
    //   if (i == 1) break;
    //   std::cout << "right cluster time: " << fEvent->ECalRecoCl->Element(rightClusters[i])->GetChannelId() << std::endl;
    // }


    fHS->FillHistoList("ETagAnalysis","NPoTPhys",fNPoT);
    TotPoTPhys += fNPoT;
    if(fNPoT>5E3) {
      fHS->FillHistoList("ETagAnalysis","NPoTPhys5K",fNPoT);
      TotPoTOver5K += fNPoT;
      fHS->FillHistoList("ETagAnalysis","XPos",fXPos);
      fHS->FillHistoList("ETagAnalysis","YPos",fYPos);
    }
    if(fNPoT>30E3) TotPoTOver30K += fNPoT;
  }  
//  vNPoT.push_back(fNPoT);
//  vNEvt.push_back(Neve);
  Neve++;
  return true;
}

Bool_t ETagAnalysis::Finalize()
{
 // TGraph* nPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vNPoT[0]);
 // fHS->SaveTGraphList("MyHistos","NPotVsTime",nPotVsTime);
  std::cout<<"TotPot        = "<<TotPoT<<std::endl;
  std::cout<<"TotPotPhys    = "<<TotPoTPhys<<std::endl;
  std::cout<<"TotPotNoPhys  = "<<TotPoTNoPhys<<std::endl;
  std::cout<<"TotPot Over30 = "<<TotPoTOver30K<<std::endl;
  std::cout<<"TotPot Over5  = "<<TotPoTOver5K<<std::endl;

  if (fVerbose) printf("---> Finalizing ETagAnalysis\n");
  return true;
}
