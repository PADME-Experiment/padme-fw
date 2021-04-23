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
#include <fstream>
#include <sstream>
#include <string>
#include "TFile.h"


struct IdEnergy 
{
    double clIdPosition;
    double clEnergy;
};
struct by_energy 
{
    bool operator()(IdEnergy const &a,IdEnergy const &b) const noexcept    
	{
	    return a.clEnergy > b.clEnergy;
	}
};



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

  fDataMCMethod=NULL;

 
}
AnnihilationSelection::AnnihilationSelection(Int_t processingMode, Int_t verbosityFlag, Bool_t NoTargetBool, Bool_t dataMCmethod)
{
  fRecoEvent      = NULL;
  fVerbose        = verbosityFlag;
  fNoTargetBool   = NoTargetBool;
  fdataMCmethod   = dataMCmethod;
  fInitToComplete =true;
  fDataMCMethod=new AnnihilationSelectionDataMCMethod();

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

  fdistanceTarget=1030+2513; // from calchep g4 hits studies
  fFRmin=115.82;
  fFRmid=172.83;
  fFRmax=258.0;

  fcountEvent=0;
  
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

  //from data, run 30547, latest result using: normalisation in [-150, -90], integral in 3sigma after double gaussian fit 
  //from data save {"0-45 out","0-45 in", "45-90 out","45-90 in", "90-135 out","90-135 in", "135-180 out","135-180 in", "180-225 out","180-225 in", "225-270 out","225-270 in", "270-315 out","270-315 in", "315-360 out","315-360 in"};
  //eff extracted 3/02/2021
  //Double_t tmpfEffInnerRRange_r1inFR[8]={ 0.805621,   0.829102,   0.87482,   0.935979,   0.928166 ,  0.854093,  0.846523 , 0.730279};
  //Double_t tmpfEffOuterRRange_r1inFR[8]={0.732969,0.636515,  0.667615, 0.975461,1., 0.662449,0.567017, 0.849711 };
  //Double_t tmpfEffInnerRRange_r1r2inFR[8]={ 0.79162 ,  0.822266 , 0.868097,  0.931237,   0.92056 , 0.851527,  0.83945,  0.714154};
  //Double_t tmpfEffOuterRRange_r1r2inFR[8]={0.572762, 0.578281,0.613415, 0.775343, 0.872763,  0.65011, 0.548761, 0.670221 };
  //for(int i=0; i<8; i++){
  //  fEffInnerRRange_r1inFR[i]=tmpfEffInnerRRange_r1inFR[i];
  //  fEffOuterRRange_r1inFR[i]=tmpfEffOuterRRange_r1inFR[i];
  //    
  //  fEffInnerRRange_r1r2inFR[i]=tmpfEffInnerRRange_r1r2inFR[i];
  //  fEffOuterRRange_r1r2inFR[i]=tmpfEffOuterRRange_r1r2inFR[i];
  //}
  //eff extracted 23/02/2021 with a cut on the custer time, rejected cluster in [-150, -110] becouse high energy spread
  
    //fillEffVector("provaggEfficiencyFromDiff_TagFR_sigmaStudies_30547_RejectM150M110_newRMid170.txt", "provaggEfficiencyFromDiff_TagAndProbeFR_sigmaStudies_30547_RejectM150M110_newRMid170.txt");
    
  //fillEffVector("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/provaggEfficiencyFromDiff_TagFR_MC_IdealAnnihilation_FixRangeFR115.8_250RMid173.txt", "/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/provaggEfficiencyFromDiff_TagAndProbeFR_MC_IdealAnnihilation_FixRange_FR115.8_250RMid173.txt");
  fillEffVector("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/provaggEfficiencyFromDiff_TagFR_MC_IdealAnnihilation_FixRangeFR115.8_250RMid173_randomDiedSU.txt", "/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/provaggEfficiencyFromDiff_TagAndProbeFR_MC_IdealAnnihilation_FixRange_FR115.8_250RMid173_randomDiedSU.txt");

  //fillEffVector("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/runsAnalysis/txtFileEff/provaggEfficiencyFromDiff_TagFR_sigmaStudies_30617_FR1158_25800_otherBkgForProblemPoins.txt", "/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/runsAnalysis/txtFileEff/provaggEfficiencyFromDiff_TagAndProbeFR_sigmaStudies_30617_FR1158_25800_otherBkgForProblemPoins.txt");

  //fillEffVector("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/runsAnalysis/txtFileEff/provaggEfficiencyFromDiff_TagFR_sigmaStudies_diffBkgNorm_fr1158_2580_run_0030624_20201115_101219.txt", "/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/runsAnalysis/txtFileEff/provaggEfficiencyFromDiff_TagAndProbeFR_sigmaStudies_diffBkgNorm_fr1158_2580_run_0030624_20201115_101219.txt"); 
  
  fillEffVectorTruth();
  //Double_t tmpfUpperSysInnerRRange_r1inFR[8]={ 0.0773673, 0.0396053, 0.0052547,  0.0246471,  0.00474522, 0.00500453, 0.0224981, 0.0310803};
  //Double_t tmpfUpperSysOuterRRange_r1inFR[8]={0.0176767, 0.0618193, 0.102187,  0.0246704, 0.0829614, 0.0851759 , 0.0822724 , 0.107262 };
  //
  //Double_t tmpfUpperSysInnerRRange_r1r2inFR[8]={0.0791538, 0.0368423, 0.0020704 , 0.0281219, 0.0258331, 0.00286638,  0.0179538,  0.0419224};
  //Double_t tmpfUpperSysOuterRRange_r1r2inFR[8]={0.00722965,  0.0646874,  0.0844937,  0.013134,  0.0505581,  0.100325,  0.0750257, 0.0631048 };
  //for(int i=0; i<8; i++){
  //  fEffUpperSysInnerRRange_r1inFR[i]=tmpfUpperSysInnerRRange_r1inFR[i]+fEffInnerRRange_r1inFR[i];
  //  fEffUpperSysOuterRRange_r1inFR[i]=tmpfUpperSysOuterRRange_r1inFR[i]+fEffOuterRRange_r1inFR[i];
  //    
  //  fEffUpperSysInnerRRange_r1r2inFR[i]=tmpfUpperSysInnerRRange_r1r2inFR[i]+fEffInnerRRange_r1r2inFR[i];
  //  fEffUpperSysOuterRRange_r1r2inFR[i]=tmpfUpperSysOuterRRange_r1r2inFR[i]+fEffOuterRRange_r1r2inFR[i];
  //}

  //eff extracted 23/02/2021 with a cut on the custer time, rejected cluster in [-150, -110] becouse high energy spread
 
  bool useAccsmeared=true;
  if(useAccsmeared){
    //this from calchep truth + calchep smeared
    //TFile *fAcc_g1g2inFR =new TFile("AccEff_450MeV_inFR106_258.root");
    //TFile *fAcc_g1inFR =new TFile("AccEff_g1inFR_450MeV_inFR106_258.root"); //most energetic photon in FR
    ////TFile *fAcc_g1g2inFR =new TFile("AccEff_430MeV_inFR106_258.root");
    ////TFile *fAcc_g1inFR =new TFile("AccEff_g1inFR_430MeV_inFR106_258.root"); //most energetic photon in FR
    //hAccEffFromCalchep_g1g2FR = (TH1D*)fAcc_g1g2inFR->Get("DivideTheta_ecalFR");
    //hAccEffFromCalchep_g1FR = (TH1D*)fAcc_g1inFR->Get("th1s_1inFR");

    TFile *fAcc_g1g2inFR =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/AccEffective_g1g2inFR_struth_calchep430MeV_FR115.8_258.root");
    TFile *fAcc_g1inFR =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/AccEffective_g1inFR_struth_calchep430MeV_FR115.8_258.root");
    hAccEffFromCalchep_g1g2FR = (TH1D*)fAcc_g1g2inFR->Get("divide_g1g2inFR");
    hAccEffFromCalchep_g1FR = (TH1D*)fAcc_g1inFR->Get("divide_g1inFR");
  }
  else{
    //this calchep truth + calchep reco
    TFile *fAcc_g1g2inFR =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/AccEffective_g1g2inFR_recotruth_calchep450MeV.root");
    TFile *fAcc_g1inFR =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/AccEffective_g1inFR_recotruth_calchep450MeV.root"); //most energetic photon in FR
    hAccEffFromCalchep_g1g2FR = (TH1D*)fAcc_g1g2inFR->Get("divide_g1g2inFR");
    hAccEffFromCalchep_g1FR = (TH1D*)fAcc_g1inFR->Get("divide_g1inFR");
  }
  
  if(fdataMCmethod)fDataMCMethod->Init(fEffInnerRRange_r1inFR, fEffOuterRRange_r1inFR, fEffInnerRRange_r1r2inFR, fEffOuterRRange_r1r2inFR);

  return true;
}


