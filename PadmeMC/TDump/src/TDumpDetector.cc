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
//#include "TDumpSD.hh"

TDumpDetector::TDumpDetector(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{

  // Connect to TDumpMessenger to enable datacard configuration
  //fTDumpMessenger = new TDumpMessenger(this);

}

TDumpDetector::~TDumpDetector()
{
  //delete fTDumpMessenger;
}

void TDumpDetector::CreateGeometry()
{

  TDumpGeometry* geo = TDumpGeometry::GetInstance();

  // Create main TDump tube
  printf("TDump will be placed at %f %f %f\n",geo->GetTDumpPosX(),geo->GetTDumpPosY(),geo->GetTDumpPosZ());
  G4ThreeVector tdumpPos = G4ThreeVector(geo->GetTDumpPosX(),geo->GetTDumpPosY(),geo->GetTDumpPosZ()); 
  G4double tdumpInnR  = geo->GetTDumpInnerRadius();
  G4double tdumpOutR  = geo->GetTDumpOuterRadius();
  G4double tdumpSizeZ = geo->GetTDumpSizeZ();
  printf("TDump size is %f %f %f\n",tdumpInnR,tdumpOutR,tdumpSizeZ);
  G4Tubs* solidTDump = new G4Tubs("TDump",tdumpInnR,tdumpOutR,0.5*tdumpSizeZ,0.*deg,360.*deg);
  fTDumpVolume = new G4LogicalVolume(solidTDump,G4Material::GetMaterial("G4_W"),"TDump",0,0,0);
  fTDumpVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Blue()));
  new G4PVPlacement(0,tdumpPos,fTDumpVolume,"TDump",fMotherVolume,false,0,false);

  // Create lead brick
  G4double brickSizeX = geo->GetBrickSizeX();
  G4double brickSizeY = geo->GetBrickSizeY();
  G4double brickSizeZ = geo->GetBrickSizeZ();
  printf("TDump brick size is %f %f %f\n",brickSizeX,brickSizeY,brickSizeZ);
  G4Box* solidBrick = new G4Box("Brick",0.5*brickSizeX,0.5*brickSizeY,0.5*brickSizeZ);
  G4LogicalVolume* logicBrick = new G4LogicalVolume(solidBrick,G4Material::GetMaterial("G4_Pb"),"Brick",0,0,0);
  logicBrick->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Grey()));

  // Position all lead bricks
  G4int nBricks = geo->GetNBricks();
  for (G4int b=0; b<nBricks; b++) {
    printf("Brick %d at %f %f %f rot %f\n",b,geo->GetBrickPosX(b),geo->GetBrickPosY(b),geo->GetBrickPosZ(b),geo->GetBrickRotZ(b));
    G4ThreeVector posBrick = G4ThreeVector(geo->GetBrickPosX(b),geo->GetBrickPosY(b),geo->GetBrickPosZ(b));
    G4RotationMatrix* rotBrick = new G4RotationMatrix;
    rotBrick->rotateZ(geo->GetBrickRotZ(b));
    new G4PVPlacement(rotBrick,posBrick,logicBrick,"Brick",fMotherVolume,false,b,false);
  }

}
