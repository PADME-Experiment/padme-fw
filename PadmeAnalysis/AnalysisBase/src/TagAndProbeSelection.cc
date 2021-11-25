#include "TagAndProbeSelection.hh"

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


TagAndProbeSelection::TagAndProbeSelection()
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
TagAndProbeSelection::TagAndProbeSelection(Int_t processingMode, Int_t verbosityFlag, Bool_t NoTargetBool, Bool_t externalPass)
{
  fRecoEvent      = NULL;
  fVerbose        = verbosityFlag;
  fNoTargetBool   = NoTargetBool;
  fexternalPass= externalPass;
  fInitToComplete =true;

}
TagAndProbeSelection::~TagAndProbeSelection()
{
}

Bool_t TagAndProbeSelection::Init(TRecoEvent* eventHeader, 
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

  fdistanceTarget=1030+2513;//from calchep g4hits studies
  fFRmin=115.82;
  fFRmid=172.83;
  fFRmax=258.0;
 
  return true;
}


Bool_t TagAndProbeSelection::Process(Bool_t isMC, Bool_t makeClSelection, std::vector<int> selCl, Bool_t allPlot)
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  if (!passPreselection(fNoTargetBool, isMC)) return true;

  Double_t EBeam=0.;
  //if(isMC)EBeam=450.0;
  if(isMC)EBeam=430.0;
  else EBeam=420.;

  Bool_t retCode = 0;
  TRecoVCluster* ecalclu=NULL;
  TRecoVCluster* ecalclu2=NULL;
  //TRecoVHit* hit;
  Int_t NClECal = fECal_ClColl->GetNElements();
  TVector3 pos1;
  TVector3 pos2;
  double dPhi = -999.;
  /* Double_t p0=0.0737203;
     Double_t p1=-0.000108075;
     Double_t thetaFunc=p0 + g1E*p1;*/
  /*Double_t constant=-2.061;    ///from data July19.... e beam490 MeV
    Double_t slope=-0.004135;*/
  Double_t constant=-2.05819e+00;///from data July20.... e beam450 MeV
  Double_t slope=-4.41752e-03;
  //Double_t constantE=6.32430e+00;///from data July20.... e beam430 MeV... run294
  //Double_t slopeE=-1.98595e+01;
  Double_t constantE=6.39845;///from calchep truth, for simulation of calchep. E Beam 450 MeV
  Double_t slopeE=-2.05831e+01;
  Double_t constantR2=4.10703e+00;///from calchep truth, for simulation of calchep. E Beam 450 MeV. Correlation between Rgamma2 vs egamma1 
  Double_t slopeR2=4.55131e-03;
  //if(NClECal<2) return true;
  for (Int_t jecal=0; jecal<NClECal; ++jecal){
    ecalclu          = fECal_ClColl->Element(jecal);
    if(makeClSelection && selCl.at(jecal)<10)continue;
    Double_t g1x=ecalclu->GetPosition().X();
    Double_t g1y=ecalclu->GetPosition().Y();
    //    std::cout<<" cluster " << jecal << " xposition " << ecalclu->GetPosition().X() << std::endl;
    Double_t g1E=ecalclu->GetEnergy();
    hname="TagAndProbe_ECalClusterEnergy";
    hSvc->FillHisto(hname, g1E, 1.);
    Int_t g1seedChId=fECal_hitEvent->Hit(ecalclu->GetSeed())->GetChannelId();//hit(ecalclu->GetSeed())->GetChannelId();
    Double_t g1t=ecalclu->GetTime();
    //if(g1t<-110) continue;//high energy spread in data October 2020
    Double_t R_1 = sqrt(g1x*g1x+ g1y*g1y+fdistanceTarget*fdistanceTarget);
    Double_t g1Recal=sqrt(g1x*g1x+g1y*g1y);
    Double_t Px_1 = g1E*g1x/ R_1;
    Double_t Py_1 = g1E*g1y/ R_1;
    Double_t Pz_1 = g1E*fdistanceTarget/ R_1;
    TLorentzVector P4g1F, P4eTarget, P4eBeam;
    P4g1F.SetPxPyPzE(Px_1,Py_1,Pz_1, g1E);
    Double_t me=0.511;
    Double_t PzBeam=sqrt(EBeam*EBeam-me*me);
    P4eTarget.SetPxPyPzE(0,0,0, me);
    P4eBeam.SetPxPyPzE(0,0,PzBeam, EBeam);
    Double_t MissingMass=(P4eTarget+P4eBeam-P4g1F)*(P4eTarget+P4eBeam-P4g1F);

    Double_t thetag1 = P4g1F.Theta();
    Double_t pi=3.14159265358979;
    pos1 = ecalclu->GetPosition();
    double phi1 = pos1.Phi()*180/pi;
    if(phi1<0.)phi1=360+phi1;
    //std::cout<<"loop " << jecal<< " pos1 rad " << pos1.Phi() << " in deg " << phi1  << "energy " << g1E << std::endl;
     
    Double_t thetaFunc=exp(constant+slope*g1E);
    Double_t DeltaThetaFunction= -(thetaFunc-thetag1);
    Double_t EnergyFunc=exp(constantE+slopeE*thetag1);
    Double_t DeltaEnergyFunction= -(EnergyFunc-g1E);
    Double_t EnergyFuncExpSecondPhoton=EBeam-EnergyFunc;
    Double_t R2Func=exp(constantR2+slopeR2*g1E);
    if(allPlot){
      hname="ECal_DeltaThetaFuncAllCl";
      hSvc->FillHisto(hname,DeltaThetaFunction, 1. );
      if(g1E>90.){
	hname="ECal_DeltaThetaFuncAllCl_ethr90MeV";
	hSvc->FillHisto(hname,DeltaThetaFunction, 1. );
      }
      hname="ECal_DeltaEnergyFuncAllCl";
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }

    if(g1Recal>fFRmin && g1Recal < fFRmax){
      hname="ECal_DeltaEnergyFuncAllCl_inFR115.8_258";
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    


    if(g1Recal>fFRmin && g1Recal < fFRmid){
      hname="ECal_DeltaEnergyFuncAllCl_inFR115.8_173";
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    if(g1Recal>=fFRmid && g1Recal < fFRmax){
      hname="ECal_DeltaEnergyFuncAllCl_inFR173_258";
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }



    if(allPlot && g1E>90.){
      hname="ECal_DeltaEnergyFuncAllCl_ethr90MeV";
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
      if(g1Recal>fFRmin && g1Recal < fFRmax){
	hname="ECal_DeltaEnergyFuncAllCl_ethr90MeV_inFR115.8_258";
	hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
      }
    }

    if(allPlot)FillHistoTagAndProbe_RadiusRegionsTag(DeltaEnergyFunction, g1Recal, "");
    if(allPlot && g1E>90.) FillHistoTagAndProbe_RadiusRegionsTag(DeltaEnergyFunction, g1Recal, "EThr90MeV");
    if(g1Recal>fFRmin && g1Recal < fFRmax){
      if(allPlot)FillHistoTagAndProbe_PhiRegionsTag(DeltaEnergyFunction, phi1, "_inFR115.8_258");
      FillMM2AsAFunctionOfPhi(MissingMass, phi1 , g1Recal, "");
    }
    if(allPlot && g1Recal>fFRmin && g1Recal < fFRmax && g1E>90.)FillHistoTagAndProbe_PhiRegionsTag(DeltaEnergyFunction, phi1, "EThr90MeV_inFR115.8_258");
    FillHistoTagAndProbe_PhiRegionsCalchep430RadiusRegionsTag(DeltaEnergyFunction,phi1 , g1Recal, "_Rmid173" );
    if(allPlot && g1E>90.)FillHistoTagAndProbe_PhiRegionsCalchep430RadiusRegionsTag(DeltaEnergyFunction,phi1 , g1Recal, "_EThr90MeV_Rmid173" );
    int probeCountIn25fr=0;
    if(allPlot && g1E< 90. ) { hname="ECal_TagR2expected_ethr90MeV";hSvc->FillHisto(hname,R2Func, 1. );}
    //if(fabs(DeltaEnergyFunction)<50.){
    Double_t XWeighted=0.;
    Double_t YWeighted=0.;
    Double_t g2x=0.;
    Double_t g2y=0.;
    Double_t g2t=0.;
    Double_t g2Recal=0.;
    Double_t g2E=0.;
    Double_t R_2 = 0.;
    Double_t Px_2 =0.;
    Double_t Py_2 =0.;
    Double_t Pz_2 =0.;
    TLorentzVector P4g2F,SumP;
    Double_t angleBetweenTwoPhoton = .0;
    Double_t thetag2 = 0.;
    double phi2 =0.;
    Double_t EnergyFunc2=0.;
    Double_t DeltaEnergyFunction2=0.;
    Double_t DeltaEnGamma12=0.;
    Double_t minChi=9999999.;
    int minClIndice=-99;
    for(Int_t jecal2=0; jecal2<NClECal; jecal2++){                ///starting second cluster loop
      if(jecal2==jecal)continue;
      if(makeClSelection && selCl.at(jecal2)<10)continue;
      if(g1E< 90. )continue;
      if(fabs(DeltaEnergyFunction)>100.)continue;
      //if(fabs(DeltaEnergyFunction)>25.)continue;  // test 8/04/2021 to try to have a probe for a good tag (to delete the tail)
      //if(fabs(DeltaEnergyFunction)>30.)continue;  // test 15/06/2021 to try to have a probe for a good tag (to delete the tail)understand the TP bias
      ecalclu2 = fECal_ClColl->Element(jecal2);
      TVector3 _pos2 = ecalclu2->GetPosition();
      Double_t _g2x=ecalclu2->GetPosition().X();
      Double_t _g2y=ecalclu2->GetPosition().Y();
      Double_t _g2E=ecalclu2->GetEnergy();
      Double_t _g2t=ecalclu2->GetTime();
      //if(_g2t<-110) continue;//high energy spread in data October 2020
      Double_t _R_2 = sqrt(_g2x*_g2x+ _g2y*_g2y+fdistanceTarget*fdistanceTarget);
      Double_t _Px_2 = _g2E*_g2x/ _R_2;
      Double_t _Py_2 = _g2E*_g2y/ _R_2;
      Double_t _Pz_2 = _g2E*fdistanceTarget/ _R_2;
      TLorentzVector g2F;
      g2F.SetPxPyPzE(_Px_2,_Py_2,_Pz_2,_g2E);
      Double_t _thetag2 = g2F.Theta();
	
      EnergyFunc2=exp(constantE+slopeE*_thetag2);
      DeltaEnergyFunction2= -(EnergyFunc2-_g2E);
      DeltaEnGamma12=-(EnergyFuncExpSecondPhoton-_g2E);

      //double mychi2=(DeltaEnGamma12*DeltaEnGamma12)/g2E;
      //double mychi2=(DeltaEnGamma12*DeltaEnGamma12)/(sqrt(2)*15)/(sqrt(2)*15);   //I'm considering both the photon with 15 MeV of resolution
      double mychi2=(DeltaEnGamma12*DeltaEnGamma12 + DeltaEnergyFunction2*DeltaEnergyFunction2)/(sqrt(2)*15)/(sqrt(2)*15);   //I'm considering both the photon with 15 MeV of resolution + the fact that also that photon is should follow the correlation
      //std::cout << "I passed the selection for probe and, Cl2 energy " << _g2E << " using function " << EnergyFunc2 << " difference " << DeltaEnergyFunction2 << " expected using cl1 diff " << DeltaEnGamma12 << " mychi2 " << mychi2 << std::endl; 
      if(fabs(g1t-_g2t)>7.)continue; //{std::cout << " this not pass probe time request  " << std::endl; continue;}
      if(_g2E< 90. )continue; // {std::cout << " this not pass probe energy request  " << std::endl; continue;}
      //if(fabs(DeltaEnergyFunction2)>50.)continue;//new entry, to avoid the combinatorial background; 24-11-2020
      if(fabs(DeltaEnergyFunction2)>100.)continue; //{std::cout << " this not pass probe deltaEnFunc request  " << std::endl; continue;}
      //std::cout<<"after the selection ....myChi2        " << mychi2 << " compared with minimum " << minChi << std::endl;
      if(allPlot){
	hname="ECAL_myChi2";
	hSvc->FillHisto(hname, mychi2, 1.);
	hname="ECAL_myChi2vsDEnExpProbe";
	hSvc->FillHisto2(hname,DeltaEnGamma12, mychi2, 1.);
	hname="ECAL_myChi2vsDEnTag";
	hSvc->FillHisto2(hname,DeltaEnergyFunction, mychi2, 1.);
      }
      if(minChi>mychi2) {
	minChi=mychi2;
	minClIndice=jecal2;
      }

    }//second loop

     if(minClIndice==-99) 
    //{
    //	//fill tag w/o probe histo (bkg)
    //	if(g1Recal>106. && g1Recal < fFRmax){
    //	  hname="ECal_DeltaEnergyFuncAllCl_inFR106_258_noMatchedProbe";
    //	  hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    //	  FillHistoTagAndProbe_PhiRegionsTag(DeltaEnergyFunction, phi1, "_inFR106_258_noMatchedProbe");
    //	}
    	continue;
    //}
    ecalclu2 = fECal_ClColl->Element(minClIndice);
    pos2 = ecalclu2->GetPosition();
    g2x=ecalclu2->GetPosition().X();
    g2y=ecalclu2->GetPosition().Y();
    g2t=ecalclu2->GetTime();
    g2Recal=sqrt(g2x*g2x+g2y*g2y);
    g2E=ecalclu2->GetEnergy();
    XWeighted= (g1x*g1E+g2x*g2E)/(g1E+g2E);
    YWeighted= (g1y*g1E+g2y*g2E)/(g1E+g2E);
    Double_t CoG= sqrt(XWeighted*XWeighted+YWeighted*YWeighted);
    R_2 = sqrt(g2x*g2x+ g2y*g2y+fdistanceTarget*fdistanceTarget);
    Px_2 = g2E*g2x/ R_2;
    Py_2 = g2E*g2y/ R_2;
    Pz_2 = g2E*fdistanceTarget/ R_2;
    //if(fabs(g1t-g2t)>7.)continue;
    //if(g2E< 90. ) continue;
    P4g2F.SetPxPyPzE(Px_2,Py_2,Pz_2,g2E);
    SumP=P4g1F+P4g2F;
    Double_t InvariantMass = SumP.M();
    angleBetweenTwoPhoton = P4g1F.Angle(P4g2F.Vect());
    thetag2 = P4g2F.Theta();
    phi2 = pos2.Phi()*180/pi;
    if(phi2<0.)phi2=360+phi2;

    EnergyFunc2=exp(constantE+slopeE*thetag2);
    DeltaEnergyFunction2= -(EnergyFunc2-g2E);
    DeltaEnGamma12=-(EnergyFuncExpSecondPhoton-g2E);

    double deltaphi=0.;
    bool phiSym = phiSymmetricalInECal(pos1, pos2, deltaphi);
    Bool_t dPhiIn25Degree=false; //0.44 rad 
    if(fabs(deltaphi-3.14)<0.44) dPhiIn25Degree=true;
    Bool_t dPhiIn30Degree=false; //0.52 rad 
    if(fabs(deltaphi-3.14)<0.52) dPhiIn30Degree=true;
    Bool_t dPhiIn35Degree=false; //0.61 rad 
    if(fabs(deltaphi-3.14)<0.61) dPhiIn35Degree=true;
    Bool_t dPhiIn40Degree=false; //0.70 rad 
    if(fabs(deltaphi-3.14)<0.70) dPhiIn40Degree=true;
    Bool_t dPhiIn45Degree=false; //0.78 rad 
    if(fabs(deltaphi-3.14)<0.78) dPhiIn45Degree=true;
    Bool_t dPhiIn50Degree=false; //0.87 rad 
    if(fabs(deltaphi-3.14)<0.87) dPhiIn50Degree=true;
    Bool_t dPhiIn70Degree=false; //1.22 rad 
    if(fabs(deltaphi-3.14)<1.22) dPhiIn70Degree=true;
    Bool_t dPhiIn90Degree=false; //1.57 rad 
    if(fabs(deltaphi-3.14)<1.57) dPhiIn90Degree=true;

    if(allPlot && g1Recal>fFRmin && g1Recal<fFRmax && g2Recal>fFRmin && g2Recal<fFRmax && dPhiIn25Degree) FillHistoTagAndProbe_PhiRegionsProbe(DeltaEnergyFunction2,phi1,"_inFR115.8_258");	
    if(allPlot){
      hname="ECAL_XCoGAllCl_InTagAndProbe";
      hSvc->FillHisto(hname,XWeighted, 1. );
      hname="ECAL_YCoGAllCl_InTagAndProbe";
      hSvc->FillHisto(hname,YWeighted, 1. );
    }
    if(allPlot && dPhiIn25Degree && g1E>90. && g2E> 90.){
      //FillHistoTagAndProbe_TagInPhiSegmentProbeInSameDir(DeltaEnergyFunction2,sectorTag);
      hname="ECAL_R1vsR2_25Deg_TagAndProbeMethod";
      hSvc->FillHisto2(hname,g1Recal,g2Recal, 1. );
      hname="ECAL_Phi1vsPhi2_25Deg_TagAndProbeMethod";
      hSvc->FillHisto2(hname,phi1,phi2, 1. );
    }
    std::string sufix="";
    if(allPlot)FillHistoTagAndProbe(DeltaEnergyFunction2,g2E, dPhiIn25Degree, dPhiIn30Degree, dPhiIn35Degree,  dPhiIn40Degree, dPhiIn45Degree, dPhiIn50Degree, dPhiIn70Degree, dPhiIn90Degree,sufix );
    
    if(allPlot && g1Recal>fFRmin && g1Recal<fFRmax && g2Recal>fFRmin && g2Recal<fFRmax){
      sufix="_inFR115.8_258";
      FillHistoTagAndProbe(DeltaEnergyFunction2,g2E, dPhiIn25Degree, dPhiIn30Degree, dPhiIn35Degree,  dPhiIn40Degree, dPhiIn45Degree, dPhiIn50Degree, dPhiIn70Degree, dPhiIn90Degree,sufix );
    }
    
    if(allPlot && dPhiIn25Degree)FillHistoTagAndProbe_PhiRegionsProbe(DeltaEnergyFunction2,phi1,"");
    if(allPlot && g1Recal>fFRmin && g1Recal<fFRmax && g2Recal>fFRmin && g2Recal<fFRmax && dPhiIn25Degree)FillHistoTagAndProbe_PhiRegionsProbe(DeltaEnergyFunction2,phi1,"_inFR115.8_258");

    if(allPlot && dPhiIn25Degree) FillHistoTagAndProbe_RadiusRegionsProbe(DeltaEnergyFunction2, g1Recal, g2Recal, "");
    if(allPlot && dPhiIn25Degree) FillHistoTagAndProbe_RadiusRegionsProbe(DeltaEnergyFunction2, g1Recal, "");

    sufix="_ExpEg2UsingE1";
    if(allPlot)FillHistoTagAndProbe(DeltaEnGamma12,g2E, dPhiIn25Degree, dPhiIn30Degree, dPhiIn35Degree,  dPhiIn40Degree, dPhiIn45Degree, dPhiIn50Degree, dPhiIn70Degree, dPhiIn90Degree,sufix );
   
    if(allPlot && g1Recal>fFRmin && g1Recal<fFRmax && g2Recal>fFRmin && g2Recal<fFRmax){
      sufix="_inFR115.8_258_ExpEg2UsingE1";
      FillHistoTagAndProbe(DeltaEnGamma12,g2E, dPhiIn25Degree, dPhiIn30Degree, dPhiIn35Degree,  dPhiIn40Degree, dPhiIn45Degree, dPhiIn50Degree, dPhiIn70Degree, dPhiIn90Degree,sufix );
    }
    if(g1Recal>fFRmin && g1Recal<fFRmax){
      sufix="_TAGinFR115.8_258_ExpEg2UsingE1";
      FillHistoTagAndProbe(DeltaEnGamma12,g2E, dPhiIn25Degree, dPhiIn30Degree, dPhiIn35Degree,  dPhiIn40Degree, dPhiIn45Degree, dPhiIn50Degree, dPhiIn70Degree, dPhiIn90Degree,sufix );
    }




    if(g1Recal>fFRmin && g1Recal<fFRmid && dPhiIn25Degree ){hname="ECal_DeltaEnergyFunc2_in25Deg_TAGinFR115.8_173_ExpEg2UsingE1"; hSvc->FillHisto(hname,DeltaEnGamma12,1.);}
    if(g1Recal>=fFRmid && g1Recal<fFRmax && dPhiIn25Degree ){hname="ECal_DeltaEnergyFunc2_in25Deg_TAGinFR173_258_ExpEg2UsingE1"; hSvc->FillHisto(hname,DeltaEnGamma12,1.);}




    if(allPlot && dPhiIn25Degree) FillHistoTagAndProbe_PhiRegionsProbe(DeltaEnGamma12,phi1,"_ExpEg2UsingE1");
    if(allPlot && dPhiIn25Degree) FillHistoTagAndProbe_RadiusRegionsProbe(DeltaEnGamma12, g1Recal, g2Recal, "_ExpEg2UsingE1");
    if(allPlot && dPhiIn25Degree) FillHistoTagAndProbe_RadiusRegionsProbe(DeltaEnGamma12, g1Recal, "_ExpEg2UsingE1");
    
    if(allPlot && g1Recal>fFRmin && g1Recal<fFRmax && g2Recal>fFRmin && g2Recal<fFRmax && dPhiIn25Degree)FillHistoTagAndProbe_PhiRegionsProbe(DeltaEnGamma12,phi1,"_inFR115.8_258_ExpEg2UsingE1");
    if(allPlot && g1Recal>fFRmin && g1Recal<fFRmax && dPhiIn25Degree)FillHistoTagAndProbe_PhiRegionsProbe(DeltaEnGamma12,phi1,"_TAGinFR115.8_258_ExpEg2UsingE1");
      
    if(allPlot && g2Recal>115.8 && g2Recal<258 && dPhiIn25Degree) FillHistoTagAndProbe_RadiusRegionsProbe(DeltaEnGamma12, g1Recal, "_inFR115.8_258_ExpEg2UsingE1");
    if(allPlot && g2Recal>115.8 && g2Recal<258 && dPhiIn25Degree) FillHistoTagAndProbe_RadiusRegionsProbe(DeltaEnGamma12, g1Recal, g2Recal, "_inFR115.8_258_ExpEg2UsingE1");

    if(allPlot && dPhiIn25Degree)FillHistoTagAndProbe_PhiProbeForTagPhiRegions(phi1 , phi2, "" );
    if(allPlot) FillHistoTagAndProbe_DeltaPhiForTagPhiRegions(phi1 , deltaphi, "" );
    if(allPlot && dPhiIn25Degree)FillHistoTagAndProbe_ProbeRadiusForTagRadiusRegions(g1Recal, g2Recal, "");
      
    if(allPlot && dPhiIn25Degree)FillHistoTagAndProbe_PhiRadiusProbeDistribution(phi1 ,g1Recal,phi2 ,g2Recal, "" );

    if(dPhiIn25Degree)FillHistoTagAndProbe_PhiRegionsCalchep430RadiusRegionsProbe(DeltaEnGamma12, phi1 , g1Recal, "_Rmid173_ExpEg2UsingE1" );
    if(allPlot && g2Recal>fFRmin && g2Recal<fFRmax && dPhiIn25Degree)FillHistoTagAndProbe_PhiRegionsCalchep430RadiusRegionsProbe(DeltaEnGamma12, phi1 , g1Recal, "_Rmid173_inFR115.8_258_ExpEg2UsingE1" );

    if(allPlot && fabs(XWeighted)<50. && fabs(YWeighted)<50.&& g1E>90 && g2E>90 && g1E<400 && g2E<400 && fabs(g1t-g2t)<10.){
      hname="ECAL_10ns5CoGEThr_TagAndProbeMethod";
      hSvc->FillHisto(hname,g1E+g2E, 1. );
      if((g1E+g2E)> 380 && (g1E+g2E)< 500 && g1Recal>85. && g2Recal>85. ){
	hname="ECAL_R1vsR2_10ns5CoGEThrSumEBeamRcut_TagAndProbeMethod";
	hSvc->FillHisto2(hname,g1Recal,g2Recal, 1. );
	hname="ECAL_Phi1vsPhi2_10ns5CoGEThrSumEBeamRcut_TagAndProbeMethod";
	hSvc->FillHisto2(hname,phi1,phi2, 1. );
      }
    }
    if(allPlot && dPhiIn25Degree){
      hname="ECal_ProbeR2expected_ethr90MeV";
      hSvc->FillHisto(hname,R2Func, 1. );
    }
    if(allPlot){
      hname="ECAL_probeCountIn25fr";
      hSvc->FillHisto(hname, probeCountIn25fr, 1.);
    }
    //} // end if loop on |deltaenergy|<100
}//first loop ecal

return true;
}




Bool_t TagAndProbeSelection::phiSymmetricalInECal(TVector3 P1, TVector3 P2,  double& distPhi)
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

Bool_t TagAndProbeSelection::passPreselection(Bool_t isTargetOut, Bool_t isMC)
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  Bool_t passed = false;

  if (!fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) 
    {
      if (!fRecoEvent->GetTriggerMaskBit(TRECOEVENT_TRIGMASKBIT_BEAM)) return passed;
    }
 
  Double_t targetConst=1.78; //to use olny if the run is taken from September 2020 !!!!->corrected in develop 22/02/2021
  //if(isMC) targetConst=1; 
  targetConst=1; 

  if (!fexternalPass && !isMC && !isTargetOut && fTarget_RecoBeam->getnPOT()<13000) return passed;  
  if (isTargetOut && fSAC_ClColl->GetNElements()>15) return passed;
  //if(fRecoEvent->GetEventNumber()>270000) return passed;

  std::string hname1 = "TagAndProbe_NposInBunch_beam";
  hSvc->FillHisto(hname1,fTarget_RecoBeam->getnPOT()*targetConst);
  hname = "TagAndProbe_nPOT";
  hSvc->FillHisto(hname,0,float(fTarget_RecoBeam->getnPOT()*targetConst)); 
  passed = true;
  return passed;
}