Bool_t AnnihilationSelection::Process(Bool_t isMC)
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

  if (!passPreselection(fNoTargetBool, isMC)) return true;

  Bool_t retCode = 0;
  TRecoVCluster* ecalclu=NULL;
  TRecoVCluster* ecalclu2=NULL;
  Int_t NClECal = fECal_ClColl->GetNElements();

  fIdCl_SortByEnergy.clear();
  SortCluster_byEnergy();
  
  Double_t EBeam=0.;
  if(isMC)EBeam=450.0;
  else EBeam=430.;
  //else EBeam=450.;
  
  if( NClECal != fIdCl_SortByEnergy.size()){
    std::cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!         error on sorted energy        !!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    return true;
  }

  for (Int_t jecal=0; jecal<NClECal; ++jecal){
    //ecalclu          = fECal_ClColl->Element(jecal);
    ecalclu          = fECal_ClColl->Element(fIdCl_SortByEnergy.at(jecal));
    Int_t NGGInTheSameEventTime10ns=0;
    Int_t NGGInTheSameEventTime10ns20DeltaPhiDeltaThetaEThr90UnderPeack=0;
            
    fg1x    =ecalclu->GetPosition().X();
    fg1y    =ecalclu->GetPosition().Y();
    fg1E    =ecalclu->GetEnergy();
    hname="Annihilation_ECalClusterEnergy";
    hSvc->FillHisto(hname, fg1E, 1.);
    fg1t    =ecalclu->GetTime();
    if(fg1t<-110) continue;//high energy spread in data October 2020  
    fR_1    = sqrt(fg1x*fg1x+ fg1y*fg1y+fdistanceTarget*fdistanceTarget);
    fg1Recal=sqrt(fg1x*fg1x+fg1y*fg1y);
    fPx_1   = fg1E*fg1x/ fR_1;
    fPy_1   = fg1E*fg1y/ fR_1;
    fPz_1   = fg1E*fdistanceTarget/ fR_1;
    TLorentzVector P4g1F, P4eTarget, P4eBeam;
    P4g1F.SetPxPyPzE(fPx_1,fPy_1,fPz_1, fg1E);
    //fthetag1 = P4g1F.Theta();
    fthetag1=atan(fg1Recal/fdistanceTarget);
    Double_t me=0.511;
    Double_t PzBeam=sqrt(EBeam*EBeam-me*me);
    P4eTarget.SetPxPyPzE(0,0,0, me);
    P4eBeam.SetPxPyPzE(0,0,PzBeam, EBeam);
    Int_t g1seedChId=fECal_hitEvent->Hit(ecalclu->GetSeed())->GetChannelId();//hit(ecalclu->GetSeed())->GetChannelId();
    for(Int_t jecal2=jecal+1; jecal2<NClECal; jecal2++){                ///starting second cluster loop
      //ecalclu2 = fECal_ClColl->Element(jecal2);
      ecalclu2 = fECal_ClColl->Element(fIdCl_SortByEnergy.at(jecal2));
      fpos1 = ecalclu->GetPosition();
      fpos2 = ecalclu2->GetPosition();
      fg2x        =ecalclu2->GetPosition().X();
      fg2y        =ecalclu2->GetPosition().Y();
      fg2E        =ecalclu2->GetEnergy();
      fg2t        =ecalclu2->GetTime();
      if(fg2t<-110) continue;//high energy spread in data October 2020  
      fg2Recal    =sqrt(fg2x*fg2x+fg2y*fg2y);
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
      //fthetag2               = P4g2F.Theta();
      fthetag2=atan(fg2Recal/fdistanceTarget);
      fphig1 = P4g1F.Phi();
      fphig2 = P4g2F.Phi();

      if(fabs(fg1t-fg2t)>10.)continue;                      //two clsuter in time 10 ns
      NGGInTheSameEventTime10ns++;

      Bool_t firstGInFR_def=false;
      Bool_t secondGInFR_def=false;
      //new FRed by TagAndProbe studies
      //if(fg1Recal>106 && fg1Recal<258 )  firstGInFR_def=true;
      //if(fg2Recal>106 && fg2Recal<258)  secondGInFR_def=true;

      if(fg1Recal>fFRmin && fg1Recal<fFRmax )  firstGInFR_def=true;
      if(fg2Recal>fFRmin && fg2Recal<fFRmax)  secondGInFR_def=true;
     
      //old FR , not symmetrical ...see tag and probe studies
      //if(fg1Recal>92 && fg1Recal<258 )  firstGInFR_def=true;
      //if(fg2Recal>92 && fg2Recal<258)  secondGInFR_def=true;

      //if(!firstGInFR_def)continue;       
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
      if(firstGInFR_def){
	FillGeneralHisto("_g1inFRDTime10");
	if(secondGInFR_def)FillGeneralHisto("_g1g2inFRDTime10");
	FillWeightedHisto_R1inFR("_g1inFRDTime10");
	if(secondGInFR_def){
	  FillWeightedHisto_R1R2inFR("_g1g2inFRDTime10");
	  FillWeightedHisto_R1R2inFR2approach("_g1g2inFRDTime10");
	}
	FillAccEffHisto_R1inFR("_g1inFRDTime10");
	FillWeightedAccEffHisto_R1inFR("_g1inFRDTime10");
	if(secondGInFR_def)FillAccEffHisto_R1R2inFR("_g1g2inFRDTime10");
	if(secondGInFR_def)FillWeightedAccEffHisto_R1R2inFR("_g1g2inFRDTime10");

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

	  if(fdataMCmethod)fDataMCMethod->Process(fg1E, fphig1, fg1Recal , fg2E , fphig2, fg2Recal);

	  Int_t countHit1_5CoG=0;
	  Int_t countHit2_5CoG=0;
	  CheckNumberHitsOnCh(g1seedChId, g2seedChId, countHit1_5CoG, countHit2_5CoG);
	  hname="ECAL_nHitOnChannel_5CoG";
	  hSvc->FillHisto(hname, countHit1_5CoG, 1.);
	  hSvc->FillHisto(hname, countHit2_5CoG, 1.);
	  FillGeneralHisto("_g1inFRin5Cog");
	  FillWeightedHisto_R1inFR("_g1inFRin5Cog");
	  if(secondGInFR_def)FillWeightedHisto_R1R2inFR("_g1g2inFRin5Cog");
	  FillWeightedTruthHisto_R1inFR("_g1inFRin5Cog");
	  if(secondGInFR_def){
	    FillWeightedTruthHisto_R1R2inFR("_g1g2inFRin5Cog");
	    FillWeightedHisto_R1R2inFR2approach("_g1g2inFRin5Cog");
	  }
	  FillAccEffHisto_R1inFR("_g1inFRin5Cog");
	  FillWeightedAccEffHisto_R1inFR("_g1inFRin5Cog");
	  if(secondGInFR_def)FillAccEffHisto_R1R2inFR("_g1g2inFRin5Cog");
	  if(secondGInFR_def)FillWeightedAccEffHisto_R1R2inFR("_g1g2inFRin5Cog");

	  if(TotEnergyCut){FillGeneralHisto("_g1inFRin5CogUnderPeak");}
	  if(InEnergy){FillGeneralHisto("_g1inFRin5CogEThr");}
	  if(TotEnergyCut && InEnergy ){FillGeneralHisto("_g1inFRin5CogUnderPeakEThr");}
	  if(InEnergy){
	    FillRadiusRangeHisto("_g1inFRin5CogEThr");
	    FillTimeRangeHisto("_g1inFRin5CogEThr");
	    FillTimeRangeCoGHisto("_g1inFRin5CogEThr");
	  }
	}
	if(fabs(fXWeighted)<50. && fabs(fYWeighted)<50. && secondGInFR_def ){
	  FillGeneralHisto("_g1g2inFRin5Cog");
	  if(TotEnergyCut && InEnergy ){FillGeneralHisto("_g1g2inFRin5CogUnderPeakEThr");}
	}
      }
      //second photon in FR
      if(secondGInFR_def){
	if(fabs(fXWeighted)<50. && fabs(fYWeighted)<50.){
	  FillGeneralHisto("_g2inFRin5Cog");
	  FillWeightedHisto_R2inFR("_g2inFRin5Cog");
	  if(TotEnergyCut && InEnergy ){FillGeneralHisto("_g2inFRin5CogUnderPeakEThr");}
	}
      }


      hname="NGammaGammaForEvent_TimeRequest"; 
      hSvc->FillHisto(hname,NGGInTheSameEventTime10ns , 1.);
      hname="NGammaGammaForEvent_10ns20DegreeDeltaThetaEthr90UnderPeack"; 
      hSvc->FillHisto(hname,NGGInTheSameEventTime10ns20DeltaPhiDeltaThetaEThr90UnderPeack, 1.);
  
    }
  }

  return true;
}

