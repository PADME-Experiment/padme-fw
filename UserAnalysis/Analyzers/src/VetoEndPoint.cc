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
  fHS = HistoSvc::GetInstance();
  SetupPVetoGeometry();
  fVetoEndPointHistosExist=0;
  fVetoEndPointHistosExist = InitialiseHistos();
  vetoendfile->cd();
  
  NoutFlange=NoutX=NoutY=NoutZ=Nin=NTotal=Neplus=Neminus=Ngamma=0;
  std::cout<<"Nout X "<<NoutX<<" NoutY "<<NoutY<<" NoutZ "<<NoutZ<<std::endl;
}

VetoEndPoint::~VetoEndPoint(){}

bool VetoEndPoint::InitialiseHistos(){
  double Ebeam = 430.;//MeV

  for (int i=0; i<400; i++){
    hbfieldy->SetBinContent(i+1,getBYfield(0,0,-2000.+10.*i));
  }

  fHS->CreateList("VetoEndPointList");
  fHS->CreateList("VetoEndPointList");

  fHS->BookHistoList("VetoEndPointList","hPVetoChID",90,-0.5,89.5);
  fHS->BookHistoList("VetoEndPointList","hEVetoChID",96,-0.5,96.5);
  
  fHS->CreateList("VetoEndPointList/PVetoThetaVsP");
  fHS->CreateList("VetoEndPointList/EVetoThetaVsP");
  
  fHS->CreateList("VetoEndPointList/PVetoThetaVsPhi");
  fHS->CreateList("VetoEndPointList/EVetoThetaVsPhi");  

  for(int ChID=0;ChID<nEVetoNFingers;ChID++){
    sprintf(name,"hEVeto_ThetaVsP_Channel%d",ChID);
    fHS->BookHisto2List("VetoEndPointList/EVetoThetaVsP",name,100,0.,Ebeam,101,0.5,200.5);

    sprintf(name,"hEVeto_ThetaVsPhi_Channel%d",ChID);
    fHS->BookHisto2List("VetoEndPointList/EVetoThetaVsPhi",name,180,-1*TMath::Pi(),TMath::Pi(),101,0.5,200);

    if(ChID<nPVetoNFingers){
      sprintf(name,"hPVeto_ThetaVsP_Channel%d",ChID);
      fHS->BookHisto2List("VetoEndPointList/PVetoThetaVsP",name,100,0.,Ebeam,101,0.5,200.5);

      sprintf(name,"hPVeto_ThetaVsPhi_Channel%d",ChID);
      fHS->BookHisto2List("VetoEndPointList/PVetoThetaVsPhi",name,180,-1*TMath::Pi(),TMath::Pi(),101,0.5,200.5);
    }
  }
  return 1;
}

void VetoEndPoint::SaveHistos(){
  if(vetoendfile)
    vetoendfile->cd();
  else std::cout<<"vetoendfile doesn't exist in VetoEndPoint::SaveHistos()"<<std::endl;

  AllFingerGraph->Write();
  hbfieldy->Write();

  vetoendfile->Close();
  std::cout<<"NTotal "<<NTotal<<" Neplus "<<Neplus<<" Neminus "<<Neminus<<" Ngamma "<<Ngamma<<" NOutFlange "<<NoutFlange<<" NOutX "<<NoutX<<" NoutY "<<NoutY<<" NoutZ "<<NoutZ<<" Nin "<<Nin<<std::endl;
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
}

