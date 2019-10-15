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

  fVerbose = 0; // Do not show debug output

  // HEPVeto box includes all the fingers, the T-shaped bars with the square holes for the fingers
  // and the vertical support structure
  fHEPVetoSizeX = 215.0*mm; // Need info from Cesidio
  // Two possible sizes of the vertical support structure: which one is used?
  //fHEPVetoSizeY = 198.0*mm; // Long fingers
  fHEPVetoSizeY = 190.0*mm; // Short fingers
  fHEPVetoSizeZ =  37.3*mm; // Need info from Cesidio

  // Position along X of center of first finger wrt border of T-shaped support
  fFirstFingerDistX = 25.0*mm; // Need info from Cesidio

  // Position along Z of center of fingers wrt border of vertical support
  fFingersDistZ = 21.9*mm; // Need info from Cesidio

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

  // Distance between back face of the vertical support and the internal wall of the vacuum chamber
  fHEPVetoDistanceToChamberWall = 0.; // Need info from Cesidio

  // Distance from the corner on the back wall of the vacuum chamber to the border of
  // the T-shaped support bars
  // Tune this to move HEPVeto along the diagonal wall of the vacuum chamber
  // Position of center of beam exit hole (400mm) is used as reference.
  fHEPVetoDistanceToCorner = 400.0*mm;

  // Thickness of the support structure between HEPVeto and diagonal wall of the vacuum chamber
  //fHEPVetoSupportThickness = 1.*cm; // Check with final design

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

  // Angle of the rotation of HEPVeto around the Y axis
  fHEPVetoRotY = -fHEPVetoChamberWallAngle;

  // Position of center of HEPVeto box
  fHEPVetoPosX = fHEPVetoChamberWallCorner.x()
    +(fHEPVetoDistanceToCorner+0.5*fHEPVetoSizeX)*cos(fHEPVetoChamberWallAngle)
    -(fHEPVetoChamberWallThickness+fHEPVetoDistanceToChamberWall+0.5*fHEPVetoSizeZ)*sin(fHEPVetoChamberWallAngle);
  fHEPVetoPosY = 0.;
  fHEPVetoPosZ = fHEPVetoChamberWallCorner.z()
    -(fHEPVetoDistanceToCorner+0.5*fHEPVetoSizeX)*sin(fHEPVetoChamberWallAngle)
    -(fHEPVetoChamberWallThickness+fHEPVetoDistanceToChamberWall+0.5*fHEPVetoSizeZ)*cos(fHEPVetoChamberWallAngle);

}

G4double HEPVetoGeometry::GetFingerPosX(G4int idx)
{
  // Verify we are within HEPVeto box
  if (idx<0 || idx>=fHEPVetoNFingers) {
    printf("HEPVetoGeometry::GetFingerPosX - ERROR - Requested finger at index %d\n",idx);
    return 0.;
  }
  //return 0;
  return -0.5*fHEPVetoSizeX+fFirstFingerDistX+idx*fFingerStep;
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
  //return 0.5*fHEPVetoSizeZ-(idx+0.5)*fFingerStep;
  return 0.5*fHEPVetoSizeZ-fFingersDistZ;
}

void HEPVetoGeometry::SetHEPVetoChamberWallAngle(G4double a)
{
  printf("HEPVetoGeometry - Setting ChamberWallAngle to %5.1f deg\n",a/deg);
  fHEPVetoChamberWallAngle = a;
  UpdateDerivedMeasures();
}

void HEPVetoGeometry::SetHEPVetoChamberWallThickness(G4double t)
{
  printf("HEPVetoGeometry - Setting ChamberWallThickness to %4.1f mm\n",t/mm);
  fHEPVetoChamberWallThickness = t;
  UpdateDerivedMeasures();
}

void HEPVetoGeometry::SetHEPVetoDistanceToCorner(G4double d)
{
  printf("HEPVetoGeometry - Setting DistanceToCorner to %5.1f mm\n",d/mm);
  fHEPVetoDistanceToCorner = d;
  UpdateDerivedMeasures();
}

void HEPVetoGeometry::SetHEPVetoChamberWallCorner(G4ThreeVector c)
{
  printf("HEPVetoGeometry - Setting ChamberWallCorner coordinates to (%.1f,%.1f,%.1f) mm\n",c.x()/mm,c.y()/mm,c.z()/mm);
  fHEPVetoChamberWallCorner = c;
  UpdateDerivedMeasures();
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
