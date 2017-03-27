// TPixGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-04
// --------------------------------------------------------------

#include "TPixGeometry.hh"

TPixGeometry* TPixGeometry::fInstance = 0;

TPixGeometry* TPixGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new TPixGeometry(); }
  return fInstance;
}

TPixGeometry::TPixGeometry()
{

  // Inizialize default parameters

  fTPixNRows = 2;
  fTPixNCols = 8;

  fChipSizeX =  14.08*mm; // 256 pixels of 55um each
  fChipSizeY =  14.08*mm; // 256 pixels of 55um each
  fChipSizeZ =   0.10*mm; // Thickness

  fChipStep = 14.10*mm; // Step between TPix chips

  // Distance from the corner on the back wall of the vacuum chamber and
  // the projection of the center of the external side of the TPix on the
  // external surface of the diagonal wall of the vacuum chamber.
  // Tune this to move TPix along the diagonal wall of the vacuum chamber
  fTPixDistanceToCorner = 30.*cm;

  // Thickness of the support structure between HEPVeto and diagonal wall of the vacuum chamber
  fTPixSupportThickness = 1.*cm; // Check with final design

  // Angle of vacuum chamber wall behind HEPVeto wrt X axis
  // This value will be modified by main program according to actual chamber measures
  fTPixChamberWallAngle = 0.32962*rad;

  // Coordinates of the corner on the back face of the vacuum chamber
  // These values will be modified by main program according to actual chamber measures
  fTPixChamberWallCorner = G4ThreeVector(-422.77*mm,0.,2249.*mm);

  fTPixSensitiveDetectorName = "TPixSD";

  UpdateDerivedMeasures();

}

TPixGeometry::~TPixGeometry()
{}

void TPixGeometry::UpdateDerivedMeasures()
{

  // Size of TPix box
  fTPixSizeX = fChipStep*fTPixNCols;
  fTPixSizeY = fChipStep*fTPixNRows;
  fTPixSizeZ = fChipSizeZ+1.*um; // Small tolerance to avoid surface overlaps

  // Angle of the rotation of TPix around the Y axis
  fTPixRotY = fTPixChamberWallAngle;

  // Position of center of TPix box
  fTPixPosX = fTPixChamberWallCorner.x()-fTPixDistanceToCorner*cos(fTPixChamberWallAngle)
    -(fTPixSupportThickness+0.5*fTPixSizeZ)*sin(fTPixChamberWallAngle)
    -0.5*fTPixSizeX*cos(fTPixChamberWallAngle);
  fTPixPosY = 0.;
  fTPixPosZ = fTPixChamberWallCorner.z()-fTPixDistanceToCorner*sin(fTPixChamberWallAngle)
    +(fTPixSupportThickness+0.5*fTPixSizeZ)*cos(fTPixChamberWallAngle)
    -0.5*fTPixSizeX*sin(fTPixChamberWallAngle);

}

G4double TPixGeometry::GetChipPosX(G4int row, G4int col)
{
  // Verify we are within TPix box
  if ( row<0 || row>=fTPixNRows || col<0 || col>=fTPixNCols ) {
    printf("TPixGeometry::GetChipPosX - ERROR - Requested finger at row %d col %d\n",row,col);
    return 0.*cm;
  }
  return -0.5*fTPixSizeX+(0.5+col)*fChipStep;
}

  G4double TPixGeometry::GetChipPosY(G4int row,G4int col)
{
  // Verify we are within TPix box
  if ( row<0 || row>=fTPixNRows || col<0 || col>=fTPixNCols ) {
    printf("TPixGeometry::GetChipPosY - ERROR - Requested finger at row %d col %d\n",row,col);
    return 0.*cm;
  }
  return -0.5*fTPixSizeY+(0.5+row)*fChipStep;
}

  G4double TPixGeometry::GetChipPosZ(G4int row,G4int col)
{
  // Verify we are within TPix box
  if ( row<0 || row>=fTPixNRows || col<0 || col>=fTPixNCols ) {
    printf("TPixGeometry::GetChipPosZ - ERROR - Requested finger at row %d col %d\n",row,col);
    return 0.*cm;
  }
  return 0.;
}

std::vector<G4String> TPixGeometry::GetHashTable()
{

  std::vector<G4String> hash;
  std::ostringstream buffer;

  buffer << "fTPixNRows " << fTPixNRows;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTPixNCols " << fTPixNCols;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fChipSizeX " << fChipSizeX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fChipSizeY " << fChipSizeX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fChipSizeZ " << fChipSizeX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTPixSizeX " << fTPixSizeX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTPixSizeY " << fTPixSizeY;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTPixSizeZ " << fTPixSizeZ;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTPixRotY " << fTPixRotY;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTPixPosX " << fTPixPosX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTPixPosY " << fTPixPosY;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTPixPosZ " << fTPixPosZ;
  hash.push_back(buffer.str());
  buffer.str("");

  return hash;
}
