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
  fTPixNCols = 5;

  fChipNominalSizeX =   0.10*mm; // Thickness
  fChipNominalSizeY =  14.08*mm; // 256 pixels of 55um each
  fChipNominalSizeZ =  14.08*mm; // 256 pixels of 55um each

  fChipGap = 0.01*mm;

  // Position of center of TPix relative to center of magnet yoke
  fTPixCenterPosX =  -92.*cm;
  fTPixCenterPosY =   0.*cm;
  fTPixCenterPosZ = 256.*cm;

  fTPixRotX =  0.   *rad;
  fTPixRotY = -0.977*rad; // ~-56deg (i.e. -90deg+34deg, see PADME drawings)
  fTPixRotZ =  0.   *rad;

  fTPixSensitiveDetectorName = "TPixSD";

}

TPixGeometry::~TPixGeometry()
{}

G4double TPixGeometry::GetChipPosX(G4int row, G4int col)
{
  // Verify we are within TPix box
  if ( row<0 || row>=fTPixNRows || col<0 || col>=fTPixNCols ) {
    printf("TPixGeometry::GetChipPosX - ERROR - Requested finger at row %d col %d\n",row,col);
    return 0.*cm;
  }
  return 0.*cm;
}

  G4double TPixGeometry::GetChipPosY(G4int row,G4int col)
{
  // Verify we are within TPix box
  if ( row<0 || row>=fTPixNRows || col<0 || col>=fTPixNCols ) {
    printf("TPixGeometry::GetChipPosY - ERROR - Requested finger at row %d col %d\n",row,col);
    return 0.*cm;
  }
  return -0.5*GetTPixSizeY()+(0.5+row)*(fChipNominalSizeY+fChipGap);
}

  G4double TPixGeometry::GetChipPosZ(G4int row,G4int col)
{
  // Verify we are within TPix box
  if ( row<0 || row>=fTPixNRows || col<0 || col>=fTPixNCols ) {
    printf("TPixGeometry::GetChipPosZ - ERROR - Requested finger at row %d col %d\n",row,col);
    return 0.*cm;
  }
  return -0.5*GetTPixSizeZ()+(0.5+col)*(fChipNominalSizeZ+fChipGap);
}
