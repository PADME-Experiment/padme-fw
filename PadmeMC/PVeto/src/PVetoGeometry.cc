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

  fVerbose = 0; // Do not show debug output

  // Inizialize default parameters
  //  fFingerDist0 = 10.309*mm ; // Need drawings!
  //  fFingerPitch = 10.309*mm;  // Need drawings!
  fFingerPitch = 11.*mm; // Need drawings! Raggi 11/10/2018 construction drawings
  fFingerDist0 = 26.*mm; // Need drawings! Raggi 11/10/2018 construction drawings
  fPVetoNFingers = 90;   // changed M. Raggi 7/3/2019 was 96 6 have been dismounted

  fFingerSizeX =  1.0*cm;
  fFingerSizeY = 17.8*cm;
  fFingerSizeZ =  1.0*cm;

  fGrooveSizeX = 1.3*mm;
  fGrooveSizeZ = 1.3*mm;

  fFingerRotY = -10.*deg; // Need drawings!

  //fSupportSizeX =   1.5*cm; // Need drawings!
  fSupportSizeX =   32.0*mm; // EL 2019/05/21 Rough measurement on technical drawings from Sofia
  fSupportSizeY =   0.5*cm; // Need drawings!
  // fSupportSizeZ = 100.0*cm; // Need drawings!
  //fSupportSizeZ = 110.0*cm; // Need drawings!  Raggi 11/10/2018 construction drawings
  fSupportSizeZ = 109.5*cm; // EL 2019/05/21 Technical drawings from Sofia

  // Distance along X from front face of support to center of finger
  fFingerCenterPosX = 15.0*mm; // EL 2019/05/21 Rough measurement on technical drawings from Sofia

  //  fPVetoInnerFacePosX =  20.*cm;
  fPVetoInnerFacePosX =  17.75*cm;  //M. Raggi from drawings 18/10/18

  //  fPVetoFrontFacePosZ = -483.55*mm; // Start 6.45mm from inner face of vacuum chamber (final position to be decided)
  fPVetoFrontFacePosZ = -472.55*mm; // Start 56.45mm from inner face of vacuum chamber (wrong but real position M. Raggi)

  fPVetoDigitizerName = "PVetoDigitizer";
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
  //return 0.*cm;
  return -0.5*fSupportSizeX+fFingerCenterPosX;
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
  return -0.5*fSupportSizeZ+fFingerDist0+idx*fFingerPitch;
}

std::vector<G4String> PVetoGeometry::GetHashTable()
{

  std::vector<G4String> hash;
  std::ostringstream buffer;

  buffer << "fFingerDist0 " << fFingerDist0;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fFingerPitch " << fFingerPitch;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fPVetoNFingers " << fPVetoNFingers;
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

  buffer << "fPVetoInnerFacePosX " << fPVetoInnerFacePosX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fPVetoFrontFacePosZ " << fPVetoFrontFacePosZ;
  hash.push_back(buffer.str());
  buffer.str("");

  return hash;
}