Bool_t TagAndProbeSelection::InitHistos(Bool_t allPlot)
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  int binX=5000;
  double minX=0;
  double maxX=3000;
  int binY=5000;
  double minY=0;
  double maxY=3000;
  hname="TagAndProbe_ECalClusterEnergy";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="TagAndProbe_NposInBunch_beam";
  hSvc->BookHisto(hname, 500, 0., 40000.);
  hname = "TagAndProbe_nPOT";
  hSvc->BookHisto(hname, 3, -1.5, 1.5);
  if(allPlot){
    hname="ECAL_probeCountIn25fr";
    hSvc->BookHisto(hname, 11, -0.5, 10.5);
    binY=5000;
    minY=0;
    maxY=3000;
    hname="ECAL_10ns5CoGEThr_TagAndProbeMethod";
    hSvc->BookHisto(hname, 800, 0., 800.);
    binY=300;
    minY=0.;
    maxY=300;
    hname="ECAL_R1vsR2_25Deg_TagAndProbeMethod";
    hSvc->BookHisto2(hname, binY, minY, maxY,  binY, minY, maxY);
    hname="ECAL_R1vsR2_10ns5CoGEThrSumEBeamRcut_TagAndProbeMethod";
    hSvc->BookHisto2(hname, binY, minY, maxY,  binY, minY, maxY);
    binY=300;
    minY=0.;
    maxY=380;
    hname="ECAL_Phi1vsPhi2_25Deg_TagAndProbeMethod";
    hSvc->BookHisto2(hname, binY, minY, maxY,  binY, minY, maxY);
    hname="ECAL_Phi1vsPhi2_10ns5CoGEThrSumEBeamRcut_TagAndProbeMethod";
    hSvc->BookHisto2(hname, binY, minY, maxY,  binY, minY, maxY);
  
    binY=600;
    minY=-300;
    maxY=300;
    hname="ECAL_XCoGAllCl_InTagAndProbe";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECAL_YCoGAllCl_InTagAndProbe";
    hSvc->BookHisto(hname, binY, minY, maxY);
    
    binY=350;
    minY=0.;
    maxY=350;
    hname="ECal_TagR2expected_ethr90MeV";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeR2expected_ethr90MeV";
    hSvc->BookHisto(hname, binY, minY, maxY);
    
    binY=300;
    minY=-0.08;
    maxY=0.08;
    hname="ECal_DeltaThetaFuncAllCl";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_DeltaThetaFuncAllCl_ethr90MeV";
    hSvc->BookHisto(hname, binY, minY, maxY);
  
  
    binY=800;
    minY=-400.;
    maxY=400.;
    hname="ECal_DeltaEnergyFuncAllCl";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_DeltaEnergyFuncAllCl_ethr90MeV";
    hSvc->BookHisto(hname, binY, minY, maxY);
  }

  binY=800;
  minY=-400.;
  maxY=400.;
  hname="ECal_DeltaEnergyFuncAllCl_inFR115.8_258";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECal_DeltaEnergyFuncAllCl_inFR115.8_173";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECal_DeltaEnergyFuncAllCl_inFR173_258";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECal_DeltaEnergyFunc2_in25Deg_TAGinFR115.8_173_ExpEg2UsingE1";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECal_DeltaEnergyFunc2_in25Deg_TAGinFR173_258_ExpEg2UsingE1";
  hSvc->BookHisto(hname, binY, minY, maxY);

  if(allPlot){
    hname="ECal_DeltaEnergyFuncAllCl_ethr90MeV_inFR115.8_258";
    hSvc->BookHisto(hname, binY, minY, maxY);
  }
  

  if(allPlot){
    hname="ECAL_myChi2";
    hSvc->BookHisto(hname, 800, -0.5, 150);
    hname="ECAL_myChi2vsDEnExpProbe";
    hSvc->BookHisto2(hname, binY, minY, maxY, 800, -0.5, 150);
    hname="ECAL_myChi2vsDEnTag";
    hSvc->BookHisto2(hname, binY, minY, maxY, 800, -0.5, 150);
  }


  std::vector<std::string> sufix;
  if(allPlot){
    sufix.push_back("");
    sufix.push_back("_inFR115.8_258");
    sufix.push_back("_EThr90MeV");
    sufix.push_back("EThr90MeV_inFR115.8_258");
  }
  sufix.push_back("_tag115.8_173R_Rmid173");
  sufix.push_back("_tag173_258R_Rmid173");
  if(allPlot){
    sufix.push_back("_tag115.8_173R_EThr90MeV_Rmid173");
    sufix.push_back("_tag173_258R_EThr90MeV_Rmid173");
  }
  for(int i=0; i<sufix.size(); i++){
    binY=800;
    minY=-400.;
    maxY=400.;
    if(allPlot && i<2){
      hname="ECal_DeltaEnergyFunc2_in25Deg"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in25Deg_ethr90MeV"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in30Deg"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in30Deg_ethr90MeV"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in35Deg"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in35Deg_ethr90MeV"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in40Deg"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in40Deg_ethr90MeV"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in45Deg"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in45Deg_ethr90MeV"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in50Deg"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in50Deg_ethr90MeV"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in70Deg"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in70Deg_ethr90MeV"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in90Deg"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in90Deg_ethr90MeV"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);

      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom0To45"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom45To90"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom90To135"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom135To180"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom180To225"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom225To270"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom270To315"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom315To360"+sufix.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
    }
    hname="ECal_TagDeltaEnergyFunc_From0To45"+sufix.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagDeltaEnergyFunc_From45To90"+sufix.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagDeltaEnergyFunc_From90To135"+sufix.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagDeltaEnergyFunc_From135To180"+sufix.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagDeltaEnergyFunc_From180To225"+sufix.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagDeltaEnergyFunc_From225To270"+sufix.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagDeltaEnergyFunc_From270To315"+sufix.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagDeltaEnergyFunc_From315To360"+sufix.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);


  }


  std::vector<std::string> sufixM;
  sufixM.push_back("_tag115.8_173R_Rmid173");
  sufixM.push_back("_tag173_258R_Rmid173");
  for(int i=0; i<sufixM.size(); i++){
    binY=600;
    minY=-200.;
    maxY=400.;
    hname="ECal_TagProbeMeth_MM2_From0To45"+sufixM.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagProbeMeth_MM2_From45To90"+sufixM.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagProbeMeth_MM2_From90To135"+sufixM.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagProbeMeth_MM2_From135To180"+sufixM.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagProbeMeth_MM2_From180To225"+sufixM.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagProbeMeth_MM2_From225To270"+sufixM.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagProbeMeth_MM2_From270To315"+sufixM.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagProbeMeth_MM2_From315To360"+sufixM.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
  }

  if(allPlot){
    binY=800;
    minY=-400.;
    maxY=400.;
    hname="ECal_TagDeltaEnergyFunc_RFrom106To125";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagDeltaEnergyFunc_RFrom115To125";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagDeltaEnergyFunc_RFrom106To125EThr90MeV";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagDeltaEnergyFunc_RFrom115To125EThr90MeV";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom106To125";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom115To125";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeDeltaEnergyFunc2_RFrom217To258_TagFrom106To125";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagDeltaEnergyFunc_RFrom125To150";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagDeltaEnergyFunc_RFrom125To150EThr90MeV";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom125To150";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeDeltaEnergyFunc2_RFrom183To217_TagFrom125To150";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagDeltaEnergyFunc_RFrom150To175";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagDeltaEnergyFunc_RFrom150To175EThr90MeV";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom150To175";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeDeltaEnergyFunc2_RFrom155To183_TagFrom150To175";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagDeltaEnergyFunc_RFrom175To200";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagDeltaEnergyFunc_RFrom175To200EThr90MeV";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom175To200";
    hSvc->BookHisto(hname, binY, minY, maxY); 
    hname="ECal_ProbeDeltaEnergyFunc2_RFrom136To155_TagFrom175To200";
    hSvc->BookHisto(hname, binY, minY, maxY); 
    hname="ECal_TagDeltaEnergyFunc_RFrom200To225";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagDeltaEnergyFunc_RFrom200To225EThr90MeV";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom200To225";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeDeltaEnergyFunc2_RFrom123To136_TagFrom200To225";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagDeltaEnergyFunc_RFrom225To258";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_TagDeltaEnergyFunc_RFrom225To258EThr90MeV";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom225To258";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeDeltaEnergyFunc2_RFrom106To123_TagFrom225To258";
    hSvc->BookHisto(hname, binY, minY, maxY);
  }

  std::vector<std::string> sufix1 ;

  if(allPlot){
    sufix1.push_back("_ExpEg2UsingE1");
    sufix1.push_back("_inFR115.8_258_ExpEg2UsingE1");    
  }
  sufix1.push_back("_TAGinFR115.8_258_ExpEg2UsingE1");
  sufix1.push_back("_tag115.8_173R_Rmid173_ExpEg2UsingE1");
  if(allPlot) sufix1.push_back("_tag115.8_173R_Rmid173_inFR115.8_258_ExpEg2UsingE1");
  sufix1.push_back("_tag173_258R_Rmid173_ExpEg2UsingE1");
  if(allPlot) sufix1.push_back("_tag173_258R_Rmid173_inFR115.8_258_ExpEg2UsingE1");

  for(int i=0; i<sufix1.size(); i++){
    binY=800;
    minY=-400.;
    maxY=400.;
    if(i==0){
      hname="ECal_DeltaEnergyFunc2_in25Deg"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in30Deg"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in35Deg"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in40Deg"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in45Deg"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in50Deg"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in70Deg"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in90Deg"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
    }
    if((allPlot && i<3) && i>0){
      hname="ECal_DeltaEnergyFunc2_in25Deg"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in30Deg"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in35Deg"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in40Deg"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in45Deg"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in50Deg"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in70Deg"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_DeltaEnergyFunc2_in90Deg"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
    }
    //if(i==2) continue;
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom0To45"+sufix1.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom45To90"+sufix1.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom90To135"+sufix1.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom135To180"+sufix1.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom180To225"+sufix1.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom225To270"+sufix1.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom270To315"+sufix1.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom315To360"+sufix1.at(i);
    hSvc->BookHisto(hname, binY, minY, maxY);
    if(allPlot && i!=2 &&/* i!=4 &&*/ i<3){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom106To125"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom115To125"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_ProbeDeltaEnergyFunc2_RFrom217To258_TagFrom106To125"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom125To150"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_ProbeDeltaEnergyFunc2_RFrom183To217_TagFrom125To150"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom150To175"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_ProbeDeltaEnergyFunc2_RFrom155To183_TagFrom150To175"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom175To200"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY); 
      hname="ECal_ProbeDeltaEnergyFunc2_RFrom136To155_TagFrom175To200"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY); 
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom200To225"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_ProbeDeltaEnergyFunc2_RFrom123To136_TagFrom200To225"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom225To258"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
      hname="ECal_ProbeDeltaEnergyFunc2_RFrom106To123_TagFrom225To258"+sufix1.at(i);
      hSvc->BookHisto(hname, binY, minY, maxY);
    }
  }

  if(allPlot){
    binY=800;
    minY=-10.;
    maxY=370.;
    hname="ECal_PhiProbe_TagFrom0To45";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_PhiProbe_TagFrom45To90";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_PhiProbe_TagFrom90To135";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_PhiProbe_TagFrom135To180";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_PhiProbe_TagFrom180To225";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_PhiProbe_TagFrom225To270";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_PhiProbe_TagFrom270To315";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_PhiProbe_TagFrom315To360";
    hSvc->BookHisto(hname, binY, minY, maxY);

    binY=400;
    minY=-4.;
    maxY=4.;
    hname="ECal_DeltaPhi_TagFrom0To45";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_DeltaPhi_TagFrom45To90";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_DeltaPhi_TagFrom90To135";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_DeltaPhi_TagFrom135To180";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_DeltaPhi_TagFrom180To225";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_DeltaPhi_TagFrom225To270";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_DeltaPhi_TagFrom270To315";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_DeltaPhi_TagFrom315To360";
    hSvc->BookHisto(hname, binY, minY, maxY);

    binY=700;
    minY=-10.;
    maxY=320.;
    hname="ECal_ProbeRadius_TagFrom106To125";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeRadius_TagFrom115To125";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeRadius_TagFrom125To150";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeRadius_TagFrom150To175";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeRadius_TagFrom175To200";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeRadius_TagFrom200To225";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeRadius_TagFrom225To258";
    hSvc->BookHisto(hname, binY, minY, maxY);

    hname="ECal_ProbeR_TagFrom0To45_tagInnerR";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeR_TagFrom0To45_tagOuterR";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeR_TagFrom45To90_tagInnerR";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeR_TagFrom45To90_tagOuterR";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeR_TagFrom90To135_tagInnerR";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeR_TagFrom90To135_tagOuterR";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeR_TagFrom135To180_tagInnerR";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeR_TagFrom135To180_tagOuterR";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeR_TagFrom180To225_tagInnerR";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeR_TagFrom180To225_tagOuterR";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeR_TagFrom225To270_tagInnerR";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeR_TagFrom225To270_tagOuterR";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeR_TagFrom270To315_tagInnerR";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeR_TagFrom270To315_tagOuterR";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeR_TagFrom315To360_tagInnerR";
    hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbeR_TagFrom315To360_tagOuterR";
    hSvc->BookHisto(hname, binY, minY, maxY);
  
    binY=800;
    minY=-10.;
    maxY=370.;
    hname="ECal_ProbePhi_TagFrom0To45_tagInnerR";   hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbePhi_TagFrom0To45_tagOuterR";   hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbePhi_TagFrom45To90_tagInnerR";  hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbePhi_TagFrom45To90_tagOuterR";  hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbePhi_TagFrom90To135_tagInnerR"; hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbePhi_TagFrom90To135_tagOuterR"; hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbePhi_TagFrom135To180_tagInnerR";hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbePhi_TagFrom135To180_tagOuterR";hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbePhi_TagFrom180To225_tagInnerR";hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbePhi_TagFrom180To225_tagOuterR";hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbePhi_TagFrom225To270_tagInnerR";hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbePhi_TagFrom225To270_tagOuterR";hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbePhi_TagFrom270To315_tagInnerR";hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbePhi_TagFrom270To315_tagOuterR";hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbePhi_TagFrom315To360_tagInnerR";hSvc->BookHisto(hname, binY, minY, maxY);
    hname="ECal_ProbePhi_TagFrom315To360_tagOuterR";hSvc->BookHisto(hname, binY, minY, maxY);
  }

  return true;
}




