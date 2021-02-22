#include "AnnihilationSelection.hh"

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


AnnihilationSelection::AnnihilationSelection()
{
  
  fRecoEvent   = NULL;

  fECal_hitEvent=0;
  fECal_ClColl=0;
  
  fSAC_hitEvent=0;
  fSAC_ClColl=0;
  
  fTarget_hitEvent=0;
  fTarget_RecoBeam=0;
  
  fVerbose        = 0;

 
}
AnnihilationSelection::AnnihilationSelection(Int_t processingMode, Int_t verbosityFlag, Bool_t NoTargetBool)
{
  fRecoEvent      = NULL;
  fVerbose        = verbosityFlag;
  fNoTargetBool   = NoTargetBool;
  fInitToComplete =true;

}
AnnihilationSelection::~AnnihilationSelection()
{
}

Bool_t AnnihilationSelection::Init(TRecoEvent* eventHeader, 
				   TRecoVObject* ECALev, TRecoVClusCollection* ECALcl,
				   TRecoVObject* SACev, TRecoVClusCollection* SACcl,
				   TRecoVObject* TARGETev, TTargetRecoBeam* TargetBeam){
  fRecoEvent = eventHeader;
  fECal_hitEvent      =ECALev   ;
  fSAC_hitEvent       =SACev    ;
  fTarget_hitEvent    =TARGETev ;
 
  fECal_ClColl      =ECALcl   ;
  fSAC_ClColl       =SACcl    ;
  fTarget_RecoBeam  =TargetBeam ;

  fdistanceTarget=3470;
  
  //time range 
  ftimerange1min=-150;
  ftimerange1max=-90;
  ftimerange2min=-90;
  ftimerange2max=-30;
  ftimerange3min=-30;
  ftimerange3max=30;
  ftimerange4min=30;
  ftimerange4max=90;
  ftimerange5min=90;
  ftimerange5max=150;


  return true;
}


