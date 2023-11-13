#include "TH2D.h"
#include "TTree.h"
#include "TProfile.h"
#include "TString.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TRandom3.h"
#include "TCanvas.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TMath.h"
#include "TVector2.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TLine.h"
#include <iostream>
#include <iomanip>

using namespace std;
const double me = 0.511; // MeV
const double hc = 197; // MeV fm
const double alpha = 0.007297;


double dsdoBha(double Estar, double Pstar, double cosThe);
double dsdoBha_scha(double Estar, double Pstar, double cosThe);
double getBYfield(double x0, double y0, double z0);
bool PointInTriangle(TVector2 pt, TVector2 v1, TVector2 v2, TVector2 v3);
float signV(TVector2 p1, TVector2 p2, TVector2 p3);
bool chosenChannels(int c, int d);
void toyba(bool fVerbose,int mode, double sqrts);

void runToy(int mode){
 const double sqmin = 16.4;
 const double sqwid = 0.1;
 const int isBins = 13;

  // const double sqmin = 17.0;
  // const double sqwid = 0.1;
  // const int isBins = 1;

  for (int is=0; is<isBins; is++){
    double sqrts = sqmin + is*sqwid;
    toyba(kFALSE,mode,sqrts);
  }
}

void readToy(int mode){

  if (mode != 0 && mode != 1) {
    std::cout <<"mode = 0 -> t+s channels; 1 = s channel only" << std::endl;
    return;
  }

  TString modeLabel[2] = {"t+s","sOnly"};

  const double sqmin = 16400;
  const double sqwid = 100.;
  const int isBins = 13;
  TGraph* ecalaccVsSqrts = new TGraph();
  ecalaccVsSqrts->SetName("ECalAcceptanceVsSqrts");
  for (int is=0; is<isBins; is++){
    double sqrts = sqmin + is*sqwid;
    TString filename = Form("batoy_%s_sqrts%d.root",modeLabel[mode].Data(),(int)(sqrts));
    TFile* filo = new TFile(filename.Data(),"OLD");
    TH2D* ECalChannelsFiredAll = (TH2D*) filo->Get("ECalChannelsFiredAll");
    TH2D* ECalChannelsFired = (TH2D*) filo->Get("ECalChannelsFired");
    ecalaccVsSqrts->SetPoint(is,sqrts/1000.,ECalChannelsFired->GetSum()/ECalChannelsFiredAll->GetSum());
    filo->Close();
    delete filo;
  }
  ecalaccVsSqrts->Draw("AP");
}

