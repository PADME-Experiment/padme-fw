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

  fVerbose = 0; // Do not show debug output

  // fFingerDist0 = 10.309*mm ; // Need drawings!
  // fFingerPitch = 10.309*mm; // Need drawings!
  fFingerPitch = 11.*mm; // Raggi 7/03/2019 construction drawings
  fFingerDist0 = 26.*mm; // Raggi 7/03/2019 construction drawings
 

  fEVetoNFingers = 96;

  fFingerSizeX =  1.0*cm;
  fFingerSizeY = 17.8*cm;  //raw correction M. Raggi need Drawings
  fFingerSizeZ =  1.0*cm;

  fGrooveSizeX = 1.3*mm;
  fGrooveSizeZ = 1.3*mm;

  fFingerRotY = 10.*deg; // Need drawings!

  //fSupportSizeX =   1.5*cm; // Need drawings!
  fSupportSizeX =   32.0*mm; // EL 2019/05/21 Rough measurement on technical drawings from Sofia
  fSupportSizeY =   0.5*cm; // Need drawings!
  //  fSupportSizeZ = 100.0*cm; // Need drawings!
  //fSupportSizeZ = 110.0*cm; // Raggi 07/03/2019 construction drawings
  fSupportSizeZ = 109.5*cm; // EL 2019/05/21 Technical drawings from Sofia

  // Distance along X from front face of support to center of finger
  fFingerCenterPosX = 15.0*mm; // EL 2019/05/21 Rough measurement on technical drawings from Sofia

  // fEVetoInnerFacePosX = -20.*cm;
  fEVetoInnerFacePosX = -17.75*cm;  //M. Raggi from drawings 07/03/2019

  //  fEVetoFrontFacePosZ = -483.55*mm; // Start 6.45mm from inner face of vacuum chamber (final position to be decided)
  fEVetoFrontFacePosZ = -472.55*mm; // Start 17.45mm from inner face of vacuum chamber (wrong but real position M. Raggi 07/03/2019)
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
  //return 0.*cm;
  return 0.5*fSupportSizeX-fFingerCenterPosX;
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
