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
#include <time.h>


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
  fMCscaleF=NULL;
  fscaleFacCalchepPileup=NULL;
  
}
AnnihilationSelection::AnnihilationSelection(Int_t processingMode, Int_t verbosityFlag, Bool_t NoTargetBool, Bool_t dataMCmethod, Bool_t scaleFMethod, Bool_t externalPass)
{
  fRecoEvent      = NULL;
  fVerbose        = verbosityFlag;
  fNoTargetBool   = NoTargetBool;
  fdataMCmethod   = dataMCmethod;
  fInitToComplete =true;
  fDataMCMethod=new AnnihilationSelectionDataMCMethod();
  fScaleFMethod=scaleFMethod;
  fMCscaleF=new ScaleFactorMethod();
  
  fsFacCalchepPileup=false;
  fscaleFacCalchepPileup= new CalchepTruthStudies_ggPileup();

  fexternalPass=externalPass;

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

  fCorrectEfficiencyForTruth=false;
  fPreSelectionAnnihilation=true;

  fcountEvent=0;
  fcountDTime10=0;
  fcountDTime10g1InFR=0;
  fcountDTime10g1InFR5CoGX=0;
  fcountDTime10g1InFR5CoGXY=0;
  fcountDTime10g1InFR5CoGXYEgCut=0;
  fcountDTime10g1InFR5CoGXYEgCutESumCut=0;
  fcountDTime10g1InFR5CoGXYEgCutESumCutg2inFR=0;

    
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
  

  //fillEffVector("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/runsAnalysis/txtFileEff/provaggEfficiencyFromDiff_TagFR_sigmaStudies_30617_FR1158_25800_otherBkgForProblemPoins.txt", "/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/runsAnalysis/txtFileEff/provaggEfficiencyFromDiff_TagAndProbeFR_sigmaStudies_30617_FR1158_25800_otherBkgForProblemPoins.txt");

  fillEffVectorData("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/data/runsAnalysis/txtFileEff/provaggEfficiencyFromDiff_TagFR_sigmaStudies_AllRuns_FR1158_25800_otherBkgForProblemPoins_234sigma.txt", "/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/data/runsAnalysis/txtFileEff/provaggEfficiencyFromDiff_TagAndProbeFR_sigmaStudies_AllRuns_FR1158_25800_otherBkgForProblemPoins_234sigma.txt");
  //fillEffVectorMC("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/runsAnalysis/txtFileEff/provaggEfficiencyFromDiff_TagFR_sigmaStudies_MC_FR1158_25800_otherBkgForProblemPoins.txt", "/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/runsAnalysis/txtFileEff/provaggEfficiencyFromDiff_TagAndProbeFR_sigmaStudies_MC_FR1158_25800_otherBkgForProblemPoins.txt");
  //fillEffVectorMC("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/provaggEfficiencyFromDiff_TagFR_MC_25kPileupAnnihilation_FixRangeFR115.8_250RMid173_noDiedSU.txt", "/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/provaggEfficiencyFromDiff_TagAndProbeFR_MC_25kPileupAnnihilation_FixRange_FR115.8_250RMid173_noDiedSU.txt");
  //fillEffVectorMC("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/provaggEfficiencyFromDiff_TagFR_MC_IdealAnnihilation_FixRangeFR115.8_250RMid173_randomDiedSU.txt", "/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/provaggEfficiencyFromDiff_TagAndProbeFR_MC_IdealAnnihilation_FixRange_FR115.8_250RMid173_randomDiedSU.txt");
  //fillEffVectorMC("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/runsAnalysis/txtFileEff/provaggEfficiencyFromDiff_TagFR_sigmaStudies_MCCalchep25kPileup_FR1158_25800.txt", "/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/runsAnalysis/txtFileEff/provaggEfficiencyFromDiff_TagAndProbeFR_sigmaStudies_MCCalchep25kPileup_FR1158_25800.txt");
  //fillEffVectorMC("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/runsAnalysis/txtFileEff/provaggEfficiencyFromDiff_TagFR_sigmaStudies_MCCalchep25kPileup_randomDied_FR1158_25800_normalisationm300m50.txt", "/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/runsAnalysis/txtFileEff/provaggEfficiencyFromDiff_TagAndProbeFR_sigmaStudies_MCCalchep25kPileup_randomDied_FR1158_25800_normalisationm300m50.txt");
  //fillEffVectorMC("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/runsAnalysis/txtFileEff/provaggEfficiencyFromDiff_TagFR_sigmaStudies_MCCalchep25kPileup_bkgMCNoTargetNoBeamLine_randomDied_FR1158_25800_normalisationm250m100DifOthP.txt", "/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/runsAnalysis/txtFileEff/provaggEfficiencyFromDiff_TagAndProbeFR_sigmaStudies_MCCalchep25kPileup_bkgMCNoTargetNoBeamLine_randomDied_FR1158_25800_normalisationm250m100DifOthP.txt");

  //fillEffVectorMC("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/provaggEfficiencyFromDiff_TagFR_MC_1Annihilation_FixRangeFR115.8_250RMid173_randomDiedSU_DETagAndProbe30.txt", "/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/provaggEfficiencyFromDiff_TagAndProbeFR_MC_1Annihilation_FixRange_FR115.8_250RMid173_randomDiedSU_DETagAndProbe30.txt");
  //fillEffVectorMC("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/provaggEfficiencyFromDiff_TagFR_MC_1Annihilation_FixRangeFR115.8_250RMid173_randomDiedSU_DET30_ProbeEThr90.txt", "/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/provaggEfficiencyFromDiff_TagAndProbeFR_MC_1Annihilation_FixRange_FR115.8_250RMid173_randomDiedSU_DET30_ProbeEThr90.txt");
  //fillEffVectorMC("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/provaggEfficiencyFromDiff_TagFR_MC_1Annihilation_FixRangeFR115.8_250RMid173_dataDiedSU2.txt", "/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/provaggEfficiencyFromDiff_TagAndProbeFR_MC_1Annihilation_FixRange_FR115.8_250RMid173_dataDiedSU2.txt");
fillEffVectorMC("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/data/runsAnalysis/txtFileEff/provaggEfficiencyFromDiff_TagFR_sigmaStudies_MCCalchep25kPileup_bkgMCNoTargetNoBeamLine_3Died_FR1158_25800_normalisationm250m100DifOthP.txt", "/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/data/runsAnalysis/txtFileEff/provaggEfficiencyFromDiff_TagAndProbeFR_sigmaStudies_MCCalchep25kPileup_bkgMCNoTargetNoBeamLine_3Died_FR1158_25800_normalisationm250m100DifOthP.txt");
  
  fillEffVectorTruth();
  fillEffVectorRooFit();
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

    TFile *fAcc_g1g2inFR =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/EffectiveAcceptance/AccEffective_g1g2inFR_struth_calchep430MeV_FR115.8_258.root");
    TFile *fAcc_g1inFR =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/EffectiveAcceptance/AccEffective_g1inFR_struth_calchep430MeV_FR115.8_258.root");
    hAccEffFromCalchep_g1g2FR = (TH1D*)fAcc_g1g2inFR->Get("divide_g1g2inFR");
    hAccEffFromCalchep_g1FR = (TH1D*)fAcc_g1inFR->Get("divide_g1inFR");
  }
  else{
    //this calchep truth + calchep reco
    TFile *fAcc_g1g2inFR =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/EffectiveAcceptance/AccEffective_g1g2inFR_recotruth_calchep450MeV.root");
    TFile *fAcc_g1inFR =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/TagAndProbe/newAnalysis/EffectiveAcceptance/AccEffective_g1inFR_recotruth_calchep450MeV.root"); //most energetic photon in FR
    hAccEffFromCalchep_g1g2FR = (TH1D*)fAcc_g1g2inFR->Get("divide_g1g2inFR");
    hAccEffFromCalchep_g1FR = (TH1D*)fAcc_g1inFR->Get("divide_g1inFR");
  }
  
  if(fdataMCmethod)fDataMCMethod->Init(fMCEffInnerRRange_r1inFR, fMCEffOuterRRange_r1inFR, fMCEffInnerRRange_r1r2inFR, fMCEffOuterRRange_r1r2inFR);
  if(fScaleFMethod)fMCscaleF->Init(fDataEffInnerRRange_r1inFR, fDataEffOuterRRange_r1inFR, fDataEffInnerRRange_r1r2inFR, fDataEffOuterRRange_r1r2inFR, fMCEffInnerRRange_r1inFR, fMCEffOuterRRange_r1inFR, fMCEffInnerRRange_r1r2inFR, fMCEffOuterRRange_r1r2inFR);
  if(fsFacCalchepPileup)fscaleFacCalchepPileup->Init(fDataEffInnerRRange_r1inFR, fDataEffOuterRRange_r1inFR, fDataEffInnerRRange_r1r2inFR, fDataEffOuterRRange_r1r2inFR, fMCEffInnerRRange_r1inFR, fMCEffOuterRRange_r1inFR, fMCEffInnerRRange_r1r2inFR, fMCEffOuterRRange_r1r2inFR);

  fr=new TRandom2();
  gRandom->SetSeed(time(NULL));

  return true;
}


void AnnihilationSelection::selectEffSample(){
  Bool_t myisMC=fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED);
  fillEffVector(myisMC);
}

