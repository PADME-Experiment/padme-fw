// LAVDetector.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#include "LAVDetector.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Tubs.hh"
#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"

#include "LAVGeometry.hh"
#include "LAVSD.hh"

LAVDetector::LAVDetector(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{

  // Connect to LAVMessenger to enable datacard configuration
  fLAVMessenger = new LAVMessenger(this);

}

LAVDetector::~LAVDetector()
{
  delete fLAVMessenger;
}

void LAVDetector::CreateGeometry()
{

  LAVGeometry* geo = LAVGeometry::GetInstance();

  // Create main LAV box
  printf("LAV will be placed at %f %f %f\n",geo->GetLAVPosX(),geo->GetLAVPosY(),geo->GetLAVPosZ());
  G4ThreeVector lavPos = G4ThreeVector(geo->GetLAVPosX(),geo->GetLAVPosY(),geo->GetLAVPosZ()); 
  G4double lavInnerR = geo->GetLAVInnerRadius();
  G4double lavOuterR = geo->GetLAVOuterRadius();
  G4double lavZLength = geo->GetLAVZLength();
  printf("LAV size is %f %f %f\n",lavInnerR,lavOuterR,lavZLength);
  G4Tubs* solidLAV = new G4Tubs("LAV",lavInnerR,lavOuterR,lavZLength*0.5,0.*rad,2.*M_PI*rad);
  //  fLAVVolume = new G4LogicalVolume(solidLAV,G4Material::GetMaterial("G4_PbWO4"),"LAV",0,0,0);
  fLAVVolume = new G4LogicalVolume(solidLAV,G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"LAV",0,0,0);
  fLAVVolume->SetVisAttributes(G4VisAttributes(G4Colour::Magenta()));
  new G4PVPlacement(0,lavPos,fLAVVolume,"LAV",fMotherVolume,false,0,false);

  // Make whole LAV a sensitive detector
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String lavSDName = geo->GetLAVSensitiveDetectorName();
  printf("Registering LAV SD %s\n",lavSDName.data());
  LAVSD* lavSD = new LAVSD(lavSDName);
  sdMan->AddNewDetector(lavSD);
  fLAVVolume->SetSensitiveDetector(lavSD);

}
