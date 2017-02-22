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
  fFingerNominalSizeY = 20.*cm;
  fFingerNominalSizeZ =  1.*cm;

  fHEPVetoNFingers = 50;

  fFingerGap = 0.1*mm;

  // Size of HEPVeto box
  fHEPVetoSizeX = 2.*cm;
  fHEPVetoSizeY = fFingerNominalSizeY;
  fHEPVetoSizeZ = 60.*cm;

  // Position of center of HEPVeto inner face relative to center of magnet yoke
  fHEPVetoInnerFacePosX = -85.*cm;
  fHEPVetoInnerFacePosY =   0.*cm;
  //fHEPVetoInnerFacePosZ = 250.*cm;
  fHEPVetoInnerFacePosZ = 220.*cm;

  fHEPVetoRotX =  0.   *rad;
  //fHEPVetoRotY = 0.977*rad; // ~56deg (i.e. 90deg-34deg, see PADME drawings)
  fHEPVetoRotY = -0.977*rad; // ~-56deg (i.e. -90deg+34deg, see PADME drawings)
  fHEPVetoRotZ =  0.   *rad;

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
  return GetHEPVetoSizeZ()*0.5-fFingerNominalSizeZ*idx-fFingerNominalSizeZ*0.5;
}

std::vector<G4String> HEPVetoGeometry::GetHashTable()
{

  std::vector<G4String> hash;
  std::ostringstream buffer;

  buffer << "fFingerGap " << fFingerGap;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fHEPVetoNFingers " << fHEPVetoNFingers;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fFingerNominalSizeX " << fFingerNominalSizeX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fFingerNominalSizeY " << fFingerNominalSizeY;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fFingerNominalSizeZ " << fFingerNominalSizeZ;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fHEPVetoSizeX " << fHEPVetoSizeX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fHEPVetoSizeY " << fHEPVetoSizeX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fHEPVetoSizeZ " << fHEPVetoSizeX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fHEPVetoRotX " << fHEPVetoRotX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fHEPVetoRotY " << fHEPVetoRotY;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fHEPVetoRotZ " << fHEPVetoRotZ;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fHEPVetoInnerFacePosX " << fHEPVetoInnerFacePosX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fHEPVetoInnerFacePosY " << fHEPVetoInnerFacePosY;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fHEPVetoInnerFacePosZ " << fHEPVetoInnerFacePosZ;
  hash.push_back(buffer.str());
  buffer.str("");

  return hash;
}
