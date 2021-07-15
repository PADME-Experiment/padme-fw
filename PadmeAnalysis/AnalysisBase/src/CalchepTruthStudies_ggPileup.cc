#include "CalchepTruthStudies_ggPileup.hh"

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


CalchepTruthStudies_ggPileup::CalchepTruthStudies_ggPileup()
{
  
  fRecoEvent   = NULL;

  fECal_hitEvent=0;
  fECal_ClColl=0;

  fVerbose        = 0;

 
}
CalchepTruthStudies_ggPileup::CalchepTruthStudies_ggPileup(Int_t processingMode, Int_t verbosityFlag)
{
  fRecoEvent      = NULL;
  fVerbose        = verbosityFlag;
}
CalchepTruthStudies_ggPileup::~CalchepTruthStudies_ggPileup()
{
}


Bool_t CalchepTruthStudies_ggPileup::Init(Double_t DataEffInnerRRange_r1inFR[8], Double_t DataEffOuterRRange_r1inFR[8], Double_t DataEffInnerRRange_r1r2inFR[8], Double_t DataEffOuterRRange_r1r2inFR[8], Double_t MCEffInnerRRange_r1inFR[8], Double_t MCEffOuterRRange_r1inFR[8], Double_t MCEffInnerRRange_r1r2inFR[8], Double_t MCEffOuterRRange_r1r2inFR[8]){
  
   for(int i=0; i<8; i++){
     fDataEffInnerRRange_r1inFR[i]  =DataEffInnerRRange_r1inFR  [i];
     fDataEffOuterRRange_r1inFR[i]  =DataEffOuterRRange_r1inFR  [i];
     fDataEffInnerRRange_r1r2inFR[i]=DataEffInnerRRange_r1r2inFR[i];
     fDataEffOuterRRange_r1r2inFR[i]=DataEffOuterRRange_r1r2inFR[i];
     fMCEffInnerRRange_r1inFR[i]  =MCEffInnerRRange_r1inFR  [i];
     fMCEffOuterRRange_r1inFR[i]  =MCEffOuterRRange_r1inFR  [i];
     fMCEffInnerRRange_r1r2inFR[i]=MCEffInnerRRange_r1r2inFR[i];
     fMCEffOuterRRange_r1r2inFR[i]=MCEffOuterRRange_r1r2inFR[i];
  }

   fdistanceTarget=1030+2513; // from calchep g4 hits studies
   fFRmin=115.82;
   fFRmid=172.83;
   fFRmax=258.0;

   sameEv1=0;
   sameEv2=0;
   sameEv3=0;

   fCalchep430.open("ee-gg_430MeV_NoPipe.txt");

  return true;
}


Bool_t CalchepTruthStudies_ggPileup::GetTruthEvent()
{
  getCalchepTruth();
 
  return true;
}

//Bool_t CalchepTruthStudies_ggPileup::Process()
//{
//  getCalchepTruth();
// 
//  return true;
//}


Bool_t CalchepTruthStudies_ggPileup::GetReco(Double_t E1,Double_t phi1, Double_t R1,Double_t E2, Double_t phi2, Double_t R2){

  fE1=E1;	 
  fR1=R1;	 
  fphi1=phi1*360./2./TMath::Pi();
  fE2=E2;	 
  fR2=R2;	 
  fphi2=phi2*360./2./TMath::Pi();
  if(fphi1<0.)fphi1=360+fphi1;
  if(fphi2<0.)fphi2=360+fphi2;

  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  hname="MethodCalchepPileup_AllAnnPassed";
  hSvc->FillHisto(hname,(fE1+fE2) ,1.);

  //  Process();
  Bool_t sameEv=ComparePhotons(fg1phi,fg1E, fg2phi, fg2E);
  if(sameEv){
    FillHisto();
    checkScaleFactorsMethod();
  }
  //std::cout <<"1: "<< sameEv1 << ", 2: " << sameEv2 << ", 3: " << sameEv3 << std::endl;
 
}


