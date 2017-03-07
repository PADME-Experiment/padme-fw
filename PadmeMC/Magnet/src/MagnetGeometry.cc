// MagnetGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-01-11
// --------------------------------------------------------------

#include "MagnetGeometry.hh"

MagnetGeometry* MagnetGeometry::fInstance = 0;

MagnetGeometry* MagnetGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new MagnetGeometry(); }
  return fInstance;
}

MagnetGeometry::MagnetGeometry()
{

  // Inizialize default parameters

  fMagnetGap = 0.1*mm;

  fYokeSizeZ      = 100.0*cm;
  fYokeSizeOuterX = 174.0*cm;
  fYokeSizeOuterY = 125.0*cm;
  fYokeSizeInnerX = 112.0*cm;
  fYokeSizeInnerY =  63.0*cm;
  fYokeSizeInBarX =  52.0*cm;
  fYokeSizeInBarY =  20.0*cm;

  fCoilInnerRadius =   5.5*cm;    // Inner radius of curved sections of coil
  fCoilOuterRadius =  29.0*cm;    // Outer radius of curved sections of coil
  fCoilLengthX     =  46.0*cm;    // Length along X of straight sections of coil outside the yoke
  fCoilLengthY     =  17.0*cm;    // Thickness of coil along Y
  fCoilLengthZ     =  fYokeSizeZ; // Length along Z of straight sections of coil inside the yoke

  fSlabThick   =  2.0*cm;
  fSlabWidth   = 30.0*cm;
  fSlabLengthZ = fYokeSizeZ;

  //fRailThick   = 0.6*cm;
  //fRailWidth   = 3.0*cm;
  //fRailLengthZ = fYokeSizeZ;

  fNeopreneThick   =  0.4*cm;
  fNeopreneWidth   = 23.5*cm;
  fNeopreneLengthZ = fYokeSizeZ;

  //fVacuumThick = 0.8*cm; // Should be asked to the ChamberGeometry class
  //fVacuumWidth = fYokeSizeInBarX-2.*fRailWidth;
  //fVacuumWidth = fYokeSizeInBarX;
  //fVacuumLengthZ = fYokeSizeZ;

  //fMagneticVolumeLengthX = fYokeSizeInnerX;
  ////fMagneticVolumeLengthY = fYokeSizeInnerY-2.*fYokeSizeInBarY-2.*fRailThick;
  ////fMagneticVolumeLengthY = fYokeSizeInnerY-2.*fYokeSizeInBarY-2.*fVacuumThick;
  //fMagneticVolumeLengthY = fYokeSizeInnerY-2.*fYokeSizeInBarY;
  //fMagneticVolumeLengthZ = fYokeSizeZ+100.*cm; // Magnetic field extends outside magnet yoke

  fYokeFrontFacePosZ = -0.5*fYokeSizeZ; // Center of magnet yoke = global reference point

  //fMagneticFieldConstantValue = -0.55*tesla;

  fMagnetSensitiveDetectorName = "MagnetSD";

}

MagnetGeometry::~MagnetGeometry()
{}
