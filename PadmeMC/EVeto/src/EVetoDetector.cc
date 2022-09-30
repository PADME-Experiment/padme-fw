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
#include "G4SubtractionSolid.hh"
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"

#include "EVetoGeometry.hh"
#include "EVetoSD.hh"
#include "EVetoDigitizer.hh"

EVetoDetector::EVetoDetector(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{

  // Connect to EVetoMessenger to enable datacard configuration
  fEVetoMessenger = new EVetoMessenger(this);

  fEVetoDisplacePosZ = 0.; // No default displacement

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
  G4ThreeVector posEVeto = G4ThreeVector(geo->GetEVetoPosX(),geo->GetEVetoPosY(),geo->GetEVetoPosZ()-fEVetoDisplacePosZ); 
  G4double eVetoX      = geo->GetEVetoSizeX();
  G4double eVetoY      = geo->GetEVetoSizeY();
  G4double eVetoLength = geo->GetEVetoSizeZ();
  printf("EVeto size is %f %f %f\n",eVetoX,eVetoY,eVetoLength);
  G4Box* solidEVeto = new G4Box("EVetoSolid",eVetoX*0.5,eVetoY*0.5,eVetoLength*0.5);
  fEVetoVolume = new G4LogicalVolume(solidEVeto,G4Material::GetMaterial("Vacuum"),"EVetoLogic",0,0,0);
  fEVetoVolume->SetVisAttributes(G4VisAttributes::GetInvisible());
  new G4PVPlacement(0,posEVeto,fEVetoVolume,"EVeto",fMotherVolume,false,0,true);

  // Create standard scintillator finger
  G4double eVetoFingerX      = geo->GetFingerSizeX();
  G4double eVetoFingerY      = geo->GetFingerSizeY();
  G4double eVetoFingerLength = geo->GetFingerSizeZ();
  printf("EVeto Finger size is %f %f %f\n",eVetoFingerX,eVetoFingerY,eVetoFingerLength);
  G4Box* solidFingerFull  = new G4Box("EVetoCrySolidFull",eVetoFingerX*0.5,eVetoFingerY*0.5,eVetoFingerLength*0.5);

  // Create groove and subtract from finger
  G4double grooveSizeX = geo->GetGrooveSizeX();
  G4double grooveSizeY = geo->GetGrooveSizeY();
  G4double grooveSizeZ = geo->GetGrooveSizeZ();
  G4double groovePosX = geo->GetGroovePosX();
  G4double groovePosY = geo->GetGroovePosY();
  G4double groovePosZ = geo->GetGroovePosZ();
  G4Box* solidGroove = new G4Box("EVetoGrooveSolid",grooveSizeX*0.5,grooveSizeY*0.5,grooveSizeZ*0.5);
  G4SubtractionSolid* solidFinger = new G4SubtractionSolid("EVetoCrySolid",solidFingerFull,solidGroove,0,G4ThreeVector(groovePosX,groovePosY,groovePosZ));

  fFingerVolume  = new G4LogicalVolume(solidFinger,G4Material::GetMaterial("G4_POLYSTYRENE"),"EVetoFingerLogic",0,0,0);
  fFingerVolume->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));

  // Get number of fingers and position them
  G4int nFingers = geo->GetEVetoNFingers();
  G4RotationMatrix* rotFinger = new G4RotationMatrix();
  rotFinger->rotateY(geo->GetFingerRotY());
  for (G4int fin=0;fin<nFingers;fin++){
    G4ThreeVector posFinger = G4ThreeVector(geo->GetFingerPosX(fin),geo->GetFingerPosY(fin),geo->GetFingerPosZ(fin));
    new G4PVPlacement(rotFinger,posFinger,fFingerVolume,"EVetoFinger",fEVetoVolume,false,fin,false);
  }

  // Create and position Up and Down supports
  G4double suppSizeX = geo->GetSupportUSizeX();
  G4double suppSizeY = geo->GetSupportUSizeY();
  G4double suppSizeZ = geo->GetSupportUSizeZ();
  G4Box* solidSupport  = new G4Box("EVetoSupport",0.5*suppSizeX,0.5*suppSizeY,0.5*suppSizeZ);
  G4LogicalVolume* logicSupport  = new G4LogicalVolume(solidSupport,G4Material::GetMaterial("G4_Al"),"EVetoSupport",0,0,0);
  logicSupport->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));

  G4ThreeVector suppUPos = G4ThreeVector(geo->GetSupportUPosX(),geo->GetSupportUPosY(),geo->GetSupportUPosZ());
  new G4PVPlacement(0,suppUPos,logicSupport,"EVetoSupportU",fEVetoVolume,false,0,true);

  G4ThreeVector suppDPos = G4ThreeVector(geo->GetSupportDPosX(),geo->GetSupportDPosY(),geo->GetSupportDPosZ());
  new G4PVPlacement(0,suppDPos,logicSupport,"EVetoSupportD",fEVetoVolume,false,0,true);
 
  // Create digitizer for EVeto
  G4DigiManager* theDM = G4DigiManager::GetDMpointer();
  G4String evetoDName = geo->GetEVetoDigitizerName();
  printf("Registering EVeto Digitizer %s\n",evetoDName.data());
  EVetoDigitizer* evetoD = new EVetoDigitizer(evetoDName);
  theDM->AddNewModule(evetoD);

  // Make finger a sensitive detector
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String eVetoSDName = geo->GetEVetoSensitiveDetectorName();
  printf("Registering EVeto Sensitive Detector %s\n",eVetoSDName.data());
  EVetoSD* eVetoSD = new EVetoSD(eVetoSDName);
  sdMan->AddNewDetector(eVetoSD);
  fFingerVolume->SetSensitiveDetector(eVetoSD);

}
