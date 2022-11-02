//Class to define whether particles interact in the Vetoes
//Created by Beth Oct 2022 from toy by Tommaso S.
#include "VetoEndPoint.hh"

#include <iostream>
#include "TVector2.h"
#include "TVector3.h"
//#include "TMath.h"
//#include "TCanvas.h"
#include "TAxis.h"

VetoEndPoint* VetoEndPoint::fInstance = 0;
VetoEndPoint* VetoEndPoint::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new VetoEndPoint(); }
  return fInstance;
}

VetoEndPoint::VetoEndPoint(){
  //Initialise geometry of the detector
  SetupPVetoGeometry();
  InitialiseHistos();
  vetoendfile->cd();
}

VetoEndPoint::~VetoEndPoint(){}

void VetoEndPoint::InitialiseHistos(){
  double Ebeam = 430.;//MeV
  for(int ChID=0;ChID<nEVetoNFingers;ChID++){
    EVeto_ThetaVsP[ChID] = new TH2D(Form("EVeto_ThetaVsP_Channel%d",ChID),Form("EVeto_ThetaVsP_Channel%d",ChID),100,0.,Ebeam,101,0.5,200.5);
    EVeto_ThetaVsPhi[ChID] = new TH2D(Form("EVeto_ThetaVsPhi_Channel%d",ChID),Form("Veto_ThetaVsPhi_Channel%d",ChID),180,-1*TMath::Pi(),TMath::Pi(),101,0.5,200.5);

    if(ChID<nPVetoNFingers){
      PVeto_ThetaVsP[ChID] = new TH2D(Form("PVeto_ThetaVsP_Channel%d",ChID),Form("EVeto_ThetaVsP_Channel%d",ChID),100,0.,Ebeam,101,0.5,200.5);
      PVeto_ThetaVsPhi[ChID] = new TH2D(Form("PVeto_ThetaVsPhi_Channel%d",ChID),Form("Veto_ThetaVsPhi_Channel%d",ChID),180,-1*TMath::Pi(),TMath::Pi(),101,0.5,200.5);
    }
  }
}

void VetoEndPoint::SaveHistos(){
  if(vetoendfile)
    vetoendfile->cd();
  else std::cout<<"vetoendfile doesn't exist in VetEndPoint::SaveHistos()"<<std::endl;

  vetoendfile->mkdir("EVeto_ThetaVsP");
  vetoendfile->mkdir("EVeto_ThetaVsPhi");

  vetoendfile->mkdir("PVeto_ThetaVsP");
  vetoendfile->mkdir("PVeto_ThetaVsPhi");

  for(int ChID=0;ChID<nEVetoNFingers;ChID++){
    vetoendfile->cd("EVeto_ThetaVsP");
    EVeto_ThetaVsP[ChID]->Write();

    vetoendfile->cd("EVeto_ThetaVsPhi");
    EVeto_ThetaVsPhi[ChID]->Write();

    if(ChID<nPVetoNFingers){
      vetoendfile->cd("PVeto_ThetaVsP");
      PVeto_ThetaVsP[ChID]->Write();

      vetoendfile->cd("EVeto_ThetaVsPhi");
      PVeto_ThetaVsPhi[ChID]->Write();
    }
  }
  vetoendfile->Close();
}

void VetoEndPoint::SetupPVetoGeometry(){
  //import geometry for PVeto

  for (int j=0; j<4; j++) { // points in clock-wise order
    int xsign = (j==0 || j==3)? -1 : 1; // -1, 1, 1, -1
    int ysign = (j==0 || j==1)?  1 : -1; //  1, 1,-1, -1
 
    double zrot = TMath::Cos(-fFingerRotY)*0.5*fFingerSizeZ*xsign - TMath::Sin(-fFingerRotY)*0.5*fFingerSizeX*ysign;
    double xrot = TMath::Sin(-fFingerRotY)*0.5*fFingerSizeZ*xsign + TMath::Cos(-fFingerRotY)*0.5*fFingerSizeX*ysign;
    pzxRot[j]=TVector2(zrot,xrot);						
  }

  for (int i=0; i<nEVetoNFingers; i++){
    for (int j=0; j<4; j++) { // points in clock-wise order
      double zstraight = pzxRot[j].X() + fPVetoFrontFacePosZ + fFingerZOffset + i*fFingerPitch;
      double xstraight = pzxRot[j].Y() + fPVetoInnerFacePosX + fFingerXOffset;
      pzx[j][i].Set(zstraight,xstraight);
    }
  }
  PlotFingerGraphs();
}