Bool_t AnnihilationSelection::Process(Bool_t isMC)
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

  if (!passPreselection(fNoTargetBool, isMC)) return true;

  Double_t EBeam=0.;
  if(isMC)EBeam=450.0;
  else EBeam=430.;
 
  Bool_t retCode = 0;
  TRecoVCluster* ecalclu=NULL;
  TRecoVCluster* ecalclu2=NULL;
  Int_t NClECal = fECal_ClColl->GetNElements();

  for (Int_t jecal=0; jecal<NClECal; ++jecal){
    ecalclu          = fECal_ClColl->Element(jecal);
    Int_t NGGInTheSameEventTime10ns=0;
    Int_t NGGInTheSameEventTime10ns20DeltaPhiDeltaThetaEThr90UnderPeack=0;
            
    fg1x    =ecalclu->GetPosition().X();
    fg1y    =ecalclu->GetPosition().Y();
    fg1E    =ecalclu->GetEnergy();
    hname="Annihilation_ECalClusterEnergy";
    hSvc->FillHisto(hname, fg1E, 1.);
    fg1t    =ecalclu->GetTime();
    fR_1    = sqrt(fg1x*fg1x+ fg1y*fg1y+fdistanceTarget*fdistanceTarget);
    fg1Recal=sqrt(fg1x*fg1x+fg1y*fg1y);
    fPx_1   = fg1E*fg1x/ fR_1;
    fPy_1   = fg1E*fg1y/ fR_1;
    fPz_1   = fg1E*fdistanceTarget/ fR_1;
    TLorentzVector P4g1F, P4eTarget, P4eBeam;
    P4g1F.SetPxPyPzE(fPx_1,fPy_1,fPz_1, fg1E);
    fthetag1 = P4g1F.Theta();
    Double_t me=0.511;
    Double_t PzBeam=sqrt(EBeam*EBeam-me*me);
    P4eTarget.SetPxPyPzE(0,0,0, me);
    P4eBeam.SetPxPyPzE(0,0,PzBeam, EBeam);
    Int_t g1seedChId=fECal_hitEvent->Hit(ecalclu->GetSeed())->GetChannelId();//hit(ecalclu->GetSeed())->GetChannelId();
    for(Int_t jecal2=jecal+1; jecal2<NClECal; jecal2++){                ///starting second cluster loop
      ecalclu2 = fECal_ClColl->Element(jecal2);
      fpos1 = ecalclu->GetPosition();
      fpos2 = ecalclu2->GetPosition();
      fg2x        =ecalclu2->GetPosition().X();
      fg2y        =ecalclu2->GetPosition().Y();
      fg2t        =ecalclu2->GetTime();
      fg2Recal    =sqrt(fg2x*fg2x+fg2y*fg2y);
      fg2E        =ecalclu2->GetEnergy();
      fXWeighted  =0.;
      fYWeighted  =0.;
      fXWeighted= (fg1x*fg1E+fg2x*fg2E)/(fg1E+fg2E);
      fYWeighted= (fg1y*fg1E+fg2y*fg2E)/(fg1E+fg2E);
      fCoG        = sqrt(fXWeighted*fXWeighted+fYWeighted*fYWeighted);
      fR_2        = sqrt(fg2x*fg2x+ fg2y*fg2y+fdistanceTarget*fdistanceTarget);
      fPx_2       = fg2E*fg2x/ fR_2;
      fPy_2       = fg2E*fg2y/ fR_2;
      fPz_2       = fg2E*fdistanceTarget/ fR_2;
      Int_t g2seedChId=fECal_hitEvent->Hit(ecalclu2->GetSeed())->GetChannelId();   
      TLorentzVector P4g2F,SumP;
      P4g2F.SetPxPyPzE(fPx_2,fPy_2,fPz_2,fg2E);
      fangleBetweenTwoPhoton = P4g1F.Angle(P4g2F.Vect());
      fthetag2               = P4g2F.Theta();
      fphig1 = P4g1F.Phi();
      fphig2 = P4g2F.Phi();

      if(fabs(fg1t-fg2t)>10.)continue;                      //two clsuter in time 10 ns
      NGGInTheSameEventTime10ns++;

      Bool_t firstGInFR_def=false;
      Bool_t secondGInFR_def=false;
      //new FR extracted by TagAndProbe studies
      if(fg1Recal>106 && fg1Recal<258 )  firstGInFR_def=true;
      if(fg2Recal>106 && fg2Recal<258)  secondGInFR_def=true;

      //old FR , not symmetrical ...see tag and probe studies
      //if(fg1Recal>92 && fg1Recal<258 )  firstGInFR_def=true;
      //if(fg2Recal>92 && fg2Recal<258)  secondGInFR_def=true;

      if(!firstGInFR_def)continue;       
      Bool_t InEnergy=false;
      if(fg1E>90 && fg2E>90 && fg1E<400 && fg2E<400) InEnergy=true;
      Bool_t TotEnergyCut=false;
      if(fg1E+fg2E>300 && fg1E+fg2E<600 ) TotEnergyCut=true;
      
      double dPhi = -999.;
      bool isPhySym = phiSymmetricalInECal(fpos1, fpos2, dPhi);
      fDeltaPhi=dPhi;
      Bool_t dPhiIn20Degree=false; //0.35 rad              
      if(fabs(dPhi-3.14)<0.35) dPhiIn20Degree=true;

      Bool_t InThetaAngle=false;
      if(fangleBetweenTwoPhoton>0.08 && fangleBetweenTwoPhoton < 0.12) InThetaAngle=true;

      std::string sufix="";
      FillGeneralHisto("_g1inFRDTime10");

      if(dPhiIn20Degree){
	sufix="_g1inFRin20Deg";
	FillGeneralHisto(sufix);
	if(InThetaAngle) {
	  if(TotEnergyCut){FillGeneralHisto("_g1inFRin20DegThetaCutUnderPeak");}
	  if(InEnergy){FillGeneralHisto("_g1inFRin20DegThetaCutEThr");}
	  if(TotEnergyCut && InEnergy ){FillGeneralHisto("_g1inFRin20DegThetaCutUnderPeakEThr");}
	}      
      }
      
      if(secondGInFR_def && dPhiIn20Degree && InThetaAngle){
	FillGeneralHisto("_g1g2inFRin20DegThetaCut");
	if(TotEnergyCut && InEnergy ){FillGeneralHisto("_g1g2inFRin20DegThetaCutUnderPeakEThr");}
      }
      
       
      if(fabs(fXWeighted)<50. && fabs(fYWeighted)<50.){
	Int_t countHit1_5CoG=0;
	Int_t countHit2_5CoG=0;
	CheckNumberHitsOnCh(g1seedChId, g2seedChId, countHit1_5CoG, countHit2_5CoG);
	hname="ECAL_nHitOnChannel_5CoG";
	hSvc->FillHisto(hname, countHit1_5CoG, 1.);
	hSvc->FillHisto(hname, countHit2_5CoG, 1.);
	FillGeneralHisto("_g1inFRin5Cog");
	if(TotEnergyCut){FillGeneralHisto("_g1inFRin5CogUnderPeak");}
	if(InEnergy){FillGeneralHisto("_g1inFRin5CogEThr");}
	if(TotEnergyCut && InEnergy ){FillGeneralHisto("_g1inFRin5CogUnderPeakEThr");}
	if(InEnergy){
	  FillRadiusRangeHisto("_g1inFRin5CogEThr");
	  FillTimeRangeHisto("_g1inFRin5CogEThr");
	}
      }
      if(fabs(fXWeighted)<50. && fabs(fYWeighted)<50. && secondGInFR_def ){
	FillGeneralHisto("_g1g2inFRin5Cog");
	if(TotEnergyCut && InEnergy ){FillGeneralHisto("_g1g2inFRin5CogUnderPeakEThr");}
      }

      hname="NGammaGammaForEvent_TimeRequest"; 
      hSvc->FillHisto(hname,NGGInTheSameEventTime10ns , 1.);
      hname="NGammaGammaForEvent_10ns20DegreeDeltaThetaEthr90UnderPeack"; 
      hSvc->FillHisto(hname,NGGInTheSameEventTime10ns20DeltaPhiDeltaThetaEThr90UnderPeack, 1.);
  
    }
  }
  return true;
}

