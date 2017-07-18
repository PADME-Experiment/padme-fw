// HEPVetoDetector.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#include "HEPVetoDetector.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Box.hh"
#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"

#include "HEPVetoGeometry.hh"
#include "HEPVetoSD.hh"

HEPVetoDetector::HEPVetoDetector(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{

  // Connect to HEPVetoMessenger to enable datacard configuration
  fHEPVetoMessenger = new HEPVetoMessenger(this);

}

HEPVetoDetector::~HEPVetoDetector()
{
  delete fHEPVetoMessenger;
}

void HEPVetoDetector::CreateGeometry()
{

  HEPVetoGeometry* geo = HEPVetoGeometry::GetInstance();

  // Create main HEPVeto box
  G4double hepVetoSizeX = geo->GetHEPVetoSizeX();
  G4double hepVetoSizeY = geo->GetHEPVetoSizeY();
  G4double hepVetoSizeZ = geo->GetHEPVetoSizeZ();
  printf("HEPVeto size is %f %f %f\n",hepVetoSizeX,hepVetoSizeY,hepVetoSizeZ);
  G4Box* solidHEPVeto = new G4Box("HEPVetoSolid",0.5*hepVetoSizeX,0.5*hepVetoSizeY,0.5*hepVetoSizeZ);
  fHEPVetoVolume = new G4LogicalVolume(solidHEPVeto,G4Material::GetMaterial("Vacuum"),"HEPVetoLogic",0,0,0);
  //fHEPVetoVolume->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));
  fHEPVetoVolume->SetVisAttributes(G4VisAttributes::Invisible);

  G4double hepVetoPosX = geo->GetHEPVetoPosX();
  G4double hepVetoPosY = geo->GetHEPVetoPosY();
  G4double hepVetoPosZ = geo->GetHEPVetoPosZ();
  printf("HEPVeto will be placed at %f %f %f\n",hepVetoPosX,hepVetoPosY,hepVetoPosZ);
  G4ThreeVector posHEPVeto = G4ThreeVector(hepVetoPosX,hepVetoPosY,hepVetoPosZ);
  G4RotationMatrix* rotHEPVeto = new G4RotationMatrix;
  rotHEPVeto->rotateY(geo->GetHEPVetoRotY());
  new G4PVPlacement(rotHEPVeto,posHEPVeto,fHEPVetoVolume,"HEPVeto",fMotherVolume,false,0,false);

  // Create standard scintillator finger
  G4double hepVetoFingerX = geo->GetFingerSizeX();
  G4double hepVetoFingerY = geo->GetFingerSizeY();
  G4double hepVetoFingerZ = geo->GetFingerSizeZ();
  printf("HEPVeto Finger size is %f %f %f\n",hepVetoFingerX,hepVetoFingerY,hepVetoFingerZ);
  G4Box* solidFinger  = new G4Box("HEPVetoFingerSolid",0.5*hepVetoFingerX,0.5*hepVetoFingerY,0.5*hepVetoFingerZ);
  fFingerVolume  = new G4LogicalVolume(solidFinger,G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"HEPVetoFingerLogic",0,0,0);
  fFingerVolume->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));

  // Make finger a sensitive detector
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String hepVetoSDName = geo->GetHEPVetoSensitiveDetectorName();
  printf("Registering HEPVeto SD %s\n",hepVetoSDName.data());
  HEPVetoSD* hepVetoSD = new HEPVetoSD(hepVetoSDName);
  sdMan->AddNewDetector(hepVetoSD);
  fFingerVolume->SetSensitiveDetector(hepVetoSD);

  // Get number of fingers and position them
  G4int nFingers = geo->GetHEPVetoNFingers();
  for (G4int fin=0;fin<nFingers;fin++){
    G4ThreeVector posFinger = G4ThreeVector(geo->GetFingerPosX(fin),geo->GetFingerPosY(fin),geo->GetFingerPosZ(fin));
    new G4PVPlacement(0,posFinger,fFingerVolume,"HEPVetoFinger",fHEPVetoVolume,false,fin,false);
  }

}
