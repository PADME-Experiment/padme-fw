// ChamberStructure.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-06-10
// --------------------------------------------------------------

#include "ChamberStructure.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

#include "G4Material.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"

#include "G4Tubs.hh"
#include "G4ExtrudedSolid.hh"
#include "G4SubtractionSolid.hh"

#include "ChamberGeometry.hh"

ChamberStructure::ChamberStructure(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{}

ChamberStructure::~ChamberStructure()
{}

void ChamberStructure::CreateGeometry()
{

  ChamberGeometry* geo = ChamberGeometry::GetInstance();

  // Create wall of chamber inside magnetic field
  G4int nVW = geo->GetVCInMagWallNVertices();
  std::vector<G4TwoVector> vW(nVW);
  for(G4int iV=0;iV<nVW;iV++) { vW[iV] = geo->GetVCInMagWallVertex(iV); }
  G4double halfWallSizeY = 0.5*geo->GetVCInMagWallSizeY();
  G4ExtrudedSolid* solidVCInMagWallNoHole = new G4ExtrudedSolid("VCInMagWallNoHole",vW,halfWallSizeY,G4TwoVector(0.,0.),1.,G4TwoVector(0.,0.),1.);

  // Drill a hole to let the beam in
  G4double holeHalfSizeZ = 0.5*geo->GetVCInMagThick()+0.1*mm;
  G4double holeRadius = geo->GetVCInMagWallHoleRadius();
  G4Tubs* solidHole = new G4Tubs("VCInMagHole",0.,holeRadius,holeHalfSizeZ,0.,2.*M_PI*rad);
  G4double holePosZ = geo->GetVCInMagWallHolePosZ();
  G4RotationMatrix* holeRot = new G4RotationMatrix;
  holeRot->rotateX(90.*deg);
  G4ThreeVector holePos = G4ThreeVector(0.,holePosZ,0.);
  G4SubtractionSolid* solidVCInMagWall = new G4SubtractionSolid("VCInMagWall",solidVCInMagWallNoHole,solidHole,holeRot,holePos);

  G4LogicalVolume* logicalVCInMagWall = new G4LogicalVolume(solidVCInMagWall,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"VCInMagWall",0,0,0);
  logicalVCInMagWall->SetVisAttributes(G4VisAttributes(G4Colour::White()));
  G4RotationMatrix* rotWall = new G4RotationMatrix;
  rotWall->rotateX(-90.*deg);
  new G4PVPlacement(rotWall,G4ThreeVector(0.,0.,0.),logicalVCInMagWall,"VCInMagWall",fMagneticVolume,false,0);

  // Create plates of chamber inside magnetic field
  G4int nVP = geo->GetVCInMagPlateNVertices();
  std::vector<G4TwoVector> vP(nVP);
  for(G4int iV=0;iV<nVP;iV++) { vP[iV] = geo->GetVCInMagPlateVertex(iV); }
  //G4double halfPlateSizeY = 0.5*geo->GetVCInMagThick();
  G4double halfPlateSizeY = 0.5*(geo->GetVCInMagThick()-0.01*mm);
  G4ExtrudedSolid* solidVCInMagPlate = new G4ExtrudedSolid("VCInMagPlate",vP,halfPlateSizeY,G4TwoVector(0.,0.),1.,G4TwoVector(0.,0.),1.);
  G4LogicalVolume* logicalVCInMagPlate = new G4LogicalVolume(solidVCInMagPlate,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"VCInMagPlate",0,0,0);
  logicalVCInMagPlate->SetVisAttributes(G4VisAttributes(G4Colour::White()));
  G4double yDispl = 0.5*(geo->GetVCInMagWallSizeY()+geo->GetVCInMagThick());
  G4RotationMatrix* rotPlate = new G4RotationMatrix;
  rotPlate->rotateX(-90.*deg);
  new G4PVPlacement(rotPlate,G4ThreeVector(0.,yDispl,0.),logicalVCInMagPlate,"VCInMagPlate",fMagneticVolume,false,0);
  new G4PVPlacement(rotPlate,G4ThreeVector(0.,-yDispl,0.),logicalVCInMagPlate,"VCInMagPlate",fMagneticVolume,false,1);

  // Create vacuum chamber wall outside of magnetic field
  G4int nVWo = geo->GetVCOutMagWallNVertices();
  std::vector<G4TwoVector> vWo(nVWo);
  for(G4int iV=0;iV<nVWo;iV++) { vWo[iV] = geo->GetVCOutMagWallVertex(iV); }
  G4double halfWallOSizeY = 0.5*geo->GetVCOutMagWallSizeY();
  G4ExtrudedSolid* solidVCOutMagWall = new G4ExtrudedSolid("VCOutMagWall",vWo,halfWallOSizeY,G4TwoVector(0.,0.),1.,G4TwoVector(0.,0.),1.);
  G4LogicalVolume* logicalVCOutMagWall = new G4LogicalVolume(solidVCOutMagWall,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"VCOutMagWall",0,0,0);
  logicalVCOutMagWall->SetVisAttributes(G4VisAttributes(G4Colour::White()));
  G4RotationMatrix* rotWallO = new G4RotationMatrix;
  rotWallO->rotateX(-90.*deg);
  new G4PVPlacement(rotWallO,G4ThreeVector(0.,0.,0.),logicalVCOutMagWall,"VCOutMagWall",fMotherVolume,false,0);

}