void toyba(bool fVerbose,int mode, double sqrts){
  if (mode != 0 && mode != 1) {
    std::cout <<"mode = 0 -> t+s channels; 1 = s channel only" << std::endl;
    return;
  }

  
  TString modeLabel[2] = {"t+s","sOnly"};
  TString filename = Form("batoy_%s_sqrts%d.root",modeLabel[mode].Data(),(int)(sqrts*1000));

  double Ebeam = (sqrts*sqrts-2*me*me)/(2.*me);// 431.6; // MeV
  double Pbeam = TMath::Sqrt(Ebeam*Ebeam-me*me);
  //  double sqrts = TMath::Sqrt(2*me*me + 2*Ebeam*me);
  double bg = Ebeam/sqrts;
  double gam = sqrt(bg*bg+1.);
  double beta = bg/gam;
  TVector3 beamBoost(0,0,beta);
  // kinematics in the center of mass frame

  double Estar = gam*Ebeam - bg*Pbeam;
  double Pstar = TMath::Sqrt(Estar*Estar - me*me);
  
  std::cout << " Ebeam,Pbeam = " << Ebeam<<" , "<<Pbeam<< " E*,P* = "<<Estar<<" , "<<Pstar << std::endl;

  const int nbins = 1000;
  TH1D* dcos = new TH1D("dsigmadcos","disgma dcos",nbins,-1,1);
  TH1D* dcos_scha = new TH1D("dsigmadcos_scha","disgma dcos s channel",nbins,-1,1);
  double cosw = 2./nbins;
  for (int i=0; i<nbins; i++){
    double cosThe = -1+(i+0.5)*cosw;
    dcos->SetBinContent(i+1,dsdoBha(Estar,Pstar,cosThe)*2*TMath::Pi()*10);//this way is in mb
    dcos_scha->SetBinContent(i+1,dsdoBha_scha(Estar,Pstar,cosThe)*2*TMath::Pi()*10);//this way is in mb
  }
  TCanvas* dsdo = new TCanvas("dsdo");
  dsdo->Divide(1,2);
  dsdo->cd(1);
  dcos->Draw("");
  dsdo->cd(2);
  dcos_scha->SetLineColor(2); dcos_scha->Draw("");
  TH1D* bfieldy = new TH1D("bfieldy","bfield y component",400,-2000.,2000.);
  for (int i=0; i<400; i++) bfieldy->SetBinContent(i+1,getBYfield(0,0,-2000.+10.*i));

  // set starting point

  const double x0 = 0; // mm
  const double y0 = 0; // mm
  const double z0 = -1028; // mm
  const double stepL = 0.2;//0.1; // mm
  double stepLM = stepL*1E-3; //m

  const double ecalZ = 2435.51; // calorimeter
  const double ecalLength = 230;//mm
  const double cellSize = 21;//mm 
  const int ncells = 29; // per row or column
  double ecalEdge = (ncells/2+0.5)*cellSize; // 304.5mm
  const int nhole = 5; // 5x5 matrix is not instrumented

  // set pveto geometry

  const double fFingerPitch = 11.;// *mm 
  const int nPVetoNFingers = 90;
  const int nEVetoNFingers = 96;
  int nVetoNFingers[2] = {nPVetoNFingers,nEVetoNFingers};
  const double fFingerSizeX =  10;// mm
  const double fFingerSizeY = 178.; // mm
  const double fFingerSizeZ =  10.; // mm;
  const double fFingerRotY = -10.*TMath::Pi()/180.;
  const double fPVetoInnerFacePosX =  177.5;// mm inner face X of structure enclosing the PVETO
  const double fPVetoFrontFacePosZ = -472.55;// *mm; // upstream face Z of structure enclosing the PVETO. Start 56.45mm from inner face of vacuum chamber (wrong but real position M. Raggi) 
  TVector3 PVetoSize(32.,188.,1095.); // size of the structure enclosing the PVETO
  TVector3 PVetoCenter(193.5,0.,74.95); // position of the center of the structure enclosing the PVETO
  
  const double fFingerXOffset = 15.; //mm Each finger is placed 15 mm in x inside the innerFace of the structure enclosing the PVETO
  const double fFingerZOffset = 26.; //mm Finger0 is placed 26 mm in Z inside the frontFace of the structure enclosing the PVETO
  // finger i has a center in (fPVetoInnerFacePosX+fFingerXOffset,0,fPVetoFrontFacePosZ+fFingerZOffset+i*fFingerPitch)
  // finger i is then rotated by fFingerRotY around the Y axis

  cout << "PVetoCenter = " << PVetoCenter.X() << " , " << PVetoCenter.Y() << " , " << PVetoCenter.Z() << endl;

  TVector2 pzxRot[4]; // fill points in the ZX plane: Z->x and X->y
  for (int j=0; j<4; j++) { // points in clock-wise order
    int xsign = (j==0 || j==3)? -1 : 1; // -1, 1, 1, -1
    int ysign = (j==0 || j==1)?  1 : -1; //  1, 1,-1, -1
    pzxRot[j].SetX(0.5*fFingerSizeZ*xsign);
    pzxRot[j].SetY(0.5*fFingerSizeX*ysign);
    cout << "j = " << j << " pzx " << pzxRot[j].X() << " , " << pzxRot[j].Y() << endl;

    pzxRot[j].SetX(TMath::Cos(-fFingerRotY)*0.5*fFingerSizeZ*xsign - TMath::Sin(-fFingerRotY)*0.5*fFingerSizeX*ysign);
    pzxRot[j].SetY(TMath::Sin(-fFingerRotY)*0.5*fFingerSizeZ*xsign + TMath::Cos(-fFingerRotY)*0.5*fFingerSizeX*ysign);
    cout << "After rotation j = " << j << " pzx " << pzxRot[j].X() << " , " << pzxRot[j].Y() << endl;
  }
  TVector2 pzx[4][nEVetoNFingers]; // fill points in the ZX plane: Z->x and X->y
  for (int i=0; i<nEVetoNFingers; i++){
    for (int j=0; j<4; j++) { // points in clock-wise order
      pzx[j][i].SetX(pzxRot[j].X() + fPVetoFrontFacePosZ + fFingerZOffset + i*fFingerPitch);
      pzx[j][i].SetY(pzxRot[j].Y() + fPVetoInnerFacePosX + fFingerXOffset);
    }
  }

  // plot finger vertices

  TGraph* PVetoFingerGraphs[nEVetoNFingers];
  for (int i=0; i<nEVetoNFingers; i++){
    PVetoFingerGraphs[i] = new TGraph(4);
    PVetoFingerGraphs[i]->SetName(Form("Finger%d",i));
    for (int j=0; j<4; j++) { // points in clock-wise order
      PVetoFingerGraphs[i]->SetPoint(j,pzx[j][i].X(),pzx[j][i].Y());
    }
  }
  TCanvas* fingerGraph = new TCanvas("fingerGraph");
  fingerGraph->SetWindowSize(600,600);
  fingerGraph->DrawFrame(
			 PVetoCenter.Z()-0.5*PVetoSize.Z(),
			 PVetoCenter.X()-0.5*PVetoSize.X(),
			 PVetoCenter.Z()+0.5*PVetoSize.Z(),
			 PVetoCenter.X()+0.5*PVetoSize.X());
  for (int i=0; i<nEVetoNFingers; i++){
    PVetoFingerGraphs[i]->Draw("Lsame");
  }
  

  const int nEv = 2000;
  TH2D* ECalChannelsFiredAll = new TH2D("ECalChannelsFiredAll","ECal channels fired",ncells*ncells+1,-1,ncells*ncells,ncells*ncells+1,-1,ncells*ncells);
  TH2D* ECalChannelsFired = new TH2D("ECalChannelsFired","ECal channels fired",ncells*ncells+1,-1,ncells*ncells,ncells*ncells+1,-1,ncells*ncells);
  TH2D* ECalFiredEn = new TH2D("ECalFiredEn","ECal channels fired E2 vs E1",100,0.,400,100,0,400);
  TH2D* ECalFiredDist = new TH2D("ECalFiredDist","dphi vs dr",100,0.,600.,100,-0.5*TMath::Pi(),0.5*TMath::Pi());
  // x and y slices                                                                                                                                                                                           
  Double_t fXMin = -21.*(14+0.5);
  Double_t fXMax = 21.*(14+0.5);
  Double_t fXW = 21; // mm                                                                                                                                                                                  
  Int_t fNXBins = (fXMax-fXMin)/fXW;
  Double_t fYMin = -21.*(14+0.5);
  Double_t fYMax = 21.*(14+0.5);
  Double_t fYW = 21; // mm                                                                                                                                                                                  
  Int_t fNYBins = (fYMax-fYMin)/fYW;

  TH2D* ECalYXFired = new TH2D("ECalYXFired","ECal channels fired y vs x ",fNXBins*10,fXMin,fXMax,fNYBins*10,fYMin,fYMax);
  TH2D* VetoInXZ[2];
  for (int i=0; i<2; i++) VetoInXZ[i] = new TH2D(Form("VetoInXZ_detector%d",i),"XZ when in Veto",
						 100,PVetoCenter.Z()-0.5*PVetoSize.Z(),PVetoCenter.Z()+0.5*PVetoSize.Z(),
						 100,PVetoCenter.X()-0.5*PVetoSize.X(),PVetoCenter.X()+0.5*PVetoSize.X()
						 );
  TH2D* Veto_ThetaVsP[nEVetoNFingers][2];
  TH2D* Veto_ThetaVsPhi[nEVetoNFingers][2];
  for (int i=0; i<nEVetoNFingers; i++){
    for (int ip=0; ip<2; ip++){
      Veto_ThetaVsP[i][ip] = new TH2D(Form("Veto_ThetaVsP_Channel_%d_particle%d",i,ip),Form("Veto Theta vs P for ch=%d det %d",i,ip),100,0.,Ebeam,101,0.5,200.5);
      Veto_ThetaVsPhi[i][ip] = new TH2D(Form("Veto_ThetaVsPhi_Channel_%d_particle%d",i,ip),Form("Veto Theta vs Phi for ch=%d det %d",i,ip),180,-1*TMath::Pi(),TMath::Pi(),101,0.5,200.5);
    }
  }

  TRandom3 rando;

//  const int nBSteps = 15;
//  const float dbw = 0.1;
//  const float bScaleMin = 0.3;

  const int nBSteps = 1;
  const float dbw = 0.1;
  const float bScaleMin = 1E-6;

  TH2D** VetoChannelsFiredAll = new TH2D*[nBSteps];
  TH2D** VetoChannelsFired = new TH2D*[nBSteps];
  TH2D** VetoChannelsFiredDT = new TH2D*[nBSteps];

  for (int ib = 0; ib<nBSteps; ib++){  

    double bScale = bScaleMin + dbw*ib;

    VetoChannelsFiredAll[ib] = new TH2D(Form("VetoChannelsFiredAll%d",ib),Form("Veto channels fired bscale = %f",bScale),nEVetoNFingers+1,-1,nEVetoNFingers,nEVetoNFingers+1,-1,nEVetoNFingers);
    VetoChannelsFired[ib] = new TH2D(Form("VetoChannelsFired%d",ib),Form("Veto channels fired bscale = %f",bScale),nEVetoNFingers+1,-1,nEVetoNFingers,nEVetoNFingers+1,-1,nEVetoNFingers);
    VetoChannelsFiredDT[ib] = new TH2D(Form("VetoChannelsFiredDT%d",ib),Form("Veto channels fired DT-weight bs = %f",bScale),nEVetoNFingers+1,-1,nEVetoNFingers,nEVetoNFingers+1,-1,nEVetoNFingers);


    int charge[2] = {1,-1};
    for (int iEv = 0; iEv < nEv; iEv++){
      if (iEv%1000 == 0) cout <<"Event " << iEv << endl;

    // simulation of bhabha kinematics

      double costhe = -1+2*rando.Rndm();//dcos->GetRandom(); // center of mass cos theta of e+
      double weight = 0;
      if (mode==1) {
	weight = dcos_scha->GetBinContent(dcos_scha->FindBin(costhe));
      }
      else {
	weight = dcos->GetBinContent(dcos->FindBin(costhe));
      }
      double sinthe = TMath::Sqrt(1.-costhe*costhe);
      double phi = rando.Rndm()*2*TMath::Pi();
      TLorentzVector eeMom[2];
      eeMom[0].SetXYZT(
		       Pstar*sinthe*TMath::Cos(phi),
		       Pstar*sinthe*TMath::Sin(phi),
		       Pstar*costhe,
		       Estar);
      eeMom[1].SetXYZT(
		       -Pstar*sinthe*TMath::Cos(phi),
		       -Pstar*sinthe*TMath::Sin(phi),
		       -Pstar*costhe,
		       Estar);

    //
      int firedChannels[2] = {-1,-1};
      int ecalfiredChannels[2] = {-1,-1};
      double ecalfiredEnergies[2] = {-1,-1};
      double lengths[2] = {0,0};

      for (int ip=0; ip<2; ip++){
	eeMom[ip].Boost(beamBoost); // boost in lab frame

      // swim the particle starting from the luminous point

	TVector3 LocalDirection(eeMom[ip].X(),eeMom[ip].Y(),eeMom[ip].Z());
	TVector3 LocalPosition(x0,y0,z0);
	TVector3 PT(0,0,0);
	TVector3 LocalPositionRot(0,0,0);
	TVector3 magField(0,1,0); // T
	TVector3 magFieldS(0,1,0);// T m
	
	//      int nSteps = (PVetoCenter.Z() + 0.5*PVetoSize.Z() - z0)/stepL*100;
	int nSteps = (ecalZ + ecalLength - z0)/stepL*20;//100;

	bool insideVeto = kFALSE; // it is the PVeto or the EVeto, depending on the charge
	bool insideECal = kFALSE; // it is in the ECal
	int channelFired = -1;
	int ecalchannelFired = -1;
	double totalLength = 0;
	double ecalfireden = -1;
	// swim loop

	for (int is = 0; is< nSteps; is++){
	  magField.SetY(getBYfield(LocalPosition.X(),LocalPosition.Y(),LocalPosition.Z())*bScale);
	  if (magField.Mag() > 1E-8){
	    magFieldS.SetXYZ(magField.X()*stepLM,magField.Y()*stepLM,magField.Z()*stepLM); // Tm
	    PT = LocalDirection.Cross(magFieldS);
	    PT *= charge[ip]*0.3*1E3/LocalDirection.Mag(); // Length in m, PT in MeV
	    double localMag = LocalDirection.Mag();
	    LocalDirection += PT;
	    LocalDirection *= (localMag/LocalDirection.Mag());
	  }
	  TVector3 dStep(stepL*LocalDirection.X()/LocalDirection.Mag(),
			 stepL*LocalDirection.Y()/LocalDirection.Mag(),
			 stepL*LocalDirection.Z()/LocalDirection.Mag());
	  
	  LocalPosition.SetX(LocalPosition.X() + dStep.X());
	  LocalPosition.SetY(LocalPosition.Y() + dStep.Y());
	  LocalPosition.SetZ(LocalPosition.Z() + dStep.Z());

	  totalLength += stepL;

	  if (fVerbose && (is%30 == 0)) {
	    cout << "In detailed magnetic Transport: PT kick = ( "<< PT.X() << ", "<< PT.Y()<<", "<< PT.Z()<< ") "
		 << " mom[MeV] = ( " << LocalDirection.X() << " ,"<<LocalDirection.Y() << " ,"<< LocalDirection.Z() << ")"
		 << " pos[mm] = ( " << LocalPosition.X() << " ,"<<LocalPosition.Y() << " ,"<< LocalPosition.Z() << ")"
		 << " local pos[mm] = ( " << LocalPositionRot.X() << " ,"<<LocalPositionRot.Y() << " ,"<< LocalPositionRot.Z() << ")"
		 << " magField[Tm] = ( " << magFieldS.X() << " ,"<<magFieldS.Y() << " ,"<<magFieldS.Z() << ")"<< endl;
	  }

	  //	if (
	  //	    fabs(LocalPosition.X()) > PVetoCenter.X()+PVetoSize.X() ||
	  //	    fabs(LocalPosition.Y()) > PVetoCenter.Y()+PVetoSize.Y() ||
	  //	    LocalPosition.Z() > PVetoCenter.Z()+PVetoSize.Z()) break;
	  if (LocalPosition.Z() > ecalZ+ecalLength) break;

	  if (!insideVeto &&
	      (TMath::Abs(charge[ip]*LocalPosition.X()-PVetoCenter.X()) < 0.5*PVetoSize.X()) && 
	      (TMath::Abs(LocalPosition.Z()-PVetoCenter.Z()) < 0.5*PVetoSize.Z()) && 
	      (TMath::Abs(LocalPosition.Y()-PVetoCenter.Y()) < 0.5*PVetoSize.Y())   ) {
	    TVector2 pt(LocalPosition.Z(),charge[ip]*LocalPosition.X()); //make the mirror x->-x for negative charges
	    int ifingerGuess = (LocalPosition.Z() - (fPVetoFrontFacePosZ + fFingerZOffset))/fFingerPitch;
	    int istart = TMath::Max(ifingerGuess-5,0);
	    int iend = TMath::Min(ifingerGuess+5,nVetoNFingers[ip]);
	    for (int i= istart; i<iend; i++){
	      if (PointInTriangle(pt,pzx[0][i],pzx[1][i],pzx[2][i]) || PointInTriangle(pt,pzx[0][i],pzx[2][i],pzx[3][i])) {
		insideVeto = kTRUE; 
		channelFired = i;
		break;
	      }
	    }
	    if (insideVeto)  break;
	  }

	  if (!insideECal) {
	    if (LocalPosition.Z()-ecalZ > 0 && LocalPosition.Z()-ecalZ < ecalLength) {

	      int icellX = LocalPosition.X()/cellSize+0.5 + ncells/2;
	      int icellY = LocalPosition.Y()/cellSize+0.5 + ncells/2;


	      if (LocalPosition.Perp() < ecalEdge) {
	    
		if (icellX < ncells && !(icellX < 0)) { 
		  if (icellY < ncells && !(icellY < 0)) {
		    bool hole = TMath::Abs(icellX-ncells/2) <= 2 && TMath::Abs(icellY-ncells/2) <=2;
		    if (!hole) {
		      insideECal = kTRUE;
		      ecalchannelFired = icellX + ncells*icellY;
		      ecalfireden = LocalDirection.Mag();
		      break;
		    }
		  }
		}
	      }
	    }
	  }

	} // loop over swim step

	if (insideVeto) {
	  firedChannels[ip] = channelFired;
	  lengths[ip] = totalLength;
	  if (channelFired < 0 || channelFired >= nVetoNFingers[ip]) {
	    cout << "Wrong PVetoChannel evaluated " << channelFired << endl;
	  }
	  else {
	    Veto_ThetaVsP[channelFired][ip]->Fill(eeMom[ip].Vect().Mag(),eeMom[ip].Vect().Theta()*1000.,weight);
	    Veto_ThetaVsPhi[channelFired][ip]->Fill(eeMom[ip].Vect().Phi(),eeMom[ip].Vect().Theta()*1000.,weight);
	    VetoInXZ[ip]->Fill(LocalPosition.Z(),charge[ip]*LocalPosition.X(),weight);
	  }
	}
	else {
	  firedChannels[ip] = -1;
	  lengths[ip] = 0;
	}
	if (insideECal) {
	  ecalfiredChannels[ip] = ecalchannelFired;	
	  ecalfiredEnergies[ip] = ecalfireden;
	}
      } // loop over e+/e- 

      VetoChannelsFiredAll[ib]->Fill(firedChannels[0],firedChannels[1],weight);
      if (firedChannels[0]>=0 && firedChannels[1]>=0) {
	VetoChannelsFired[ib]->Fill(firedChannels[0],firedChannels[1],weight);
	//      if (chosenChannels(firedChannels[0],firedChannels[1]) VetoChannelsFiredGood->Fill(firedChannels[0],firedChannels[1],weight);
      }

      ECalChannelsFiredAll->Fill(ecalfiredChannels[0],ecalfiredChannels[1],weight);
      if (ecalfiredChannels[0]>=0 && ecalfiredChannels[1]>=0) {
	ECalChannelsFired->Fill(ecalfiredChannels[0],ecalfiredChannels[1],weight);
	ECalFiredEn->Fill(ecalfiredEnergies[0],ecalfiredEnergies[1],weight);
	TVector2 posclus[2];
	for (int ig = 0; ig<2; ig++){
	  int icellY = ecalfiredChannels[ig]/ncells;
	  int icellX = ecalfiredChannels[ig] - icellY*ncells;	
	  posclus[ig].SetX(fXMin+21*icellX);
	  posclus[ig].SetY(fYMin+21*icellY);
	  ECalYXFired->Fill((icellX-ncells/2)*cellSize,(icellY-ncells/2)*cellSize,weight);
	}
	ECalFiredDist->Fill((posclus[0]-posclus[1]).Mod(),TMath::ASin((posclus[0].Y()*posclus[1].X()-posclus[0].X()*posclus[1].Y())/(posclus[0].Mod()*posclus[1].Mod())),weight);

      }

      if (firedChannels[0]>=0 && firedChannels[1]>=0) {
	double DT = lengths[0]/(eeMom[0].Beta()*TMath::C()/1E6) - lengths[1]/(eeMom[1].Beta()*TMath::C()/1E6);
	VetoChannelsFiredDT[ib]->Fill(firedChannels[0],firedChannels[1],weight*DT);
      }
    } // loop over MC events



  }

  TFile* filo = new TFile(filename.Data(),"RECREATE");

  TH2D* dtVsdiff = new TH2D("dtvsdiff","average dt vs PVeto-EVeto",181,-90.5,90.5,100,-5,5);
  for (int ib = 0; ib<nBSteps; ib++){  
    for (int ix=0; ix<VetoChannelsFired[ib]->GetXaxis()->GetNbins(); ix++){ 
      for (int iy=0; iy<VetoChannelsFired[ib]->GetYaxis()->GetNbins(); iy++){ 
	double deno = VetoChannelsFired[ib]->GetBinContent(ix+1,iy+1);
	if (deno > 0) { 
	  VetoChannelsFiredDT[ib]->SetBinContent(ix+1,iy+1,VetoChannelsFiredDT[ib]->GetBinContent(ix+1,iy+1)/deno);
	  dtVsdiff->Fill(ix-iy,VetoChannelsFiredDT[ib]->GetBinContent(ix+1,iy+1));
	}
      }
    }
  }
  dtVsdiff->Write();


  ECalChannelsFiredAll->Write();
  ECalChannelsFired->Write();
  ECalYXFired->Write();
  ECalFiredEn->Write();
  ECalFiredDist->Write();

  TGraph* VetoAccVsBscale = new TGraph();
  VetoAccVsBscale->SetName(Form("Acceptance%s_sqrts%f",modeLabel[mode].Data(),sqrts));
  for (int ib = 0; ib<nBSteps; ib++){  
    VetoChannelsFiredAll[ib]->Write();
    VetoChannelsFired[ib]->Write();
    VetoChannelsFiredDT[ib]->Write();
    double bScale = bScaleMin + dbw*ib;
    VetoAccVsBscale->SetPoint(ib, bScale, VetoChannelsFired[ib]->GetSum()/VetoChannelsFiredAll[ib]->GetSum());
  }
  VetoAccVsBscale->Write();

  for (int ip = 0; ip<2; ip++) {
    VetoInXZ[ip]->Write();
    for (int i=0; i<nEVetoNFingers; i++){
      Veto_ThetaVsP[i][ip]->Write();
      Veto_ThetaVsPhi[i][ip]->Write();
    }
  }





  dcos->Write();
  dcos_scha->Write();
  bfieldy->Write();
  filo->Close();



}

