// EVetoGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#include "EVetoGeometry.hh"

EVetoGeometry* EVetoGeometry::fInstance = 0;

EVetoGeometry* EVetoGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new EVetoGeometry(); }
  return fInstance;
}

EVetoGeometry::EVetoGeometry()
{

  // Inizialize default parameters

  fFingerGap = 0.3*mm;

  //fEVetoNFingers = 100;
  fEVetoNFingers = 96;

  fFingerNominalSizeX =  1.0*cm;
  fFingerNominalSizeY = 16.0*cm;
  fFingerNominalSizeZ =  1.0*cm;

  //fFingerRotX = 0.*deg;
  fFingerRotY = -10.*deg;
  //fFingerRotZ = 0.*deg;

  fSupportGap = 0.1*mm;

  fSupportNominalSizeX =   2.0*cm;
  fSupportNominalSizeY =   0.5*cm;
  fSupportNominalSizeZ = 100.0*cm;

  fEVetoInnerFacePosX = 20.*cm;

  fEVetoFrontFacePosZ = -50.*cm; // Start at front face of yoke

  fEVetoSensitiveDetectorName = "EVetoSD";

}

EVetoGeometry::~EVetoGeometry()
{}

G4double EVetoGeometry::GetFingerPosX(G4int idx)
{
  // Verify we are within EVeto box
  if (idx<0 || idx>=fEVetoNFingers) {
    printf("EVetoGeometry::GetFingerPosX - ERROR - Requested finger at index %d\n",idx);
    return 0.*cm;
  }
  return 0.*cm;
}

G4double EVetoGeometry::GetFingerPosY(G4int idx)
{
  // Verify we are within EVeto box
  if (idx<0 || idx>=fEVetoNFingers) {
    printf("EVetoGeometry::GetFingerPosY - ERROR - Requested finger at index %d\n",idx);
    return 0.*cm;
  }
  return 0.*cm;
}

G4double EVetoGeometry::GetFingerPosZ(G4int idx)
{
  // Verify we are within EVeto box
  if (idx<0 || idx>=fEVetoNFingers) {
    printf("EVetoGeometry::GetFingerPosZ - ERROR - Requested finger at index %d\n",idx);
    return 0.*cm;
  }
  return (fFingerNominalSizeZ+fFingerGap)*(-0.5*fEVetoNFingers+1.*idx+0.5);
}