void AnnihilationSelection::FillGeneralHisto(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  hname="ECAL_gravTwoPhoton10ns"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
  hname="ECAL_Xgg_10ns"+sufix;
  hSvc->FillHisto(hname,fg1x, 1.);
  hSvc->FillHisto(hname,fg2x, 1.);
  hname="ECAL_Ygg_10ns"+sufix;
  hSvc->FillHisto(hname,fg1y, 1.);
  hSvc->FillHisto(hname,fg2y, 1.);
  hname="ECAL_Rgg_10ns"+sufix;
  hSvc->FillHisto(hname,fg1Recal, 1.);
  hSvc->FillHisto(hname,fg2Recal, 1.);
  hname="ECAL_R2_10ns"+sufix;
  hSvc->FillHisto(hname,fg2Recal, 1.);
  hname="ECAL_Egg_10ns"+sufix;
  hSvc->FillHisto(hname,fg1E, 1.);
  hSvc->FillHisto(hname,fg2E, 1.);
  hname="ECAL_tgg_10ns"+sufix;
  hSvc->FillHisto(hname,fg1t, 1.);
  hSvc->FillHisto(hname,fg2t, 1.);
  hname="ECAL_Dtgg_10ns"+sufix;
  hSvc->FillHisto(hname,fg1t-fg2t, 1.);
  hname="ECAL_Phig1_10ns"+sufix;
  hSvc->FillHisto(hname,fphig1, 1.);
  hname="ECAL_Phig2_10ns"+sufix;
  hSvc->FillHisto(hname,fphig2, 1.);
  hname="ECAL_Thetag1_10ns"+sufix;
  hSvc->FillHisto(hname,fthetag1, 1.);
  hname="ECAL_Thetag2_10ns"+sufix;
  hSvc->FillHisto(hname,fthetag2, 1.);
  hname="ECAL_DTheta_10ns"+sufix;
  hSvc->FillHisto(hname,fangleBetweenTwoPhoton, 1.);
  hname="ECAL_DPhi_10ns"+sufix;
  hSvc->FillHisto(hname,fDeltaPhi, 1.);
  hname="ECAL_xCoGgg_10ns"+sufix;
  hSvc->FillHisto(hname,fXWeighted, 1.);
  hname="ECAL_yCoGgg_10ns"+sufix;
  hSvc->FillHisto(hname,fYWeighted, 1.);
  hname="MAPgg_10ns"+sufix;
  hSvc->FillHisto2(hname,fg1x, fg1y);
  hSvc->FillHisto2(hname,fg2x, fg2y);
  hname="MAPggW_10ns"+sufix;
  hSvc->FillHisto2(hname,fg1x, fg1y, fg1E);
  hSvc->FillHisto2(hname,fg2x, fg2y, fg2E);
  hname="ECAL_XggVsEgg_10ns"+sufix;
  hSvc->FillHisto2(hname,fg1x, fg1E);
  hSvc->FillHisto2(hname,fg2x, fg2E);
  hname="ECAL_YggVsEgg_10ns"+sufix;
  hSvc->FillHisto2(hname,fg1y, fg1E);
  hSvc->FillHisto2(hname,fg2y, fg2E);
  hname="ECAL_RggVsEgg_10ns"+sufix;
  hSvc->FillHisto2(hname,fg1Recal, fg1E);
  hSvc->FillHisto2(hname,fg2Recal, fg2E);
  hname="ECAL_tggVsEgg_10ns"+sufix;
  hSvc->FillHisto2(hname,fg1t, fg1E);
  hSvc->FillHisto2(hname,fg2t, fg2E);
  hname="ECAL_Phi2vsPhi1"+sufix;
  hSvc->FillHisto2(hname,fphig1,fphig2);
  hname="ECAL_Theta2vsTheta1"+sufix;
  hSvc->FillHisto2(hname,fthetag1,fthetag2);
  hname="ECAL_ThetavsEnergy"+sufix;
  hSvc->FillHisto2(hname,fg1E, fthetag1);
  hSvc->FillHisto2(hname,fg2E, fthetag2);
  hname="ECAL_PhivsEnergy"+sufix;
  hSvc->FillHisto2(hname,fg1E, fphig1);
  hSvc->FillHisto2(hname,fg2E, fphig2);
  hname="ECAL_YCogvsXCog"+sufix;
  hSvc->FillHisto2(hname,fXWeighted,fYWeighted);

}