Bool_t AnnihilationSelection::Process(Bool_t isMC,Bool_t makeClSelection ,std::vector<int> selCl, Bool_t allAnnPlot)
{
  if(fsFacCalchepPileup)fscaleFacCalchepPileup->GetTruthEvent();
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  //std::cout  << "in Annihilation process isMC bool " << isMC << "from reco event "<< fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED) << std::endl;
  if (!passPreselection(fNoTargetBool, isMC)) return true;
  fcountEvent++;  

  Bool_t retCode = 0;
  TRecoVCluster* ecalclu=NULL;
  TRecoVCluster* ecalclu2=NULL;
  Int_t NClECal = fECal_ClColl->GetNElements();

  fIdCl_SortByEnergy.clear();
  SortCluster_byEnergy();
  
  Double_t EBeam=0.;
  if(isMC)EBeam=430.0;
  else EBeam=420.;
  //else EBeam=450.;
  
  if( NClECal != fIdCl_SortByEnergy.size()){
    std::cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!         error on sorted energy        !!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    return true;
  }

  //to study eff of CoG cut: from calchep truth w/o pilup, if nC>1 and r1 in FR + energies> thr -> this should be an annihilation event
  if(NClECal>1){
    ecalclu          = fECal_ClColl->Element(fIdCl_SortByEnergy.at(0));//most energetic->g1
    ecalclu2         = fECal_ClColl->Element(fIdCl_SortByEnergy.at(1));//g2
    Double_t r1=sqrt(ecalclu->GetPosition().X()*ecalclu->GetPosition().X()+ecalclu->GetPosition().Y()*ecalclu->GetPosition().Y());
    if(r1>fFRmin && r1<fFRmax){
      if(allAnnPlot && ecalclu->GetEnergy()>50. && ecalclu2->GetEnergy()>50.){
	 hname="Annihilation_NClAtLeast2g1inFREThr50";
	 hSvc->FillHisto(hname, ecalclu->GetEnergy()+ecalclu2->GetEnergy(), 1.);
      }
    }
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
    //if(fg1t<-110) continue;//high energy spread in data October 2020  
    if(makeClSelection && selCl.at(fIdCl_SortByEnergy.at(jecal))<10)continue;
    fR_1    = sqrt(fg1x*fg1x+ fg1y*fg1y+fdistanceTarget*fdistanceTarget);
    fg1Recal=sqrt(fg1x*fg1x+fg1y*fg1y);
    fPx_1   = fg1E*fg1x/ fR_1;
    fPy_1   = fg1E*fg1y/ fR_1;
    fPz_1   = fg1E*fdistanceTarget/ fR_1;
    TLorentzVector P4g1F, P4eTarget, P4eBeam;
    P4g1F.SetPxPyPzE(fPx_1,fPy_1,fPz_1, fg1E);
    fphig1 =TMath::ATan2(fg1y, fg1x);// P4g1F.Phi();
    //fthetag1 = P4g1F.Theta();
    fthetag1=atan(fg1Recal/fdistanceTarget);
    Double_t me=0.511;
    Double_t PzBeam=sqrt(EBeam*EBeam-me*me);
    P4eTarget.SetPxPyPzE(0,0,0, me);
    P4eBeam.SetPxPyPzE(0,0,PzBeam, EBeam);
    if(fPreSelectionAnnihilation && !passAnnPreSelection(fg1E, fthetag1)) continue;
    Bool_t firstGInFR_def=false;
    if(fg1Recal>fFRmin && fg1Recal<fFRmax )  firstGInFR_def=true;
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
      //if(fg2t<-110) continue;//high energy spread in data October 2020  
      if(makeClSelection && selCl.at(fIdCl_SortByEnergy.at(jecal2))<10)continue;
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
      fphig2 =TMath::ATan2(fg2y, fg2x);// P4g2F.Phi();
      if(fPreSelectionAnnihilation && !passAnnPreSelection(fg2E, fthetag2)) continue;
      Bool_t secondGInFR_def=false;
      if(fg2Recal>fFRmin && fg2Recal<fFRmax)  secondGInFR_def=true;

      if(fabs(fg1t-fg2t)>10.)continue;                      //two clsuter in time 10 ns
      NGGInTheSameEventTime10ns++;
      fcountDTime10++;
      if(allAnnPlot){
	hname="ECAL_gravTwoPhoton10ns";
	hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
      }
      if(fg1E>90.){
	if(allAnnPlot){
	  hname="ECAL_gravTwoPhoton10nsg1EThr";
	  hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	}
      }
      if(fg2E>90.) {
	if(allAnnPlot){
	  hname="ECAL_gravTwoPhoton10nsg2EThr";
	  hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	}
      }
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
	fcountDTime10g1InFR++;
	if(allAnnPlot){
	  hname="ECAL_gravTwoPhoton10nsg1InFR";
	  hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	}
	if(fg1E>90){
	  if(allAnnPlot){
	    hname="ECAL_gravTwoPhoton10nsg1InFRg1Ethr";
	    hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	  }
	}
	if(fg2E>90){
	  if(allAnnPlot){
	    hname="ECAL_gravTwoPhoton10nsg1InFRg2Ethr";
	    hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	  }
	}
	FillGeneralHisto("_g1inFRDTime10");
	FillCoGAsAFunctionOfPhi("_g1inFRDTime10");
	FillWeightedHisto_R1inFR("_g1inFRDTime10");
	FillWeightedHistoRooFit_R1inFR("_g1inFRDTime10");

	if(allAnnPlot) if(secondGInFR_def)FillGeneralHisto("_g1g2inFRDTime10");
	if(allAnnPlot && secondGInFR_def){
	  FillWeightedHisto_R1R2inFR("_g1g2inFRDTime10");
	  FillWeightedHisto_R1R2inFR2approach("_g1g2inFRDTime10");
	}
	if(allAnnPlot) FillAccEffHisto_R1inFR("_g1inFRDTime10");
	if(allAnnPlot) FillWeightedAccEffHisto_R1inFR("_g1inFRDTime10");
	if(allAnnPlot & secondGInFR_def)FillAccEffHisto_R1R2inFR("_g1g2inFRDTime10");
	if(allAnnPlot & secondGInFR_def)FillWeightedAccEffHisto_R1R2inFR("_g1g2inFRDTime10");

	if(allAnnPlot && dPhiIn20Degree){
	  sufix="_g1inFRin20Deg";
	  FillGeneralHisto(sufix);
	  if(InThetaAngle) {
	    if(TotEnergyCut){FillGeneralHisto("_g1inFRin20DegThetaCutUnderPeak");}
	    if(InEnergy){FillGeneralHisto("_g1inFRin20DegThetaCutEThr");}
	    if(TotEnergyCut && InEnergy ){FillGeneralHisto("_g1inFRin20DegThetaCutUnderPeakEThr");}
	  }      
	}
	
	if(allAnnPlot && secondGInFR_def && dPhiIn20Degree && InThetaAngle){
	  FillGeneralHisto("_g1g2inFRin20DegThetaCut");
	  if(TotEnergyCut && InEnergy )FillGeneralHisto("_g1g2inFRin20DegThetaCutUnderPeakEThr");
	}
  
	if(allAnnPlot){
	  if(fabs(fXWeighted)<40. && fabs(fYWeighted)<40.){
	    hname="ECAL_gravTwoPhoton10ns_g1inFRin4Cog";
	    hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	  }
	  if(fabs(fXWeighted)<42. && fabs(fYWeighted)<42.){
	    hname="ECAL_gravTwoPhoton10ns_g1inFRin4_2Cog";
	    hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	  }
	  if(fabs(fXWeighted)<44. && fabs(fYWeighted)<44.){
	    hname="ECAL_gravTwoPhoton10ns_g1inFRin4_4Cog";
	    hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	  }
	  if(fabs(fXWeighted)<46. && fabs(fYWeighted)<46.){
	    hname="ECAL_gravTwoPhoton10ns_g1inFRin4_6Cog";
	    hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	  }
	  if(fabs(fXWeighted)<48. && fabs(fYWeighted)<48.){
	    hname="ECAL_gravTwoPhoton10ns_g1inFRin4_8Cog";
	    hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	  }
	  if(fabs(fXWeighted)<52. && fabs(fYWeighted)<52.){
	    hname="ECAL_gravTwoPhoton10ns_g1inFRin5_2Cog";
	    hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	  }
	  if(fabs(fXWeighted)<54. && fabs(fYWeighted)<54.){
	    hname="ECAL_gravTwoPhoton10ns_g1inFRin5_4Cog";
	    hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	  }
	  if(fabs(fXWeighted)<56. && fabs(fYWeighted)<56.){
	    hname="ECAL_gravTwoPhoton10ns_g1inFRin5_6Cog";
	    hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	  }
	  if(fabs(fXWeighted)<58. && fabs(fYWeighted)<58.){
	    hname="ECAL_gravTwoPhoton10ns_g1inFRin5_8Cog";
	    hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	  } 
	  if(fabs(fXWeighted)<60. && fabs(fYWeighted)<60.){
	    hname="ECAL_gravTwoPhoton10ns_g1inFRin6Cog";
	    hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	  }
	}

	if(fabs(fXWeighted)<50.){
	  fcountDTime10g1InFR5CoGX++;
	  if( fabs(fYWeighted)<50.){
	    fcountDTime10g1InFR5CoGXY++;
	    if(InEnergy){
	      fcountDTime10g1InFR5CoGXYEgCut++;
	      if(TotEnergyCut){
		fcountDTime10g1InFR5CoGXYEgCutESumCut++;
		if(secondGInFR_def)fcountDTime10g1InFR5CoGXYEgCutESumCutg2inFR++;
	      }
	    }
	  }
	}

	if(fabs(fXWeighted)<50. && fabs(fYWeighted)<50.){
	  if(allAnnPlot){
	    hname="ECAL_gravTwoPhoton10nsg1InFR5CoG";
	    hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	  }
	  if(fg1E>90){
	    if(allAnnPlot){
	      hname="ECAL_gravTwoPhoton10nsg1InFR5CoGg1EThr";
	      hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	    }
	  }
	  if(fg2E>90){
	    if(allAnnPlot){
	      hname="ECAL_gravTwoPhoton10nsg1InFR5CoGg2EThr";
	      hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	    }
	  }
	  if(secondGInFR_def){
	    if(allAnnPlot){
	      hname="ECAL_gravTwoPhoton10nsg1g2InFR5CoG";
	      hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	    }
	    if(fg1E>90){
	      if(allAnnPlot){
		hname="ECAL_gravTwoPhoton10nsg1g2InFR5CoGg1EThr";
		hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	      }
	    }
	    if(fg2E>90){
	      if(allAnnPlot){
		hname="ECAL_gravTwoPhoton10nsg1g2InFR5CoGg2EThr";
		hSvc->FillHisto(hname,(fg1E+fg2E) , 1.);
	      }
	    }
	  }
	  if(fdataMCmethod)fDataMCMethod->Process(fg1E, fphig1, fg1Recal , fg2E , fphig2, fg2Recal);
	  if(fScaleFMethod)fMCscaleF->Process(fg1E, fphig1, fg1Recal , fg2E , fphig2, fg2Recal);
	  if(fsFacCalchepPileup)fscaleFacCalchepPileup->GetReco(fg1E, fphig1, fg1Recal , fg2E , fphig2, fg2Recal);

	  Int_t countHit1_5CoG=0;
	  Int_t countHit2_5CoG=0;
	  CheckNumberHitsOnCh(g1seedChId, g2seedChId, countHit1_5CoG, countHit2_5CoG);
	  hname="ECAL_nHitOnChannel_5CoG";
	  hSvc->FillHisto(hname, countHit1_5CoG, 1.);
	  hSvc->FillHisto(hname, countHit2_5CoG, 1.);
	  if(allAnnPlot){
	    FillGeneralHisto("_g1inFRin5Cog");
	    FillWeightedHisto_R1inFR("_g1inFRin5Cog");
	    FillWeightedHistoRooFit_R1inFR("_g1inFRin5Cog");
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
	    
	    if(fabs(fXWeighted)<50. && fabs(fYWeighted)<50. && secondGInFR_def ){
	      FillGeneralHisto("_g1g2inFRin5Cog");
	      if(TotEnergyCut && InEnergy ){FillGeneralHisto("_g1g2inFRin5CogUnderPeakEThr");}
	    }
	  
	    //second photon in FR
	    if(secondGInFR_def){
	      if(fabs(fXWeighted)<50. && fabs(fYWeighted)<50.){
		FillGeneralHisto("_g2inFRin5Cog");
		FillWeightedHisto_R2inFR("_g2inFRin5Cog");
		if(TotEnergyCut && InEnergy ){FillGeneralHisto("_g2inFRin5CogUnderPeakEThr");}
	      }
	    }
	  }
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

void AnnihilationSelection::FillWeightedHistoRooFit_R1inFR(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Double_t effg1=ReturnEfficiencyRooFitR1inFR(fg1Recal,fphig1);
  Double_t effg2=ReturnEfficiencyRooFitR1inFR(fg2Recal,fphig2);
  Double_t effScaleFactor=(1./effg1)*(1./effg2);
  std::string sufix2;
  sufix2=sufix+"_RooFit";
  FillWeightedHistoAsAFunctionOfPhi(effScaleFactor,sufix2);

  effg1=ReturnEfficiencyRooFitEThr90R1inFR(fg1Recal,fphig1);
  effg2=ReturnEfficiencyRooFitEThr90R1inFR(fg2Recal,fphig2);
  effScaleFactor=(1./effg1)*(1./effg2);
  //sufix2=sufix+"_RooFitEThr";
  //FillWeightedHistoAsAFunctionOfPhi(effScaleFactor,sufix2);

}


void AnnihilationSelection::FillWeightedHisto_R1inFR(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  //PlusSystematic=0 -> eff w/o sys; =1 with upper sys; =-1 with lower sys; =2 random upper sys; =-2 random lower sys; =3 apply the statistic error
  Int_t plusSystematic=0;
  //Double_t effg1=ReturnEfficiencyR1R2inFR(fg1Recal,fphig1,plusSystematic);
  Double_t effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  Double_t effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  Double_t effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffR1inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
  std::string sufix2;
  sufix2=sufix+"_NoWeight";
  FillWeightedHistoAsAFunctionOfPhi(1.,sufix2);
  sufix2=sufix+"";
  FillWeightedHistoAsAFunctionOfPhi(effScaleFactor,sufix2);

  Double_t ipoteticalEffEvent=1./ReturnIpoteticalEventEfficiencyR1inFR(fphig1);
  hname="ECAL_gravTwoPhoton10ns_WIpotEvEffR1inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,ipoteticalEffEvent);


  plusSystematic=1;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffPlusSysR1inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );

  plusSystematic=-1;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffPlusLowSysR1inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );

  plusSystematic=2;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffPlusSysRandomR1inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );

  plusSystematic=-2;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffPlusLowSysRandomR1inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );

  //now i'll weight the event with a random estraction but if the first is weighted with the lowerSys, the secon one should be weighted with a random in the opposite side (UpSys)
  //let's start with the first weighted with upSys
  plusSystematic=2;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  plusSystematic=-2;
  effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffPlusUpSysFirstLowSysSecondRandomR1inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );

  plusSystematic=-2;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  plusSystematic=2;
  effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffPlusLowSysFirstUpSysSecondRandomR1inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );

  //now i'll weight the event w/o a random estraction but if the first is weighted with the lowerSys, the secon one should be weighted with a random in the opposite side (UpSys)
  //let's start with the first weighted with upSys
  plusSystematic=1;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  plusSystematic=-1;
  effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffPlusUpSysFirstLowSysSecondR1inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );

  plusSystematic=-1;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  plusSystematic=1;
  effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffPlusLowSysFirstUpSysSecondR1inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );


  plusSystematic=3;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffPlusStatisticErrorR1inFR"+sufix;
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
  Int_t  plusSystematic=0;
  //Double_t effg1=ReturnEfficiencyR1R2inFR(fg1Recal,fphig1,plusSystematic);
  Double_t effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  Double_t effg2=ReturnEfficiencyR1R2inFR(fg2Recal,fphig2,plusSystematic);
  Double_t effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
  
  plusSystematic=1;
  //effg1=ReturnEfficiencyR1R2inFR(fg1Recal,fphig1,plusSystematic);
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  effg2=ReturnEfficiencyR1R2inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffPlusSysR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );

  plusSystematic=-1;
  //effg1=ReturnEfficiencyR1R2inFR(fg1Recal,fphig1,plusSystematic);
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  effg2=ReturnEfficiencyR1R2inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffPlusLowSysR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );

  plusSystematic=2;
  //effg1=ReturnEfficiencyR1R2inFR(fg1Recal,fphig1,plusSystematic);
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  effg2=ReturnEfficiencyR1R2inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffPlusSysRandomR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );

  plusSystematic=-2;
  //effg1=ReturnEfficiencyR1R2inFR(fg1Recal,fphig1,plusSystematic);
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  effg2=ReturnEfficiencyR1R2inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffPlusLowSysRandomR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );

  //now i'll weight the event with a random estraction but if the first is weighted with the lowerSys, the secon one should be weighted with a random in the opposite side (UpSys)
  //let's start with the first weighted with upSys
  plusSystematic=2;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  plusSystematic=-2;
  effg2=ReturnEfficiencyR1R2inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffPlusUpSysFirstLowSysSecondRandomR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );

  plusSystematic=-2;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  plusSystematic=2;
  effg2=ReturnEfficiencyR1R2inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffPlusLowSysFirstUpSysSecondRandomR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );

  //now i'll weight the event w/o a random estraction but if the first is weighted with the lowerSys, the secon one should be weighted with a random in the opposite side (UpSys)
  //let's start with the first weighted with upSys
  plusSystematic=1;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  plusSystematic=-1;
  effg2=ReturnEfficiencyR1R2inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffPlusUpSysFirstLowSysSecondR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );

  plusSystematic=-1;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  plusSystematic=1;
  effg2=ReturnEfficiencyR1R2inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffPlusLowSysFirstUpSysSecondR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );


  plusSystematic=3;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  effg2=ReturnEfficiencyR1R2inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2);
  hname="ECAL_gravTwoPhoton10ns_WEffPlusStatisticErrorR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
}


