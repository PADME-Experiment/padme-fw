// TDumpDetector.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-01-28
// --------------------------------------------------------------

#include "TDumpDetector.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"

#include "TDumpGeometry.hh"
#include "TDumpSD.hh"

TDumpDetector::TDumpDetector(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{

  // Connect to TDumpMessenger to enable datacard configuration
  fTDumpMessenger = new TDumpMessenger(this);

}

TDumpDetector::~TDumpDetector()
{
  //delete fTDumpMessenger;
}

void TDumpDetector::CreateGeometry()
{

  TDumpGeometry* geo = TDumpGeometry::GetInstance();

  // Create main TDump box
  G4double tdumpSizeX = geo->GetTDumpSizeX();
  G4double tdumpSizeY = geo->GetTDumpSizeY();
  G4double tdumpSizeZ = geo->GetTDumpSizeZ();
  printf("TDump size is x=%f y=%f z=%f cm\n",tdumpSizeX/cm,tdumpSizeY/cm,tdumpSizeZ/cm);
  G4Box* solidTDump = new G4Box("TDump",0.5*tdumpSizeX,0.5*tdumpSizeY,0.5*tdumpSizeZ);
  fTDumpVolume = new G4LogicalVolume(solidTDump,G4Material::GetMaterial("Vacuum"),"TDump",0,0,0);
  fTDumpVolume->SetVisAttributes(G4VisAttributes::Invisible);
  //fTDumpVolume->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
  printf("TDump will be placed at x=%f y=%f z=%f cm\n",geo->GetTDumpPosX()/cm,geo->GetTDumpPosY()/cm,geo->GetTDumpPosZ()/cm);
  G4ThreeVector tdumpPos = G4ThreeVector(geo->GetTDumpPosX(),geo->GetTDumpPosY(),geo->GetTDumpPosZ()); 
  new G4PVPlacement(0,tdumpPos,fTDumpVolume,"TDump",fMotherVolume,false,0,false);

  // Create TDump target
  G4double targInnR  = geo->GetTargetInnerRadius();
  G4double targOutR  = geo->GetTargetOuterRadius();
  G4double targSizeZ = geo->GetTargetSizeZ();
  printf("TDump target size is ri=%f ro=%f z=%f cm\n",targInnR/cm,targOutR/cm,targSizeZ/cm);
  G4Tubs* solidTarg = new G4Tubs("TDumpTarget",targInnR,targOutR,0.5*targSizeZ,0.*deg,360.*deg);
  fTargetVolume = new G4LogicalVolume(solidTarg,G4Material::GetMaterial("G4_W"),"TDumpTarget",0,0,0);
  fTargetVolume->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
  printf("TDump target will be placed at x=%f y=%f z=%f cm\n",geo->GetTargetPosX()/cm,geo->GetTargetPosY()/cm,geo->GetTargetPosZ()/cm);
  G4ThreeVector targPos = G4ThreeVector(geo->GetTargetPosX(),geo->GetTargetPosY(),geo->GetTargetPosZ()); 
  new G4PVPlacement(0,targPos,fTargetVolume,"TDumpTarget",fTDumpVolume,false,0,false);

  // Create lead brick
  G4double brickSizeX = geo->GetBrickSizeX();
  G4double brickSizeY = geo->GetBrickSizeY();
  G4double brickSizeZ = geo->GetBrickSizeZ();
  printf("TDump brick size is x=%f y=%f z=%f cm\n",brickSizeX/cm,brickSizeY/cm,brickSizeZ/cm);
  G4Box* solidBrick = new G4Box("Brick",0.5*brickSizeX,0.5*brickSizeY,0.5*brickSizeZ);
  G4LogicalVolume* logicBrick = new G4LogicalVolume(solidBrick,G4Material::GetMaterial("G4_Pb"),"Brick",0,0,0);
  logicBrick->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));

  // Position all lead bricks
  G4int nBricks = geo->GetNBricks();
  for (G4int b=0; b<nBricks; b++) {
    //printf("TDump Brick %d at x=%f y=%f z=%f cm rot=%f rad\n",b,geo->GetBrickPosX(b)/cm,geo->GetBrickPosY(b)/cm,geo->GetBrickPosZ(b)/cm,geo->GetBrickRotZ(b)/rad);
    G4ThreeVector posBrick = G4ThreeVector(geo->GetBrickPosX(b),geo->GetBrickPosY(b),geo->GetBrickPosZ(b));
    G4RotationMatrix* rotBrick = new G4RotationMatrix;
    rotBrick->rotateZ(geo->GetBrickRotZ(b));
    new G4PVPlacement(rotBrick,posBrick,logicBrick,"Brick",fTDumpVolume,false,b,false);
  }

  // Make TDump target a sensitive detector
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String tdumpSDName = geo->GetTDumpSensitiveDetectorName();
  printf("Registering TDump SD %s\n",tdumpSDName.data());
  TDumpSD* tdumpSD = new TDumpSD(tdumpSDName);
  sdMan->AddNewDetector(tdumpSD);
  fTargetVolume->SetSensitiveDetector(tdumpSD);

}