Bool_t CalchepTruthStudies_ggPileup::InitHistos(){
  
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  int binX=500;
  double minX=0;
  double maxX=2000;
  int binY=500;
  double minY=0;
  double maxY=2000;

  hname="MethodCalchepPileup_AllAnnPassed";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MethodCalchepPileup_AnnCloseToTheTruth";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MethodCalchepPileup_AnnCloseToTheTruth_WscaleFactorsR1inFR";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MethodCalchepPileup_AnnCloseToTheTruth_WscaleFactorsR1R2inFR";
  hSvc->BookHisto(hname, binX, minX, maxX);

  binX=500;
  minX=-500;
  maxX=500;
  hname="MethodCalchepPileup_DE1CalchepReco";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MethodCalchepPileup_DE2CalchepReco";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MethodCalchepPileup_DE2vsDE1CalchepReco";
  hSvc->BookHisto2(hname, binX, minX, maxX, binX, minX, maxX);
  binX=360;
  minX=-360;
  maxX=360;
  hname="MethodCalchepPileup_DPhi1CalchepReco";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MethodCalchepPileup_DPhi2CalchepReco";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MethodCalchepPileup_DPhi2vsDPhi1CalchepReco";
  hSvc->BookHisto2(hname, binX, minX, maxX, binX, minX, maxX);

  binX=500;
  minX=0.;
  maxX=2000.;
  hname="MethodCalchepPileup_TruthDataSample";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MethodCalchepPileup_TruthMCSample";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MethodDataMCPileup_Data";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MethodDataMCPileup_MC";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MethodDataMCPileup_Datag1inFR";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MethodDataMCPileup_MCg1inFR";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MethodDataMCPileup_Datag1g2inFR";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MethodDataMCPileup_MCg1g2inFR";
  hSvc->BookHisto(hname, binX, minX, maxX);


  std::vector<std::string> sufixW;
  sufixW.push_back("DataSample");
  sufixW.push_back("MCSample_usualW");
  for(int i=0; i<sufixW.size(); i++){
    binX=500;
    minX=0.;
    maxX=2000.;
    hname="MethodDataMCPileup_WEffR1inFR"+sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="MethodDataMCPileup_WEffR1R2inFR"+sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
  }

 std::vector<std::string> sufixW1;
 sufixW1.push_back("MCSample_DataMCW");
 for(int i=0; i<sufixW1.size(); i++){
   binX=500;
   minX=0.;
   maxX=2000.;
   hname="MethodDataMCPileup_WEffR1inFRDataPlus90"+sufixW1.at(i);
   hSvc->BookHisto(hname, binX, minX, maxX);
   hname="MethodDataMCPileup_WEffR1R2inFRDataPlus90"+sufixW1.at(i);
   hSvc->BookHisto(hname, binX, minX, maxX);
   }


 return true;
}