void VetoEndPoint::ParticleSwim(TLorentzVector FourMomentum, TVector3 startposition, Double_t starttime, Int_t particlecharge){
  //for Bhabhas originating from the target, where do they end up?
  NTotal++;

  // std::cout<<"to swim, charge "<<particlecharge<<" energy "<<FourMomentum.E()<<"  momentum "<<FourMomentum.Px()<<" "<<FourMomentum.Py()<<" "<<FourMomentum.Pz()<<" position "<<startposition.X()<<" "<<startposition.Y()<<" "<<startposition.Z()<<std::endl;
  
  fEndFinger = -10000;
  fEndTime = -10000.;
  fEndPosition = TVector3(-10000,-10000,-10000);
  fStepLength = 0.2;//0.1; // mm

  if(particlecharge==-1) Neminus++;
  if(particlecharge==1)  Neplus++;
  if(particlecharge==0)  Ngamma++;
  
  double StepLengthMetres = fStepLength*1e-3;

  //copy starting values into local variables so that the starting variables don't change during this process
  TVector3 LocalMomentum = FourMomentum.Vect();
  //TVector3 LocalDirection = LocalMomentum*(1/LocalMomentum.Mag());
  TVector3 LocalPosition = startposition;

  double c = 3e8;
  // double Mass = FourMomentum.M()*1e3;//FourMomentum.M() returns mass in GeV, so multiply by 1000 to get MeV
  // std::cout<<FourMomentum.P()<<" "<<FourMomentum.E()<<" "<<FourMomentum.M()<<std::endl;
  double beta = FourMomentum.P()/FourMomentum.E();//velocity/c;
  double betagamma = FourMomentum.P()/FourMomentum.M();//velocity/c;
  double gamma = betagamma/beta;

  //  std::cout<<FourMomentum.P()<<" "<<Mass<<" "<<beta<<" "<<gamma<<std::endl;
  //if(fabs(LocalMomentum.Mag()-1.)>1e-9) std::cout<<"dir mag problem "<<LocalDirection.Mag()<<std::endl;

  TVector3 PT(0,0,0);
  TVector3 magField(0,1,0); // T
  TVector3 magFieldS(0,1,0);// T m -> magnetic field integrated over step length
  
  int nSteps = (PVetoCenter.Z() + 0.5*PVetoSize.Z() - z0)/stepL*100; //steps of 0.01mm
  bool insideVeto = kFALSE;
  double fTotalPathLength = 0; //total length of trajectory (mm?)
  double totalTime = 0; //total length of trajectory (mm?)

  Bool_t outX = 0;
  Bool_t outY = 0;
  Bool_t outZ = 0;

  Double_t Xloc;
  Double_t Yloc;
  Double_t Zloc;
  
  for (int istep = 0; istep< nSteps; istep++){
     //Assuming that any fringe fields in X and Z have little imapct, we simulate only the field in the Y direction
     magField.SetY(getBYfield(LocalPosition.X(),LocalPosition.Y(),LocalPosition.Z()));
     if (magField.Mag() > 1E-8){
       //Change in direction depends on length travelled in a certain B field, therefore calculate B*dS where dS is distance travelled in a certain step
       magFieldS.SetXYZ(magField.X()*StepLengthMetres,magField.Y()*StepLengthMetres,magField.Z()*StepLengthMetres); // Tm
       PT = LocalMomentum.Cross(magFieldS);

       //0.3e3 -> c mm/ns(?)
       PT *= particlecharge*0.3*1E3/LocalMomentum.Mag(); // Length in m, PT in MeV, charge in +/- 1
       double localMag = LocalMomentum.Mag();
       LocalMomentum += PT;
       LocalMomentum *= (localMag/LocalMomentum.Mag());
     }
     
     TVector3 dStep(stepL*LocalMomentum.X()/LocalMomentum.Mag(),
		    stepL*LocalMomentum.Y()/LocalMomentum.Mag(),
		    stepL*LocalMomentum.Z()/LocalMomentum.Mag());
     
     LocalPosition.SetX(LocalPosition.X() + dStep.X());
     LocalPosition.SetY(LocalPosition.Y() + dStep.Y());
     LocalPosition.SetZ(LocalPosition.Z() + dStep.Z());
     
     fTotalPathLength += stepL*1e-3; //m
     //     std::cout<<fTotalPathLength<<std::endl;

     Xloc = LocalPosition.X();
     Yloc = LocalPosition.Y();
     Zloc = LocalPosition.Z();
     
     //if particle is outside the flange aperture, move on to next particle
     if (fabs(Zloc-fVCInnerFacePosZ)<2&&TMath::Sqrt(Xloc*Xloc+Yloc*Yloc)>fFlangeRadius){
       NoutFlange++;
       break;
     }
     /*     if (istep%30 == 0) {
       std::cout << "In detailed magnetic Transport: PT kick = ( "<< PT.X() << ", "<< PT.Y()<<", "<< PT.Z()<< ") "
	    << " mom[MeV] = ( " << LocalMomentum.X() << " ,"<<LocalMomentum.Y() << " ,"<< LocalMomentum.Z() << ")"
	    << " pos[mm] = ( " << LocalPosition.X() << " ,"<<LocalPosition.Y() << " ,"<< LocalPosition.Z() << ")"
	    << " local pos[mm] = ( " << LocalPositionRot.X() << " ,"<<LocalPositionRot.Y() << " ,"<< LocalPositionRot.Z() << ")"
	    << " magField[Tm] = ( " << magFieldS.X() << " ,"<<magFieldS.Y() << " ,"<<magFieldS.Z() << ")"<<std::endl;
	    }*/

     //how many times is it out in X, how many Y and how many Z?
     
     Bool_t XCheck = (TMath::Abs(particlecharge*LocalPosition.X()-PVetoCenter.X()) < 0.5*PVetoSize.X());
     Bool_t ZCheck = (TMath::Abs(LocalPosition.Z()-PVetoCenter.Z()) < 0.5*PVetoSize.Z());
     Bool_t YCheck = (TMath::Abs(LocalPosition.Y()-PVetoCenter.Y()) < 0.5*PVetoSize.Y());

     if(!XCheck) outX = 1;
     if(!YCheck) outY = 1;
     if(!ZCheck) outZ = 1;
     
     //if particle is beyond the veto, move on to next particle
     if (
      	 fabs(LocalPosition.X()) > PVetoCenter.X()+PVetoSize.X() ||
      	 fabs(LocalPosition.Y()) > PVetoCenter.Y()+PVetoSize.Y() ||
      	 LocalPosition.Z() > PVetoCenter.Z()+PVetoSize.Z()) break; //what happens to these?
     
     
     //for as long as the particle is in the magnet gap and hasn't previously reached the veto, if it's now in the veto volume, work out which finger it's in
     if (!insideVeto &&
       //why support size, not finger size?
	 (TMath::Abs(particlecharge*LocalPosition.X()-PVetoCenter.X()) < 0.5*PVetoSize.X()) && 
	 (TMath::Abs(LocalPosition.Z()-PVetoCenter.Z()) < 0.5*PVetoSize.Z()) && 
	 (TMath::Abs(LocalPosition.Y()-PVetoCenter.Y()) < 0.5*PVetoSize.Y())   ) {

       outX=outY=outZ = 0;
      
       TVector2 point(LocalPosition.Z(),particlecharge*LocalPosition.X()); //make the mirror x->-x for negative charges
       int ifingerGuess = (LocalPosition.Z() - (fPVetoFrontFacePosZ + fFingerZOffset))/fFingerPitch;
       int istart = TMath::Max(ifingerGuess-5,0);
       int iend = TMath::Min(ifingerGuess+5,nEVetoNFingers);
       for (int ii= istart; ii<iend; ii++){
	 if (PointInTriangle(point,pzx[0][ii],pzx[1][ii],pzx[2][ii]) || PointInTriangle(point,pzx[0][ii],pzx[2][ii],pzx[3][ii])) {
	   Nin++;
	   insideVeto = kTRUE; 
	   fEndFinger = ii;
	   fEndTime = 1e9*fTotalPathLength/(c*beta)+(LocalPosition.Y()-PVetoSize.Y()/2)*1.5/c;//ns, including time of light propagation in scintillator to SiPM at the top
	   //	   std::cout<<"charge "<<particlecharge<<" energy "<<FourMomentum.E()<<"  momentum "<<FourMomentum.Px()<<" "<<FourMomentum.Py()<<" "<<FourMomentum.Pz()<<" position "<<startposition.X()<<" "<<startposition.Y()<<" "<<startposition.Z()<<" fEndFinger "<<fEndFinger<<" fEndTime "<<fEndTime<<std::endl;
	   //   if(particlecharge == -1) std::cout<<"I fire EVeto"<<std::endl;
	   break;
	 }
       }
       if (insideVeto)  break;
     }
  } // loop over swim step
  if (insideVeto) {
    if (fEndFinger < 0 || fEndFinger >= nEVetoNFingers) {
      std::cout << "Wrong VetoChannel evaluated " << fEndFinger << std::endl;
    }
    else {
      if(particlecharge==1&&fEndFinger<nPVetoNFingers){
	fHS->FillHistoList("VetoEndPointList","hPVetoChID",fEndFinger);
	
	sprintf(name,"hPVeto_ThetaVsP_Channel%d",fEndFinger);
	fHS->FillHisto2List("VetoEndPointList/PVetoThetaVsP",name,FourMomentum.Vect().Mag(),FourMomentum.Vect().Theta()*1000.);
	
	sprintf(name,"hPVeto_ThetaVsPhi_Channel%d",fEndFinger);
	fHS->FillHisto2List("VetoEndPointList/PVetoThetaVsPhi",name,FourMomentum.Vect().Phi(),FourMomentum.Vect().Theta()*1000.);
      }
      else if(particlecharge==-1&&fEndFinger<nEVetoNFingers){
	fHS->FillHistoList("VetoEndPointList","hEVetoChID",fEndFinger);
	
	sprintf(name,"hEVeto_ThetaVsP_Channel%d",fEndFinger);
	fHS->FillHisto2List("VetoEndPointList/EVetoThetaVsP",name,FourMomentum.Vect().Mag(),FourMomentum.Vect().Theta()*1000.);

	sprintf(name,"hEVeto_ThetaVsPhi_Channel%d",fEndFinger);
	fHS->FillHisto2List("VetoEndPointList/EVetoThetaVsPhi",name,FourMomentum.Vect().Phi(),FourMomentum.Vect().Theta()*1000.);
	 
       }

       //       VetoInXZ[ip]->Fill(LocalPosition.Z(),charge[ip]*LocalPosition.X(),weight);
     }
   }
   else {//if I haven't gone into the veto, why not?
     //     std::cout<<"I else, Nout X "<<NoutX<<" NoutY "<<NoutY<<" NoutZ "<<NoutZ<<std::endl;
     if(outX){
       //       std::cout<<"Out in X"<<std::endl;
       NoutX++;
     }
     if(outY){
       //       std::cout<<"Out in Y"<<std::endl;
       NoutY++;
     }
     if(outZ){
       //       std::cout<<"Out in Z"<<std::endl;
       NoutZ++;
     }
     if((!outX)&&(!outY)&&(!outZ)) std::cout<<"Not in Veto but not out in X, Y or Z!"<<std::endl;
     //    firedChannels[ip] = -1;
     //     lengths[ip] = 0;
   }
}

double VetoEndPoint::getBYfield(double x0, double y0, double z0){ // input in mm

  double x = x0*0.1;
  double y = y0*0.1;
  double z = z0*0.1-fMagneticVolumePosZ;
  
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
  //  std::cout<<"I point to triangles"<<std::endl;
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
