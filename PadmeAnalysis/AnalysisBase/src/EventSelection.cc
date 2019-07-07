#include "TTargetRecoBeam.hh"
#include "TRecoEvent.hh"
#include "TSACRecoEvent.hh"
#include "TECalRecoEvent.hh"
#include "TPVetoRecoEvent.hh"
#include "THEPVetoRecoEvent.hh"
#include "TEVetoRecoEvent.hh"
#include "TTargetRecoEvent.hh"
#include "TRecoVObject.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "HistoSvc.hh"
#include "TVector3.h"
#include <iostream>
#include "EventSelection.hh"

EventSelection::EventSelection()
{
  fRecoEvent   = NULL;

  fhitEventTarget= NULL;
  fhitEventECal= NULL;
  fhitEventPVeto= NULL;
  fhitEventEVeto= NULL;
  fhitEventHEPVeto= NULL;
  fhitEventSAC= NULL;

  fClCollTarget= NULL;
  fClCollECal= NULL;
  fClCollPVeto= NULL;
  fClCollEVeto= NULL;
  fClCollHEPVeto= NULL;
  fClCollSAC= NULL;
  
  /*fhitEvent    = NULL;
  fClColl      = NULL;*/
  fProcessingMode = 0;
  fVerbose        = 0;
}
EventSelection::EventSelection(Int_t processingMode, Int_t verbosityFlag)
{
  /*fRecoEvent   = NULL;

  fhitEventTarget= NULL;
  fhitEventECal= NULL;
  fhitEventPVeto= NULL;
  fhitEventEVeto= NULL;
  fhitEventHEPVeto= NULL;
  fhitEventSAC= NULL;

  fClCollTarget= NULL;
  fClCollECal= NULL;
  fClCollPVeto= NULL;
  fClCollEVeto= NULL;
  fClCollHEPVeto= NULL;
  fClCollSAC= NULL;*/

  fProcessingMode = processingMode;
  fVerbose        = verbosityFlag;
}
EventSelection::~EventSelection()
{
}


Bool_t EventSelection::InitSelection(TRecoEvent* evHeader, TRecoVObject* targetev, TTargetRecoBeam* targetcl, TRecoVObject* ecalev, TRecoVClusCollection* ecalcl, TRecoVObject* pvetoev, TRecoVClusCollection* pvetocl, TRecoVObject* evetoev, TRecoVClusCollection* evetocl, TRecoVObject* hepvetoev, TRecoVClusCollection* hepvetocl,TRecoVObject* sacev, TRecoVClusCollection* saccl)
{
  Bool_t retCode = 0;
  fRecoEvent = evHeader;

  fhitEventTarget = targetev;
  fClCollTarget = targetcl;

  fhitEventECal = ecalev;
  fClCollECal   = ecalcl;
  
  fhitEventPVeto = pvetoev;
  fClCollPVeto = pvetocl;

  fhitEventEVeto = evetoev;
  fClCollEVeto = evetocl;

  fhitEventHEPVeto = hepvetoev;
  fClCollHEPVeto = hepvetocl;

  fhitEventSAC = sacev;
  fClCollSAC = saccl;
  
  //std::cout <<"I'm in initHisto EventSelection " << long(fhitEventPVeto)<<std::endl;

  //std::cout << "I'm in Init  EventSelection" <<" fhitEventPVeto  "<<(TPVetoRecoEvent*)(fhitEventPVeto)->GetNHits()<<" fhitEventEVeto  "<<fhitEventEVeto->GetNHits()<< std::endl;
  return retCode;
}


