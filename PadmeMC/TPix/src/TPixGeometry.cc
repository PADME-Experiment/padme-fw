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

  fVerbose = 0; // Do not show debug output

  fTPixNRows = 2;
  fTPixNCols = 6;

  fChipSizeX =  14.08*mm; // 256 pixels of 55um each
  fChipSizeY =  14.08*mm; // 256 pixels of 55um each
  fChipSizeZ =   0.30*mm; // Thickness M. Raggi after E. Spiriti Mail 29/03/2019

  fChipStep = 14.10*mm; // Step between TPix chips

  // Distance from the corner on the back wall of the vacuum chamber and
  // the projection of the center of the external side of the TPix on the
  // external surface of the diagonal wall of the vacuum chamber.
  // Six positions are possible given the TimePix support mounting screw holes (see Cesidio's drawings)
  // Position of center of beam exit hole (400mm) is used as reference.
  //fTPixDistanceToCorner = 400.0*mm- 42.68*mm;
  //fTPixDistanceToCorner = 400.0*mm-115.70*mm;
  //fTPixDistanceToCorner = 400.0*mm- 85.68*mm;
  fTPixDistanceToCorner = 400.0*mm-102.70*mm; // Selected by looking at photos: need check
  //fTPixDistanceToCorner = 400.0*mm- 55.70*mm;
  //fTPixDistanceToCorner = 400.0*mm-145.68*mm;

  // Distance between back surface of the diagonal wall of the vacuum chamber and front surface of the TimePix
  fTPixSupportThickness = 62.5*mm; // 1.5mm(extra)+35mm(steel cap)+16mm(Al flange)+10mm(TimePix box thickness)

  // Angle of vacuum chamber wall behind HEPVeto wrt X axis
  // This value will be modified by main program according to actual chamber measures
  fTPixChamberWallAngle = 0.;

  // Coordinates of the corner on the back face of the vacuum chamber
  // These values will be modified by main program according to actual chamber measures
  fTPixChamberWallCorner = G4ThreeVector(0.,0.,0.);

  fTPixDigitizerName = "TPixDigitizer";
  fTPixSensitiveDetectorName = "TPixSD";

  UpdateDerivedMeasures();

}

TPixGeometry::~TPixGeometry()
{}

void TPixGeometry::SetTPixNCols(G4int c)
{
  printf("TPixGeometry - Setting NCols to %d\n",c);
  fTPixNCols = c;
  UpdateDerivedMeasures();
}

void TPixGeometry::SetTPixNRows(G4int r)
{
  printf("TPixGeometry - Setting NRows to %d\n",r);
  fTPixNRows = r;
  UpdateDerivedMeasures();
}

void TPixGeometry::SetTPixChamberWallAngle(G4double a)
{
  printf("TPixGeometry - Setting ChamberWallAngle to %5.1f deg\n",a/deg);
  fTPixChamberWallAngle = a;
  UpdateDerivedMeasures();
}

void TPixGeometry::SetTPixSupportThickness(G4double t)
{
  printf("TPixGeometry - Setting SupportThickness to %4.1f mm\n",t/mm);
  fTPixSupportThickness = t;
  UpdateDerivedMeasures();
}

void TPixGeometry::SetTPixDistanceToCorner(G4double d)
{
  printf("TPixGeometry - Setting DistanceToCorner to %5.1f mm\n",d/mm);
  fTPixDistanceToCorner = d;
  UpdateDerivedMeasures();
}

void TPixGeometry::SetTPixChamberWallCorner(G4ThreeVector c)
{
  printf("TPixGeometry - Setting ChamberWallCorner coordinates to (%.1f,%.1f,%.1f) mm\n",c.x()/mm,c.y()/mm,c.z()/mm);
  fTPixChamberWallCorner = c;
  UpdateDerivedMeasures();
}

void TPixGeometry::UpdateDerivedMeasures()
{

  // Size of TPix box
  fTPixSizeX = fChipStep*fTPixNCols;
  fTPixSizeY = fChipStep*fTPixNRows;
  fTPixSizeZ = fChipSizeZ+1.*um; // Small tolerance to avoid surface overlaps

  // Angle of the rotation of TPix around the Y axis
  fTPixRotY = -fTPixChamberWallAngle;
  
//  //Brutal patch for Run III M. Raggi June 2022
//  bool fIsRunIII=true;
//  if(fIsRunIII){
//    fTPixRotY = 0;
//  }
  // Position of center of TPix box
  //fTPixPosX = fTPixChamberWallCorner.x()-fTPixDistanceToCorner*cos(fTPixChamberWallAngle)
  //  -(fTPixSupportThickness+0.5*fTPixSizeZ)*sin(fTPixChamberWallAngle)
  //  -0.5*fTPixSizeX*cos(fTPixChamberWallAngle);
  fTPixPosX = fTPixChamberWallCorner.x()
    +(fTPixDistanceToCorner+0.5*fTPixSizeX)*cos(fTPixChamberWallAngle)
    +(fTPixSupportThickness+0.5*fTPixSizeZ)*sin(fTPixChamberWallAngle);
  fTPixPosY = 0.;
  fTPixPosZ = fTPixChamberWallCorner.z()
    -(fTPixDistanceToCorner+0.5*fTPixSizeX)*sin(fTPixChamberWallAngle)
    +(fTPixSupportThickness+0.5*fTPixSizeZ)*cos(fTPixChamberWallAngle);

//  //Brutal patch for Run III M. Raggi June 2022
//  if(fIsRunIII){
//    fTPixPosX = 0;
//    fTPixPosY = 0;
//    fTPixPosZ = +3000*mm; //Ex SAC Front Face position 
//  }
  //printf("TPix size %f %f %f\n",fTPixSizeX,fTPixSizeY,fTPixSizeZ);
  //printf("TPix corner %f %f %f\n",fTPixChamberWallCorner.x(),fTPixChamberWallCorner.y(),fTPixChamberWallCorner.z());
  //printf("TPix angle %f\n",fTPixChamberWallAngle);
  //printf("TPix distance %f support %f\n",fTPixDistanceToCorner,fTPixSupportThickness);

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
