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
  hname="ECAL_gravTwoPhoton3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns1cmOfRadius";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns2cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns3cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns5cm"; 
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_twoPhotonInTime1.5ns";
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
  hname="ECal_xCoG";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_yCoG";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_y:xCog";
  hSvc->BookHisto2(hname, binX, minX, maxX, binX, minX, maxX);
  hname="ECal_xCoG_in3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_yCoG_in3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  binX=101;   
  minX=-0.5;
  maxX=10.5;
  hname="ECAL_ImpactParameter_3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  
  binX=101;   
  minX=-0.5;
  maxX=70.5;
  binY=70;   
  minY=-0.5;
  maxY=30.5;
  hname="InvariantMass_3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="InvariantMassByMomentum_3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMassCog_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_InvariantMassMomentumCog_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  
  hname="InvariantMass_1.5ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="InvariantMassByMomentum_1.5ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMassCog_1.5ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_InvariantMassMomentumCog_1.5ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="InvariantMass_GInFiducialRegion_1.5ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMassCog_GInFiducialRegion_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="InvariantMass_GInFiducialRegion_3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMassCog_GInFiducialRegion_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECal_InvariantMassLorentz_3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_InvariantMassLorentz_3ns1cm_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_InvariantMassLorentz_3ns2cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_InvariantMassLorentz_3ns2cm_withAngleCut";
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
  
 /* hname="InvariantMass_InvariantMassCog_SumEnergyLower550_3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMassCog_SumEnergyLower550_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="InvariantMassByMomentum_Seed_3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMassMomentumCog_Seed_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);*/
  
  
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
        //std::cout<< "x1 " << g1x<< " e1 " << g1E <<" x2 "<<g2x << " e2 "<< g2E<< std::endl;
        XWeighted= (g1x*g1E+g2x*g2E)/(g1E+g2E);
        YWeighted= (g1y*g1E+g2y*g2E)/(g1E+g2E);
        
        hname="ECal_xCoG";
        hSvc->FillHisto(hname,XWeighted , 1.);
        hname="ECal_yCoG";
        hSvc->FillHisto(hname,YWeighted , 1.);
        hname="ECal_y:xCog";
        hSvc->FillHisto2(hname, XWeighted, YWeighted, 1.);
        Double_t CoG= sqrt(XWeighted*XWeighted+YWeighted*YWeighted);
        if(CoG<1.){
          hname="ECAL_gravTwoPhoton3ns1cmOfRadius";
          hSvc->FillHisto(hname,g1E+g2E , 1.);
        }
        if(fabs(XWeighted)<10. && fabs(YWeighted)<10.){
          /*g1x=ecalclu->GetChannelId()/100*21.;
          g1y=ecalclu->GetChannelId()%100*21.;
          g2x=ecalclu2->GetChannelId()/100*21.;
          g2y=ecalclu2->GetChannelId()%100*21.;*/
          
          hname="ECAL_gravTwoPhoton3ns1cm";
          hSvc->FillHisto(hname,g1E+g2E , 1.);
          Double_t Distance2Gamma1_2   = (g1x-g2x)*(g1x-g2x)+(g1y-g2y)*(g1y-g2y);
          hname="ECal_xCoG_in3ns1cm";
          hSvc->FillHisto(hname,XWeighted , 1.);
          hname="ECal_yCoG_in3ns1cm";
          hSvc->FillHisto(hname,YWeighted , 1.);
          
          
          Bool_t InFiducialRegion=false;
          //Bool_t OutOfEnergy=false;
          if((fabs((g1x))>90. || fabs(g1y)>90.) && (fabs((g2x))>90. || fabs(g2y)>90. )) InFiducialRegion=true;
          //if(g1E+g1E>550) OutOfEnergy=true;
          Double_t InvariantMass  = sqrt(g1E*g2E * Distance2Gamma1_2 /(3470.*3470.));
          
          hname="ECAL_InvariantMassCog_3ns1cm";
          hSvc->FillHisto2(hname, InvariantMass, CoG , 1.);
          hname="InvariantMass_3ns1cm";
          hSvc->FillHisto(hname, InvariantMass, 1.);
          if(InFiducialRegion){
            hname="ECAL_InvariantMassCog_GInFiducialRegion_3ns1cm";
            hSvc->FillHisto2(hname, InvariantMass, CoG , 1.);
            hname="InvariantMass_GInFiducialRegion_3ns1cm";
            hSvc->FillHisto(hname, InvariantMass, 1.);
          }
          /*if (!OutOfEnergy){
            hname="ECAL_InvariantMassCog_SumEnergyLower550_3ns1cm";
            hSvc->FillHisto2(hname, InvariantMass, CoG , 1.);
            hname="InvariantMass_InvariantMassCog_SumEnergyLower550_3ns1cm";
            hSvc->FillHisto(hname, InvariantMass, 1.);
          }  */       
          
         //Double_t R_1 = sqrt(g1x*g1x+ g1y*g1y+ecalclu->GetPosition().Z()*ecalclu->GetPosition().Z());
         Double_t R_1 = sqrt(g1x*g1x+ g1y*g1y+3470*3470);
         Double_t Px_1 = g1E*g1x/ R_1;
         Double_t Py_1 = g1E*g1y/ R_1;
         //Double_t Pz_1 = g1E*ecalclu->GetPosition().Z()/ R_1;
         //Double_t R_2 = sqrt(g2x*g2x+ g2y*g2y+ecalclu2->GetPosition().Z()*ecalclu2->GetPosition().Z());
         Double_t Pz_1 = g1E*3470/ R_1;
         Double_t R_2 = sqrt(g2x*g2x+ g2y*g2y+3470*3470);
         Double_t Px_2 = g2E*g2x/ R_2;
         Double_t Py_2 = g2E*g2y/ R_2;
         //Double_t Pz_2 = g2E*ecalclu2->GetPosition().Z()/ R_2;
         Double_t Pz_2 = g2E*3470/ R_2;
         
         
         TLorentzVector P4g1F,P4g2F,SumP;
         P4g1F.SetPxPyPzE(Px_1,Py_1,Pz_1, g1E);
         P4g2F.SetPxPyPzE(Px_2,Py_2,Pz_2,g2E);
         SumP=P4g1F+P4g2F;
         SumP.M();
         hname="ECal_InvariantMassLorentz_3ns1cm";
         hSvc->FillHisto(hname, SumP.M(), 1.);
         Double_t angleBetweenTwoPhoton = P4g1F.Angle(P4g2F.Vect());
         //std::cout << "TLOrebtz angle " << angleBetweenTwoPhoton<<  std::endl;
         hname="ECal_angleBetweenTwoPhotonIn3ns1cm";
         hSvc->FillHisto(hname, angleBetweenTwoPhoton, 1.);
         hname ="ECal_angleBetweenTwoPhotonVSEnergyIn3ns1cm";
         hSvc->FillHisto2(hname, g1E+g2E,  angleBetweenTwoPhoton, 1.);
         if(angleBetweenTwoPhoton>0.08 && angleBetweenTwoPhoton<0.12){
           hname="ECal_InvariantMassLorentz_3ns1cm_withAngleCut";
           hSvc->FillHisto(hname, SumP.M(), 1.);
         }
         
          Double_t ImpactParameter = fabs(ecalclu->GetPosition().Y()-ecalclu->GetPosition().X()*((ecalclu2->GetPosition().Y()-ecalclu->GetPosition().Y())/(ecalclu2->GetPosition().X()-ecalclu->GetPosition().X())))/sqrt(1+((ecalclu2->GetPosition().Y()-ecalclu->GetPosition().Y())/(ecalclu2->GetPosition().X()-ecalclu->GetPosition().X()))*((ecalclu2->GetPosition().Y()-ecalclu->GetPosition().Y())/(ecalclu2->GetPosition().X()-ecalclu->GetPosition().X())));
          hname="ECAL_ImpactParameter_3ns1cm";
          hSvc->FillHisto(hname, ImpactParameter, 1.);
         
          Double_t InvariantMassSecondCalculation =sqrt((g1E+g2E)*(g1E+g2E)-(Px_1+Px_2)*(Px_1+Px_2)-(Py_1+Py_2)*(Py_1+Py_2)-(Pz_1+Pz_2)*(Pz_1+Pz_2));
          hname="InvariantMassByMomentum_3ns1cm";
          hSvc->FillHisto(hname, InvariantMassSecondCalculation, 1.);
          hname="ECAL_InvariantMassMomentumCog_3ns1cm";
          hSvc->FillHisto2(hname, InvariantMassSecondCalculation , CoG , 1.);
          if(InFiducialRegion){
            hname="ECAL_InvariantMassCog_GInFiducialRegion_3ns1cm";
            hSvc->FillHisto2(hname, InvariantMass, CoG , 1.);
            hname="InvariantMass_GInFiducialRegion_3ns1cm";
            hSvc->FillHisto(hname, InvariantMass, 1.);
          }
         
         
        }
        if(fabs(XWeighted)<20. && fabs(YWeighted)<20.){
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
          SumP.M();
          Double_t angleBetweenTwoPhoton = P4g1F.Angle(P4g2F.Vect());
          
          hname="ECal_InvariantMassLorentz_3ns2cm";
          hSvc->FillHisto(hname, SumP.M(), 1.);
          //std::cout << "TLOrebtz angle " << angleBetweenTwoPhoton<<  std::endl;
          hname="ECal_angleBetweenTwoPhotonIn3ns2cm";
          hSvc->FillHisto(hname, angleBetweenTwoPhoton, 1.);
          hname ="ECal_angleBetweenTwoPhotonVSEnergyIn3ns2cm";
          hSvc->FillHisto2(hname, g1E+g2E,  angleBetweenTwoPhoton, 1.);
          if(angleBetweenTwoPhoton>0.08 && angleBetweenTwoPhoton<0.12){
           hname="ECal_InvariantMassLorentz_3ns2cm_withAngleCut";
           hSvc->FillHisto(hname, SumP.M(), 1.);
         }
          
          hname="ECAL_gravTwoPhoton3ns2cm";
          hSvc->FillHisto(hname,g1E+g2E , 1.);
        }
        if(fabs(XWeighted)<30. && fabs(YWeighted)<30.){
          hname="ECAL_gravTwoPhoton3ns3cm";
          hSvc->FillHisto(hname,g1E+g2E , 1.);
        }
        if(fabs(XWeighted)<50. && fabs(YWeighted)<50.){
          hname="ECAL_gravTwoPhoton3ns5cm";
          hSvc->FillHisto(hname,g1E+g2E , 1.);
        }
        
          
          Int_t IndexSeed=ecalclu->GetSeed();
          Int_t IndexSeed2=ecalclu2->GetSeed();
          hit=fhitEventECal->Hit(IndexSeed);
          hit2=fhitEventECal->Hit(IndexSeed2);
          Double_t XWeightedHit= (hit->GetPosition().X()*g1E+hit2->GetPosition().X()*g2E)/(g1E+g2E);
          Double_t YWeightedHit= (hit->GetPosition().Y()*g1E+hit2->GetPosition().Y()*g2E)/(g1E+g2E);
          if(fabs(XWeightedHit)<10. && fabs(YWeightedHit)<10.){
         Double_t CoG= sqrt(XWeightedHit*XWeightedHit+YWeightedHit*YWeightedHit);
         Double_t R_1 = sqrt(hit->GetPosition().X()*hit->GetPosition().X()+ hit->GetPosition().Y()*hit->GetPosition().Y()+3470*3470);
         Double_t Px_1 = g1E*hit->GetPosition().X()/ R_1;
         Double_t Py_1 = g1E*hit->GetPosition().Y()/ R_1;
         //Double_t Pz_1 = sqrt(hit->GetEnergy()*hit->GetEnergy()-Px_1*Px_1-Py_1*Py_1);
         Double_t Pz_1 = g1E*3470/ R_1;
         Double_t R_2 = sqrt(hit2->GetPosition().X()*hit2->GetPosition().X()+ hit2->GetPosition().Y()*hit2->GetPosition().Y()+3470*3470);
         Double_t Px_2 = g2E*hit2->GetPosition().X()/ R_2;
         Double_t Py_2 = g2E*hit2->GetPosition().Y()/ R_2;
         //Double_t Pz_2 = sqrt(hit2->GetEnergy()*hit2->GetEnergy()-Px_2*Px_2-Py_2*Py_2);
         Double_t Pz_2 = g2E*3470/ R_2;
         Double_t InvariantMassSecondCalculation =sqrt((g1E+g2E)*(g1E+g2E)-(Px_1+Px_2)*(Px_1+Px_2)-(Py_1+Py_2)*(Py_1+Py_2)-(Pz_1+Pz_2)*(Pz_1+Pz_2));
         /* hname="InvariantMassByMomentum_Seed_3ns1cm";
          hSvc->FillHisto(hname, InvariantMassSecondCalculation, 1.);
          hname="ECAL_InvariantMassMomentumCog_Seed_3ns1cm";
          hSvc->FillHisto2(hname, InvariantMassSecondCalculation , CoG , 1.);*/
            
          }
          
      }
      if(fabs(ecalclu->GetTime()-ecalclu2->GetTime())<1.5){
        Double_t g1E=ecalclu->GetEnergy();
        Double_t g2E=ecalclu2->GetEnergy();
        hname="ECAL_twoPhotonInTime1.5ns";
        hSvc->FillHisto(hname,g1E+g2E , 1.);
        Double_t g1x=ecalclu->GetPosition().X();
        Double_t g1y=ecalclu->GetPosition().Y();
        Double_t g2x=ecalclu2->GetPosition().X();
        Double_t g2y=ecalclu2->GetPosition().Y();
        Double_t XWeighted=0.;
        Double_t YWeighted=0.;
        /*
        Double_t g1x=ecalclu->GetPosition().X()*21.;
        Double_t g1y=ecalclu->GetPosition().Y()*21.;
        Double_t g2x=ecalclu2->GetPosition().X()*21.;
        Double_t g2y=ecalclu2->GetPosition().Y()*21.;
        */
        
        //<<"chid " << ecalclu->GetChannelId() << std::endl;
        XWeighted= (g1x*g1E+g2x*g2E)/(g1E+g2E);
        YWeighted= (g1y*g1E+g2y*g2E)/(g1E+g2E);
        if(fabs(XWeighted)<10. && fabs(YWeighted)<10.){
         /* g1x=ecalclu->GetChannelId()/100*21.;
          g1y=ecalclu->GetChannelId()%100*21.;
          g2x=ecalclu2->GetChannelId()/100*21.;
          g2y=ecalclu2->GetChannelId()%100*21.;*/
          hname="ECAL_gravTwoPhoton1.5ns1cm";
          hSvc->FillHisto(hname,g1E+g2E , 1.);
          Double_t Distance2Gamma1_2   = (g1x-g2x)*(g1x-g2x)+(g1y-g2y)*(g1y-g2y);
          Double_t InvariantMass  = sqrt(g1E*g2E * Distance2Gamma1_2 /(3470.*3470.));
          hname="InvariantMass_1.5ns1cm";
          hSvc->FillHisto(hname, InvariantMass, 1.);
          Double_t CoG= sqrt(XWeighted*XWeighted+YWeighted*YWeighted);
          hname="ECAL_InvariantMassCog_1.5ns1cm";
          hSvc->FillHisto2(hname, InvariantMass, CoG , 1.);
          
          Bool_t InFiducialRegion=false;
          if((fabs((g1x))>90. || fabs(g1y)>90.) && (fabs((g2x))>90. || fabs(g2y)>90. )) InFiducialRegion=true;
          
         //Double_t R_1 = sqrt(g1x*g1x+ g1y*g1y+ecalclu->GetPosition().Z()*ecalclu->GetPosition().Z());
         Double_t R_1 = sqrt(g1x*g1x+ g1y*g1y+3470*3470);
         Double_t Px_1 = g1E*g1x/ R_1;
         Double_t Py_1 = g1E*g1y/ R_1;
         //Double_t Pz_1 = g1E*ecalclu->GetPosition().Z()/ R_1;
         //Double_t R_2 = sqrt(g2x*g2x+ g2y*g2y+ecalclu2->GetPosition().Z()*ecalclu2->GetPosition().Z());
         Double_t Pz_1 = g1E*3470/ R_1;
         Double_t R_2 = sqrt(g2x*g2x+ g2y*g2y+3470*3470);
         Double_t Px_2 = g2E*g2x/ R_2;
         Double_t Py_2 = g2E*g2y/ R_2;
         //Double_t Pz_2 = g2E*ecalclu2->GetPosition().Z()/ R_2;
         Double_t Pz_2 = g2E*3470/ R_2;
         Double_t InvariantMassSecondCalculation =sqrt((g1E+g2E)*(g1E+g2E)-(Px_1+Px_2)*(Px_1+Px_2)-(Py_1+Py_2)*(Py_1+Py_2)-(Pz_1+Pz_2)*(Pz_1+Pz_2));
          hname="InvariantMassByMomentum_1.5ns1cm";
          hSvc->FillHisto(hname, InvariantMassSecondCalculation, 1.);
          hname="ECAL_InvariantMassMomentumCog_1.5ns1cm";
          hSvc->FillHisto2(hname, InvariantMassSecondCalculation , CoG , 1.);
        
        }
          
        if(fabs(XWeighted)<20. && fabs(YWeighted)<20.){
          hname="ECAL_gravTwoPhoton1.5ns2cm";
          hSvc->FillHisto(hname,ecalclu->GetEnergy()+ecalclu2->GetEnergy() , 1.);
        }
        if(fabs(XWeighted)<30. && fabs(YWeighted)<30.){
          hname="ECAL_gravTwoPhoton1.5ns3cm";
          hSvc->FillHisto(hname,ecalclu->GetEnergy()+ecalclu2->GetEnergy() , 1.);
        }
        if(fabs(XWeighted)<50. && fabs(YWeighted)<50.){
          hname="ECAL_gravTwoPhoton1.5ns5cm";
          hSvc->FillHisto(hname,ecalclu->GetEnergy()+ecalclu2->GetEnergy() , 1.);
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
        
        if(fabs(xTimeLinCorr-ClTsac)<.2){
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

