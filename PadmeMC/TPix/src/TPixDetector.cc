// TPixDetector.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#include "TPixDetector.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Box.hh"
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"

#include "TPixGeometry.hh"
#include "TPixSD.hh"
#include "TPixDigitizer.hh"

TPixDetector::TPixDetector(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{

  // Connect to TPixMessenger to enable datacard configuration
  fTPixMessenger = new TPixMessenger(this);

}

TPixDetector::~TPixDetector()
{
  delete fTPixMessenger;
}

void TPixDetector::CreateGeometry()
{

  TPixGeometry* geo = TPixGeometry::GetInstance();

  // Create main TPix box
  G4double boxSizeX = geo->GetBoxSizeX();
  G4double boxSizeY = geo->GetBoxSizeY();
  G4double boxSizeZ = geo->GetBoxSizeZ();
  printf("TPix Box size is %.2f x %.2f x %.2f mm3\n",boxSizeX/mm,boxSizeY/mm,boxSizeZ/mm);
  G4Box* solidBox = new G4Box("TPixBoxSolid",0.5*boxSizeX,0.5*boxSizeY,0.5*boxSizeZ);
  fTPixBoxVolume = new G4LogicalVolume(solidBox,G4Material::GetMaterial("Air"),"TPixBoxLogic",0,0,0);
  //fTPixVolume->SetVisAttributes(G4VisAttributes::Invisible);
  fTPixBoxVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));

  G4double boxPosX = geo->GetBoxPosX();
  G4double boxPosY = geo->GetBoxPosY();
  G4double boxPosZ = geo->GetBoxPosZ();
  G4ThreeVector posBox = G4ThreeVector(boxPosX,boxPosY,boxPosZ);
  G4double boxRotY = geo->GetTPixRotY();
  G4RotationMatrix* rotBox = new G4RotationMatrix;
  rotBox->rotateY(boxRotY);
  new G4PVPlacement(rotBox,posBox,fTPixBoxVolume,"TPixBox",fMotherVolume,false,0,false);
  printf("TPix Box placed at (%.1f,%.1f,%.1f) mm with a rotation of %.3f rad\n",
	 boxPosX/mm,boxPosY/mm,boxPosZ/mm,boxRotY/rad);

  // Create TPix box for chips
  G4double tpixSizeX = geo->GetTPixSizeX();
  G4double tpixSizeY = geo->GetTPixSizeY();
  G4double tpixSizeZ = geo->GetTPixSizeZ();
  printf("TPix size is %.2f x %.2f x %.2f mm3\n",tpixSizeX/mm,tpixSizeY/mm,tpixSizeZ/mm);
  G4Box* solidTPix = new G4Box("TPixSolid",0.5*tpixSizeX,0.5*tpixSizeY,0.5*tpixSizeZ);
  fTPixVolume = new G4LogicalVolume(solidTPix,G4Material::GetMaterial("Vacuum"),"TPixLogic",0,0,0);
  fTPixVolume->SetVisAttributes(G4VisAttributes::Invisible);
  //fTPixVolume->SetVisAttributes(G4VisAttributes(G4Colour::White()));

  /*
  // Position TPix in MotherVolume
  G4double tpixPosX = geo->GetTPixPosX();
  G4double tpixPosY = geo->GetTPixPosY();
  G4double tpixPosZ = geo->GetTPixPosZ();
  G4ThreeVector posTPix = G4ThreeVector(tpixPosX,tpixPosY,tpixPosZ);
  G4double tpixRotY = geo->GetTPixRotY();
  G4RotationMatrix* rotTPix = new G4RotationMatrix;
  rotTPix->rotateY(tpixRotY);
  new G4PVPlacement(rotTPix,posTPix,fTPixVolume,"TPix",fMotherVolume,false,0,false);
  printf("TPix placed at (%.1f,%.1f,%.1f) mm with a rotation of %.3f rad\n",
	 tpixPosX/mm,tpixPosY/mm,tpixPosZ/mm,tpixRotY/rad);
  */

  // Position TPix in main box
  G4double tpixDispX = geo->GetTPixDispX();
  G4double tpixDispY = geo->GetTPixDispY();
  G4double tpixDispZ = geo->GetTPixDispZ();
  G4ThreeVector dispTPix = G4ThreeVector(tpixDispX,tpixDispY,tpixDispZ);
  new G4PVPlacement(0,dispTPix,fTPixVolume,"TPix",fTPixBoxVolume,false,0,false);
  printf("TPix placed inside main box at (%.1f,%.1f,%.1f) mm\n",
	 tpixDispX/mm,tpixDispY/mm,tpixDispZ/mm);

  // Create standard TimePix chip
  G4double tpixChipX = geo->GetChipSizeX();
  G4double tpixChipY = geo->GetChipSizeY();
  G4double tpixChipZ = geo->GetChipSizeZ();
  printf("TPix Chip size is %.2f x %.2f x %.2f mm3\n",tpixChipX/mm,tpixChipY/mm,tpixChipZ/mm);
  G4Box* solidChip  = new G4Box("TPixChipSolid",0.5*tpixChipX,0.5*tpixChipY,0.5*tpixChipZ);
  fTPixChipVolume  = new G4LogicalVolume(solidChip,G4Material::GetMaterial("G4_Si"),"TPixChipLogic",0,0,0);
  fTPixChipVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));

  // Get number of fingers and position them
  for (G4int row=0;row<geo->GetTPixNRows();row++){
    for (G4int col=0;col<geo->GetTPixNCols();col++){
      G4int idx = 10*row+col;
      G4double x = geo->GetChipPosX(row,col);
      G4double y = geo->GetChipPosY(row,col);
      G4double z = geo->GetChipPosZ(row,col);
      printf("TPix chip %02d will be placed at (%.1f,%.1f,%.1f) mm\n",idx,x/mm,y/mm,z/mm);
      G4ThreeVector posChip = G4ThreeVector(x,y,z);
      new G4PVPlacement(0,posChip,fTPixChipVolume,"TPixChip",fTPixVolume,false,idx,false);
    }
  }

  // Create digitizer for TPix
  G4DigiManager* theDM = G4DigiManager::GetDMpointer();
  G4String tpixDName = geo->GetTPixDigitizerName();
  printf("Registering TPix Digitizer %s\n",tpixDName.data());
  TPixDigitizer* tpixD = new TPixDigitizer(tpixDName);
  theDM->AddNewModule(tpixD);

  // Make chip a sensitive detector
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String tpixSDName = geo->GetTPixSensitiveDetectorName();
  printf("Registering TPix Sensitive Detector %s\n",tpixSDName.data());
  TPixSD* tpixSD = new TPixSD(tpixSDName);
  sdMan->AddNewDetector(tpixSD);
  fTPixChipVolume->SetSensitiveDetector(tpixSD);

}
/*
void TPixDetector::SetTPixChamberWallAngle(G4double a)
{
  TPixGeometry::GetInstance()->SetTPixChamberWallAngle(a);
}

void TPixDetector::SetTPixChamberWallCorner(G4ThreeVector c)
{
  TPixGeometry::GetInstance()->SetTPixChamberWallCorner(c);
}
*/
