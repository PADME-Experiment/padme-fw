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
  
  return retCode;
}


Bool_t EventSelection::InitHistosSelection()
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

  int binX=500;   
  int minX=0;
  int maxX=1500;
  int binY=40;
  int minY=0;
  int maxY=40;

  hname="AnnihilationGammaEcalGammaSac"; 
  hSvc->BookHisto(hname, binX, minX, maxX); 
  
  return true;
}

Bool_t EventSelection::ProcessSelection()
{
  
  Bool_t retCode = 0;
  TRecoVCluster* ecalclu=NULL;
  TRecoVCluster* SACclu=NULL;
  std::string hname;
  int loop=0;
  
  HistoSvc* hSvc =  HistoSvc::GetInstance();
 
  Int_t NClSAC = fClCollSAC->GetNElements();
  Int_t NClECal = fClCollECal->GetNElements();
   
  for (Int_t jecal=0; jecal<NClECal; ++jecal){
  ecalclu          = fClCollECal->Element(jecal);
  Double_t ClTECal = ecalclu->GetTime();
  Double_t EnClECal= ecalclu->GetEnergy();
  
  for (Int_t jsac=0; jsac<NClSAC; ++jsac){
    SACclu          = fClCollSAC->Element(jsac);
    Double_t ClTsac = SACclu->GetTime();
    Double_t EnClsac= SACclu->GetEnergy();
    if(fabs(ClTECal-ClTsac)<50.){ 
       hname="AnnihilationGammaEcalGammaSac";
       hSvc->FillHisto(hname, float(EnClECal) + float(EnClsac), 1.);
    } 
   }
  
  }
   
   return retCode;
}

