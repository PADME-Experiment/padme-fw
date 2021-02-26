#include "DarkPhoton.hh"

#include "TRecoEvent.hh"
#include "TRecoVObject.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "TTargetRecoBeam.hh"
#include "HistoSvc.hh"
#include "TVector3.h"
#include "TLorentzVector.h"
#include <iostream>


DarkPhoton::DarkPhoton()
{

 
  fInitToComplete=true;
  
  fRecoEvent   = NULL;

  fECal_hitEvent=0;
  fECal_ClColl=0;
  
  fPVeto_hitEvent=0;
  fPVeto_ClColl=0;
  
  fEVeto_hitEvent=0;
  fEVeto_ClColl=0;
  
  fSAC_hitEvent=0;
  fSAC_ClColl=0;
  
  fTarget_hitEvent=0;
  fTarget_RecoBeam=0;
  
  fHEPVeto_hitEvent=0;
  fHEPVeto_ClColl=0;

  fProcessingMode = 0;
  fVerbose        = 0;

 
}
DarkPhoton::DarkPhoton(Int_t processingMode, Int_t verbosityFlag)
{
  
  fRecoEvent   = NULL;
  fProcessingMode = processingMode;
  fVerbose        = verbosityFlag;

 
}
DarkPhoton::~DarkPhoton()
{
}

Bool_t DarkPhoton::Init(TRecoEvent* eventHeader, 
			    TRecoVObject* ECALev, TRecoVClusCollection* ECALcl,
			    TRecoVObject* PVetoev, TRecoVClusCollection* PVetocl,
			    TRecoVObject* EVetoev, TRecoVClusCollection* EVetocl,
			    TRecoVObject* HEPVetoev, TRecoVClusCollection* HEPVetocl,
			    TRecoVObject* SACev, TRecoVClusCollection* SACcl,
			    TRecoVObject* TARGETev, TTargetRecoBeam* TargetBeam)
{


  //  Bool_t retCode = 0;


  fRecoEvent = eventHeader;
  fECal_hitEvent      =ECALev   ;
  fPVeto_hitEvent     =PVetoev  ;
  fEVeto_hitEvent     =EVetoev  ;
  fSAC_hitEvent       =SACev    ;
  fTarget_hitEvent    =TARGETev ;
  fHEPVeto_hitEvent   =HEPVetoev;

  fECal_ClColl      =ECALcl   ;
  fPVeto_ClColl     =PVetocl  ;
  fEVeto_ClColl     =EVetocl  ;
  fSAC_ClColl       =SACcl    ;
  fTarget_RecoBeam  =TargetBeam ;
  fHEPVeto_ClColl   =HEPVetocl ;

  
  return true;
}



Double_t DarkPhoton::MissingMass(TVector3 V, Double_t E){
  double PositronE  = 490 + 0.511;
  double PositronPz = sqrt(PositronE*PositronE-0.511*0.511);
  TLorentzVector Positron (0,0,PositronPz,PositronE);
  TLorentzVector Electron (0,0,         0,    0.511);
  Double_t x = V.x();Double_t y = V.y();Double_t z = V.z();
  Double_t r = sqrt(x*x+y*y+z*z);
  double Px = E*x/r;
  double Py = E*y/r;
  double Pz = E*z/r;
  TLorentzVector Photon   (Px,Py,Pz,E);
  TLorentzVector Missing  (0,0,0,0); 
  Missing  = Positron + Electron - Photon;

  return    Missing*Missing;
}


Bool_t DarkPhoton::passPreselection()
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();

  
  Bool_t passed = false;
  std::string hname="PreSelectCutFlow";
  hSvc->FillHisto(hname,ps_cut_all);

  if (!fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) 
    {
      // std::cout<<"in passPreselection, event status bit  "<< fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)<< std::endl;
      if (!fRecoEvent->GetTriggerMaskBit(TRECOEVENT_TRIGMASKBIT_BEAM)) return passed;
    }
  hSvc->FillHisto(hname,ps_cut_trg);
  //std::cout<<"reco ev " <<fRecoEvent->GetEventNumber() << std::endl;
  // if( fTarget_RecoBeam->getnPOT()>3000 && fTarget_RecoBeam->getnPOT()<6000) std::cout << "event      " <<fRecoEvent->GetEventNumber() <<  "       po/bunch      " <<  fTarget_RecoBeam->getnPOT() << std::endl;
 
  // if(fRecoEvent->GetEventNumber()<290000 || fRecoEvent->GetEventNumber()>375000)return passed;
  Double_t targetConst=1.78; //to use olny if the run is taken from September 2020 !!!!
  //Double_t targetConst=1; 
  std::string hname1 = "NposInBunch_beam";
  hSvc->FillHisto(hname1,fTarget_RecoBeam->getnPOT()*targetConst);
  //std::cout<<"preselection...Pot control" << std::endl;
  //if (fTarget_RecoBeam->getnPOT()<12000. && fTarget_RecoBeam->getnPOT()>3500.) return passed;
  if (fTarget_RecoBeam->getnPOT()<-9999) return passed;
  hSvc->FillHisto(hname,ps_cut_POT);
  //if (fSAC_ClColl->GetNElements()>15) return passed;
  /* if(fSAC_ClColl->GetNElements()<2){
     std::cout << "I'm in sacClu rejection loop " <<std::endl;
     hname = "EventNumberRejectedBySAC";
     hSvc->FillHisto(hname1,fRecoEvent->GetEventNumber());
     if(fSAC_ClColl->GetNElements()>0.001){
     hname = "EventNumberRejectedBySAC_not0";
     hSvc->FillHisto(hname1,fRecoEvent->GetEventNumber());
     }
     return passed;
     }*/
  //std::cout<<"eventNumber " << fRecoEvent->GetEventNumber() << " in SAC " << fSAC_ClColl->GetNElements() << " in target " << fTarget_RecoBeam->getnPOT() << std::endl;
  //if(fRecoEvent->GetEventNumber()>270000) return passed;

  hname = "nPOT";
  hSvc->FillHisto(hname,0,float(fTarget_RecoBeam->getnPOT()*targetConst)); 
  //if(fSAC_ClColl->GetNElements() > 10. ) return passed;
  //std::cout<<"passed ok " << std::endl;
  
  passed = true;
  return passed;
}