void AnnihilationSelection::FillWeightedHisto_R1R2inFR2approach(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Int_t  plusSystematic=0;
  //Double_t effg1=ReturnEfficiencyR1R2inFR(fg1Recal,fphig1,plusSystematic);
  Double_t effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  Double_t effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  Double_t Acc=ReturnAccettanceEffective_g1g2FR(fg1Recal);
  Double_t effScaleFactor=(1./effg1)*(1./effg2)*(1./Acc);
  hname="ECAL_gravTwoPhoton10ns_W2ndApproachR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );


  plusSystematic=1;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2)*(1./Acc);
  hname="ECAL_gravTwoPhoton10ns_W2ndApproachPlusSysR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
  plusSystematic=-1;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2)*(1./Acc);
  hname="ECAL_gravTwoPhoton10ns_W2ndApproachPlusLowSysR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
  plusSystematic=2;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2)*(1./Acc);
  hname="ECAL_gravTwoPhoton10ns_W2ndApproachPlusSysRandomR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
  plusSystematic=-2;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2)*(1./Acc);
  hname="ECAL_gravTwoPhoton10ns_W2ndApproachPlusLowSysRandomR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );

  //now i'll weight the event with a random estraction but if the first is weighted with the lowerSys, the secon one should be weighted with a random in the opposite side (UpSys)
  //let's start with the first weighted with upSys
  plusSystematic=2;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  plusSystematic=-2;
  effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2)*(1./Acc);
  hname="ECAL_gravTwoPhoton10ns_W2ndApproachPlusUpSysFirstLowSysSecondRandomR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );

  plusSystematic=-2;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  plusSystematic=2;
  effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2)*(1./Acc);
  hname="ECAL_gravTwoPhoton10ns_W2ndApproachPlusLowSysFirstUpSysSecondRandomR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );

  //now i'll weight the event w/o random extraction but if the first is weighted with the lowerSys, the secon one should be weighted with a random in the opposite side (UpSys)
  //let's start with the first weighted with upSys
  plusSystematic=1;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  plusSystematic=-1;
  effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2)*(1./Acc);
  hname="ECAL_gravTwoPhoton10ns_W2ndApproachPlusUpSysFirstLowSysSecondR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );

  plusSystematic=-1;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  plusSystematic=1;
  effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2)*(1./Acc);
  hname="ECAL_gravTwoPhoton10ns_W2ndApproachPlusLowSysFirstUpSysSecondR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );

  plusSystematic=3;
  effg1=ReturnEfficiencyR1inFR(fg1Recal,fphig1,plusSystematic);
  effg2=ReturnEfficiencyR1inFR(fg2Recal,fphig2,plusSystematic);
  effScaleFactor=(1./effg1)*(1./effg2)*(1./Acc);
  hname="ECAL_gravTwoPhoton10ns_W2ndApproachPlusStatisticErrorR1R2inFR"+sufix;
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
  hname="ECAL_gravTwoPhoton10nsTrend"+sufix;
  hSvc->FillHisto2(hname,fg1t, (fg1E+fg2E) , 1.);
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
  hname="ECAL_EnergyvsTheta"+sufix;
  hSvc->FillHisto2(hname, fthetag1,fg1E);
  hSvc->FillHisto2(hname, fthetag2,fg2E);
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

  
void AnnihilationSelection::FillWeightedHistoAsAFunctionOfPhi(Double_t effScaleFactor,std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Double_t phiDeg=fphig1*360./(2*TMath::Pi());
  if(phiDeg<0.)phiDeg=360+phiDeg;

  Double_t phiDeg2=fphig2*360./(2*TMath::Pi());
  if(phiDeg2<0.)phiDeg2=360+phiDeg2;
  Double_t phi1Pred=phiDeg+180;
  if(phi1Pred>360.) phi1Pred=phi1Pred-360;
  Double_t dphi=phi1Pred-phiDeg2;

  if(fabs(dphi)>180.)dphi=TMath::Sign(1., dphi)*(360-fabs(dphi)); //IO 11/01/2022

  hname="ECAL_gravTwoPhoton10ns_WEffR1inFR_total"+sufix;     hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
  hname="ECAL_gravTwoPhoton10nsDPhi_WEffR1inFR_total"+sufix;     hSvc->FillHisto(hname,dphi ,effScaleFactor );
  hname="ECAL_gravTwoPhoton10nsLargeDPhi_WEffR1inFR_total"+sufix;     hSvc->FillHisto(hname,dphi ,effScaleFactor );
  if(phiDeg>=0. && phiDeg<45.)    { hname="ECAL_gravTwoPhoton10ns_WEffR1inFR_g1in0_45"+sufix;     hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor ); 
    hname="ECAL_gravTwoPhoton10nsDPhi_WEffR1inFR_g1in0_45"+sufix;     hSvc->FillHisto(hname,dphi ,effScaleFactor );}
  else if(phiDeg>=45. && phiDeg<90.)   { hname="ECAL_gravTwoPhoton10ns_WEffR1inFR_g1in45_90"+sufix;    hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
    hname="ECAL_gravTwoPhoton10nsDPhi_WEffR1inFR_g1in45_90"+sufix;     hSvc->FillHisto(hname, dphi ,effScaleFactor );}
  else if(phiDeg>=90. && phiDeg<135.)  { hname="ECAL_gravTwoPhoton10ns_WEffR1inFR_g1in90_135"+sufix;   hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
    hname="ECAL_gravTwoPhoton10nsDPhi_WEffR1inFR_g1in90_135"+sufix;   hSvc->FillHisto(hname, dphi  ,effScaleFactor );}
  else if(phiDeg>=135. && phiDeg<180.) { hname="ECAL_gravTwoPhoton10ns_WEffR1inFR_g1in135_180"+sufix;  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
    hname="ECAL_gravTwoPhoton10nsDPhi_WEffR1inFR_g1in135_180"+sufix;  hSvc->FillHisto(hname, dphi ,effScaleFactor );}
  else if(phiDeg>=180. && phiDeg<225.) { hname="ECAL_gravTwoPhoton10ns_WEffR1inFR_g1in180_225"+sufix;  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
    hname="ECAL_gravTwoPhoton10nsDPhi_WEffR1inFR_g1in180_225"+sufix;  hSvc->FillHisto(hname,dphi ,effScaleFactor );}
  else if(phiDeg>=225. && phiDeg<270.) { hname="ECAL_gravTwoPhoton10ns_WEffR1inFR_g1in225_270"+sufix;  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
    hname="ECAL_gravTwoPhoton10nsDPhi_WEffR1inFR_g1in225_270"+sufix;  hSvc->FillHisto(hname,dphi ,effScaleFactor );}
  else if(phiDeg>=270. && phiDeg<315.) { hname="ECAL_gravTwoPhoton10ns_WEffR1inFR_g1in270_315"+sufix;  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
    hname="ECAL_gravTwoPhoton10nsDPhi_WEffR1inFR_g1in270_315"+sufix;  hSvc->FillHisto(hname, dphi  ,effScaleFactor );}
  else if(phiDeg>=315. && phiDeg<360.) { hname="ECAL_gravTwoPhoton10ns_WEffR1inFR_g1in315_360"+sufix;  hSvc->FillHisto(hname,(fg1E+fg2E) ,effScaleFactor );
    hname="ECAL_gravTwoPhoton10nsDPhi_WEffR1inFR_g1in315_360"+sufix;  hSvc->FillHisto(hname,dphi ,effScaleFactor );}
}


