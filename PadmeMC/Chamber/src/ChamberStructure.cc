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
#include "G4Sphere.hh"
#include "G4ExtrudedSolid.hh"
#include "G4SubtractionSolid.hh"

#include "ChamberGeometry.hh"

using namespace CLHEP;

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

  /////////////////////////////////////////
  // Thin window flange in front of ECal //
  /////////////////////////////////////////

  printf("Vacuum chamber window\n");
  G4double ewR = geo->GetEWRadius(); // Radius of the membrane
  G4double ewC = geo->GetEWConvexity(); // Convexity at membrane center

  G4double ewd1 = geo->GetEWBackMylarThick(); // Thickness of mylar (layer 1) on external face of chamber
  G4double ewd2 = geo->GetEWKevlarThick(); // Thickness of kevlar (layer 2) between mylar layers
  G4double ewd3 = geo->GetEWFrontMylarThick(); // Thickness of mylar (layer 3) on internal face of chamber

  printf("ewR %f ewC %f ewd1 %f ewd2 %f ewd3 %f\n",ewR,ewC,ewd1,ewd2,ewd3);

  // External mylar membrane
  G4double ewr1 = (ewR*ewR+ewC*ewC)/(2.*ewC);
  G4double ewz1 = geo->GetVCBackFacePosZ()+ewr1-ewC;
  G4double ewth1 = asin(ewR/ewr1);
  printf("ewr1 %f ewz1 %f ewth1 %11.9f pi-ewth1 %11.9f\n",ewr1,ewz1,ewth1,180.*deg-ewth1);
  G4Sphere* solidEWSphere1 = new G4Sphere("EWSphere1",ewr1,ewr1+ewd1,0.*deg,360.*deg,180.*deg-ewth1,ewth1);
  //G4LogicalVolume* logicalEWSphere1 = new G4LogicalVolume(solidEWSphere1,G4Material::GetMaterial("G4_MYLAR"), "EWSphere1",0,0,0);
  //logicalEWSphere1->SetVisAttributes(G4VisAttributes(G4Colour::White()));
  //new G4PVPlacement(0,G4ThreeVector(0.,0.,ewz1),logicalEWSphere1,"EWSphere1",fMotherVolume,false,0);
  G4Tubs* solidEWRing1 = new G4Tubs("EWRing1",ewR-0.01*mm,ewR+10.*mm,0.7*ewd1,0.*deg,360.*deg);
  G4ThreeVector ring1Pos = G4ThreeVector(0.,0.,-sqrt(ewr1*ewr1-ewR*ewR)-0.5*ewd1);
  //G4LogicalVolume* logicalEWRing1 = new G4LogicalVolume(solidEWRing1,G4Material::GetMaterial("G4_MYLAR"), "EWRing1",0,0,0);
  //logicalEWRing1->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  //new G4PVPlacement(0,ring1Pos,logicalEWRing1,"EWRing1",fMotherVolume,false,0);
  G4SubtractionSolid* solidEWLayer1 = new G4SubtractionSolid("EWLayer1",solidEWSphere1,solidEWRing1,0,ring1Pos);
  G4LogicalVolume* logicalEWLayer1 = new G4LogicalVolume(solidEWLayer1,G4Material::GetMaterial("G4_MYLAR"), "EWLayer1",0,0,0);
  logicalEWLayer1->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));
  new G4PVPlacement(0,G4ThreeVector(0.,0.,ewz1),logicalEWLayer1,"EWLayer1",fMotherVolume,false,0);

  // Central kevlar membrane
  G4double ewr2 = ewr1+ewd1;
  G4double ewz2 = ewz1;
  G4double ewth2 = asin(ewR/ewr2);
  printf("ewr2 %f ewz2 %f ewth2 %11.9f\n",ewr2,ewz2,ewth2);
  G4Sphere* solidEWSphere2 = new G4Sphere("EWSphere2",ewr2,ewr2+ewd2,0.*deg,360.*deg,180.*deg-ewth2,ewth2);
  G4Tubs* solidEWRing2 = new G4Tubs("EWRing2",ewR-0.01*mm,ewR+10.*mm,0.7*ewd2,0.*deg,360.*deg);
  G4ThreeVector ring2Pos = G4ThreeVector(0.,0.,-sqrt(ewr2*ewr2-ewR*ewR)-0.5*ewd2);
  G4SubtractionSolid* solidEWLayer2 = new G4SubtractionSolid("EWLayer2",solidEWSphere2,solidEWRing2,0,ring2Pos);
  G4LogicalVolume* logicalEWLayer2 = new G4LogicalVolume(solidEWLayer2,G4Material::GetMaterial("Kevlar"), "EWLayer2",0,0,0);
  logicalEWLayer2->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));
  new G4PVPlacement(0,G4ThreeVector(0.,0.,ewz2),logicalEWLayer2,"EWLayer2",fMotherVolume,false,0);

  // Internal mylar membrane
  G4double ewr3 = ewr2+ewd2;
  G4double ewz3 = ewz1;
  G4double ewth3 = asin(ewR/ewr3);
  printf("ewr3 %f ewz3 %f ewth3 %11.9f\n",ewr3,ewz3,ewth3);
  G4Sphere* solidEWSphere3 = new G4Sphere("EWSphere3",ewr3,ewr3+ewd3,0.*deg,360.*deg,180.*deg-ewth3,ewth3);
  G4Tubs* solidEWRing3 = new G4Tubs("EWRing3",ewR-0.01*mm,ewR+10.*mm,0.7*ewd3,0.*deg,360.*deg);
  G4ThreeVector ring3Pos = G4ThreeVector(0.,0.,-sqrt(ewr3*ewr3-ewR*ewR)-0.5*ewd3);
  G4SubtractionSolid* solidEWLayer3 = new G4SubtractionSolid("EWLayer3",solidEWSphere3,solidEWRing3,0,ring3Pos);
  G4LogicalVolume* logicalEWLayer3 = new G4LogicalVolume(solidEWLayer3,G4Material::GetMaterial("G4_MYLAR"), "EWLayer3",0,0,0);
  logicalEWLayer3->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));
  new G4PVPlacement(0,G4ThreeVector(0.,0.,ewz3),logicalEWLayer3,"EWLayer3",fMotherVolume,false,0);

  // First steel ring (outer)
  G4Tubs* solidEWIronRing1 = new G4Tubs("EWIronRing1",geo->GetEWF1RIn(),geo->GetEWF1ROut(),0.5*geo->GetEWF1Thick(),0.*deg,360.*deg);
  G4LogicalVolume* logicalEWIronRing1 = new G4LogicalVolume(solidEWIronRing1,G4Material::GetMaterial("G4_STAINLESS-STEEL"), "EWIronRing1",0,0,0);
  logicalEWIronRing1->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
  new G4PVPlacement(0,G4ThreeVector(0.,0.,geo->GetEWF1PosZ()),logicalEWIronRing1,"EWIronRing1",fMotherVolume,false,0);

  // Second steel ring
  G4Tubs* solidEWIronRing2 = new G4Tubs("EWIronRing2",geo->GetEWF2RIn(),geo->GetEWF2ROut(),0.5*geo->GetEWF2Thick(),0.*deg,360.*deg);
  G4LogicalVolume* logicalEWIronRing2 = new G4LogicalVolume(solidEWIronRing2,G4Material::GetMaterial("G4_STAINLESS-STEEL"), "EWIronRing2",0,0,0);
  logicalEWIronRing2->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
  new G4PVPlacement(0,G4ThreeVector(0.,0.,geo->GetEWF2PosZ()),logicalEWIronRing2,"EWIronRing2",fMotherVolume,false,0);

  // Third steel ring
  G4Tubs* solidEWIronRing3 = new G4Tubs("EWIronRing3",geo->GetEWF3RIn(),geo->GetEWF3ROut(),0.5*geo->GetEWF3Thick(),0.*deg,360.*deg);
  G4LogicalVolume* logicalEWIronRing3 = new G4LogicalVolume(solidEWIronRing3,G4Material::GetMaterial("G4_STAINLESS-STEEL"), "EWIronRing3",0,0,0);
  logicalEWIronRing3->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
  new G4PVPlacement(0,G4ThreeVector(0.,0.,geo->GetEWF3PosZ()),logicalEWIronRing3,"EWIronRing3",fMotherVolume,false,0);

  //////////////////////////////////
  // Main vacuum chamber cylinder //
  //////////////////////////////////

  // Flange toward thin window flange
  G4Tubs* solidVCCCFRing = new G4Tubs("VCCCFRing",geo->GetVCCFRIn(),geo->GetVCCFROut(),0.5*geo->GetVCCFThick(),0.*deg,360.*deg);
  G4LogicalVolume* logicalVCCCFRing = new G4LogicalVolume(solidVCCCFRing,G4Material::GetMaterial("G4_STAINLESS-STEEL"), "VCCCFRing",0,0,0);
  logicalVCCCFRing->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));
  new G4PVPlacement(0,G4ThreeVector(0.,0.,geo->GetVCCFPosZ()),logicalVCCCFRing,"VCCCFRing",fMotherVolume,false,0);

  // Big cylinder
  G4double cyLen = geo->GetVCCLength();
  G4double cyRIn = geo->GetVCCRIn();
  G4double cyROut = geo->GetVCCROut();
  G4double cyPosZ = geo->GetVCCPosZ();
  G4Tubs* solidVCCyRing = new G4Tubs("VCCyRing",cyRIn,cyROut,0.5*cyLen,0.*deg,360.*deg);
  // Remove open section
  G4ThreeVector cyOpenPos = G4ThreeVector(-cyROut,0.,0.);
  G4Box* solidVCCyOpen = new G4Box("VCCyOpen",0.5*cyROut,0.5*geo->GetVCOutMagWallSizeY(),0.5*cyLen+1.*mm);
  G4SubtractionSolid* solidVCCylinder = new G4SubtractionSolid("VCCylinder",solidVCCyRing,solidVCCyOpen,0,cyOpenPos);
  G4LogicalVolume* logicalVCCylinder = new G4LogicalVolume(solidVCCylinder,G4Material::GetMaterial("G4_STAINLESS-STEEL"), "VCCylinder",0,0,0);
  logicalVCCylinder->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));
  new G4PVPlacement(0,G4ThreeVector(0.,0.,cyPosZ),logicalVCCylinder,"VCCylinder",fMotherVolume,false,0);
}
