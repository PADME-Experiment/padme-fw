#include "CalchepTruthStudies.hh"

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



CalchepTruthStudies::CalchepTruthStudies()
{
  
  fRecoEvent   = NULL;

  fECal_hitEvent=0;
  fECal_ClColl=0;

  fVerbose        = 0;

 
}
CalchepTruthStudies::CalchepTruthStudies(Int_t processingMode, Int_t verbosityFlag)
{
  fRecoEvent      = NULL;
  fVerbose        = verbosityFlag;
  fInitToComplete =true;

}
CalchepTruthStudies::~CalchepTruthStudies()
{
}

Bool_t CalchepTruthStudies::Init(TRecoEvent* eventHeader, 
				 TRecoVObject* ECALev, TRecoVClusCollection* ECALcl){
  fRecoEvent = eventHeader;
  fECal_hitEvent      =ECALev   ;
  fECal_ClColl        =ECALcl   ;

  //fdistanceTarget=3470;
  //fdistanceTarget=3470+230/2;
  //fdistanceTarget=3511; //from fit on HitG4 z MPV posistion
  fdistanceTarget=3513; //from fit on HitG4 z MPV but target position given by the same sw used to simulate ... 
  fdistanceTarget=1030+2513; //from fit on HitG4 z mean but target position given by the same sw used to simulate ... 
  fFRmin=115.82;
  fFRmid=172.83;
  fFRmax=258.0;
  fcountEvent=0;
  energyCut=true;


  Double_t tmpradiusRangeMin[3]={fFRmin, fFRmid, fFRmin};
  Double_t tmpradiusRangeMax[3]={fFRmid, fFRmax, fFRmax};
  Double_t tmpphiRange[9]={0., 45., 90., 135., 180., 225., 270., 315., 360.};      
  for(int i=0; i<9; i++){
    if(i<3){
      radiusRangeMin[i]=tmpradiusRangeMin[i];
      radiusRangeMax[i]=tmpradiusRangeMax[i];
    }
    phiRange[i]=tmpphiRange[i];
  }
  for(int i=0; i<3; i++){
    for(int j=0;j<8; j++){
      nTruth[i][j]=0;
      nCluster_ifTruthOutRin[i][j]=0;
      nCluster_ifTruthOutRout[i][j]=0;
      nClusterOutRin_ifTruth[i][j]=0;
      nClusterOutRout_ifTruth[i][j]=0;
      nCluster_ifTRuth[i][j]=0;
      nCluster[i][j]=0;
      nCluster_ifnoTruth[i][j]=0;      
    }
  }

  fCalchep450.open("ee-gg_450MeV_NoPipe.txt");
  fCalchep430.open("ee-gg_430MeV_NoPipe.txt");

  fSigmaDTheta=2.06452e-03;
  fr=new TRandom2();
  gRandom->SetSeed(time(NULL));

  return true;
}


Bool_t CalchepTruthStudies::Process()
{

  Int_t NClECal = fECal_ClColl->GetNElements();

  fIdCl_SortByEnergy.clear();
  SortCluster_byEnergy();
  
  if( NClECal != fIdCl_SortByEnergy.size()){
    std::cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!         error on sorted energy        !!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    return true;
  }

  getCalchepTruth();

  fcountEvent ++;
  //std::cout<<"event "  << fcountEvent << std::endl;
  return true;
}


Bool_t CalchepTruthStudies::InitHistos()
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  int binX=5000;
  double minX=0;
  double maxX=3000;
  int binY=5000;
  double minY=0;
  double maxY=3000;

  binX=360;
  minX=0.;
  maxX=360;
  hname="CalchepVar_Phi1_g1inFR";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="CalchepVar_Phi2_g1inFR";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="CalchepVar_Phi1Lorentz_g1inFR";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="CalchepVar_Phi2Lorentz_g1inFR";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="CalchepVar_Phi1_g1inFR_AlongY";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="CalchepVar_Phi2_g1inFR_AlongY";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="CalchepVar_Phi1_g1inFR_AlongX";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="CalchepVar_Phi2_g1inFR_AlongX";
  hSvc->BookHisto(hname, binX, minX, maxX);
  binX=720;
  minX=-360;
  maxX=360;
  hname="CalchepVar_MAP_g1inFR";
  hSvc->BookHisto2(hname, binX, minX, maxX,binX, minX, maxX);

  binX=500;
  minX=0.;
  maxX=500;
  std::vector<std::string> sufix;
  sufix.push_back("CalchepVar");
  sufix.push_back("CalchepVar_g1inFR");
  sufix.push_back("CalchepVar_g1inFR_PhiCut");
  sufix.push_back("CalchepVar_g1g2inFR");
  sufix.push_back("CalchepVar_Ncl2");
  sufix.push_back("CalchepSim_Ncl2");
  sufix.push_back("CalchepVar_Ncl2_g1inFR");
  sufix.push_back("CalchepSim_Ncl2_g1inFR");
  sufix.push_back("CalchepVar_Ncl2_g1inFR_PhiCut");
  sufix.push_back("CalchepSim_Ncl2_g1inFR_PhiCut");
  sufix.push_back("CalchepVar_Ncl2_g1g2inFR");
  sufix.push_back("CalchepSim_Ncl2_g1g2inFR");

  sufix.push_back("CalchepVar_Ncl3");
  sufix.push_back("CalchepSim_Ncl3");
  sufix.push_back("CalchepVar_Ncl3_g1inFR");
  sufix.push_back("CalchepSim_Ncl3_g1inFR");
  sufix.push_back("CalchepVar_g1inFR_AlongY");
  sufix.push_back("CalchepVar_g1g2inFR_AlongY");
  sufix.push_back("CalchepVar_g1inFR_AlongX");
  sufix.push_back("CalchepVar_g1g2inFR_AlongX");

  for(int i=0; i<sufix.size(); i++){ 
    hname="CalchepComparison_R1_"+sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_R2_"+sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_R2vsR1_"+sufix.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX,binX, minX, maxX);
    hname="CalchepComparison_E1_"+sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_E2_"+sufix.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_E1vsR1_"+sufix.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX,binX, minX, maxX);
    hname="CalchepComparison_E2vsR2_"+sufix.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX,binX, minX, maxX);
    hname="CalchepComparison_EvsR_"+sufix.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX,binX, minX, maxX);
  }

  std::vector<std::string> sufixC;
  sufixC.push_back("CalchepSim_Ncl2");
  sufixC.push_back("CalchepSim_Ncl2_g1inFR");
  sufixC.push_back("CalchepSim_Ncl2_g1inFR_PhiCut");
  sufixC.push_back("CalchepSim_Ncl2_g1g2inFR");
  for(int i=0; i<sufixC.size(); i++){
    binX=700;
    minX=-500;
    maxX=200;
    hname="CalchepComparison_DR1_"+sufixC.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_DR2_"+sufixC.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_DR2vsDR1_"+sufixC.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX,binX, minX, maxX);
    binX=500;
    minX=0;
    maxX=0.2;
    binY=1000;
    minY=-500;
    maxY=500;
    hname="CalchepComparison_DR1vsg1ThetaTruth_"+sufixC.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX,binY, minY, maxY);
    hname="CalchepComparison_DR2vsg2ThetaTruth_"+sufixC.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX,binY, minY, maxY);
    binX=300;
    minX=0;
    maxX=500.;
    hname="CalchepComparison_R1truthvsR1reco_"+sufixC.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX,binX, minX, maxX);
    hname="CalchepComparison_R2truthvsR2reco_"+sufixC.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX,binX, minX, maxX);


    binX=1000;
    minX=-500;
    maxX=500;
    hname="CalchepComparison_DE1_"+sufixC.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_DE2_"+sufixC.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_DE1vsDR1_"+sufixC.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX,binX, minX, maxX);
    hname="CalchepComparison_DE2vsDR2_"+sufixC.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX,binX, minX, maxX);
    binX=500;
    minX=0;
    maxX=0.2;
    binY=1000;
    minY=-500;
    maxY=500;
    hname="CalchepComparison_DE1vsg1ThetaTruth_"+sufixC.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX,binY, minY, maxY);
    hname="CalchepComparison_DE2vsg2ThetaTruth_"+sufixC.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX,binY, minY, maxY);

  }
  
  std::vector<std::string> sufix3;
  sufix3.push_back("CalchepSim_Ncl3");
  sufix3.push_back("CalchepSim_Ncl3_g1inFR");
  for(int i=0; i<sufix3.size(); i++){
    binX=250;
    minX=0.;
    maxX=500;  
    hname="CalchepComparison_R3_"+sufix3.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_E3_"+sufix3.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    binX=300;
    minX=-300;
    maxX=300;
    hname="CalchepComparison_DR13_"+sufix3.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_DR23_"+sufix3.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_DR23vsDR13_"+sufix3.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX,binX, minX, maxX);
    binX=550;
    minX=0.;
    maxX=550;
    hname="CalchepComparison_ToTE_"+sufix3.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_ToTE12_"+sufix3.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_E3vsR3_"+sufix3.at(i);
    hSvc->BookHisto2(hname, binX, minX, maxX,binX, minX, maxX);
}

  std::vector<std::string> sufixA;
  sufixA.push_back("CalchepVar_AccEff_g1inFR");
  sufixA.push_back("CalchepVar_AccEff_g1g2inFR");
  for(int i=0; i<sufixA.size(); i++){
    binX=500;
    minX=0.01;
    maxX=0.1; 
    hname="CalchepComparison_Th1_"+sufixA.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_Th2_"+sufixA.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_sTh1_"+sufixA.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_sTh2_"+sufixA.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
  }

  hname="CalchepTruth_g1TruthinFR_RecoNCl";
  hSvc->BookHisto(hname, 6, -0.5, 5.5);
  std::vector<std::string> sufixE;
  sufixE.push_back("Truthg1inFR_NCl0");
  sufixE.push_back("Truthg1inFR_NCl1");
  sufixE.push_back("Truthg1inFR_NCl2");
  sufixE.push_back("Truthg1inFR_NCl3");
  sufixE.push_back("truthFR_Truthg1inFR");
  sufixE.push_back("truthFR_Truthg1g2inFR");
  sufixE.push_back("truthFR_Recog1inFR");
  sufixE.push_back("truthFR_Recog1g2inFR");
  sufixE.push_back("Recog1inFR");
  sufixE.push_back("Recog1g2inFR");
  for(int i=0; i<sufixE.size(); i++){
    binX=500;
    minX=0.;
    maxX=500; 
    hname="CalchepComparison_R1_"+sufixE.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_R2_"+sufixE.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_E1_"+sufixE.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_E2_"+sufixE.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
  }

  std::vector<std::string> sufixE1;
  sufixE1.push_back("truthg1_g1inFR");
  sufixE1.push_back("truthg2_g1inFR");
  sufixE1.push_back("truthg1g2_g1inFR");
  sufixE1.push_back("truthg1_g1g2inFR");
  sufixE1.push_back("truthg2_g1g2inFR");
  sufixE1.push_back("truthg1g2_g1g2inFR");
  sufixE1.push_back("truthFR_recog1_g1inFR");
  sufixE1.push_back("truthFR_recog2_g1inFR");
  sufixE1.push_back("truthFR_recog1g2_g1inFR");
  sufixE1.push_back("truthFR_recog1_g1g2inFR");
  sufixE1.push_back("truthFR_recog2_g1g2inFR");
  sufixE1.push_back("truthFR_recog1g2_g1g2inFR");
  sufixE1.push_back("recog1_g1inFR");
  sufixE1.push_back("recog2_g1inFR");
  sufixE1.push_back("recog1g2_g1inFR");
  sufixE1.push_back("recog1_g1g2inFR");
  sufixE1.push_back("recog2_g1g2inFR");
  sufixE1.push_back("recog1g2_g1g2inFR");
  sufixE1.push_back("truthFR_recoAll");
  sufixE1.push_back("recoAll");
  for(int i=0; i<sufixE1.size(); i++){
    binX=500;
    minX=0.;
    maxX=500; 
    hname="CalchepComparison_From0To45_tagInnerR"   +sufixE1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_From0To45_tagOuterR"   +sufixE1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_From45To90_tagInnerR"  +sufixE1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_From45To90_tagOuterR"  +sufixE1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_From90To135_tagInnerR" +sufixE1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_From90To135_tagOuterR" +sufixE1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_From135To180_tagInnerR"+sufixE1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_From135To180_tagOuterR"+sufixE1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_From180To225_tagInnerR"+sufixE1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_From180To225_tagOuterR"+sufixE1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_From225To270_tagInnerR"+sufixE1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_From225To270_tagOuterR"+sufixE1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_From270To315_tagInnerR"+sufixE1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_From270To315_tagOuterR"+sufixE1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_From315To360_tagInnerR"+sufixE1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_From315To360_tagOuterR"+sufixE1.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
}


  std::vector<std::string> sufixE2;
  sufixE2.push_back("truthg1g2_g1inFR");
  for(int i=0; i<sufixE2.size(); i++){
    binX=400;
    minX=0.;
    maxX=400; 
    hname="CalchepComparison_RFrom0To45_tagInnerR"   +sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_PhiFrom0To45_tagInnerR"   +sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_RFrom0To45_tagOuterR"   +sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_PhiFrom0To45_tagOuterR"   +sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_RFrom45To90_tagInnerR"  +sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_PhiFrom45To90_tagInnerR"  +sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_RFrom45To90_tagOuterR"  +sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_PhiFrom45To90_tagOuterR"  +sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_RFrom90To135_tagInnerR" +sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_PhiFrom90To135_tagInnerR" +sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_RFrom90To135_tagOuterR" +sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_PhiFrom90To135_tagOuterR" +sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_RFrom135To180_tagInnerR"+sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_PhiFrom135To180_tagInnerR"+sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_RFrom135To180_tagOuterR"+sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_PhiFrom135To180_tagOuterR"+sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_RFrom180To225_tagInnerR"+sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_PhiFrom180To225_tagInnerR"+sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_RFrom180To225_tagOuterR"+sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_PhiFrom180To225_tagOuterR"+sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_RFrom225To270_tagInnerR"+sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_PhiFrom225To270_tagInnerR"+sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_RFrom225To270_tagOuterR"+sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_PhiFrom225To270_tagOuterR"+sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_RFrom270To315_tagInnerR"+sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_PhiFrom270To315_tagInnerR"+sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_RFrom270To315_tagOuterR"+sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_PhiFrom270To315_tagOuterR"+sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_RFrom315To360_tagInnerR"+sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_PhiFrom315To360_tagInnerR"+sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_RFrom315To360_tagOuterR"+sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_PhiFrom315To360_tagOuterR"+sufixE2.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
}


  return true;
}