void AnnihilationSelection::FillRadiusRangeHisto(std::string sufix){

  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
 
  if(fg1Recal>90  && fg1Recal<260 ) {
    hname="ECAL_g1RadiusRange90_260_twoPhotonInTime10ns"+sufix;
    hSvc->FillHisto(hname,(fg1E + fg2E),1.);
  }
  if(fg1Recal>110 && fg1Recal<240)  {
    hname="ECAL_g1RadiusRange110_240_twoPhotonInTime10ns"+sufix;
    hSvc->FillHisto(hname,(fg1E+ fg2E),1.);
  }
  if(fg1Recal>130 && fg1Recal<220)  {
    hname="ECAL_g1RadiusRange130_220_twoPhotonInTime10ns"+sufix;
    hSvc->FillHisto(hname,(fg1E+ fg2E),1.);
  }
  if(fg1Recal>150 && fg1Recal<200)  {
    hname="ECAL_g1RadiusRange150_200_twoPhotonInTime10ns"+sufix;
    hSvc->FillHisto(hname,(fg1E+ fg2E),1.);
  }
  //out of four crystals								
  if(fg1Recal>134 && fg1Recal<216)  {
    hname="ECAL_g1RadiusRange134_216_twoPhotonInTime10ns"+sufix;
    hSvc->FillHisto(hname,(fg1E+ fg2E),1.);
  }

}




void AnnihilationSelection::FillTimeRangeHisto(std::string sufix){

  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
 
  if(fg1t>ftimerange1min && fg1t<=ftimerange1max){
    hname="ECAL_1TimeRange_twoPhotonInTime10ns"+sufix;
    hSvc->FillHisto(hname,(fg1E+ fg2E),1.);
  }
  if(fg1t>ftimerange2min && fg1t<=ftimerange2max){
    hname="ECAL_2TimeRange_twoPhotonInTime10ns"+sufix;
    hSvc->FillHisto(hname,(fg1E+ fg2E),1.);
  }
  if(fg1t>ftimerange3min && fg1t<=ftimerange3max){
    hname="ECAL_3TimeRange_twoPhotonInTime10ns"+sufix;
    hSvc->FillHisto(hname,(fg1E+ fg2E),1.);
  }
  if(fg1t>ftimerange4min && fg1t<=ftimerange4max){
    hname="ECAL_4TimeRange_twoPhotonInTime10ns"+sufix;
    hSvc->FillHisto(hname,(fg1E+ fg2E),1.);
  }
  if(fg1t>ftimerange5min && fg1t<=ftimerange5max){
    hname="ECAL_5TimeRange_twoPhotonInTime10ns"+sufix;
    hSvc->FillHisto(hname,(fg1E+ fg2E),1.);
  }

}




