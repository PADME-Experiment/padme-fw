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

  fcountEvent=0;


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
    hname="CalchepComparison_R1_"+sufix3.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_E1_"+sufix3.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_R2_"+sufix3.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="CalchepComparison_E2_"+sufix3.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
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
  Double_t PTransverse1 = sqrt(p1[0]*p1[0]+p1[1]*p1[1]);
  Double_t PTransverse2 = sqrt(p2[0]*p2[0]+p2[1]*p2[1]);
  Double_t shiftEcal=sqrt(3.13*3.13+3.86*3.86);
  Double_t g1Recal=PTransverse1*fdistanceTarget/fabs(p1[2]);
  Double_t g2Recal=PTransverse2*fdistanceTarget/fabs(p2[2]);

  FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar");

  Double_t g1phi=TMath::ATan2(p1[1], p1[0])*360/TMath::Pi();
  Double_t g2phi=TMath::ATan2(p2[1], p2[0])*360/TMath::Pi();

  if(g1Recal>115.8 && g1Recal<258){
    FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_g1inFR");
    if(fabs(g1phi)<45. || fabs(g1phi-180)< 45. ) FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_g1inFR_PhiCut");
    if(g2Recal>115.8 && g2Recal<258){
      FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_g1g2inFR");
    }
  }

  FillCalchepSmearedHisto(g1Recal,g2Recal);

  TRecoVCluster* ecalclu=NULL;
  TRecoVCluster* ecalclu2=NULL;
  TRecoVCluster* ecalclu3=NULL;

  if(fECal_ClColl->GetNElements()==0)return;
  ecalclu          = fECal_ClColl->Element(fIdCl_SortByEnergy.at(0));
  Double_t cl1E = ecalclu->GetEnergy();
  Double_t cl1x = ecalclu->GetPosition().X();
  Double_t cl1y = ecalclu->GetPosition().Y();
  Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
  Double_t phi1=TMath::ATan2(cl1y, cl1x)*360/TMath::Pi();
    
  if(fECal_ClColl->GetNElements()==2){
    ecalclu2          = fECal_ClColl->Element(fIdCl_SortByEnergy.at(1));
    Double_t cl2E = ecalclu2->GetEnergy();
    Double_t cl2x = ecalclu2->GetPosition().X();
    Double_t cl2y = ecalclu2->GetPosition().Y();
    Double_t cl2r=sqrt(cl2x*cl2x+cl2y*cl2y);
    Double_t phi2=TMath::ATan2(cl2y, cl2x)*360/TMath::Pi();
     
    FillGeneralHisto(cl1r, cl1E,cl2r,cl2E, "CalchepSim_Ncl2");
    FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_Ncl2");
    FillComparisonHisto(g1Recal, g1E,g2Recal,g2E,cl1r, cl1E,cl2r,cl2E, "CalchepSim_Ncl2");

    if(cl1r>115.8 && cl1r<258){
      FillGeneralHisto(cl1r, cl1E,cl2r,cl2E, "CalchepSim_Ncl2_g1inFR");
      FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_Ncl2_g1inFR");
      FillComparisonHisto(g1Recal, g1E,g2Recal,g2E,cl1r, cl1E,cl2r,cl2E, "CalchepSim_Ncl2_g1inFR");

      if(fabs(phi1)<45. || fabs(phi1-180)< 45. ){
	FillGeneralHisto(cl1r, cl1E,cl2r,cl2E, "CalchepSim_Ncl2_g1inFR_PhiCut");
	FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_Ncl2_g1inFR_PhiCut");
	FillComparisonHisto(g1Recal, g1E,g2Recal,g2E,cl1r, cl1E,cl2r,cl2E, "CalchepSim_Ncl2_g1inFR_PhiCut");
      }
    }
    if(cl1r>115.8 && cl1r<258 && cl2r>115.8 && cl2r<258){
      FillGeneralHisto(cl1r, cl1E,cl2r,cl2E, "CalchepSim_Ncl2_g1g2inFR");
      FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_Ncl2_g1g2inFR");
      FillComparisonHisto(g1Recal, g1E,g2Recal,g2E,cl1r, cl1E,cl2r,cl2E, "CalchepSim_Ncl2_g1g2inFR");

    }  
  }//end of if ncl==2


  if(fECal_ClColl->GetNElements()==3){
  std::cout<<"n clsuter=3 , ncl " << fECal_ClColl->GetNElements() << " nElement in sorted " <<  fIdCl_SortByEnergy.size() << std::endl;
    ecalclu2          = fECal_ClColl->Element(fIdCl_SortByEnergy.at(1));
    Double_t cl2E = ecalclu2->GetEnergy();
    Double_t cl2x = ecalclu2->GetPosition().X();
    Double_t cl2y = ecalclu2->GetPosition().Y();
    Double_t cl2r=sqrt(cl2x*cl2x+cl2y*cl2y);
    Double_t phi2=TMath::ATan2(cl2y, cl2x)*360/TMath::Pi();
    
    ecalclu3          = fECal_ClColl->Element(fIdCl_SortByEnergy.at(2));
    Double_t cl3E = ecalclu3->GetEnergy();
    Double_t cl3x = ecalclu3->GetPosition().X();
    Double_t cl3y = ecalclu3->GetPosition().Y();
    Double_t cl3r=sqrt(cl3x*cl3x+cl3y*cl3y);
    Double_t phi3=TMath::ATan2(cl3y, cl3x)*360/TMath::Pi();
    
    //+ comparison g1 truth g1 reco, g2 truth g2 reco if cl1 in FR e.g.
    FillGeneralHisto(g1Recal, g1E,g2Recal,g2E, "CalchepVar_Ncl3");
    FillGeneralHisto(cl1r, cl1E,cl2r,cl2E, "CalchepSim_Ncl3");
    FillHistoNCl3(cl1r, cl1E,cl2r, cl2E,cl3r, cl3E, "CalchepSim_Ncl3");
    if(cl1r>115.8 && cl1r<258){
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
  if(R1>115.8 && R1<258){
    sufix="CalchepVar_AccEff_g1inFR";
    hname="CalchepComparison_Th1_"+sufix;
    hSvc->FillHisto(hname, g1Th,1.);
    hname="CalchepComparison_Th2_"+sufix;
    hSvc->FillHisto(hname, g2Th,1.);
   if(R2>115.8 && R2<258){
      sufix="CalchepVar_AccEff_g1g2inFR";
      hname="CalchepComparison_Th1_"+sufix;
      hSvc->FillHisto(hname, g1Th,1.);
      hname="CalchepComparison_Th2_"+sufix;
      hSvc->FillHisto(hname, g2Th,1.);
   }
  }
  Double_t sR1=tan(sTh1)*fdistanceTarget;
  Double_t sR2=tan(sTh2)*fdistanceTarget;
  if(sR1>115.8 && sR1<258){
    sufix="CalchepVar_AccEff_g1inFR";
    hname="CalchepComparison_sTh1_"+sufix;
    hSvc->FillHisto(hname, sTh1,1.);
    hname="CalchepComparison_sTh2_"+sufix;
    hSvc->FillHisto(hname, sTh2,1.);
    if(sR2>115.8 && sR2<258){
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