void AnnihilationSelection::FillWeightedHisto_R1inFR(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Bool_t plusSystematic=false;
  //Double_t effg1=ReturnEfficiencyR1R2inFR(fg1Recal,fphig1,plusSystematic);
  Double_t effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  Double_t effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  Double_t effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffR1inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
  plusSystematic=true;
  //effg1=ReturnEfficiencyR1R2inFR(fg1Recal,fphig1,plusSystematic);
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffPlusSysR1inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
}

void AnnihilationSelection::FillWeightedHisto_R2inFR(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Bool_t plusSystematic=false;
  Double_t effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  Double_t effg2=ReturnEfficiencyR1R2inFR(fg2Recal,fphig2,plusSystematic);
  Double_t effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffR2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
}


void AnnihilationSelection::FillWeightedHisto_R1R2inFR(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Bool_t  plusSystematic=false;
  //Double_t effg1=ReturnEfficiencyR1R2inFR(fg1Recal,fphig1,plusSystematic);
  Double_t effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  Double_t effg2=ReturnEfficiencyR1R2inFR(fg2Recal,fphig2,plusSystematic);
  Double_t effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
  plusSystematic=true;
  //effg1=ReturnEfficiencyR1R2inFR(fg1Recal,fphig1,plusSystematic);
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  effg2=ReturnEfficiencyR1R2inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffPlusSysR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
}