void AnnihilationSelection::FillCoGAsAFunctionOfPhi(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Double_t phiDeg=fphig1*360./(2*TMath::Pi());
  if(phiDeg<0.)phiDeg=360+phiDeg;
  //fXWeighted
  
  hname="ECAL_gravTwoPhoton10ns_XCoG_total"+sufix;     hSvc->FillHisto(hname,fXWeighted  );
  hname="ECAL_gravTwoPhoton10ns_YCoG_total"+sufix;     hSvc->FillHisto(hname,fYWeighted  );
  hname="ECAL_gravTwoPhoton10ns_CoGmap_total"+sufix;     hSvc->FillHisto2(hname,fXWeighted,fYWeighted  );
  if(phiDeg>=0. && phiDeg<45.){ 
    hname="ECAL_gravTwoPhoton10ns_XCoG_g1in0_45"+sufix;     hSvc->FillHisto(hname,fXWeighted  ); 
    hname="ECAL_gravTwoPhoton10ns_YCoG_g1in0_45"+sufix;     hSvc->FillHisto(hname,fYWeighted  ); 
    hname="ECAL_gravTwoPhoton10ns_CoGmap_g1in0_45"+sufix;     hSvc->FillHisto2(hname,fXWeighted,fYWeighted  );
  }
  else if(phiDeg>=45. && phiDeg<90.){ 
    hname="ECAL_gravTwoPhoton10ns_XCoG_g1in45_90"+sufix;    hSvc->FillHisto(hname,fXWeighted  );
    hname="ECAL_gravTwoPhoton10ns_YCoG_g1in45_90"+sufix;    hSvc->FillHisto(hname,fYWeighted  );
    hname="ECAL_gravTwoPhoton10ns_CoGmap_g1in45_90"+sufix;     hSvc->FillHisto2(hname, fXWeighted,fYWeighted  );
  }
  else if(phiDeg>=90. && phiDeg<135.){ 
    hname="ECAL_gravTwoPhoton10ns_XCoG_g1in90_135"+sufix;   hSvc->FillHisto(hname,fXWeighted  );
    hname="ECAL_gravTwoPhoton10ns_YCoG_g1in90_135"+sufix;   hSvc->FillHisto(hname,fYWeighted  );
    hname="ECAL_gravTwoPhoton10ns_CoGmap_g1in90_135"+sufix;   hSvc->FillHisto2(hname, fXWeighted,fYWeighted   );
  }
  else if(phiDeg>=135. && phiDeg<180.){ 
    hname="ECAL_gravTwoPhoton10ns_XCoG_g1in135_180"+sufix;  hSvc->FillHisto(hname,fXWeighted  );
    hname="ECAL_gravTwoPhoton10ns_YCoG_g1in135_180"+sufix;  hSvc->FillHisto(hname,fYWeighted  );
    hname="ECAL_gravTwoPhoton10ns_CoGmap_g1in135_180"+sufix;  hSvc->FillHisto2(hname, fXWeighted,fYWeighted  );
  }
  else if(phiDeg>=180. && phiDeg<225.){ 
    hname="ECAL_gravTwoPhoton10ns_XCoG_g1in180_225"+sufix;  hSvc->FillHisto(hname,fXWeighted  );
    hname="ECAL_gravTwoPhoton10ns_YCoG_g1in180_225"+sufix;  hSvc->FillHisto(hname,fYWeighted  );
    hname="ECAL_gravTwoPhoton10ns_CoGmap_g1in180_225"+sufix;  hSvc->FillHisto2(hname,fXWeighted,fYWeighted  );
  }
  else if(phiDeg>=225. && phiDeg<270.){ 
    hname="ECAL_gravTwoPhoton10ns_XCoG_g1in225_270"+sufix;  hSvc->FillHisto(hname,fXWeighted  );
    hname="ECAL_gravTwoPhoton10ns_YCoG_g1in225_270"+sufix;  hSvc->FillHisto(hname,fYWeighted  );
    hname="ECAL_gravTwoPhoton10ns_CoGmap_g1in225_270"+sufix;  hSvc->FillHisto2(hname,fXWeighted,fYWeighted  );
  }
  else if(phiDeg>=270. && phiDeg<315.){ 
    hname="ECAL_gravTwoPhoton10ns_XCoG_g1in270_315"+sufix;  hSvc->FillHisto(hname,fXWeighted  );
    hname="ECAL_gravTwoPhoton10ns_YCoG_g1in270_315"+sufix;  hSvc->FillHisto(hname,fYWeighted  );
    hname="ECAL_gravTwoPhoton10ns_CoGmap_g1in270_315"+sufix;  hSvc->FillHisto2(hname, fXWeighted,fYWeighted   );
  }
  else if(phiDeg>=315. && phiDeg<360.){ 
    hname="ECAL_gravTwoPhoton10ns_XCoG_g1in315_360"+sufix;  hSvc->FillHisto(hname,fXWeighted  );
    hname="ECAL_gravTwoPhoton10ns_YCoG_g1in315_360"+sufix;  hSvc->FillHisto(hname,fYWeighted  );
    hname="ECAL_gravTwoPhoton10ns_CoGmap_g1in315_360"+sufix;  hSvc->FillHisto2(hname,fXWeighted,fYWeighted  );
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

  if (!fexternalPass && !isMC && !isTargetOut && fTarget_RecoBeam->getnPOT()<13000) return passed;
  //std::cout<<"tagetnPOT "<<fTarget_RecoBeam->getnPOT() << " and isTargetOutBool " << isTargetOut<< std::endl;
  if (isTargetOut && fSAC_ClColl->GetNElements()>15) return passed;  
  //if(fRecoEvent->GetEventNumber()>270000) return passed;

  std::string hname1 = "AnnMet_NposInBunch_beam";
  hSvc->FillHisto(hname1,fTarget_RecoBeam->getnPOT()*targetConst);
  hname = "AnnMet_nPOT";
  hSvc->FillHisto(hname,0,float(fTarget_RecoBeam->getnPOT()*targetConst)); 
  passed = true;
  return passed;
}

Double_t AnnihilationSelection::ReturnEfficiencyR1inFR(Double_t radius, Double_t phi, Int_t PlusSystematic)
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

  //PlusSystematic=0 -> eff w/o sys; =1 with upper sys; =-1 with lower sys; =2 random upper sys; =-2 random lower sys
  if(PlusSystematic==0){
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

  if(PlusSystematic==1){
    if(inner)eff=fEffUpperSysInnerRRange_r1inFR[phiSlice];
    else if(outer)eff=fEffUpperSysOuterRRange_r1inFR[phiSlice];
  }
  if(PlusSystematic==-1){
    if(inner)eff=fEffLowerSysInnerRRange_r1inFR[phiSlice];
    else if(outer)eff=fEffLowerSysOuterRRange_r1inFR[phiSlice];
  }
  if(PlusSystematic==2){
    if(inner){
      Double_t sigma=fEffUpperSysInnerRRange_r1inFR[phiSlice]-fEffInnerRRange_r1inFR[phiSlice];
      Double_t randomSys=fabs(fr->Gaus(0., sigma));
      eff=fEffInnerRRange_r1inFR[phiSlice]+randomSys;
    }
    else if(outer){
      eff=fEffUpperSysOuterRRange_r1inFR[phiSlice];
      Double_t sigma=fEffUpperSysOuterRRange_r1inFR[phiSlice]-fEffOuterRRange_r1inFR[phiSlice];
      Double_t randomSys=fabs(fr->Gaus(0., sigma));
      eff=fEffOuterRRange_r1inFR[phiSlice]+randomSys;
    }
  }
  if(PlusSystematic==-2){
    if(inner){
      Double_t sigma=fEffInnerRRange_r1inFR[phiSlice]-fEffLowerSysInnerRRange_r1inFR[phiSlice];
      //std::cout<<"eff " << fEffInnerRRange_r1inFR[phiSlice] << " effLow " << fEffLowerSysInnerRRange_r1inFR[phiSlice] << std::endl;
      Double_t randomSys=fabs(fr->Gaus(0., sigma));
      //std::cout<<"sigma " << sigma << " randomSys "<<randomSys << " eff " << fEffInnerRRange_r1inFR[phiSlice]-randomSys << std::endl;
      eff=fEffInnerRRange_r1inFR[phiSlice]-randomSys;
    }
    else if(outer){
      Double_t sigma=fEffOuterRRange_r1inFR[phiSlice]-fEffLowerSysOuterRRange_r1inFR[phiSlice];
      Double_t randomSys=fabs(fr->Gaus(0., sigma));
      //std::cout<<"sigma " << sigma << " randomSys "<<randomSys  << " eff " << fEffOuterRRange_r1inFR[phiSlice]-randomSys << std::endl;
      eff=fEffOuterRRange_r1inFR[phiSlice]-randomSys;
    }
  }


  if(PlusSystematic==3){
    if(inner){
      Double_t sigma=ferrEffInnerRRange_r1inFR[phiSlice];
      Double_t randomSys=fr->Gaus(0., sigma);
      eff=fEffInnerRRange_r1inFR[phiSlice]+randomSys;
    }
    else if(outer){
      Double_t sigma=ferrEffOuterRRange_r1inFR[phiSlice];
      Double_t randomSys=fr->Gaus(0., sigma);
      eff=fEffOuterRRange_r1inFR[phiSlice]+randomSys;
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

Double_t AnnihilationSelection::ReturnEfficiencyRooFitR1inFR(Double_t radius, Double_t phi)
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
    eff=fEffInnerRooFit_r1inFR[phiSlice];
  }
  else if(outer){
    eff=fEffOuterRooFit_r1inFR[phiSlice];
  }
  return eff;
}

Double_t AnnihilationSelection::ReturnEfficiencyRooFitEThr90R1inFR(Double_t radius, Double_t phi)
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
    eff=fEffInnerRooFitEThr90_r1inFR[phiSlice];
  }
  else if(outer){
    eff=fEffOuterRooFitEThr90_r1inFR[phiSlice];
  }
  return eff;
}


