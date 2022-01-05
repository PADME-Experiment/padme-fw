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

  fdistanceTarget=1030+2513;//from calchep g4hits studies
  fFRmin=115.82;
  fFRmid=172.83;
  fFRmax=258.0;
  fEBeam=420;

  fPreSelectionAnnihilation=false;

  Double_t meanA[7] ={5.45591e+00,2.41556e+01,5.53455e+01, 9.86662e+01, 1.54738e+02, 2.23622e+02, 2.98974e+02};
  Double_t sigmaA[7]={1.73744e+01,1.70744e+01, 1.61503e+01, 1.44383e+01, 1.27064e+01, 9.77353e+00, 4.81163e+00};
  fminArange[7]={0.};
  fmaxArange[7]={0.};
  for(int i=0; i<7; i++){
    fminArange[i]=meanA[i]-2*sigmaA[i];
    fmaxArange[i]=meanA[i]+2*sigmaA[i];
  }


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
			TRecoVObject* TARGETev, TTargetRecoBeam* TargetBeam){
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
  double PositronE  = fEBeam + 0.511;
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


Bool_t DarkPhoton::passPreselection(Bool_t isTargetOut, Bool_t isMC, Bool_t externalPass)
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Bool_t passed = false;
  if (!fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED))
    {
      if (!fRecoEvent->GetTriggerMaskBit(TRECOEVENT_TRIGMASKBIT_BEAM)) return passed;
    }
  Double_t targetConst=1.78; //to use olny if the run is taken from September 2020 !!!! -> corrected in develop
  //if(isMC) targetConst=1;
  targetConst=1;
  if (!externalPass && !isMC && !isTargetOut && fTarget_RecoBeam->getnPOT()<13000) return passed;
  //std::cout<<"tagetnPOT "<<fTarget_RecoBeam->getnPOT() << " and isTargetOutBool " << isTargetOut<< std::endl;
  if (isTargetOut && fSAC_ClColl->GetNElements()>15) return passed;
  //if(fRecoEvent->GetEventNumber()>270000) return passed;
  std::string hname1 = "DP_NposInBunch_beam";
  hSvc->FillHisto(hname1,fTarget_RecoBeam->getnPOT()*targetConst);
  hname = "DP_nPOT";
  hSvc->FillHisto(hname,0,float(fTarget_RecoBeam->getnPOT()*targetConst));
  passed = true;
  return passed;
}