void AnnihilationSelection::FillWeightedHisto_R1R2inFR2approach(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Bool_t  plusSystematic=false;
  //Double_t effg1=ReturnEfficiencyR1R2inFR(fg1Recal,fphig1,plusSystematic);
  Double_t effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  Double_t effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  Double_t Acc=ReturnAccettanceEffective_g1g2FR(fg1Recal);
  Double_t effScaleFactor=(1./effg1)*(1./effg2)*(1./Acc);
  hname="ECAL_gravTwoPhoton10ns_W2ndApproachR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
 
}

void AnnihilationSelection::FillWeightedTruthHisto_R1inFR(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  //Double_t effg1=ReturnEfficiencyTruthR1R2inFR(fg1Recal,fphig1);
  Double_t effg1=ReturnEfficiencyTruthR1inFR(fg1Recal,fphig1);
  Double_t effg2=ReturnEfficiencyTruthR1inFR(fg2Recal,fphig2);
  Double_t effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffTruthR1inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
}

void AnnihilationSelection::FillWeightedTruthHisto_R1R2inFR(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  //Double_t effg1=ReturnEfficiencyTruthR1R2inFR(fg1Recal,fphig1);
  Double_t effg1=ReturnEfficiencyTruthR1inFR(fg1Recal,fphig1);
  Double_t effg2=ReturnEfficiencyTruthR1R2inFR(fg2Recal,fphig2);
  Double_t effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffTruthR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
}


void AnnihilationSelection::FillAccEffHisto_R1inFR(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Double_t Acc=ReturnAccettanceEffective_g1FR(fg1Recal); 
  Double_t effScaleFactor=(1./Acc);
  hname="ECAL_gravTwoPhoton10ns_WAccR1inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
}

void AnnihilationSelection::FillAccEffHisto_R1R2inFR(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Double_t Acc=ReturnAccettanceEffective_g1g2FR(fg1Recal);
  Double_t effScaleFactor=(1./Acc);
  hname="ECAL_gravTwoPhoton10ns_WAccR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
}

void AnnihilationSelection::FillWeightedAccEffHisto_R1inFR(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Bool_t  plusSystematic=false;
  Double_t effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  //Double_t effg1=ReturnEfficiencyR1R2inFR(fg1Recal,fphig1,plusSystematic);
  Double_t effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  Double_t Acc=ReturnAccettanceEffective_g1FR(fg1Recal);
  Double_t effScaleFactor=(1./effg1)*(1./effg2)*(1/Acc);
  hname="ECAL_gravTwoPhoton10ns_WAccEffR1inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
  plusSystematic=true;
  //effg1=ReturnEfficiencyR1R2inFR(fg1Recal,fphig1,plusSystematic);
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  Acc=ReturnAccettanceEffective_g1FR(fg1Recal);
  effScaleFactor=(1./effg1)*(1./effg2)*(1/Acc);
  hname="ECAL_gravTwoPhoton10ns_WAccEffPlusSysR1inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
}