void CalchepTruthStudies_ggPileup::getCalchepTruth(){
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
  fg1E=sqrt(p1[0]*p1[0]+p1[1]*p1[1]+p1[2]*p1[2])*1000;//to convert in MeV
  fg2E=sqrt(p2[0]*p2[0]+p2[1]*p2[1]+p2[2]*p2[2])*1000;
  if(fg1E<fg2E){
    ptmp[0]=-p1[0];
    ptmp[1]=-p1[1];
    ptmp[2]=-p1[2];
    p1[0]=-p2[0];
    p1[1]=-p2[1];
    p1[2]=-p2[2];
    p2[0]=ptmp[0];
    p2[1]=ptmp[1];
    p2[2]=ptmp[2];
    etmp=fg1E;
    fg1E=fg2E;
    fg2E=etmp;
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
  fg1Recal=PTransverse1*fdistanceTarget/p1[2];
  fg2Recal=PTransverse2*fdistanceTarget/p2[2];

  fg1phi=TMath::ATan2(p1[1], p1[0])*360./2./TMath::Pi();
  fg2phi=TMath::ATan2(p2[1], p2[0])*360./2./TMath::Pi();
  Double_t x1= fg1Recal*cos(fg1phi);
  Double_t y1= fg1Recal*sin(fg1phi);
  Double_t x2= fg2Recal*cos(fg2phi);
  Double_t y2= fg2Recal*sin(fg2phi);
  //std::cout<<"g1, atan2 " << TMath::ATan2(p1[1], p1[0]) << " in deg " << g1phi << std::endl;
  TLorentzVector P4g1F, P4g2F;
  P4g1F.SetPxPyPzE(p1[0],p1[1],p1[2],fg1E);
  P4g2F.SetPxPyPzE(p2[0],p2[1],p2[2],fg2E);
  if(fg1phi<0.){fg1phi=360+fg1phi;  }//std::cout<<"g1<0, so after correction " <<  g1phi << std::endl;}
  if(fg2phi<0.) fg2phi=360+fg2phi;
  
  if(fg1Recal>fFRmin && fg1Recal<fFRmax ){
    //    if(fabs(fg1phi-90)<45 || fabs(fg1phi-270)<45){
    if(fabs(fg1phi-135)<45 || fabs(fg1phi-315)<45){
      hname="MethodCalchepPileup_TruthDataSample";
      hSvc->FillHisto(hname,(fg1E+fg2E),1.);
    }
    else{
      hname="MethodCalchepPileup_TruthMCSample";
      hSvc->FillHisto(hname,(fg1E+fg2E),1.);
    }
  }

}


Bool_t CalchepTruthStudies_ggPileup::ComparePhotons(Double_t phig1, Double_t e1T, Double_t phig2, Double_t e2T){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  hname="MethodCalchepPileup_DE1CalchepReco";
  hSvc->FillHisto(hname,(e1T-fE1),1.);
  hname="MethodCalchepPileup_DE2CalchepReco";
  hSvc->FillHisto(hname,(e2T-fE2),1.);
  hname="MethodCalchepPileup_DPhi1CalchepReco";
  hSvc->FillHisto(hname,(phig1-fphi1),1.);
  hname="MethodCalchepPileup_DPhi2CalchepReco";
  hSvc->FillHisto(hname,(phig2-fphi2),1.);
  hname="MethodCalchepPileup_DE2vsDE1CalchepReco";
  hSvc->FillHisto2(hname,(e1T-fE1),(e2T-fE2),1.);
  hname="MethodCalchepPileup_DPhi2vsDPhi1CalchepReco";
  hSvc->FillHisto2(hname, (phig1-fphi1), (phig2-fphi2) ,1.);

  Bool_t sameEvent=false;
  if(fabs(e1T-fE1)<50. && fabs(e2T-fE2)<50. && fabs(phig1-fphi1)<50. && fabs(phig2-fphi2)<50.){sameEv1++; sameEvent=true;}
  if(fabs(phig1-fphi1)>170. && fabs(phig2-fphi2)>170.&& fabs(phig1-fphi1)<190. && fabs(phig2-fphi2)<190.){
    if(fabs(e1T-fE2)<50. && fabs(e1T-fE2)<50. && fabs(phig1-fphi2)<50. && fabs(phig2-fphi1)<50.){sameEv2++; sameEvent=true;}
    if(((fabs(phig1-fphi2)-360<50. && (fabs(phig2-fphi1)<50.)) || (fabs(phig2-fphi1)-360<50. &&fabs(phig1-fphi2)<50.) ) && fabs(e1T-fE2)<50. && fabs(e2T-fE1)<50.)sameEvent=true;
  }
  if(((fabs(phig1-fphi1)-360<50. && (fabs(phig2-fphi2)<50.)) || (fabs(phig2-fphi2)-360<50. &&fabs(phig1-fphi1)<50.) ) && fabs(e1T-fE1)<50. && fabs(e2T-fE2)<50.)sameEvent=true;
  if(sameEvent) sameEv3++;
  return sameEvent;
}

void CalchepTruthStudies_ggPileup::FillHisto(){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

  hname="MethodCalchepPileup_AnnCloseToTheTruth";
  hSvc->FillHisto(hname,(fE1+fE2) ,1.);

  FillWeightedHisto_R1inFR_MC();
  if(fR2>fFRmin && fR2<fFRmax)FillWeightedHisto_R1R2inFR_MC();

}

void CalchepTruthStudies_ggPileup::FillWeightedHisto_R1inFR_MC(){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  
  //Double_t effg1=ReturnEfficiencyR1R2inFR_MC(fR1,fphi1);
  Double_t effg1=ReturnEfficiencyR1inFR_MC(fR1,fphi1);
  Double_t effg2=ReturnEfficiencyR1inFR_MC(fR2,fphi2);
  //Double_t effScaleFactor=(1./effg1)*(1./effg2);
  Double_t effScaleFactor=effg1*effg2;
  hname="MethodCalchepPileup_AnnCloseToTheTruth_WscaleFactorsR1inFR";
  hSvc->FillHisto(hname,(fE1+fE2) ,effScaleFactor );

}


void CalchepTruthStudies_ggPileup::FillWeightedHisto_R1R2inFR_MC(){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  //Double_t effg1=ReturnEfficiencyR1R2inFR_MC(fR1,fphi1);
  Double_t effg1=ReturnEfficiencyR1inFR_MC(fR1,fphi1);
  Double_t effg2=ReturnEfficiencyR1inFR_MC(fR2,fphi2);
  //  Double_t effg2=ReturnEfficiencyR1R2inFR_MC(fR2,fphi2);
  //Double_t effScaleFactor=(1./effg1)*(1./effg2);
  Double_t effScaleFactor=effg1*effg2;
  hname="MethodCalchepPileup_AnnCloseToTheTruth_WscaleFactorsR1R2inFR";
  hSvc->FillHisto(hname,(fE1+fE2) ,effScaleFactor );
  
}

Double_t CalchepTruthStudies_ggPileup::ReturnEfficiencyR1inFR_MC(Double_t radius, Double_t phi){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
   std::string hname;
  Bool_t inner=false;
  Bool_t outer=false;
  if(radius<fFRmid )inner=true;
  if(radius>=fFRmid)outer=true;
  //Double_t phiDeg=phi*360./(2*TMath::Pi());//i've just converted in deg
  //if(phiDeg<0.)phiDeg=360+phiDeg;
  int phiSliceMC=phi/45.;
  Double_t eff;
 
  if(inner){
    eff=fDataEffInnerRRange_r1inFR[phiSliceMC]/fMCEffInnerRRange_r1inFR[phiSliceMC];
  }
  else if(outer){
    eff=fDataEffOuterRRange_r1inFR[phiSliceMC]/fMCEffOuterRRange_r1inFR[phiSliceMC];
  }
  
  return eff;
}

Double_t CalchepTruthStudies_ggPileup::ReturnEfficiencyR1R2inFR_MC(Double_t radius, Double_t phi){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Bool_t inner=false;
  Bool_t outer=false;
  if(radius>fFRmin && radius<fFRmid)inner=true;
  if(radius>=fFRmid && radius<fFRmax)outer=true;
  //Double_t phiDeg=phi*360/(2*TMath::Pi());
  //if(phiDeg<0.)phiDeg=360+phiDeg;
  int phiSliceMC=phi/45.;
  Double_t eff;
  if(inner){
    eff=fDataEffInnerRRange_r1r2inFR[phiSliceMC]/fMCEffInnerRRange_r1r2inFR[phiSliceMC];
  }
  else if(outer){
    eff=fDataEffOuterRRange_r1r2inFR[phiSliceMC]/fMCEffOuterRRange_r1r2inFR[phiSliceMC];
  }
 
  return eff;
}



void CalchepTruthStudies_ggPileup::checkScaleFactorsMethod(){
  if(checkIfItIsDataOrMC(fphi1,fphi2 ))FillHistoData();   //data==1 so data otherwise MC; phi of reco, it is in deg
  else FillHistoMC();
}


Int_t CalchepTruthStudies_ggPileup::checkIfItIsDataOrMC(Double_t phi1,Double_t phi2){
  Int_t data=0;
  //if(fabs(phi1-90)<45 || fabs(phi1-270)<45)data=1;
  if(fabs(phi1-135)<45 || fabs(phi1-315)<45)data=1;
  return data;

}

void CalchepTruthStudies_ggPileup::FillHistoData(){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

  hname="MethodDataMCPileup_Data";
  hSvc->FillHisto(hname, fE1+fE2, 1.);
  if(fR1>fFRmin && fR1< fFRmax){
    hname="MethodDataMCPileup_Datag1inFR";
    hSvc->FillHisto(hname, fE1+fE2, 1.);
    FillWeightedHisto_R1inFR("DataSample");
    if(fR2>fFRmin && fR2< fFRmax){
      FillWeightedHisto_R1R2inFR("DataSample");
        hname="MethodDataMCPileup_Datag1g2inFR";
	hSvc->FillHisto(hname, fE1+fE2, 1.);

    }
  }
}

void CalchepTruthStudies_ggPileup::FillHistoMC(){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

  hname="MethodDataMCPileup_MC";
  hSvc->FillHisto(hname, fE1+fE2, 1.);
  if(fR1>fFRmin && fR1< fFRmax){
    hname="MethodDataMCPileup_MCg1inFR";
    hSvc->FillHisto(hname, fE1+fE2, 1.);
    FillWeightedHisto_R1inFR("MCSample_usualW");
    if(fR2>fFRmin && fR2< fFRmax)FillWeightedHisto_R1R2inFR("MCSample_usualW");
    FillWeightedHisto_R1inFR_DataMC("MCSample_DataMCW");
    if(fR2>fFRmin && fR2< fFRmax){
      FillWeightedHisto_R1R2inFR_DataMC("MCSample_DataMCW");
      hname="MethodDataMCPileup_MCg1g2inFR";
      hSvc->FillHisto(hname, fE1+fE2, 1.);
    }
  }

}

void  CalchepTruthStudies_ggPileup::FillWeightedHisto_R1inFR(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
 
  Double_t effg1=ReturnEfficiencyR1inFR(fR1,fphi1);
  Double_t effg2=ReturnEfficiencyR1inFR(fR2,fphi2);
  Double_t effScaleFactor=(1./effg1)*(1./effg2);
  hname="MethodDataMCPileup_WEffR1inFR"+sufix;
  hSvc->FillHisto(hname,(fE1+fE2) ,effScaleFactor );

}


void  CalchepTruthStudies_ggPileup::FillWeightedHisto_R1R2inFR(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  
  Double_t effg1=ReturnEfficiencyR1inFR(fR1,fphi1);
  Double_t effg2=ReturnEfficiencyR1R2inFR(fR2,fphi2);
  Double_t effScaleFactor=(1./effg1)*(1./effg2);
  hname="MethodDataMCPileup_WEffR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fE1+fE2) ,effScaleFactor ); 
}

void  CalchepTruthStudies_ggPileup::FillWeightedHisto_R1inFR_DataMC(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  
  Double_t effg1=ReturnEfficiencyR1inFR_DataMC(fR1,fphi1);
  Double_t effg2=ReturnEfficiencyR1inFR_DataMC(fR2,fphi2);
  Double_t effScaleFactor=effg1*effg2;
  hname="MethodDataMCPileup_WEffR1inFRDataPlus90"+sufix;
  hSvc->FillHisto(hname,(fE1+fE2) ,effScaleFactor );
  
}


void  CalchepTruthStudies_ggPileup::FillWeightedHisto_R1R2inFR_DataMC(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
 
  Double_t effg1=ReturnEfficiencyR1inFR_DataMC(fR1,fphi1);
  Double_t effg2=ReturnEfficiencyR1R2inFR_DataMC(fR2,fphi2);
  Double_t effScaleFactor=effg1*effg2;
  hname="MethodDataMCPileup_WEffR1R2inFRDataPlus90"+sufix;
  hSvc->FillHisto(hname,(fE1+fE2) ,effScaleFactor );
}


Double_t CalchepTruthStudies_ggPileup::ReturnEfficiencyR1inFR(Double_t radius, Double_t phi){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
   std::string hname;
  Bool_t inner=false;
  Bool_t outer=false;
  if(radius<fFRmid )inner=true;
  if(radius>=fFRmid)outer=true;
  int phiSlice=phi/45.;
  Double_t eff;
 
  if(inner){
    eff=fMCEffInnerRRange_r1inFR[phiSlice];
  }
  else if(outer){
    eff=fMCEffOuterRRange_r1inFR[phiSlice];
  }

  return eff;
}

Double_t CalchepTruthStudies_ggPileup::ReturnEfficiencyR1R2inFR(Double_t radius, Double_t phi){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Bool_t inner=false;
  Bool_t outer=false;
  if(radius>fFRmin && radius<fFRmid)inner=true;
  if(radius>=fFRmid && radius<fFRmax)outer=true;
  int phiSlice=phi/45.;
  Double_t eff;
  if(inner){
    eff=fMCEffInnerRRange_r1r2inFR[phiSlice];
  }
  else if(outer){
    eff=fMCEffOuterRRange_r1r2inFR[phiSlice];
  }
 
  return eff;
}


Double_t  CalchepTruthStudies_ggPileup::ReturnEfficiencyR1inFR_DataMC(Double_t radius, Double_t phi){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
   std::string hname;
  Bool_t inner=false;
  Bool_t outer=false;
  if(radius<fFRmid )inner=true;
  if(radius>=fFRmid)outer=true;
  int phiSliceMC=phi/45.;
  int phiSliceD=phi/45.+2;
  if(phiSliceD>7){
    phiSliceD=phiSliceD-8;
  }
  Double_t eff;
 
  if(inner){
    eff=fMCEffInnerRRange_r1inFR[phiSliceD]/fMCEffInnerRRange_r1inFR[phiSliceMC];
  }
  else if(outer){
    eff=fMCEffOuterRRange_r1inFR[phiSliceD]/fMCEffOuterRRange_r1inFR[phiSliceMC];
  }
  return eff;
}

Double_t  CalchepTruthStudies_ggPileup::ReturnEfficiencyR1R2inFR_DataMC(Double_t radius, Double_t phi){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Bool_t inner=false;
  Bool_t outer=false;
  if(radius>fFRmin && radius<fFRmid)inner=true;
  if(radius>=fFRmid && radius<fFRmax)outer=true;
  int phiSliceMC=phi/45.;
  int phiSliceD=phi/45.+2;
  if(phiSliceD>7){
    phiSliceD=phiSliceD-8;
  }
  Double_t eff;
  if(inner){
    eff=fMCEffInnerRRange_r1r2inFR[phiSliceD]/fMCEffInnerRRange_r1r2inFR[phiSliceMC];
  }
  else if(outer){
    eff=fMCEffOuterRRange_r1r2inFR[phiSliceD]/fMCEffOuterRRange_r1r2inFR[phiSliceMC];
  }
 
  return eff;
}

