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
#include "G4DigiManager.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"
#include "G4SubtractionSolid.hh"

#include "HEPVetoGeometry.hh"
#include "HEPVetoSD.hh"
#include "HEPVetoDigitizer.hh"

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
  new G4PVPlacement(rotHEPVeto,posHEPVeto,fHEPVetoVolume,"HEPVeto",fMotherVolume,false,0,true);

  // Create standard scintillator finger
  G4double hepVetoFingerX = geo->GetFingerSizeX();
  G4double hepVetoFingerY = geo->GetFingerSizeY();
  G4double hepVetoFingerZ = geo->GetFingerSizeZ();
  printf("HEPVeto Finger size is %f %f %f\n",hepVetoFingerX,hepVetoFingerY,hepVetoFingerZ);
  G4Box* solidFingerFull  = new G4Box("HEPVetoFingerSolidFull",0.5*hepVetoFingerX,0.5*hepVetoFingerY,0.5*hepVetoFingerZ);

// Create groove and subtract from finger  M. Raggi 23/06/2018
  G4double grooveSizeX = geo->GetGrooveSizeX();
  G4double grooveSizeY = geo->GetGrooveSizeY();
  G4double grooveSizeZ = geo->GetGrooveSizeZ();
  G4double groovePosX = geo->GetGroovePosX();
  G4double groovePosY = geo->GetGroovePosY();
  G4double groovePosZ = geo->GetGroovePosZ();
  G4cout<<"GSZ "<<grooveSizeX<<" "<<grooveSizeY<<" "<< grooveSizeZ<<G4endl;
  G4Box* solidGroove = new G4Box("HEPVetoGrooveSolid",grooveSizeX*0.5,grooveSizeY*0.5,grooveSizeZ*0.5);
//
  G4SubtractionSolid* solidFinger = new G4SubtractionSolid("HEPVetoCrySolid",solidFingerFull,solidGroove,0,G4ThreeVector(groovePosX,groovePosY,groovePosZ));
//
  fFingerVolume  = new G4LogicalVolume(solidFinger,G4Material::GetMaterial("G4_POLYSTYRENE"),"HEPVetoFingerLogic",0,0,0);
  fFingerVolume->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));

  // Get number of fingers and position them
  G4int nFingers = geo->GetHEPVetoNFingers();
  // Added rotation accordig to positron veto M. Raggi 23/06/2018 Check with drawing!
  G4RotationMatrix* rotFinger = new G4RotationMatrix();
  rotFinger->rotateY(geo->GetFingerRotY());
  for (G4int fin=0;fin<nFingers;fin++){
    G4ThreeVector posFinger = G4ThreeVector(geo->GetFingerPosX(fin),geo->GetFingerPosY(fin),geo->GetFingerPosZ(fin));
    new G4PVPlacement(rotFinger,posFinger,fFingerVolume,"HEPVetoFinger",fHEPVetoVolume,false,fin,false);
  }

  // Create digitizer for HEPVeto
  G4DigiManager* theDM = G4DigiManager::GetDMpointer();
  G4String hepvetoDName = geo->GetHEPVetoDigitizerName();
  printf("Registering HEPVeto Digitizer %s\n",hepvetoDName.data());
  HEPVetoDigitizer* hepvetoD = new HEPVetoDigitizer(hepvetoDName);
  theDM->AddNewModule(hepvetoD);

  // Make finger a sensitive detector
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String hepVetoSDName = geo->GetHEPVetoSensitiveDetectorName();
  printf("Registering HEPVeto Sensitive Detector %s\n",hepVetoSDName.data());
  HEPVetoSD* hepVetoSD = new HEPVetoSD(hepVetoSDName);
  sdMan->AddNewDetector(hepVetoSD);
  fFingerVolume->SetSensitiveDetector(hepVetoSD);

}
