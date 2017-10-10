// TungstenDetector.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#include "TungstenDetector.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"

#include "G4Element.hh"
#include "G4Material.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4SystemOfUnits.hh"

#include "TungstenGeometry.hh"
#include "TungstenMessenger.hh"

TungstenDetector::TungstenDetector(G4LogicalVolume* motherVolume):fMotherVolume(motherVolume)
{

  // Connect to TungstenDetectorMessenger to enable datacard configuration
  fTungstenMessenger = new TungstenMessenger(this);

  fTungstenDisplacePosZ = 0.; // No default displacement

}

TungstenDetector::~TungstenDetector()
{
  delete fTungstenMessenger;
}

void TungstenDetector::CreateGeometry()
{

  TungstenGeometry* geo = TungstenGeometry::GetInstance();

  // Get Tungsten target dump position and dimensions

  G4ThreeVector tungPos = G4ThreeVector(geo->GetTungstenPosX(),geo->GetTungstenPosY(),geo->GetTungstenPosZ());
  printf("Tungsten will be placed at %.3fmm %.3fmm %.3fmm\n",tungPos.x()/mm,tungPos.y()/mm,tungPos.z()/mm);

  G4double tungLength = geo->GetTungstenSizeZ();
  G4double tungRadius = geo->GetTungstenRadius();
  printf("Tungsten length %.3fmm radius %.3fmm\n",tungLength/mm,tungRadius/mm);

  // Create main Tungsten cylinder and position it
  // Do not forget to take into account mother volume Z displacement
  G4Tubs* tungSolid = new G4Tubs("Tungsten",0.,tungRadius,0.5*tungLength,0.*deg,360.*deg);
  G4LogicalVolume* tungLogical = new G4LogicalVolume(tungSolid,G4Material::GetMaterial("G4_W"),"Tungsten",0,0,0);
  tungLogical->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  new G4PVPlacement(0,tungPos-G4ThreeVector(0.,0.,fTungstenDisplacePosZ),tungLogical,"Tungsten",fMotherVolume,false,0,true);

}
