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
#include "TMath.h"
#include "TLorentzVector.h"
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
  int maxX=2000;
  int binY=40;
  int minY=0;
  int maxY=40;

  hname="AnnihilationGammaEcalGammaSac"; 
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_twoPhotonInTime3ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns_InDeltaPhi";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns1cm_InDeltaPhi";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns1cm_InDeltaPhi_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns2cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns3cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns5cm"; 
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_twoPhotonInTime1.5ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton1.5ns_InDeltaPhi";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton1.5ns1cm_InDeltaPhi";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton1.5ns1cm_InDeltaPhi_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton1.5ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton1.5ns2cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton1.5ns3cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton1.5ns5cm"; 
  hSvc->BookHisto(hname, binX, minX, maxX);
  
  binX=601;   
  minX=-300.5;
  maxX=300.5;
  hname="ECal_xCoG_3ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_yCoG_3ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_y:xCog_3ns";
  hSvc->BookHisto2(hname, binX, minX, maxX, binX, minX, maxX);
  hname="ECal_xCoG_in3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_yCoG_in3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_ImpactParameter_3ns";
  hSvc->BookHisto(hname, binX, -0.5, maxX);
  hname="ECal_xCoG_1.5ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_yCoG_1.5ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_y:xCog_1.5ns";
  hSvc->BookHisto2(hname, binX, minX, maxX, binX, minX, maxX);
  hname="ECal_xCoG_in1.5ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_yCoG_in1.5ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_ImpactParameter_1.5ns";
  hSvc->BookHisto(hname, binX, -0.5, maxX);
  
  binX=101;   
  minX=-0.5;
  maxX=70.5;
  binY=70;   
  minY=-0.5;
  maxY=30.5;
  hname="InvariantMass_GInFiducialRegion_3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMassCog_GInFiducialRegion_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="InvariantMass_GInFiducialRegion_1.5ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMassCog_GInFiducialRegion_1.5ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_InvariantMass_InDeltaPhi_3ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMass_InDeltaPhi_3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMass_InDeltaPhi_3ns1cm_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMass_InDeltaPhi_1.5ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMass_InDeltaPhi_1.5ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMass_InDeltaPhi_1.5ns1cm_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_InvariantMassLorentz_3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_CoGVsInvariantMassLorentz_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECal_InvariantMassLorentz_3ns1cm_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_InvariantMassLorentz_3ns2cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_InvariantMassLorentz_3ns2cm_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  
  hname="ECal_InvariantMassLorentz_1.5ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_CoGVsInvariantMassLorentz_1.5ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECal_InvariantMassLorentz_1.5ns1cm_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_InvariantMassLorentz_1.5ns2cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_InvariantMassLorentz_1.5ns2cm_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  
  
  binX=501;   
  minX=-0.5;
  maxX=2000.5;
  binY=700;   
  minY=-0.5;
  maxY=0.2;
  
  hname="ECal_angleBetweenTwoPhotonIn3ns1cm";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname ="ECal_angleBetweenTwoPhotonVSEnergyIn3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECal_angleBetweenTwoPhotonIn3ns2cm";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname ="ECal_angleBetweenTwoPhotonVSEnergyIn3ns2cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECal_angleBetweenTwoPhotonIn1.5ns1cm";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname ="ECal_angleBetweenTwoPhotonVSEnergyIn1.5ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECal_angleBetweenTwoPhotonIn1.5ns2cm";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname ="ECal_angleBetweenTwoPhotonVSEnergyIn1.5ns2cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  
  binY=400;   
  minY=-300.5;
  maxY=300.5;
  
  hname="ECAL_XGamma1_3ns1cm";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_XGamma2_3ns1cm";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_YGamma1_3ns1cm";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_YGamma2_3ns1cm";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_XGamma1_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_XGamma2_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_YGamma1_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_YGamma2_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto(hname, binY, minY, maxY);
  
  hname="ECAL_XGamma1_1.5ns1cm";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_XGamma2_1.5ns1cm";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_YGamma1_1.5ns1cm";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_YGamma2_1.5ns1cm";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_XGamma1_1.5ns1cm_InInvariantMassRange";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_XGamma2_1.5ns1cm_InInvariantMassRange";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_YGamma1_1.5ns1cm_InInvariantMassRange";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_YGamma2_1.5ns1cm_InInvariantMassRange";
  hSvc->BookHisto(hname, binY, minY, maxY);
  
  binX=501;   
  minX=-0.5;
  maxX=500.5;
  binY=200;   
  minY=-0.5;
  maxY=0.1;  
  hname="ECAL_EGamma2VSEGamma1_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binX, minX, maxX);
  hname="ECAL_Theta1VSEGamma1_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Theta2VSEGamma2_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Theta2VSTheta1_3ns1cm";
  hSvc->BookHisto2(hname, binY, minY, maxY, binY, minY, maxY);
  binY=500;   
  minY=-6.16;
  maxY=6.16; 
  hname="ECAL_Phi1VSEGamma1_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Phi2VSEGamma2_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Phi2VSPhi1_3ns1cm";
  hSvc->BookHisto2(hname, binY, minY, maxY, binY, minY, maxY);
  
  hname="ECAL_EGamma2VSEGamma1_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binX, minX, maxX, binX, minX, maxX);
  binY=200;   
  minY=-0.5;
  maxY=0.1;
  hname="ECAL_Theta1VSEGamma1_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Theta2VSEGamma2_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Theta2VSTheta1_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binY, minY, maxY, binY, minY, maxY);
  binY=500;   
  minY=-6.16;
  maxY=6.16;
  hname="ECAL_Phi1VSEGamma1_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Phi2VSEGamma2_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Phi2VSPhi1_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binY, minY, maxY, binY, minY, maxY);
  
  binX=501;   
  minX=-0.5;
  maxX=500.5;
  binY=200;   
  minY=-0.5;
  maxY=0.1; 
  hname="ECAL_EGamma2VSEGamma1_1.5ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binX, minX, maxX);
  hname="ECAL_Theta1VSEGamma1_1.5ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Theta2VSEGamma2_1.5ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Theta2VSTheta1_1.5ns1cm";
  hSvc->BookHisto2(hname, binY, minY, maxY, binY, minY, maxY);
  binY=500;   
  minY=-6.16;
  maxY=6.16;
  hname="ECAL_Phi1VSEGamma1_1.5ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Phi2VSEGamma2_1.5ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Phi2VSPhi1_1.5ns1cm";
  hSvc->BookHisto2(hname, binY, minY, maxY, binY, minY, maxY);
  
  binY=200;   
  minY=-0.5;
  maxY=0.1;
  hname="ECAL_EGamma2VSEGamma1_1.5ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binX, minX, maxX, binX, minX, maxX);
  hname="ECAL_Theta1VSEGamma1_1.5ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Theta2VSEGamma2_1.5ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Theta2VSTheta1_1.5ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binY, minY, maxY, binY, minY, maxY);
  binY=500;   
  minY=-6.16;
  maxY=6.16;
  hname="ECAL_Phi1VSEGamma1_1.5ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Phi2VSEGamma2_1.5ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Phi2VSPhi1_1.5ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binY, minY, maxY, binY, minY, maxY);
  
  binX=71;   
  minX=-0.5;
  maxX=30.5;
  binY=500;   
  minY=-6.16;
  maxY=6.16;
  hname="ECAL_Phi1VsInvariantMass_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Phi2VsInvariantMass_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  binY=200;   
  minY=-0.5;
  maxY=0.1; 
  hname="ECAL_Theta1VsInvariantMass_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Theta2VsInvariantMass_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  
  
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
  hname="SAC_evSel_SinglePhotonAnnihilationEnergy_TimeCoincidenceRequest2ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  
  
  return true;
}

