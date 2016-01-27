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

  fFingerNominalSizeX =  1.0*cm;
  fFingerNominalSizeY = 18.0*cm;
  fFingerNominalSizeZ =  1.0*cm;

  fPVetoNFingers = 100;

  fFingerGap = 0.1*mm;

  //fPVetoFrontFacePosZ = -50.*cm;
  fPVetoInnerFacePosX = 22.*cm; // Along X fingers are positioned just inside magnet rail

  fPVetoSensitiveDetectorName = "PVetoSD";

}

PVetoGeometry::~PVetoGeometry()
{}

G4double PVetoGeometry::GetPVetoPosX()
{
  return fPVetoInnerFacePosX+GetPVetoSizeX()*0.5;
}

G4double PVetoGeometry::GetPVetoPosY()
{
  return 0.;
}

G4double PVetoGeometry::GetPVetoPosZ()
{
  return 0.; // Centered within MagneticVolume
}

G4double PVetoGeometry::GetPVetoSizeX()
{
  return fFingerNominalSizeX;
}

G4double PVetoGeometry::GetPVetoSizeY()
{
  return fFingerNominalSizeY;
}

G4double PVetoGeometry::GetPVetoSizeZ()
{
  return fFingerNominalSizeZ*fPVetoNFingers;
}

G4double PVetoGeometry::GetFingerSizeX()
{
  return fFingerNominalSizeX-fFingerGap;
}

G4double PVetoGeometry::GetFingerSizeY()
{
  return fFingerNominalSizeY-fFingerGap;
}

G4double PVetoGeometry::GetFingerSizeZ()
{
  return fFingerNominalSizeZ-fFingerGap;
}

G4double PVetoGeometry::GetFingerPosX(G4int idx)
{
  // Verify we are within PVeto box
  if (idx<0 || idx>=fPVetoNFingers) {
    printf("PVetoGeometry::GetFingerPosX - ERROR - Requested finger at index %d\n",idx);
    return 0.;
  }
  return 0.;
}

G4double PVetoGeometry::GetFingerPosY(G4int idx)
{
  // Verify we are within PVeto box
  if (idx<0 || idx>=fPVetoNFingers) {
    printf("PVetoGeometry::GetFingerPosY - ERROR - Requested finger at index %d\n",idx);
    return 0.;
  }
  return 0.;
}

G4double PVetoGeometry::GetFingerPosZ(G4int idx)
{
  // Verify we are within PVeto box
  if (idx<0 || idx>=fPVetoNFingers) {
    printf("PVetoGeometry::GetFingerPosZ - ERROR - Requested finger at index %d\n",idx);
    return 0.;
  }
  return -GetPVetoSizeZ()*0.5+fFingerNominalSizeZ*idx+fFingerNominalSizeZ*0.5;
}