Bool_t EventSelection::InitHistosSelection()
{
  //Validation histos
  //std::cout <<"I'm in initHistoValidation EventSelection" << std::endl;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

  int binX=100;   
  int minX=0;
  int maxX=100;
  int binY=40;
  int minY=0;
  int maxY=40;


  hname="PVetoChIdHitsVsEVetoChIdHits";
  hSvc->BookHisto2(hname, binX, minX, maxX, binX, minX, maxX);

  hname="PVetoChIdClustersVsEVetoChIdClusters";
  hSvc->BookHisto2(hname, binX, minX, maxX, binX, minX, maxX);

  hname="HEPVetoChIdHitsVsEVetoChIdHits";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);

  hname="HEPVetoChIdClustersVsEVetoChIdClusters";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);

  hname="HEPVetoChIdHitsVsPVetoChIdHits";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);

  hname="HEPVetoChIdClustersVsPVetoChIdClusters";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);




  int binT =  400;
  int minT = -100; 
  int maxT =  300;
 

  hname="PVetoChIdHitsVsPVetoHitsTime";
  hSvc->BookHisto2(hname, binT, minT, maxT, binX, minX, maxX); 
  
  hname="PVetoChIdClustersVsPVetoClustersTime";
  hSvc->BookHisto2(hname, binT, minT, maxT, binX, minX, maxX);  

  hname="EVetoChIdHitsVsEVetoHitsTime";
  hSvc->BookHisto2(hname, binT, minT, maxT, binX, minX, maxX);  

  hname="EVetoChIdClustersVsEVetoClustersTime";
  hSvc->BookHisto2(hname, binT, minT, maxT, binX, minX, maxX); 

  hname="HEPVetoChIdHitsVsHEPVetoHitsTime";
  hSvc->BookHisto2(hname, binT, minT, maxT, binY, minY, maxY); 

  hname="HEPVetoChIdClustersVsHEPVetoClustersTime";
  hSvc->BookHisto2(hname, binT, minT, maxT, binY, minY, maxY); 

  hname="HEPVetoHitsTimeVsPVetoHitsTime";
  hSvc->BookHisto2(hname, binT, minT, maxT, binT, minT, maxT); 

  hname="PVetoHitsTimeVsEVetoHitsTime";
  hSvc->BookHisto2(hname, binT, minT, maxT, binT, minT, maxT);

  hname="HEPVetoHitsTimeVsEVetoHitsTime";
  hSvc->BookHisto2(hname, binT, minT, maxT, binT, minT, maxT);

  hname="PVetoHitsTimeVsECalHitsTime";
  hSvc->BookHisto2(hname, binT, minT, maxT, binT, minT, maxT);

  hname="HEPVetoClustersTimeVsPVetoClustersTime"; 
  hSvc->BookHisto2(hname, binT, minT, maxT, binT, minT, maxT);  

  hname="PVetoClustersTimeVsEVetoClustersTime";
  hSvc->BookHisto2(hname, binT, minT, maxT, binT, minT, maxT);

  hname="HEPVetoClustersTimeVsEVetoClustersTime";
  hSvc->BookHisto2(hname, binT, minT, maxT, binT, minT, maxT);


  int binE =  500;
  int minE =    0; 
  int maxE =  500;
  
  hname="PVetoChIdClustersVsECalClustersEnergy";
  hSvc->BookHisto2(hname, binE, minE, maxE, binX, minX, maxX); 
 
  hname="PVetoChIdClustersVsECalClustersTime";
  hSvc->BookHisto2(hname, binT, minT, maxT, binX, minX, maxX); 

  
  
  //hname="PVetoChId";
  //hSvc->BookHisto(hname, nbin, minX, maxX);  
  
  return true;
}