Bool_t EventSelection::ProcessSelection()
{
  
  Bool_t retCode = 0;
  TRecoVCluster* ecalclu=NULL;
  TRecoVCluster* ecalclu2=NULL;
  TRecoVCluster* SACclu=NULL;
  TRecoVHit* sacHit;
  TRecoVHit* hit;
  TRecoVHit* hit2;
  TRecoVHit* targetHit;
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
    for(Int_t jecal2=jecal+1; jecal2<NClECal; jecal2++){
      ecalclu2 = fClCollECal->Element(jecal2);
      if(fabs(ecalclu->GetTime()-ecalclu2->GetTime())<3.){
        hname="ECAL_twoPhotonInTime3ns";
        hSvc->FillHisto(hname,ecalclu->GetEnergy()+ecalclu2->GetEnergy() , 1.);
        Double_t XWeighted=0.;
        Double_t YWeighted=0.;
        Double_t g1x=ecalclu->GetPosition().X();
        Double_t g1y=ecalclu->GetPosition().Y();
        Double_t g2x=ecalclu2->GetPosition().X();
        Double_t g2y=ecalclu2->GetPosition().Y();
        Double_t g1E=ecalclu->GetEnergy();
        Double_t g2E=ecalclu2->GetEnergy();
        XWeighted= (g1x*g1E+g2x*g2E)/(g1E+g2E);
        YWeighted= (g1y*g1E+g2y*g2E)/(g1E+g2E);
        
        hname="ECal_xCoG_3ns";
        hSvc->FillHisto(hname,XWeighted , 1.);
        hname="ECal_yCoG_3ns";
        hSvc->FillHisto(hname,YWeighted , 1.);
        hname="ECal_y:xCog_3ns";
        hSvc->FillHisto2(hname, XWeighted, YWeighted, 1.);
        
        
        Double_t CoG= sqrt(XWeighted*XWeighted+YWeighted*YWeighted);
        Double_t R_1 = sqrt(g1x*g1x+ g1y*g1y+3470*3470);
        Double_t Px_1 = g1E*g1x/ R_1;
        Double_t Py_1 = g1E*g1y/ R_1;
        Double_t Pz_1 = g1E*3470/ R_1;
        Double_t R_2 = sqrt(g2x*g2x+ g2y*g2y+3470*3470);
        Double_t Px_2 = g2E*g2x/ R_2;
        Double_t Py_2 = g2E*g2y/ R_2;
        Double_t Pz_2 = g2E*3470/ R_2;
         
        TLorentzVector P4g1F,P4g2F,SumP;
        P4g1F.SetPxPyPzE(Px_1,Py_1,Pz_1, g1E);
        P4g2F.SetPxPyPzE(Px_2,Py_2,Pz_2,g2E);
        SumP=P4g1F+P4g2F;
        Double_t InvariantMass = SumP.M();
        Double_t angleBetweenTwoPhoton = P4g1F.Angle(P4g2F.Vect());
         
        Double_t ImpactParameter = fabs(ecalclu->GetPosition().Y()-ecalclu->GetPosition().X()*((ecalclu2->GetPosition().Y()-ecalclu->GetPosition().Y())/(ecalclu2->GetPosition().X()-ecalclu->GetPosition().X())))/sqrt(1+((ecalclu2->GetPosition().Y()-ecalclu->GetPosition().Y())/(ecalclu2->GetPosition().X()-ecalclu->GetPosition().X()))*((ecalclu2->GetPosition().Y()-ecalclu->GetPosition().Y())/(ecalclu2->GetPosition().X()-ecalclu->GetPosition().X())));
        hname="ECAL_ImpactParameter_3ns";
        hSvc->FillHisto(hname, ImpactParameter, 1.);
        
        Double_t thetag1 = P4g1F.Theta();
        Double_t thetag2 = P4g2F.Theta(); 
        Double_t phig1 = P4g1F.Phi();
        Double_t phig2 = P4g2F.Phi();
        
        Bool_t InInvariantMassRange = false;
        if(InvariantMass>12. && InvariantMass< 20) InInvariantMassRange=true;
        
        Bool_t InFiducialRegion=false;
        if((fabs((g1x))>90. || fabs(g1y)>90.) && (fabs((g2x))>90. || fabs(g2y)>90. )) InFiducialRegion=true;
        
        Bool_t InDeltaPhi = false;
        Double_t distance = sqrt((g1x-g2x)*(g1x-g2x)+(g1y-g2y)*(g1y-g2y));
        if(/*distance >R_1 &&*/ (phig2*phig1<0 && (fabs(phig1-phig2)-3.14)<0.1) || (phig2*phig1>0 && (fabs(phig1+phig2)-3.14)< 0.1)) InDeltaPhi= true;
        std::cout<< "DPhi " << fabs(phig1-phig2) << " bool "<< InDeltaPhi << std::endl;
        
        if(InDeltaPhi){
          hname="ECAL_gravTwoPhoton3ns_InDeltaPhi";
          hSvc->FillHisto(hname,g1E+g2E , 1.);
          hname="ECAL_InvariantMass_InDeltaPhi_3ns";
          hSvc->FillHisto(hname, InvariantMass, 1.);
        }
         
        if(fabs(XWeighted)<10. && fabs(YWeighted)<10.){
         hname="ECAL_gravTwoPhoton3ns1cm";
         hSvc->FillHisto(hname,g1E+g2E , 1.);
         hname="ECal_xCoG_in3ns1cm";
         hSvc->FillHisto(hname,XWeighted , 1.);
         hname="ECal_yCoG_in3ns1cm";
         hSvc->FillHisto(hname,YWeighted , 1.);
         hname="ECal_InvariantMassLorentz_3ns1cm";
         hSvc->FillHisto(hname, InvariantMass, 1.);
         hname="ECal_CoGVsInvariantMassLorentz_3ns1cm";
         hSvc->FillHisto2(hname, InvariantMass, CoG,  1.);
         hname="ECal_angleBetweenTwoPhotonIn3ns1cm";
         hSvc->FillHisto(hname, angleBetweenTwoPhoton, 1.);
         hname ="ECal_angleBetweenTwoPhotonVSEnergyIn3ns1cm";
         hSvc->FillHisto2(hname, g1E+g2E,  angleBetweenTwoPhoton, 1.);
         hname="ECAL_Phi1VsInvariantMass_3ns1cm";
         hSvc->FillHisto2(hname, InvariantMass,  phig1, 1.);
         hname="ECAL_Phi2VsInvariantMass_3ns1cm";
         hSvc->FillHisto2(hname, InvariantMass,  phig2, 1.);
         hname="ECAL_Theta1VsInvariantMass_3ns1cm";
         hSvc->FillHisto2(hname, InvariantMass,  thetag1, 1.);
         hname="ECAL_Theta2VsInvariantMass_3ns1cm";
         hSvc->FillHisto2(hname, InvariantMass,  thetag1, 1.);
         
         if(InDeltaPhi){
          hname="ECAL_gravTwoPhoton3ns1cm_InDeltaPhi";
          hSvc->FillHisto(hname,g1E+g2E , 1.);
          hname="ECAL_InvariantMass_InDeltaPhi_3ns1cm";
          hSvc->FillHisto(hname, InvariantMass, 1.);
        }
         if(angleBetweenTwoPhoton>0.08 && angleBetweenTwoPhoton<0.12){
           hname="ECal_InvariantMassLorentz_3ns1cm_withAngleCut";
           hSvc->FillHisto(hname, SumP.M(), 1.);
           if(InDeltaPhi){
            hname="ECAL_gravTwoPhoton3ns1cm_InDeltaPhi_withAngleCut";
            hSvc->FillHisto(hname,g1E+g2E , 1.);
            hname="ECAL_InvariantMass_InDeltaPhi_3ns1cm_withAngleCut";
            hSvc->FillHisto(hname, InvariantMass, 1.);
          }
         }
         if(InFiducialRegion){
            hname="ECAL_InvariantMassCog_GInFiducialRegion_3ns1cm";
            hSvc->FillHisto2(hname, InvariantMass, CoG , 1.);
            hname="InvariantMass_GInFiducialRegion_3ns1cm";
            hSvc->FillHisto(hname, InvariantMass, 1.);
          }
          
          
          hname="ECAL_XGamma1_3ns1cm";
          hSvc->FillHisto(hname, g1x, 1.);
          hname="ECAL_XGamma2_3ns1cm";
          hSvc->FillHisto(hname, g2x, 1.);
          hname="ECAL_YGamma1_3ns1cm";
          hSvc->FillHisto(hname, g1y, 1.);
          hname="ECAL_YGamma2_3ns1cm";
          hSvc->FillHisto(hname, g2y, 1.);
          
          hname="ECAL_EGamma2VSEGamma1_3ns1cm";
          hSvc->FillHisto2(hname, g1E , g2E , 1.);
          hname="ECAL_Theta1VSEGamma1_3ns1cm";
          hSvc->FillHisto2(hname, g1E , thetag1 , 1.);
          hname="ECAL_Theta2VSEGamma2_3ns1cm";
          hSvc->FillHisto2(hname, g2E , thetag2 , 1.);
          hname="ECAL_Theta2VSTheta1_3ns1cm";
          hSvc->FillHisto2(hname,  thetag1 , thetag2, 1.);
          hname="ECAL_Phi1VSEGamma1_3ns1cm";
          hSvc->FillHisto2(hname, g1E , phig1 , 1.);
          hname="ECAL_Phi2VSEGamma2_3ns1cm";
          hSvc->FillHisto2(hname, g2E , phig2 , 1.);
          hname="ECAL_Phi2VSPhi1_3ns1cm";
          hSvc->FillHisto2(hname,  phig1 , phig2, 1.);
          
          if(InInvariantMassRange){
            hname="ECAL_XGamma1_3ns1cm_InInvariantMassRange";
            hSvc->FillHisto(hname, g1x, 1.);
            hname="ECAL_XGamma2_3ns1cm_InInvariantMassRange";
            hSvc->FillHisto(hname, g2x, 1.);
            hname="ECAL_YGamma1_3ns1cm_InInvariantMassRange";
            hSvc->FillHisto(hname, g1y, 1.);
            hname="ECAL_YGamma2_3ns1cm_InInvariantMassRange";
            hSvc->FillHisto(hname, g2y, 1.);
            
            hname="ECAL_EGamma2VSEGamma1_3ns1cm_InInvariantMassRange";
            hSvc->FillHisto2(hname, g1E , g2E , 1.);
            hname="ECAL_Theta1VSEGamma1_3ns1cm_InInvariantMassRange";
            hSvc->FillHisto2(hname, g1E , thetag1 , 1.);
            hname="ECAL_Theta2VSEGamma2_3ns1cm_InInvariantMassRange";
            hSvc->FillHisto2(hname, g2E , thetag2 , 1.);
            hname="ECAL_Theta2VSTheta1_3ns1cm_InInvariantMassRange";
            hSvc->FillHisto2(hname,  thetag1 , thetag2, 1.);
            hname="ECAL_Phi1VSEGamma1_3ns1cm_InInvariantMassRange";
            hSvc->FillHisto2(hname, g1E , phig1 , 1.);
            hname="ECAL_Phi2VSEGamma2_3ns1cm_InInvariantMassRange";
            hSvc->FillHisto2(hname, g2E , phig2 , 1.);
            hname="ECAL_Phi2VSPhi1_3ns1cm_InInvariantMassRange";
            hSvc->FillHisto2(hname,  phig1 , phig2, 1.);
            
            
          }
        }
        
        
        if(fabs(XWeighted)<20. && fabs(YWeighted)<20.){
          hname="ECAL_gravTwoPhoton3ns2cm";
          hSvc->FillHisto(hname,g1E+g2E , 1.);      
          hname="ECal_InvariantMassLorentz_3ns2cm";
          hSvc->FillHisto(hname, InvariantMass, 1.);
          hname="ECal_angleBetweenTwoPhotonIn3ns2cm";
          hSvc->FillHisto(hname, angleBetweenTwoPhoton, 1.);
          hname ="ECal_angleBetweenTwoPhotonVSEnergyIn3ns2cm";
          hSvc->FillHisto2(hname, g1E+g2E,  angleBetweenTwoPhoton, 1.);
          if(angleBetweenTwoPhoton>0.08 && angleBetweenTwoPhoton<0.12){
           hname="ECal_InvariantMassLorentz_3ns2cm_withAngleCut";
           hSvc->FillHisto(hname, SumP.M(), 1.);
          }
        }
        
        if(fabs(XWeighted)<30. && fabs(YWeighted)<30.){
          hname="ECAL_gravTwoPhoton3ns3cm";
          hSvc->FillHisto(hname,g1E+g2E , 1.);
        }
        
        if(fabs(XWeighted)<50. && fabs(YWeighted)<50.){
          hname="ECAL_gravTwoPhoton3ns5cm";
          hSvc->FillHisto(hname,g1E+g2E , 1.);
        }
      }
   
      if(fabs(ecalclu->GetTime()-ecalclu2->GetTime())<1.5){
        hname="ECAL_twoPhotonInTime1.5ns";
        hSvc->FillHisto(hname,ecalclu->GetEnergy()+ecalclu2->GetEnergy() , 1.);
        Double_t XWeighted=0.;
        Double_t YWeighted=0.;
        Double_t g1x=ecalclu->GetPosition().X();
        Double_t g1y=ecalclu->GetPosition().Y();
        Double_t g2x=ecalclu2->GetPosition().X();
        Double_t g2y=ecalclu2->GetPosition().Y();
        Double_t g1E=ecalclu->GetEnergy();
        Double_t g2E=ecalclu2->GetEnergy();
        XWeighted= (g1x*g1E+g2x*g2E)/(g1E+g2E);
        YWeighted= (g1y*g1E+g2y*g2E)/(g1E+g2E);
        
        hname="ECal_xCoG_1.5ns";
        hSvc->FillHisto(hname,XWeighted , 1.);
        hname="ECal_yCoG_1.5ns";
        hSvc->FillHisto(hname,YWeighted , 1.);
        hname="ECal_y:xCog_1.5ns";
        hSvc->FillHisto2(hname, XWeighted, YWeighted, 1.);
        
        Double_t CoG= sqrt(XWeighted*XWeighted+YWeighted*YWeighted);
        Double_t R_1 = sqrt(g1x*g1x+ g1y*g1y+3470*3470);
        Double_t Px_1 = g1E*g1x/ R_1;
        Double_t Py_1 = g1E*g1y/ R_1;
        Double_t Pz_1 = g1E*3470/ R_1;
        Double_t R_2 = sqrt(g2x*g2x+ g2y*g2y+3470*3470);
        Double_t Px_2 = g2E*g2x/ R_2;
        Double_t Py_2 = g2E*g2y/ R_2;
        Double_t Pz_2 = g2E*3470/ R_2;
         
        TLorentzVector P4g1F,P4g2F,SumP;
        P4g1F.SetPxPyPzE(Px_1,Py_1,Pz_1, g1E);
        P4g2F.SetPxPyPzE(Px_2,Py_2,Pz_2,g2E);
        SumP=P4g1F+P4g2F;
        Double_t InvariantMass = SumP.M();
        Double_t angleBetweenTwoPhoton = P4g1F.Angle(P4g2F.Vect());
         
        Double_t ImpactParameter = fabs(ecalclu->GetPosition().Y()-ecalclu->GetPosition().X()*((ecalclu2->GetPosition().Y()-ecalclu->GetPosition().Y())/(ecalclu2->GetPosition().X()-ecalclu->GetPosition().X())))/sqrt(1+((ecalclu2->GetPosition().Y()-ecalclu->GetPosition().Y())/(ecalclu2->GetPosition().X()-ecalclu->GetPosition().X()))*((ecalclu2->GetPosition().Y()-ecalclu->GetPosition().Y())/(ecalclu2->GetPosition().X()-ecalclu->GetPosition().X())));
        hname="ECAL_ImpactParameter_1.5ns";
         hSvc->FillHisto(hname, ImpactParameter, 1.);
         
        Double_t thetag1 = P4g1F.Theta();
        Double_t thetag2 = P4g2F.Theta(); 
        Double_t phig1 = P4g1F.Phi();
        Double_t phig2 = P4g2F.Phi();
         
        Bool_t InFiducialRegion=false;
        if((fabs((g1x))>90. || fabs(g1y)>90.) && (fabs((g2x))>90. || fabs(g2y)>90. )) InFiducialRegion=true;
        
        Bool_t InInvariantMassRange = false;
        if(InvariantMass>12. && InvariantMass< 20) InInvariantMassRange=true;
        
        Bool_t InDeltaPhi = false; // DeltaPhi<2.5Deg
        Double_t distance = sqrt((g1x-g2x)*(g1x-g2x)+(g1y-g2y)*(g1y-g2y));
        if(distance >R_1 && fabs(phig1-phig2)<0.04) InDeltaPhi= true;
        
        
        if(InDeltaPhi){
          hname="ECAL_gravTwoPhoton1.5ns_InDeltaPhi";
          hSvc->FillHisto(hname,g1E+g2E , 1.);
          hname="ECAL_InvariantMass_InDeltaPhi_1.5ns";
          hSvc->FillHisto(hname, InvariantMass, 1.);
        }
         
        if(fabs(XWeighted)<10. && fabs(YWeighted)<10.){
         hname="ECAL_gravTwoPhoton1.5ns1cm";
         hSvc->FillHisto(hname,g1E+g2E , 1.);
         hname="ECal_xCoG_in1.5ns1cm";
         hSvc->FillHisto(hname,XWeighted , 1.);
         hname="ECal_yCoG_in1.5ns1cm";
         hSvc->FillHisto(hname,YWeighted , 1.);
         hname="ECal_InvariantMassLorentz_1.5ns1cm";
         hSvc->FillHisto(hname, InvariantMass, 1.);
         hname="ECal_CoGVsInvariantMassLorentz_1.5ns1cm";
         hSvc->FillHisto2(hname, InvariantMass, CoG,  1.);
         hname="ECal_angleBetweenTwoPhotonIn1.5ns1cm";
         hSvc->FillHisto(hname, angleBetweenTwoPhoton, 1.);
         hname ="ECal_angleBetweenTwoPhotonVSEnergyIn1.5ns1cm";
         hSvc->FillHisto2(hname, g1E+g2E,  angleBetweenTwoPhoton, 1.);
         
         if(InDeltaPhi){
          hname="ECAL_gravTwoPhoton1.5ns1cm_InDeltaPhi";
          hSvc->FillHisto(hname,g1E+g2E , 1.);
          hname="ECAL_InvariantMass_InDeltaPhi_1.5ns1cm";
          hSvc->FillHisto(hname, InvariantMass, 1.);
        }
        
         if(angleBetweenTwoPhoton>0.08 && angleBetweenTwoPhoton<0.12){
           hname="ECal_InvariantMassLorentz_1.5ns1cm_withAngleCut";
           hSvc->FillHisto(hname, InvariantMass, 1.);
           if(InDeltaPhi){
            hname="ECAL_gravTwoPhoton1.5ns1cm_InDeltaPhi_withAngleCut";
            hSvc->FillHisto(hname,g1E+g2E , 1.);
            hname="ECAL_InvariantMass_InDeltaPhi_1.5ns1cm_withAngleCut";
            hSvc->FillHisto(hname, InvariantMass, 1.);
          }
         }
         if(InFiducialRegion){
            hname="ECAL_InvariantMassCog_GInFiducialRegion_1.5ns1cm";
            hSvc->FillHisto2(hname, InvariantMass, CoG , 1.);
            hname="InvariantMass_GInFiducialRegion_1.5ns1cm";
            hSvc->FillHisto(hname, InvariantMass, 1.);
          }
          
          hname="ECAL_XGamma1_1.5ns1cm";
          hSvc->FillHisto(hname, g1x, 1.);
          hname="ECAL_XGamma2_1.5ns1cm";
          hSvc->FillHisto(hname, g2x, 1.);
          hname="ECAL_YGamma1_1.5ns1cm";
          hSvc->FillHisto(hname, g1y, 1.);
          hname="ECAL_YGamma2_1.5ns1cm";
          hSvc->FillHisto(hname, g2y, 1.);
          
          hname="ECAL_EGamma2VSEGamma1_1.5ns1cm";
          hSvc->FillHisto2(hname, g1E , g2E , 1.);
          hname="ECAL_Theta1VSEGamma1_1.5ns1cm";
          hSvc->FillHisto2(hname, g1E , thetag1 , 1.);
          hname="ECAL_Theta2VSEGamma2_1.5ns1cm";
          hSvc->FillHisto2(hname, g2E , thetag2 , 1.);
          hname="ECAL_Theta2VSTheta1_1.5ns1cm";
          hSvc->FillHisto2(hname,  thetag1 , thetag2, 1.);
          hname="ECAL_Phi1VSEGamma1_1.5ns1cm";
          hSvc->FillHisto2(hname, g1E , phig1 , 1.);
          hname="ECAL_Phi2VSEGamma2_1.5ns1cm";
          hSvc->FillHisto2(hname, g2E , phig2 , 1.);
          hname="ECAL_Phi2VSPhi1_1.5ns1cm";
          hSvc->FillHisto2(hname,  phig1 , phig2 ,1.);
          
          if(InInvariantMassRange){
            hname="ECAL_XGamma1_1.5ns1cm_InInvariantMassRange";
            hSvc->FillHisto(hname, g1x, 1.);
            hname="ECAL_XGamma2_1.5ns1cm_InInvariantMassRange";
            hSvc->FillHisto(hname, g2x, 1.);
            hname="ECAL_YGamma1_1.5ns1cm_InInvariantMassRange";
            hSvc->FillHisto(hname, g1y, 1.);
            hname="ECAL_YGamma2_1.5ns1cm_InInvariantMassRange";
            hSvc->FillHisto(hname, g2y, 1.);
            
            hname="ECAL_EGamma2VSEGamma1_1.5ns1cm_InInvariantMassRange";
            hSvc->FillHisto2(hname, g1E , g2E , 1.);
            hname="ECAL_Theta1VSEGamma1_1.5ns1cm_InInvariantMassRange";
            hSvc->FillHisto2(hname, g1E , thetag1 , 1.);
            hname="ECAL_Theta2VSEGamma2_1.5ns1cm_InInvariantMassRange";
            hSvc->FillHisto2(hname, g2E , thetag2 , 1.);
            hname="ECAL_Theta2VSTheta1_1.5ns1cm_InInvariantMassRange";
            hSvc->FillHisto2(hname,  thetag1 , thetag2, 1.);
            hname="ECAL_Phi1VSEGamma1_1.5ns1cm_InInvariantMassRange";
            hSvc->FillHisto2(hname, g1E , phig1 , 1.);
            hname="ECAL_Phi2VSEGamma2_1.5ns1cm_InInvariantMassRange";
            hSvc->FillHisto2(hname, g2E , phig2 , 1.);
            hname="ECAL_Phi2VSPhi1_1.5ns1cm_InInvariantMassRange";
            hSvc->FillHisto2(hname,  phig1 , phig2 ,1.);
            
            
          }
        }
        
        
        if(fabs(XWeighted)<20. && fabs(YWeighted)<20.){
          hname="ECAL_gravTwoPhoton1.5ns2cm";
          hSvc->FillHisto(hname,g1E+g2E , 1.);      
          hname="ECal_InvariantMassLorentz_1.5ns2cm";
          hSvc->FillHisto(hname, InvariantMass, 1.);
          hname="ECal_angleBetweenTwoPhotonIn1.5ns2cm";
          hSvc->FillHisto(hname, angleBetweenTwoPhoton, 1.);
          hname ="ECal_angleBetweenTwoPhotonVSEnergyIn1.5ns2cm";
          hSvc->FillHisto2(hname, g1E+g2E,  angleBetweenTwoPhoton, 1.);
          if(angleBetweenTwoPhoton>0.08 && angleBetweenTwoPhoton<0.12){
           hname="ECal_InvariantMassLorentz_1.5ns2cm_withAngleCut";
           hSvc->FillHisto(hname, SumP.M(), 1.);
          }
        }
        
        if(fabs(XWeighted)<30. && fabs(YWeighted)<30.){
          hname="ECAL_gravTwoPhoton1.5ns3cm";
          hSvc->FillHisto(hname,g1E+g2E , 1.);
        }
        
        if(fabs(XWeighted)<50. && fabs(YWeighted)<50.){
          hname="ECAL_gravTwoPhoton1.5ns5cm";
          hSvc->FillHisto(hname,g1E+g2E , 1.);
        }
        
    }
   }
    
  
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
        
        if(fabs(xTimeLinCorr-ClTsac)<.1){
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
               if(fabs(SACclu->GetTime() - SACclu2->GetTime())<2. )
               {
                 Annihilation=false; 
               }
             }
             if(Annihilation){
               hname="SAC_evSel_SinglePhotonAnnihilationEnergy_TimeCoincidenceRequest2ns";
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

