#include "CalchepTruthStudies_TPComparison.hh"

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



CalchepTruthStudies_TPComparison::CalchepTruthStudies_TPComparison()
{
  
  fRecoEvent   = NULL;

  fECal_hitEvent=0;
  fECal_ClColl=0;

  fVerbose        = 0;

 
}
CalchepTruthStudies_TPComparison::CalchepTruthStudies_TPComparison(Int_t processingMode, Int_t verbosityFlag)
{
  fRecoEvent      = NULL;
  fVerbose        = verbosityFlag;
  fInitToComplete =true;

}
CalchepTruthStudies_TPComparison::~CalchepTruthStudies_TPComparison()
{
}

Bool_t CalchepTruthStudies_TPComparison::Init(TRecoEvent* eventHeader, 
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

  fminPhiTag=45;
  fmaxPhiTag=90;
  fminPhiTagTruth=45;
  fmaxPhiTagTruth=90;
  fminPhiProbeTruth=225;
  fmaxPhiProbeTruth=270;


  Double_t tmpradiusRangeMin[2]={fFRmin, fFRmid};
  Double_t tmpradiusRangeMax[2]={fFRmid, fFRmax};
  Double_t tmpphiRange[9]={0., 45., 90., 135., 180., 225., 270., 315., 360.};      
  for(int i=0; i<9; i++){
    if(i<2){
      radiusRangeMin[i]=tmpradiusRangeMin[i];
      radiusRangeMax[i]=tmpradiusRangeMax[i];
    }
    phiRange[i]=tmpphiRange[i];
  }

  
  for(int i=0; i<2; i++){
    nTag[i]=0;
    nTag_ifTruth[i]=0;
    nTag_ifTruthOutRIn[i]=0;
    nTag_ifTruthOutRout[i]=0;
    nTag_noProbe[i]=0;
    nTruth_noProbe[i]=0;
    nTruthOutRIn_noProbe[i]=0;
    nTruthOutRout_noProbe[i]=0;

    nProbe25[i]=0;
    noProbe25[i]=0;
    nProbe25inbin_ifTruth[i]=0;
    nProbe25inbin_ifTruthOutRIn[i]=0;
    nProbe25inbin_ifTruthOutRout[i]=0;
    nProbe25OutRIn_ifTruth[i]=0;
    nProbe25OutRIn_ifTruthOutRIn[i]=0;
    nProbe25OutRIn_ifTruthOutRout[i]=0;
    nProbe25OutROut_ifTruth[i]=0;
    nProbe25OutROut_ifTruthOutRIn[i]=0;
    nProbe25OutROut_ifTruthOutRout[i]=0;
    nProbe25_noTruthinPhi[i]=0;
  }

  fCalchep450.open("ee-gg_450MeV_NoPipe.txt");
  fCalchep430.open("ee-gg_430MeV_NoPipe.txt");

  return true;
}


Bool_t CalchepTruthStudies_TPComparison::Process()
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


Bool_t CalchepTruthStudies_TPComparison::InitHistos()
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
  //hname="CalchepVar_Phi1_g1inFR";
  //hSvc->BookHisto(hname, binX, minX, maxX);
  

  return true;
}



