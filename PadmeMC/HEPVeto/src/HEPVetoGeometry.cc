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
  // Size of scintillator fingers
  fFingerSizeX =  1.*cm;
  fFingerSizeY = 17.8*cm;  // raw correction M. Raggi needs Drawing
  fFingerSizeZ =  1.*cm;

  //Groove size
  fGrooveSizeX = 1.3*mm;
  fGrooveSizeZ = 1.3*mm;

  fFingerRotY = -15.*deg; // Checked with drawings M. Raggi 24/06/2018

  // Number of fingers in HEPVeto
  fHEPVetoNFingers = 16;

  // Step between HEPVeto fingers
  fFingerStep = 11.*mm;

  // Distance from the corner on the back wall of the vacuum chamber and
  // the projection of the center of the external face of the first finger
  // of HEPVeto on the external surface of the diagonal wall of the vacuum chamber.
  // Tune this to move HEPVeto along the diagonal wall of the vacuum chamber
  fHEPVetoDistanceToCorner = 43.*cm;

  // Thickness of the support structure between HEPVeto and diagonal wall of the vacuum chamber
  fHEPVetoSupportThickness = 1.*cm; // Check with final design

  // Angle of vacuum chamber wall behind HEPVeto wrt X axis
  // This value will be modified by main program according to actual chamber measures
  fHEPVetoChamberWallAngle = 0.;    //raw rotation angle M. Raggi

  // Thickness of the vacuum chamber wall behind HEPVeto
  // This value will be modified by main program according to actual chamber measures
  fHEPVetoChamberWallThickness = 0.;

  // Coordinates of the corner on the back face of the vacuum chamber
  // These values will be modified by main program according to actual chamber measures
  fHEPVetoChamberWallCorner = G4ThreeVector(0.,0.,0.);

  fHEPVetoSensitiveDetectorName = "HEPVetoSD";

  UpdateDerivedMeasures();

}

HEPVetoGeometry::~HEPVetoGeometry()
{}

void HEPVetoGeometry::UpdateDerivedMeasures()
{

  // Size of HEPVeto box
  fHEPVetoSizeX = fFingerSizeX+1.*cm; // This takes into account the pedestal to hold the fingers
  fHEPVetoSizeY = fFingerSizeY+1.*cm; // This takes into account the pedestal to hold the fingers
  fHEPVetoSizeZ = fFingerStep*fHEPVetoNFingers;

  // Angle of the rotation of HEPVeto around the Y axis
  fHEPVetoRotY = 90.*deg-fHEPVetoChamberWallAngle;

  // Position of center of HEPVeto box
  fHEPVetoPosX = fHEPVetoChamberWallCorner.x()
    +(fHEPVetoDistanceToCorner+0.5*fHEPVetoSizeZ)*cos(fHEPVetoChamberWallAngle)
    -(fHEPVetoChamberWallThickness+fHEPVetoSupportThickness+0.5*fHEPVetoSizeX)*sin(fHEPVetoChamberWallAngle);
  fHEPVetoPosY = 0.;
  fHEPVetoPosZ = fHEPVetoChamberWallCorner.z()
    -(fHEPVetoDistanceToCorner+0.5*fHEPVetoSizeZ)*sin(fHEPVetoChamberWallAngle)
    -(fHEPVetoChamberWallThickness+fHEPVetoSupportThickness+0.5*fHEPVetoSizeX)*cos(fHEPVetoChamberWallAngle);

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
  return 0.5*fHEPVetoSizeZ-(idx+0.5)*fFingerStep;
}

std::vector<G4String> HEPVetoGeometry::GetHashTable()
{

  std::vector<G4String> hash;
  std::ostringstream buffer;

  buffer << "fFingerStep " << fFingerStep;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fHEPVetoNFingers " << fHEPVetoNFingers;
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

  buffer << "fHEPVetoSizeX " << fHEPVetoSizeX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fHEPVetoSizeY " << fHEPVetoSizeX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fHEPVetoSizeZ " << fHEPVetoSizeX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fHEPVetoRotY " << fHEPVetoRotY;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fHEPVetoPosX " << fHEPVetoPosX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fHEPVetoPosY " << fHEPVetoPosY;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fHEPVetoPosZ " << fHEPVetoPosZ;
  hash.push_back(buffer.str());
  buffer.str("");

  return hash;
}
