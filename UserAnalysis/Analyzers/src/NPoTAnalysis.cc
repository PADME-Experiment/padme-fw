// Written by E. Leonardi 
// Last modified by M. Raggi 2/08/2021
#include "NPoTAnalysis.hh"

NPoTAnalysis::NPoTAnalysis(TString cfgFile, Int_t verbose)
{
  Neve=0;
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating NPoTAnalysis\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  fHS = HistoSvc::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
}

NPoTAnalysis::~NPoTAnalysis(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t NPoTAnalysis::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing NPoTAnalysis\n");
  fEvent = event;
  InitHistos();
  return true;
}

Bool_t NPoTAnalysis::InitHistos(){
  // NPoTAnalysis directory will contain all histograms related to this analysis
  fHS->CreateList("NPoTAnalysis");
  fHS->BookHistoList("NPoTAnalysis","NPoT",600,-1000.,59000.);
  fHS->BookHistoList("NPoTAnalysis","NPoTNoPhys",600,-1000.,59000.);
  fHS->BookHistoList("NPoTAnalysis","NPoTPhys",600,-1000.,59000.);
  fHS->BookHistoList("NPoTAnalysis","NPoTPhys5K",600,-1000.,59000.);

  fHS->BookHistoList("NPoTAnalysis","XPos",200,-25.,25.);
  fHS->BookHistoList("NPoTAnalysis","YPos",200,-25.,25.);
  return true;
}

Bool_t NPoTAnalysis::Process(){

  if(!fEvent->TargetRecoBeam) return 0;

  fNPoT = fEvent->TargetRecoBeam->getnPOT();
  fXPos = fEvent->TargetRecoBeam->getX();
  fYPos = fEvent->TargetRecoBeam->getY();
  TotPoT+=fNPoT;

  fHS->FillHistoList("NPoTAnalysis","NPoT",fNPoT);

  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  if( !(trigMask & (1 << 0)) ){
    fHS->FillHistoList("NPoTAnalysis","NPoTNoPhys",fNPoT);
    TotPoTNoPhys += fNPoT;
  }
  
  if( (trigMask & (1 << 0)) ) {
    fHS->FillHistoList("NPoTAnalysis","NPoTPhys",fNPoT);
    TotPoTPhys += fNPoT;
    if(fNPoT>5E3) {
      fHS->FillHistoList("NPoTAnalysis","NPoTPhys5K",fNPoT);
      TotPoTOver5K += fNPoT;
      fHS->FillHistoList("NPoTAnalysis","XPos",fXPos);
      fHS->FillHistoList("NPoTAnalysis","YPos",fYPos);
    }
    if(fNPoT>30E3) TotPoTOver30K += fNPoT;
  }  
//  vNPoT.push_back(fNPoT);
//  vNEvt.push_back(Neve);
  Neve++;
  return true;
}

Bool_t NPoTAnalysis::Finalize()
{
 // TGraph* nPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vNPoT[0]);
 // fHS->SaveTGraphList("MyHistos","NPotVsTime",nPotVsTime);
  std::cout<<"TotPot        = "<<TotPoT<<std::endl;
  std::cout<<"TotPotPhys    = "<<TotPoTPhys<<std::endl;
  std::cout<<"TotPotNoPhys  = "<<TotPoTNoPhys<<std::endl;
  std::cout<<"TotPot Over30 = "<<TotPoTOver30K<<std::endl;
  std::cout<<"TotPot Over5  = "<<TotPoTOver5K<<std::endl;

  if (fVerbose) printf("---> Finalizing NPoTAnalysis\n");
  return true;
}
