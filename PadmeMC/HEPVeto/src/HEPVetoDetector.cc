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
  printf("HEPVeto will be placed at %f %f %f\n",geo->GetHEPVetoPosX(),geo->GetHEPVetoPosY(),geo->GetHEPVetoPosZ());
  G4ThreeVector posHEPVeto = G4ThreeVector(geo->GetHEPVetoPosX(),geo->GetHEPVetoPosY(),geo->GetHEPVetoPosZ());
  G4RotationMatrix* rotHEPVeto = new G4RotationMatrix;
  rotHEPVeto->rotateX(geo->GetHEPVetoRotX());
  G4double hepVetoX      = geo->GetHEPVetoSizeX();
  G4double hepVetoY      = geo->GetHEPVetoSizeY();
  G4double hepVetoLength = geo->GetHEPVetoSizeZ();
  printf("HEPVeto size is %f %f %f\n",hepVetoX,hepVetoY,hepVetoLength);
  G4Box* solidHEPVeto = new G4Box("HEPVetoSolid",hepVetoX*0.5,hepVetoY*0.5,hepVetoLength*0.5);
  fHEPVetoVolume = new G4LogicalVolume(solidHEPVeto,G4Material::GetMaterial("G4_Galactic"),"HEPVetoLogic",0,0,0);
  fHEPVetoVolume->SetVisAttributes(G4VisAttributes::Invisible);
  new G4PVPlacement(rotHEPVeto,posHEPVeto,fHEPVetoVolume,"HEPVeto",fMotherVolume,false,0,false);

  // Create standard scintillator finger
  G4double hepVetoFingerX      = geo->GetFingerSizeX();
  G4double hepVetoFingerY      = geo->GetFingerSizeY();
  G4double hepVetoFingerLength = geo->GetFingerSizeZ();
  printf("HEPVeto Finger size is %f %f %f\n",hepVetoFingerX,hepVetoFingerY,hepVetoFingerLength);
  G4Box* solidFinger  = new G4Box("HEPVetoFingerSolid",hepVetoFingerX*0.5,hepVetoFingerY*0.5,hepVetoFingerLength*0.5);
  fFingerVolume  = new G4LogicalVolume(solidFinger,G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"HEPVetoFingerLogic",0,0,0);
  fFingerVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Yellow()));

  // Make crystal a sensitive detector
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
