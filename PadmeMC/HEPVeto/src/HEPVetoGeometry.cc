// HEPVetoGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#include "HEPVetoGeometry.hh"

HEPVetoGeometry* HEPVetoGeometry::fInstance = 0;

HEPVetoGeometry* HEPVetoGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new HEPVetoGeometry(); }
  return fInstance;
}

HEPVetoGeometry::HEPVetoGeometry()
{

  // Inizialize default parameters

  fFingerNominalSizeX = 20.*cm;
  fFingerNominalSizeY =  1.*cm;
  fFingerNominalSizeZ =  1.*cm;

  fHEPVetoNFingers = 50;

  fFingerGap = 0.1*mm;

  fHEPVetoCenterPosX =   0.*cm;
  fHEPVetoCenterPosY = -75.*cm;
  fHEPVetoCenterPosZ = 223.*cm;

  fHEPVetoRotX = 0.593*rad;
  fHEPVetoRotY =  0.   *rad;
  fHEPVetoRotZ =  0.   *rad;

  fHEPVetoSensitiveDetectorName = "HEPVetoSD";

}

HEPVetoGeometry::~HEPVetoGeometry()
{}

G4double HEPVetoGeometry::GetHEPVetoSizeX()
{
  return fFingerNominalSizeX;
}

G4double HEPVetoGeometry::GetHEPVetoSizeY()
{
  return fFingerNominalSizeY;
}

G4double HEPVetoGeometry::GetHEPVetoSizeZ()
{
  return fFingerNominalSizeZ*fHEPVetoNFingers;
}

G4double HEPVetoGeometry::GetFingerSizeX()
{
  return fFingerNominalSizeX-fFingerGap;
}

G4double HEPVetoGeometry::GetFingerSizeY()
{
  return fFingerNominalSizeY-fFingerGap;
}

G4double HEPVetoGeometry::GetFingerSizeZ()
{
  return fFingerNominalSizeZ-fFingerGap;
}

G4double HEPVetoGeometry::GetFingerPosX(G4int idx)
{
  // Verify we are within HEPVeto box
  if (idx<0 || idx>=fHEPVetoNFingers) {
    printf("HEPVetoGeometry::GetFingerPosX - ERROR - Requested finger at index %d\n",idx);
    return 0.;
  }
  return 0;
}

G4double HEPVetoGeometry::GetFingerPosY(G4int idx)
{
  // Verify we are within HEPVeto box
  if (idx<0 || idx>=fHEPVetoNFingers) {
    printf("HEPVetoGeometry::GetFingerPosY - ERROR - Requested finger at index %d\n",idx);
    return 0.;
  }
  return 0;
}

G4double HEPVetoGeometry::GetFingerPosZ(G4int idx)
{
  // Verify we are within HEPVeto box
  if (idx<0 || idx>=fHEPVetoNFingers) {
    printf("HEPVetoGeometry::GetFingerPosZ - ERROR - Requested finger at index %d\n",idx);
    return 0.;
  }
  return -GetHEPVetoSizeZ()*0.5+fFingerNominalSizeZ*idx+fFingerNominalSizeZ*0.5;
}
