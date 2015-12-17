// EVetoDetector.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#include "EVetoDetector.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Box.hh"
#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"

#include "EVetoGeometry.hh"
#include "EVetoSD.hh"

EVetoDetector::EVetoDetector(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{

  // Connect to EVetoMessenger to enable datacard configuration
  fEVetoMessenger = new EVetoMessenger(this);

}

EVetoDetector::~EVetoDetector()
{
  delete fEVetoMessenger;
}

void EVetoDetector::CreateGeometry()
{

  EVetoGeometry* geo = EVetoGeometry::GetInstance();

  // Create main EVeto box
  printf("EVeto will be placed at %f %f %f\n",geo->GetEVetoPosX(),geo->GetEVetoPosY(),geo->GetEVetoPosZ());
  G4ThreeVector posEVeto = G4ThreeVector(geo->GetEVetoPosX(),geo->GetEVetoPosY(),geo->GetEVetoPosZ()); 
  G4double eVetoX      = geo->GetEVetoSizeX();
  G4double eVetoY      = geo->GetEVetoSizeY();
  G4double eVetoLength = geo->GetEVetoSizeZ();
  printf("EVeto size is %f %f %f\n",eVetoX,eVetoY,eVetoLength);
  G4Box* solidEVeto = new G4Box("EVetoSolid",eVetoX*0.5,eVetoY*0.5,eVetoLength*0.5);
  fEVetoVolume = new G4LogicalVolume(solidEVeto,G4Material::GetMaterial("G4_Galactic"),"EVetoLogic",0,0,0);
  fEVetoVolume->SetVisAttributes(G4VisAttributes::Invisible);
  new G4PVPlacement(0,posEVeto,fEVetoVolume,"EVeto",fMotherVolume,false,0,false);

  // Create standard scintillator finger
  G4double eVetoFingerX      = geo->GetFingerSizeX();
  G4double eVetoFingerY      = geo->GetFingerSizeY();
  G4double eVetoFingerLength = geo->GetFingerSizeZ();
  printf("EVeto Finger size is %f %f %f\n",eVetoFingerX,eVetoFingerY,eVetoFingerLength);
  G4Box* solidFinger  = new G4Box("EVetoCrySolid",eVetoFingerX*0.5,eVetoFingerY*0.5,eVetoFingerLength*0.5);
  fFingerVolume  = new G4LogicalVolume(solidFinger,G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"EVetoFingerLogic",0,0,0);

  // Make crystal a sensitive detector
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String eVetoSDName = geo->GetEVetoSensitiveDetectorName();
  printf("Registering EVeto SD %s\n",eVetoSDName.data());
  EVetoSD* eVetoSD = new EVetoSD(eVetoSDName);
  sdMan->AddNewDetector(eVetoSD);
  fFingerVolume->SetSensitiveDetector(eVetoSD);

  // Get number of fingers and position them
  G4int nFingers = geo->GetEVetoNFingers();
  for (G4int fin=0;fin<nFingers;fin++){
    G4ThreeVector posFinger = G4ThreeVector(geo->GetFingerPosX(fin),geo->GetFingerPosY(fin),geo->GetFingerPosZ(fin));
    new G4PVPlacement(0,posFinger,fFingerVolume,"EVetoFinger",fEVetoVolume,false,fin,false);
  }

}