void AnnihilationSelection::FillWeightedAccEffHisto_R1R2inFR(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Bool_t  plusSystematic=false;
  //Double_t effg1=ReturnEfficiencyR1R2inFR(fg1Recal,fphig1,plusSystematic);
  Double_t effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  Double_t effg2=ReturnEfficiencyR1R2inFR(fg2Recal,fphig2,plusSystematic);
  Double_t Acc=ReturnAccettanceEffective_g1g2FR(fg1Recal);
  Double_t effScaleFactor=(1./effg1)*(1./effg2)*(1/Acc);
  hname="ECAL_gravTwoPhoton10ns_WAccEffR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
  plusSystematic=true;
  //effg1=ReturnEfficiencyR1R2inFR(fg1Recal,fphig1,plusSystematic);
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  effg2=ReturnEfficiencyR1R2inFR(fg2Recal,fphig2,plusSystematic);
  Acc=ReturnAccettanceEffective_g1g2FR(fg1Recal);
  effScaleFactor=(1./effg1)*(1./effg2)*(1/Acc);
  hname="ECAL_gravTwoPhoton10ns_WAccEffPlusSysR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
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
  hname="ECAL_R2ggVsR1gg_10ns"+sufix;
  hSvc->FillHisto2(hname,fg1Recal,fg2Recal);
  hname="ECAL_tggVsEgg_10ns"+sufix;
  hSvc->FillHisto2(hname,fg1t, fg1E);
  hSvc->FillHisto2(hname,fg2t, fg2E);
  hname="ECAL_Energy1vsEnergy2"+sufix;
  hSvc->FillHisto2(hname, fg1E, fg2E);
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


void AnnihilationSelection::FillTimeRangeCoGHisto(std::string sufix){

  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
 
  if(fg1t>ftimerange1min && fg1t<=ftimerange1max){
    hname="ECAL_1TimeRange_XCoGInTime10ns"+sufix;
    hSvc->FillHisto(hname,fXWeighted,1.);
    hname="ECAL_1TimeRange_YCoGInTime10ns"+sufix;
    hSvc->FillHisto(hname,fYWeighted,1.);
  }
  if(fg1t>ftimerange2min && fg1t<=ftimerange2max){
    hname="ECAL_2TimeRange_XCoGInTime10ns"+sufix;
    hSvc->FillHisto(hname,fXWeighted,1.);
    hname="ECAL_2TimeRange_YCoGInTime10ns"+sufix;
    hSvc->FillHisto(hname,fYWeighted,1.);
  }
  if(fg1t>ftimerange3min && fg1t<=ftimerange3max){
    hname="ECAL_3TimeRange_XCoGInTime10ns"+sufix;
    hSvc->FillHisto(hname,fXWeighted,1.);
    hname="ECAL_3TimeRange_YCoGInTime10ns"+sufix;
    hSvc->FillHisto(hname,fYWeighted,1.);
  }
  if(fg1t>ftimerange4min && fg1t<=ftimerange4max){
    hname="ECAL_4TimeRange_XCoGInTime10ns"+sufix;
    hSvc->FillHisto(hname,fXWeighted,1.);
    hname="ECAL_4TimeRange_YCoGInTime10ns"+sufix;
    hSvc->FillHisto(hname,fYWeighted,1.);
  }
  if(fg1t>ftimerange5min && fg1t<=ftimerange5max){
    hname="ECAL_5TimeRange_XCoGInTime10ns"+sufix;
    hSvc->FillHisto(hname,fXWeighted,1.);
    hname="ECAL_5TimeRange_YCoGInTime10ns"+sufix;
    hSvc->FillHisto(hname,fYWeighted,1.);
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
 
  Double_t targetConst=1.78; //to use olny if the run is taken from September 2020 !!!! -> corrected in develop
  //if(isMC) targetConst=1; 
   targetConst=1; 
  std::string hname1 = "AnnMet_NposInBunch_beam";
  hSvc->FillHisto(hname1,fTarget_RecoBeam->getnPOT()*targetConst);
  if (!isMC && !isTargetOut && fTarget_RecoBeam->getnPOT()<13000) return passed;
  //std::cout<<"tagetnPOT "<<fTarget_RecoBeam->getnPOT() << " and isTargetOutBool " << isTargetOut<< std::endl;
  if (isTargetOut && fSAC_ClColl->GetNElements()>15) return passed;
  
  //if(fRecoEvent->GetEventNumber()>270000) return passed;

  hname = "AnnMet_nPOT";
  hSvc->FillHisto(hname,0,float(fTarget_RecoBeam->getnPOT()*targetConst)); 
  passed = true;
  return passed;
}

Double_t AnnihilationSelection::ReturnEfficiencyR1inFR(Double_t radius, Double_t phi, Bool_t PlusSystematic)
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
   std::string hname;
  Bool_t inner=false;
  Bool_t outer=false;
  //if(radius>106. && radius<182)inner=true;
  //if(radius>=182. && radius<258)outer=true;
  //if(radius<182. )inner=true;
  //if(radius>=182.)outer=true;
  //if(radius<170. )inner=true;
  //if(radius>=170.)outer=true;
  if(radius<fFRmid )inner=true;
  if(radius>=fFRmid)outer=true;
  Double_t phiDeg=phi*360./(2*TMath::Pi());
  if(phiDeg<0.)phiDeg=360+phiDeg;
  int phiSlice=phiDeg/45.;
  Double_t eff;
  if(PlusSystematic){
    if(inner)eff=fEffUpperSysInnerRRange_r1inFR[phiSlice];
    else if(outer)eff=fEffUpperSysOuterRRange_r1inFR[phiSlice];
  }
  else{
    if(inner){
      eff=fEffInnerRRange_r1inFR[phiSlice];
      hname="ECAL_R1inFR_EfficiencyVSPhi_InnerRadius";
      hSvc->FillHisto2(hname, phiDeg, eff);
    }
    else if(outer){
      eff=fEffOuterRRange_r1inFR[phiSlice];
      hname="ECAL_R1inFR_EfficiencyVSPhi_OutRadius";
      hSvc->FillHisto2(hname, phiDeg, eff);
    }
  }


  return eff;
}

Double_t AnnihilationSelection::ReturnEfficiencyTruthR1inFR(Double_t radius, Double_t phi)
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
   std::string hname;
  Bool_t inner=false;
  Bool_t outer=false;
  if(radius<fFRmid )inner=true;
  if(radius>=fFRmid)outer=true;
  Double_t phiDeg=phi*360./(2*TMath::Pi());
  if(phiDeg<0.)phiDeg=360+phiDeg;
  int phiSlice=phiDeg/45.;
  Double_t eff;
  if(inner){
    eff=fEffInnerRRangeFromTruth_r1inFR[phiSlice];
  }
  else if(outer){
    eff=fEffOuterRRangeFromTruth_r1inFR[phiSlice];
  }
  return eff;
}



Double_t AnnihilationSelection::ReturnEfficiencyR1R2inFR(Double_t radius, Double_t phi, Bool_t PlusSystematic)
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Bool_t inner=false;
  Bool_t outer=false;
  //if(radius>106. && radius<182.)inner=true;
  //if(radius>=182. && radius<258.)outer=true;
  //if(radius>106. && radius<170.)inner=true;
  //if(radius>=170 && radius<258.)outer=true;
  if(radius>fFRmin && radius<fFRmid)inner=true;
  if(radius>=fFRmid && radius<fFRmax)outer=true;
  Double_t phiDeg=phi*360/(2*TMath::Pi());
  if(phiDeg<0.)phiDeg=360+phiDeg;
  int phiSlice=phiDeg/45.;
  Double_t eff;
  if(PlusSystematic){
    if(inner)eff=fEffUpperSysInnerRRange_r1r2inFR[phiSlice];
    else if(outer)eff=fEffUpperSysOuterRRange_r1r2inFR[phiSlice];
  }
  else{
    if(inner){
      eff=fEffInnerRRange_r1r2inFR[phiSlice];
      hname="ECAL_R1R2inFR_EfficiencyVSPhi_InnerRadius";
      hSvc->FillHisto2(hname, phiDeg, eff);
    }
    else if(outer){
      eff=fEffOuterRRange_r1r2inFR[phiSlice];
      hname="ECAL_R1R2inFR_EfficiencyVSPhi_OutRadius";
      hSvc->FillHisto2(hname, phiDeg, eff);
    }
  }

  return eff;
}


Double_t AnnihilationSelection::ReturnEfficiencyTruthR1R2inFR(Double_t radius, Double_t phi)
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
   std::string hname;
  Bool_t inner=false;
  Bool_t outer=false;
  if(radius<fFRmid )inner=true;
  if(radius>=fFRmid)outer=true;
  Double_t phiDeg=phi*360./(2*TMath::Pi());
  if(phiDeg<0.)phiDeg=360+phiDeg;
  int phiSlice=phiDeg/45.;
  Double_t eff;
  if(inner){
    eff=fEffInnerRRangeFromTruth_r1r2inFR[phiSlice];
  }
  else if(outer){
    eff=fEffOuterRRangeFromTruth_r1r2inFR[phiSlice];
  }
  return eff;
}



