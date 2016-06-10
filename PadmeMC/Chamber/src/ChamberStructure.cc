// ChamberStructure.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-06-10
// --------------------------------------------------------------

#include "ChamberStructure.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Box.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"

#include "ChamberGeometry.hh"

ChamberStructure::ChamberStructure(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{}

ChamberStructure::~ChamberStructure()
{}

void ChamberStructure::CreateGeometry()
{

  ChamberGeometry* geo = ChamberGeometry::GetInstance();

  /*
  // Create single concrete block
  G4double blockSide = geo->GetBlockSide(); // Gap between blocks already subtracted
  G4Box* blockSolid = new G4Box("WallBlock",0.5*blockSide,0.5*blockSide,0.5*blockSide);
  G4LogicalVolume* blockVolume = new G4LogicalVolume(blockSolid,G4Material::GetMaterial("G4_CONCRETE"),"WallBlock",0,0,0);
  blockVolume->SetVisAttributes(G4VisAttributes(G4Colour::White()));

  // Position blocks to make wall
  for(G4int i = 0; i < geo->GetNumberOfBlocks(); i++) {
    G4ThreeVector blockPos = geo->GetBlockPosition(i);
    G4cout << "ChamberStructure - Block at " << blockPos << G4endl;
    new G4PVPlacement(0,blockPos,blockVolume,"WallBlock",fMotherVolume,false,i,false);
  }
  */
}
