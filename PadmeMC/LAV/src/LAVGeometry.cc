// LAVGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#include "LAVGeometry.hh"

LAVGeometry* LAVGeometry::fInstance = 0;

LAVGeometry* LAVGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new LAVGeometry(); }
  return fInstance;
}

LAVGeometry::LAVGeometry()
{

  // Inizialize default parameters

  fLAVInnerRadius = 4.*cm;
  fLAVOuterRadius = 10.*cm;
  fLAVZLength = 20.*cm;

  fLAVFrontFacePosZ = -10.*cm;

  fLAVSensitiveDetectorName = "LAVSD";

}

LAVGeometry::~LAVGeometry()
{}
