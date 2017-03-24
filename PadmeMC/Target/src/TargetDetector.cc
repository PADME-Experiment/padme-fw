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

#include "G4Element.hh"
#include "G4Material.hh"

#include "TargetGeometry.hh"
#include "TargetSD.hh"

TargetDetector::TargetDetector(G4LogicalVolume* motherVolume):fMotherVolume(motherVolume)
{

  // Connect to TargetDetectorMessenger to enable datacard configuration
  fTargetMessenger = new TargetMessenger(this);

  fTargetDisplacePosZ = 0.; // No default displacement

}

TargetDetector::~TargetDetector()
{
  delete fTargetMessenger;
}

void TargetDetector::CreateGeometry()
{

  TargetGeometry* geo = TargetGeometry::GetInstance();

  // Get target position and dimensions

  G4ThreeVector targetPos = G4ThreeVector(geo->GetTargetPosX(),geo->GetTargetPosY(),geo->GetTargetPosZ());
  printf("Target will be placed at %f %f %f\n",targetPos.x(),targetPos.y(),targetPos.z());

  G4double targetSizeX = geo->GetTargetSizeX();
  G4double targetSizeY = geo->GetTargetSizeY();
  G4double targetSizeZ = geo->GetTargetSizeZ();
  printf("Target dimensions are %f %f %f\n",targetSizeX,targetSizeY,targetSizeZ);

  // Create main Target box and position it
  // Do not forget to take into account mother volume Z displacement
  G4Box* solidTarget = new G4Box("Target",targetSizeX*0.5,targetSizeY*0.5,targetSizeZ*0.5);
  fTargetVolume = new G4LogicalVolume(solidTarget,G4Material::GetMaterial("Diamond"),"Target",0,0,0);
  new G4PVPlacement(0,targetPos-G4ThreeVector(0.,0.,fTargetDisplacePosZ),fTargetVolume,"Target",fMotherVolume,false,0,false);

  // The whole target is a sensitive detector
  // Digitization will take care of mapping hits to readout strips
  G4String targetSDName = geo->GetTargetSensitiveDetectorName();
  printf("Registering Target SD %s\n",targetSDName.data());
  TargetSD* targetSD = new TargetSD(targetSDName);
  fTargetVolume->SetSensitiveDetector(targetSD);
  G4SDManager::GetSDMpointer()->AddNewDetector(targetSD);
  /*
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String targetSDName = geo->GetTargetSensitiveDetectorName();
  printf("Registering Target SD %s\n",targetSDName.data());
  TargetSD* targetSD = new TargetSD(targetSDName);
  sdMan->AddNewDetector(targetSD);
  fTargetVolume->SetSensitiveDetector(targetSD);
  */

}
