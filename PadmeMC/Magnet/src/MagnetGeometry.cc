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

  fMagnetSizeInnerX =  20.*cm;
  fMagnetSizeInnerY = 116.*cm;
  fMagnetSizeOuterX = 174.*cm;
  fMagnetSizeOuterY = 200.*cm;
  fMagnetSizeZ      = 100.*cm;

  fCoilInnerRadius =  15.*cm;
  fCoilOuterRadius =  35.*cm;
  fCoilLength      =  10.*cm;
  fCoilSpanStart   =   0.*deg;
  fCoilSpan        = 180.*deg;

  fMagnetFrontFacePosZ = 20.*cm;

  fMagnetSensitiveDetectorName = "MagnetSD";

}

MagnetGeometry::~MagnetGeometry()
{}

