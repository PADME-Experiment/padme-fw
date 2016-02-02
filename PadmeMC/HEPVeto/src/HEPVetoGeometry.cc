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

  fFingerNominalSizeX =  1.*cm;
  fFingerNominalSizeY = 18.*cm;
  fFingerNominalSizeZ =  1.*cm;

  fHEPVetoNFingers = 50;

  fFingerGap = 0.1*mm;

  // Position of center of HEPVeto relative to center of magnet yoke
  fHEPVetoCenterPosX = 120.*cm;
  fHEPVetoCenterPosY =   0.*cm;
  fHEPVetoCenterPosZ = 250.*cm;

  fHEPVetoRotX = 0.   *rad;
  fHEPVetoRotY = 0.977*rad; // ~56deg (i.e. 90deg-34deg, see PADME drawings)
  fHEPVetoRotZ = 0.   *rad;

  fHEPVetoSensitiveDetectorName = "HEPVetoSD";

}

HEPVetoGeometry::~HEPVetoGeometry()
{}

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