void VetoEndPoint::PlotFingerGraphs(){
  // plot finger vertices
  for (int i=0; i<nEVetoNFingers; i++){
    PVetoFingerGraphs[i] = new TGraph(4);
    PVetoFingerGraphs[i]->SetName(Form("Finger%d",i));
    for (int j=0; j<4; j++) { // points in clock-wise order
      PVetoFingerGraphs[i]->SetPoint(j,pzx[j][i].X(),pzx[j][i].Y());
      AllFingerGraph->Add(PVetoFingerGraphs[i]);
    }
  }

  for (int i=0; i<nEVetoNFingers; i++){
    if ((i+1)%2 == 0) continue;
    PVetoFingerGraphs[i]->SetLineColor(1);//black
    PVetoFingerGraphs[i]->SetLineStyle(1);//solid 
    PVetoFingerGraphs[i]->Draw("Lsame");
  }
  for (int i=0; i<nEVetoNFingers; i++){
    if (i%2 == 0) continue;
    PVetoFingerGraphs[i]->SetLineColor(2);//red
    PVetoFingerGraphs[i]->SetLineStyle(2);//dashed                                    
    PVetoFingerGraphs[i]->Draw("Lsame");
  }
  AllFingerGraph->Draw("AP");
  AllFingerGraph->GetXaxis()->SetTitle("Z position (mm)");
  AllFingerGraph->GetYaxis()->SetTitle("X position (mm)");
  AllFingerGraph->Write();
}