void AnnihilationSelection::CheckNumberHitsOnCh(Int_t chSeed1, Int_t chSeed2, Int_t& countHit1, Int_t& countHit2){
  TRecoVHit* hit=NULL;
  Int_t countHitForCh1=0;
  Int_t countHitForCh2=0;
  Int_t fNhits = fECal_hitEvent->GetNHits();
  for (Int_t i=0; i<fNhits; ++i){
    hit = fECal_hitEvent->Hit(i);
    Int_t chId=hit->GetChannelId();
    if(chSeed1==chId){countHitForCh1++;}
    if(chSeed2==chId){countHitForCh2++;}
  }
  countHit1=countHitForCh1;
  countHit2=countHitForCh2;
  return; 
}


Bool_t AnnihilationSelection::phiSymmetricalInECal(TVector3 P1, TVector3 P2,  double& distPhi)
{
  distPhi = -999.;
  P1.SetZ(0.);
  P2.SetZ(0.);
  TVector3 Pb = P2-P1;
  
  double scalarP = P1.Dot(P2);
  //cross
  double cosphi = scalarP/P1.Perp()/P2.Perp();
  double dPhi = acos(cosphi);

  distPhi = dPhi;
  if (cosphi<-0.985) return true; //phi > 170.06 degree
  return false;

}

 Bool_t AnnihilationSelection::passPreselection(Bool_t isTargetOut, Bool_t isMC)
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
   std::string hname;
  Bool_t passed = false;

  if (!fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) 
    {
      if (!fRecoEvent->GetTriggerMaskBit(TRECOEVENT_TRIGMASKBIT_BEAM)) return passed;
    }
 
  Double_t targetConst=1.78; //to use olny if the run is taken from September 2020 !!!!
  if(isMC) targetConst=1; 
  std::string hname1 = "AnnMet_NposInBunch_beam";
  hSvc->FillHisto(hname1,fTarget_RecoBeam->getnPOT()*targetConst);
  if (fTarget_RecoBeam->getnPOT()<-9999) return passed;
  //std::cout<<"tagetnPOT "<<fTarget_RecoBeam->getnPOT() << " and isTargetOutBool " << isTargetOut<< std::endl;
  if (isTargetOut && fSAC_ClColl->GetNElements()>15) return passed;
  
  //if(fRecoEvent->GetEventNumber()>270000) return passed;

  hname = "AnnMet_nPOT";
  hSvc->FillHisto(hname,0,float(fTarget_RecoBeam->getnPOT()*targetConst)); 
  passed = true;
  return passed;
}



