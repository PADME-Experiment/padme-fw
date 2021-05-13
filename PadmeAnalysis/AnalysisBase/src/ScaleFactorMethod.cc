#include "ScaleFactorMethod.hh"

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


ScaleFactorMethod::ScaleFactorMethod()
{
}

ScaleFactorMethod::~ScaleFactorMethod()
{
}


Bool_t ScaleFactorMethod::Init(Double_t DataEffInnerRRange_r1inFR[8], Double_t DataEffOuterRRange_r1inFR[8], Double_t DataEffInnerRRange_r1r2inFR[8], Double_t DataEffOuterRRange_r1r2inFR[8], Double_t MCEffInnerRRange_r1inFR[8], Double_t MCEffOuterRRange_r1inFR[8], Double_t MCEffInnerRRange_r1r2inFR[8], Double_t MCEffOuterRRange_r1r2inFR[8]){
  
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

  return true;
}

Bool_t ScaleFactorMethod::Process(Double_t E1,Double_t phi1, Double_t R1,Double_t E2, Double_t phi2, Double_t R2){

  fE1=E1;	 
  fR1=R1;	 
  fphi1=phi1;
  fE2=E2;	 
  fR2=R2;	 
  fphi2=phi2;

  FillHistoMC();

  return true;
}



void ScaleFactorMethod::FillHistoMC(){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

  hname="MethodScaleFactor_MC";
  hSvc->FillHisto(hname, fE1+fE2, 1.);

  if(fR1>fFRmin && fR1< fFRmax){
    hname="MethodScaleFactor_MCg1inFR";
    hSvc->FillHisto(hname, fE1+fE2, 1.);
    FillWeightedHisto_R1inFR("MCSample_usualW");
    if(fR2>fFRmin && fR2<fFRmax )FillWeightedHisto_R1R2inFR("MCSample_usualW");
   
    FillWeightedHisto_R1inFR_MC("MCSample_ScaleFactorW");
   
    if(fR2>fFRmin && fR2< fFRmax){
      FillWeightedHisto_R1R2inFR_MC("MCSample_ScaleFactorW");
      hname="MethodScaleFactor_MCg1g2inFR";
      hSvc->FillHisto(hname, fE1+fE2, 1.);
    }

  }

}


void ScaleFactorMethod::FillWeightedHisto_R1inFR(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  
  //Double_t effg1=ReturnEfficiencyR1R2inFR(fR1,fphi1);
  Double_t effg1=ReturnEfficiencyR1inFR(fR1,fphi1);
  Double_t effg2=ReturnEfficiencyR1inFR(fR2,fphi2);
  Double_t effScaleFactor=(1./effg1)*(1./effg2);
  hname="MethodScaleFactor_WEffR1inFR"+sufix;
  hSvc->FillHisto(hname,(fE1+fE2) ,effScaleFactor );


}


void ScaleFactorMethod::FillWeightedHisto_R1R2inFR(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  //Double_t effg1=ReturnEfficiencyR1R2inFR(fR1,fphi1);
  Double_t effg1=ReturnEfficiencyR1inFR(fR1,fphi1);
  Double_t effg2=ReturnEfficiencyR1R2inFR(fR2,fphi2);
  Double_t effScaleFactor=(1./effg1)*(1./effg2);
  hname="MethodScaleFactor_WEffR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fE1+fE2) ,effScaleFactor );
 
}

void ScaleFactorMethod::FillWeightedHisto_R1inFR_MC(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  
  //Double_t effg1=ReturnEfficiencyR1R2inFR_MC(fR1,fphi1);
  Double_t effg1=ReturnEfficiencyR1inFR_MC(fR1,fphi1);
  Double_t effg2=ReturnEfficiencyR1inFR_MC(fR2,fphi2);
  //Double_t effScaleFactor=(1./effg1)*(1./effg2);
  Double_t effScaleFactor=effg1*effg2;
  hname="MethodScaleFactor_WEffR1inFR"+sufix;
  hSvc->FillHisto(hname,(fE1+fE2) ,effScaleFactor );

}


void ScaleFactorMethod::FillWeightedHisto_R1R2inFR_MC(std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  //Double_t effg1=ReturnEfficiencyR1R2inFR_MC(fR1,fphi1);
  Double_t effg1=ReturnEfficiencyR1inFR_MC(fR1,fphi1);
  Double_t effg2=ReturnEfficiencyR1R2inFR_MC(fR2,fphi2);
  //Double_t effScaleFactor=(1./effg1)*(1./effg2);
  Double_t effScaleFactor=effg1*effg2;
  hname="MethodScaleFactor_WEffR1R2inFR"+sufix;
  hSvc->FillHisto(hname,(fE1+fE2) ,effScaleFactor );
  
}