void CalchepTruthStudies_TPComparison::SortCluster_byEnergy(){

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


void CalchepTruthStudies_TPComparison::getCalchepTruth(){
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
  if(g1phi<0.){g1phi=360+g1phi;  }//std::cout<<"g1<0, so after correction " <<  g1phi << std::endl;}
  if(g2phi<0.)g2phi=360+g2phi;
 
  CountersExtraction(g1Recal, g1phi,g1E, g2Recal, g2phi, g2E);

  
}


void CalchepTruthStudies_TPComparison::CountersExtraction(Double_t rg1T,Double_t phig1, Double_t e1T, Double_t rg2T, Double_t phig2, Double_t e2T){ 

  TRecoVCluster* ecalclu =NULL;
  TRecoVCluster* ecalclu2 =NULL;
  Double_t constantE=6.39845;///from calchep truth, for simulation of calchep. E Beam 450 MeV
  Double_t slopeE=-2.05831e+01;
  Double_t EBeam=430.;
  for(int i=0; i< fECal_ClColl->GetNElements(); i++){
    ecalclu       = fECal_ClColl->Element(fIdCl_SortByEnergy.at(i));
    Double_t cl1e  = ecalclu->GetEnergy();
    Double_t cl1x = ecalclu->GetPosition().X();
    Double_t cl1y = ecalclu->GetPosition().Y();
    TVector3 pos1 = ecalclu->GetPosition();
    Double_t R_1 = sqrt(cl1x*cl1x+ cl1y*cl1y+fdistanceTarget*fdistanceTarget);
    Double_t Px_1 = cl1e*cl1x/ R_1;
    Double_t Py_1 = cl1e*cl1y/ R_1;
    Double_t Pz_1 = cl1e*fdistanceTarget/ R_1;
    TLorentzVector P4g1F;
    P4g1F.SetPxPyPzE(Px_1,Py_1,Pz_1, cl1e);
    Double_t thetag1 = P4g1F.Theta();

    Double_t cl1r=sqrt(cl1x*cl1x+cl1y*cl1y);
    Double_t phi1=TMath::ATan2(cl1y, cl1x)*360./2./TMath::Pi();
    if(phi1<0.)phi1=360.+phi1;

    Double_t EnergyFunc=exp(constantE+slopeE*thetag1);
    Double_t DeltaEnergyFunction= -(EnergyFunc-cl1e);
    Double_t EnergyFuncExpSecondPhoton=EBeam-EnergyFunc;
    int probeIndex=returnChiBetterClIndex(i, constantE, slopeE, EnergyFuncExpSecondPhoton);
    if(phi1>=fminPhiTag && phi1 < fmaxPhiTag){
      if(cl1r>fFRmin && cl1r<fFRmid){
	fillCountersTagInner(rg1T,phig1,rg2T,phig2);
	if(probeIndex==-99){fillCountersInnerNoProbe(rg1T,phig1,e1T, rg2T,phig2,e2T, cl1r, phi1, cl1e); }
      }
      if(cl1r>=fFRmid && cl1r<fFRmax){
	fillCountersTagOuter(rg1T,phig1,rg2T,phig2);
	if(probeIndex==-99){fillCountersOuterNoProbe(rg1T,phig1,e1T, rg2T,phig2,e2T, cl1r, phi1, cl1e);}
      }
    }//end of phi cluster request
    if(probeIndex==-99)continue;
    ecalclu2 = fECal_ClColl->Element(probeIndex);
    Double_t cl2e  = ecalclu2->GetEnergy();
    Double_t cl2x = ecalclu2->GetPosition().X();
    Double_t cl2y = ecalclu2->GetPosition().Y();
    TVector3 pos2 = ecalclu2->GetPosition();
    Double_t cl2r=sqrt(cl2x*cl2x+cl2y*cl2y);
    Double_t phi2=TMath::ATan2(cl2y, cl2x)*360./2./TMath::Pi();
    if(phi2<0.)phi2=360.+phi2;
    
    double deltaphi=0.;
    bool phiSym = phiSymmetricalInECal(pos1, pos2, deltaphi);
    Bool_t dPhiIn25Degree=false; //0.44 rad 
    if(fabs(deltaphi-3.14)<0.44) dPhiIn25Degree=true;
    //if(fabs(deltaphi-3.14)<0.78) dPhiIn25Degree=true; //it is in 45 deg like calchep studies

    if(dPhiIn25Degree){
      if(phi1>= fminPhiTag&& phi1 < fmaxPhiTag){
	if(cl1r>fFRmin && cl1r<fFRmid) fillCountersProbeIn25_TagInner(rg1T,phig1,e1T, rg2T,phig2,e2T, cl2r, phi2, cl2e);
	if(cl1r>=fFRmid && cl1r<fFRmax)fillCountersProbeIn25_TagOuter(rg1T,phig1,e1T, rg2T,phig2,e2T, cl2r, phi2, cl2e);
      }
    }
    else {
      if(phi1>=fminPhiTag && phi1 <fmaxPhiTag ){
	if(cl1r>fFRmin && cl1r<fFRmid) noProbe25[0]++;
	if(cl1r>=fFRmid && cl1r<fFRmax)noProbe25[1]++;
      }
    }
  }//end of cluster loop
  
}


void CalchepTruthStudies_TPComparison::fillCountersTagInner(Double_t rg1T,Double_t phig1, Double_t rg2T, Double_t phig2){
  nTag[0]++;
  if(phig1>= fminPhiTagTruth && phig1 < fmaxPhiTagTruth){
    if(rg1T>fFRmin && rg1T<fFRmid) nTag_ifTruth[0]++;
    if(rg1T<=fFRmin) nTag_ifTruthOutRIn[0]++;
    if(rg1T>=fFRmid) nTag_ifTruthOutRout[0]++;
  }//end phi1Truth request
  if(phig2>=fminPhiTagTruth && phig2 < fmaxPhiTagTruth){
    if(rg2T>fFRmin && rg2T<fFRmid) nTag_ifTruth[0]++;
    if(rg2T<=fFRmin ) nTag_ifTruthOutRIn[0]++;
    if(rg2T>=fFRmid ) nTag_ifTruthOutRout[0]++;
  }//end phi2Truth request
}

void CalchepTruthStudies_TPComparison::fillCountersTagOuter(Double_t rg1T,Double_t phig1, Double_t rg2T, Double_t phig2){
  nTag[1]++;
  if(phig1>=fminPhiTagTruth && phig1 < fmaxPhiTagTruth){
    if(rg1T>=fFRmid && rg1T<fFRmax) nTag_ifTruth[1]++;
    if(rg1T<fFRmid ) nTag_ifTruthOutRIn[1]++;
    if(rg1T>=fFRmax) nTag_ifTruthOutRout[1]++;
  }//end phi1Truth request
  if(phig2>=fminPhiTagTruth && phig2 < fmaxPhiTagTruth){
    if(rg2T>=fFRmid && rg2T<fFRmax)nTag_ifTruth[1]++;
    if(rg2T<fFRmid ) nTag_ifTruthOutRIn[1]++;
    if(rg2T>=fFRmax ) nTag_ifTruthOutRout[1]++;
  }//end phi2Truth request
}


void CalchepTruthStudies_TPComparison::fillCountersInnerNoProbe(Double_t rg1T,Double_t phig1, Double_t e1T, Double_t rg2T, Double_t phig2,Double_t e2T, Double_t  cl1r, Double_t phi1, Double_t cl1e){
  nTag_noProbe[0]++;
  Double_t dE1=e1T-cl1e;
  Double_t dE2=e2T-cl1e;
  bool isFirst=false;
  if(dE1<dE2)isFirst=true;
  if(isFirst){ //tag is the g1 in truth, so i'm missing the g2
    if(phig2>=fminPhiProbeTruth && phig2 < fmaxPhiProbeTruth){
      if(rg2T>fFRmin && rg2T<fFRmid) nTruth_noProbe[0]++;
      if(rg2T<=fFRmin ) nTruthOutRIn_noProbe[0]++;
      if(rg2T>=fFRmid ) nTruthOutRout_noProbe[0]++;
    }//end phi2Truth request
  }
  else{
    if(phig1>=fminPhiProbeTruth && phig1 < fmaxPhiProbeTruth){
      if(rg1T>fFRmin && rg1T<fFRmid) nTruth_noProbe[0]++;
      if(rg1T<=fFRmin ) nTruthOutRIn_noProbe[0]++;
      if(rg1T>=fFRmid ) nTruthOutRout_noProbe[0]++;
    }//end phi1Truth request
  }
}


void CalchepTruthStudies_TPComparison::fillCountersOuterNoProbe(Double_t rg1T,Double_t phig1, Double_t e1T, Double_t rg2T, Double_t phig2,Double_t e2T, Double_t  cl1r, Double_t phi1, Double_t cl1e){
  nTag_noProbe[1]++;
  Double_t dE1=e1T-cl1e;
  Double_t dE2=e2T-cl1e;
  bool isFirst=false;
  if(dE1<dE2)isFirst=true;
  if(isFirst){ //tag is the g1 in truth, so i'm missing the g2
    if(phig2>=fminPhiProbeTruth && phig2 < fmaxPhiProbeTruth){
      if(rg2T>=fFRmid && rg2T<fFRmax) nTruth_noProbe[1]++;
      if(rg2T<fFRmid ) nTruthOutRIn_noProbe[1]++;
      if(rg2T>=fFRmax ) nTruthOutRout_noProbe[1]++;
    }//end phi2Truth request
  }
  else{
    if(phig1>=fminPhiProbeTruth && phig1 < fmaxPhiProbeTruth){
      if(rg1T>=fFRmid && rg1T<fFRmax) nTruth_noProbe[1]++;
      if(rg1T<fFRmid ) nTruthOutRIn_noProbe[1]++;
      if(rg1T>=fFRmax ) nTruthOutRout_noProbe[1]++;
    }//end phi1Truth request
  }
}


void CalchepTruthStudies_TPComparison::fillCountersProbeIn25_TagInner(Double_t rg1T,Double_t phig1, Double_t e1T, Double_t rg2T, Double_t phig2,Double_t e2T, Double_t  cl2r, Double_t phi2, Double_t cl2e){
  nProbe25[0]++;
  Double_t dE1=fabs(e1T-cl2e);
  Double_t dE2=fabs(e2T-cl2e);
  bool isFirst=false;
  if(dE1<dE2)isFirst=true;
  if(isFirst){//probe is g1, so i'm looking where is the first photon
    if(phig1>=fminPhiProbeTruth && phig1 <fmaxPhiProbeTruth){
      if(cl2r>=fFRmid && cl2r<fFRmax){
	if(rg1T>=fFRmid && rg1T<fFRmax) nProbe25inbin_ifTruth[0]++;
	if(rg1T<fFRmid) nProbe25inbin_ifTruthOutRIn[0]++;
	if(rg1T>=fFRmax) nProbe25inbin_ifTruthOutRout[0]++;
      }//end of probe in bin
      if(cl2r<fFRmid){
	if(rg1T>=fFRmid && rg1T<fFRmax) nProbe25OutRIn_ifTruth[0]++;
	if(rg1T<fFRmid)  nProbe25OutRIn_ifTruthOutRIn[0]++;
	if(rg1T>=fFRmax) nProbe25OutRIn_ifTruthOutRout[0]++;
      }//end of probe out rin
      if(cl2r>=fFRmax){
	if(rg1T>=fFRmid && rg1T<fFRmax) nProbe25OutROut_ifTruth[0]++;
	if(rg1T<fFRmid) nProbe25OutROut_ifTruthOutRIn[0]++;
	if(rg1T>=fFRmax) nProbe25OutROut_ifTruthOutRout[0]++;
      }//end of probe out rmid
    }//end phi1Truth request
    else nProbe25_noTruthinPhi[0]++;
  
  }//end of if on if it is first photon

  else{//i shoud do the same check on second photon
    if(phig2>=fminPhiProbeTruth && phig2 <fmaxPhiProbeTruth){
      if(cl2r>=fFRmid && cl2r<fFRmax){
	if(rg2T>=fFRmid && rg2T<fFRmax) nProbe25inbin_ifTruth[0]++;
	if(rg2T<fFRmid) nProbe25inbin_ifTruthOutRIn[0]++;
	if(rg2T>=fFRmax) nProbe25inbin_ifTruthOutRout[0]++;
      }//end of probe in bin
      if(cl2r<fFRmid){
	if(rg2T>=fFRmid && rg2T<fFRmax) nProbe25OutRIn_ifTruth[0]++;
	if(rg2T<fFRmid)  nProbe25OutRIn_ifTruthOutRIn[0]++;
	if(rg2T>=fFRmax) nProbe25OutRIn_ifTruthOutRout[0]++;
      }//end of probe out rin
      if(cl2r>=fFRmax){
	if(rg2T>=fFRmid && rg2T<fFRmax) nProbe25OutROut_ifTruth[0]++;
	if(rg2T<fFRmid) nProbe25OutROut_ifTruthOutRIn[0]++;
	if(rg2T>=fFRmax) nProbe25OutROut_ifTruthOutRout[0]++;
      }//end of probe out rmid
    }//end phi1Truth request
    else nProbe25_noTruthinPhi[0]++;
  }

}


void CalchepTruthStudies_TPComparison::fillCountersProbeIn25_TagOuter(Double_t rg1T,Double_t phig1, Double_t e1T, Double_t rg2T, Double_t phig2,Double_t e2T, Double_t  cl2r, Double_t phi2, Double_t cl2e){
  nProbe25[1]++;
  Double_t dE1=fabs(e1T-cl2e);
  Double_t dE2=fabs(e2T-cl2e);
  //std::cout<<"g1E " << e1T << " g2 " << e2T << " second cluster " << cl2e <<" de1 " << dE1 << " de2 " << dE2 << std::endl;
  bool isFirst=false;
  if(dE1<dE2)isFirst=true;
  //std::cout<<"tagOuterProbe25,is first " << isFirst << " probeRadius " << cl2r << " probePhi " << phi2 << " rg1 " << rg1T << " rg2 " << rg2T << " phig1 " <<phig1 << " phig2 " << phig2 << std::endl;
  if(isFirst){//probe is g1, so i'm looking where is the first photon
    if(phig1>= fminPhiProbeTruth&& phig1 <fmaxPhiProbeTruth){	    
      if(cl2r>fFRmin && cl2r<fFRmid){
	if(rg1T>fFRmin && rg1T<fFRmid) nProbe25inbin_ifTruth[1]++;
	if(rg1T<=fFRmin) nProbe25inbin_ifTruthOutRIn[1]++;
	if(rg1T>=fFRmid) nProbe25inbin_ifTruthOutRout[1]++;
      }//end of probe in bin
      if(cl2r<=fFRmin){
	if(rg1T>fFRmin && rg1T<fFRmid) nProbe25OutRIn_ifTruth[1]++;
	if(rg1T<=fFRmin) nProbe25OutRIn_ifTruthOutRIn[1]++;
	if(rg1T>=fFRmid) nProbe25OutRIn_ifTruthOutRout[1]++;
    }//end of probe out rin
      if(cl2r>=fFRmid){
      	if(rg1T>fFRmin && rg1T<fFRmid) nProbe25OutROut_ifTruth[1]++;
	if(rg1T<=fFRmin) nProbe25OutROut_ifTruthOutRIn[1]++;
	if(rg1T>=fFRmid) nProbe25OutROut_ifTruthOutRout[1]++;
      }//end of probe out rmid
    }//end phi1Truth request
    else nProbe25_noTruthinPhi[1]++;
  }//end of if on if it is first photon

  else{//i shoud do the same check on second photon
    if(phig2>=fminPhiProbeTruth && phig2 <fmaxPhiProbeTruth){
      if(cl2r>fFRmin && cl2r<fFRmid){
	if(rg2T>fFRmin && rg2T<fFRmid) nProbe25inbin_ifTruth[1]++;
	if(rg2T<=fFRmin) nProbe25inbin_ifTruthOutRIn[1]++;
	if(rg2T>=fFRmid) nProbe25inbin_ifTruthOutRout[1]++;
    }//end of probe in bin
      if(cl2r<=fFRmin){
      	if(rg2T>fFRmin && rg2T<fFRmid) nProbe25OutRIn_ifTruth[1]++;
	if(rg2T<=fFRmin) nProbe25OutRIn_ifTruthOutRIn[1]++;
	if(rg2T>=fFRmid) nProbe25OutRIn_ifTruthOutRout[1]++;
      }//end of probe out rin
      if(cl2r>=fFRmid){
	if(rg2T>fFRmin && rg2T<fFRmid) nProbe25OutROut_ifTruth[1]++;
	if(rg2T<=fFRmin) nProbe25OutROut_ifTruthOutRIn[1]++;
	if(rg2T>=fFRmid) nProbe25OutROut_ifTruthOutRout[1]++;
      }//end of probe out rmid
    }//end phi1Truth request
    else nProbe25_noTruthinPhi[1]++;
   
    
  }

}



int CalchepTruthStudies_TPComparison::returnChiBetterClIndex(int firstClIndex, Double_t constantE, Double_t slopeE, Double_t EnergyFuncExpSecondPhoton){

  TRecoVCluster* ecalclu2 =NULL;
  int minClIndice=-99;
  Double_t minChi=9999999.;
  for(Int_t jecal2=0; jecal2< fECal_ClColl->GetNElements(); jecal2++){        
    if(jecal2==fIdCl_SortByEnergy.at(firstClIndex))continue;
      ecalclu2 = fECal_ClColl->Element(jecal2);
      TVector3 _pos2 = ecalclu2->GetPosition();
      Double_t _g2x=ecalclu2->GetPosition().X();
      Double_t _g2y=ecalclu2->GetPosition().Y();
      Double_t _g2E=ecalclu2->GetEnergy();
      Double_t _R_2 = sqrt(_g2x*_g2x+ _g2y*_g2y+fdistanceTarget*fdistanceTarget);
      Double_t _Px_2 = _g2E*_g2x/ _R_2;
      Double_t _Py_2 = _g2E*_g2y/ _R_2;
      Double_t _Pz_2 = _g2E*fdistanceTarget/ _R_2;
      TLorentzVector g2F;
      g2F.SetPxPyPzE(_Px_2,_Py_2,_Pz_2,_g2E);
      Double_t _thetag2 = g2F.Theta();
	
      Double_t EnergyFunc2=exp(constantE+slopeE*_thetag2);
      Double_t DeltaEnergyFunction2= -(EnergyFunc2-_g2E);
      Double_t DeltaEnGamma12=-(EnergyFuncExpSecondPhoton-_g2E);

      double mychi2=(DeltaEnGamma12*DeltaEnGamma12 + DeltaEnergyFunction2*DeltaEnergyFunction2)/(sqrt(2)*15)/(sqrt(2)*15);  

      if(minChi>mychi2) {
	minChi=mychi2;
	minClIndice=jecal2;
      }

    }//end of loop on clusters 
  return minClIndice;

}


void CalchepTruthStudies_TPComparison::Terminate(){
  std::cout<<"calchep truth T&P succesfully terminated" <<std::endl;
  printCounters();
  
}




void CalchepTruthStudies_TPComparison::printCounters(){ 

  std::cout<<"--------range--------- [ " << radiusRangeMin[0] << " , " << radiusRangeMax[0] << " ]" << std::endl;
  std::cout<<"phi in [  ,  ] ,   nTag    ,  nTag_ifTRuth,    nTag_ifTruthOutRin,   nTag_ifTruthOutRout"<<std::endl;
  std::cout << "[45,90] " << nTag[0] << " " <<nTag_ifTruth[0]<<" " << nTag_ifTruthOutRIn[0]<< " " << nTag_ifTruthOutRout[0] << std::endl; 
  std::cout<<"nTag_noProbe , nTruth_noProbe   ,     nTruthOutRIn_noProbe,    nTruthOutRout_noProbe" << std::endl;
  std::cout<<nTag_noProbe[0]<<" "<< nTruth_noProbe[0] << " "<< nTruthOutRIn_noProbe[0] << " "<< nTruthOutRout_noProbe[0] << std::endl;
  std::cout<<"Probein25Deg " << nProbe25[0] << " out25Deg " << noProbe25[0] <<" nProbe25_noTruthinPhi " << nProbe25_noTruthinPhi[0] << std::endl;
  std::cout << "nProbe25inbin_ifTruth  ,  nProbe25inbin_ifTruthOutRIn  ,  nProbe25inbin_ifTruthOutRout  ,  nProbe25OutRIn_ifTruth  ,  nProbe25OutRIn_ifTruthOutRIn  ,  nProbe25OutRIn_ifTruthOutRout  ,  nProbe25OutROut_ifTruth  ,  nProbe25OutROut_ifTruthOutRIn  ,   nProbe25OutROut_ifTruthOutRout" << std::endl;
  std::cout << nProbe25inbin_ifTruth[0] << " " << nProbe25inbin_ifTruthOutRIn[0] << " " <<  nProbe25inbin_ifTruthOutRout[0] << " " <<  nProbe25OutRIn_ifTruth[0] << " " <<  nProbe25OutRIn_ifTruthOutRIn[0] << " " <<  nProbe25OutRIn_ifTruthOutRout[0] << " " <<  nProbe25OutROut_ifTruth[0] << " " <<  nProbe25OutROut_ifTruthOutRIn[0] << " " <<  nProbe25OutROut_ifTruthOutRout[0] << std::endl;

  std::cout<<"--------range--------- [ " << radiusRangeMin[1] << " , " << radiusRangeMax[1] << " ]" << std::endl;
  std::cout<<"phi in [  ,  ] ,   nTag    ,  nTag_ifTRuth,    nTag_ifTruthOutRin,   nTag_ifTruthOutRout"<<std::endl;
  std::cout << "[45,90] " << nTag[1] << " "<< nTag_ifTruth[1]<<" " << nTag_ifTruthOutRIn[1]<< " " << nTag_ifTruthOutRout[1] << std::endl;
  std::cout<<"nTag_noProbe , nTruth_noProbe   ,     nTruthOutRIn_noProbe,    nTruthOutRout_noProbe" << std::endl;
  std::cout<<nTag_noProbe[1]<<" "<< nTruth_noProbe[1] << " "<< nTruthOutRIn_noProbe[1] << " "<< nTruthOutRout_noProbe[1] << std::endl;
  std::cout<<"Probein25Deg " << nProbe25[1] << " out25Deg " << noProbe25[1] << " nProbe25_noTruthinPhi " << nProbe25_noTruthinPhi[1] << std::endl;
  std::cout << "nProbe25inbin_ifTruth  ,  nProbe25inbin_ifTruthOutRIn  ,  nProbe25inbin_ifTruthOutRout  ,  nProbe25OutRIn_ifTruth  ,  nProbe25OutRIn_ifTruthOutRIn  ,  nProbe25OutRIn_ifTruthOutRout  ,  nProbe25OutROut_ifTruth  ,  nProbe25OutROut_ifTruthOutRIn  ,   nProbe25OutROut_ifTruthOutRout" << std::endl;
  std::cout << nProbe25inbin_ifTruth[1] << " " << nProbe25inbin_ifTruthOutRIn[1] << " " <<  nProbe25inbin_ifTruthOutRout[1] << " " <<  nProbe25OutRIn_ifTruth[1] << " " <<  nProbe25OutRIn_ifTruthOutRIn[1] << " " <<  nProbe25OutRIn_ifTruthOutRout[1] << " " <<  nProbe25OutROut_ifTruth[1] << " " <<  nProbe25OutROut_ifTruthOutRIn[1] << " " <<  nProbe25OutROut_ifTruthOutRout[1] << std::endl;


}






Bool_t CalchepTruthStudies_TPComparison::phiSymmetricalInECal(TVector3 P1, TVector3 P2,  double& distPhi){
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