void CalchepTruthStudies::SortCluster_byEnergy(){

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


void CalchepTruthStudies::getCalchepTruth(){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  std::string line;
  //std::getline(fCalchep450,line);
  std::getline(fCalchep430,line);
  //std::cout<< line << std::endl;
  std::istringstream iss(line);
  int it;
  double_t dt1, dt2;
  Double_t p1[3]={0.};
  Double_t p2[3]={0.};
  Double_t ptmp[3]={0.};
  Double_t etmp;
  iss>>it>>dt1>>dt2>> p1[0] >>p1[1] >> p1[2] >>  p2[0] >>p2[1] >> p2[2] ;
  //std::cout << "momentum " << p1[0] << "    " << "    " << p1[1] << "    " << p1[2] <<  "    " << p2[0] <<"    " << p2[1] << "    " << p2[2] << std::endl;
  Double_t g1E=sqrt(p1[0]*p1[0]+p1[1]*p1[1]+p1[2]*p1[2])*1000;//to convert in MeV
  Double_t g2E=sqrt(p2[0]*p2[0]+p2[1]*p2[1]+p2[2]*p2[2])*1000;
  if(g1E<g2E){
    ptmp[0]=-p1[0];
    ptmp[1]=-p1[1];
    ptmp[2]=-p1[2];
    p1[0]=-p2[0];
    p1[1]=-p2[1];
    p1[2]=-p2[2];
    p2[0]=ptmp[0];
    p2[1]=ptmp[1];
    p2[2]=ptmp[2];
    etmp=g1E;
    g1E=g2E;
    g2E=etmp;
  }
  else{
    p1[0]=-p1[0];
    p1[1]=-p1[1];
    p1[2]=-p1[2];
    p2[0]=-p2[0];
    p2[1]=-p2[1];
    p2[2]=-p2[2];
  }
  Double_t PTransverse1 = sqrt(p1[0]*p1[0]+p1[1]*p1[1]);
  Double_t PTransverse2 = sqrt(p2[0]*p2[0]+p2[1]*p2[1]);
  Double_t shiftEcal=sqrt(3.13*3.13+3.86*3.86);
  //Double_t g1Recal=PTransverse1*fdistanceTarget/fabs(p1[2]);
  //Double_t g2Recal=PTransverse2*fdistanceTarget/fabs(p2[2]);
  Double_t g1Recal=PTransverse1*fdistanceTarget/p1[2];
  Double_t g2Recal=PTransverse2*fdistanceTarget/p2[2];

  FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar");

  Double_t g1phi=TMath::ATan2(p1[1], p1[0])*360./2./TMath::Pi();
  Double_t g2phi=TMath::ATan2(p2[1], p2[0])*360./2./TMath::Pi();
  Double_t x1= g1Recal*cos(g1phi);
  Double_t y1= g1Recal*sin(g1phi);
  Double_t x2= g2Recal*cos(g2phi);
  Double_t y2= g2Recal*sin(g2phi);
  //std::cout<<"g1, atan2 " << TMath::ATan2(p1[1], p1[0]) << " in deg " << g1phi << std::endl;
  TLorentzVector P4g1F, P4g2F;
  P4g1F.SetPxPyPzE(p1[0],p1[1],p1[2],g1E);
  P4g2F.SetPxPyPzE(p2[0],p2[1],p2[2],g2E);
  Double_t g1phiL = P4g1F.Phi()*360./(2.*TMath::Pi());
  Double_t g2phiL = P4g2F.Phi()*360./(2.*TMath::Pi());
  if(g1phi<0.){g1phi=360+g1phi;  }//std::cout<<"g1<0, so after correction " <<  g1phi << std::endl;}
  if(g2phi<0.)g2phi=360+g2phi;
  if(g1phiL<0.)g1phiL=360+g1phiL;
  if(g2phiL<0.)g2phiL=360+g2phiL;

  if(g1Recal>fFRmin && g1Recal<258){
    hname="CalchepVar_Phi1_g1inFR";
    hSvc->FillHisto(hname, g1phi, 1.);
    hname="CalchepVar_Phi2_g1inFR";
    hSvc->FillHisto(hname, g2phi, 1.);
    hname="CalchepVar_Phi1Lorentz_g1inFR";
    hSvc->FillHisto(hname, g1phi, 1.);
    hname="CalchepVar_Phi2Lorentz_g1inFR";
    hSvc->FillHisto(hname, g2phi, 1.);
    hname="CalchepVar_MAP_g1inFR";
    hSvc->FillHisto2(hname, x1, y1, 1.);
    hSvc->FillHisto2(hname, x2, y2, 1.);
    if(fabs(g1phi-90)<45 || fabs(g1phi-270)<45){
      FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_g1inFR_AlongY");
      hname="CalchepVar_Phi1_g1inFR_AlongY";
      hSvc->FillHisto(hname, g1phi, 1.);
      hname="CalchepVar_Phi2_g1inFR_AlongY";
      hSvc->FillHisto(hname, g2phi, 1.);
      if(g2Recal>fFRmin && g2Recal<258)FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_g1g2inFR_AlongY");
    }
    else {
      hname="CalchepVar_Phi1_g1inFR_AlongX";
      hSvc->FillHisto(hname, g1phi, 1.);
      hname="CalchepVar_Phi2_g1inFR_AlongX";
      hSvc->FillHisto(hname, g2phi, 1.);
      FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_g1inFR_AlongX");
      if(g2Recal>fFRmin && g2Recal<258)FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_g1g2inFR_AlongX");
    }

    FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_g1inFR");
    if(fabs(g1phi)<45. || fabs(g1phi-180)< 45. ) FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_g1inFR_PhiCut");
    if(g2Recal>fFRmin && g2Recal<258){
      FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_g1g2inFR");
    }
  }

  FillCalchepSmearedHisto(g1Recal,g2Recal);
  //if(g1phi<0.)g1phi=360+g1phi;
  //if(g2phi<0.)g2phi=360+g2phi;
  extractEfficiency(g1Recal, g1E,g1phi,g2Recal,g2E,g2phi);
  CountersExtraction(g1Recal, g1phi, g2Recal, g2phi);

  TRecoVCluster* ecalclu=NULL;
  TRecoVCluster* ecalclu2=NULL;
  TRecoVCluster* ecalclu3=NULL;

  if(fECal_ClColl->GetNElements()==0)return;
  ecalclu          = fECal_ClColl->Element(fIdCl_SortByEnergy.at(0));
  Double_t cl1E = ecalclu->GetEnergy();
  Double_t cl1x = ecalclu->GetPosition().X();
  Double_t cl1y = ecalclu->GetPosition().Y();
  Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
  Double_t phi1=TMath::ATan2(cl1y, cl1x)*360./2./TMath::Pi();
  if(phi1<0.)phi1=360+phi1;

  if(fECal_ClColl->GetNElements()==2){
    ecalclu2          = fECal_ClColl->Element(fIdCl_SortByEnergy.at(1));
    Double_t cl2E = ecalclu2->GetEnergy();
    Double_t cl2x = ecalclu2->GetPosition().X();
    Double_t cl2y = ecalclu2->GetPosition().Y();
    Double_t cl2r=sqrt(cl2x*cl2x+cl2y*cl2y);
    Double_t phi2=TMath::ATan2(cl2y, cl2x)*360/2/TMath::Pi();
    if(phi2<0.)phi2=360+phi2;

    FillGeneralHisto(cl1r, cl1E,cl2r,cl2E, "CalchepSim_Ncl2");
    FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_Ncl2");
    FillComparisonHisto(g1Recal, g1E,g2Recal,g2E,cl1r, cl1E,cl2r,cl2E, "CalchepSim_Ncl2");

    if(cl1r>fFRmin && cl1r<258){
      FillGeneralHisto(cl1r, cl1E,cl2r,cl2E, "CalchepSim_Ncl2_g1inFR");
      FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_Ncl2_g1inFR");
      FillComparisonHisto(g1Recal, g1E,g2Recal,g2E,cl1r, cl1E,cl2r,cl2E, "CalchepSim_Ncl2_g1inFR");

      if(fabs(phi1)<45. || fabs(phi1-180)< 45. ){
	FillGeneralHisto(cl1r, cl1E,cl2r,cl2E, "CalchepSim_Ncl2_g1inFR_PhiCut");
	FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_Ncl2_g1inFR_PhiCut");
	FillComparisonHisto(g1Recal, g1E,g2Recal,g2E,cl1r, cl1E,cl2r,cl2E, "CalchepSim_Ncl2_g1inFR_PhiCut");
      }
    }
    if(cl1r>fFRmin && cl1r<258 && cl2r>fFRmin && cl2r<258){
      FillGeneralHisto(cl1r, cl1E,cl2r,cl2E, "CalchepSim_Ncl2_g1g2inFR");
      FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_Ncl2_g1g2inFR");
      FillComparisonHisto(g1Recal, g1E,g2Recal,g2E,cl1r, cl1E,cl2r,cl2E, "CalchepSim_Ncl2_g1g2inFR");

    }  
  }//end of if ncl==2


  if(fECal_ClColl->GetNElements()==3){
    ecalclu2          = fECal_ClColl->Element(fIdCl_SortByEnergy.at(1));
    Double_t cl2E = ecalclu2->GetEnergy();
    Double_t cl2x = ecalclu2->GetPosition().X();
    Double_t cl2y = ecalclu2->GetPosition().Y();
    Double_t cl2r=sqrt(cl2x*cl2x+cl2y*cl2y);
    Double_t phi2=TMath::ATan2(cl2y, cl2x)*360/2/TMath::Pi();
    if(phi2<0.)phi2=360+phi2;
    ecalclu3          = fECal_ClColl->Element(fIdCl_SortByEnergy.at(2));
    Double_t cl3E = ecalclu3->GetEnergy();
    Double_t cl3x = ecalclu3->GetPosition().X();
    Double_t cl3y = ecalclu3->GetPosition().Y();
    Double_t cl3r=sqrt(cl3x*cl3x+cl3y*cl3y);
    Double_t phi3=TMath::ATan2(cl3y, cl3x)*360/2/TMath::Pi();
    if(phi3<0.)phi3=360+phi3;
    //+ comparison g1 truth g1 reco, g2 truth g2 reco if cl1 in FR e.g.
    FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_Ncl3");
    FillGeneralHisto(cl1r, cl1E,cl2r,cl2E, "CalchepSim_Ncl3");
    FillHistoNCl3(cl1r, cl1E,cl2r, cl2E,cl3r, cl3E, "CalchepSim_Ncl3");
    if(cl1r>fFRmin && cl1r<258){
      FillHistoNCl3(cl1r, cl1E,cl2r, cl2E,cl3r, cl3E, "CalchepSim_Ncl3_g1inFR");
      FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_Ncl3_g1inFR");
      FillGeneralHisto(cl1r, cl1E,cl2r,cl2E, "CalchepSim_Ncl3_g1inFR");
    }
  }//end of if ncl==3
  
}


void CalchepTruthStudies::FillGeneralHisto(double R1, double E1,double R2, double E2, std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  hname="CalchepComparison_R1_"+sufix;
  hSvc->FillHisto(hname, R1,1.);
  hname="CalchepComparison_R2_"+sufix;
  hSvc->FillHisto(hname, R2,1.);
  hname="CalchepComparison_R2vsR1_"+sufix;
  hSvc->FillHisto2(hname, R1, R2, 1.);
  hname="CalchepComparison_E1_"+sufix;
  hSvc->FillHisto(hname, E1,1.);
  hname="CalchepComparison_E2_"+sufix;
  hSvc->FillHisto(hname, E2,1.);
  hname="CalchepComparison_E1vsR1_"+sufix;
  hSvc->FillHisto2(hname, R1, E1, 1.);
  hname="CalchepComparison_E2vsR2_"+sufix;
  hSvc->FillHisto2(hname, R2, E2, 1.);
  hname="CalchepComparison_EvsR_"+sufix;
  hSvc->FillHisto2(hname, R1, E1, 1.);
  hSvc->FillHisto2(hname, R2, E2, 1.);
}


void CalchepTruthStudies::FillCalchepSmearedHisto(double R1, double R2){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

  Double_t g1Th=atan(R1/fdistanceTarget);  
  Double_t g2Th=atan(R2/fdistanceTarget);
  Double_t dTh1=fr->Gaus(0., fSigmaDTheta);
  Double_t dTh2=fr->Gaus(0., fSigmaDTheta);
  Double_t sTh1=g1Th+dTh1;
  Double_t sTh2=g2Th+dTh2;
  std::string sufix;
  if(R1>fFRmin && R1<258){
    sufix="CalchepVar_AccEff_g1inFR";
    hname="CalchepComparison_Th1_"+sufix;
    hSvc->FillHisto(hname, g1Th,1.);
    hname="CalchepComparison_Th2_"+sufix;
    hSvc->FillHisto(hname, g2Th,1.);
   if(R2>fFRmin && R2<258){
      sufix="CalchepVar_AccEff_g1g2inFR";
      hname="CalchepComparison_Th1_"+sufix;
      hSvc->FillHisto(hname, g1Th,1.);
      hname="CalchepComparison_Th2_"+sufix;
      hSvc->FillHisto(hname, g2Th,1.);
   }
  }
  Double_t sR1=tan(sTh1)*fdistanceTarget;
  Double_t sR2=tan(sTh2)*fdistanceTarget;
  if(sR1>fFRmin && sR1<258){
    sufix="CalchepVar_AccEff_g1inFR";
    hname="CalchepComparison_sTh1_"+sufix;
    hSvc->FillHisto(hname, sTh1,1.);
    hname="CalchepComparison_sTh2_"+sufix;
    hSvc->FillHisto(hname, sTh2,1.);
    if(sR2>fFRmin && sR2<258){
      sufix="CalchepVar_AccEff_g1g2inFR";
      hname="CalchepComparison_sTh1_"+sufix;
      hSvc->FillHisto(hname, sTh1,1.);
      hname="CalchepComparison_sTh2_"+sufix;
      hSvc->FillHisto(hname, sTh2,1.);
    }
  }

}


void CalchepTruthStudies::FillComparisonHisto(double R1, double E1,double R2, double E2,double R1reco, double E1reco,double R2reco, double E2reco, std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  double_t dR1=R1reco-R1;
  double_t dR2=R2reco-R2;
  double_t dE1=E1reco-E1;
  double_t dE2=E2reco-E2;
  Double_t g1Th=atan(R1/fdistanceTarget);  
  Double_t g2Th=atan(R2/fdistanceTarget);  
  Double_t g1ThReco=atan(R1reco/fdistanceTarget);  
  Double_t g2ThReco=atan(R2reco/fdistanceTarget);  

  hname="CalchepComparison_DR1_"+sufix;
  hSvc->FillHisto(hname, dR1,1.);
  hname="CalchepComparison_DR2_"+sufix;
  hSvc->FillHisto(hname, dR2,1.);
  hname="CalchepComparison_DR2vsDR1_"+sufix;
  hSvc->FillHisto2(hname, dR1, dR2, 1.);
  hname="CalchepComparison_DR1vsg1ThetaTruth_"+sufix;
  hSvc->FillHisto2(hname,  g1Th,dR1, 1.);
  hname="CalchepComparison_DR2vsg2ThetaTruth_"+sufix;
  hSvc->FillHisto2(hname,  g2Th,dR2, 1.);
  hname="CalchepComparison_R1truthvsR1reco_"+sufix;
  hSvc->FillHisto2(hname,  R1reco,R1, 1.);
  hname="CalchepComparison_R2truthvsR2reco_"+sufix;
  hSvc->FillHisto2(hname, R2reco,R2,  1.);

  hname="CalchepComparison_DE1_"+sufix;
  hSvc->FillHisto(hname, dR1,1.);
  hname="CalchepComparison_DE2_"+sufix;
  hSvc->FillHisto(hname, dR2,1.);
  hname="CalchepComparison_DE1vsDR1_"+sufix;
  hSvc->FillHisto2(hname, dR1, dE1, 1.);
  hname="CalchepComparison_DE2vsDR2_"+sufix;
  hSvc->FillHisto2(hname, dR2, dE2, 1.);
  hname="CalchepComparison_DE1vsg1ThetaTruth_"+sufix;
  hSvc->FillHisto2(hname, g1Th,dE1, 1.);
  hname="CalchepComparison_DE2vsg2ThetaTruth_"+sufix;
  hSvc->FillHisto2(hname, g2Th,dE2, 1.);
}


void CalchepTruthStudies::FillHistoNCl3(double R1, double E1,double R2, double E2,double R3, double E3, std::string sufix){
    //i'd like to study: dRcl1-cl3, cl2-cl3;   E cl1+cl2+cl3;  EvsR cl3,     

  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Double_t dR13=R1-R3;
  Double_t dR23=R2-R3;
  Double_t sumE=E1+E2+E3;

  hname="CalchepComparison_R1_"+sufix;
  hSvc->FillHisto(hname, R1,1.);
  hname="CalchepComparison_E1_"+sufix;
  hSvc->FillHisto(hname, E1,1.);
  hname="CalchepComparison_R2_"+sufix;
  hSvc->FillHisto(hname, R2,1.);
  hname="CalchepComparison_E2_"+sufix;
  hSvc->FillHisto(hname, E2,1.);
  hname="CalchepComparison_R3_"+sufix;
  hSvc->FillHisto(hname, R3,1.);
  hname="CalchepComparison_E3_"+sufix;
  hSvc->FillHisto(hname, E3,1.);
  hname="CalchepComparison_DR13_"+sufix;
  hSvc->FillHisto(hname, dR13,1.);
  hname="CalchepComparison_DR23_"+sufix;
  hSvc->FillHisto(hname, dR23,1.);
  hname="CalchepComparison_DR23vsDR13_"+sufix;
  hSvc->FillHisto2(hname, dR13, dR23, 1.);
  hname="CalchepComparison_ToTE_"+sufix;
  hSvc->FillHisto(hname, sumE,1.);
  hname="CalchepComparison_ToTE12_"+sufix;
  hSvc->FillHisto(hname, E1+E2,1.);
  hname="CalchepComparison_E3vsR3_"+sufix;
  hSvc->FillHisto2(hname, R3, E3, 1.);

}


void CalchepTruthStudies::extractEfficiency(Double_t rg1T, Double_t eg1T,Double_t phig1, Double_t rg2T, Double_t eg2T, Double_t phig2){
  
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

  if(rg1T>fFRmin && rg1T<fFRmax){
    hname="CalchepTruth_g1TruthinFR_RecoNCl";
    hSvc->FillHisto(hname, fECal_ClColl->GetNElements(), 1.);
    FillPhiRHistograms(rg1T, eg1T,phig1, "truthg1_g1inFR");
    FillPhiRHistograms_noFR(rg2T, eg2T,phig2, "truthg2_g1inFR");

    FillPhiRHistograms(rg1T, eg1T,phig1, "truthg1g2_g1inFR");
    FillPhiRHistograms_noFR(rg2T, eg2T,phig2, "truthg1g2_g1inFR");

    FillPhiRVariablesHistograms_noFR(rg1T,phig1, "truthg1g2_g1inFR");//i remember you that g1 is in FR for the preliminary selection
    FillPhiRVariablesHistograms_noFR(rg2T,phig2, "truthg1g2_g1inFR");
    
    FillGeneralHistoEff(rg1T, eg1T,rg2T, eg2T,"truthFR_Truthg1inFR");
    if(rg2T>fFRmin && rg2T<fFRmax){
      FillPhiRHistograms(rg1T, eg1T,phig1, "truthg1_g1g2inFR");
      FillPhiRHistograms(rg2T, eg2T,phig2, "truthg2_g1g2inFR");

      FillPhiRHistograms(rg1T, eg1T,phig1, "truthg1g2_g1g2inFR");
      FillPhiRHistograms(rg2T, eg2T,phig2, "truthg1g2_g1g2inFR");

      FillGeneralHistoEff(rg1T, eg1T,rg2T, eg2T,"truthFR_Truthg1g2inFR");
    }

    if(fECal_ClColl->GetNElements()==0){
      FillGeneralHistoEff(rg1T,eg1T,rg2T, eg2T,"Truthg1inFR_NCl0");
    }
    if(fECal_ClColl->GetNElements()==1){
      FillGeneralHistoEff(rg1T,eg1T,rg2T, eg2T,"Truthg1inFR_NCl1");
    }
    if(fECal_ClColl->GetNElements()==2){
      FillGeneralHistoEff(rg1T,eg1T,rg2T, eg2T,"Truthg1inFR_NCl2");
    }
    if(fECal_ClColl->GetNElements()==3){
      FillGeneralHistoEff(rg1T,eg1T,rg2T, eg2T,"Truthg1inFR_NCl3");
    }

    if(fECal_ClColl->GetNElements()>1){
      TRecoVCluster* ecalclu =NULL;
      TRecoVCluster* ecalclu2=NULL;
      ecalclu       = fECal_ClColl->Element(fIdCl_SortByEnergy.at(0));
      Double_t cl1E = ecalclu->GetEnergy();
      Double_t cl1x = ecalclu->GetPosition().X();
      Double_t cl1y = ecalclu->GetPosition().Y();
      Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
      Double_t phi1=TMath::ATan2(cl1y, cl1x)*360./2./TMath::Pi();
      if(phi1<0.)phi1=360.+phi1;
      ecalclu2      = fECal_ClColl->Element(fIdCl_SortByEnergy.at(1));
      Double_t cl2E = ecalclu2->GetEnergy();
      Double_t cl2x = ecalclu2->GetPosition().X();
      Double_t cl2y = ecalclu2->GetPosition().Y();
      Double_t cl2r=sqrt(cl2x*cl2x+cl2y*cl2y);
      Double_t phi2=TMath::ATan2(cl2y, cl2x)*360./2./TMath::Pi();
      if(phi2<0.)phi2=360.+phi2;

      if(cl1r>fFRmin && cl1r<fFRmax){
	FillPhiRHistograms(cl1r, cl1E, phi1,  "truthFR_recog1_g1inFR");
	FillPhiRHistograms_noFR(cl2r, cl2E, phi2,  "truthFR_recog2_g1inFR");

	FillPhiRHistograms(cl1r, cl1E, phi1,  "truthFR_recog1g2_g1inFR");
	FillPhiRHistograms_noFR(cl2r, cl2E, phi2,  "truthFR_recog1g2_g1inFR");

	FillGeneralHistoEff(cl1r,cl1E,cl2r, cl2E,"truthFR_Recog1inFR");
	if(cl2r>fFRmin && cl2r<fFRmax){
	  FillPhiRHistograms(cl1r, cl1E, phi1,  "truthFR_recog1_g1g2inFR");
	  FillPhiRHistograms(cl2r, cl2E, phi2,  "truthFR_recog2_g1g2inFR");

	  FillPhiRHistograms(cl1r, cl1E, phi1,  "truthFR_recog1g2_g1g2inFR");
	  FillPhiRHistograms(cl2r, cl2E, phi2,  "truthFR_recog1g2_g1g2inFR");

	  FillGeneralHistoEff(cl1r,cl1E,cl2r, cl2E,"truthFR_Recog1g2inFR");
	}
      }
    }//end of if nCl>1
    TRecoVCluster* ecalclu =NULL;
    for(int i=0; i< fECal_ClColl->GetNElements(); i++){
      ecalclu       = fECal_ClColl->Element(fIdCl_SortByEnergy.at(i));
      Double_t cl1E = ecalclu->GetEnergy();
      Double_t cl1x = ecalclu->GetPosition().X();
      Double_t cl1y = ecalclu->GetPosition().Y();
      Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
      Double_t phi1=TMath::ATan2(cl1y, cl1x)*360./2./TMath::Pi();
      if(phi1<0.)phi1=360.+phi1;
      FillPhiRHistograms(cl1r, cl1E, phi1, "truthFR_recoAll");
    }//end pf loop on all clusters
  }//end of truth in fr

  TRecoVCluster* ecalclu =NULL;
    for(int i=0; i< fECal_ClColl->GetNElements(); i++){
      ecalclu       = fECal_ClColl->Element(fIdCl_SortByEnergy.at(i));
      Double_t cl1E = ecalclu->GetEnergy();
      Double_t cl1x = ecalclu->GetPosition().X();
      Double_t cl1y = ecalclu->GetPosition().Y();
      Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
      Double_t phi1=TMath::ATan2(cl1y, cl1x)*360./2./TMath::Pi();
      if(phi1<0.)phi1=360.+phi1;
      FillPhiRHistograms(cl1r, cl1E, phi1, "recoAll");
    }//end pf loop on all clusters

  if(fECal_ClColl->GetNElements()>1){
    TRecoVCluster* ecalclu =NULL;
    TRecoVCluster* ecalclu2=NULL;
    ecalclu       = fECal_ClColl->Element(fIdCl_SortByEnergy.at(0));
    Double_t cl1E = ecalclu->GetEnergy();
    Double_t cl1x = ecalclu->GetPosition().X();
    Double_t cl1y = ecalclu->GetPosition().Y();
    Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
    Double_t phi1=TMath::ATan2(cl1y, cl1x)*360./2./TMath::Pi();
    if(phi1<0.)phi1=360.+phi1;
    ecalclu2      = fECal_ClColl->Element(fIdCl_SortByEnergy.at(1));
    Double_t cl2E = ecalclu2->GetEnergy();
    Double_t cl2x = ecalclu2->GetPosition().X();
    Double_t cl2y = ecalclu2->GetPosition().Y();
    Double_t cl2r=sqrt(cl2x*cl2x+cl2y*cl2y);
    Double_t phi2=TMath::ATan2(cl2y, cl2x)*360./2./TMath::Pi();
    if(phi2<0.)phi2=360.+phi2;

    if(cl1r>fFRmin && cl1r<fFRmax){
      FillPhiRHistograms(cl1r, cl1E, phi1,  "recog1_g1inFR");
      FillPhiRHistograms_noFR(cl2r, cl2E, phi2,  "recog2_g1inFR");

      FillPhiRHistograms(cl1r, cl1E, phi1,  "recog1g2_g1inFR");
      FillPhiRHistograms_noFR(cl2r, cl2E, phi2,  "recog1g2_g1inFR");

      FillGeneralHistoEff(cl1r,cl1E,cl2r, cl2E,"Recog1inFR");
      if(cl2r>fFRmin && cl2r<fFRmax){
	FillPhiRHistograms(cl1r, cl1E, phi1,  "recog1_g1g2inFR");
	FillPhiRHistograms(cl2r, cl2E, phi2,  "recog2_g1g2inFR");

	FillPhiRHistograms(cl1r, cl1E, phi1,  "recog1g2_g1g2inFR");
	FillPhiRHistograms(cl2r, cl2E, phi2,  "recog1g2_g1g2inFR");

	FillGeneralHistoEff(cl1r,cl1E,cl2r, cl2E,"Recog1g2inFR");
      }
    }
  }//end of if nCl>1
}


void CalchepTruthStudies::FillGeneralHistoEff(Double_t rg1, Double_t eg1, Double_t rg2, Double_t eg2, std::string sufix){
  
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  hname="CalchepComparison_R1_"+sufix;
  hSvc->FillHisto(hname, rg1, 1.);
  hname="CalchepComparison_R2_"+sufix;
  hSvc->FillHisto(hname, rg2, 1.);
  hname="CalchepComparison_E1_"+sufix;
  hSvc->FillHisto(hname, eg1, 1.);
  hname="CalchepComparison_E2_"+sufix;
  hSvc->FillHisto(hname, eg2, 1.);
}


//void CalchepTruthStudies::FillGeneralHistoRecoEff_1cl(Double_t rg1, Double_t eg1, Double_t rg2, Double_t eg2, std::string sufix){
//
//  HistoSvc* hSvc =  HistoSvc::GetInstance();
//  std::string hname;
//
//  TRecoVCluster* ecalclu=NULL;
//
//  ecalclu       = fECal_ClColl->Element(fIdCl_SortByEnergy.at(0));
//  Double_t cl1E = ecalclu->GetEnergy();
//  Double_t cl1x = ecalclu->GetPosition().X();
//  Double_t cl1y = ecalclu->GetPosition().Y();
//  Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
//  Double_t phi1=TMath::ATan2(cl1y, cl1x)*360/2/TMath::Pi();
//
//  hname="CalchepComparison_R1reco_1cl"+sufix;
//  hSvc->FillHisto(hname, cl1r, 1.);
//  hname="CalchepComparison_E1reco_1cl"+sufix;
//  hSvc->FillHisto(hname, cl1E, 1.);
//
//
//}


void CalchepTruthStudies::FillPhiRHistograms(Double_t radius, Double_t energy, Double_t phi, std::string sufix){
 
  //mid= 172.83mm   from calchep truth
 HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  if(phi>=0. && phi<45.){
    if(radius>fFRmin && radius<fFRmid){
      hname="CalchepComparison_From0To45_tagInnerR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="CalchepComparison_From0To45_tagOuterR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
  }
  else if(phi>=45. && phi<90.){
    if(radius>fFRmin && radius<fFRmid){
      hname="CalchepComparison_From45To90_tagInnerR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="CalchepComparison_From45To90_tagOuterR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
  }
  else if(phi>=90. && phi<135.){
    if(radius>fFRmin && radius<fFRmid){
      hname="CalchepComparison_From90To135_tagInnerR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="CalchepComparison_From90To135_tagOuterR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
  }
  else if(phi>=135. && phi<180.){
    if(radius>fFRmin && radius<fFRmid){
      hname="CalchepComparison_From135To180_tagInnerR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="CalchepComparison_From135To180_tagOuterR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
  }
  else if(phi>=180. && phi<225.){
    if(radius>fFRmin && radius<fFRmid){
      hname="CalchepComparison_From180To225_tagInnerR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="CalchepComparison_From180To225_tagOuterR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
  }
  else if(phi>=225. && phi<270.){
    if(radius>fFRmin && radius<fFRmid){
      hname="CalchepComparison_From225To270_tagInnerR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="CalchepComparison_From225To270_tagOuterR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
  }
  else if(phi>=270. && phi<315.){
    if(radius>fFRmin && radius<fFRmid){
      hname="CalchepComparison_From270To315_tagInnerR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="CalchepComparison_From270To315_tagOuterR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
  }
  else if(phi>=315. && phi<360.){
    if(radius>fFRmin && radius<fFRmid){
      hname="CalchepComparison_From315To360_tagInnerR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="CalchepComparison_From315To360_tagOuterR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
  }


}



void CalchepTruthStudies::FillPhiRHistograms_noFR(Double_t radius, Double_t energy, Double_t phi, std::string sufix){
 
  //mid= 172.83mm   from calchep truth
 HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  if(phi>=0. && phi<45.){
    if( radius<fFRmid){
      hname="CalchepComparison_From0To45_tagInnerR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
    else if(radius>=fFRmid){
      hname="CalchepComparison_From0To45_tagOuterR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
  }
  else if(phi>=45. && phi<90.){
    if( radius<fFRmid){
      hname="CalchepComparison_From45To90_tagInnerR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
    else if(radius>=fFRmid){
      hname="CalchepComparison_From45To90_tagOuterR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
  }
  else if(phi>=90. && phi<135.){
    if( radius<fFRmid){
      hname="CalchepComparison_From90To135_tagInnerR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
    else if(radius>=fFRmid){
      hname="CalchepComparison_From90To135_tagOuterR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
  }
  else if(phi>=135. && phi<180.){
    if( radius<fFRmid){
      hname="CalchepComparison_From135To180_tagInnerR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
    else if(radius>=fFRmid){
      hname="CalchepComparison_From135To180_tagOuterR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
  }
  else if(phi>=180. && phi<225.){
    if( radius<fFRmid){
      hname="CalchepComparison_From180To225_tagInnerR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
    else if(radius>=fFRmid){
      hname="CalchepComparison_From180To225_tagOuterR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
  }
  else if(phi>=225. && phi<270.){
    if( radius<fFRmid){
      hname="CalchepComparison_From225To270_tagInnerR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
    else if(radius>=fFRmid){
      hname="CalchepComparison_From225To270_tagOuterR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
  }
  else if(phi>=270. && phi<315.){
    if( radius<fFRmid){
      hname="CalchepComparison_From270To315_tagInnerR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
    else if(radius>=fFRmid){
      hname="CalchepComparison_From270To315_tagOuterR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
  }
  else if(phi>=315. && phi<360.){
    if( radius<fFRmid){
      hname="CalchepComparison_From315To360_tagInnerR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
    else if(radius>=fFRmid){
      hname="CalchepComparison_From315To360_tagOuterR"+sufix;
      hSvc->FillHisto(hname,energy, 1. );
    }
  }


}





void CalchepTruthStudies::FillPhiRVariablesHistograms_noFR(Double_t radius, Double_t phi, std::string sufix){
 
  //mid= 172.83mm   from calchep truth
 HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  if(phi>=0. && phi<45.){
    if( radius<fFRmid){
      hname="CalchepComparison_RFrom0To45_tagInnerR"+sufix;
      hSvc->FillHisto(hname,radius, 1. );
      hname="CalchepComparison_PhiFrom0To45_tagInnerR"+sufix;
      hSvc->FillHisto(hname,phi, 1. );
    }
    else if(radius>=fFRmid){
      hname="CalchepComparison_RFrom0To45_tagOuterR"+sufix;
      hSvc->FillHisto(hname,radius, 1. );
      hname="CalchepComparison_PhiFrom0To45_tagOuterR"+sufix;
      hSvc->FillHisto(hname,phi, 1. );
    }
  }
  else if(phi>=45. && phi<90.){
    if( radius<fFRmid){
      hname="CalchepComparison_RFrom45To90_tagInnerR"+sufix;
      hSvc->FillHisto(hname,radius, 1. );
      hname="CalchepComparison_PhiFrom45To90_tagInnerR"+sufix;
      hSvc->FillHisto(hname,phi, 1. );
    }
    else if(radius>=fFRmid){
      hname="CalchepComparison_RFrom45To90_tagOuterR"+sufix;
      hSvc->FillHisto(hname,radius, 1. );
      hname="CalchepComparison_PhiFrom45To90_tagOuterR"+sufix;
      hSvc->FillHisto(hname,phi, 1. );
    }
  }
  else if(phi>=90. && phi<135.){
    if( radius<fFRmid){
      hname="CalchepComparison_RFrom90To135_tagInnerR"+sufix;
      hSvc->FillHisto(hname,radius, 1. );
      hname="CalchepComparison_PhiFrom90To135_tagInnerR"+sufix;
      hSvc->FillHisto(hname,phi, 1. );
    }
    else if(radius>=fFRmid){
      hname="CalchepComparison_RFrom90To135_tagOuterR"+sufix;
      hSvc->FillHisto(hname,radius, 1. );
      hname="CalchepComparison_PhiFrom90To135_tagOuterR"+sufix;
      hSvc->FillHisto(hname,phi, 1. );
    }
  }
  else if(phi>=135. && phi<180.){
    if( radius<fFRmid){
      hname="CalchepComparison_RFrom135To180_tagInnerR"+sufix;
      hSvc->FillHisto(hname,radius, 1. );
      hname="CalchepComparison_PhiFrom135To180_tagInnerR"+sufix;
      hSvc->FillHisto(hname,phi, 1. );
    }
    else if(radius>=fFRmid){
      hname="CalchepComparison_RFrom135To180_tagOuterR"+sufix;
      hSvc->FillHisto(hname,radius, 1. );
      hname="CalchepComparison_PhiFrom135To180_tagOuterR"+sufix;
      hSvc->FillHisto(hname,phi, 1. );
    }
  }
  else if(phi>=180. && phi<225.){
    if( radius<fFRmid){
      hname="CalchepComparison_RFrom180To225_tagInnerR"+sufix;
      hSvc->FillHisto(hname,radius, 1. );
      hname="CalchepComparison_PhiFrom180To225_tagInnerR"+sufix;
      hSvc->FillHisto(hname,phi, 1. );
    }
    else if(radius>=fFRmid){
      hname="CalchepComparison_RFrom180To225_tagOuterR"+sufix;
      hSvc->FillHisto(hname,radius, 1. );
      hname="CalchepComparison_PhiFrom180To225_tagOuterR"+sufix;
      hSvc->FillHisto(hname,phi, 1. );
    }
  }
  else if(phi>=225. && phi<270.){
    if( radius<fFRmid){
      hname="CalchepComparison_RFrom225To270_tagInnerR"+sufix;
      hSvc->FillHisto(hname,radius, 1. );
      hname="CalchepComparison_PhiFrom225To270_tagInnerR"+sufix;
      hSvc->FillHisto(hname,phi, 1. );
    }
    else if(radius>=fFRmid){
      hname="CalchepComparison_RFrom225To270_tagOuterR"+sufix;
      hSvc->FillHisto(hname,radius, 1. );
      hname="CalchepComparison_PhiFrom225To270_tagOuterR"+sufix;
      hSvc->FillHisto(hname,phi, 1. );
    }
  }
  else if(phi>=270. && phi<315.){
    if( radius<fFRmid){
      hname="CalchepComparison_RFrom270To315_tagInnerR"+sufix;
      hSvc->FillHisto(hname,radius, 1. );
      hname="CalchepComparison_PhiFrom270To315_tagInnerR"+sufix;
      hSvc->FillHisto(hname,phi, 1. );
    }
    else if(radius>=fFRmid){
      hname="CalchepComparison_RFrom270To315_tagOuterR"+sufix;
      hSvc->FillHisto(hname,radius, 1. );
      hname="CalchepComparison_PhiFrom270To315_tagOuterR"+sufix;
      hSvc->FillHisto(hname,phi, 1. );
    }
  }
  else if(phi>=315. && phi<360.){
    if( radius<fFRmid){
      hname="CalchepComparison_RFrom315To360_tagInnerR"+sufix;
      hSvc->FillHisto(hname,radius, 1. );
      hname="CalchepComparison_PhiFrom315To360_tagInnerR"+sufix;
      hSvc->FillHisto(hname,phi, 1. );
    }
    else if(radius>=fFRmid){
      hname="CalchepComparison_RFrom315To360_tagOuterR"+sufix;
      hSvc->FillHisto(hname,radius, 1. );
      hname="CalchepComparison_PhiFrom315To360_tagOuterR"+sufix;
      hSvc->FillHisto(hname,phi, 1. );
    }
  }


}


void CalchepTruthStudies::CountersExtraction(Double_t rg1T,Double_t phig1, Double_t rg2T, Double_t phig2){ 

  for(int p=0; p<8; p++){
    if(phig1>=phiRange[p] && phig1<phiRange[p+1]){
      countsRminRmid(rg1T,phig1, p);
      //countsRminRmid(rg2T,phig2, p);
      countsRmidRmax(rg1T,phig1, p);
      //countsRmidRmax(rg2T,phig2, p);
      countsRminRmax(rg1T,phig1, p);
      //countsRminRmax(rg2T,phig2, p);
    }//end of phi1 range looop
    if(phig2>=phiRange[p] && phig2<phiRange[p+1]){
      countsRminRmid(rg2T,phig2, p);
      countsRmidRmax(rg2T,phig2, p);
      countsRminRmax(rg2T,phig2, p);
    }//end of phi2 range looop
  
    countsReco(p);
  }//end of phi loop
  

}


void CalchepTruthStudies::countsRminRmid(Double_t rg1T,Double_t phig1, int p){
  TRecoVCluster* ecalclu =NULL;
  
  if(rg1T>fFRmin && rg1T< fFRmid){
    nTruth[0][p]++;
    for(int i=0; i< fECal_ClColl->GetNElements(); i++){
      ecalclu       = fECal_ClColl->Element(fIdCl_SortByEnergy.at(i));
      if(energyCut && ecalclu->GetEnergy()<90. ) return;
      Double_t cl1x = ecalclu->GetPosition().X();
      Double_t cl1y = ecalclu->GetPosition().Y();
      Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
      Double_t phi1=TMath::ATan2(cl1y, cl1x)*360./2./TMath::Pi();
      if(phi1<0.)phi1=360.+phi1;
      if(p==0){
	if(phi1>=315. || phi1< 90.){
	  if(cl1r>fFRmin && cl1r<fFRmid)nCluster_ifTRuth[0][p]++;
	  if(cl1r<=fFRmin)nClusterOutRin_ifTruth[0][p]++;
	  if(cl1r>=fFRmid)nClusterOutRout_ifTruth[0][p]++;
	}
      }
      else if(p==7)
	{
	  if(phi1>=270. || phi1< 45.){
	    if(cl1r>fFRmin && cl1r<fFRmid)nCluster_ifTRuth[0][p]++;
	    if(cl1r<=fFRmin)nClusterOutRin_ifTruth[0][p]++;
	    if(cl1r>=fFRmid)nClusterOutRout_ifTruth[0][p]++;
	  }
	}
      else{
	if(fabs(phi1+45.)>=phiRange[p] && fabs(phi1-45.) < phiRange[p+1]){
	  if(cl1r>fFRmin && cl1r<fFRmid)nCluster_ifTRuth[0][p]++;
	  if(cl1r<=fFRmin)nClusterOutRin_ifTruth[0][p]++;
	  if(cl1r>=fFRmid)nClusterOutRout_ifTruth[0][p]++;
	}
      }
    }//end of cluster loop
  } // gamma1 in radius range end of if

  if(rg1T<=fFRmin){
    for(int i=0; i< fECal_ClColl->GetNElements(); i++){
      ecalclu       = fECal_ClColl->Element(fIdCl_SortByEnergy.at(i));
      if(energyCut && ecalclu->GetEnergy()<90. ) return;
      Double_t cl1x = ecalclu->GetPosition().X();
      Double_t cl1y = ecalclu->GetPosition().Y();
      Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
      Double_t phi1=TMath::ATan2(cl1y, cl1x)*360./2./TMath::Pi();
      if(phi1<0.)phi1=360.+phi1;
      if(p==0){
	if(phi1>=315. || phi1< 90.){
	  if(cl1r>fFRmin && cl1r<fFRmid)nCluster_ifTruthOutRin[0][p]++;
	}
      }
      else if(p==7)
	{
	  if(phi1>=270. || phi1< 45.){
	   if(cl1r>fFRmin && cl1r<fFRmid)nCluster_ifTruthOutRin[0][p]++;
	  }
	}
      else{
	if(fabs(phi1+45.)>=phiRange[p] && fabs(phi1-45.) < phiRange[p+1]){
	if(cl1r>fFRmin && cl1r<fFRmid)nCluster_ifTruthOutRin[0][p]++;
	}
      }
    }//end of cluster loop
  } // gamma1 in radius range end of if

  if(rg1T>=fFRmid){
    for(int i=0; i< fECal_ClColl->GetNElements(); i++){
      ecalclu       = fECal_ClColl->Element(fIdCl_SortByEnergy.at(i));
      if(energyCut && ecalclu->GetEnergy()<90. ) return;
      Double_t cl1x = ecalclu->GetPosition().X();
      Double_t cl1y = ecalclu->GetPosition().Y();
      Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
      Double_t phi1=TMath::ATan2(cl1y, cl1x)*360./2./TMath::Pi();
      if(phi1<0.)phi1=360.+phi1;
      if(p==0){
	if(phi1>=315. || phi1< 90.){
	 if(cl1r>fFRmin && cl1r<fFRmid)nCluster_ifTruthOutRout[0][p]++;
	}
      }
      else if(p==7)
	{
	  if(phi1>=270. || phi1< 45.){
	   if(cl1r>fFRmin && cl1r<fFRmid)nCluster_ifTruthOutRout[0][p]++;
	  }
	}
      else{
	if(fabs(phi1+45.)>=phiRange[p] && fabs(phi1-45.) < phiRange[p+1]){
	  if(cl1r>fFRmin && cl1r<fFRmid)nCluster_ifTruthOutRout[0][p]++;
	}
      }
    }//end of cluster loop
  } // gamma1 in radius range end of if

  if(rg1T<=fFRmin || rg1T>=fFRmid){
    for(int i=0; i< fECal_ClColl->GetNElements(); i++){
      ecalclu       = fECal_ClColl->Element(fIdCl_SortByEnergy.at(i));
      if(energyCut && ecalclu->GetEnergy()<90. ) return;
      Double_t cl1x = ecalclu->GetPosition().X();
      Double_t cl1y = ecalclu->GetPosition().Y();
      Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
      Double_t phi1=TMath::ATan2(cl1y, cl1x)*360./2./TMath::Pi();
      if(phi1<0.)phi1=360.+phi1;
      if(p==0){
	if(phi1>=315. || phi1< 90.){
	  if(cl1r>fFRmin && cl1r<fFRmid)nCluster_ifnoTruth[0][p]++;
	}
      }
      else if(p==7)
	{
	  if(phi1>=270. || phi1< 45.){
	    if(cl1r>fFRmin && cl1r<fFRmid)nCluster_ifnoTruth[0][p]++;
	  }
	}
      else{
	if(fabs(phi1+45.)>=phiRange[p] && fabs(phi1-45.) < phiRange[p+1]){
	  if(cl1r>fFRmin && cl1r<fFRmid)nCluster_ifnoTruth[0][p]++;
	}
      }
    }//end of cluster loop
  } // gamma1 in radius range end of if
 

}



void CalchepTruthStudies::countsRmidRmax(Double_t rg1T,Double_t phig1, int p){
  TRecoVCluster* ecalclu =NULL;
 
  //std::cout<<"p index " << p << " minCl " << indexMinCl << " max " << indexMaxCl << std::endl;

  if(rg1T>=fFRmid && rg1T<fFRmax){
    nTruth[1][p]++;
    for(int i=0; i< fECal_ClColl->GetNElements(); i++){
      ecalclu       = fECal_ClColl->Element(fIdCl_SortByEnergy.at(i));
      if(energyCut && ecalclu->GetEnergy()<90. ) return;
      Double_t cl1x = ecalclu->GetPosition().X();
      Double_t cl1y = ecalclu->GetPosition().Y();
      Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
      Double_t phi1=TMath::ATan2(cl1y, cl1x)*360./2./TMath::Pi();
      if(phi1<0.)phi1=360.+phi1;
      if(p==0){
	if(phi1>=315. || phi1< 90.){
	 if(cl1r>=fFRmid && cl1r<fFRmax)nCluster_ifTRuth[1][p]++;
	 if(cl1r<fFRmid)nClusterOutRin_ifTruth[1][p]++;
	 if(cl1r>=fFRmax)nClusterOutRout_ifTruth[1][p]++;
	}
      }
      else if(p==7)
	{
	  if(phi1>=270. || phi1< 45.){
	   if(cl1r>=fFRmid && cl1r<fFRmax)nCluster_ifTRuth[1][p]++;
	   if(cl1r<fFRmid)nClusterOutRin_ifTruth[1][p]++;
	   if(cl1r>=fFRmax)nClusterOutRout_ifTruth[1][p]++;
	  }
	}
      else{
	if(fabs(phi1+45.)>=phiRange[p] && fabs(phi1-45.) < phiRange[p+1]){
	  if(cl1r>=fFRmid && cl1r<fFRmax)nCluster_ifTRuth[1][p]++;
	  if(cl1r<fFRmid)nClusterOutRin_ifTruth[1][p]++;
	  if(cl1r>=fFRmax)nClusterOutRout_ifTruth[1][p]++;
	}
      }
    }//end of cluster loop
  } // gamma1 in radius range end of if

  if(rg1T<fFRmid){
    for(int i=0; i< fECal_ClColl->GetNElements(); i++){
      ecalclu       = fECal_ClColl->Element(fIdCl_SortByEnergy.at(i));
      if(energyCut && ecalclu->GetEnergy()<90. ) return;
      Double_t cl1x = ecalclu->GetPosition().X();
      Double_t cl1y = ecalclu->GetPosition().Y();
      Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
      Double_t phi1=TMath::ATan2(cl1y, cl1x)*360./2./TMath::Pi();
      if(phi1<0.)phi1=360.+phi1;
      if(p==0){
	if(phi1>=315. || phi1< 90.){
	 if(cl1r>=fFRmid && cl1r<fFRmax)nCluster_ifTruthOutRin[1][p]++;
	}
      }
      else if(p==7)
	{
	  if(phi1>=270. || phi1< 45.){
	   if(cl1r>=fFRmid && cl1r<fFRmax)nCluster_ifTruthOutRin[1][p]++;
	  }
	}
      else{
	if(fabs(phi1+45.)>=phiRange[p] && fabs(phi1-45.) < phiRange[p+1]){
	  if(cl1r>=fFRmid && cl1r<fFRmax)nCluster_ifTruthOutRin[1][p]++;
	}
      }
    }//end of cluster loop
  } // gamma1 in radius range end of if
      
  if(rg1T>=fFRmax){
    for(int i=0; i< fECal_ClColl->GetNElements(); i++){
      ecalclu       = fECal_ClColl->Element(fIdCl_SortByEnergy.at(i));
      if(energyCut && ecalclu->GetEnergy()<90. ) return;
      Double_t cl1x = ecalclu->GetPosition().X();
      Double_t cl1y = ecalclu->GetPosition().Y();
      Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
      Double_t phi1=TMath::ATan2(cl1y, cl1x)*360./2./TMath::Pi();
      if(phi1<0.)phi1=360.+phi1;
      if(p==0){
	if(phi1>=315. || phi1< 90.){
	 if(cl1r>=fFRmid && cl1r<fFRmax)nCluster_ifTruthOutRout[1][p]++;
	}
      }
      else if(p==7)
	{
	  if(phi1>=270. || phi1< 45.){
	   if(cl1r>=fFRmid && cl1r<fFRmax)nCluster_ifTruthOutRout[1][p]++;
	  }
	}
      else{
	if(fabs(phi1+45.)>=phiRange[p] && fabs(phi1-45.) < phiRange[p+1]){
	  if(cl1r>=fFRmid && cl1r<fFRmax)nCluster_ifTruthOutRout[1][p]++;
	}
      }
    }//end of cluster loop
  } // gamma1 in radius range end of if
  
  if(rg1T<fFRmid || rg1T>=fFRmax){
    for(int i=0; i< fECal_ClColl->GetNElements(); i++){
      ecalclu       = fECal_ClColl->Element(fIdCl_SortByEnergy.at(i));
      if(energyCut && ecalclu->GetEnergy()<90. ) return;
      Double_t cl1x = ecalclu->GetPosition().X();
      Double_t cl1y = ecalclu->GetPosition().Y();
      Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
      Double_t phi1=TMath::ATan2(cl1y, cl1x)*360./2./TMath::Pi();
      if(phi1<0.)phi1=360.+phi1;
      if(p==0){
	if(phi1>=315. || phi1< 90.){
	 if(cl1r>=fFRmid && cl1r<fFRmax)nCluster_ifnoTruth[1][p]++;
	}
      }
      else if(p==7)
	{
	  if(phi1>=270. || phi1< 45.){
	    if(cl1r>=fFRmid && cl1r<fFRmax)nCluster_ifnoTruth[1][p]++;
	  }
	}
      else{
	if(fabs(phi1+45.)>=phiRange[p] && fabs(phi1-45.) < phiRange[p+1]){
	  if(cl1r>=fFRmid && cl1r<fFRmax)nCluster_ifnoTruth[1][p]++;
	}
      }
    }//end of cluster loop
  } // gamma1 in radius range end of if

}


void CalchepTruthStudies::countsRminRmax(Double_t rg1T,Double_t phig1, int p){
  TRecoVCluster* ecalclu =NULL;
 
  if(rg1T>fFRmin && rg1T<fFRmax){
    nTruth[2][p]++;
    for(int i=0; i< fECal_ClColl->GetNElements(); i++){
      ecalclu       = fECal_ClColl->Element(fIdCl_SortByEnergy.at(i));
      if(energyCut && ecalclu->GetEnergy()<90. ) return;
      Double_t cl1x = ecalclu->GetPosition().X();
      Double_t cl1y = ecalclu->GetPosition().Y();
      Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
      Double_t phi1=TMath::ATan2(cl1y, cl1x)*360./2./TMath::Pi();
      if(phi1<0.)phi1=360.+phi1;
      if(p==0){
	if(phi1>=315. || phi1< 90.){
	  if(cl1r>fFRmin && cl1r<fFRmax)nCluster_ifTRuth[2][p]++;
	  if(cl1r<=fFRmin)nClusterOutRin_ifTruth[2][p]++;
	  if(cl1r>=fFRmax)nClusterOutRout_ifTruth[2][p]++;
	}
      }
      else if(p==7)
	{
	  if(phi1>=270. || phi1< 45.){
	    if(cl1r>fFRmin && cl1r<fFRmax)nCluster_ifTRuth[2][p]++;
	    if(cl1r<=fFRmin)nClusterOutRin_ifTruth[2][p]++;
	    if(cl1r>=fFRmax)nClusterOutRout_ifTruth[2][p]++;
	  }
	}
      else{
	if(fabs(phi1+45.)>=phiRange[p] && fabs(phi1-45.) < phiRange[p+1]){
	  if(cl1r>fFRmin && cl1r<fFRmax)nCluster_ifTRuth[2][p]++;
	  if(cl1r<=fFRmin)nClusterOutRin_ifTruth[2][p]++;
	  if(cl1r>=fFRmax)nClusterOutRout_ifTruth[2][p]++;
	}
      }
    }//end of cluster loop
  } // gamma1 in radius range end of if

  if(rg1T<=fFRmin){
    for(int i=0; i< fECal_ClColl->GetNElements(); i++){
      ecalclu       = fECal_ClColl->Element(fIdCl_SortByEnergy.at(i));
      if(energyCut && ecalclu->GetEnergy()<90. ) return;
      Double_t cl1x = ecalclu->GetPosition().X();
      Double_t cl1y = ecalclu->GetPosition().Y();
      Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
      Double_t phi1=TMath::ATan2(cl1y, cl1x)*360./2./TMath::Pi();
      if(phi1<0.)phi1=360.+phi1;
      if(p==0){
	if(phi1>=315. || phi1< 90.){
	  if(cl1r>fFRmin && cl1r<fFRmax)nCluster_ifTruthOutRin[2][p]++;
	}
      }
      else if(p==7)
	{
	  if(phi1>=270. || phi1< 45.){
	    if(cl1r>fFRmin && cl1r<fFRmax)nCluster_ifTruthOutRin[2][p]++;
	  }
	}
      else{
	 if(fabs(phi1+45.)>=phiRange[p] && fabs(phi1-45.) < phiRange[p+1]){
	   if(cl1r>fFRmin && cl1r<fFRmax)nCluster_ifTruthOutRin[2][p]++;
	 }
      }
    }//end of cluster loop
  } // gamma1 in radius range end of if

  if(rg1T>=fFRmax){
    for(int i=0; i< fECal_ClColl->GetNElements(); i++){
      ecalclu       = fECal_ClColl->Element(fIdCl_SortByEnergy.at(i));
      if(energyCut && ecalclu->GetEnergy()<90. ) return;
      Double_t cl1x = ecalclu->GetPosition().X();
      Double_t cl1y = ecalclu->GetPosition().Y();
      Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
      Double_t phi1=TMath::ATan2(cl1y, cl1x)*360./2./TMath::Pi();
      if(phi1<0.)phi1=360.+phi1;
      if(p==0){
	if(phi1>=315. || phi1< 90.){
	  if(cl1r>fFRmin && cl1r<fFRmax)nCluster_ifTruthOutRout[2][p]++;
	}
      }
      else if(p==7)
	{
	  if(phi1>=270. || phi1< 45.){
	    if(cl1r>fFRmin && cl1r<fFRmax)nCluster_ifTruthOutRout[2][p]++;
	  }
	}
      else{
	if(fabs(phi1+45.)>=phiRange[p] && fabs(phi1-45.) < phiRange[p+1]){
	  if(cl1r>fFRmin && cl1r<fFRmax)nCluster_ifTruthOutRout[2][p]++;
	}
      }
    }//end of cluster loop
  } // gamma1 in radius range end of if

  if(rg1T<=fFRmin || rg1T>=fFRmax){
    for(int i=0; i< fECal_ClColl->GetNElements(); i++){
      ecalclu       = fECal_ClColl->Element(fIdCl_SortByEnergy.at(i));
      if(energyCut && ecalclu->GetEnergy()<90. ) return;
      Double_t cl1x = ecalclu->GetPosition().X();
      Double_t cl1y = ecalclu->GetPosition().Y();
      Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
      Double_t phi1=TMath::ATan2(cl1y, cl1x)*360./2./TMath::Pi();
      if(phi1<0.)phi1=360.+phi1;
      if(p==0){
	if(phi1>=315. || phi1< 90.){
	  if(cl1r>fFRmin && cl1r<fFRmax)nCluster_ifnoTruth[2][p]++;
	}
      }
      else if(p==7)
	{
	  if(phi1>=270. || phi1< 45.){
	   if(cl1r>fFRmin && cl1r<fFRmax)nCluster_ifnoTruth[2][p]++;
	  }
	}
      else{
	if(fabs(phi1+45.)>=phiRange[p] && fabs(phi1-45.) < phiRange[p+1]){
	  if(cl1r>fFRmin && cl1r<fFRmax)nCluster_ifnoTruth[2][p]++;
	}
      }
    }//end of cluster loop
  } // gamma1 in radius range end of if

}


void CalchepTruthStudies::countsReco(int p){
  TRecoVCluster* ecalclu =NULL;

  for(int i=0; i< fECal_ClColl->GetNElements(); i++){
    ecalclu       = fECal_ClColl->Element(fIdCl_SortByEnergy.at(i));
    if(energyCut && ecalclu->GetEnergy()<90. ) return;
    Double_t cl1x = ecalclu->GetPosition().X();
    Double_t cl1y = ecalclu->GetPosition().Y();
    Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
    Double_t phi1=TMath::ATan2(cl1y, cl1x)*360./2./TMath::Pi();
    if(phi1<0.)phi1=360.+phi1;
    if(phi1>=phiRange[p] && phi1 < phiRange[p+1]){
      if(cl1r>fFRmin && cl1r<fFRmid)nCluster[0][p]++;
      if(cl1r>=fFRmid && cl1r<fFRmax)nCluster[1][p]++;
      if(cl1r>fFRmin && cl1r<fFRmax)nCluster[2][p]++;
    }//end of phi cluster request
  }//end of cluster loop
  
}


void CalchepTruthStudies::Terminate(){
  std::cout<<"calchep truth succesfully terminated" <<std::endl;
  printCounters();
  
}




void CalchepTruthStudies::printCounters(){
  //radiusRangeMin[3]={fFRmin, fFRmid, fFRmin};
  //radiusRangeMax[3]={fFRmid, fFRmax, fFRmax};
  //phiRange[9]={0., 45., 90., 135., 170., 225., 270., 315., 360.};      
  //nTruth[3][8]={0};
  //nCluster_ifTruthOutRin[3][8]={0};
  //nCluster_ifTruthOutRout[3][8]={0};
  //nClusterOutRin_ifTruth[3][8]={0};
  //nClusterOutRout_ifTruth[3][8]={0};
  //nCluster_ifTRuth[3][8]={0};
  //nCluster[3][8]={0};
  //nCluster_ifnoTruth[3][8]={0};  

  std::cout<<"--------range--------- [ " << radiusRangeMin[0] << " , " << radiusRangeMax[0] << " ]" << std::endl;
  std::cout<<"phi in [  ,  ] ,   nTruth    , nCluster_ifTruthOutRin,   nCluster_ifTruthOutRout,   nClusterOutRin_ifTruth,   nClusterOutRout_ifTruth,   nCluster_ifTRuth, nCluster,     nCluster_ifnoTrut"<<std::endl;
  for(int i=0; i<8; i++){
    std::cout << "[" << phiRange[i] << ","<<phiRange[i+1]<< "] " << nTruth[0][i] << " " << nCluster_ifTruthOutRin[0][i]<< " " << nCluster_ifTruthOutRout[0][i]<< " " << nClusterOutRin_ifTruth[0][i]<< " " << nClusterOutRout_ifTruth[0][i]<< " " << nCluster_ifTRuth[0][i]<< " "<< nCluster[0][i]<< " " <<nCluster_ifnoTruth[0][i] << std::endl; 
  }

  std::cout<<"--------range--------- [ " << radiusRangeMin[1] << " , " << radiusRangeMax[1] << " ]" << std::endl;
  std::cout<<"phi in [  ,  ] ,   nTruth    , nCluster_ifTruthOutRin,   nCluster_ifTruthOutRout,   nClusterOutRin_ifTruth,   nClusterOutRout_ifTruth,   nCluster_ifTRuth, nCluster,     nCluster_ifnoTrut"<<std::endl;
    for(int i=0; i<8; i++){
    std::cout << "[" << phiRange[i] << ","<<phiRange[i+1]<< "] " << nTruth[1][i] << " " << nCluster_ifTruthOutRin[1][i]<< " " << nCluster_ifTruthOutRout[1][i]<< " " << nClusterOutRin_ifTruth[1][i]<< " " << nClusterOutRout_ifTruth[1][i]<< " " << nCluster_ifTRuth[1][i]<< " "<< nCluster[1][i]<< " " <<nCluster_ifnoTruth[1][i] << std::endl; 
  }

  std::cout<<"--------range--------- [ " << radiusRangeMin[2] << " , " << radiusRangeMax[2] << " ]" << std::endl;
  std::cout<<"phi in [  ,  ] ,   nTruth    , nCluster_ifTruthOutRin,   nCluster_ifTruthOutRout,   nClusterOutRin_ifTruth,   nClusterOutRout_ifTruth,   nCluster_ifTRuth, nCluster,     nCluster_ifnoTrut"<<std::endl;
    for(int i=0; i<8; i++){
    std::cout << "[" << phiRange[i] << ","<<phiRange[i+1]<< "] " << nTruth[2][i] << " " << nCluster_ifTruthOutRin[2][i]<< " " << nCluster_ifTruthOutRout[2][i]<< " " << nClusterOutRin_ifTruth[2][i]<< " " << nClusterOutRout_ifTruth[2][i]<< " " << nCluster_ifTRuth[2][i]<< " "<< nCluster[2][i]<< " " <<nCluster_ifnoTruth[2][i] << std::endl; 
  }

}
