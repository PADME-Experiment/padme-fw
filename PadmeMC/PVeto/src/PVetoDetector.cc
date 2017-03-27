// PVetoDetector.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#include "PVetoDetector.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Box.hh"
#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"

#include "PVetoGeometry.hh"
#include "PVetoSD.hh"

PVetoDetector::PVetoDetector(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{

  // Connect to PVetoMessenger to enable datacard configuration
  fPVetoMessenger = new PVetoMessenger(this);

  fPVetoDisplacePosZ = 0.; // No default displacement

}

PVetoDetector::~PVetoDetector()
{
  delete fPVetoMessenger;
}

void PVetoDetector::CreateGeometry()
{

  PVetoGeometry* geo = PVetoGeometry::GetInstance();

  // Create main PVeto box
  printf("PVeto will be placed at %f %f %f\n",geo->GetPVetoPosX(),geo->GetPVetoPosY(),geo->GetPVetoPosZ());
  G4ThreeVector posPVeto = G4ThreeVector(geo->GetPVetoPosX(),geo->GetPVetoPosY(),geo->GetPVetoPosZ()-fPVetoDisplacePosZ); 
  G4double pVetoX      = geo->GetPVetoSizeX();
  G4double pVetoY      = geo->GetPVetoSizeY();
  G4double pVetoLength = geo->GetPVetoSizeZ();
  printf("PVeto size is %f %f %f\n",pVetoX,pVetoY,pVetoLength);
  G4Box* solidPVeto = new G4Box("PVetoSolid",pVetoX*0.5,pVetoY*0.5,pVetoLength*0.5);
  fPVetoVolume = new G4LogicalVolume(solidPVeto,G4Material::GetMaterial("Vacuum"),"PVetoLogic",0,0,0);
  fPVetoVolume->SetVisAttributes(G4VisAttributes::Invisible);
  //  fPVetoVolume->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));
  new G4PVPlacement(0,posPVeto,fPVetoVolume,"PVeto",fMotherVolume,false,0,false);

  // Create standard scintillator finger
  G4double pVetoFingerX      = geo->GetFingerSizeX();
  G4double pVetoFingerY      = geo->GetFingerSizeY();
  G4double pVetoFingerLength = geo->GetFingerSizeZ();
  printf("PVeto Finger size is %f %f %f\n",pVetoFingerX,pVetoFingerY,pVetoFingerLength);
  G4Box* solidFinger  = new G4Box("PVetoCrySolid",pVetoFingerX*0.5,pVetoFingerY*0.5,pVetoFingerLength*0.5);
  fFingerVolume  = new G4LogicalVolume(solidFinger,G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"PVetoFingerLogic",0,0,0);
  fFingerVolume->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));

  // Make finger a sensitive detector
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String pVetoSDName = geo->GetPVetoSensitiveDetectorName();
  printf("Registering PVeto SD %s\n",pVetoSDName.data());
  PVetoSD* pVetoSD = new PVetoSD(pVetoSDName);
  sdMan->AddNewDetector(pVetoSD);
  fFingerVolume->SetSensitiveDetector(pVetoSD);

  // Get number of fingers and position them
  G4int nFingers = geo->GetPVetoNFingers();
  G4RotationMatrix* rotFinger = new G4RotationMatrix();
  rotFinger->rotateY(geo->GetFingerRotY());
  for (G4int fin=0;fin<nFingers;fin++){
    G4ThreeVector posFinger = G4ThreeVector(geo->GetFingerPosX(fin),geo->GetFingerPosY(fin),geo->GetFingerPosZ(fin));
    new G4PVPlacement(rotFinger,posFinger,fFingerVolume,"PVetoFinger",fPVetoVolume,false,fin,false);
  }

  // Create and position Up and Down supports
  G4double suppSizeX = geo->GetSupportUSizeX();
  G4double suppSizeY = geo->GetSupportUSizeY();
  G4double suppSizeZ = geo->GetSupportUSizeZ();
  G4Box* solidSupport  = new G4Box("PVetoSupport",0.5*suppSizeX,0.5*suppSizeY,0.5*suppSizeZ);
  G4LogicalVolume* logicSupport  = new G4LogicalVolume(solidSupport,G4Material::GetMaterial("G4_Al"),"PVetoSupport",0,0,0);
  logicSupport->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));

  G4ThreeVector suppUPos = G4ThreeVector(geo->GetSupportUPosX(),geo->GetSupportUPosY(),geo->GetSupportUPosZ());
  new G4PVPlacement(0,suppUPos,logicSupport,"PVetoSupportU",fPVetoVolume,false,0,false);

  G4ThreeVector suppDPos = G4ThreeVector(geo->GetSupportDPosX(),geo->GetSupportDPosY(),geo->GetSupportDPosZ());
  new G4PVPlacement(0,suppDPos,logicSupport,"PVetoSupportD",fPVetoVolume,false,0,false);

}
