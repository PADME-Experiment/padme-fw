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

  fMagnetSizeZ      = 100.0*cm;
  fMagnetSizeOuterX = 125.0*cm;
  fMagnetSizeOuterY = 174.0*cm;
  fMagnetSizeInnerX =  63.0*cm;
  fMagnetSizeInnerY = 112.0*cm;
  fMagnetSizeInBarX =  20.0*cm;
  fMagnetSizeInBarY =  52.0*cm;

  fCoilInnerRadius =   5.5*cm;
  fCoilOuterRadius =  29.0*cm;
  fCoilLengthX     =  17.0*cm;
  fCoilLengthY     =  46.0*cm;
  fCoilLengthZ     =  fMagnetSizeZ;

  fSlabThick   =  2.0*cm;
  fSlabWidth   = 30.0*cm;
  fSlabLengthZ = fMagnetSizeZ;

  fRailThick   = 0.6*cm;
  fRailWidth   = 3.0*cm;
  fRailLengthZ = fMagnetSizeZ;

  fNeopreneThick   =  0.4*cm;
  fNeopreneWidth   = 23.5*cm;
  fNeopreneLengthZ = fMagnetSizeZ;

  fMagneticVolumeLengthX = 20.*cm;
  fMagneticVolumeLengthY = 52.*cm;
  fMagneticVolumeLengthZ = fMagnetSizeZ+100.*cm; // Magnetic field extends outside magnet yoke

  fMagnetFrontFacePosZ = 20.*cm;

  fMagneticFieldConstantValue = 0.45*tesla;

  fMagnetSensitiveDetectorName = "MagnetSD";

}

MagnetGeometry::~MagnetGeometry()
{}