Bool_t DarkPhoton::ProcessDarkPhoton(Bool_t isTargetOut, Bool_t externalPass, Bool_t makeClSelection ,std::vector<int> selCl )
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

  Bool_t isMC=false;
  //std::cout<<"in ProcessAnalysis ... evsel ... "<<fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)<<std::endl;
  if (fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) {
    isMC=true;
    fEBeam=430.;
    //std::cout<<"input data are simulatetd "<<std::endl;
  }

  // apply BTF bunch preselection
  if (!passPreselection(isTargetOut, isMC, externalPass)) return true;

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
  Double_t shiftMomentumECalPVeto;
  Double_t ShiftSacPVeto;
  Double_t shiftMomentumsacPVeto;
  if(isMC){
    ShiftECalSac=0;
    ShiftECalPVeto=0;
    ShiftECalEVeto=0;
    shiftMomentumECalPVeto=0.;
    ShiftSacPVeto=0.;
    shiftMomentumsacPVeto=0.;
  }
  else{
    ShiftECalSac=3.2-3.11;
    ShiftECalPVeto=3.6-3.3;
    ShiftECalEVeto=1;
    //shiftMomentumECalPVeto=79;
    shiftMomentumECalPVeto=0;
    ShiftSacPVeto=0.;
    shiftMomentumsacPVeto=7.3;
  }

  Int_t NClECal = fECal_ClColl->GetNElements();
  Int_t NClSAC = fSAC_ClColl->GetNElements();
  Int_t NClPVeto = fPVeto_ClColl->GetNElements();
  Int_t NClEVeto = fEVeto_ClColl->GetNElements();

  Int_t EcalShiftTime=100;
  Int_t minTimeBunch=-140;
  Int_t maxTimebunch=150;

  for (Int_t jecal=0; jecal<NClECal; ++jecal){
    ecalclu          = fECal_ClColl->Element(jecal);
    Double_t ClTECal = ecalclu->GetTime();
    Double_t ClTEcalShifted= ClTECal + EcalShiftTime;
    if(ClTEcalShifted> maxTimebunch){
      //std::cout<<"--------------------------------" << std::endl;
      Double_t covered = maxTimebunch-ClTECal;
      //std::cout<<" covered " << covered << " (max range " << maxTimebunch <<" and TCl " <<ClTECal << " )" << std::endl;
      Double_t missingT = EcalShiftTime-covered;
      ClTEcalShifted = minTimeBunch + missingT;
      //std::cout<< " time cl + shift " << ClTECal + EcalShiftTime << " sfhifted " << ClTEcalShifted << std::endl;
    }
    hname="MissingMass_AllEcalTime";
    hSvc->FillHisto(hname,ClTECal, 1.);
    hname="MissingMass_AllEcalTimeShifted";
    hSvc->FillHisto(hname,ClTEcalShifted , 1.);
    Double_t g1x=ecalclu->GetPosition().X();
    Double_t g1y=ecalclu->GetPosition().Y();
    if(makeClSelection && selCl.at(jecal)<10)continue;
    Double_t g1E=ecalclu->GetEnergy();
    Double_t R_1 = sqrt(g1x*g1x+ g1y*g1y+fdistanceTarget*fdistanceTarget);
    Double_t R1ecal = sqrt(g1x*g1x+ g1y*g1y);
    Double_t Px_1 = g1E*g1x/ R_1;
    Double_t Py_1 = g1E*g1y/ R_1;
    Double_t Pz_1 = g1E*fdistanceTarget/ R_1;
    TLorentzVector P4g1F, P4eTarget, P4eBeam;
    P4g1F.SetPxPyPzE(Px_1,Py_1,Pz_1, g1E);
    Double_t me=0.511;
    Double_t PzBeam=sqrt(fEBeam*fEBeam-me*me);
    P4eTarget.SetPxPyPzE(0,0,0, me);
    P4eBeam.SetPxPyPzE(0,0,PzBeam, fEBeam);
    Double_t thetag1=atan(R1ecal/fdistanceTarget);
    if(fPreSelectionAnnihilation && passAnnPreSelection(g1E, thetag1)) continue;
    Double_t MissingMass=(P4eTarget+P4eBeam-P4g1F)*(P4eTarget+P4eBeam-P4g1F);

    Bool_t AmassRange[7];
    checkAMassBelonging(MissingMass, AmassRange);
    //for(int i=0; i< 7; i++) std::cout<<" massVecPos " << i << " bool " << AmassRange[i] << std::endl;
    Double_t phig1 =TMath::ATan2(g1y, g1x);
    //Double_t eff=1./extractEff(R1ecal, phig1);
    hname="MissingMass_AllECALclu";
    hSvc->FillHisto(hname,MissingMass , 1.);
    if(R1ecal<fFRmin || R1ecal>fFRmax) continue;
    hname="MissingMass_AllECALcluinFR";
    hSvc->FillHisto(hname,MissingMass , 1.);
    hname="MissingMass_EcalEvsEcalR";
    hSvc->FillHisto2(hname, R1ecal, g1E, 1.);
    Bool_t PassEneThr=false;
    Bool_t NoInTime4=true;
    if(g1E>90){
      PassEneThr=true;
      hname="MissingMass_AllECALcluinFR_ThrEne90MeV";
      hSvc->FillHisto(hname,MissingMass , 1.);
      hname="MissingMass_EcalEvsEcalR_ThrEne90MeV";
      hSvc->FillHisto2(hname, R1ecal, g1E, 1.);
      for(Int_t jecal2=0; jecal2<NClECal; jecal2++){                ///starting second cluster loop
        if(jecal==jecal2) continue;
        ecalclu2 = fECal_ClColl->Element(jecal2);
        if (ecalclu2->GetEnergy()<90.) continue;
        Double_t dTimeEcalEcal = ecalclu->GetTime()-ecalclu2->GetTime();
        Double_t dTimeEcalShiftEcal = ClTEcalShifted-ecalclu2->GetTime();
        hname="MissingMass_DTimeEcalEcal_ThrEne90MeV";
	hSvc->FillHisto(hname, dTimeEcalEcal, 1.);
        FillAMassRange(AmassRange, dTimeEcalEcal, dTimeEcalShiftEcal, "MissingMass_AllECALcluinFR_ThrEne90MeV_DTimeEcalEcal");
        if(fabs(ecalclu->GetTime()-ecalclu2->GetTime())<4.){
	  NoInTime4=false;  // 3 sigma of annihilation DTime = 3*1.3
        }
      }//end of second photon in ecal search loop
    } // end g1>90 MeV

    if(NoInTime4 && PassEneThr){
      hname="MissingMass_NoClInTime4ns_ThrEne90MeV";
      hSvc->FillHisto(hname,MissingMass, 1.);
      FillAMassRangeSimple(AmassRange, 1, "MissingMass_AllECALcluinFR_ThrEne90MeV_NoPhotonin4ns");
      Bool_t CoincidencePVeto=true;
      Bool_t CoincidenceSAC=true;
      Bool_t BremSac_PVeto=false;
      Bool_t forEffEcalPveto=false;
      Bool_t forEffEcalPveto2=false;
      for(int jpveto=0; jpveto<NClPVeto; jpveto++){
    	PVetoclu=fPVeto_ClColl->Element(jpveto);
    	Double_t cluTime=PVetoclu->GetTime();
    	Double_t cluZpos=PVetoclu->GetPosition().Z();
    	Double_t cluXpos=PVetoclu->GetPosition().X();
	Double_t DT= ClTECal-cluTime-ShiftECalPVeto;
	Double_t DTshifted=ClTEcalShifted-cluTime-ShiftECalPVeto;
    	hname="MissingMass_DTimeEcalPVeto_NoClInTime4ns_ThrEne90MeV";
    	hSvc->FillHisto(hname, DT, 1.);
	Double_t momentumPositron=CalculateMomentumPositron(cluZpos,cluXpos);
	Double_t DE= momentumPositron+g1E-fEBeam-shiftMomentumECalPVeto;
	hname="MissingMass_DTimevsDEEcalPVeto_NoClInTime4ns_ThrEne90MeVs";
	hSvc->FillHisto2(hname,DE, DT , 1.);
    	if(fabs(DT)<9.){
    	  hname="MissingMass_DEEcalPVeto_NoClInTime4ns_ThrEne90MeV_DTcalPVeto9ns";
    	  hSvc->FillHisto(hname,DE , 1.);
	  hname="MissingMass_EcalPVetoBrem__NoClInTime4ns_ThrEne90MeV_DTcalPVeto9ns";
	  hSvc->FillHisto2(hname, PVetoclu->GetChannelId(), g1E,1.);
	  hname="MissingMass_EcalPVetoBremMom__NoClInTime4ns_ThrEne90MeV_DTcalPVeto9ns";
	  hSvc->FillHisto2(hname,momentumPositron , g1E,1.);
	  if(DE>40. && DE<120.){
	    hname="MissingMass_EcalPVetoBrem__NoClInTime4ns_ThrEne90MeV_DTcalPVeto9ns_DEecalPVetoAroundpeak";
	    hSvc->FillHisto2(hname, PVetoclu->GetChannelId(), g1E,1.);
	  }
    	  if(fabs(DE)<50){
    	    CoincidencePVeto=false;
	    forEffEcalPveto=true;
    	  }
    	}
	if(fabs(DTshifted)<9.){
    	  if(fabs(DE)<50){
	    forEffEcalPveto2=true;
    	  }
    	}
      }//end of PVeto
      if(forEffEcalPveto) FillAMassRangeSimple(AmassRange, 1, "MissingMass_AllECALcluinFR_ThrEne90MeV_NoPhotonin4ns_BremEcalPVetoTag");
      if(forEffEcalPveto2)FillAMassRangeSimple(AmassRange, 1, "MissingMass_AllECALcluinFR_ThrEne90MeV_NoPhotonin4ns_BremEcalPVetoTagAfterShift");

      if(CoincidencePVeto)FillAMassRangeSimple(AmassRange, 1, "MissingMass_AllECALcluinFR_ThrEne90MeV_NoPhotonin4ns_NoBremEcalPveto");
      Bool_t forEffEcalSAC=false;
      for(int jsac=0; jsac<NClSAC; jsac++){
	SACclu=fSAC_ClColl->Element(jsac);
	Double_t cluTime=SACclu->GetTime();
	Double_t DTecalsac = ClTECal-cluTime-ShiftECalSac;
	Double_t DTecalsacShifted = ClTEcalShifted-cluTime-ShiftECalSac;
	Double_t DEecalsac = SACclu->GetEnergy() +g1E-fEBeam;
	if(SACclu->GetEnergy()>50.){
	  hname="MissingMass_DTimeEcalSAC_NoClInTime4ns_ThrEne90MeVSacEneThr50";
	  hSvc->FillHisto(hname,DTecalsac, 1.);
	  hname="MissingMass_DESacEcal_NoClInTime4ns_ThrEne90MeVSacEneThr50";
	  hSvc->FillHisto(hname,DEecalsac , 1.);
	  hname="MissingMass_DTimevsDEEcalSac_NoClInTime4ns_ThrEne90MeVSacEneThr50";
	  hSvc->FillHisto2(hname,DEecalsac, DTecalsac, 1.);
	  if(CoincidencePVeto){
	    hname="MissingMass_DTimeEcalSAC_NoClInTime4ns_ThrEne90MeVSacEneThr50_EcalBremRejected";
	    hSvc->FillHisto(hname,DTecalsac, 1.);
	    hname="MissingMass_DESacEcal_NoClInTime4ns_ThrEne90MeVSacEneThr50_EcalBremRejected";
	    hSvc->FillHisto(hname,DEecalsac , 1.);
	    hname="MissingMass_DTimevsDEEcalSac_NoClInTime4ns_ThrEne90MeVSacEneThr50_EcalBremRejected";
	    hSvc->FillHisto2(hname,DEecalsac, DTecalsac, 1.);
	  }
	}
	if(fabs(DTecalsac)<14. && SACclu->GetEnergy()>50 && CoincidencePVeto){
	  for(int jpveto=0; jpveto<NClPVeto; jpveto++){
	    PVetoclu=fPVeto_ClColl->Element(jpveto);
	    Double_t cluTimeP=PVetoclu->GetTime();
	    Double_t cluZpos=PVetoclu->GetPosition().Z();
	    Double_t cluXpos=PVetoclu->GetPosition().X();
	    Double_t DTsacpveto = cluTime-cluTimeP-ShiftSacPVeto;
	    hname="MissingMass_DTimeSACPVeto_NoClInTime4ns_ThrEne90MeV_ECALSACTimeCoincidence14_EThrSAC50_EcalBremRejected";
	    hSvc->FillHisto(hname,DTsacpveto, 1.);
	    if(fabs(DTsacpveto)<4.){
	      Double_t momentumPositron=CalculateMomentumPositron(cluZpos,cluXpos);
	      Double_t DEsacpveto = momentumPositron+SACclu->GetEnergy()-fEBeam-shiftMomentumsacPVeto;
	      hname="MissingMass_DEsacPVeto_NoClInTime4ns_ThrEne90MeV_ECALSACTimeCoincidence14_EThrSAC50_EcalBremRejected";
	      hSvc->FillHisto(hname,DEsacpveto, 1.);
	      if(fabs(DEsacpveto)<40.){
		BremSac_PVeto=true;
	      }
	    }
	  }// end of SAC-PVeto brem check
	  if(!BremSac_PVeto) {
	    CoincidenceSAC=false;
	    hname="MissingMass_DTimeEcalSAC_NoClInTime4ns_ThrEne90MeV_ECALSACTimeCoincidence14_EThrSAC50_EcalBremRejected_noBremSACPVeto";
	    hSvc->FillHisto(hname, DTecalsac, 1.);
	    hname="MissingMass_DESacEcal_NoClInTime4ns_ThrEne90MeV_ECALSACTimeCoincidence14_EThrSAC50_EcalBremRejected_noBremSACPVeto";
	    hSvc->FillHisto(hname,DEecalsac , 1.);
	    hname="MissingMass_DTimevsDEEcalSac_NoClInTime4ns_ThrEne90MeV_ECALSACTimeCoincidence14_EThrSAC50_EcalBremRejected_noBremSACPVeto";
	    hSvc->FillHisto2(hname,DEecalsac, DTecalsac, 1.);

	  }
	}// end check dTime ecal sac + sac ene >50 + CoincidencePVeto

	if(fabs(DTecalsacShifted)<14. && SACclu->GetEnergy()>50 && CoincidencePVeto){
	  Bool_t bremsac=false;
	  for(int jpveto=0; jpveto<NClPVeto; jpveto++){
	    PVetoclu=fPVeto_ClColl->Element(jpveto);
	    Double_t cluTimeP=PVetoclu->GetTime();
	    Double_t cluZpos=PVetoclu->GetPosition().Z();
	    Double_t cluXpos=PVetoclu->GetPosition().X();
	    Double_t DTsacpveto = cluTime-cluTimeP-ShiftSacPVeto;
	    if(fabs(DTsacpveto)<4.){
	      Double_t momentumPositron=CalculateMomentumPositron(cluZpos,cluXpos);
	      Double_t DEsacpveto = momentumPositron+SACclu->GetEnergy()-fEBeam-shiftMomentumsacPVeto;
	      if(fabs(DEsacpveto)<40.)bremsac=true;
	    }
	  }// end of SAC-PVeto brem check
	  if(!bremsac)forEffEcalSAC=true;
	}// end check dTime ecal sac + sac ene >50 + CoincidencePVeto time shifted

      }// end sac loop
      if(forEffEcalSAC)FillAMassRangeSimple(AmassRange, 1, "MissingMass_AllECALcluinFR_ThrEne90MeV_NoPhotonin4ns_NoBremEcalPveto_EcalSacCoincTagAfterShift");

      if(CoincidencePVeto){
	hname="MissingMass_NoClInTime4ns_ThrEne90MeV_NoCoincidencePVeto";
	hSvc->FillHisto(hname,MissingMass, 1.);
	if(CoincidenceSAC){
	  hname="MissingMass_NoClInTime4ns_ThrEne90MeV_NoCoincidencePVeto_NoCoincidenceSAC";
	  hSvc->FillHisto(hname,MissingMass, 1.);
	}
      }
    }// end no cluster in 4ns and EgEcal>90

  }// end of ecal cluster loop
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