Double_t AnnihilationSelection::ReturnEfficiencyR1R2inFR(Double_t radius, Double_t phi, Int_t PlusSystematic)
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
  //PlusSystematic=0 -> eff w/o sys; =1 with upper sys; =-1 with lower sys; =2 random upper sys; =-2 random lower sys
  if(PlusSystematic==0){
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
  if(PlusSystematic==1){
    if(inner)eff=fEffUpperSysInnerRRange_r1r2inFR[phiSlice];
    else if(outer)eff=fEffUpperSysOuterRRange_r1r2inFR[phiSlice];
  }
  if(PlusSystematic==-1){
    if(inner)eff=fEffLowerSysInnerRRange_r1r2inFR[phiSlice];
    else if(outer)eff=fEffLowerSysOuterRRange_r1r2inFR[phiSlice];
  }
  if(PlusSystematic==2){
    if(inner){
      Double_t sigma=fEffUpperSysInnerRRange_r1r2inFR[phiSlice]-fEffInnerRRange_r1r2inFR[phiSlice];
      Double_t randomSys=fabs(fr->Gaus(0., sigma));
      eff=fEffInnerRRange_r1r2inFR[phiSlice]+randomSys;
    }
    else if(outer){
      Double_t sigma=fEffUpperSysOuterRRange_r1r2inFR[phiSlice]-fEffOuterRRange_r1r2inFR[phiSlice];
      Double_t randomSys=fabs(fr->Gaus(0., sigma));
      eff=fEffOuterRRange_r1r2inFR[phiSlice]+randomSys;
    }
  }
  if(PlusSystematic==-2){
    if(inner){
      Double_t sigma=fEffInnerRRange_r1r2inFR[phiSlice]-fEffLowerSysInnerRRange_r1r2inFR[phiSlice];
      Double_t randomSys=fabs(fr->Gaus(0., sigma));
      eff=fEffInnerRRange_r1r2inFR[phiSlice]-randomSys;
    }
    else if(outer){
      Double_t sigma=fEffOuterRRange_r1r2inFR[phiSlice]-fEffLowerSysOuterRRange_r1r2inFR[phiSlice];
      Double_t randomSys=fabs(fr->Gaus(0., sigma));
      eff=fEffOuterRRange_r1r2inFR[phiSlice]-randomSys;
    }
  }


  if(PlusSystematic==3){
    if(inner){
      Double_t sigma=ferrEffInnerRRange_r1r2inFR[phiSlice];
      Double_t randomSys=fr->Gaus(0., sigma);
      eff=fEffInnerRRange_r1r2inFR[phiSlice]+randomSys;
    }
    else if(outer){
      Double_t sigma=ferrEffOuterRRange_r1r2inFR[phiSlice];
      Double_t randomSys=fr->Gaus(0., sigma);
      eff=fEffOuterRRange_r1r2inFR[phiSlice]+randomSys;
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


Double_t AnnihilationSelection::ReturnIpoteticalEventEfficiencyR1inFR(Double_t phi)
{
  Double_t phiDeg=phi*360./(2*TMath::Pi());
  if(phiDeg<0.)phiDeg=360+phiDeg;
  int phiSlice=phiDeg/45.;
  Double_t effEv[8]={0.640, 0.495, 0.465, 0.627, 0.868, 0.703, 0.623, 0.661};
  return effEv[phiSlice];
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


void AnnihilationSelection::fillEffVectorData(std::string fnameTagFR, std::string fnameTagProbeFR){

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
	        fDataEffOuterRRange_r1inFR[0]>>fDataEffInnerRRange_r1inFR[0]>>fDataEffOuterRRange_r1inFR[1]>>fDataEffInnerRRange_r1inFR[1]>>fDataEffOuterRRange_r1inFR[2]>>fDataEffInnerRRange_r1inFR[2]>>fDataEffOuterRRange_r1inFR[3]>>fDataEffInnerRRange_r1inFR[3]>>fDataEffOuterRRange_r1inFR[4]>>fDataEffInnerRRange_r1inFR[4]>>fDataEffOuterRRange_r1inFR[5]>>fDataEffInnerRRange_r1inFR[5]>>fDataEffOuterRRange_r1inFR[6]>>fDataEffInnerRRange_r1inFR[6]>>fDataEffOuterRRange_r1inFR[7]>>fDataEffInnerRRange_r1inFR[7]>>
	   tmp >>tmp >>tmp >>tmp >>tmp >>tmp )std::cout<<"outer range tag fr "<<fDataEffOuterRRange_r1inFR[0]<< std::endl;
	std::getline( tagfr, line);
	std::istringstream stat(line);
	if(stat>>tmp>>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >> 
	        tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >> 
	        ferrDataEffOuterRRange_r1inFR[0]>>ferrDataEffInnerRRange_r1inFR[0]>>ferrDataEffOuterRRange_r1inFR[1]>>ferrDataEffInnerRRange_r1inFR[1]>>ferrDataEffOuterRRange_r1inFR[2]>>ferrDataEffInnerRRange_r1inFR[2]>>ferrDataEffOuterRRange_r1inFR[3]>>ferrDataEffInnerRRange_r1inFR[3]>>ferrDataEffOuterRRange_r1inFR[4]>>ferrDataEffInnerRRange_r1inFR[4]>>ferrDataEffOuterRRange_r1inFR[5]>>ferrDataEffInnerRRange_r1inFR[5]>>ferrDataEffOuterRRange_r1inFR[6]>>ferrDataEffInnerRRange_r1inFR[6]>>ferrDataEffOuterRRange_r1inFR[7]>>ferrDataEffInnerRRange_r1inFR[7]>>
	   tmp >>tmp >>tmp >>tmp >>tmp >>tmp )std::cout<<"outer range tag fr err "<<ferrDataEffOuterRRange_r1inFR[0]<< std::endl;
      }
      if(line.find("errEffSysUpper")!=std::string::npos){
      	//std::getline( tagfr, line);
	std::istringstream effSysSup(line);
	if(effSysSup>>tmp>>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >> 
	        tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>
	        fDataEffUpperSysOuterRRange_r1inFR[0]>>fDataEffUpperSysInnerRRange_r1inFR[0]>>fDataEffUpperSysOuterRRange_r1inFR[1]>>fDataEffUpperSysInnerRRange_r1inFR[1]>>fDataEffUpperSysOuterRRange_r1inFR[2]>>fDataEffUpperSysInnerRRange_r1inFR[2]>>fDataEffUpperSysOuterRRange_r1inFR[3]>>fDataEffUpperSysInnerRRange_r1inFR[3]>>fDataEffUpperSysOuterRRange_r1inFR[4]>>fDataEffUpperSysInnerRRange_r1inFR[4]>>fDataEffUpperSysOuterRRange_r1inFR[5]>>fDataEffUpperSysInnerRRange_r1inFR[5]>>fDataEffUpperSysOuterRRange_r1inFR[6]>>fDataEffUpperSysInnerRRange_r1inFR[6]>>fDataEffUpperSysOuterRRange_r1inFR[7]>>fDataEffUpperSysInnerRRange_r1inFR[7]>>
	   tmp >>tmp >>tmp >>tmp >>tmp >>tmp )std::cout<<"outer range tag fr sys "<<fDataEffUpperSysOuterRRange_r1inFR[0]<< std::endl;
      }
      if(line.find("errEffSysLower")!=std::string::npos){
      	//std::getline( tagfr, line);
	std::istringstream effSysSup(line);
	if(effSysSup>>tmp>>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >> 
	        tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>
	        fDataEffLowerSysOuterRRange_r1inFR[0]>>fDataEffLowerSysInnerRRange_r1inFR[0]>>fDataEffLowerSysOuterRRange_r1inFR[1]>>fDataEffLowerSysInnerRRange_r1inFR[1]>>fDataEffLowerSysOuterRRange_r1inFR[2]>>fDataEffLowerSysInnerRRange_r1inFR[2]>>fDataEffLowerSysOuterRRange_r1inFR[3]>>fDataEffLowerSysInnerRRange_r1inFR[3]>>fDataEffLowerSysOuterRRange_r1inFR[4]>>fDataEffLowerSysInnerRRange_r1inFR[4]>>fDataEffLowerSysOuterRRange_r1inFR[5]>>fDataEffLowerSysInnerRRange_r1inFR[5]>>fDataEffLowerSysOuterRRange_r1inFR[6]>>fDataEffLowerSysInnerRRange_r1inFR[6]>>fDataEffLowerSysOuterRRange_r1inFR[7]>>fDataEffLowerSysInnerRRange_r1inFR[7]>>
	   tmp >>tmp >>tmp >>tmp >>tmp >>tmp )std::cout<<"outer range tag fr sys Low "<<fDataEffLowerSysOuterRRange_r1inFR[0]<< std::endl;
      }
      //errEffSysLower
    }//end while cycle

  if(fCorrectEfficiencyForTruth)CorrectEff(fDataEffInnerRRange_r1inFR, fDataEffOuterRRange_r1inFR , fDataEffUpperSysInnerRRange_r1inFR ,fDataEffLowerSysInnerRRange_r1inFR, fDataEffUpperSysOuterRRange_r1inFR,fDataEffInnerRRange_r1inFR);
  std::cout<<"after truth correction inner0" << fDataEffInnerRRange_r1inFR[0]<< std::endl;


  std::ifstream tagprobefr(fnameTagProbeFR.c_str());
  while( std::getline( tagprobefr, line ))
    {
      std::istringstream iss(line);
      if(line.find("3sigma")!=std::string::npos){
      	std::getline( tagprobefr, line);
	std::istringstream eff(line);
	if(eff>>tmp>>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >> 
	        tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>
	        fDataEffOuterRRange_r1r2inFR[0]>>fDataEffInnerRRange_r1r2inFR[0]>>fDataEffOuterRRange_r1r2inFR[1]>>fDataEffInnerRRange_r1r2inFR[1]>>fDataEffOuterRRange_r1r2inFR[2]>>fDataEffInnerRRange_r1r2inFR[2]>>fDataEffOuterRRange_r1r2inFR[3]>>fDataEffInnerRRange_r1r2inFR[3]>>fDataEffOuterRRange_r1r2inFR[4]>>fDataEffInnerRRange_r1r2inFR[4]>>fDataEffOuterRRange_r1r2inFR[5]>>fDataEffInnerRRange_r1r2inFR[5]>>fDataEffOuterRRange_r1r2inFR[6]>>fDataEffInnerRRange_r1r2inFR[6]>>fDataEffOuterRRange_r1r2inFR[7]>>fDataEffInnerRRange_r1r2inFR[7]>>
		tmp >>tmp >>tmp >>tmp >>tmp >>tmp )std::cout<<"outer range tag probe fr "<<fDataEffOuterRRange_r1r2inFR[0]<< std::endl;
	std::getline( tagprobefr, line);
	std::istringstream stat(line);
	if(stat>>tmp>>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >> 
	        tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>
	        ferrDataEffOuterRRange_r1r2inFR[0]>>ferrDataEffInnerRRange_r1r2inFR[0]>>ferrDataEffOuterRRange_r1r2inFR[1]>>ferrDataEffInnerRRange_r1r2inFR[1]>>ferrDataEffOuterRRange_r1r2inFR[2]>>ferrDataEffInnerRRange_r1r2inFR[2]>>ferrDataEffOuterRRange_r1r2inFR[3]>>ferrDataEffInnerRRange_r1r2inFR[3]>>ferrDataEffOuterRRange_r1r2inFR[4]>>ferrDataEffInnerRRange_r1r2inFR[4]>>ferrDataEffOuterRRange_r1r2inFR[5]>>ferrDataEffInnerRRange_r1r2inFR[5]>>ferrDataEffOuterRRange_r1r2inFR[6]>>ferrDataEffInnerRRange_r1r2inFR[6]>>ferrDataEffOuterRRange_r1r2inFR[7]>>ferrDataEffInnerRRange_r1r2inFR[7]>>tmp >>tmp >>tmp >>tmp >>tmp >>tmp )std::cout<<"outer range tag probe fr err "<<ferrDataEffOuterRRange_r1r2inFR[0]<< std::endl;
      }
      if(line.find("errEffSysUpper")!=std::string::npos){
      	//std::getline( tagprobefr, line);
	std::istringstream effSysSup(line);
	if(effSysSup>>tmp>>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >> 
	        tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>
	        fDataEffUpperSysOuterRRange_r1r2inFR[0]>>fDataEffUpperSysInnerRRange_r1r2inFR[0]>>fDataEffUpperSysOuterRRange_r1r2inFR[1]>>fDataEffUpperSysInnerRRange_r1r2inFR[1]>>fDataEffUpperSysOuterRRange_r1r2inFR[2]>>fDataEffUpperSysInnerRRange_r1r2inFR[2]>>fDataEffUpperSysOuterRRange_r1r2inFR[3]>>fDataEffUpperSysInnerRRange_r1r2inFR[3]>>fDataEffUpperSysOuterRRange_r1r2inFR[4]>>fDataEffUpperSysInnerRRange_r1r2inFR[4]>>fDataEffUpperSysOuterRRange_r1r2inFR[5]>>fDataEffUpperSysInnerRRange_r1r2inFR[5]>>fDataEffUpperSysOuterRRange_r1r2inFR[6]>>fDataEffUpperSysInnerRRange_r1r2inFR[6]>>fDataEffUpperSysOuterRRange_r1r2inFR[7]>>fDataEffUpperSysInnerRRange_r1r2inFR[7]>>
	   tmp >>tmp >>tmp >>tmp >>tmp >>tmp )std::cout<<"outer range tag probe fr sys "<<fDataEffUpperSysOuterRRange_r1r2inFR[0]<< std::endl;
      }
      if(line.find("errEffSysLower")!=std::string::npos){
      	//std::getline( tagprobefr, line);
	std::istringstream effSysSup(line);
	if(effSysSup>>tmp>>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >> 
	        tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>
	        fDataEffLowerSysOuterRRange_r1r2inFR[0]>>fDataEffLowerSysInnerRRange_r1r2inFR[0]>>fDataEffLowerSysOuterRRange_r1r2inFR[1]>>fDataEffLowerSysInnerRRange_r1r2inFR[1]>>fDataEffLowerSysOuterRRange_r1r2inFR[2]>>fDataEffLowerSysInnerRRange_r1r2inFR[2]>>fDataEffLowerSysOuterRRange_r1r2inFR[3]>>fDataEffLowerSysInnerRRange_r1r2inFR[3]>>fDataEffLowerSysOuterRRange_r1r2inFR[4]>>fDataEffLowerSysInnerRRange_r1r2inFR[4]>>fDataEffLowerSysOuterRRange_r1r2inFR[5]>>fDataEffLowerSysInnerRRange_r1r2inFR[5]>>fDataEffLowerSysOuterRRange_r1r2inFR[6]>>fDataEffLowerSysInnerRRange_r1r2inFR[6]>>fDataEffLowerSysOuterRRange_r1r2inFR[7]>>fDataEffLowerSysInnerRRange_r1r2inFR[7]>>
	   tmp >>tmp >>tmp >>tmp >>tmp >>tmp )std::cout<<"outer range tag probe fr sysLow "<<fDataEffLowerSysOuterRRange_r1r2inFR[0]<< std::endl;
      }
    }//end while cycle

  if(fCorrectEfficiencyForTruth)CorrectEff(fDataEffInnerRRange_r1r2inFR, fDataEffOuterRRange_r1r2inFR , fDataEffUpperSysInnerRRange_r1r2inFR ,fDataEffLowerSysInnerRRange_r1r2inFR, fDataEffUpperSysOuterRRange_r1r2inFR,fDataEffInnerRRange_r1r2inFR);
  std::cout<<"r1r2 after truth correction " << fDataEffInnerRRange_r1r2inFR[0]<< std::endl;
  
  for(int i=0; i<8; i++){
    fDataEffUpperSysOuterRRange_r1inFR[i]  =fDataEffUpperSysOuterRRange_r1inFR[i]   + fDataEffOuterRRange_r1inFR[i];
    fDataEffUpperSysOuterRRange_r1r2inFR[i]=fDataEffUpperSysOuterRRange_r1r2inFR[i] + fDataEffOuterRRange_r1r2inFR[i];
    fDataEffUpperSysInnerRRange_r1inFR[i]  =fDataEffUpperSysInnerRRange_r1inFR[i]   + fDataEffInnerRRange_r1inFR[i];
    fDataEffUpperSysInnerRRange_r1r2inFR[i]=fDataEffUpperSysInnerRRange_r1r2inFR[i] + fDataEffInnerRRange_r1r2inFR[i];
    fDataEffLowerSysOuterRRange_r1inFR[i]  =fDataEffOuterRRange_r1inFR[i]  -fDataEffLowerSysOuterRRange_r1inFR[i]   ;
    fDataEffLowerSysOuterRRange_r1r2inFR[i]=fDataEffOuterRRange_r1r2inFR[i]-fDataEffLowerSysOuterRRange_r1r2inFR[i] ;
    fDataEffLowerSysInnerRRange_r1inFR[i]  =fDataEffInnerRRange_r1inFR[i]  -fDataEffLowerSysInnerRRange_r1inFR[i]   ;
    fDataEffLowerSysInnerRRange_r1r2inFR[i]=fDataEffInnerRRange_r1r2inFR[i]-fDataEffLowerSysInnerRRange_r1r2inFR[i] ;
    /*  
    if(fEffOuterRRange_r1inFR[i]>1.) fEffOuterRRange_r1inFR[i]=1.;
    if(fEffOuterRRange_r1r2inFR[i]>1.) fEffOuterRRange_r1r2inFR[i]=1.;
    if(fEffInnerRRange_r1inFR[i]>1.) fEffOuterRRange_r1inFR[i]=1.;
    if(fEffInnerRRange_r1r2inFR[i]>1.) fEffOuterRRange_r1r2inFR[i]=1.;
    if(fEffUpperSysOuterRRange_r1inFR[i]  >1.)fEffUpperSysOuterRRange_r1inFR[i]  =1.;
    if(fEffUpperSysOuterRRange_r1r2inFR[i]>1.)fEffUpperSysOuterRRange_r1r2inFR[i]=1.;
    if(fEffUpperSysInnerRRange_r1inFR[i]  >1.)fEffUpperSysInnerRRange_r1inFR[i]  =1.;
    if(fEffUpperSysInnerRRange_r1r2inFR[i]>1.)fEffUpperSysInnerRRange_r1r2inFR[i]=1.;
    */
    }

}


void AnnihilationSelection::fillEffVectorMC(std::string fnameTagFR, std::string fnameTagProbeFR){

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
	        fMCEffOuterRRange_r1inFR[0]>>fMCEffInnerRRange_r1inFR[0]>>fMCEffOuterRRange_r1inFR[1]>>fMCEffInnerRRange_r1inFR[1]>>fMCEffOuterRRange_r1inFR[2]>>fMCEffInnerRRange_r1inFR[2]>>fMCEffOuterRRange_r1inFR[3]>>fMCEffInnerRRange_r1inFR[3]>>fMCEffOuterRRange_r1inFR[4]>>fMCEffInnerRRange_r1inFR[4]>>fMCEffOuterRRange_r1inFR[5]>>fMCEffInnerRRange_r1inFR[5]>>fMCEffOuterRRange_r1inFR[6]>>fMCEffInnerRRange_r1inFR[6]>>fMCEffOuterRRange_r1inFR[7]>>fMCEffInnerRRange_r1inFR[7]>>
	   tmp >>tmp >>tmp >>tmp >>tmp >>tmp )std::cout<<"outer range tag fr "<<fMCEffOuterRRange_r1inFR[0]<< std::endl;
	std::getline( tagfr, line);
	std::istringstream sist(line);
	if(sist>>tmp>>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >> 
	        tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >> 
	        ferrMCEffOuterRRange_r1inFR[0]>>ferrMCEffInnerRRange_r1inFR[0]>>ferrMCEffOuterRRange_r1inFR[1]>>ferrMCEffInnerRRange_r1inFR[1]>>ferrMCEffOuterRRange_r1inFR[2]>>ferrMCEffInnerRRange_r1inFR[2]>>ferrMCEffOuterRRange_r1inFR[3]>>ferrMCEffInnerRRange_r1inFR[3]>>ferrMCEffOuterRRange_r1inFR[4]>>ferrMCEffInnerRRange_r1inFR[4]>>ferrMCEffOuterRRange_r1inFR[5]>>ferrMCEffInnerRRange_r1inFR[5]>>ferrMCEffOuterRRange_r1inFR[6]>>ferrMCEffInnerRRange_r1inFR[6]>>ferrMCEffOuterRRange_r1inFR[7]>>ferrMCEffInnerRRange_r1inFR[7]>>
	   tmp >>tmp >>tmp >>tmp >>tmp >>tmp )std::cout<<"outer range tag fr err "<<ferrMCEffOuterRRange_r1inFR[0]<< std::endl;
      }
      if(line.find("errEffSysUpper")!=std::string::npos){
      	//std::getline( tagfr, line);
	std::istringstream effSysSup(line);
	if(effSysSup>>tmp>>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >> 
	        tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>
	        fMCEffUpperSysOuterRRange_r1inFR[0]>>fMCEffUpperSysInnerRRange_r1inFR[0]>>fMCEffUpperSysOuterRRange_r1inFR[1]>>fMCEffUpperSysInnerRRange_r1inFR[1]>>fMCEffUpperSysOuterRRange_r1inFR[2]>>fMCEffUpperSysInnerRRange_r1inFR[2]>>fMCEffUpperSysOuterRRange_r1inFR[3]>>fMCEffUpperSysInnerRRange_r1inFR[3]>>fMCEffUpperSysOuterRRange_r1inFR[4]>>fMCEffUpperSysInnerRRange_r1inFR[4]>>fMCEffUpperSysOuterRRange_r1inFR[5]>>fMCEffUpperSysInnerRRange_r1inFR[5]>>fMCEffUpperSysOuterRRange_r1inFR[6]>>fMCEffUpperSysInnerRRange_r1inFR[6]>>fMCEffUpperSysOuterRRange_r1inFR[7]>>fMCEffUpperSysInnerRRange_r1inFR[7]>>
	   tmp >>tmp >>tmp >>tmp >>tmp >>tmp )std::cout<<"outer range tag fr sys "<<fMCEffUpperSysOuterRRange_r1inFR[0]<< std::endl;
      }
      if(line.find("errEffSysLower")!=std::string::npos){
      	//std::getline( tagfr, line);
	std::istringstream effSysSup(line);
	if(effSysSup>>tmp>>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >> 
	        tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>
	        fMCEffLowerSysOuterRRange_r1inFR[0]>>fMCEffLowerSysInnerRRange_r1inFR[0]>>fMCEffLowerSysOuterRRange_r1inFR[1]>>fMCEffLowerSysInnerRRange_r1inFR[1]>>fMCEffLowerSysOuterRRange_r1inFR[2]>>fMCEffLowerSysInnerRRange_r1inFR[2]>>fMCEffLowerSysOuterRRange_r1inFR[3]>>fMCEffLowerSysInnerRRange_r1inFR[3]>>fMCEffLowerSysOuterRRange_r1inFR[4]>>fMCEffLowerSysInnerRRange_r1inFR[4]>>fMCEffLowerSysOuterRRange_r1inFR[5]>>fMCEffLowerSysInnerRRange_r1inFR[5]>>fMCEffLowerSysOuterRRange_r1inFR[6]>>fMCEffLowerSysInnerRRange_r1inFR[6]>>fMCEffLowerSysOuterRRange_r1inFR[7]>>fMCEffLowerSysInnerRRange_r1inFR[7]>>
	   tmp >>tmp >>tmp >>tmp >>tmp >>tmp )std::cout<<"outer range tag fr sys Low "<<fMCEffLowerSysOuterRRange_r1inFR[0]<< std::endl;
      }
      //errEffSysLower
    }//end while cycle

  if(fCorrectEfficiencyForTruth)CorrectEff(fMCEffInnerRRange_r1inFR, fMCEffOuterRRange_r1inFR , fMCEffUpperSysInnerRRange_r1inFR ,fMCEffLowerSysInnerRRange_r1inFR, fMCEffUpperSysOuterRRange_r1inFR,fMCEffInnerRRange_r1inFR);
  std::cout<<"MC after truth correction " << fMCEffInnerRRange_r1inFR[0]<< std::endl;

  std::ifstream tagprobefr(fnameTagProbeFR.c_str());
  while( std::getline( tagprobefr, line ))
    {
      std::istringstream iss(line);
      if(line.find("3sigma")!=std::string::npos){
      	std::getline( tagprobefr, line);
	std::istringstream eff(line);
	if(eff>>tmp>>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >> 
	        tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>
	        fMCEffOuterRRange_r1r2inFR[0]>>fMCEffInnerRRange_r1r2inFR[0]>>fMCEffOuterRRange_r1r2inFR[1]>>fMCEffInnerRRange_r1r2inFR[1]>>fMCEffOuterRRange_r1r2inFR[2]>>fMCEffInnerRRange_r1r2inFR[2]>>fMCEffOuterRRange_r1r2inFR[3]>>fMCEffInnerRRange_r1r2inFR[3]>>fMCEffOuterRRange_r1r2inFR[4]>>fMCEffInnerRRange_r1r2inFR[4]>>fMCEffOuterRRange_r1r2inFR[5]>>fMCEffInnerRRange_r1r2inFR[5]>>fMCEffOuterRRange_r1r2inFR[6]>>fMCEffInnerRRange_r1r2inFR[6]>>fMCEffOuterRRange_r1r2inFR[7]>>fMCEffInnerRRange_r1r2inFR[7]>>
		tmp >>tmp >>tmp >>tmp >>tmp >>tmp )std::cout<<"outer range tag probe fr "<<fMCEffOuterRRange_r1r2inFR[0]<< std::endl;
	std::getline( tagprobefr, line);
	std::istringstream sist(line);
	if(sist>>tmp>>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >> 
	        tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>
	        ferrMCEffOuterRRange_r1r2inFR[0]>>ferrMCEffInnerRRange_r1r2inFR[0]>>ferrMCEffOuterRRange_r1r2inFR[1]>>ferrMCEffInnerRRange_r1r2inFR[1]>>ferrMCEffOuterRRange_r1r2inFR[2]>>ferrMCEffInnerRRange_r1r2inFR[2]>>ferrMCEffOuterRRange_r1r2inFR[3]>>ferrMCEffInnerRRange_r1r2inFR[3]>>ferrMCEffOuterRRange_r1r2inFR[4]>>ferrMCEffInnerRRange_r1r2inFR[4]>>ferrMCEffOuterRRange_r1r2inFR[5]>>ferrMCEffInnerRRange_r1r2inFR[5]>>ferrMCEffOuterRRange_r1r2inFR[6]>>ferrMCEffInnerRRange_r1r2inFR[6]>>ferrMCEffOuterRRange_r1r2inFR[7]>>ferrMCEffInnerRRange_r1r2inFR[7]>>
		tmp >>tmp >>tmp >>tmp >>tmp >>tmp )std::cout<<"outer range tag probe fr err "<<ferrMCEffOuterRRange_r1r2inFR[0]<< std::endl;
      }
      if(line.find("errEffSysUpper")!=std::string::npos){
      	//std::getline( tagprobefr, line);
	std::istringstream effSysSup(line);
	if(effSysSup>>tmp>>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >> 
	        tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>
	        fMCEffUpperSysOuterRRange_r1r2inFR[0]>>fMCEffUpperSysInnerRRange_r1r2inFR[0]>>fMCEffUpperSysOuterRRange_r1r2inFR[1]>>fMCEffUpperSysInnerRRange_r1r2inFR[1]>>fMCEffUpperSysOuterRRange_r1r2inFR[2]>>fMCEffUpperSysInnerRRange_r1r2inFR[2]>>fMCEffUpperSysOuterRRange_r1r2inFR[3]>>fMCEffUpperSysInnerRRange_r1r2inFR[3]>>fMCEffUpperSysOuterRRange_r1r2inFR[4]>>fMCEffUpperSysInnerRRange_r1r2inFR[4]>>fMCEffUpperSysOuterRRange_r1r2inFR[5]>>fMCEffUpperSysInnerRRange_r1r2inFR[5]>>fMCEffUpperSysOuterRRange_r1r2inFR[6]>>fMCEffUpperSysInnerRRange_r1r2inFR[6]>>fMCEffUpperSysOuterRRange_r1r2inFR[7]>>fMCEffUpperSysInnerRRange_r1r2inFR[7]>>
	   tmp >>tmp >>tmp >>tmp >>tmp >>tmp )std::cout<<"outer range tag probe fr sys "<<fMCEffUpperSysOuterRRange_r1r2inFR[0]<< std::endl;
      }
      if(line.find("errEffSysLower")!=std::string::npos){
      	//std::getline( tagprobefr, line);
	std::istringstream effSysSup(line);
	if(effSysSup>>tmp>>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >> 
	        tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>tmp >>
	        fMCEffLowerSysOuterRRange_r1r2inFR[0]>>fMCEffLowerSysInnerRRange_r1r2inFR[0]>>fMCEffLowerSysOuterRRange_r1r2inFR[1]>>fMCEffLowerSysInnerRRange_r1r2inFR[1]>>fMCEffLowerSysOuterRRange_r1r2inFR[2]>>fMCEffLowerSysInnerRRange_r1r2inFR[2]>>fMCEffLowerSysOuterRRange_r1r2inFR[3]>>fMCEffLowerSysInnerRRange_r1r2inFR[3]>>fMCEffLowerSysOuterRRange_r1r2inFR[4]>>fMCEffLowerSysInnerRRange_r1r2inFR[4]>>fMCEffLowerSysOuterRRange_r1r2inFR[5]>>fMCEffLowerSysInnerRRange_r1r2inFR[5]>>fMCEffLowerSysOuterRRange_r1r2inFR[6]>>fMCEffLowerSysInnerRRange_r1r2inFR[6]>>fMCEffLowerSysOuterRRange_r1r2inFR[7]>>fMCEffLowerSysInnerRRange_r1r2inFR[7]>>
	   tmp >>tmp >>tmp >>tmp >>tmp >>tmp )std::cout<<"outer range tag probe fr sysLow "<<fMCEffLowerSysOuterRRange_r1r2inFR[0]<< std::endl;
      }
    }//end while cycle

  if(fCorrectEfficiencyForTruth)CorrectEff(fMCEffInnerRRange_r1r2inFR, fMCEffOuterRRange_r1r2inFR , fMCEffUpperSysInnerRRange_r1r2inFR ,fMCEffLowerSysInnerRRange_r1r2inFR, fMCEffUpperSysOuterRRange_r1r2inFR,fMCEffInnerRRange_r1r2inFR);
  std::cout<<"MC after truth correction r1r2 " << fMCEffInnerRRange_r1r2inFR[0]<< std::endl;
  
  for(int i=0; i<8; i++){
    fMCEffUpperSysOuterRRange_r1inFR[i]  =fMCEffUpperSysOuterRRange_r1inFR[i]   + fMCEffOuterRRange_r1inFR[i];
    fMCEffUpperSysOuterRRange_r1r2inFR[i]=fMCEffUpperSysOuterRRange_r1r2inFR[i] + fMCEffOuterRRange_r1r2inFR[i];
    fMCEffUpperSysInnerRRange_r1inFR[i]  =fMCEffUpperSysInnerRRange_r1inFR[i]   + fMCEffInnerRRange_r1inFR[i];
    fMCEffUpperSysInnerRRange_r1r2inFR[i]=fMCEffUpperSysInnerRRange_r1r2inFR[i] + fMCEffInnerRRange_r1r2inFR[i];
    fMCEffLowerSysOuterRRange_r1inFR[i]  =fMCEffOuterRRange_r1inFR[i]  -fMCEffLowerSysOuterRRange_r1inFR[i]   ;
    fMCEffLowerSysOuterRRange_r1r2inFR[i]=fMCEffOuterRRange_r1r2inFR[i]-fMCEffLowerSysOuterRRange_r1r2inFR[i] ;
    fMCEffLowerSysInnerRRange_r1inFR[i]  =fMCEffInnerRRange_r1inFR[i]  -fMCEffLowerSysInnerRRange_r1inFR[i]   ;
    fMCEffLowerSysInnerRRange_r1r2inFR[i]=fMCEffInnerRRange_r1r2inFR[i]-fMCEffLowerSysInnerRRange_r1r2inFR[i] ;
    /*  
    if(fEffOuterRRange_r1inFR[i]>1.) fEffOuterRRange_r1inFR[i]=1.;
    if(fEffOuterRRange_r1r2inFR[i]>1.) fEffOuterRRange_r1r2inFR[i]=1.;
    if(fEffInnerRRange_r1inFR[i]>1.) fEffOuterRRange_r1inFR[i]=1.;
    if(fEffInnerRRange_r1r2inFR[i]>1.) fEffOuterRRange_r1r2inFR[i]=1.;
    if(fEffUpperSysOuterRRange_r1inFR[i]  >1.)fEffUpperSysOuterRRange_r1inFR[i]  =1.;
    if(fEffUpperSysOuterRRange_r1r2inFR[i]>1.)fEffUpperSysOuterRRange_r1r2inFR[i]=1.;
    if(fEffUpperSysInnerRRange_r1inFR[i]  >1.)fEffUpperSysInnerRRange_r1inFR[i]  =1.;
    if(fEffUpperSysInnerRRange_r1r2inFR[i]>1.)fEffUpperSysInnerRRange_r1r2inFR[i]=1.;
    */
    }

}

