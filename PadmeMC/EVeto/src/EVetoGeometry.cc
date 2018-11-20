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

  fFingerDist0 = 10.309*mm ; // Need drawings!
  fFingerPitch = 10.309*mm; // Need drawings!

  fEVetoNFingers = 96;

  fFingerSizeX =  1.0*cm;
  fFingerSizeY = 17.8*cm;  //raw correction M. Raggi need Drawings
  fFingerSizeZ =  1.0*cm;

  fGrooveSizeX = 1.3*mm;
  fGrooveSizeZ = 1.3*mm;

  fFingerRotY = 10.*deg; // Need drawings!

  fSupportSizeX =   1.5*cm; // Need drawings!
  fSupportSizeY =   0.5*cm; // Need drawings!
  fSupportSizeZ = 100.0*cm; // Need drawings!

  fEVetoInnerFacePosX = -20.*cm;

  fEVetoFrontFacePosZ = -483.55*mm; // Start 6.45mm from inner face of vacuum chamber (final position to be decided)

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
  return -0.5*fSupportSizeZ+fFingerDist0+idx*fFingerPitch;
}

std::vector<G4String> EVetoGeometry::GetHashTable()
{

  std::vector<G4String> hash;
  std::ostringstream buffer;

  buffer << "fFingerDist0 " << fFingerDist0;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fFingerPitch " << fFingerPitch;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fEVetoNFingers " << fEVetoNFingers;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fFingerSizeX " << fFingerSizeX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fFingerSizeY " << fFingerSizeY;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fFingerSizeZ " << fFingerSizeZ;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fFingerRotY " << fFingerRotY;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fSupportSizeX " << fSupportSizeX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fSupportSizeY " << fSupportSizeY;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fSupportSizeZ " << fSupportSizeZ;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fEVetoInnerFacePosX " << fEVetoInnerFacePosX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fEVetoFrontFacePosZ " << fEVetoFrontFacePosZ;
  hash.push_back(buffer.str());
  buffer.str("");

  return hash;
}
