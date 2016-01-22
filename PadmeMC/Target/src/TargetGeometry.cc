// TargetGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#include "TargetGeometry.hh"

TargetGeometry* TargetGeometry::fInstance = 0;

TargetGeometry* TargetGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new TargetGeometry(); }
  return fInstance;
}

TargetGeometry::TargetGeometry()
{

  // Inizialize default parameters

  fTargetSizeX =   2.*cm;
  fTargetSizeY =   2.*cm;
  fTargetSizeZ = 100.*um;

  //fTargetFrontFacePosZ = -20.*cm;
  fTargetFrontFacePosZ = -50.*cm; // Relative to center of magnet

  fTargetSensitiveDetectorName = "TargetSD";

}

TargetGeometry::~TargetGeometry()
{}
