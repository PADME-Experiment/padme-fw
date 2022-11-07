#ifndef VetoEndPoint_h
#define VetoEndPoint_h

#include "TVector3.h"
#include "TMath.h"
#include "TFile.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TH2D.h"
#include "HistoSvc.hh"

class VetoEndPoint {

protected :
  VetoEndPoint();

public :
  ~VetoEndPoint();
  static VetoEndPoint* GetInstance();
  void ParticleSwim(TVector3 startmomentum, TVector3 startposition, Double_t starttime, Int_t particlecharge);
  int GetEndFinger(){return fEndFinger;};
  double GetEndTime(){return fEndTime;};
  TVector3 GetEndPosition(){return fEndPosition;};
  void SaveHistos();

 private :
  HistoSvc* fHS;
  char name[256];
  
  int NoutX;
  int NoutY;
  int NoutZ;
  int Nin;
  int NTotal;
  
  int fEndFinger;
  double fEndTime;
  TVector3 fEndPosition;
  double fStepLength;

  static VetoEndPoint* fInstance;
  void SetupPVetoGeometry();
  void InitialiseHistos();
  void PlotFingerGraphs();
  double getBYfield(double x0, double y0, double z0);
  bool PointInTriangle(TVector2 pt, TVector2 v1, TVector2 v2, TVector2 v3);
  float signV(TVector2 p1, TVector2 p2, TVector2 p3);
  // set starting point
  const double x0 = 0; // mm
  const double y0 = 0; // mm
  const double z0 = -1028; // mm
  const double stepL = 0.2;//0.1; // mm
  double stepLM = stepL*1E-3; //m

  // set pveto geometry
  const double fFingerPitch = 11.;// *mm 
  static const int nPVetoNFingers = 90;
  static const int nEVetoNFingers = 96;
  int nVetoNFingers[2] = {nPVetoNFingers,nEVetoNFingers};
  const double fFingerSizeX =  10;// mm
  const double fFingerSizeY = 178.; // mm
  const double fFingerSizeZ =  10.; // mm;
  const double fFingerRotY = -10.*TMath::Pi()/180.;//10 degrees = 10*pi/180 radians
  const double fPVetoInnerFacePosX =  177.5;// mm inner face X of structure enclosing the PVETO
  const double fPVetoFrontFacePosZ = -472.55;// *mm; // upstream face Z of structure enclosing the PVETO. Start 56.45mm from inner face of vacuum chamber (wrong but real position M. Raggi) 
  TVector3 PVetoSize = TVector3(32.,188.,1095.); // size of the structure enclosing the PVETO
  TVector3 PVetoCenter = TVector3(193.5,0.,74.95); // position of the center of the structure enclosing the PVETO
  
  const double fFingerXOffset = 15.; //mm Each finger is placed 15 mm in x inside the innerFace of the structure enclosing the PVETO
  const double fFingerZOffset = 26.; //mm Finger0 is placed 26 mm in Z inside the frontFace of the structure enclosing the PVETO
  // finger i has a center in (fPVetoInnerFacePosX+fFingerXOffset,0,fPVetoFrontFacePosZ+fFingerZOffset+i*fFingerPitch)
  // finger i is then rotated by fFingerRotY around the Y axis

  double fConstantMagneticField = 0.4542; // [T] 
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

  TVector2 pzxRot[4]; // fill points in the ZX plane: Z->x and X->y
  TVector2 pzx[4][96]; // fill points in the ZX plane: Z->x and X->y

  TFile * vetoendfile = new TFile("VetoEndPointOutFile.root","RECREATE");

  TMultiGraph * AllFingerGraph = new TMultiGraph("PVetoFingersZX","PVetoFingersZX");
  TGraph* PVetoFingerGraphs[nEVetoNFingers];

  TH1D* hbfieldy  = new TH1D("hbfieldy","bfield y component",400,-2000.,2000.);

};
#endif
