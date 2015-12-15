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

  fTargetFrontFacePosZ = -20.*cm;

  fTargetSensitiveDetectorName = "TargetSD";

}

TargetGeometry::~TargetGeometry()
{}

G4double TargetGeometry::GetTargetPosX() { return 0.; }

G4double TargetGeometry::GetTargetPosY() { return 0.; }

G4double TargetGeometry::GetTargetPosZ() { return fTargetFrontFacePosZ+fTargetSizeZ*0.5; }