Bool_t EventSelection::ProcessSelection()
{
  // << "I'm in process  EventSelection" <<" fhitEventPVeto  "<<fhitEventPVeto->GetNHits()<<" fhitEventEVeto  "<<fhitEventEVeto->GetNHits()<< std::endl;
  
  Bool_t retCode = 0;
  TRecoVHit* hepvetohit=NULL;
  TRecoVHit* pvetohit=NULL;
  TRecoVHit* evetohit=NULL;
  TRecoVHit* ecalhit=NULL;
  TRecoVCluster* pvetoclu=NULL;
  TRecoVCluster* hepvetoclu=NULL;
  TRecoVCluster* evetoclu=NULL;
  TRecoVCluster* ecalclu=NULL;
  std::string hname;
  int loop=0;

  HistoSvc* hSvc =  HistoSvc::GetInstance();
  //hname = "PVetoChIdVsEVetoChId";

  Int_t fNhitsPVeto   = fhitEventPVeto->GetNHits();
  Int_t fNhitsHEPVeto = fhitEventHEPVeto->GetNHits();
  Int_t fNhitsEVeto   = fhitEventEVeto->GetNHits();
  Int_t fNhitsECal    = fhitEventECal->GetNHits();

  Int_t fNClPVeto = fClCollPVeto->GetNElements();
  Int_t fNClHEPVeto = fClCollHEPVeto->GetNElements();
  Int_t fNClEVeto = fClCollEVeto->GetNElements();
  Int_t fNClECal  = fClCollECal ->GetNElements();
  
  //index i loop on hits
  for (Int_t ipv=0; ipv<fNhitsPVeto; ++ipv){ 

  pvetohit = fhitEventPVeto->Hit(ipv);
  Int_t    chIdPVeto = pvetohit->GetChannelId();
  Double_t HitTPVeto = pvetohit->GetTime();

  hname="PVetoChIdHitsVsPVetoHitsTime";
  hSvc->FillHisto2(hname, float(HitTPVeto), float(chIdPVeto),  1.);
  
  for (Int_t iev=0; iev<fNhitsEVeto; ++iev){
   
  evetohit = fhitEventEVeto->Hit(iev);
  Int_t chIdEVeto =evetohit->GetChannelId();
  Double_t HitTEVeto = evetohit->GetTime();
 
  hname="EVetoChIdHitsVsEVetoHitsTime";
  hSvc->FillHisto2(hname, float(HitTEVeto), float(chIdEVeto), 1.);
  
  hname="PVetoChIdHitsVsEVetoChIdHits";
  hSvc->FillHisto2(hname, float(chIdEVeto), float(chIdPVeto), 1.);

  hname="PVetoHitsTimeVsEVetoHitsTime";
  hSvc->FillHisto2(hname,  float(HitTEVeto), float(HitTPVeto), 1.);
  

  for (Int_t iecal=0; iecal<fNhitsECal; ++iecal){
   
  ecalhit = fhitEventECal->Hit(iecal);
  Int_t chIdECal =ecalhit->GetChannelId();
  Double_t HitTECal = ecalhit->GetTime();

  hname="PVetoHitsTimeVsECalHitsTime";
  hSvc->FillHisto2(hname, float(HitTECal), float(HitTPVeto), 1.);
  }
 
 for (Int_t ihepv=0; ihepv<fNhitsHEPVeto; ++ihepv){
 
  hepvetohit = fhitEventHEPVeto->Hit(ihepv);
  Int_t    chIdHEPVeto = hepvetohit->GetChannelId();
  Double_t HitTHEPVeto = hepvetohit->GetTime();

  hname="HEPVetoChIdHitsVsHEPVetoHitsTime";
  hSvc->FillHisto2(hname,  HitTHEPVeto, chIdHEPVeto, 1.);

  hname="HEPVetoChIdHitsVsPVetoChIdHits";
  hSvc->FillHisto2(hname, chIdPVeto, chIdHEPVeto, 1.);

  hname="HEPVetoChIdHitsVsEVetoChIdHits";
  hSvc->FillHisto2(hname, chIdEVeto, chIdHEPVeto, 1.);

  hname="HEPVetoHitsTimeVsPVetoHitsTime";
  hSvc->FillHisto2(hname, HitTPVeto, HitTHEPVeto, 1.);

  hname="HEPVetoHitsTimeVsEVetoHitsTime";
  hSvc->FillHisto2(hname, HitTEVeto, HitTHEPVeto, 1.);
  
 }//Nhits HEPveto
}//Nhits Eveto
 
}//Nhits Pveto

//  std::cout<<"  fNClHEPVeto "<<fNClHEPVeto<<"  fNClEVeto "<<fNClEVeto<<"  fNClPVeto "<<fNClPVeto<<" fNClECal "<<fNClECal<<std::endl;
 
//index j loop on clusters

for (Int_t jpv=0; jpv<fNClPVeto; ++jpv){
 
  pvetoclu    = fClCollPVeto->Element(jpv);
  Int_t chIdClPVeto =pvetoclu->GetChannelId();
  Double_t ClTPVeto = pvetoclu->GetTime();

  hname="PVetoChIdClustersVsPVetoClustersTime";
  hSvc->FillHisto2(hname, float(ClTPVeto), float(chIdClPVeto), 1.);
  
  
 for (Int_t jev=0; jev<fNClEVeto; ++jev){

  evetoclu    = fClCollEVeto->Element(jev);
  Int_t chIdClEVeto =evetoclu->GetChannelId();
  Double_t ClTEVeto  = evetoclu->GetTime();

  hname="EVetoChIdClustersVsEVetoClustersTime";
  hSvc->FillHisto2(hname, float(ClTEVeto), float(chIdClEVeto),  1.);

  hname="PVetoChIdClustersVsEVetoChIdClusters";
  hSvc->FillHisto2(hname, float(chIdClEVeto), float(chIdClPVeto), 1.);

  hname="PVetoClustersTimeVsEVetoClustersTime";
  hSvc->FillHisto2(hname, float(ClTEVeto), float(ClTPVeto), 1.);
 
 /* hname="EVetoChIdHitsVsEVetoHitsTime";
  hSvc->FillHisto2(hname, float(chIdEVeto), float(HitTEVeto), 1.);
  
  //std::cout<<" j "<<j<<" pveto "<<float(chIdPVeto)<<"  eveto "<<float(chIdEVeto)<<std::endl;
  hname="PVetoChIdHitsdVsEVetoChIdHits";
  hSvc->FillHisto2(hname, float(chIdPVeto), float(chIdEVeto), 1.);*/
  

 for (Int_t jecal=0; jecal<fNClECal; ++jecal){

  ecalclu    = fClCollECal->Element(jecal);
  Int_t chIdClECal =ecalclu->GetChannelId();
  Double_t ClTECal = ecalclu->GetTime();
  Double_t EnClECal= ecalclu->GetEnergy();
  hname="PVetoChIdClustersVsECalClustersEnergy";
  hSvc->FillHisto2(hname, float(EnClECal), float(chIdClPVeto), 1.);
  hname="PVetoChIdClustersVsECalClustersTime";
  hSvc->FillHisto2(hname, float(ClTECal), float(chIdClPVeto), 1.);
  }

 
 for (Int_t jhepv=0; jhepv<fNClHEPVeto; ++jhepv){
 
  hepvetoclu = fClCollHEPVeto->Element(jhepv);
  Int_t    chIdClHEPVeto = hepvetohit->GetChannelId();
  Double_t ClTHEPVeto = hepvetohit->GetTime();

  hname="HEPVetoChIdClustersVsHEPVetoClustersTime";
  hSvc->FillHisto2(hname,  float(ClTHEPVeto), float(chIdClHEPVeto),  1.);

  hname="HEPVetoChIdClustersVsEVetoChIdClusters";
  hSvc->FillHisto2(hname, float(chIdClEVeto), float(chIdClHEPVeto), 1.);

  hname="HEPVetoChIdClustersVsPVetoChIdClusters";
  hSvc->FillHisto2(hname, float(chIdClPVeto), float(chIdClHEPVeto), 1.);
 
  hname="HEPVetoClustersTimeVsEVetoClustersTime";
  hSvc->FillHisto2(hname, float(ClTEVeto), float(ClTHEPVeto), 1.);

  hname="HEPVetoClustersTimeVsPVetoClustersTime";
  hSvc->FillHisto2(hname, float(ClTPVeto), float(ClTHEPVeto), 1.);
  
  
  } //end N clusters HEPveto
 }//end N clusters Eveto
}//end N clusters Pveto

   
   return retCode;
}