void TagAndProbeSelection::FillHistoTagAndProbe(Double_t DeltaEnergyFunction2, Double_t g2E, Bool_t dPhiIn25Degree,  Bool_t dPhiIn30Degree,  Bool_t dPhiIn35Degree,  Bool_t dPhiIn40Degree,  Bool_t dPhiIn45Degree,  Bool_t dPhiIn50Degree,  Bool_t dPhiIn70Degree,  Bool_t dPhiIn90Degree, std::string sufix ){

  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

  if(dPhiIn25Degree){
    hname="ECal_DeltaEnergyFunc2_in25Deg"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }
  if(dPhiIn30Degree){
    hname="ECal_DeltaEnergyFunc2_in30Deg"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }
  if(dPhiIn35Degree){
    hname="ECal_DeltaEnergyFunc2_in35Deg"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }
  if(dPhiIn40Degree){
    hname="ECal_DeltaEnergyFunc2_in40Deg"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }
  if(dPhiIn45Degree){
    hname="ECal_DeltaEnergyFunc2_in45Deg"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }
  if(dPhiIn50Degree){
    hname="ECal_DeltaEnergyFunc2_in50Deg"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }

  if(dPhiIn70Degree){
    hname="ECal_DeltaEnergyFunc2_in70Deg"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }

  if(dPhiIn90Degree){
    hname="ECal_DeltaEnergyFunc2_in90Deg"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }

}




