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
  
  binX=1001;   
  minX=-500.5;
  maxX=500.5;

  hname="DTimeECalSac";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="DTimeECalPVeto";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="DTimePVetoSac";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="DTimeCorrectPVetoSAC";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="DTimePVetoSACCh21";
  hSvc->BookHisto(hname, binX, minX, maxX);
  
  binX=101;   
  minX=-0.5;
  maxX=100.5;
  binY=700;   
  minY=-0.5;
  maxY=1400.5;
  hname="PVetoChIdClEnSAC";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);  
  
  binX=700;   
  minX=-0.5;
  maxX=1400.5;  
  hname="EnTotEvBremsstrhlungSACPVeto";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="SAC_ClEnergy_evNoBremsstrahlung";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="SAC_evSel_SinglePhotonAnnihilationEnergy_TimeCoincidenceRequest1ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  
  
  return true;
}

Bool_t EventSelection::ProcessSelection()
{
  
  Bool_t retCode = 0;
  TRecoVCluster* ecalclu=NULL;
  TRecoVCluster* SACclu=NULL;
  TRecoVHit* sacHit;
  TRecoVCluster* SACclu2=NULL;
  TRecoVCluster* PVetoclu=NULL;
  TRecoVHit* pVetoHit;
  std::string hname;
  int loop=0;
  
  HistoSvc* hSvc =  HistoSvc::GetInstance();
 
  Int_t NClSAC = fClCollSAC->GetNElements();
  Int_t NClECal = fClCollECal->GetNElements();
  Int_t NClPVeto = fClCollPVeto->GetNElements();
  Int_t NHitsSac = fhitEventSAC->GetNHits();
  Int_t NHitsPVeto = fhitEventPVeto->GetNHits();
  
  for(int jpveto=0; jpveto<NHitsPVeto; jpveto++){
    pVetoHit=fhitEventPVeto->Hit(jpveto);
    Double_t pvetoTimeCorr=applyTimePVetoLinCorr((pVetoHit->GetChannelId()), pVetoHit->GetTime());
    for(int jsac=0; jsac<NHitsSac; jsac++){
      sacHit=fhitEventSAC->Hit(jsac);
      if(sacHit->GetChannelId()!=21) continue;
      hname="DTimePVetoSACCh21";
      hSvc->FillHisto(hname, (pvetoTimeCorr-sacHit->GetTime()), 1.);
      
    }
  }
  
  for (Int_t jecal=0; jecal<NClECal; ++jecal){
    ecalclu          = fClCollECal->Element(jecal);
    Double_t ClTECal = ecalclu->GetTime();
    Double_t EnClECal= ecalclu->GetEnergy();
  
    for (Int_t jsac=0; jsac<NClSAC; ++jsac){
      SACclu          = fClCollSAC->Element(jsac);
      Double_t ClTsac = SACclu->GetTime();
      Double_t EnClsac= SACclu->GetEnergy();
      Double_t DTimeECalSAC = ClTECal-ClTsac ;
      hname="DTimeECalSac";
      hSvc->FillHisto(hname, DTimeECalSAC , 1.);
      if(fabs(ClTECal-ClTsac)<50.){ 
         hname="AnnihilationGammaEcalGammaSac";
         hSvc->FillHisto(hname, float(EnClECal) + float(EnClsac), 1.);
      } 
     }
   
    for (Int_t jpveto=0; jpveto<NClPVeto; ++jpveto){
      PVetoclu          = fClCollPVeto->Element(jpveto);
      Double_t ClTpv = PVetoclu->GetTime();
      Double_t EnClpv= PVetoclu->GetEnergy();
      Double_t DTimeECalPVeto = ClTECal-ClTpv;
      hname="DTimeECalPVeto";
      hSvc->FillHisto(hname, DTimeECalPVeto , 1.);
    
    }
 } 
 
   // loop on SAC-PVeto    
    for (Int_t jsac=0; jsac<NClSAC; ++jsac){
      SACclu          = fClCollSAC->Element(jsac);
      Double_t ClTsac = SACclu->GetTime();
      Double_t EnClsac= SACclu->GetEnergy();
      for (Int_t jpveto=0; jpveto<NClPVeto; ++jpveto){
        PVetoclu          = fClCollPVeto->Element(jpveto);
        Double_t ClTpv = PVetoclu->GetTime();
        Double_t xTimeLinCorr = applyTimePVetoLinCorr((float)PVetoclu->GetChannelId(), ClTpv);
        Double_t EnClpv= PVetoclu->GetEnergy();
        
        Double_t DTimePVetoSAC = ClTpv-ClTsac;
        hname="DTimePVetoSac";
        hSvc->FillHisto(hname, DTimePVetoSAC , 1.);
        Double_t DTimePVetoCorrectSAC = ClTsac-xTimeLinCorr;
        hname="DTimeCorrectPVetoSAC";
        hSvc->FillHisto(hname, DTimePVetoCorrectSAC , 1.);
        
        if(fabs(xTimeLinCorr-ClTsac)<1.){
          hname="EnTotEvBremsstrhlungSACPVeto";
          hSvc->FillHisto(hname, EnClsac+EnClpv , 1.);
          hname="PVetoChIdClEnSAC";
          hSvc->FillHisto2(hname, PVetoclu->GetChannelId(), EnClsac , 1.);
          
        }
        else{
          hname="SAC_ClEnergy_evNoBremsstrahlung";
          hSvc->FillHisto(hname, EnClsac, 1.);
          Bool_t Annihilation=true;
          for(int j2sac=jsac+1; j2sac< NClSAC; j2sac++){
               SACclu2   = fClCollSAC->Element(j2sac);
               if(fabs(SACclu->GetTime() - SACclu2->GetTime())<1. )
               {
                 Annihilation=false; 
               }
             }
             if(Annihilation){
               hname="SAC_evSel_SinglePhotonAnnihilationEnergy_TimeCoincidenceRequest1ns";
               hSvc->FillHisto(hname, SACclu->GetEnergy());
             }
           }
           
          }
          
        }
   
   return retCode;
}

Double_t EventSelection::applyTimePVetoLinCorr(Double_t  xChId, Double_t xTime)
{
  return xTime - (32.9 + 9.*(xChId - 13.)/65.);
}

