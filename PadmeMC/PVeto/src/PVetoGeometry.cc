// PVetoGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#include "PVetoGeometry.hh"

PVetoGeometry* PVetoGeometry::fInstance = 0;

PVetoGeometry* PVetoGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new PVetoGeometry(); }
  return fInstance;
}

PVetoGeometry::PVetoGeometry()
{

  // Inizialize default parameters

  fFingerGap = 0.3*mm;

  //fPVetoNFingers = 100;
  fPVetoNFingers = 96;

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

  fPVetoInnerFacePosX =  20.*cm;

  fPVetoFrontFacePosZ = -50.*cm; // Start at front face of yoke

  fPVetoSensitiveDetectorName = "PVetoSD";

}

PVetoGeometry::~PVetoGeometry()
{}

G4double PVetoGeometry::GetFingerPosX(G4int idx)
{
  // Verify we are within PVeto box
  if (idx<0 || idx>=fPVetoNFingers) {
    printf("PVetoGeometry::GetFingerPosX - ERROR - Requested finger at index %d\n",idx);
    return 0.*cm;
  }
  return 0.*cm;
}

G4double PVetoGeometry::GetFingerPosY(G4int idx)
{
  // Verify we are within PVeto box
  if (idx<0 || idx>=fPVetoNFingers) {
    printf("PVetoGeometry::GetFingerPosY - ERROR - Requested finger at index %d\n",idx);
    return 0.*cm;
  }
  return 0.*cm;
}

G4double PVetoGeometry::GetFingerPosZ(G4int idx)
{
  // Verify we are within PVeto box
  if (idx<0 || idx>=fPVetoNFingers) {
    printf("PVetoGeometry::GetFingerPosZ - ERROR - Requested finger at index %d\n",idx);
    return 0.*cm;
  }
  return (fFingerNominalSizeZ+fFingerGap)*(-0.5*fPVetoNFingers+1.*idx+0.5);
}
