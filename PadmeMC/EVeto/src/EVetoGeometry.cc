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

  fFingerNominalSizeX = 19.8*cm;
  fFingerNominalSizeY =  1.0*cm;
  fFingerNominalSizeZ =  1.0*cm;

  fEVetoNFingers = 100;

  fFingerGap = 0.1*mm;

  //fEVetoFrontFacePosZ = -50.*cm;
  fEVetoInnerFacePosY = 20.*cm;

  fEVetoSensitiveDetectorName = "EVetoSD";

}

EVetoGeometry::~EVetoGeometry()
{}

G4double EVetoGeometry::GetEVetoPosX()
{
  return 0.;
}

G4double EVetoGeometry::GetEVetoPosY()
{
  return fEVetoInnerFacePosY+GetEVetoSizeY()*0.5;
}

G4double EVetoGeometry::GetEVetoPosZ()
{
  return 0.;
  //return fEVetoFrontFacePosZ+GetEVetoSizeZ()*0.5;
}

G4double EVetoGeometry::GetEVetoSizeX()
{
  return fFingerNominalSizeX;
}

G4double EVetoGeometry::GetEVetoSizeY()
{
  return fFingerNominalSizeY;
}

G4double EVetoGeometry::GetEVetoSizeZ()
{
  return fFingerNominalSizeZ*fEVetoNFingers;
}

G4double EVetoGeometry::GetFingerSizeX()
{
  return fFingerNominalSizeX-fFingerGap;
}

G4double EVetoGeometry::GetFingerSizeY()
{
  return fFingerNominalSizeY-fFingerGap;
}

G4double EVetoGeometry::GetFingerSizeZ()
{
  return fFingerNominalSizeZ-fFingerGap;
}

G4double EVetoGeometry::GetFingerPosX(G4int idx)
{
  // Verify we are within EVeto box
  if (idx<0 || idx>=fEVetoNFingers) {
    printf("EVetoGeometry::GetFingerPosX - ERROR - Requested finger at index %d\n",idx);
    return 0.;
  }
  return 0;
}

G4double EVetoGeometry::GetFingerPosY(G4int idx)
{
  // Verify we are within EVeto box
  if (idx<0 || idx>=fEVetoNFingers) {
    printf("EVetoGeometry::GetFingerPosY - ERROR - Requested finger at index %d\n",idx);
    return 0.;
  }
  return 0;
}

G4double EVetoGeometry::GetFingerPosZ(G4int idx)
{
  // Verify we are within EVeto box
  if (idx<0 || idx>=fEVetoNFingers) {
    printf("EVetoGeometry::GetFingerPosZ - ERROR - Requested finger at index %d\n",idx);
    return 0.;
  }
  return -GetEVetoSizeZ()*0.5+fFingerNominalSizeZ*idx+fFingerNominalSizeZ*0.5;
}