double dsdoBha(double Estar, double Pstar, double cosThe){
  double p_k = 2*Estar*Estar-me*me;
  double pprime_k = Estar*Estar + Pstar*Pstar*cosThe;
  double kprime_k = Estar*Estar - Pstar*Pstar*cosThe;
  double pprime_p = kprime_k;
  double kprime_p = pprime_k;
  double pprime_kprime = Estar*Estar + Pstar*Pstar;
  double t = 2*me*me - 2*pprime_p;
  double s = 2*me*me + 2*p_k;

  // e^2 = 4pi alpha hc

  double e2 = 4*TMath::Pi()*alpha*hc; // = e^2
  
  double iMfi = 8*e2*e2*(
			 1./(s*s)*(pprime_p*kprime_k + pprime_k*kprime_p + me*me*pprime_kprime + me*me*p_k + 2*me*me*me*me)
			 -1/(s*t)*(pprime_p*pprime_k - pprime_k*p_k)
			 -1/(s*t)*(kprime_k*kprime_p - kprime_p*p_k)
			 +1/(t*t)*(kprime_p*pprime_k + pprime_kprime*p_k - me*me*pprime_p - me*me*kprime_k + 2*me*me*me*me)
			 );
  return iMfi/(64*TMath::Pi()*TMath::Pi()*s);
}


