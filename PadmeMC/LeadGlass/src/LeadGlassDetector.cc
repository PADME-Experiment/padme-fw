// LeadGlassDetector.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2023-09-20
// --------------------------------------------------------------

#include "LeadGlassDetector.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"
#include "G4DigiManager.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"

#include "LeadGlassMessenger.hh"
#include "LeadGlassGeometry.hh"
#include "LeadGlassBlock.hh"
#include "LeadGlassSD.hh"
//#include "LeadGlassDigitizer.hh"

LeadGlassDetector::LeadGlassDetector(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{

  // Connect to LeadGlassMessenger to enable datacard configuration
  fLeadGlassMessenger = new LeadGlassMessenger(this);

}

LeadGlassDetector::~LeadGlassDetector()
{
  delete fLeadGlassMessenger;
}

void LeadGlassDetector::CreateGeometry()
{

  //G4VisAttributes LeadGlassBarVisAtt = G4VisAttributes(G4Colour::Green());
  LeadGlassGeometry* geo = LeadGlassGeometry::GetInstance();

  // Create PbGl block
  LeadGlassBlock* lgBlock = new LeadGlassBlock(geo->GetBlockId());
  fLeadGlassVolume = lgBlock->GetBlockLogicalVolume();

  //G4ThreeVector LeadGlassPos = G4ThreeVector(geo->GetETagPosX(),geo->GetETagPosY(),geo->GetETagPosZ());
  G4ThreeVector posLeadGlass = G4ThreeVector(0.,0.,geo->GetLeadGlassFrontFacePosZ()+0.5*lgBlock->GetBlockZLength());
  G4RotationMatrix* rotLeadGlass = new G4RotationMatrix;
  rotLeadGlass->rotateY(180.*deg);
  new G4PVPlacement(rotLeadGlass,posLeadGlass,fLeadGlassVolume,"LeadGlass",fMotherVolume,false,0,true);

  /*
  // Create digitizer for LeadGlass
  G4DigiManager* theDM = G4DigiManager::GetDMpointer();
  G4String LeadGlassDName = geo->GetLeadGlassDigitizerName();
  printf("Registering LeadGlass Digitizer %s\n",LeadGlassDName.data());
  LeadGlassDigitizer* LeadGlassD = new LeadGlassDigitizer(LeadGlassDName);
  theDM->AddNewModule(LeadGlassD);
  */

  // Make whole LeadGlass (PbGl block and light guide) a sensitive detector
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String leadglassSDName = geo->GetLeadGlassSensitiveDetectorName();
  printf("Registering LeadGlass SD %s\n",leadglassSDName.data());
  LeadGlassSD* leadglassSD = new LeadGlassSD(leadglassSDName);
  sdMan->AddNewDetector(leadglassSD);
  lgBlock->GetPbGlBlockLogicalVolume()->SetSensitiveDetector(leadglassSD);
  lgBlock->GetLightGuideLogicalVolume()->SetSensitiveDetector(leadglassSD);

}