Bool_t AnnihilationSelection::InitHistos()
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  int binX=5000;
  double minX=0;
  double maxX=3000;
  int binY=5000;
  double minY=0;
  double maxY=3000;

  hname="Annihilation_ECalClusterEnergy";
  hSvc->BookHisto(hname, binX, minX, maxX);

  hname="AnnMet_NposInBunch_beam";
  hSvc->BookHisto(hname, 500, 0., 40000.);
  hname = "AnnMet_nPOT";
  hSvc->BookHisto(hname, 3, -1.5, 1.5);

  std::vector<std::string> sufix;
  sufix.push_back("_g1inFRDTime10");
  sufix.push_back("_g1inFRin20Deg");
  sufix.push_back("_g1inFRin20DegThetaCutUnderPeak");
  sufix.push_back("_g1inFRin20DegThetaCutEThr");
  sufix.push_back("_g1inFRin20DegThetaCutUnderPeakEThr");
  sufix.push_back("_g1g2inFRin20DegThetaCut");
  sufix.push_back("_g1g2inFRin20DegThetaCutUnderPeakEThr");
  sufix.push_back("_g1inFRin5Cog");
  sufix.push_back("_g1inFRin5CogUnderPeak");
  sufix.push_back("_g1inFRin5CogEThr");
  sufix.push_back("_g1inFRin5CogUnderPeakEThr");
  sufix.push_back("_g1g2inFRin5Cog");
  sufix.push_back("_g1g2inFRin5CogUnderPeakEThr");
  for(int i=0; i<sufix.size(); i++){
    binX=500;
    minX=0.;
    maxX=2000.;
    hname="ECAL_gravTwoPhoton10ns" + sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    binX=350;
    minX=-300.;
    maxX=350.;
    hname="ECAL_Xgg_10ns"+sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_Ygg_10ns"+sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    binX=400;
    minX=0.;
    maxX=350.;
    hname="ECAL_Rgg_10ns"+sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_R2_10ns"+sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    binX=800;
    minX=0.;
    maxX=800.;
    hname="ECAL_Egg_10ns"+sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    binX=600;
    minX=-300.;
    maxX=300.;
    hname="ECAL_tgg_10ns"+sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    binX=100;
    minX=-10.;
    maxX=10.;
    hname="ECAL_Dtgg_10ns"+sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    binX=200;
    minX=-4.;
    maxX=4.;
    hname="ECAL_Phig1_10ns"+sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_Phig2_10ns"+sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    binX=300;
    minX=-0.05;
    maxX=0.5;
    hname="ECAL_Thetag1_10ns"+sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_Thetag2_10ns"+sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    binX=500;
    minX=-0.05;
    maxX=0.2;
    hname="ECAL_DTheta_10ns"+sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    binX=500;
    minX=-0.05;
    maxX=4;
    hname="ECAL_DPhi_10ns"+sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    binX=600;
    minX=-300.;
    maxX=300.;
    hname="ECAL_xCoGgg_10ns"+sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_yCoGgg_10ns"+sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    binY=400;
    minY=-300.;
    maxY=300.;
    hname="MAPgg_10ns"+sufix.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    hname="MAPggW_10ns"+sufix.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    binY=500;
    minY=0.;
    maxY=800.;
    binX=300;
    minX=-300.;
    maxX=300.;
    hname="ECAL_XggVsEgg_10ns"+sufix.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    hname="ECAL_YggVsEgg_10ns"+sufix.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    binX=200;
    minX=0.;
    maxX=300.;
    hname="ECAL_RggVsEgg_10ns"+sufix.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    binX=500;
    minX=-300.;
    maxX=300.;
    hname="ECAL_tggVsEgg_10ns"+sufix.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    binX=200;
    minX=-4;
    maxX=4.;
    binY=200;
    minY=-4.;
    maxY=4.;	
    hname="ECAL_Phi2vsPhi1"+sufix.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    binX=200;
    minX=-0.05;
    maxX=0.3;
    binY=200;
    minY=-0.05;
    maxY=0.3;
    hname="ECAL_Theta2vsTheta1"+sufix.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    binX=600;
    minX=0.;
    maxX=800.;
    hname="ECAL_ThetavsEnergy"+sufix.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    binY=200;
    minY=-4;
    maxY=4;
    hname="ECAL_PhivsEnergy"+sufix.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    binX=600;
    minX=-300.;
    maxX=300.;
    binY=600;
    minY=-300.;
    maxY=300.;
    hname="ECAL_YCogvsXCog"+sufix.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  }

  binX=500;
  minX=0.;
  maxX=2000.;
  std::vector<std::string> sufix1;
  sufix1.push_back("_g1inFRin5CogEThr");
  for(int i=0; i<sufix1.size(); i++){
    hname="ECAL_g1RadiusRange90_260_twoPhotonInTime10ns" +sufix1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_g1RadiusRange110_240_twoPhotonInTime10ns"+sufix1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_g1RadiusRange130_220_twoPhotonInTime10ns"+sufix1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_g1RadiusRange150_200_twoPhotonInTime10ns"+sufix1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_g1RadiusRange134_216_twoPhotonInTime10ns"+sufix1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
  }


  std::vector<std::string> sufix2;
  sufix2.push_back("_g1inFRin5CogEThr");
  for(int i=0; i<sufix2.size(); i++){
    hname="ECAL_1TimeRange_twoPhotonInTime10ns"+sufix2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_2TimeRange_twoPhotonInTime10ns"+sufix2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_3TimeRange_twoPhotonInTime10ns"+sufix2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_4TimeRange_twoPhotonInTime10ns"+sufix2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_5TimeRange_twoPhotonInTime10ns"+sufix2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
  }

  binX=6;
  minX=0.5;
  maxX=5.5;
  hname="ECAL_nHitOnChannel_5CoG";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="NGammaGammaForEvent_TimeRequest"; 
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="NGammaGammaForEvent_10ns20DegreeDeltaThetaEthr90UnderPeack"; 
  hSvc->BookHisto(hname, binX, minX, maxX);

  return true;
}


