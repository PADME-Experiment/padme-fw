// ECalDetector.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#include "ECalDetector.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Box.hh"

#include "G4SDManager.hh"

#include "G4Material.hh"

#include "ECalGeometryParameters.hh"
#include "ECalSD.hh"

ECalDetector::ECalDetector(G4LogicalVolume* motherVolume)
{

  fMotherVolume = motherVolume;

  // Connect to ECalDetectorMessenger to enable datacard configuration
  fECalMessenger = new ECalMessenger(this);

}

ECalDetector::~ECalDetector()
{
  delete fECalMessenger;
}

void ECalDetector::CreateGeometry()
{

  ECalGeometryParameters* geo = ECalGeometryParameters::GetInstance();

  // Create main ECal box
  G4ThreeVector positionEcal = G4ThreeVector(geo->GetECalPosX(),geo->GetECalPosY(),geo->GetECalPosZ()); 
  G4double ECalX      = geo->GetECalSizeX();
  G4double ECalY      = geo->GetECalSizeY();
  G4double ECalLength = geo->GetECalSizeZ();

  G4Box* solidEcal = new G4Box("ECalSolid",ECalX*0.5,ECalY*0.5,ECalLength*0.5);
  fECalVolume = new G4LogicalVolume(solidEcal,G4Material::GetMaterial("G4_Galactic"),"ECalLogic",0, 0, 0);
  new G4PVPlacement(0,positionEcal,fECalVolume,"ECal",fMotherVolume,false,0,false);

  // Create standard BGO crystal
  G4double ECryX      = geo->GetCrystalSizeX();
  G4double ECryY      = geo->GetCrystalSizeY();
  G4double ECryLength = geo->GetCrystalSizeZ();
  G4Box* solidCry  = new G4Box("Ecry",ECryX*0.5,ECryY*0.5,ECryLength*0.5);
  fCrystalVolume  = new G4LogicalVolume(solidCry,G4Material::GetMaterial("G4_BGO"),"ECry",0, 0, 0);

  // Make crystal a sensitive detector
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String ecalSDName = geo->GetECalSensitiveDetectorName();
  ECalSD* ecalSD = (ECalSD*)sdMan->FindSensitiveDetector(ecalSDName);
  if (!ecalSD) {
     ecalSD = new ECalSD(ecalSDName);
     sdMan->AddNewDetector(ecalSD);
  }
  fCrystalVolume->SetSensitiveDetector(ecalSD);

  // Get number of rows and columns of crystals and position all crystals
  G4int nRow = geo->GetECalNRows();
  G4int nCol = geo->GetECalNCols();
  for (G4int row=0;row<nRow;row++){
     for (G4int col=0;col<nCol;col++){
       if (geo->ExistsCrystalAt(row,col)) {
	 G4int idxCry = col*nRow+row;
	 G4ThreeVector positionCry = G4ThreeVector(geo->GetCrystalPosX(row,col),geo->GetCrystalPosY(row,col),geo->GetCrystalPosZ(row,col));
	 new G4PVPlacement(0,positionCry,fCrystalVolume,"ECry",fECalVolume,false,idxCry,false);
       }
     }
  }

}