double dsdoBha_scha(double Estar, double Pstar, double cosThe){
  double p_k = 2*Estar*Estar-me*me;
  double pprime_k = Estar*Estar + Pstar*Pstar*cosThe;
  double kprime_k = Estar*Estar - Pstar*Pstar*cosThe;
  double pprime_p = kprime_k;
  double kprime_p = pprime_k;
  double pprime_kprime = Estar*Estar + Pstar*Pstar;
  double t = 2*me*me - 2*pprime_p;
  double s = 2*me*me + 2*p_k;

  // e^2 = 4pi alpha hc

  double e2 = 4*TMath::Pi()*alpha*hc; // = e^2
  
  double iMfi = 8*e2*e2*(
			 1./(s*s)*(pprime_p*kprime_k + pprime_k*kprime_p + me*me*pprime_kprime + me*me*p_k + 2*me*me*me*me)
			 );
  return iMfi/(64*TMath::Pi()*TMath::Pi()*s);
}



double getBYfield(double x0, double y0, double z0){ // input in mm
  double fConstantMagneticField = 0.36;//0.4542; // [T] 
  //fConstantMagneticField = 0.;                                                                                                                                                                      

  double fConstantMagneticFieldXmin = -26.0;//[cm]*cm;
  double fConstantMagneticFieldXmax =  26.0;//[cm]*cm;

  double fConstantMagneticFieldZmin = -37.5;// [cm]*cm;
  double fConstantMagneticFieldZmax =  37.5;// [cm]*cm;

  double fSigmaFront = 27.4;//*cm;   //Based of the LNF magnetic measurement M. Raggi .ppt nov 2018                                                                                                             
  double fSigmaBack  = 27.4;//*cm;   //Based of the LNF magnetic measurement M. Raggi .ppt nov 2018                                                                                                             

  // The magnetic volume is a box centered at magnet center with x and y dimensions                                                                                                                   
  // as the magnet cavity and with z extends 50cm outside both sides of the magnet                                                                                                                    
  double fMagneticVolumePosZ = 0.;
  double fMagneticVolumeLengthX = 112.;//*cm;
  double fMagneticVolumeLengthY = 23.;//*cm;
  double fMagneticVolumeLengthZ = 200.;//*cm;

  double x = x0*0.1;
  double y = y0*0.1;
  double z = z0*0.1-fMagneticVolumePosZ;

  double B0 = 1.;
  if ( (x<-0.5*fMagneticVolumeLengthX) || (x>0.5*fMagneticVolumeLengthX) ||
       (y<-0.5*fMagneticVolumeLengthY) || (y>0.5*fMagneticVolumeLengthY) ||
       (z<-0.5*fMagneticVolumeLengthZ) || (z>0.5*fMagneticVolumeLengthZ) ) {
    // Field outside magnetic volume is always null                                                                                                                                                   
    B0 = 0.;
  } else if (x<fConstantMagneticFieldXmin || x>fConstantMagneticFieldXmax) {
    // Will need a function/map to smoothly send B0 to 0 along X                                                                                                                                      
    B0 = 0.;
  } else if (z<fConstantMagneticFieldZmin) {
    // Use gaussian to model upstream magnetic field rise                                                                                                                                             
    double dZS = (z-fConstantMagneticFieldZmin)/fSigmaFront;
    B0 = exp(-dZS*dZS);
  } else if (z>fConstantMagneticFieldZmax) {
    // Use gaussian to model downstream magnetic field fall                                                                                                                                           
    double dZS = (z-fConstantMagneticFieldZmax)/fSigmaBack;
    B0 = exp(-dZS*dZS);
  }
  return -fConstantMagneticField*B0;
}



bool PointInTriangle(TVector2 pt, TVector2 v1, TVector2 v2, TVector2 v3)
{
  bool b1, b2, b3;

  b1 = signV(pt, v1, v2) < 0.0f;
  b2 = signV(pt, v2, v3) < 0.0f;
  b3 = signV(pt, v3, v1) < 0.0f;

  return ((b1 == b2) && (b2 == b3));
}

float signV(TVector2 p1, TVector2 p2, TVector2 p3)
{
  return (p1.X() - p3.X()) * (p2.Y() - p3.Y()) - (p2.X() - p3.X()) * (p1.Y() - p3.Y());
}


bool chosenChannels(int c, int d){
  return d < -75*(c*c/(77*77.)-1) ;
}