Bool_t DarkPhoton::passAnnPreSelection(Double_t clE, Double_t clTheta){
  bool pass=false;
  if(clE<90.)return pass;
  Double_t constantE=6.39845;
  Double_t slopeE=-2.05831e+01;
  Double_t EnergyFunc=exp(constantE+slopeE*clTheta);
  Double_t DeltaEnergyFunction= -(EnergyFunc-clE);
  if(fabs(DeltaEnergyFunction)<100.) pass=true;
  return pass;
}


void DarkPhoton::checkAMassBelonging(Double_t MMiss, Bool_t (&boolMass)[7]){
  for(int i=0; i<7; i++){
    if(MMiss>fminArange[i] && MMiss<fmaxArange[i]) boolMass[i]=true;
    else boolMass[i]=false;
    //std::cout<<"mmiss " << MMiss << " minRange " << fminArange[i] << " maxRange " << fmaxArange[i] << " bool " << boolMass[i] << std::endl;
  }

}

void DarkPhoton::FillAMassRangeSimple(Bool_t MRange[7], Double_t var1, std::string hPartialName){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

  std::string mass[7]={"2.5", "5", "7.5", "10", "12.5", "15", "17.5"};
  for(int i=0; i<7; i++){
    if(MRange[i]==true){
      hname=hPartialName+mass[i];
      hSvc->FillHisto(hname, var1, 1.);
    }
  }

}