Bool_t DarkPhoton::ProcessDarkPhoton()
{
  
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
 
  Bool_t isMC=false;
  //std::cout<<"in ProcessAnalysis ... evsel ... "<<fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)<<std::endl;
  if (fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) {
    isMC=true;
    //std::cout<<"input data are simulatetd "<<std::endl;
  }
 
  // apply BTF bunch preselection 
  //if (!passPreselection()) return true;

  //std::cout<<"Time " << fRecoEvent->GetEventTime().GetNanoSec() << std::endl;


  std::vector<Int_t> hitVInCl;
  
  Bool_t retCode = 0;
  TRecoVCluster* ecalclu=NULL;
  TRecoVCluster* ecalclu2=NULL;
  TRecoVCluster* SACclu=NULL;
  
  TRecoVCluster* PVetoclu=NULL;
    
  
  Double_t ShiftECalSac;
  Double_t ShiftECalPVeto;
  Double_t ShiftECalEVeto;
  if(isMC){
    ShiftECalSac=0;
    ShiftECalPVeto=0;
    ShiftECalEVeto=0;
  }
  else{
    ShiftECalSac=0;
    ShiftECalPVeto=0;
    ShiftECalEVeto=1;
  }

  Int_t NClECal = fECal_ClColl->GetNElements();
  Int_t NClSAC = fSAC_ClColl->GetNElements();
  Int_t NClPVeto = fPVeto_ClColl->GetNElements();
  Int_t NClEVeto = fEVeto_ClColl->GetNElements();
  
 
  for (Int_t jecal=0; jecal<NClECal; ++jecal){
    ecalclu          = fECal_ClColl->Element(jecal);
    Double_t ClTECal = ecalclu->GetTime();
    Double_t distanceTarget=3470;
    //Double_t distanceTarget=3577;
    Double_t g1x=ecalclu->GetPosition().X();
    Double_t g1y=ecalclu->GetPosition().Y();
    Double_t g1E=ecalclu->GetEnergy();
    Double_t R_1 = sqrt(g1x*g1x+ g1y*g1y+distanceTarget*distanceTarget);
    Double_t Px_1 = g1E*g1x/ R_1;
    Double_t Py_1 = g1E*g1y/ R_1;
    Double_t Pz_1 = g1E*distanceTarget/ R_1;
    TLorentzVector P4g1F, P4eTarget, P4eBeam;
    P4g1F.SetPxPyPzE(Px_1,Py_1,Pz_1, g1E);
    Double_t EBeam=450.0;
    Double_t me=0.511;
    Double_t PzBeam=sqrt(EBeam*EBeam-me*me);
    P4eTarget.SetPxPyPzE(0,0,0, me);
    P4eBeam.SetPxPyPzE(0,0,PzBeam, EBeam);
    Double_t MissingMass=(P4eTarget+P4eBeam-P4g1F)*(P4eTarget+P4eBeam-P4g1F);

    Bool_t PassEneThr=false;
    if(g1E>50){
      PassEneThr=true;
      hname="MissingMass_AllECALclu_ThrEne50MeV";
      hSvc->FillHisto(hname,MissingMass , 1.);
    }
    Bool_t NoInTime10=true;
    Bool_t NoInTime20=true;
    for(Int_t jecal2=jecal+1; jecal2<NClECal; jecal2++){                ///starting second cluster loop
      ecalclu2 = fECal_ClColl->Element(jecal2);
      if(fabs(ecalclu->GetTime()-ecalclu2->GetTime())<20.){
	NoInTime20=false;
      }
      if(fabs(ecalclu->GetTime()-ecalclu2->GetTime())<10.){
	NoInTime10=false;
      }

    }

    if(NoInTime20 && PassEneThr){
      hname="MissingMass_NoClInTime20ns_ThrEne50MeV";
      hSvc->FillHisto(hname,MissingMass, 1.);
      Bool_t CoincidencePVeto=true;
      for(int jpveto=0; jpveto<NClPVeto; jpveto++){
	PVetoclu=fPVeto_ClColl->Element(jpveto);
	Double_t cluTime=PVetoclu->GetTime();
	Double_t cluZpos=PVetoclu->GetPosition().Z();
	Double_t cluXpos=PVetoclu->GetPosition().X();
	if(fabs(ClTECal-cluTime-ShiftECalPVeto)<2.){
	  Double_t momentumPositron=CalculateMomentumPositron(cluZpos,cluXpos);
	  if(fabs(momentumPositron+g1E-490)<50){
	    CoincidencePVeto=false;
	  }
	}
      }
    }//end of nointime20


    if(NoInTime10 && PassEneThr){
      hname="MissingMass_NoClInTime10ns_ThrEne50MeV";
      hSvc->FillHisto(hname,MissingMass, 1.);
      Bool_t CoincidencePVeto=true;
      Bool_t CoincidenceSAC=true;
      for(int jpveto=0; jpveto<NClPVeto; jpveto++){
	PVetoclu=fPVeto_ClColl->Element(jpveto);
	Double_t cluTime=PVetoclu->GetTime();
	Double_t cluZpos=PVetoclu->GetPosition().Z();
	Double_t cluXpos=PVetoclu->GetPosition().X();
	if(fabs(ClTECal-cluTime-ShiftECalPVeto)<2.){
	  Double_t momentumPositron=CalculateMomentumPositron(cluZpos,cluXpos);
	  if(fabs(momentumPositron+g1E-490)<50){
	    CoincidencePVeto=false;
	  }
	}
      }
 
      for(int jsac=0; jsac<NClSAC; jsac++){
	SACclu=fSAC_ClColl->Element(jsac);
	Double_t cluTime=SACclu->GetTime();
	if(fabs(ClTECal-cluTime-ShiftECalSac)<2. && SACclu->GetEnergy()>50){
	  CoincidenceSAC=false;
	}
	// }
      }
      if(CoincidencePVeto){
	hname="MissingMass_NoClInTime10ns_ThrEne50MeV_NoCoincidencePVeto";
	hSvc->FillHisto(hname,MissingMass, 1.);
	if(CoincidenceSAC){
	  hname="MissingMass_NoClInTime10ns_ThrEne50MeV_NoCoincidencePVeto_NoCoincidenceSAC";
	  hSvc->FillHisto(hname,MissingMass, 1.);
	}
      }

    }//end of noinTime10

  }

  return retCode;
 
}


