// TargetDetector.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#include "TargetDetector.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Box.hh"

#include "G4SDManager.hh"

#include "G4Material.hh"

#include "TargetGeometry.hh"
#include "TargetSD.hh"

TargetDetector::TargetDetector(G4LogicalVolume* motherVolume):fMotherVolume(motherVolume)
{
  // Connect to TargetDetectorMessenger to enable datacard configuration
  fTargetMessenger = new TargetMessenger(this);
}

TargetDetector::~TargetDetector()
{
  delete fTargetMessenger;
}

void TargetDetector::CreateGeometry()
{

  TargetGeometry* geo = TargetGeometry::GetInstance();

  // Create main Target box
  printf("Target will be placed at %f %f %f\n",geo->GetTargetPosX(),geo->GetTargetPosY(),geo->GetTargetPosZ());
  G4ThreeVector positionTarget = G4ThreeVector(geo->GetTargetPosX(),geo->GetTargetPosY(),geo->GetTargetPosZ()); 
  G4double targetX = geo->GetTargetSizeX();
  G4double targetY = geo->GetTargetSizeY();
  G4double targetZ = geo->GetTargetSizeZ();

  G4Box* solidTarget = new G4Box("TargetSolid",targetX*0.5,targetY*0.5,targetZ*0.5);
  fTargetVolume = new G4LogicalVolume(solidTarget,G4Material::GetMaterial("G4_C"),"TargetLogic",0,0,0);
  new G4PVPlacement(0,positionTarget,fTargetVolume,"Target",fMotherVolume,false,0,false);

  // The whole target is a sensitive detector
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String targetSDName = geo->GetTargetSensitiveDetectorName();
  printf("Registering Target SD %s\n",targetSDName.data());
  TargetSD* targetSD = new TargetSD(targetSDName);
  sdMan->AddNewDetector(targetSD);
  fTargetVolume->SetSensitiveDetector(targetSD);

}