void VetoEndPoint::ParticleSwim(TVector3 startmomentum, TVector3 startposition, Double_t starttime, Int_t particlecharge){
  //for Bhabhas originating from the target, where do they end up?

  fEndFinger = -100;
  fEndTime = -100.;
  fEndPosition = TVector3(-10000,-10000,-10000);
  fStepLength = 0.2;//0.1; // mm

  double StepLengthMetres = fStepLength*1e-3;

  //copy starting values into local variables so that the starting variables don't change during this process
  TVector3 LocalMomentum = startmomentum;
  TVector3 LocalDirection = LocalMomentum*(1/LocalMomentum.Mag());
  TVector3 LocalPosition = startposition;

  if(fabs(LocalDirection.Mag()-1.)>1e-9) std::cout<<"dir mag problem "<<LocalDirection.Mag()<<std::endl;

  TVector3 PT(0,0,0);
  TVector3 magField(0,1,0); // T
  TVector3 magFieldS(0,1,0);// T m -> magnetic field integrated over step length
  
  int nSteps = (PVetoCenter.Z() + 0.5*PVetoSize.Z() - z0)/stepL*100; //steps of 0.01mm
  bool insideVeto = kFALSE;
  int channelFired = -1; //channelID of channel where particle finishes
  double totalLength = 0; //total length of trajectory (mm?)

   for (int is = 0; is< nSteps; is++){
     //Assuming that any fringe fields in X and Z have little imapct, we simulate only the field in the Y direction
     magField.SetY(getBYfield(LocalPosition.X(),LocalPosition.Y(),LocalPosition.Z()));
     
     if (magField.Mag() > 1E-8){
       //Change in direction depends on length travelled in a certain B field, therefore calculate B*dS where dS is distance travelled in a certain step
       magFieldS.SetXYZ(magField.X()*StepLengthMetres,magField.Y()*StepLengthMetres,magField.Z()*StepLengthMetres); // Tm
       PT = LocalDirection.Cross(magFieldS);

       //0.3e3 -> c mm/ns(?)
       PT *= particlecharge*0.3*1E3/LocalDirection.Mag(); // Length in m, PT in MeV, charge in +/- 1

       LocalDirection += PT;
       LocalDirection *= (1/LocalDirection.Mag());
     }
    
     TVector3 dStep(stepL*LocalDirection.X()/LocalDirection.Mag(),
		    stepL*LocalDirection.Y()/LocalDirection.Mag(),
		    stepL*LocalDirection.Z()/LocalDirection.Mag());
     
     LocalPosition.SetX(LocalPosition.X() + dStep.X());
     LocalPosition.SetY(LocalPosition.Y() + dStep.Y());
     LocalPosition.SetZ(LocalPosition.Z() + dStep.Z());
     
     totalLength += stepL;
   
     /*     if (is%30 == 0) {
       std::cout << "In detailed magnetic Transport: PT kick = ( "<< PT.X() << ", "<< PT.Y()<<", "<< PT.Z()<< ") "
	    << " mom[MeV] = ( " << LocalDirection.X() << " ,"<<LocalDirection.Y() << " ,"<< LocalDirection.Z() << ")"
	    << " pos[mm] = ( " << LocalPosition.X() << " ,"<<LocalPosition.Y() << " ,"<< LocalPosition.Z() << ")"
	    << " local pos[mm] = ( " << LocalPositionRot.X() << " ,"<<LocalPositionRot.Y() << " ,"<< LocalPositionRot.Z() << ")"
	    << " magField[Tm] = ( " << magFieldS.X() << " ,"<<magFieldS.Y() << " ,"<<magFieldS.Z() << ")"<<std::endl;
	    }*/
     
     //if particle is beyond the veto, move on to next particle
     if (
	 fabs(LocalPosition.X()) > PVetoCenter.X()+PVetoSize.X() ||
	 fabs(LocalPosition.Y()) > PVetoCenter.Y()+PVetoSize.Y() ||
	 LocalPosition.Z() > PVetoCenter.Z()+PVetoSize.Z()) break;
     
     Bool_t XCheck = (TMath::Abs(particlecharge*LocalPosition.X()-PVetoCenter.X()) < 0.5*PVetoSize.X());
     Bool_t ZCheck = (TMath::Abs(LocalPosition.Z()-PVetoCenter.Z()) < 0.5*PVetoSize.Z());
     Bool_t YCheck = (TMath::Abs(LocalPosition.Y()-PVetoCenter.Y()) < 0.5*PVetoSize.Y());

     /*     if(XCheck&&ZCheck&&YCheck&&particlecharge==-1)
	    std::cout<<"insideVeto "<<insideVeto<<" XCheck "<<XCheck<<" ZCheck "<<ZCheck<<" YCheck "<<YCheck<<std::endl;*/

     //for as long as the particle is in the magnet gap and hasn't previously reached the veto, if it's now in the veto volume, work out which finger it's in
     if (!insideVeto &&
       //why support size, not finger size?
	 (TMath::Abs(particlecharge*LocalPosition.X()-PVetoCenter.X()) < 0.5*PVetoSize.X()) && 
	 (TMath::Abs(LocalPosition.Z()-PVetoCenter.Z()) < 0.5*PVetoSize.Z()) && 
	 (TMath::Abs(LocalPosition.Y()-PVetoCenter.Y()) < 0.5*PVetoSize.Y())   ) {
       //       if(particlecharge==-1) std::cout<<"I've entered!"<<std::endl;
       TVector2 point(LocalPosition.Z(),particlecharge*LocalPosition.X()); //make the mirror x->-x for negative charges
       int ifingerGuess = (LocalPosition.Z() - (fPVetoFrontFacePosZ + fFingerZOffset))/fFingerPitch;
       int istart = TMath::Max(ifingerGuess-5,0);
       int iend = TMath::Min(ifingerGuess+5,nEVetoNFingers);
       for (int i= istart; i<iend; i++){
	 if (PointInTriangle(point,pzx[0][i],pzx[1][i],pzx[2][i]) || PointInTriangle(point,pzx[0][i],pzx[2][i],pzx[3][i])) {
	   insideVeto = kTRUE; 
	   channelFired = i;
	   //   if(particlecharge == -1) std::cout<<"I fire EVeto"<<std::endl;
	   break;
	 }
       }
       if (insideVeto)  break;
     }
   } // loop over swim step
   
   if (insideVeto) {
     if (channelFired < 0 || channelFired >= nEVetoNFingers) {
       std::cout << "Wrong VetoChannel evaluated " << channelFired << std::endl;
     }
     else {
       if(particlecharge==1&&channelFired<nPVetoNFingers){
	 PVeto_ThetaVsP[channelFired]->Fill(startmomentum.Mag(),startmomentum.Theta()*1000.);//,weight);
	 PVeto_ThetaVsPhi[channelFired]->Fill(startmomentum.Phi(),startmomentum.Theta()*1000.);//,weight);
       }
       else if(particlecharge==-1&&channelFired<nEVetoNFingers){
	 //	 std::cout<<channelFired<<std::endl;
	 EVeto_ThetaVsP[channelFired]->Fill(startmomentum.Mag(),startmomentum.Theta()*1000.);//,weight);
	 EVeto_ThetaVsPhi[channelFired]->Fill(startmomentum.Phi(),startmomentum.Theta()*1000.);//,weight);
       }
       //       VetoInXZ[ip]->Fill(LocalPosition.Z(),charge[ip]*LocalPosition.X(),weight);
     }
   }
   else {
     //    firedChannels[ip] = -1;
     //     lengths[ip] = 0;
   }
}

double VetoEndPoint::getBYfield(double x0, double y0, double z0){ // input in mm

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

bool VetoEndPoint::PointInTriangle(TVector2 pt, TVector2 v1, TVector2 v2, TVector2 v3)
{
  bool b1, b2, b3;

  b1 = signV(pt, v1, v2) < 0.0f;
  b2 = signV(pt, v2, v3) < 0.0f;
  b3 = signV(pt, v3, v1) < 0.0f;

  //if signV of all points has the same sign, the point passes 
  return ((b1 == b2) && (b2 == b3));
}

//returns |distance from triangle centre| * sin(angle)
float VetoEndPoint::signV(TVector2 p1, TVector2 p2, TVector2 p3)
{
  return (p1.X() - p3.X()) * (p2.Y() - p3.Y()) - (p2.X() - p3.X()) * (p1.Y() - p3.Y());
}