void AnnihilationSelection::fillEffVector(Bool_t isMC){

  if(isMC ||( fScaleFMethod && fNoTargetBool)){//in the analysis i would use the efficiency T&P extracted from MC sample. Due to the fact that MC has not backgroud, if i would study the scale factor analysis I should weight the bkg with MC eff
    std::cout<<"NOTE: I'm using the MC efficiencies----------" << std::endl;
    for(int i=0; i<8; i++){
      fEffInnerRRange_r1inFR[i]	               =fMCEffInnerRRange_r1inFR[i];	               
      fEffOuterRRange_r1inFR[i]	               =fMCEffOuterRRange_r1inFR[i];	              
      fEffInnerRRange_r1r2inFR[i]              =fMCEffInnerRRange_r1r2inFR[i];	         	       
      fEffOuterRRange_r1r2inFR[i]              =fMCEffOuterRRange_r1r2inFR[i];	

      ferrEffInnerRRange_r1inFR[i]	       =ferrMCEffInnerRRange_r1inFR[i];	               
      ferrEffOuterRRange_r1inFR[i]	       =ferrMCEffOuterRRange_r1inFR[i];	              
      ferrEffInnerRRange_r1r2inFR[i]           =ferrMCEffInnerRRange_r1r2inFR[i];	         	       
      ferrEffOuterRRange_r1r2inFR[i]           =ferrMCEffOuterRRange_r1r2inFR[i];	            	       
                                    	     
      fEffInnerRRangeFromTruth_r1inFR[i]       =fMCEffInnerRRangeFromTruth_r1inFR[i];    
      fEffOuterRRangeFromTruth_r1inFR[i]       =fMCEffOuterRRangeFromTruth_r1inFR[i];     
      fEffInnerRRangeFromTruth_r1r2inFR[i]     =fMCEffInnerRRangeFromTruth_r1r2inFR[i]; 
      fEffOuterRRangeFromTruth_r1r2inFR[i]     =fMCEffOuterRRangeFromTruth_r1r2inFR[i];    
                                    	     
      fEffUpperSysInnerRRange_r1inFR[i]        =fMCEffUpperSysInnerRRange_r1inFR[i];        
      fEffUpperSysOuterRRange_r1inFR[i]        =fMCEffUpperSysOuterRRange_r1inFR[i];        
      fEffUpperSysInnerRRange_r1r2inFR[i]      =fMCEffUpperSysInnerRRange_r1r2inFR[i];        
      fEffUpperSysOuterRRange_r1r2inFR[i]      =fMCEffUpperSysOuterRRange_r1r2inFR[i];        
                                    	     
      fEffLowerSysInnerRRange_r1inFR[i]        =fMCEffLowerSysInnerRRange_r1inFR[i];              
      fEffLowerSysOuterRRange_r1inFR[i]        =fMCEffLowerSysOuterRRange_r1inFR[i];              
      fEffLowerSysInnerRRange_r1r2inFR[i]      =fMCEffLowerSysInnerRRange_r1r2inFR[i];           
      fEffLowerSysOuterRRange_r1r2inFR[i]      =fMCEffLowerSysOuterRRange_r1r2inFR[i];            
    }
  }
  else{//in the analysis i would use the efficiency T&P extracted from data sample
    std::cout<<"NOTE: I'm using the data efficiencies----------" << std::endl;
    for(int i=0; i<8; i++){
      fEffInnerRRange_r1inFR[i]	               =fDataEffInnerRRange_r1inFR[i];	               
      fEffOuterRRange_r1inFR[i]	               =fDataEffOuterRRange_r1inFR[i];	              
      fEffInnerRRange_r1r2inFR[i]              =fDataEffInnerRRange_r1r2inFR[i];	         	       
      fEffOuterRRange_r1r2inFR[i]              =fDataEffOuterRRange_r1r2inFR[i];

      ferrEffInnerRRange_r1inFR[i]	       =ferrDataEffInnerRRange_r1inFR[i];	               
      ferrEffOuterRRange_r1inFR[i]	       =ferrDataEffOuterRRange_r1inFR[i];	              
      ferrEffInnerRRange_r1r2inFR[i]           =ferrDataEffInnerRRange_r1r2inFR[i];	         	       
      ferrEffOuterRRange_r1r2inFR[i]           =ferrDataEffOuterRRange_r1r2inFR[i];	            	       
                                    	     	
      fEffInnerRRangeFromTruth_r1inFR[i]       =fDataEffInnerRRangeFromTruth_r1inFR[i];    
      fEffOuterRRangeFromTruth_r1inFR[i]       =fDataEffOuterRRangeFromTruth_r1inFR[i];     
      fEffInnerRRangeFromTruth_r1r2inFR[i]     =fDataEffInnerRRangeFromTruth_r1r2inFR[i]; 
      fEffOuterRRangeFromTruth_r1r2inFR[i]     =fDataEffOuterRRangeFromTruth_r1r2inFR[i];    
                                    	     	
      fEffUpperSysInnerRRange_r1inFR[i]        =fDataEffUpperSysInnerRRange_r1inFR[i];        
      fEffUpperSysOuterRRange_r1inFR[i]        =fDataEffUpperSysOuterRRange_r1inFR[i];        
      fEffUpperSysInnerRRange_r1r2inFR[i]      =fDataEffUpperSysInnerRRange_r1r2inFR[i];        
      fEffUpperSysOuterRRange_r1r2inFR[i]      =fDataEffUpperSysOuterRRange_r1r2inFR[i];        
                                    	     
      fEffLowerSysInnerRRange_r1inFR[i]        =fDataEffLowerSysInnerRRange_r1inFR[i];              
      fEffLowerSysOuterRRange_r1inFR[i]        =fDataEffLowerSysOuterRRange_r1inFR[i];              
      fEffLowerSysInnerRRange_r1r2inFR[i]      =fDataEffLowerSysInnerRRange_r1r2inFR[i];           
      fEffLowerSysOuterRRange_r1r2inFR[i]      =fDataEffLowerSysOuterRRange_r1r2inFR[i];            

    }
  }

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

void AnnihilationSelection::fillEffVectorRooFit(){
  //roofit efficiency extraction->Fit on Tag distribution (Beam+Pileup(27kCalchep) background) in range [-100, 100] ; nProbe as usual: bkg subtraction and integral in 3 sigma  //only for r1 in FR
  
  Double_t nTagRF_in[8]={11007., 7617., 6992., 8634., 9196., 8409., 7706., 9411.};
  Double_t nTagRF_out[8]={8493., 9261., 8082., 8552., 7963., 8184., 8700., 8615.};

  Double_t nTagRFEThr90_in[8]= {9136., 7381., 6687., 8305., 8813., 7657., 7112., 8222. };
  Double_t nTagRFEThr90_out[8]={8498., 9255., 8051., 8438., 7815., 8148., 8629., 8387.};

  Double_t nProbe3Sigma_in[8]= {8725., 7017., 6421., 8383., 8654., 7474., 7218., 7958.};
  Double_t nProbe3Sigma_out[8]={8793., 8086., 7412., 8548., 8262., 6781., 6373., 8072.};


  for(int i=0; i<8; i++){ 
    fEffInnerRooFit_r1inFR[i]      =nProbe3Sigma_in[i] /nTagRF_in[i];	
    fEffOuterRooFit_r1inFR[i]      =nProbe3Sigma_out[i]/nTagRF_out[i];	
    fEffInnerRooFitEThr90_r1inFR[i]=nProbe3Sigma_in[i] /nTagRFEThr90_in[i];
    fEffOuterRooFitEThr90_r1inFR[i]=nProbe3Sigma_out[i]/nTagRFEThr90_out[i];
  }

}


Bool_t AnnihilationSelection::InitHistos(Bool_t allAnnPlot)
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
  if(allAnnPlot){
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
  }
  for(int i=0; i<sufix.size(); i++){
    binX=500;
    minX=0.;
    maxX=2000.;
    hname="ECAL_gravTwoPhoton10ns" + sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10nsTrend" + sufix.at(i);
    hSvc->BookHisto2(hname,400, -200, 200, binX, minX, maxX);
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
    binX=200;
    minX=0.01;
    maxX=0.1;
    binY=600;
    minY=0.;
    maxY=800.;
    hname="ECAL_EnergyvsTheta"+sufix.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    binX=600;
    minX=0.;
    maxX=800.;
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
  if(allAnnPlot){
    binX=500;
    minX=0.;
    maxX=2000.;
    hname="ECAL_gravTwoPhoton10ns_g1inFRin4Cog";
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_g1inFRin4_2Cog";
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_g1inFRin4_4Cog";
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_g1inFRin4_6Cog";
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_g1inFRin4_8Cog";
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_g1inFRin5_2Cog";
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_g1inFRin5_4Cog";
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_g1inFRin5_6Cog";
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_g1inFRin5_8Cog";
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_g1inFRin6Cog";
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="Annihilation_NClAtLeast2g1inFREThr50";
    hSvc->BookHisto(hname, binX, minX, maxX);
  }

  //eff weighted hitograms
  std::vector<std::string> sufixW;
  sufixW.push_back("_g1inFRDTime10");
   if(allAnnPlot)sufixW.push_back("_g1inFRin5Cog");
  for(int i=0; i<sufixW.size(); i++){
    binX=500;
    minX=0.;
    maxX=2000.;
    hname="ECAL_gravTwoPhoton10ns_WEffR1inFR" + sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WIpotEvEffR1inFR"+sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WEffPlusStatisticErrorR1inFR" + sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WEffPlusSysR1inFR" + sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WEffPlusLowSysR1inFR" + sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WEffPlusSysRandomR1inFR" + sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WEffPlusLowSysRandomR1inFR" + sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WEffPlusUpSysFirstLowSysSecondRandomR1inFR"+sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WEffPlusLowSysFirstUpSysSecondRandomR1inFR"+sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WEffPlusUpSysFirstLowSysSecondR1inFR"+sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WEffPlusLowSysFirstUpSysSecondR1inFR"+sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WAccR1inFR"+sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WAccEffR1inFR"+sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WAccEffPlusSysR1inFR"+sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    
  }
 if(allAnnPlot){
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
     hname="ECAL_gravTwoPhoton10ns_WEffPlusStatisticErrorR1R2inFR" + sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_WEffPlusSysR1R2inFR" + sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_WEffPlusLowSysR1R2inFR" + sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_WEffPlusSysRandomR1R2inFR" + sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_WEffPlusLowSysRandomR1R2inFR" + sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_WEffPlusUpSysFirstLowSysSecondRandomR1R2inFR"+sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_WEffPlusLowSysFirstUpSysSecondRandomR1R2inFR"+sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_WEffPlusUpSysFirstLowSysSecondR1R2inFR"+sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_WEffPlusLowSysFirstUpSysSecondR1R2inFR"+sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_WAccR1R2inFR"+sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_WAccEffR1R2inFR"+sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_WAccEffPlusSysR1R2inFR"+sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_W2ndApproachR1R2inFR"+sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_W2ndApproachPlusStatisticErrorR1R2inFR" + sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_W2ndApproachPlusSysR1R2inFR"+sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_W2ndApproachPlusLowSysR1R2inFR"+sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_W2ndApproachPlusSysRandomR1R2inFR"+sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_W2ndApproachPlusLowSysRandomR1R2inFR"+sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_W2ndApproachPlusUpSysFirstLowSysSecondRandomR1R2inFR"+sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_W2ndApproachPlusLowSysFirstUpSysSecondRandomR1R2inFR"+sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_W2ndApproachPlusUpSysFirstLowSysSecondR1R2inFR"+sufixW2.at(i);
     hSvc->BookHisto(hname, binX, minX, maxX);
     hname="ECAL_gravTwoPhoton10ns_W2ndApproachPlusLowSysFirstUpSysSecondR1R2inFR"+sufixW2.at(i);
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
   hname="ECAL_R1R2inFR_EfficiencyVSPhi_InnerRadius";
   hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY );
   hname="ECAL_R1R2inFR_EfficiencyVSPhi_OutRadius";
   hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY );
 }   //end of  if(allAnnPlot)
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

 std::vector<std::string> sufixP;
 sufixP.push_back("_g1inFRDTime10_NoWeight");
 sufixP.push_back("_g1inFRDTime10");
 sufixP.push_back("_g1inFRDTime10_RooFit"); 
 if(allAnnPlot){
   sufixP.push_back("_g1inFRDTime10_RooFitEThr");
   sufixP.push_back("_g1inFRin5Cog_NoWeight");
   sufixP.push_back("_g1inFRin5Cog");
   sufixP.push_back("_g1inFRin5Cog_RooFit");
   sufixP.push_back("_g1inFRin5Cog_RooFitEThr");
 }
 for(int i=0; i<sufixP.size(); i++){
   binX=500;
   minX=0.;
   maxX=2000.;
   hname="ECAL_gravTwoPhoton10ns_WEffR1inFR_total"+sufixP.at(i); 
   hSvc->BookHisto(hname, binX, minX, maxX);  
   hname="ECAL_gravTwoPhoton10ns_WEffR1inFR_g1in0_45"+sufixP.at(i); 
   hSvc->BookHisto(hname, binX, minX, maxX);  
   hname="ECAL_gravTwoPhoton10ns_WEffR1inFR_g1in45_90"+sufixP.at(i); 
   hSvc->BookHisto(hname, binX, minX, maxX); 
    hname="ECAL_gravTwoPhoton10ns_WEffR1inFR_g1in90_135"+sufixP.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX); 
    hname="ECAL_gravTwoPhoton10ns_WEffR1inFR_g1in135_180"+sufixP.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WEffR1inFR_g1in180_225"+sufixP.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WEffR1inFR_g1in225_270"+sufixP.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WEffR1inFR_g1in270_315"+sufixP.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10ns_WEffR1inFR_g1in315_360"+sufixP.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    binX=120;
    minX=-30.;
    maxX=30.;
    hname="ECAL_gravTwoPhoton10nsDPhi_WEffR1inFR_total"+sufixP.at(i); 
    hSvc->BookHisto(hname, binX, minX, maxX);  
    hname="ECAL_gravTwoPhoton10nsLargeDPhi_WEffR1inFR_total"+sufixP.at(i); 
    hSvc->BookHisto(hname, 800, -400, 400); 
    hname="ECAL_gravTwoPhoton10nsDPhi_WEffR1inFR_g1in0_45"+sufixP.at(i); 
    hSvc->BookHisto(hname, binX, minX, maxX);  
    hname="ECAL_gravTwoPhoton10nsDPhi_WEffR1inFR_g1in45_90"+sufixP.at(i); 
    hSvc->BookHisto(hname, binX, minX, maxX); 
    hname="ECAL_gravTwoPhoton10nsDPhi_WEffR1inFR_g1in90_135"+sufixP.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX); 
    hname="ECAL_gravTwoPhoton10nsDPhi_WEffR1inFR_g1in135_180"+sufixP.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10nsDPhi_WEffR1inFR_g1in180_225"+sufixP.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10nsDPhi_WEffR1inFR_g1in225_270"+sufixP.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10nsDPhi_WEffR1inFR_g1in270_315"+sufixP.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10nsDPhi_WEffR1inFR_g1in315_360"+sufixP.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
  }


 if(allAnnPlot){
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

  if(allAnnPlot){
    binX=250;
    minX=0.;
    maxX=1000.;
    hname="ECAL_gravTwoPhoton10ns"; 
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10nsg1EThr";
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10nsg2EThr";
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10nsg1InFR";
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10nsg1InFRg1Ethr";
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10nsg1InFRg2Ethr";
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10nsg1InFR5CoG";
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10nsg1InFR5CoGg1EThr";
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10nsg1InFR5CoGg2EThr";
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10nsg1g2InFR5CoG";
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10nsg1g2InFR5CoGg1EThr";
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="ECAL_gravTwoPhoton10nsg1g2InFR5CoGg2EThr";
    hSvc->BookHisto(hname, binX, minX, maxX);
  }



  std::vector<std::string> sufixC;
  sufixC.push_back("_g1inFRDTime10");
  for(int i=0; i<sufixC.size(); i++){
    binX=400;
    minX=-300.;
    maxX=300.;
    hname="ECAL_gravTwoPhoton10ns_XCoG_total"+sufixC.at(i);          hSvc->BookHisto(hname,binX, minX, maxX  );
    hname="ECAL_gravTwoPhoton10ns_XCoG_g1in0_45"+sufixC.at(i);       hSvc->BookHisto(hname,binX, minX, maxX  );     
    hname="ECAL_gravTwoPhoton10ns_XCoG_g1in45_90"+sufixC.at(i);      hSvc->BookHisto(hname,binX, minX, maxX  );
    hname="ECAL_gravTwoPhoton10ns_XCoG_g1in90_135"+sufixC.at(i);     hSvc->BookHisto(hname,binX, minX, maxX  );
    hname="ECAL_gravTwoPhoton10ns_XCoG_g1in135_180"+sufixC.at(i);    hSvc->BookHisto(hname,binX, minX, maxX  );
    hname="ECAL_gravTwoPhoton10ns_XCoG_g1in180_225"+sufixC.at(i);    hSvc->BookHisto(hname,binX, minX, maxX  );
    hname="ECAL_gravTwoPhoton10ns_XCoG_g1in225_270"+sufixC.at(i);    hSvc->BookHisto(hname,binX, minX, maxX  );
    hname="ECAL_gravTwoPhoton10ns_XCoG_g1in270_315"+sufixC.at(i);    hSvc->BookHisto(hname,binX, minX, maxX  );
    hname="ECAL_gravTwoPhoton10ns_XCoG_g1in315_360"+sufixC.at(i);    hSvc->BookHisto(hname,binX, minX, maxX  );

    hname="ECAL_gravTwoPhoton10ns_YCoG_total"+sufixC.at(i);          hSvc->BookHisto(hname,binX, minX, maxX  );
    hname="ECAL_gravTwoPhoton10ns_YCoG_g1in0_45"+sufixC.at(i);       hSvc->BookHisto(hname,binX, minX, maxX  ); 
    hname="ECAL_gravTwoPhoton10ns_YCoG_g1in45_90"+sufixC.at(i);      hSvc->BookHisto(hname,binX, minX, maxX  );
    hname="ECAL_gravTwoPhoton10ns_YCoG_g1in90_135"+sufixC.at(i);     hSvc->BookHisto(hname,binX, minX, maxX  );
    hname="ECAL_gravTwoPhoton10ns_YCoG_g1in135_180"+sufixC.at(i);    hSvc->BookHisto(hname,binX, minX, maxX  );
    hname="ECAL_gravTwoPhoton10ns_YCoG_g1in180_225"+sufixC.at(i);    hSvc->BookHisto(hname,binX, minX, maxX  );
    hname="ECAL_gravTwoPhoton10ns_YCoG_g1in225_270"+sufixC.at(i);    hSvc->BookHisto(hname,binX, minX, maxX  );
    hname="ECAL_gravTwoPhoton10ns_YCoG_g1in270_315"+sufixC.at(i);    hSvc->BookHisto(hname,binX, minX, maxX  );
    hname="ECAL_gravTwoPhoton10ns_YCoG_g1in315_360"+sufixC.at(i);    hSvc->BookHisto(hname,binX, minX, maxX  );

    hname="ECAL_gravTwoPhoton10ns_CoGmap_total"+sufixC.at(i);        hSvc->BookHisto2(hname,binX, minX, maxX,binX, minX, maxX  );
    hname="ECAL_gravTwoPhoton10ns_CoGmap_g1in0_45"+sufixC.at(i);     hSvc->BookHisto2(hname,binX, minX, maxX,binX, minX, maxX  );    
    hname="ECAL_gravTwoPhoton10ns_CoGmap_g1in45_90"+sufixC.at(i);    hSvc->BookHisto2(hname, binX, minX, maxX,binX, minX, maxX  );
    hname="ECAL_gravTwoPhoton10ns_CoGmap_g1in90_135"+sufixC.at(i);   hSvc->BookHisto2(hname, binX, minX, maxX,binX, minX, maxX   );
    hname="ECAL_gravTwoPhoton10ns_CoGmap_g1in135_180"+sufixC.at(i);  hSvc->BookHisto2(hname, binX, minX, maxX,binX, minX, maxX  );
    hname="ECAL_gravTwoPhoton10ns_CoGmap_g1in180_225"+sufixC.at(i);  hSvc->BookHisto2(hname,binX, minX, maxX,binX, minX, maxX  );
    hname="ECAL_gravTwoPhoton10ns_CoGmap_g1in225_270"+sufixC.at(i);  hSvc->BookHisto2(hname,binX, minX, maxX,binX, minX, maxX  );
    hname="ECAL_gravTwoPhoton10ns_CoGmap_g1in270_315"+sufixC.at(i);  hSvc->BookHisto2(hname, binX, minX, maxX,binX, minX, maxX   );
    hname="ECAL_gravTwoPhoton10ns_CoGmap_g1in315_360"+sufixC.at(i);  hSvc->BookHisto2(hname,binX, minX, maxX,binX, minX, maxX  );

  }

  if(fdataMCmethod)fDataMCMethod->InitHistos();
  if(fScaleFMethod)fMCscaleF->InitHistos();
  if(fsFacCalchepPileup)fscaleFacCalchepPileup->InitHistos();

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



void AnnihilationSelection::CorrectEff(Double_t (&Inner)[8], Double_t (&Outer)[8], Double_t (&sysUpIn)[8],Double_t (&sysLowIn)[8], Double_t (&sysUpOut)[8],Double_t (&sysLowOut)[8]){
  //i'm using the correction factor extracted from EffTruth/EffReco of calchep no died crystals. usual TP selection
  //Double_t correctionVecInner[8]={1.01163726064154, 1.01381153337342, 1.01035993138096, 0.99720825699279, 1.02454579025111, 1.00924031342547, 1.01202785185185,1.00666262750399};
  //Double_t correctionVecOuter[8]={1.02403962844488, 1.00911518815546, 1.01235272323083, 1.00684984021632, 1.01184012443577, 1.01493262701661, 1.01074456974749, 0.99795367224734};

  //i'm using the correction factor extracted from EffTruth/EffReco of calchep no died crystals. usual TP selection + EThr 90MeV on nCluster in TruthEff
  Double_t correctionVecInner[8]={0.990157195391025,1.00136156848396,0.99517977722772,0.98836650117909,1.00615357142857,0.98880659778414,0.99570363295880,0.99577457901958};
  Double_t correctionVecOuter[8]={1.00927536160217, 1.00860645566441,1.00426175219024,0.99984002994759,1.01097562848297,1.0042923768997	,1.00582515207945,0.997208670736262};

  std::cout<<"inner0 in correctEff " << Inner[0] << std::endl;
  for(int i=0; i<8; i++){
    std::cout<<"Eff before correction inner " << Inner[i] <<" correction " << correctionVecInner[i] << " outer " << Outer[i] << " correction " << correctionVecOuter[i] << std::endl;
    Inner[i]=Inner[i]*correctionVecInner[i];
    sysUpIn[i]=sysUpIn[i]*correctionVecInner[i];
    sysLowIn[i]=sysLowIn[i]*correctionVecInner[i];

    Outer[i]=Outer[i]*correctionVecOuter[i];
    sysUpOut[i] =sysUpOut[i]*correctionVecOuter[i];
    sysLowOut[i]=sysLowOut[i]*correctionVecOuter[i];
    std::cout<<"Eff after correction inner " << Inner[i] << " outer " << Outer[i] << std::endl;
  }


}

Bool_t AnnihilationSelection::passAnnPreSelection(Double_t clE, Double_t clTheta){
  bool pass=false;
  if(clE<90.)return pass;
  Double_t constantE=6.39845;
  Double_t slopeE=-2.05831e+01;
  Double_t EnergyFunc=exp(constantE+slopeE*clTheta);
  Double_t DeltaEnergyFunction= -(EnergyFunc-clE);
  if(fabs(DeltaEnergyFunction)<100.) pass=true;
  return pass;
}

void  AnnihilationSelection::printCounters(){
  std::cout<<"Annihilation selection counters" << std::endl;
  std::cout<<"fcountEvent                                 "<< fcountEvent          << std::endl;
  std::cout<<"fcountDTime10                               "<<fcountDTime10                    << std::endl;
  std::cout<<"fcountDTime10g1InFR                         "<<fcountDTime10g1InFR              << std::endl;
  std::cout<<"fcountDTime10g1InFR5CoGX                    "<<fcountDTime10g1InFR5CoGX                    <<std::endl;;
  std::cout<<"fcountDTime10g1InFR5CoGXY                   "<<fcountDTime10g1InFR5CoGXY                   <<std::endl;;
  std::cout<<"fcountDTime10g1InFR5CoGXYEgCut              "<<fcountDTime10g1InFR5CoGXYEgCut              <<std::endl;;
  std::cout<<"fcountDTime10g1InFR5CoGXYEgCutESumCut       "<<fcountDTime10g1InFR5CoGXYEgCutESumCut       <<std::endl;;
  std::cout<<"fcountDTime10g1InFR5CoGXYEgCutESumCutg2inFR "<<fcountDTime10g1InFR5CoGXYEgCutESumCutg2inFR <<std::endl;;

}