Double_t DarkPhoton::CalculateMomentumPositron(Double_t z,Double_t x){
  //return ((z+631.5)*(z+631.5)+x*x)/2/x*0.3*0.3986;
  return ((z+608.3)*(z+608.3)+x*x)/2/x*0.3*0.389;
}


Bool_t DarkPhoton::phiSymmetricalInECal(TVector3 P1, TVector3 P2,  double& distPhi)
{
  distPhi = -999.;
  //distR   = -999.;
  //b       = -999.;
  P1.SetZ(0.);
  P2.SetZ(0.);
  TVector3 Pb = P2-P1;
  
  double scalarP = P1.Dot(P2);
  //cross
  double cosphi = scalarP/P1.Perp()/P2.Perp();
  double dPhi = acos(cosphi);

  distPhi = dPhi;
  //distR   = Pb.Perp();
  //b = impPar;
  //std::cout<<"dPhi = "<<distPhi<<" b = "<<b<<" impPar "<<impPar<<std::endl;

  //if (cosphi<-0.985 && fabs(b)<10.) return true;
  if (cosphi<-0.985) return true; //phi > 170.06 degree
  return false;

}




Bool_t DarkPhoton::InitHistos()
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
 
  int binX=500;   
  Double_t minX=-200.5;
  Double_t maxX=600.5;  
  hname="MissingMass_AllECALclu";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_AllECALclu_ThrEne50MeV";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_NoClInTime20ns_ThrEne50MeV";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_NoClInTime10ns_ThrEne50MeV";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="SAC_evSel_SinglePhotonAnnihilationEnergy_TimeCoincidenceRequest2ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_NoClInTime20ns_ThrEne50MeV_NoCoincidencePVeto";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_NoClInTime20ns_ThrEne50MeV_NoCoincidencePVeto_NoCoincidenceSAC";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_NoClInTime10ns_ThrEne50MeV_NoCoincidencePVeto";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_NoClInTime10ns_ThrEne50MeV_NoCoincidencePVeto_NoCoincidenceSAC";
  hSvc->BookHisto(hname, binX, minX, maxX);
  binX=200;   
  minX=-200.5;
  maxX=200.5;  
  hname="ECAL_MissingMassAnnihilation_10ns5CoGUnderPeakEThr90";
  hSvc->BookHisto(hname, binX, minX, maxX);
  
  return true;
}