void DarkPhoton::FillAMassRange(Bool_t MRange[7], Double_t var1, Double_t var1AfterShift, std::string hPartialName){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  std::string hname1;

  std::string mass[7]={"2.5", "5", "7.5", "10", "12.5", "15", "17.5"};
  for(int i=0; i<7; i++){
    if(MRange[i]==true){
      hname=hPartialName+"noTShift"+mass[i];
      hSvc->FillHisto(hname, var1, 1.);
      hname1=hPartialName+"TShift"+mass[i];
      hSvc->FillHisto(hname1, var1AfterShift, 1.);
    }
  }

}

Double_t DarkPhoton::extractEff(Double_t radius, Double_t phi){

  Double_t effRFIn[8] ={0.721,0.742,0.821,0.723,0.682,0.785,0.726,0.684};
  Double_t effRFOut[8]={0.844,0.748,0.75 ,0.779,0.813,0.63 ,0.515,0.801};

  Bool_t inner=false;
  Bool_t outer=false;
  if(radius<fFRmid )inner=true;
  if(radius>=fFRmid)outer=true;
  Double_t phiDeg=phi*360./(2*TMath::Pi());
  if(phiDeg<0.)phiDeg=360+phiDeg;

  int phiSlice=phiDeg/45.;
  Double_t eff;
  if(inner){
    eff=effRFIn[phiSlice];
  }
  else if(outer){
    eff=effRFOut[phiSlice];
  }
  return eff;


}