Double_t AnnihilationSelection::ReturnAccettanceEffective_g1g2FR(Double_t radius)
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Double_t theta=atan2(radius, fdistanceTarget);
  Double_t bin=hAccEffFromCalchep_g1g2FR->FindBin(theta);
  Double_t acc=hAccEffFromCalchep_g1g2FR->GetBinContent(bin);
  hname="ECAL_AcceptanceEffective_r1r2inFR";
  hSvc->FillHisto(hname,acc,1. );
  hname="ECAL_AcceptanceEffectiveVStheta1_r1r2inFR";
  hSvc->FillHisto2(hname,theta,acc,1. );
  return acc;
}

Double_t AnnihilationSelection::ReturnAccettanceEffective_g1FR(Double_t radius)
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Double_t theta=atan2(radius, fdistanceTarget);
  Double_t bin=hAccEffFromCalchep_g1FR->FindBin(theta);
  Double_t acc=hAccEffFromCalchep_g1FR->GetBinContent(bin);
  hname="ECAL_AcceptanceEffective_r1inFR";
  hSvc->FillHisto(hname,acc,1. );
  hname="ECAL_AcceptanceEffectiveVStheta1_r1inFR";
  hSvc->FillHisto2(hname,theta,acc,1. );
  return acc;
}


void AnnihilationSelection::fillEffVector(std::string fnameTagFR, std::string fnameTagProbeFR){


  std::ifstream tagfr(fnameTagFR.c_str());
  std::string line;
  std::string tmp;
  while( std::getline( tagfr, line ))
    {
      std::istringstream iss(line);
      if(line.find("3sigma")!=std::string::npos){
      	std::getline( tagfr, line);
	std::istringstream eff(line);
	if(eff>>tmp>>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >> 
	        tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>
	        fEffOuterRRange_r1inFR[0]>>fEffInnerRRange_r1inFR[0]>>fEffOuterRRange_r1inFR[1]>>fEffInnerRRange_r1inFR[1]>>fEffOuterRRange_r1inFR[2]>>fEffInnerRRange_r1inFR[2]>>fEffOuterRRange_r1inFR[3]>>fEffInnerRRange_r1inFR[3]>>fEffOuterRRange_r1inFR[4]>>fEffInnerRRange_r1inFR[4]>>fEffOuterRRange_r1inFR[5]>>fEffInnerRRange_r1inFR[5]>>fEffOuterRRange_r1inFR[6]>>fEffInnerRRange_r1inFR[6]>>fEffOuterRRange_r1inFR[7]>>fEffInnerRRange_r1inFR[7]>>
	   tmp >>tmp >>tmp >>tmp >>tmp >>tmp )std::cout<<"outer range tag fr "<<fEffOuterRRange_r1inFR[0]<< std::endl;
      }
      if(line.find("errEffSysUpper")!=std::string::npos){
      	//std::getline( tagfr, line);
	std::istringstream effSysSup(line);
	if(effSysSup>>tmp>>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >> 
	        tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>
	        fEffUpperSysOuterRRange_r1inFR[0]>>fEffUpperSysInnerRRange_r1inFR[0]>>fEffUpperSysOuterRRange_r1inFR[1]>>fEffUpperSysInnerRRange_r1inFR[1]>>fEffUpperSysOuterRRange_r1inFR[2]>>fEffUpperSysInnerRRange_r1inFR[2]>>fEffUpperSysOuterRRange_r1inFR[3]>>fEffUpperSysInnerRRange_r1inFR[3]>>fEffUpperSysOuterRRange_r1inFR[4]>>fEffUpperSysInnerRRange_r1inFR[4]>>fEffUpperSysOuterRRange_r1inFR[5]>>fEffUpperSysInnerRRange_r1inFR[5]>>fEffUpperSysOuterRRange_r1inFR[6]>>fEffUpperSysInnerRRange_r1inFR[6]>>fEffUpperSysOuterRRange_r1inFR[7]>>fEffUpperSysInnerRRange_r1inFR[7]>>
	   tmp >>tmp >>tmp >>tmp >>tmp >>tmp )std::cout<<"outer range tag fr sys "<<fEffUpperSysOuterRRange_r1inFR[0]<< std::endl;
      }
    }//end while cycle


  std::ifstream tagprobefr(fnameTagProbeFR.c_str());
  while( std::getline( tagprobefr, line ))
    {
      std::istringstream iss(line);
      if(line.find("3sigma")!=std::string::npos){
      	std::getline( tagprobefr, line);
	std::istringstream eff(line);
	if(eff>>tmp>>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >> 
	        tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>
	        fEffOuterRRange_r1r2inFR[0]>>fEffInnerRRange_r1r2inFR[0]>>fEffOuterRRange_r1r2inFR[1]>>fEffInnerRRange_r1r2inFR[1]>>fEffOuterRRange_r1r2inFR[2]>>fEffInnerRRange_r1r2inFR[2]>>fEffOuterRRange_r1r2inFR[3]>>fEffInnerRRange_r1r2inFR[3]>>fEffOuterRRange_r1r2inFR[4]>>fEffInnerRRange_r1r2inFR[4]>>fEffOuterRRange_r1r2inFR[5]>>fEffInnerRRange_r1r2inFR[5]>>fEffOuterRRange_r1r2inFR[6]>>fEffInnerRRange_r1r2inFR[6]>>fEffOuterRRange_r1r2inFR[7]>>fEffInnerRRange_r1r2inFR[7]>>
		tmp >>tmp >>tmp >>tmp >>tmp >>tmp )std::cout<<"outer range tag probe fr "<<fEffOuterRRange_r1r2inFR[0]<< std::endl;
      }
      if(line.find("errEffSysUpper")!=std::string::npos){
      	//std::getline( tagprobefr, line);
	std::istringstream effSysSup(line);
	if(effSysSup>>tmp>>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >> 
	        tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>
	        fEffUpperSysOuterRRange_r1r2inFR[0]>>fEffUpperSysInnerRRange_r1r2inFR[0]>>fEffUpperSysOuterRRange_r1r2inFR[1]>>fEffUpperSysInnerRRange_r1r2inFR[1]>>fEffUpperSysOuterRRange_r1r2inFR[2]>>fEffUpperSysInnerRRange_r1r2inFR[2]>>fEffUpperSysOuterRRange_r1r2inFR[3]>>fEffUpperSysInnerRRange_r1r2inFR[3]>>fEffUpperSysOuterRRange_r1r2inFR[4]>>fEffUpperSysInnerRRange_r1r2inFR[4]>>fEffUpperSysOuterRRange_r1r2inFR[5]>>fEffUpperSysInnerRRange_r1r2inFR[5]>>fEffUpperSysOuterRRange_r1r2inFR[6]>>fEffUpperSysInnerRRange_r1r2inFR[6]>>fEffUpperSysOuterRRange_r1r2inFR[7]>>fEffUpperSysInnerRRange_r1r2inFR[7]>>
	   tmp >>tmp >>tmp >>tmp >>tmp >>tmp )std::cout<<"outer range tag probe fr sys"<<fEffUpperSysOuterRRange_r1r2inFR[0]<< std::endl;
      }
    }//end while cycle
  /*
  for(int i=0; i<8; i++){
    fEffUpperSysOuterRRange_r1inFR[i]  =fEffUpperSysOuterRRange_r1inFR[i]   + fEffOuterRRange_r1inFR[i];
    fEffUpperSysOuterRRange_r1r2inFR[i]=fEffUpperSysOuterRRange_r1r2inFR[i] + fEffOuterRRange_r1r2inFR[i];
    fEffUpperSysInnerRRange_r1inFR[i]  =fEffUpperSysInnerRRange_r1inFR[i]   + fEffInnerRRange_r1inFR[i];
    fEffUpperSysInnerRRange_r1r2inFR[i]=fEffUpperSysInnerRRange_r1r2inFR[i] + fEffInnerRRange_r1r2inFR[i];
        
    if(fEffOuterRRange_r1inFR[i]>1.) fEffOuterRRange_r1inFR[i]=1.;
    if(fEffOuterRRange_r1r2inFR[i]>1.) fEffOuterRRange_r1r2inFR[i]=1.;
    if(fEffInnerRRange_r1inFR[i]>1.) fEffOuterRRange_r1inFR[i]=1.;
    if(fEffInnerRRange_r1r2inFR[i]>1.) fEffOuterRRange_r1r2inFR[i]=1.;
    if(fEffUpperSysOuterRRange_r1inFR[i]  >1.)fEffUpperSysOuterRRange_r1inFR[i]  =1.;
    if(fEffUpperSysOuterRRange_r1r2inFR[i]>1.)fEffUpperSysOuterRRange_r1r2inFR[i]=1.;
    if(fEffUpperSysInnerRRange_r1inFR[i]  >1.)fEffUpperSysInnerRRange_r1inFR[i]  =1.;
    if(fEffUpperSysInnerRRange_r1r2inFR[i]>1.)fEffUpperSysInnerRRange_r1r2inFR[i]=1.;
    
    }*/

}

