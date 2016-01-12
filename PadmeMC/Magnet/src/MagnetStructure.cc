// MagnetStructure.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-01-11
// --------------------------------------------------------------

#include "MagnetStructure.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"

#include "MagnetGeometry.hh"
#include "MagnetSD.hh"

MagnetStructure::MagnetStructure(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{

  // Connect to MagnetMessenger to enable datacard configuration
  //fMagnetMessenger = new MagnetMessenger(this);

}

MagnetStructure::~MagnetStructure()
{
  //delete fMagnetMessenger;
}

void MagnetStructure::CreateGeometry()
{

  MagnetGeometry* geo = MagnetGeometry::GetInstance();

  // Small gap between magnet yoke's sections
  G4double magGap = geo->GetMagnetGap();

  // Create and position upper part of magnet yoke
  G4double magUpSizeX = geo->GetMagnetUpSizeX();
  G4double magUpSizeY = geo->GetMagnetUpSizeY();
  G4double magUpSizeZ = geo->GetMagnetUpSizeZ();
  G4Box* magUpSolid = new G4Box("MagnetUp",0.5*(magUpSizeX-magGap),0.5*(magUpSizeY-magGap),0.5*(magUpSizeZ-magGap));
  G4LogicalVolume* magUpVolume = new G4LogicalVolume(magUpSolid,G4Material::GetMaterial("G4_Fe"),"MagnetUp",0,0,0);
  G4ThreeVector magUpPos = G4ThreeVector(geo->GetMagnetUpPosX(),geo->GetMagnetUpPosY(),geo->GetMagnetUpPosZ());
  new G4PVPlacement(0,magUpPos,magUpVolume,"MagnetUp",fMotherVolume,false,0,false);

  // Create and position lower part of magnet yoke
  G4double magDownSizeX = geo->GetMagnetDownSizeX();
  G4double magDownSizeY = geo->GetMagnetDownSizeY();
  G4double magDownSizeZ = geo->GetMagnetDownSizeZ();
  G4Box* magDownSolid = new G4Box("MagnetDown",0.5*(magDownSizeX-magGap),0.5*(magDownSizeY-magGap),0.5*(magDownSizeZ-magGap));
  G4LogicalVolume* magDownVolume = new G4LogicalVolume(magDownSolid,G4Material::GetMaterial("G4_Fe"),"MagnetDown",0,0,0);
  G4ThreeVector magDownPos = G4ThreeVector(geo->GetMagnetDownPosX(),geo->GetMagnetDownPosY(),geo->GetMagnetDownPosZ());
  new G4PVPlacement(0,magDownPos,magDownVolume,"MagnetDown",fMotherVolume,false,0,false);

  // Create and position right part of magnet yoke
  G4double magRightSizeX = geo->GetMagnetRightSizeX();
  G4double magRightSizeY = geo->GetMagnetRightSizeY();
  G4double magRightSizeZ = geo->GetMagnetRightSizeZ();
  G4Box* magRightSolid = new G4Box("MagnetRight",0.5*(magRightSizeX-magGap),0.5*(magRightSizeY-magGap),0.5*(magRightSizeZ-magGap));
  G4LogicalVolume* magRightVolume = new G4LogicalVolume(magRightSolid,G4Material::GetMaterial("G4_Fe"),"MagnetRight",0,0,0);
  G4ThreeVector magRightPos = G4ThreeVector(geo->GetMagnetRightPosX(),geo->GetMagnetRightPosY(),geo->GetMagnetRightPosZ());
  new G4PVPlacement(0,magRightPos,magRightVolume,"MagnetRight",fMotherVolume,false,0,false);

  // Create and position left part of magnet yoke
  G4double magLeftSizeX = geo->GetMagnetLeftSizeX();
  G4double magLeftSizeY = geo->GetMagnetLeftSizeY();
  G4double magLeftSizeZ = geo->GetMagnetLeftSizeZ();
  G4Box* magLeftSolid = new G4Box("MagnetLeft",0.5*(magLeftSizeX-magGap),0.5*(magLeftSizeY-magGap),0.5*(magLeftSizeZ-magGap));
  G4LogicalVolume* magLeftVolume = new G4LogicalVolume(magLeftSolid,G4Material::GetMaterial("G4_Fe"),"MagnetLeft",0,0,0);
  G4ThreeVector magLeftPos = G4ThreeVector(geo->GetMagnetLeftPosX(),geo->GetMagnetLeftPosY(),geo->GetMagnetLeftPosZ());
  new G4PVPlacement(0,magLeftPos,magLeftVolume,"MagnetLeft",fMotherVolume,false,0,false);

  // Create coil solid and logical volume
  G4double coilInnR = geo->GetCoilInnerRadius();
  G4double coilOutR = geo->GetCoilOuterRadius();
  G4double coilLen = geo->GetCoilLength();
  G4double coilSpanStart = geo->GetCoilSpanStart();
  G4double coilSpan = geo->GetCoilSpan();
  G4Tubs* coilSolid = new G4Tubs("Coil",coilInnR,coilOutR,coilLen,coilSpanStart,coilSpan);
  G4LogicalVolume* coilVolume = new G4LogicalVolume(coilSolid,G4Material::GetMaterial("G4_Cu"),"Coil",0,0,0);

  // Position front/up coil
  //G4RotationMatrix* coilFURot = new G4RotationMatrix(geo->GetCoilFrontUpRotX(),geo->GetCoilFrontUpRotY(),geo->GetCoilFrontUpRotZ());
  G4RotationMatrix* coilFURot = new G4RotationMatrix();
  //coilFURot->rotateX(geo->GetCoilFrontUpRotX()); // Not needed
  coilFURot->rotateY(geo->GetCoilFrontUpRotY());
  coilFURot->rotateZ(geo->GetCoilFrontUpRotZ());
  G4ThreeVector coilFUPos = G4ThreeVector(geo->GetCoilFrontUpPosX(),geo->GetCoilFrontUpPosY(),geo->GetCoilFrontUpPosZ());
  new G4PVPlacement(coilFURot,coilFUPos,coilVolume,"CoilFU",fMotherVolume,false,0,false);

  // Position front/down coil
  //G4RotationMatrix* coilFDRot = new G4RotationMatrix(geo->GetCoilFrontDownRotX(),geo->GetCoilFrontDownRotY(),geo->GetCoilFrontDownRotZ());
  G4RotationMatrix* coilFDRot = new G4RotationMatrix();
  //coilFDRot->rotateX(geo->GetCoilFrontDownRotX()); // Not needed
  coilFDRot->rotateY(geo->GetCoilFrontDownRotY());
  coilFDRot->rotateZ(geo->GetCoilFrontDownRotZ());
  G4ThreeVector coilFDPos = G4ThreeVector(geo->GetCoilFrontDownPosX(),geo->GetCoilFrontDownPosY(),geo->GetCoilFrontDownPosZ());
  new G4PVPlacement(coilFDRot,coilFDPos,coilVolume,"CoilFD",fMotherVolume,false,0,false);

  // Position back/up coil
  //G4RotationMatrix* coilBURot = new G4RotationMatrix(geo->GetCoilBackUpRotX(),geo->GetCoilBackUpRotY(),geo->GetCoilBackUpRotZ());
  G4RotationMatrix* coilBURot = new G4RotationMatrix();
  //coilBURot->rotateX(geo->GetCoilBackUpRotX()); // Not needed
  coilBURot->rotateY(geo->GetCoilBackUpRotY());
  coilBURot->rotateZ(geo->GetCoilBackUpRotZ());
  G4ThreeVector coilBUPos = G4ThreeVector(geo->GetCoilBackUpPosX(),geo->GetCoilBackUpPosY(),geo->GetCoilBackUpPosZ());
  new G4PVPlacement(coilBURot,coilBUPos,coilVolume,"CoilBU",fMotherVolume,false,0,false);

  // Position back/down coil
  //G4RotationMatrix* coilBDRot = new G4RotationMatrix(geo->GetCoilBackDownRotX(),geo->GetCoilBackDownRotY(),geo->GetCoilBackDownRotZ());
  G4RotationMatrix* coilBDRot = new G4RotationMatrix();
  //coilBDRot->rotateX(geo->GetCoilBackDownRotX()); // Not needed
  coilBDRot->rotateY(geo->GetCoilBackDownRotY());
  coilBDRot->rotateZ(geo->GetCoilBackDownRotZ());
  G4ThreeVector coilBDPos = G4ThreeVector(geo->GetCoilBackDownPosX(),geo->GetCoilBackDownPosY(),geo->GetCoilBackDownPosZ());
  new G4PVPlacement(coilBDRot,coilBDPos,coilVolume,"CoilBD",fMotherVolume,false,0,false);

  // Make upper and lower sections of magnet yoke a sensitive detector
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String magnetSDName = geo->GetMagnetSensitiveDetectorName();
  printf("Registering Magnet SD %s\n",magnetSDName.data());
  MagnetSD* magnetSD = new MagnetSD(magnetSDName);
  sdMan->AddNewDetector(magnetSD);
  magUpVolume->SetSensitiveDetector(magnetSD);
  magDownVolume->SetSensitiveDetector(magnetSD);

}
