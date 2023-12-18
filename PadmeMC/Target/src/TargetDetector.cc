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
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"

#include "G4SDManager.hh"
#include "G4DigiManager.hh"

#include "G4Element.hh"
#include "G4Material.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "TargetGeometry.hh"
#include "TargetSD.hh"
#include "TargetDigitizer.hh"

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
  //  fTargetVolume = new G4LogicalVolume(solidTarget,G4Material::GetMaterial("Vacuum"),"Target",0,0,0);
  fTargetVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  new G4PVPlacement(0,targetPos-G4ThreeVector(0.,0.,fTargetDisplacePosZ),fTargetVolume,"Target",fMotherVolume,false,0,true);

  // Create Target support structure
  G4double tsL1 = geo->GetTSupportL1();
  G4double tsL2 = geo->GetTSupportL2();
  G4double tsL3 = geo->GetTSupportL3();
  G4double tsL4 = geo->GetTSupportL4();
  G4double tsThick = geo->GetTSupportThick();
  G4Box* solidTS1 = new G4Box("TS1",0.5*tsL2,0.5*tsL1,0.5*tsThick);
  G4Box* solidTS2 = new G4Box("TS2",0.5*tsL3,0.5*tsL4,0.5*tsThick);
  G4ThreeVector posTS2TS1 = G4ThreeVector(-0.5*(tsL2+tsL3),0.,0.);
  G4UnionSolid* solidTS3 = new G4UnionSolid("TS3",solidTS1,solidTS2,0,posTS2TS1);

  G4double tsHL = geo->GetTSupportHoleL();
  G4double tsHD = geo->GetTSupportHoleD();
  G4Box* solidTSH = new G4Box("TSH",0.5*tsHL,0.5*tsHL,0.5*tsThick+1.*um);
  G4ThreeVector posTSHTS3 = G4ThreeVector(0.5*tsL2-tsHD,0.,0.);
  G4SubtractionSolid* solidTargetSupport = new G4SubtractionSolid("TargetSupport",solidTS3,solidTSH,0,posTSHTS3);

  G4LogicalVolume* logicTargetSupport = new G4LogicalVolume(solidTargetSupport,G4Material::GetMaterial("PCB"),"TargetSupport",0,0,0);
  logicTargetSupport->SetVisAttributes(G4VisAttributes(G4Colour::Green()));
  G4ThreeVector tsPosition = G4ThreeVector(geo->GetTSupportPosX(),geo->GetTSupportPosY(),geo->GetTSupportPosZ());
  new G4PVPlacement(0,tsPosition-G4ThreeVector(0.,0.,fTargetDisplacePosZ),logicTargetSupport,"TargetSupport",fMotherVolume,false,0,true);

  // Create digitizer for Target
  G4DigiManager* theDM = G4DigiManager::GetDMpointer();
  G4String targetDName = geo->GetTargetDigitizerName();
  printf("Registering Target Digitizer %s\n",targetDName.data());
  TargetDigitizer* targetD = new TargetDigitizer(targetDName);
  theDM->AddNewModule(targetD);

  // The whole target is a sensitive detector
  // Digitization will take care of mapping hits to readout strips
  G4String targetSDName = geo->GetTargetSensitiveDetectorName();
  printf("Registering Target Sensitive Detector %s\n",targetSDName.data());
  TargetSD* targetSD = new TargetSD(targetSDName);
  fTargetVolume->SetSensitiveDetector(targetSD);
  G4SDManager::GetSDMpointer()->AddNewDetector(targetSD);

}