Bool_t DarkPhoton::InitHistos()
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

  hname="DP_NposInBunch_beam";
  hSvc->BookHisto(hname, 500, 0., 40000.);
  hname = "DP_nPOT";
  hSvc->BookHisto(hname, 3, -1.5, 1.5);

  hname="MissingMass_AllEcalTime";
  hSvc->BookHisto(hname, 100, -200, 200);
  hname="MissingMass_AllEcalTimeShifted";
  hSvc->BookHisto(hname, 100, -200, 200);

  int binX=200;
  Double_t minX=-200;
  Double_t maxX=600;

  hname="MissingMass_EvsTheta_10nsEThr90";
  hSvc->BookHisto2(hname, 100, 0.1,0.2, 400, 0, 500);


  hname="MissingMass_AllECALclu";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_AllECALcluinFR";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_AllECALcluinFR_ThrEne90MeV";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_NoClInTime4ns_ThrEne90MeV";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="SAC_evSel_SinglePhotonAnnihilationEnergy_TimeCoincidenceRequest2ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_NoClInTime4ns_ThrEne90MeV_NoCoincidencePVeto";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_NoClInTime4ns_ThrEne90MeV_NoCoincidencePVeto_NoCoincidenceSAC";
  hSvc->BookHisto(hname, binX, minX, maxX);

  binX=200;
  minX=-200.5;
  maxX=200.5;
  hname="ECAL_MissingMassAnnihilation_10ns5CoGUnderPeakEThr90";
  hSvc->BookHisto(hname, binX, minX, maxX);

  binX=500;
  minX=-500;
  maxX=500;
  hname="MissingMass_DTimeEcalEcal_ThrEne90MeV";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_EcalEvsEcalR";
  hSvc->BookHisto2(hname, 100, 30, 320, 200, 0, 600);
  hname="MissingMass_EcalEvsEcalR_ThrEne90MeV";
  hSvc->BookHisto2(hname, 100, 30, 320, 200, 0, 600);


  binX=500;
  minX=-500;
  maxX=500;
  hname="MissingMass_DTimeEcalPVeto_NoClInTime4ns_ThrEne90MeV";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_DTimeEcalSAC_NoClInTime4ns_ThrEne90MeVSacEneThr50";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_DTimeEcalSAC_NoClInTime4ns_ThrEne90MeVSacEneThr50_EcalBremRejected";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_DTimeSACPVeto_NoClInTime4ns_ThrEne90MeV_ECALSACTimeCoincidence14_EThrSAC50_EcalBremRejected";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_DTimeEcalSAC_NoClInTime4ns_ThrEne90MeV_ECALSACTimeCoincidence14_EThrSAC50_EcalBremRejected_noBremSACPVeto";
  hSvc->BookHisto(hname, binX, minX, maxX);

  binX=500;
  minX=-500;
  maxX=700;
  hname="MissingMass_DEEcalPVeto_NoClInTime4ns_ThrEne90MeV_DTcalPVeto9ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_DESacEcal_NoClInTime4ns_ThrEne90MeVSacEneThr50";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_DESacEcal_NoClInTime4ns_ThrEne90MeVSacEneThr50_EcalBremRejected";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_DEsacPVeto_NoClInTime4ns_ThrEne90MeV_ECALSACTimeCoincidence14_EThrSAC50_EcalBremRejected";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_DESacEcal_NoClInTime4ns_ThrEne90MeV_ECALSACTimeCoincidence14_EThrSAC50_EcalBremRejected_noBremSACPVeto";
  hSvc->BookHisto(hname, binX, minX, maxX);


  Double_t maxX1=500;
  hname="MissingMass_DTimevsDEEcalSac_NoClInTime4ns_ThrEne90MeVSacEneThr50";
  hSvc->BookHisto2(hname, binX, minX, maxX1, binX, minX, maxX);
  hname="MissingMass_DTimevsDEEcalPVeto_NoClInTime4ns_ThrEne90MeVs";
  hSvc->BookHisto2(hname, binX, minX, maxX1, binX, minX, maxX);
  hname="MissingMass_DTimevsDEEcalSac_NoClInTime4ns_ThrEne90MeVSacEneThr50_EcalBremRejected";
  hSvc->BookHisto2(hname, binX, minX, maxX1, binX, minX, maxX);
  hname="MissingMass_DTimevsDEEcalSac_NoClInTime4ns_ThrEne90MeV_ECALSACTimeCoincidence14_EThrSAC50_EcalBremRejected_noBremSACPVeto";
  hSvc->BookHisto2(hname, binX, minX, maxX1, binX, minX, maxX);


  hname="MissingMass_EcalPVetoBrem__NoClInTime4ns_ThrEne90MeV_DTcalPVeto9ns";
  hSvc->BookHisto2(hname, 100, -0.5, 99.5, 200 ,  0, 500);
  hname="MissingMass_EcalPVetoBremMom__NoClInTime4ns_ThrEne90MeV_DTcalPVeto9ns";
  hSvc->BookHisto2(hname, 200, 0, 500, 200 ,  0, 500);
  hname="MissingMass_EcalPVetoBrem__NoClInTime4ns_ThrEne90MeV_DTcalPVeto9ns_DEecalPVetoAroundpeak";
  hSvc->BookHisto2(hname, 100, -0.5, 99.5, 200 ,  0, 500);



  std::string mass[7]={"2.5", "5", "7.5", "10", "12.5", "15", "17.5"};
  std::string hPartialName = "MissingMass_AllECALcluinFR_ThrEne90MeV_DTimeEcalEcal";
  std::string hname1;
  binX=500;
  minX=-500;
  maxX=500;
  for(int i=0; i<7; i++){
    hname=hPartialName+"noTShift"+mass[i];
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname1=hPartialName+"TShift"+mass[i];
    hSvc->BookHisto(hname1, binX, minX, maxX);
  }

  std::string hPartialName1 ="MissingMass_AllECALcluinFR_ThrEne90MeV_NoPhotonin4ns";
  std::string hPartialName2 ="MissingMass_AllECALcluinFR_ThrEne90MeV_NoPhotonin4ns_BremEcalPVetoTag";
  std::string hPartialName3 ="MissingMass_AllECALcluinFR_ThrEne90MeV_NoPhotonin4ns_BremEcalPVetoTagAfterShift";
  std::string hname2;
  binX=3;
  minX=-0.5;
  maxX=2.5;
  for(int i=0; i<7; i++){
    hname=hPartialName1+mass[i];
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname1=hPartialName2+mass[i];
    hSvc->BookHisto(hname1, binX, minX, maxX);
    hname2=hPartialName3+mass[i];
    hSvc->BookHisto(hname2, binX, minX, maxX);
  }

  hPartialName1 ="MissingMass_AllECALcluinFR_ThrEne90MeV_NoPhotonin4ns_NoBremEcalPveto";
  hPartialName2 ="MissingMass_AllECALcluinFR_ThrEne90MeV_NoPhotonin4ns_NoBremEcalPveto_EcalSacCoincTagAfterShift";
  for(int i=0; i<7; i++){
    hname=hPartialName1+mass[i];
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname1=hPartialName2+mass[i];
    hSvc->BookHisto(hname1, binX, minX, maxX);
  }

  return true;
}