void AnnihilationSelection::fillEffVectorTruth(){
//g1inFR 1.00444 0.967738 1.01185 1.02237 1.01882 1.01919 0.985641 0.952614 0.978396 0.948886 1.00498 1.0104 0.970007 0.986012 0.986831 0.959898
//g1g2inFR 0.97273 0.933219 0.954395 0.969104 0.955429 0.961473 0.949275 0.910046 0.948846 0.914181 0.948037 0.958628 0.914865 0.935757 0.958247 0.927363

  //Double_t tmpfEffInnerRRange_r1inFR[8]={ 0.967738,  1.02237, 1.01919, 0.952614, 0.948886,1.0104, 0.986012, 0.959898};
  //Double_t tmpfEffOuterRRange_r1inFR[8]={1.00444, 1.01185, 1.01882 , 0.985641 , 0.978396 , 1.00498 , 0.970007 , 0.986831};
  //Double_t tmpfEffInnerRRange_r1r2inFR[8]={0.933219,  0.969104,  0.961473,  0.910046,  0.914181 , 0.958628, 0.935757, 0.927363};
  //Double_t tmpfEffOuterRRange_r1r2inFR[8]={0.97273 , 0.954395, 0.955429, 0.949275, 0.948846, 0.948037 , 0.914865 , 0.958247};

  //extracted with no baco in phi plus random SU died (on 45-90 deg)
  Double_t tmpfEffInnerRRange_r1inFR[8]  ={0.999019 , 0.969775 , 1.01461 , 0.997533 , 0.991246 , 0.948591 , 0.973229 , 0.977754};
  Double_t tmpfEffOuterRRange_r1inFR[8]  ={0.999633 , 0.973006 , 0.989611 , 0.983603 , 1.01689 , 1.00221 , 1.01803 , 1.00743};
  Double_t tmpfEffInnerRRange_r1r2inFR[8]={0.955572 , 0.924146 , 0.971135 , 0.954787 , 0.946952 , 0.899782 , 0.928067 , 0.933817};
  Double_t tmpfEffOuterRRange_r1r2inFR[8]={0.954751 , 0.9242 , 0.943643 , 0.941048 , 0.972736 , 0.956847 , 0.97449 , 0.9635};

  //reconstruction efficiency, nreco/ntruth no additional requests random SU died (on 45-90 deg)
  Double_t tmpfEffInnerRRange[8]  ={1.0027 , 0.97404 , 1.01944 , 1.00086 , 0.996353 , 0.977138 , 0.983208 , 0.981502};
  Double_t tmpfEffOuterRRange[8]  ={1.00917 , 0.981031 , 1 , 0.997571 , 1.02639 , 1.02842 , 1.02749 , 1.02156};

  for(int i=0; i<8; i++){
    //fEffInnerRRangeFromTruth_r1inFR[i]  =tmpfEffInnerRRange_r1inFR[i];	
    //fEffOuterRRangeFromTruth_r1inFR[i]  =tmpfEffOuterRRange_r1inFR[i];	
    //fEffInnerRRangeFromTruth_r1r2inFR[i]=tmpfEffInnerRRange_r1r2inFR[i];
    //fEffOuterRRangeFromTruth_r1r2inFR[i]=tmpfEffOuterRRange_r1r2inFR[i];

    //i'm applying the reconstruction efficiency...of course there is no selection g1 , g1g2 in FR, so for both the selection the efficiency is the same 
    fEffInnerRRangeFromTruth_r1inFR[i]  =tmpfEffInnerRRange[i];	
    fEffOuterRRangeFromTruth_r1inFR[i]  =tmpfEffOuterRRange[i];	
    fEffInnerRRangeFromTruth_r1r2inFR[i]=tmpfEffInnerRRange[i];
    fEffOuterRRangeFromTruth_r1r2inFR[i]=tmpfEffOuterRRange[i];
  }

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
  sufix.push_back("_g1g2inFRDTime10");
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
  sufix.push_back("_g2inFRin5Cog");
  sufix.push_back("_g2inFRin5CogUnderPeakEThr");
  for(int i=0; i<sufix.size(); i++){
    binX=250;
    minX=0.;
    maxX=1000.;
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
    binX=400;
    minX=0.;
    maxX=0.1;
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
    binX=350;
    minX=0.;
    maxX=350.;
    hname="ECAL_R2ggVsR1gg_10ns"+sufix.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX, binX, minX, maxX);
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
    minX=0.01;
    maxX=0.1;
    binY=200;
    minY=0.01;
    maxY=0.1;
    hname="ECAL_Theta2vsTheta1"+sufix.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    binX=600;
    minX=0.;
    maxX=800.;
    hname="ECAL_ThetavsEnergy"+sufix.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    hname="ECAL_Energy1vsEnergy2"+sufix.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX, binX, minX, maxX);
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

  //eff weighted hitograms
  std::vector<std::string> sufixW;
  sufixW.push_back("_g1inFRDTime10");
  sufixW.push_back("_g1inFRin5Cog");
  for(int i=0; i<sufixW.size(); i++){
    binX=500;
    minX=0.;
    maxX=2000.;
    hname="ECAL_gravTwoPhoton10ns_WEffR1inFR" + sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WEffPlusSysR1inFR" + sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WAccR1inFR"+sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WAccEffR1inFR"+sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WAccEffPlusSysR1inFR"+sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    
  }
  std::vector<std::string> sufixW1;
  sufixW1.push_back("_g2inFRin5Cog");
  for(int i=0; i<sufixW1.size(); i++){
    binX=500;
    minX=0.;
    maxX=2000.;
    hname="ECAL_gravTwoPhoton10ns_WEffR2inFR" + sufixW1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);   
  }

  std::vector<std::string> sufixW2;
  sufixW2.push_back("_g1g2inFRDTime10");
  sufixW2.push_back("_g1g2inFRin5Cog");
  for(int i=0; i<sufixW2.size(); i++){
    binX=500;
    minX=0.;
    maxX=2000.;
    hname="ECAL_gravTwoPhoton10ns_WEffR1R2inFR" + sufixW2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WEffPlusSysR1R2inFR" + sufixW2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WAccR1R2inFR"+sufixW2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WAccEffR1R2inFR"+sufixW2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WAccEffPlusSysR1R2inFR"+sufixW2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_W2ndApproachR1R2inFR"+sufixW2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
  }

  std::vector<std::string> sufixT;
  sufixT.push_back("_g1inFRin5Cog");
  for(int i=0; i<sufixT.size(); i++){
    binX=500;
    minX=0.;
    maxX=2000.;
    hname="ECAL_gravTwoPhoton10ns_WEffTruthR1inFR"+sufixT.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
  }

  std::vector<std::string> sufixT2;
  sufixT2.push_back("_g1g2inFRin5Cog");
  for(int i=0; i<sufixT2.size(); i++){
    binX=500;
    minX=0.;
    maxX=2000.;
    hname="ECAL_gravTwoPhoton10ns_WEffTruthR1R2inFR"+sufixT2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
  }


  binX=50;
  minX=0.;
  maxX=1.5;
  hname="ECAL_AcceptanceEffective_r1inFR";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_AcceptanceEffective_r1r2inFR";
  hSvc->BookHisto(hname, binX, minX, maxX);
  binX=100;
  minX=0.;
  maxX=0.15;
  binY=50;
  minY=0.;
  maxY=1.5;
  hname="ECAL_AcceptanceEffectiveVStheta1_r1inFR";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY );
  hname="ECAL_AcceptanceEffectiveVStheta1_r1r2inFR";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY );

  binX=370;
  minX=-10;
  maxX=360;
  binY=50;
  minY=0.;
  maxY=1.5;
  hname="ECAL_R1inFR_EfficiencyVSPhi_InnerRadius";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY );
  hname="ECAL_R1inFR_EfficiencyVSPhi_OutRadius";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY );
  hname="ECAL_R1R2inFR_EfficiencyVSPhi_InnerRadius";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY );
  hname="ECAL_R1R2inFR_EfficiencyVSPhi_OutRadius";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY );
  
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
    binX=400;
    minX=-60;
    maxX=60.;
    hname="ECAL_1TimeRange_XCoGInTime10ns"+sufix2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_2TimeRange_XCoGInTime10ns"+sufix2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_3TimeRange_XCoGInTime10ns"+sufix2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_4TimeRange_XCoGInTime10ns"+sufix2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_5TimeRange_XCoGInTime10ns"+sufix2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_1TimeRange_YCoGInTime10ns"+sufix2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_2TimeRange_YCoGInTime10ns"+sufix2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_3TimeRange_YCoGInTime10ns"+sufix2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_4TimeRange_YCoGInTime10ns"+sufix2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_5TimeRange_YCoGInTime10ns"+sufix2.at(i);
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

  if(fdataMCmethod)fDataMCMethod->InitHistos();

  return true;
}



void AnnihilationSelection::SortCluster_byEnergy(){

 TRecoVCluster* ecalclu=NULL;
 Int_t NClECal = fECal_ClColl->GetNElements();
 Double_t maxEne=-9999;

 std::vector<IdEnergy> clStruct;
 for (Int_t jecal=0; jecal<NClECal; ++jecal){
   ecalclu          = fECal_ClColl->Element(jecal);
   clStruct.push_back(IdEnergy());
   clStruct.back().clIdPosition = jecal;
   clStruct.back().clEnergy = ecalclu->GetEnergy();
  }

 std::sort(clStruct.begin(), clStruct.end(), by_energy());
 for(int i=0; i< clStruct.size(); i++){
   fIdCl_SortByEnergy.push_back(clStruct[i].clIdPosition);
 }

}