void TagAndProbeSelection::FillHistoTagAndProbe_PhiRegionsTag(Double_t DeltaEnergyFunction, Double_t phi , std::string sufix ){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  if(phi>=0. && phi<45.){
    hname="ECal_TagDeltaEnergyFunc_From0To45"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }
  else if(phi>=45. && phi<90.){
    hname="ECal_TagDeltaEnergyFunc_From45To90"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }
  else if(phi>=90. && phi<135.){
    hname="ECal_TagDeltaEnergyFunc_From90To135"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }
  else if(phi>=135. && phi<180.){
    hname="ECal_TagDeltaEnergyFunc_From135To180"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }
  else if(phi>=180. && phi<225.){
    hname="ECal_TagDeltaEnergyFunc_From180To225"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }
  else if(phi>=225. && phi<270.){
    hname="ECal_TagDeltaEnergyFunc_From225To270"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }
  else if(phi>=270. && phi<315){
    hname="ECal_TagDeltaEnergyFunc_From270To315"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }
  else if(phi>=315. && phi<360.){
    hname="ECal_TagDeltaEnergyFunc_From315To360"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }

}


void TagAndProbeSelection::FillHistoTagAndProbe_PhiRegionsRadiusRegionsTag(Double_t DeltaEnergyFunction, Double_t phi ,Double_t radius, std::string sufix ){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  if(phi>=0. && phi<45.){
    if(radius>106 && radius<182){
      hname="ECal_TagDeltaEnergyFunc_From0To45_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From0To45_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=45. && phi<90.){
    if(radius>106 && radius<182){
      hname="ECal_TagDeltaEnergyFunc_From45To90_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From45To90_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=90. && phi<135.){
    if(radius>106 && radius<182){
      hname="ECal_TagDeltaEnergyFunc_From90To135_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From90To135_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=135. && phi<180.){
    if(radius>106 && radius<182){
      hname="ECal_TagDeltaEnergyFunc_From135To180_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From135To180_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=180. && phi<225.){
    if(radius>106 && radius<182){
      hname="ECal_TagDeltaEnergyFunc_From180To225_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From180To225_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=225. && phi<270.){
    if(radius>106 && radius<182){
      hname="ECal_TagDeltaEnergyFunc_From225To270_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From225To270_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=270. && phi<315){
    if(radius>106 && radius<182){
      hname="ECal_TagDeltaEnergyFunc_From270To315_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From270To315_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=315. && phi<360.){
    if(radius>106 && radius<182){
      hname="ECal_TagDeltaEnergyFunc_From315To360_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From315To360_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }

}


void TagAndProbeSelection::FillHistoTagAndProbe_PhiRegionsNewRadiusRegionsTag(Double_t DeltaEnergyFunction, Double_t phi ,Double_t radius, std::string sufix ){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  if(phi>=0. && phi<45.){
    if(radius>106 && radius<170){
      hname="ECal_TagDeltaEnergyFunc_From0To45_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=170 && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From0To45_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=45. && phi<90.){
    if(radius>106 && radius<170){
      hname="ECal_TagDeltaEnergyFunc_From45To90_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=170 && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From45To90_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=90. && phi<135.){
    if(radius>106 && radius<170){
      hname="ECal_TagDeltaEnergyFunc_From90To135_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=170 && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From90To135_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=135. && phi<180.){
    if(radius>106 && radius<170){
      hname="ECal_TagDeltaEnergyFunc_From135To180_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=170 && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From135To180_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=180. && phi<225.){
    if(radius>106 && radius<170){
      hname="ECal_TagDeltaEnergyFunc_From180To225_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=170 && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From180To225_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=225. && phi<270.){
    if(radius>106 && radius<170){
      hname="ECal_TagDeltaEnergyFunc_From225To270_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=170 && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From225To270_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=270. && phi<315){
    if(radius>106 && radius<170){
      hname="ECal_TagDeltaEnergyFunc_From270To315_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=170 && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From270To315_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=315. && phi<360.){
    if(radius>106 && radius<170){
      hname="ECal_TagDeltaEnergyFunc_From315To360_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=170 && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From315To360_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }

}


void TagAndProbeSelection::FillHistoTagAndProbe_PhiRegionsCalchep430RadiusRegionsTag(Double_t DeltaEnergyFunction, Double_t phi ,Double_t radius, std::string sufix ){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  if(phi>=0. && phi<45.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_TagDeltaEnergyFunc_From0To45_tag115.8_173R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From0To45_tag173_258R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=45. && phi<90.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_TagDeltaEnergyFunc_From45To90_tag115.8_173R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From45To90_tag173_258R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=90. && phi<135.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_TagDeltaEnergyFunc_From90To135_tag115.8_173R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From90To135_tag173_258R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=135. && phi<180.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_TagDeltaEnergyFunc_From135To180_tag115.8_173R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From135To180_tag173_258R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=180. && phi<225.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_TagDeltaEnergyFunc_From180To225_tag115.8_173R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From180To225_tag173_258R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=225. && phi<270.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_TagDeltaEnergyFunc_From225To270_tag115.8_173R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From225To270_tag173_258R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=270. && phi<315){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_TagDeltaEnergyFunc_From270To315_tag115.8_173R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From270To315_tag173_258R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=315. && phi<360.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_TagDeltaEnergyFunc_From315To360_tag115.8_173R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_TagDeltaEnergyFunc_From315To360_tag173_258R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }

}




void TagAndProbeSelection::FillHistoTagAndProbe_PhiRegionsRadiusRegionsProbe(Double_t DeltaEnergyFunction, Double_t phi ,Double_t radius, std::string sufix ){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  if(phi>=0. && phi<45.){
    if(radius>106 && radius<182){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom0To45_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom0To45_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=45. && phi<90.){
    if(radius>106 && radius<182){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom45To90_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom45To90_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=90. && phi<135.){
    if(radius>106 && radius<182){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom90To135_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom90To135_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=135. && phi<180.){
    if(radius>106 && radius<182){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom135To180_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom135To180_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=180. && phi<225.){
    if(radius>106 && radius<182){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom180To225_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom180To225_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=225. && phi<270.){
    if(radius>106 && radius<182){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom225To270_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom225To270_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=270. && phi<315){
    if(radius>106 && radius<182){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom270To315_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom270To315_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=315. && phi<360.){
    if(radius>106 && radius<182){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom315To360_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom315To360_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }

}


void TagAndProbeSelection::FillHistoTagAndProbe_PhiRegionsCalchep430RadiusRegionsProbe(Double_t DeltaEnergyFunction, Double_t phi ,Double_t radius, std::string sufix ){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  if(phi>=0. && phi<45.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom0To45_tag115.8_173R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom0To45_tag173_258R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=45. && phi<90.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom45To90_tag115.8_173R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom45To90_tag173_258R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=90. && phi<135.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom90To135_tag115.8_173R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom90To135_tag173_258R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=135. && phi<180.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom135To180_tag115.8_173R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom135To180_tag173_258R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=180. && phi<225.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom180To225_tag115.8_173R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom180To225_tag173_258R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=225. && phi<270.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom225To270_tag115.8_173R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom225To270_tag173_258R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=270. && phi<315){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom270To315_tag115.8_173R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom270To315_tag173_258R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=315. && phi<360.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom315To360_tag115.8_173R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom315To360_tag173_258R"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }

}


void TagAndProbeSelection::FillHistoTagAndProbe_PhiRegionsNewRadiusRegionsProbe(Double_t DeltaEnergyFunction, Double_t phi ,Double_t radius, std::string sufix ){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  if(phi>=0. && phi<45.){
    if(radius>106 && radius<170){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom0To45_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=170 && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom0To45_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=45. && phi<90.){
    if(radius>106 && radius<170){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom45To90_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=170 && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom45To90_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=90. && phi<135.){
    if(radius>106 && radius<170){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom90To135_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=170 && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom90To135_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=135. && phi<180.){
    if(radius>106 && radius<170){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom135To180_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=170 && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom135To180_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=180. && phi<225.){
    if(radius>106 && radius<170){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom180To225_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=170 && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom180To225_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=225. && phi<270.){
    if(radius>106 && radius<170){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom225To270_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=170 && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom225To270_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=270. && phi<315){
    if(radius>106 && radius<170){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom270To315_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=170 && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom270To315_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }
  else if(phi>=315. && phi<360.){
    if(radius>106 && radius<170){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom315To360_tagInnerR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
    else if(radius>=170 && radius<fFRmax){
      hname="ECal_ProbeDeltaEnergyFunc2_TagFrom315To360_tagOuterR"+sufix;
      hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
    }
  }

}



void TagAndProbeSelection::FillHistoTagAndProbe_PhiProbeForTagPhiRegions( Double_t phi ,Double_t phi2, std::string sufix ){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  if(phi>=0. && phi<45.){
    hname="ECal_PhiProbe_TagFrom0To45"+sufix;
    hSvc->FillHisto(hname,phi2, 1. );
  }
  else if(phi>=45. && phi<90.){
    hname="ECal_PhiProbe_TagFrom45To90"+sufix;
    hSvc->FillHisto(hname,phi2, 1. );
  }
  else if(phi>=90. && phi<135. ){
    hname="ECal_PhiProbe_TagFrom90To135"+sufix;
    hSvc->FillHisto(hname,phi2, 1. );
  }
  else if(phi>=135. && phi<180.){
    hname="ECal_PhiProbe_TagFrom135To180"+sufix;
    hSvc->FillHisto(hname,phi2, 1. );
  }
  else if(phi>=180. && phi<225.){
    hname="ECal_PhiProbe_TagFrom180To225"+sufix;
    hSvc->FillHisto(hname,phi2, 1. );
  }
  else if(phi>=225. && phi<270.){
    hname="ECal_PhiProbe_TagFrom225To270"+sufix;
    hSvc->FillHisto(hname,phi2, 1. );
  }
  else if(phi>=270. && phi<315){
    hname="ECal_PhiProbe_TagFrom270To315"+sufix;
    hSvc->FillHisto(hname,phi2, 1. );
  }
  else if(phi>=315. && phi<360){
    hname="ECal_PhiProbe_TagFrom315To360"+sufix;
    hSvc->FillHisto(hname,phi2, 1. );
  }

}


void TagAndProbeSelection::FillHistoTagAndProbe_DeltaPhiForTagPhiRegions(Double_t phi ,Double_t deltaphi, std::string sufix  ){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  deltaphi=deltaphi-3.14;
  if(phi>=0. && phi<45.){
    hname="ECal_DeltaPhi_TagFrom0To45"+sufix;
    hSvc->FillHisto(hname,deltaphi, 1. );
  }
  else if(phi>=45. && phi<90.){
    hname="ECal_DeltaPhi_TagFrom45To90"+sufix;
    hSvc->FillHisto(hname,deltaphi, 1. );
  }
  else if(phi>=90. && phi<135. ){
    hname="ECal_DeltaPhi_TagFrom90To135"+sufix;
    hSvc->FillHisto(hname,deltaphi, 1. );
  }
  else if(phi>=135. && phi<180.){
    hname="ECal_DeltaPhi_TagFrom135To180"+sufix;
    hSvc->FillHisto(hname,deltaphi, 1. );
  }
  else if(phi>=180. && phi<225.){
    hname="ECal_DeltaPhi_TagFrom180To225"+sufix;
    hSvc->FillHisto(hname,deltaphi, 1. );
  }
  else if(phi>=225. && phi<270.){
    hname="ECal_DeltaPhi_TagFrom225To270"+sufix;
    hSvc->FillHisto(hname,deltaphi, 1. );
  }
  else if(phi>=270. && phi<315){
    hname="ECal_DeltaPhi_TagFrom270To315"+sufix;
    hSvc->FillHisto(hname,deltaphi, 1. );
  }
  else if(phi>=315. && phi<360){
    hname="ECal_DeltaPhi_TagFrom315To360"+sufix;
    hSvc->FillHisto(hname,deltaphi, 1. );
  }

}



void TagAndProbeSelection::FillHistoTagAndProbe_PhiRegionsProbe(Double_t DeltaEnergyFunction, Double_t phi , std::string sufix ){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  if(phi>=0. && phi<45.){
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom0To45"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }
  else if(phi>=45. && phi<90.){
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom45To90"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }
  else if(phi>=90. && phi<135.){
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom90To135"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }
  else if(phi>=135. && phi<180.){
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom135To180"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }
  else if(phi>=180. && phi<225.){
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom180To225"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }
  else if(phi>=225. && phi<270.){
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom225To270"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }
  else if(phi>=270. && phi<315){
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom270To315"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }
  else if(phi>=315. && phi<360.){
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom315To360"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }

}


void TagAndProbeSelection::FillHistoTagAndProbe_RadiusRegionsProbe(Double_t DeltaEnergyFunction2, Double_t  radius, Double_t radius2, std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

  if(radius>106 && radius<=125 && radius2<fFRmax && radius2>=217){
    hname="ECal_ProbeDeltaEnergyFunc2_RFrom217To258_TagFrom106To125"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }
  if(radius>125 && radius<=150  && radius2<217 && radius2>=183){
    hname="ECal_ProbeDeltaEnergyFunc2_RFrom183To217_TagFrom125To150"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }
  if(radius>150 && radius<=175  && radius2<183 && radius2>=155){
    hname="ECal_ProbeDeltaEnergyFunc2_RFrom155To183_TagFrom150To175"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }
  if(radius>175 && radius<=200  && radius2<155 && radius2>=136){
    hname="ECal_ProbeDeltaEnergyFunc2_RFrom136To155_TagFrom175To200"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }
  if(radius>200 && radius<=225  && radius2<136 && radius2>=123){
    hname="ECal_ProbeDeltaEnergyFunc2_RFrom123To136_TagFrom200To225"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }
  if(radius>225 && radius<=fFRmax  && radius2<123 && radius2>=106){
    hname="ECal_ProbeDeltaEnergyFunc2_RFrom106To123_TagFrom225To258"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }

}


void TagAndProbeSelection::FillHistoTagAndProbe_ProbeRadiusForTagRadiusRegions(Double_t  radius, Double_t radius2, std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

  if(radius>106 && radius<=125){
    hname="ECal_ProbeRadius_TagFrom106To125"+sufix;
    hSvc->FillHisto(hname,radius2, 1. );
  }
  if(radius>115 && radius<=125){
    hname="ECal_ProbeRadius_TagFrom115To125"+sufix;
    hSvc->FillHisto(hname,radius2, 1. );
  }
  if(radius>125 && radius<=150 ){
    hname="ECal_ProbeRadius_TagFrom125To150"+sufix;
    hSvc->FillHisto(hname,radius2, 1. );
  }
  if(radius>150 && radius<=175 ){
    hname="ECal_ProbeRadius_TagFrom150To175"+sufix;
    hSvc->FillHisto(hname,radius2, 1. );
  }
  if(radius>175 && radius<=200 ){
    hname="ECal_ProbeRadius_TagFrom175To200"+sufix;
    hSvc->FillHisto(hname,radius2, 1. );
  }
  if(radius>200 && radius<=225  ){
    hname="ECal_ProbeRadius_TagFrom200To225"+sufix;
    hSvc->FillHisto(hname,radius2, 1. );
  }
  if(radius>225 && radius<=fFRmax ){
    hname="ECal_ProbeRadius_TagFrom225To258"+sufix;
    hSvc->FillHisto(hname,radius2, 1. );
  }

}


void TagAndProbeSelection::FillHistoTagAndProbe_RadiusRegionsProbe(Double_t DeltaEnergyFunction2, Double_t  radius , std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

  if(radius>106 && radius<=125){
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom106To125"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }
  if(radius>115 && radius<=125){
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom115To125"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }
  if(radius>125 && radius<=150){
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom125To150"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }
  if(radius>150 && radius<=175){
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom150To175"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }
  if(radius>175 && radius<=200){
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom175To200"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }
  if(radius>200 && radius<=225){
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom200To225"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }
  if(radius>225 && radius<=fFRmax){
    hname="ECal_ProbeDeltaEnergyFunc2_TagFrom225To258"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction2, 1. );
  }

}


void TagAndProbeSelection::FillHistoTagAndProbe_RadiusRegionsTag(Double_t DeltaEnergyFunction,Double_t   radius, std::string sufix){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

  if(radius>106 && radius<=125){
    hname="ECal_TagDeltaEnergyFunc_RFrom106To125"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }
  if(radius>115 && radius<=125){
    hname="ECal_TagDeltaEnergyFunc_RFrom115To125"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }
  if(radius>125 && radius<=150){
    hname="ECal_TagDeltaEnergyFunc_RFrom125To150"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }
  if(radius>150 && radius<=175){
    hname="ECal_TagDeltaEnergyFunc_RFrom150To175"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }
  if(radius>175 && radius<=200){
    hname="ECal_TagDeltaEnergyFunc_RFrom175To200"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }
  if(radius>200 && radius<=225){
    hname="ECal_TagDeltaEnergyFunc_RFrom200To225"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }
  if(radius>225 && radius<=fFRmax){
    hname="ECal_TagDeltaEnergyFunc_RFrom225To258"+sufix;
    hSvc->FillHisto(hname,DeltaEnergyFunction, 1. );
  }

}



void TagAndProbeSelection::FillHistoTagAndProbe_PhiRadiusProbeDistribution(Double_t phi ,Double_t radius,Double_t phi2 ,Double_t radius2, std::string sufix ){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

  if(phi>=0. && phi<45.){
    if(radius>106 && radius<182){
      hname="ECal_ProbePhi_TagFrom0To45_tagInnerR"+sufix;
      hSvc->FillHisto(hname,phi2, 1. );
      hname="ECal_ProbeR_TagFrom0To45_tagInnerR"+sufix;
      hSvc->FillHisto(hname,radius2, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_ProbePhi_TagFrom0To45_tagOuterR"+sufix;
      hSvc->FillHisto(hname,phi2, 1. );
      hname="ECal_ProbeR_TagFrom0To45_tagOuterR"+sufix;
      hSvc->FillHisto(hname,radius2, 1. );
    }
  }

  else if(phi>=45. && phi<90.){
    if(radius>106 && radius<182){
      hname="ECal_ProbePhi_TagFrom45To90_tagInnerR"+sufix;
      hSvc->FillHisto(hname,phi2, 1. );
      hname="ECal_ProbeR_TagFrom45To90_tagInnerR"+sufix;
      hSvc->FillHisto(hname,radius2, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_ProbePhi_TagFrom45To90_tagOuterR"+sufix;
      hSvc->FillHisto(hname,phi2, 1. );
      hname="ECal_ProbeR_TagFrom45To90_tagOuterR"+sufix;
      hSvc->FillHisto(hname,radius2, 1. );
    }
  }

  else if(phi>=90. && phi<135.){
    if(radius>106 && radius<182){
      hname="ECal_ProbePhi_TagFrom90To135_tagInnerR"+sufix;
      hSvc->FillHisto(hname,phi2, 1. );
      hname="ECal_ProbeR_TagFrom90To135_tagInnerR"+sufix;
      hSvc->FillHisto(hname,radius2, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_ProbePhi_TagFrom90To135_tagOuterR"+sufix;
      hSvc->FillHisto(hname,phi2, 1. );
      hname="ECal_ProbeR_TagFrom90To135_tagOuterR"+sufix;
      hSvc->FillHisto(hname,radius2, 1. );
    }
  }

  else if(phi>=135. && phi<180.){
    if(radius>106 && radius<182){
      hname="ECal_ProbePhi_TagFrom135To180_tagInnerR"+sufix;
      hSvc->FillHisto(hname,phi2, 1. );
      hname="ECal_ProbeR_TagFrom135To180_tagInnerR"+sufix;
      hSvc->FillHisto(hname,radius2, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_ProbePhi_TagFrom135To180_tagOuterR"+sufix;
      hSvc->FillHisto(hname,phi2, 1. );
      hname="ECal_ProbeR_TagFrom135To180_tagOuterR"+sufix;
      hSvc->FillHisto(hname,radius2, 1. );
    }
  }

  else if(phi>=180. && phi<225.){
    if(radius>106 && radius<182){
      hname="ECal_ProbePhi_TagFrom180To225_tagInnerR"+sufix;
      hSvc->FillHisto(hname,phi2, 1. );
      hname="ECal_ProbeR_TagFrom180To225_tagInnerR"+sufix;
      hSvc->FillHisto(hname,radius2, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_ProbePhi_TagFrom180To225_tagOuterR"+sufix;
      hSvc->FillHisto(hname,phi2, 1. );
      hname="ECal_ProbeR_TagFrom180To225_tagOuterR"+sufix;
      hSvc->FillHisto(hname,radius2, 1. );
    }
  }

  else if(phi>=225. && phi<270.){
    if(radius>106 && radius<182){
      hname="ECal_ProbePhi_TagFrom225To270_tagInnerR"+sufix;
      hSvc->FillHisto(hname,phi2, 1. );
      hname="ECal_ProbeR_TagFrom225To270_tagInnerR"+sufix;
      hSvc->FillHisto(hname,radius2, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_ProbePhi_TagFrom225To270_tagOuterR"+sufix;
      hSvc->FillHisto(hname,phi2, 1. );
      hname="ECal_ProbeR_TagFrom225To270_tagOuterR"+sufix;
      hSvc->FillHisto(hname,radius2, 1. );
    }
  }

  else if(phi>=270. && phi<315){
    if(radius>106 && radius<182){
      hname="ECal_ProbePhi_TagFrom270To315_tagInnerR"+sufix;
      hSvc->FillHisto(hname,phi2, 1. );
      hname="ECal_ProbeR_TagFrom270To315_tagInnerR"+sufix;
      hSvc->FillHisto(hname,radius2, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_ProbePhi_TagFrom270To315_tagOuterR"+sufix;
      hSvc->FillHisto(hname,phi2, 1. );
      hname="ECal_ProbeR_TagFrom270To315_tagOuterR"+sufix;
      hSvc->FillHisto(hname,radius2, 1. );
    }
  }

  else if(phi>=315. && phi<360.){
    if(radius>106 && radius<182){
      hname="ECal_ProbePhi_TagFrom315To360_tagInnerR"+sufix;
      hSvc->FillHisto(hname,phi2, 1. );
      hname="ECal_ProbeR_TagFrom315To360_tagInnerR"+sufix;
      hSvc->FillHisto(hname,radius2, 1. );
    }
    else if(radius>=182 && radius<fFRmax){
      hname="ECal_ProbePhi_TagFrom315To360_tagOuterR"+sufix;
      hSvc->FillHisto(hname,phi2, 1. );
      hname="ECal_ProbeR_TagFrom315To360_tagOuterR"+sufix;
      hSvc->FillHisto(hname,radius2, 1. );
    }
  }

}


void TagAndProbeSelection::FillMM2AsAFunctionOfPhi(Double_t mm2, Double_t phi ,Double_t radius, std::string sufix ){
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

  if(phi>=0. && phi<45.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_TagProbeMeth_MM2_From0To45_tag115.8_173R_Rmid173"+sufix;
      hSvc->FillHisto(hname,mm2, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_TagProbeMeth_MM2_From0To45_tag173_258R_Rmid173"+sufix;
      hSvc->FillHisto(hname,mm2, 1. );
    }
  }
  else if(phi>=45. && phi<90.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_TagProbeMeth_MM2_From45To90_tag115.8_173R_Rmid173"+sufix;
      hSvc->FillHisto(hname,mm2, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_TagProbeMeth_MM2_From45To90_tag173_258R_Rmid173"+sufix;
      hSvc->FillHisto(hname,mm2, 1. );
    }
  }
  else if(phi>=90. && phi<135.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_TagProbeMeth_MM2_From90To135_tag115.8_173R_Rmid173"+sufix;
      hSvc->FillHisto(hname,mm2, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_TagProbeMeth_MM2_From90To135_tag173_258R_Rmid173"+sufix;
      hSvc->FillHisto(hname,mm2, 1. );
    }
  }
  else if(phi>=135. && phi<180.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_TagProbeMeth_MM2_From135To180_tag115.8_173R_Rmid173"+sufix;
      hSvc->FillHisto(hname,mm2, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_TagProbeMeth_MM2_From135To180_tag173_258R_Rmid173"+sufix;
      hSvc->FillHisto(hname,mm2, 1. );
    }
  }
  else if(phi>=180. && phi<225.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_TagProbeMeth_MM2_From180To225_tag115.8_173R_Rmid173"+sufix;
      hSvc->FillHisto(hname,mm2, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_TagProbeMeth_MM2_From180To225_tag173_258R_Rmid173"+sufix;
      hSvc->FillHisto(hname,mm2, 1. );
    }
  }
  else if(phi>=225. && phi<270.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_TagProbeMeth_MM2_From225To270_tag115.8_173R_Rmid173"+sufix;
      hSvc->FillHisto(hname,mm2, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_TagProbeMeth_MM2_From225To270_tag173_258R_Rmid173"+sufix;
      hSvc->FillHisto(hname,mm2, 1. );
    }
  }
  else if(phi>=270. && phi<315){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_TagProbeMeth_MM2_From270To315_tag115.8_173R_Rmid173"+sufix;
      hSvc->FillHisto(hname,mm2, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_TagProbeMeth_MM2_From270To315_tag173_258R_Rmid173"+sufix;
      hSvc->FillHisto(hname,mm2, 1. );
    }
  }
  else if(phi>=315. && phi<360.){
    if(radius>fFRmin && radius<fFRmid){
      hname="ECal_TagProbeMeth_MM2_From315To360_tag115.8_173R_Rmid173"+sufix;
      hSvc->FillHisto(hname,mm2, 1. );
    }
    else if(radius>=fFRmid && radius<fFRmax){
      hname="ECal_TagProbeMeth_MM2_From315To360_tag173_258R_Rmid173"+sufix;
      hSvc->FillHisto(hname,mm2, 1. );
    }
  }

}