Double_t ScaleFactorMethod::ReturnEfficiencyR1inFR(Double_t radius, Double_t phi){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
   std::string hname;
  Bool_t inner=false;
  Bool_t outer=false;
  if(radius<173. )inner=true;
  if(radius>=173.)outer=true;
  Double_t phiDeg=phi*360./(2*TMath::Pi());
  if(phiDeg<0.)phiDeg=360+phiDeg;
  int phiSlice=phiDeg/45.;
  Double_t eff;
 
  if(inner){
    eff=fMCEffInnerRRange_r1inFR[phiSlice];
  }
  else if(outer){
    eff=fMCEffOuterRRange_r1inFR[phiSlice];
  }

  return eff;
}

Double_t ScaleFactorMethod::ReturnEfficiencyR1R2inFR(Double_t radius, Double_t phi){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Bool_t inner=false;
  Bool_t outer=false;
  if(radius>115.8 && radius<173.)inner=true;
  if(radius>=173 && radius<258.)outer=true;
  Double_t phiDeg=phi*360/(2*TMath::Pi());
  if(phiDeg<0.)phiDeg=360+phiDeg;
  int phiSlice=phiDeg/45.;
  Double_t eff;
  if(inner){
    eff=fMCEffInnerRRange_r1r2inFR[phiSlice];
  }
  else if(outer){
    eff=fMCEffOuterRRange_r1r2inFR[phiSlice];
  }
 
  return eff;
}


Double_t ScaleFactorMethod::ReturnEfficiencyR1inFR_MC(Double_t radius, Double_t phi){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
   std::string hname;
  Bool_t inner=false;
  Bool_t outer=false;
  if(radius<173. )inner=true;
  if(radius>=173.)outer=true;
  Double_t phiDeg=phi*360./(2*TMath::Pi());
  if(phiDeg<0.)phiDeg=360+phiDeg;
  int phiSliceMC=phiDeg/45.;
  Double_t eff;
 
  if(inner){
    eff=fDataEffInnerRRange_r1inFR[phiSliceMC]/fMCEffInnerRRange_r1inFR[phiSliceMC];
  }
  else if(outer){
    eff=fDataEffOuterRRange_r1inFR[phiSliceMC]/fMCEffOuterRRange_r1inFR[phiSliceMC];
  }
  
  return eff;
}

Double_t ScaleFactorMethod::ReturnEfficiencyR1R2inFR_MC(Double_t radius, Double_t phi){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Bool_t inner=false;
  Bool_t outer=false;
  if(radius>115.8 && radius<173.)inner=true;
  if(radius>=173 && radius<258.)outer=true;
  Double_t phiDeg=phi*360/(2*TMath::Pi());
  if(phiDeg<0.)phiDeg=360+phiDeg;
  int phiSliceMC=phiDeg/45.;
  Double_t eff;
  if(inner){
    eff=fDataEffInnerRRange_r1r2inFR[phiSliceMC]/fMCEffInnerRRange_r1r2inFR[phiSliceMC];
  }
  else if(outer){
    eff=fDataEffOuterRRange_r1r2inFR[phiSliceMC]/fMCEffOuterRRange_r1r2inFR[phiSliceMC];
  }
 
  return eff;
}



Bool_t ScaleFactorMethod::InitHistos(){
  
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  int binX=500;
  double minX=0;
  double maxX=2000;
  int binY=500;
  double minY=0;
  double maxY=2000;

  hname="MethodScaleFactor_MC";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MethodScaleFactor_MCg1inFR";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MethodScaleFactor_MCg1g2inFR";
  hSvc->BookHisto(hname, binX, minX, maxX);


  std::vector<std::string> sufixW;
  sufixW.push_back("MCSample_usualW");
  for(int i=0; i<sufixW.size(); i++){
    binX=500;
    minX=0.;
    maxX=2000.;
    hname="MethodScaleFactor_WEffR1inFR"+sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
    hname="MethodScaleFactor_WEffR1R2inFR"+sufixW.at(i);
    hSvc->BookHisto(hname, binX, minX, maxX);
  }

 std::vector<std::string> sufixW1;
 sufixW1.push_back("MCSample_ScaleFactorW");
 for(int i=0; i<sufixW1.size(); i++){
   binX=500;
   minX=0.;
   maxX=2000.;
   hname="MethodScaleFactor_WEffR1inFR"+sufixW1.at(i);
   hSvc->BookHisto(hname, binX, minX, maxX);
   hname="MethodScaleFactor_WEffR1R2inFR"+sufixW1.at(i);
   hSvc->BookHisto(hname, binX, minX, maxX);
   }


 return true;
}



