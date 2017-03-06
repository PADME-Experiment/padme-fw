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

#include "G4Box.hh"
#include "G4Trap.hh"
#include "G4GenericTrap.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4ExtrudedSolid.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4TessellatedSolid.hh"
#include "G4TriangularFacet.hh"
#include "G4QuadrangularFacet.hh"
#include "G4Polyhedron.hh"

#include "G4UIcommand.hh"

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

  /*
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
  //new G4PVPlacement(rotWall,G4ThreeVector(0.,0.,0.),logicalVCInMagWall,"VCInMagWall",fMagneticVolume,false,0);
  new G4PVPlacement(rotWall,G4ThreeVector(0.,0.,0.),logicalVCInMagWall,"VCInMagWall",fMotherVolume,false,0);

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
  //new G4PVPlacement(rotPlate,G4ThreeVector(0.,yDispl,0.),logicalVCInMagPlate,"VCInMagPlate",fMagneticVolume,false,0);
  //new G4PVPlacement(rotPlate,G4ThreeVector(0.,-yDispl,0.),logicalVCInMagPlate,"VCInMagPlate",fMagneticVolume,false,1);
  new G4PVPlacement(rotPlate,G4ThreeVector(0., yDispl,0.),logicalVCInMagPlate,"VCInMagPlate",fMotherVolume,false,0);
  new G4PVPlacement(rotPlate,G4ThreeVector(0.,-yDispl,0.),logicalVCInMagPlate,"VCInMagPlate",fMotherVolume,false,1);

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
  */

  /////////////////////////////////////////
  // Thin window flange in front of ECal //
  /////////////////////////////////////////
  /*
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
  */
    /////////////////////////
   // Main vacuum chamber //
  /////////////////////////

  printf("Creating global VC volume\n");
  //G4UnionSolid* solidGlobVC = CreateVCGlobalSolid();
  G4UnionSolid* solidGlobVC = CreateVCFacetGlobalSolid();
  fGlobalLogicalVolume = new G4LogicalVolume(solidGlobVC,G4Material::GetMaterial("Vacuum"),"ChamberGlobal",0,0,0);
  fGlobalLogicalVolume->SetVisAttributes(G4VisAttributes::Invisible);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),fGlobalLogicalVolume,"VacuumChamber",fMotherVolume,false,0);

  printf("Creating external VC volume\n");
  //G4UnionSolid* solidExtVC = CreateVCExternalSolid();
  G4UnionSolid* solidExtVC = CreateVCFacetExternalSolid();
  //fExternalLogicalVolume = new G4LogicalVolume(solidExtVC,G4Material::GetMaterial("G4_STAINLESS-STEEL"),
  //					       "VCExternal",0,0,0);
  //fExternalLogicalVolume->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));
  //new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),fExternalLogicalVolume,"ChamberExternal",fGlobalLogicalVolume,false,0);

  printf("Creating internal VC volume\n");
  //G4UnionSolid* solidIntVC = CreateVCInternalSolid();
  G4UnionSolid* solidIntVC = CreateVCFacetInternalSolid();
  //fInternalLogicalVolume = new G4LogicalVolume(solidIntVC,G4Material::GetMaterial("Vacuum"), "VCInternal",0,0,0);
  //fInternalLogicalVolume->SetVisAttributes(G4VisAttributes::Invisible);
  //fInternalLogicalVolume->SetVisAttributes(G4VisAttributes(G4Colour::White()));
  //new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),fInternalLogicalVolume,"ChamberInternal",fMotherVolume,false,0);

  G4SubtractionSolid* solidSteelShell = new G4SubtractionSolid("ChamberSteelShell",solidExtVC,solidIntVC,0,G4ThreeVector(0.,0.,0.));
  fSteelShellLogicalVolume = new G4LogicalVolume(solidSteelShell,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"ChamberShell",0,0,0);
  fSteelShellLogicalVolume->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));
  new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),fSteelShellLogicalVolume,"ChamberShell",fGlobalLogicalVolume,false,0);

  /*
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
  */
}

G4double ChamberStructure::GetChamberMostExternalX()
{
  return ChamberGeometry::GetInstance()->GetVCMostExternalX();
}

G4double ChamberStructure::GetChamberMostAdvancedZ()
{
  return ChamberGeometry::GetInstance()->GetVCMostAdvancedZ();
}

G4UnionSolid* ChamberStructure::CreateVCGlobalSolid()
{

  // Create the main volume of the vacuum chamber

  ChamberGeometry* geo = ChamberGeometry::GetInstance();

  G4String solidTag = "VCGlobSect";
  G4String unionSolidTag = "VCGlobSolid";
  G4String solidName,unionSolidName;

  // Vertices of the front and back face of each section
  G4ThreeVector vf[4],vb[4];

  // Number of rectangular sections which define the main chamber body
  G4int nsect = geo->GetVCNSections();

  // Start from the reference section, i.e. centered at (0,0,0)
  G4int sref = geo->GetVCRefSection();
  vf[0] = geo->GetVCExtVtx(sref,0);
  vf[1] = geo->GetVCExtVtx(sref,1);
  vf[2] = geo->GetVCExtVtx(sref,2);
  vf[3] = geo->GetVCExtVtx(sref,3);
  vb[0] = geo->GetVCExtVtx(sref+1,0);
  vb[1] = geo->GetVCExtVtx(sref+1,1);
  vb[2] = geo->GetVCExtVtx(sref+1,2);
  vb[3] = geo->GetVCExtVtx(sref+1,3);

  printf("Section %d\n",sref);
  for(G4int i=0;i<4;i++) { printf("vf[%d] %f %f %f\n",i,vf[i].x(),vf[i].y(),vf[i].z()); }
  for(G4int i=0;i<4;i++) { printf("vb[%d] %f %f %f\n",i,vb[i].x(),vb[i].y(),vb[i].z()); }

  unionSolidName = unionSolidTag+"Ref";
  G4double boxSideX = vf[0].x()-vf[1].x();
  G4double boxSideY = vf[3].y()-vf[0].y();
  G4double boxSideZ = vb[0].z()-vf[0].z();
  printf("Box sides %f %f %f\n",boxSideX,boxSideY,boxSideZ);
  G4Box* solidVCRef = new G4Box(unionSolidName,0.5*boxSideX,0.5*boxSideY,0.5*boxSideZ);

  //G4LogicalVolume* logicalVC = new G4LogicalVolume(solidVCRef,G4Material::GetMaterial("Vacuum"), "VC",0,0,0);
  //new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicalVC,"VC",fMotherVolume,false,0);

  std::vector<G4UnionSolid*> solidVC(nsect);
  G4int isolid = 0;
  for(G4int is = 0; is<nsect-1; is++) {

    //printf("Adding section %d as solid %d\n",is,isolid);
    if (is != sref) {

      // Get vertices coordinates from geometry
      vf[0] = geo->GetVCExtVtx(is,0);
      vf[1] = geo->GetVCExtVtx(is,1);
      vf[2] = geo->GetVCExtVtx(is,2);
      vf[3] = geo->GetVCExtVtx(is,3);
      vb[0] = geo->GetVCExtVtx(is+1,0);
      vb[1] = geo->GetVCExtVtx(is+1,1);
      vb[2] = geo->GetVCExtVtx(is+1,2);
      vb[3] = geo->GetVCExtVtx(is+1,3);

      // Check if this section has depth
      if (vf[0].z() != vb[0].z()) {

	solidName = solidTag+G4UIcommand::ConvertToString(is);
	unionSolidName = unionSolidTag+G4UIcommand::ConvertToString(isolid);

	printf("Section %d\n",is);
	for(G4int i=0;i<4;i++) { printf("vf[%d] %f %f %f\n",i,vf[i].x(),vf[i].y(),vf[i].z()); }
	for(G4int i=0;i<4;i++) { printf("vb[%d] %f %f %f\n",i,vb[i].x(),vb[i].y(),vb[i].z()); }

	// Check if it is a box or a trap
	if ( (vf[0].x() == vb[0].x()) && (vf[1].x() == vb[1].x()) &&
	     (vf[0].y() == vb[0].y()) && (vf[3].y() == vb[3].y()) ) {

	  // It's a box
	  //printf("It's a box\n");
	  G4double boxSideX = vf[0].x()-vf[1].x();
	  G4double boxSideY = vf[3].y()-vf[0].y();
	  G4double boxSideZ = vb[0].z()-vf[0].z();
	  printf("Box sides %f %f %f\n",boxSideX,boxSideY,boxSideZ);
	  //G4Box* box = new G4Box(solidName,0.5*abs(vf[1].x()-vf[0].x()),0.5*abs(vf[3].y()-vf[0].y()),0.5*abs(vb[0].z()-vf[0].z()));
	  G4Box* box = new G4Box(solidName,0.5*boxSideX,0.5*boxSideY,0.5*boxSideZ);
	  G4ThreeVector boxPos = G4ThreeVector(0.5*(vf[1].x()+vf[0].x()),0.5*(vf[3].y()+vf[0].y()),0.5*(vb[0].z()+vf[0].z()));
	  printf("boxPos %f %f %f\n",boxPos.x(),boxPos.y(),boxPos.z());
	  //G4LogicalVolume* logicalVC = new G4LogicalVolume(box,G4Material::GetMaterial("Vacuum"),solidName,0,0,0);
	  //new G4PVPlacement(0,boxPos,logicalVC,solidName,fMotherVolume,false,0);

	  // Attach this solid to the existing structure
	  if (isolid==0) {
	    solidVC[isolid] = new G4UnionSolid(unionSolidName,solidVCRef,box,0,boxPos);
	  } else {
	    solidVC[isolid] = new G4UnionSolid(unionSolidName,solidVC[isolid-1],box,0,boxPos);
	  }
	  isolid++;

	} else {

	  // It's a trap
	  //printf("It's a trap\n");
	  std::vector<G4TwoVector> vtx;
	  for(G4int i=0; i<4; i++) { vtx.push_back(G4TwoVector(vf[i].x(),vf[i].y())); }
	  for(G4int i=0; i<4; i++) { vtx.push_back(G4TwoVector(vb[i].x(),vb[i].y())); }
	  for(G4int i=0; i<8; i++) { printf("vtx[%d] %f %f\n",i,vtx[i].x(),vtx[i].y()); }
	  G4double lenZ = vb[0].z()-vf[0].z();
	  G4GenericTrap* trap = new G4GenericTrap(solidName,0.5*lenZ,vtx);
	  G4ThreeVector trapPos = G4ThreeVector(0.,0.,0.5*(vb[0].z()+vf[0].z()));
	  //G4LogicalVolume* logicalVC = new G4LogicalVolume(trap,G4Material::GetMaterial("Vacuum"),solidName,0,0,0);
	  //new G4PVPlacement(0,trapPos,logicalVC,solidName,fMotherVolume,false,0);

	  // Attach this solid to the existing structure
	  if (isolid==0) {
	    solidVC[isolid] = new G4UnionSolid(unionSolidName,solidVCRef,trap,0,trapPos);
	  } else {
	    solidVC[isolid] = new G4UnionSolid(unionSolidName,solidVC[isolid-1],trap,0,trapPos);
	  }
	  isolid++;

	}

	//} else {
	//	printf("This section has no depth: no solid associated\n");
      }

      //} else {
      //printf("This is the reference section: skipping\n");
    }

  }

  // Big cylinder
  G4double cyLen = geo->GetVCCLength();
  //G4double cyThick = geo->GetVCCThick();
  //G4double cyRIn = geo->GetVCCRIn();
  G4double cyROut = geo->GetVCCROut();
  G4double cyPosZ = geo->GetVCCPosZ();
  //printf("Cylinder %f %f %f %f %f\n",cyLen,cyThick,cyRIn,cyROut,cyPosZ);
  G4Tubs* solidCyl = new G4Tubs("VCCylGlob",0.,cyROut,0.5*cyLen,0.*deg,360.*deg);

  //G4LogicalVolume* logicalCyl = new G4LogicalVolume(solidCyl,G4Material::GetMaterial("Vacuum"),"Cyl",0,0,0);
  //new G4PVPlacement(0,G4ThreeVector(0.,0.,cyPosZ),logicalCyl,"Cyl",fMotherVolume,false,0);

  // Flange towards ECal thin window
  G4double flaThick = geo->GetVCCFThick();
  //G4double flaRIn = geo->GetVCCFRIn();
  G4double flaROut = geo->GetVCCFROut();
  G4double flaPosZ = geo->GetVCCFPosZ();
  G4Tubs* solidFla = new G4Tubs("VCFlaGlob",0.,flaROut,0.5*flaThick,0.*deg,360.*deg);

  // Attach cylinder and flange to vacuum chamber
  G4UnionSolid* solid0 = new G4UnionSolid("VCGlobal0",solidVC[isolid-1],solidCyl,0,G4ThreeVector(0.,0.,cyPosZ));
  //G4UnionSolid* solid0 = new G4UnionSolid("VCGlobal0",solidVCRef,solidCyl,0,G4ThreeVector(0.,0.,cyPosZ));
  //G4UnionSolid* solid0 = new G4UnionSolid("VCGlobal0",solidVC[2],solidCyl,0,G4ThreeVector(0.,0.,cyPosZ));
  G4UnionSolid* solidFinal = new G4UnionSolid("ChamberGlobal",solid0,solidFla,0,G4ThreeVector(0.,0.,flaPosZ));

  return solidFinal;
  //return solidVC[isolid-1];
  //return solidVC[0];

}

G4UnionSolid* ChamberStructure::CreateVCExternalSolid()
{

  // Create the external volume of the vacuum chamber

  ChamberGeometry* geo = ChamberGeometry::GetInstance();

  G4String solidTag = "VCExtSect";
  G4String unionSolidTag = "VCExtSolid";
  G4String solidName,unionSolidName;

  // Vertices of the front and back face of each section
  G4ThreeVector vf[4],vb[4];

  // Number of rectangular sections which define the main chamber body
  G4int nsect = geo->GetVCNSections();

  // Start from the reference section, i.e. centered at (0,0,0)
  G4int sref = geo->GetVCRefSection();
  // Shrink the section by 1um along X and Y wrt the global volume to avoid face overlaps
  vf[0] = geo->GetVCExtVtx(sref,0)+G4ThreeVector(-1.*um, 1.*um,0.);
  vf[1] = geo->GetVCExtVtx(sref,1)+G4ThreeVector( 1.*um, 1.*um,0.);
  vf[2] = geo->GetVCExtVtx(sref,2)+G4ThreeVector( 1.*um,-1.*um,0.);
  vf[3] = geo->GetVCExtVtx(sref,3)+G4ThreeVector(-1.*um,-1.*um,0.);
  vb[0] = geo->GetVCExtVtx(sref+1,0)+G4ThreeVector(-1.*um, 1.*um,0.);
  vb[1] = geo->GetVCExtVtx(sref+1,1)+G4ThreeVector( 1.*um, 1.*um,0.);
  vb[2] = geo->GetVCExtVtx(sref+1,2)+G4ThreeVector( 1.*um,-1.*um,0.);
  vb[3] = geo->GetVCExtVtx(sref+1,3)+G4ThreeVector(-1.*um,-1.*um,0.);

  //printf("%d %f %f %f %f %f %f\n",sref,vf[1].x(),vf[0].x(),vf[2].y(),vf[1].y(),vb[0].z(),vf[0].z());
  unionSolidName = unionSolidTag+"Ref";
  G4Box* solidVCRef = new G4Box(unionSolidName,0.5*abs(vf[1].x()-vf[0].x()),0.5*abs(vf[2].y()-vf[1].y()),0.5*abs(vb[0].z()-vf[0].z()));

  //G4LogicalVolume* logicalVC = new G4LogicalVolume(solidVCRef,G4Material::GetMaterial("Vacuum"), "VC",0,0,0);
  //new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicalVC,"VC",fMotherVolume,false,0);

  std::vector<G4UnionSolid*> solidVC(nsect);
  G4int isolid = 0;
  for(G4int is = 0; is<nsect-1; is++) {

    //printf("Adding section %d as solid %d\n",is,isolid);
    if (is != sref) {

      // Get vertices coordinates from geometry
      // Shrink the section by 1um along X and Y wrt the global volume to avoid face overlaps
      vf[0] = geo->GetVCExtVtx(is,0)+G4ThreeVector(-1.*um, 1.*um,0.);
      vf[1] = geo->GetVCExtVtx(is,1)+G4ThreeVector( 1.*um, 1.*um,0.);
      vf[2] = geo->GetVCExtVtx(is,2)+G4ThreeVector( 1.*um,-1.*um,0.);
      vf[3] = geo->GetVCExtVtx(is,3)+G4ThreeVector(-1.*um,-1.*um,0.);
      vb[0] = geo->GetVCExtVtx(is+1,0)+G4ThreeVector(-1.*um, 1.*um,0.);
      vb[1] = geo->GetVCExtVtx(is+1,1)+G4ThreeVector( 1.*um, 1.*um,0.);
      vb[2] = geo->GetVCExtVtx(is+1,2)+G4ThreeVector( 1.*um,-1.*um,0.);
      vb[3] = geo->GetVCExtVtx(is+1,3)+G4ThreeVector(-1.*um,-1.*um,0.);

      // Shrink first and last sector by 1um along Z wrt the global volume to avoid face overlaps
      if (is==0) {
	vf[0] += G4ThreeVector(0.,0.,1.*um);
	vf[1] += G4ThreeVector(0.,0.,1.*um);
	vf[2] += G4ThreeVector(0.,0.,1.*um);
	vf[3] += G4ThreeVector(0.,0.,1.*um);
      } else if (is==nsect-2) {
	vb[0] += G4ThreeVector(0.,0.,-1.*um);
	vb[1] += G4ThreeVector(0.,0.,-1.*um);
	vb[2] += G4ThreeVector(0.,0.,-1.*um);
	vb[3] += G4ThreeVector(0.,0.,-1.*um);
      }

      // Check if this section has depth
      if (vf[0].z() != vb[0].z()) {

	solidName = solidTag+G4UIcommand::ConvertToString(is);
	unionSolidName = unionSolidTag+G4UIcommand::ConvertToString(isolid);

	printf("Section %d\n",is);
	for(G4int i=0;i<4;i++) { printf("vf[%d] %f %f %f\n",i,vf[i].x(),vf[i].y(),vf[i].z()); }
	for(G4int i=0;i<4;i++) { printf("vb[%d] %f %f %f\n",i,vb[i].x(),vb[i].y(),vb[i].z()); }

	// Check if it is a box or a trap
	if ( (vf[0].x() == vb[0].x()) && (vf[1].x() == vb[1].x()) &&
	     (vf[0].y() == vb[0].y()) && (vf[3].y() == vb[3].y()) ) {

	  // It's a box
	  //printf("It's a box\n");
	  G4double boxSideX = vf[0].x()-vf[1].x();
	  G4double boxSideY = vf[3].y()-vf[0].y();
	  G4double boxSideZ = vb[0].z()-vf[0].z();
	  printf("Box sides %f %f %f\n",boxSideX,boxSideY,boxSideZ);
	  //G4Box* box = new G4Box(solidName,0.5*abs(vf[1].x()-vf[0].x()),0.5*abs(vf[3].y()-vf[0].y()),0.5*abs(vb[0].z()-vf[0].z()));
	  G4Box* box = new G4Box(solidName,0.5*boxSideX,0.5*boxSideY,0.5*boxSideZ);
	  G4ThreeVector boxPos = G4ThreeVector(0.5*(vf[1].x()+vf[0].x()),0.5*(vf[3].y()+vf[0].y()),0.5*(vb[0].z()+vf[0].z()));
	  printf("boxPos %f %f %f\n",boxPos.x(),boxPos.y(),boxPos.z());
	  //G4LogicalVolume* logicalVC = new G4LogicalVolume(box,G4Material::GetMaterial("Vacuum"),solidName,0,0,0);
	  //new G4PVPlacement(0,boxPos,logicalVC,solidName,fMotherVolume,false,0);

	  // Attach this solid to the existing structure
	  if (isolid==0) {
	    solidVC[isolid] = new G4UnionSolid(unionSolidName,solidVCRef,box,0,boxPos);
	  } else {
	    solidVC[isolid] = new G4UnionSolid(unionSolidName,solidVC[isolid-1],box,0,boxPos);
	  }
	  isolid++;

	} else {

	  // It's a trap
	  //printf("It's a trap\n");
	  std::vector<G4TwoVector> vtx;
	  for(G4int i=0; i<4; i++) { vtx.push_back(G4TwoVector(vf[i].x(),vf[i].y())); }
	  for(G4int i=0; i<4; i++) { vtx.push_back(G4TwoVector(vb[i].x(),vb[i].y())); }
	  for(G4int i=0; i<8; i++) { printf("vtx[%d] %f %f\n",i,vtx[i].x(),vtx[i].y()); }
	  G4double lenZ = vb[0].z()-vf[0].z();
	  G4GenericTrap* trap = new G4GenericTrap(solidName,0.5*lenZ,vtx);
	  G4ThreeVector trapPos = G4ThreeVector(0.,0.,0.5*(vb[0].z()+vf[0].z()));
	  //G4LogicalVolume* logicalVC = new G4LogicalVolume(trap,G4Material::GetMaterial("Vacuum"),solidName,0,0,0);
	  //new G4PVPlacement(0,trapPos,logicalVC,solidName,fMotherVolume,false,0);

	  // Attach this solid to the existing structure
	  if (isolid==0) {
	    solidVC[isolid] = new G4UnionSolid(unionSolidName,solidVCRef,trap,0,trapPos);
	  } else {
	    solidVC[isolid] = new G4UnionSolid(unionSolidName,solidVC[isolid-1],trap,0,trapPos);
	  }
	  isolid++;

	}

	//} else {
	//printf("This section has no depth: no solid associated\n");
      }

      //} else {
      //printf("This is the reference section: skipping\n");
    }

  }

  // Big cylinder
  G4double cyLen = geo->GetVCCLength();
  //G4double cyThick = geo->GetVCCThick();
  //G4double cyRIn = geo->GetVCCRIn();
  G4double cyROut = geo->GetVCCROut();
  // Shift cylinder 1um forward to connect to flange
  G4double cyPosZ = geo->GetVCCPosZ()+0.001*mm;
  //printf("Cylinder %f %f %f %f %f\n",cyLen,cyThick,cyRIn,cyROut,cyPosZ);
  // Shrink radius of cylinder by 1um wrt global volume to avoid face overlaps
  G4Tubs* solidCyl = new G4Tubs("VCTubsExt",0.,cyROut-0.001*mm,0.5*cyLen,0.*deg,360.*deg);

  //G4LogicalVolume* logicalCyl = new G4LogicalVolume(solidCyl,G4Material::GetMaterial("Vacuum"),"Cyl",0,0,0);
  //new G4PVPlacement(0,G4ThreeVector(0.,0.,cyPosZ),logicalCyl,"Cyl",fMotherVolume,false,0);

  // Flange towards ECal thin window
  G4double flaThick = geo->GetVCCFThick();
  //G4double flaRIn = geo->GetVCCFRIn();
  G4double flaROut = geo->GetVCCFROut();
  G4double flaPosZ = geo->GetVCCFPosZ();
  // Shrink radius and thickness of flange by 1um wrt global volume to avoid face overlaps
  G4Tubs* solidFla = new G4Tubs("VCFlaGlob",0.,flaROut-0.001*mm,0.5*flaThick-0.001*mm,0.*deg,360.*deg);

  // Attach cylinder and flange to vacuum chamber
  G4UnionSolid* solid0 = new G4UnionSolid("VCExternal0",solidVC[isolid-1],solidCyl,0,G4ThreeVector(0.,0.,cyPosZ));
  //G4UnionSolid* solid0 = new G4UnionSolid("VCExternal0",solidVCRef,solidCyl,0,G4ThreeVector(0.,0.,cyPosZ));
  //G4UnionSolid* solid0 = new G4UnionSolid("VCExternal0",solidVC[2],solidCyl,0,G4ThreeVector(0.,0.,cyPosZ));
  G4UnionSolid* solidFinal = new G4UnionSolid("ChamberExternal",solid0,solidFla,0,G4ThreeVector(0.,0.,flaPosZ));

  return solidFinal;

}

G4UnionSolid* ChamberStructure::CreateVCInternalSolid()
{

  ChamberGeometry* geo = ChamberGeometry::GetInstance();

  G4String solidTag = "VCIntSect";
  G4String unionSolidTag = "VCIntSolid";
  G4String solidName,unionSolidName;

  // Vertices of the front and back face of each section
  G4ThreeVector vf[4],vb[4];

  // Number of rectangular sections which define the main chamber body
  G4int nsect = geo->GetVCNSections();

  // Start from the reference section, i.e. centered at (0,0,0)
  // Make z coordinate of the two faces sliiiiiightly far apart to guarantee overlap
  G4int sref = geo->GetVCRefSection();
  vf[0] = geo->GetVCIntVtx(sref,0);
  vf[1] = geo->GetVCIntVtx(sref,1);
  vf[2] = geo->GetVCIntVtx(sref,2);
  vf[3] = geo->GetVCIntVtx(sref,3);
  vb[0] = geo->GetVCIntVtx(sref+1,0);
  vb[1] = geo->GetVCIntVtx(sref+1,1);
  vb[2] = geo->GetVCIntVtx(sref+1,2);
  vb[3] = geo->GetVCIntVtx(sref+1,3);

  printf("Section %d\n",sref);
  for(G4int i=0;i<4;i++) { printf("vf[%d] %f %f %f\n",i,vf[i].x(),vf[i].y(),vf[i].z()); }
  for(G4int i=0;i<4;i++) { printf("vb[%d] %f %f %f\n",i,vb[i].x(),vb[i].y(),vb[i].z()); }

  unionSolidName = unionSolidTag+"Ref";
  G4double boxSideX = vf[0].x()-vf[1].x();
  G4double boxSideY = vf[3].y()-vf[0].y();
  G4double boxSideZ = vb[0].z()-vf[0].z();
  printf("Box sides %f %f %f\n",boxSideX,boxSideY,boxSideZ);
  G4Box* solidVCRef = new G4Box(unionSolidName,0.5*boxSideX,0.5*boxSideY,0.5*boxSideZ);

  //G4LogicalVolume* logicalVC = new G4LogicalVolume(solidVCRef,G4Material::GetMaterial("Vacuum"), "VC",0,0,0);
  //new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicalVC,"VC",fMotherVolume,false,0);

  std::vector<G4UnionSolid*> solidVC(nsect);
  G4int isolid = 0;
  for(G4int is = 0; is<nsect-1; is++) {

    //printf("Adding section %d as solid %d\n",is,isolid);
    if (is != sref) {

      // Get vertices coordinates from geometry
      vf[0] = geo->GetVCIntVtx(is,0);
      vf[1] = geo->GetVCIntVtx(is,1);
      vf[2] = geo->GetVCIntVtx(is,2);
      vf[3] = geo->GetVCIntVtx(is,3);
      vb[0] = geo->GetVCIntVtx(is+1,0);
      vb[1] = geo->GetVCIntVtx(is+1,1);
      vb[2] = geo->GetVCIntVtx(is+1,2);
      vb[3] = geo->GetVCIntVtx(is+1,3);

      // Check if this section has depth
      if (vf[0].z() != vb[0].z()) {

	solidName = solidTag+G4UIcommand::ConvertToString(is);
	unionSolidName = unionSolidTag+G4UIcommand::ConvertToString(isolid);

	printf("Section %d\n",is);
	for(G4int i=0;i<4;i++) { printf("vf[%d] %f %f %f\n",i,vf[i].x(),vf[i].y(),vf[i].z()); }
	for(G4int i=0;i<4;i++) { printf("vb[%d] %f %f %f\n",i,vb[i].x(),vb[i].y(),vb[i].z()); }

	// Check if it is a box or a trap
	if ( (vf[0].x() == vb[0].x()) && (vf[1].x() == vb[1].x()) &&
	     (vf[0].y() == vb[0].y()) && (vf[3].y() == vb[3].y()) ) {

	  // It's a box
	  //printf("It's a box\n");
	  //printf("It's a box\n");
	  G4double boxSideX = vf[0].x()-vf[1].x();
	  G4double boxSideY = vf[3].y()-vf[0].y();
	  G4double boxSideZ = vb[0].z()-vf[0].z();
	  printf("Box sides %f %f %f\n",boxSideX,boxSideY,boxSideZ);
	  //G4Box* box = new G4Box(solidName,0.5*abs(vf[1].x()-vf[0].x()),0.5*abs(vf[3].y()-vf[0].y()),0.5*abs(vb[0].z()-vf[0].z()));
	  G4Box* box = new G4Box(solidName,0.5*boxSideX,0.5*boxSideY,0.5*boxSideZ);
	  G4ThreeVector boxPos = G4ThreeVector(0.5*(vf[1].x()+vf[0].x()),0.5*(vf[3].y()+vf[0].y()),0.5*(vb[0].z()+vf[0].z()));
	  printf("boxPos %f %f %f\n",boxPos.x(),boxPos.y(),boxPos.z());
	  //G4LogicalVolume* logicalVC = new G4LogicalVolume(box,G4Material::GetMaterial("Vacuum"),solidName,0,0,0);
	  //new G4PVPlacement(0,boxPos,logicalVC,solidName,fMotherVolume,false,0);

	  // Attach this solid to the existing structure
	  if (isolid==0) {
	    solidVC[isolid] = new G4UnionSolid(unionSolidName,solidVCRef,box,0,boxPos);
	  } else {
	    solidVC[isolid] = new G4UnionSolid(unionSolidName,solidVC[isolid-1],box,0,boxPos);
	  }
	  isolid++;

	} else {

	  // It's a trap
	  //printf("It's a trap\n");
	  std::vector<G4TwoVector> vtx;
	  for(G4int i=0; i<4; i++) { vtx.push_back(G4TwoVector(vf[i].x(),vf[i].y())); }
	  for(G4int i=0; i<4; i++) { vtx.push_back(G4TwoVector(vb[i].x(),vb[i].y())); }
	  for(G4int i=0; i<8; i++) { printf("vtx[%d] %f %f\n",i,vtx[i].x(),vtx[i].y()); }
	  G4double lenZ = vb[0].z()-vf[0].z();
	  G4GenericTrap* trap = new G4GenericTrap(solidName,0.5*lenZ,vtx);
	  G4ThreeVector trapPos = G4ThreeVector(0.,0.,0.5*(vb[0].z()+vf[0].z()));
	  //G4LogicalVolume* logicalVC = new G4LogicalVolume(trap,G4Material::GetMaterial("Vacuum"), solidName,0,0,0);
	  //new G4PVPlacement(0,trapPos,logicalVC,solidName,fMotherVolume,false,0);

	  // Attach this solid to the existing structure
	  if (isolid==0) {
	    solidVC[isolid] = new G4UnionSolid(unionSolidName,solidVCRef,trap,0,trapPos);
	  } else {
	    solidVC[isolid] = new G4UnionSolid(unionSolidName,solidVC[isolid-1],trap,0,trapPos);
	  }
	  isolid++;

	}

	//} else {
	//printf("This section has no depth: no solid associated\n");
      }

      //} else {
      //printf("This is the reference section: skipping\n");
    }

  }

  // Big cylinder
  G4double cyThick = geo->GetVCCThick();
  G4double cyRIn = geo->GetVCCRIn();
  //G4double cyROut = geo->GetVCCROut();
  G4double cyLen = geo->GetVCCLength()-cyThick+0.001*mm;
  G4double cyPosZ = geo->GetVCCPosZ()+0.5*(cyThick+0.001*mm);
  //printf("Cylinder %f %f %f %f %f\n",cyLen,cyThick,cyRIn,cyROut,cyPosZ);
  G4Tubs* solidCyl = new G4Tubs("VCTubsInt",0.,cyRIn,0.5*cyLen,0.*deg,360.*deg);

  //G4LogicalVolume* logicalCyl = new G4LogicalVolume(solidCyl,G4Material::GetMaterial("Vacuum"),"Cyl",0,0,0);
  //new G4PVPlacement(0,G4ThreeVector(0.,0.,cyPosZ),logicalCyl,"Cyl",fMotherVolume,false,0);

  // Flange towards ECal thin window
  G4double flaThick = geo->GetVCCFThick();
  G4double flaRIn = geo->GetVCCFRIn();
  //G4double flaROut = geo->GetVCCFROut();
  G4double flaPosZ = geo->GetVCCFPosZ();
  // Shrink thickness of flange by 1um wrt global volume to be identical to external flange
  //G4Tubs* solidFla = new G4Tubs("VCFlaGlob",0.,flaRIn,0.5*flaThick-0.001*mm,0.*deg,360.*deg);
  G4Tubs* solidFla = new G4Tubs("VCFlaGlob",0.,flaRIn,0.5*flaThick,0.*deg,360.*deg);

  // Create hole at beam entrance (will need flange to connect to target structure)
  G4double holeThick = geo->GetVCInHoleThick()+2.*um;
  G4double holeRadius = geo->GetVCInHoleRadius();
  G4double holePosZ = geo->GetVCInHolePosZ();
  G4Tubs* solidInHole = new G4Tubs("VCInHole",0.,holeRadius,0.5*holeThick,0.*deg,360.*deg);

  // Create hole at beam exit (will need flange to connect to thin TPix window)
  G4double hoT = geo->GetVCOutHoleThick();
  G4double hoR = geo->GetVCOutHoleRadius();
  G4double hoL = geo->GetVCOutHoleLength();
  G4double hoD = geo->GetVCOutHoleDistToEdge();
  G4double hoA = geo->GetVCBackFaceAngle();
  G4Box* solidHO0 = new G4Box("VCHOut0",0.5*hoL,hoR,0.5*hoT+2.*um);
  G4Tubs* solidHO1 = new G4Tubs("VCHOut1",0.,hoR,0.5*hoT+2.*um,0.*deg,360.*deg);
  G4UnionSolid* solidHO2 = new G4UnionSolid("VCHOut2",solidHO0,solidHO1,0,G4ThreeVector(-0.5*hoL-0.7*mm,0.,0.));
  G4UnionSolid* solidOutHole = new G4UnionSolid("VCHOut3",solidHO2,solidHO1,0,G4ThreeVector(0.5*hoL+0.7*mm,0.,0.));
  G4RotationMatrix* rotHOut = new G4RotationMatrix;
  rotHOut->rotateY(hoA);
  G4ThreeVector hoEdge = geo->GetVCExtVtx(11,1);
  G4double hoCX = hoEdge.x()-hoD*cos(hoA)+0.5*hoT*sin(hoA);
  G4double hoCZ = hoEdge.z()-hoD*sin(hoA)-0.5*hoT*cos(hoA);
  G4ThreeVector posHOut = G4ThreeVector(hoCX,0.,hoCZ);
  //printf("Exit hole %f %f %f %f %f %f %f\n",hoT,hoR,hoL,hoD,hoA,hoCX,hoCZ);

  //G4LogicalVolume* logicalHOut = new G4LogicalVolume(solidHO2,G4Material::GetMaterial("Vacuum"),"VOutHole",0,0,0);
  //new G4PVPlacement(rotHOut,posHOut,logicalHOut,"VOutHole",fMotherVolume,false,0);

  // Attach cylinder and flange to vacuum chamber
  G4UnionSolid* solid0 = new G4UnionSolid("VCInternal0",solidVC[isolid-1],solidCyl,0,G4ThreeVector(0.,0.,cyPosZ));
  //G4UnionSolid* solid0 = new G4UnionSolid("VCInternal0",solidVCRef,solidCyl,0,G4ThreeVector(0.,0.,cyPosZ));
  //G4UnionSolid* solid0 = new G4UnionSolid("VCInternal0",solidVC[2],solidCyl,0,G4ThreeVector(0.,0.,cyPosZ));
  G4UnionSolid* solid1 = new G4UnionSolid("VCInternal1",solid0,solidFla,0,G4ThreeVector(0.,0.,flaPosZ));
  G4UnionSolid* solid2 = new G4UnionSolid("VCInternal2",solid1,solidOutHole,rotHOut,posHOut);
  //G4UnionSolid* solid2 = new G4UnionSolid("VCInternal2",solid1,solidHO2,rotHOut,posHOut);
  G4UnionSolid* solidFinal = new G4UnionSolid("ChamberInternal",solid2,solidInHole,0,G4ThreeVector(0.,0.,holePosZ));

  return solidFinal;

}

G4UnionSolid* ChamberStructure::CreateVCFacetGlobalSolid()
{

  // Create the main volume of the vacuum chamber

  ChamberGeometry* geo = ChamberGeometry::GetInstance();

  // Create main VC volume as a tassellated solid
  G4TessellatedSolid* solidMain = new G4TessellatedSolid("VCGlobMain");

  G4int nFacets = geo->GetVCNFacets();
  for(G4int f=0; f<nFacets; f++) {
    G4ThreeVector vtx[4];
    for(G4int v=0; v<4; v++) {
      G4int ivtx = geo->GetVCFacetVtx(f,v);
      if (ivtx != -1) {
	G4int is = ivtx/10; // Section id
	G4int iv = ivtx%10; // Vertex id
	vtx[v] = geo->GetVCExtVtx(is,iv);
      } else {
	vtx[v] = G4ThreeVector(0.,0.,0.); // The vacuum chamber structure cannot go through the origin
      }
    }
    if ( (vtx[3].x() == 0.) && (vtx[3].x() == 0.) && (vtx[3].x() == 0.) ) {
      G4TriangularFacet* facet = new G4TriangularFacet(vtx[0],vtx[1],vtx[2],ABSOLUTE);
      solidMain->AddFacet((G4VFacet*)facet);
    } else {
      G4QuadrangularFacet* facet = new G4QuadrangularFacet(vtx[0],vtx[1],vtx[2],vtx[3],ABSOLUTE);
      solidMain->AddFacet((G4VFacet*)facet);
    }
  }
  solidMain->SetSolidClosed(true);

  //G4cout << "Dump of solid:\n" << solidMain << G4endl;
  //G4Polyhedron* polyhedron = solidMain->GetPolyhedron();
  //G4cout << "\nLocal polyhedron coordinates:\n" << *polyhedron << G4endl;

  //G4LogicalVolume* logicalMain = new G4LogicalVolume(solidMain,G4Material::GetMaterial("Vacuum"),"VCGlobMain",0,0,0);
  //new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicalMain,"VCGlobMain",fMotherVolume,false,0);

  // Big cylinder
  G4double cyLen = geo->GetVCCLength();
  //G4double cyThick = geo->GetVCCThick();
  //G4double cyRIn = geo->GetVCCRIn();
  G4double cyROut = geo->GetVCCROut();
  G4double cyPosZ = geo->GetVCCPosZ();
  //printf("Cylinder %f %f %f %f %f\n",cyLen,cyThick,cyRIn,cyROut,cyPosZ);
  G4Tubs* solidCyl = new G4Tubs("VCCylGlob",0.,cyROut,0.5*cyLen,0.*deg,360.*deg);

  //G4LogicalVolume* logicalCyl = new G4LogicalVolume(solidCyl,G4Material::GetMaterial("Vacuum"),"Cyl",0,0,0);
  //new G4PVPlacement(0,G4ThreeVector(0.,0.,cyPosZ),logicalCyl,"Cyl",fMotherVolume,false,0);

  // Flange towards ECal thin window (includes thin window and its flange)
  G4double flaLen = geo->GetVCMostAdvancedZ()-(cyPosZ+0.5*cyLen);
  //G4double flaThick = geo->GetVCCFThick();
  //G4double flaRIn = geo->GetVCCFRIn();
  G4double flaROut = geo->GetVCCFROut();
  //G4double flaPosZ = geo->GetVCCFPosZ();
  G4double flaPosZ = cyPosZ+0.5*cyLen+0.5*flaLen;
  //G4Tubs* solidFla = new G4Tubs("VCFlaGlob",0.,flaROut,0.5*flaThick,0.*deg,360.*deg);
  G4Tubs* solidFla = new G4Tubs("VCFlaGlob",0.,flaROut,0.5*flaLen,0.*deg,360.*deg);

  // Attach cylinder and flange to vacuum chamber
  G4UnionSolid* solid0 = new G4UnionSolid("VCGlobal0",solidMain,solidCyl,0,G4ThreeVector(0.,0.,cyPosZ));
  G4UnionSolid* solidFinal = new G4UnionSolid("ChamberGlobal",solid0,solidFla,0,G4ThreeVector(0.,0.,flaPosZ));
  //G4UnionSolid* solidFinal = new G4UnionSolid("ChamberGlobal",solidMain,solidCyl,0,G4ThreeVector(0.,0.,cyPosZ));

  return solidFinal;

}

G4UnionSolid* ChamberStructure::CreateVCFacetExternalSolid()
{

  // Create the main volume of the vacuum chamber

  ChamberGeometry* geo = ChamberGeometry::GetInstance();

  // Shrinking factor
  G4ThreeVector vShrink[4];
  vShrink[0] = G4ThreeVector(-1.*um, 1.*um,0.);
  vShrink[1] = G4ThreeVector( 1.*um, 1.*um,0.);
  vShrink[2] = G4ThreeVector( 1.*um,-1.*um,0.);
  vShrink[3] = G4ThreeVector(-1.*um,-1.*um,0.);

  // Create main VC volume as a tassellated solid
  G4TessellatedSolid* solidMain = new G4TessellatedSolid("VCExtMain");

  G4int nFacets = geo->GetVCNFacets();
  for(G4int f=0; f<nFacets; f++) {
    G4ThreeVector vtx[4];
    for(G4int v=0; v<4; v++) {
      G4int ivtx = geo->GetVCFacetVtx(f,v);
      if (ivtx != -1) {
	G4int is = ivtx/10; // Section id
	G4int iv = ivtx%10; // Vertex id
	// Shrink the whole structure by 1um to avoid syrface overlap with VC global container
	vtx[v] = geo->GetVCExtVtx(is,iv)+vShrink[iv];
	if (is==0 || is == 4 || is == 5) { vtx[v] += G4ThreeVector(0.,0.,1.*um); }
	if (is==11) { vtx[v] += G4ThreeVector(0.,0.,-1.*um); }
      } else {
	vtx[v] = G4ThreeVector(0.,0.,0.); // The vacuum chamber structure cannot go through the origin
      }
    }
    if ( (vtx[3].x() == 0.) && (vtx[3].x() == 0.) && (vtx[3].x() == 0.) ) {
      G4TriangularFacet* facet = new G4TriangularFacet(vtx[0],vtx[1],vtx[2],ABSOLUTE);
      solidMain->AddFacet((G4VFacet*)facet);
    } else {
      G4QuadrangularFacet* facet = new G4QuadrangularFacet(vtx[0],vtx[1],vtx[2],vtx[3],ABSOLUTE);
      solidMain->AddFacet((G4VFacet*)facet);
    }
  }
  solidMain->SetSolidClosed(true);

  //G4cout << "Dump of solid:\n" << solidMain << G4endl;
  //G4Polyhedron* polyhedron = solidMain->GetPolyhedron();
  //G4cout << "\nLocal polyhedron coordinates:\n" << *polyhedron << G4endl;

  //G4LogicalVolume* logicalMain = new G4LogicalVolume(solidMain,G4Material::GetMaterial("Vacuum"),"VCGlobMain",0,0,0);
  //new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicalMain,"VCGlobMain",fMotherVolume,false,0);

  // Big cylinder
  G4double cyLen = geo->GetVCCLength();
  //G4double cyThick = geo->GetVCCThick();
  //G4double cyRIn = geo->GetVCCRIn();
  G4double cyROut = geo->GetVCCROut()-1.*um;
  G4double cyPosZ = geo->GetVCCPosZ()+1.*um;
  //printf("Cylinder %f %f %f %f %f\n",cyLen,cyThick,cyRIn,cyROut,cyPosZ);
  G4Tubs* solidCyl = new G4Tubs("VCCylExt",0.,cyROut,0.5*cyLen,0.*deg,360.*deg);

  //G4LogicalVolume* logicalCyl = new G4LogicalVolume(solidCyl,G4Material::GetMaterial("Vacuum"),"Cyl",0,0,0);
  //new G4PVPlacement(0,G4ThreeVector(0.,0.,cyPosZ),logicalCyl,"Cyl",fMotherVolume,false,0);

  // Flange towards ECal thin window
  G4double flaLen = geo->GetVCCFThick()-2.*um;
  //G4double flaRIn = geo->GetVCCFRIn();
  G4double flaROut = geo->GetVCCFROut()-1.*um;
  //G4double flaPosZ = geo->GetVCCFPosZ();
  G4double flaPosZ = geo->GetVCCFPosZ();
  G4Tubs* solidFla = new G4Tubs("VCFlaExt",0.,flaROut,0.5*flaLen,0.*deg,360.*deg);

  // Attach cylinder and flange to vacuum chamber
  G4UnionSolid* solid0 = new G4UnionSolid("VCExt0",solidMain,solidCyl,0,G4ThreeVector(0.,0.,cyPosZ));
  G4UnionSolid* solidFinal = new G4UnionSolid("ChamberExternal",solid0,solidFla,0,G4ThreeVector(0.,0.,flaPosZ));
  //G4UnionSolid* solidFinal = new G4UnionSolid("ChamberGlobal",solidMain,solidCyl,0,G4ThreeVector(0.,0.,cyPosZ));

  return solidFinal;

}

G4UnionSolid* ChamberStructure::CreateVCFacetInternalSolid()
{

  // Create the main volume of the vacuum chamber

  ChamberGeometry* geo = ChamberGeometry::GetInstance();

  // Create main VC volume as a tassellated solid
  G4TessellatedSolid* solidMain = new G4TessellatedSolid("VCIntMain");

  G4int nFacets = geo->GetVCNFacets();
  for(G4int f=0; f<nFacets; f++) {
    G4ThreeVector vtx[4];
    for(G4int v=0; v<4; v++) {
      G4int ivtx = geo->GetVCFacetVtx(f,v);
      if (ivtx != -1) {
	G4int is = ivtx/10; // Section id
	G4int iv = ivtx%10; // Vertex id
	vtx[v] = geo->GetVCIntVtx(is,iv);
      } else {
	vtx[v] = G4ThreeVector(0.,0.,0.); // The vacuum chamber structure cannot go through the origin
      }
    }
    if ( (vtx[3].x() == 0.) && (vtx[3].x() == 0.) && (vtx[3].x() == 0.) ) {
      G4TriangularFacet* facet = new G4TriangularFacet(vtx[0],vtx[1],vtx[2],ABSOLUTE);
      solidMain->AddFacet((G4VFacet*)facet);
    } else {
      G4QuadrangularFacet* facet = new G4QuadrangularFacet(vtx[0],vtx[1],vtx[2],vtx[3],ABSOLUTE);
      solidMain->AddFacet((G4VFacet*)facet);
    }
  }
  solidMain->SetSolidClosed(true);

  G4cout << "Dump of solid:\n" << solidMain << G4endl;
  G4Polyhedron* polyhedron = solidMain->GetPolyhedron();
  G4cout << "\nLocal polyhedron coordinates:\n" << *polyhedron << G4endl;

  //G4LogicalVolume* logicalMain = new G4LogicalVolume(solidMain,G4Material::GetMaterial("Vacuum"),"VCGlobMain",0,0,0);
  //new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicalMain,"VCGlobMain",fMotherVolume,false,0);

  // Big cylinder
  G4double cyLen = geo->GetVCCLength()-geo->GetVCCThick();
  //G4double cyThick = geo->GetVCCThick();
  //G4double cyRIn = geo->GetVCCRIn();
  G4double cyROut = geo->GetVCCROut()-geo->GetVCCThick();
  G4double cyPosZ = geo->GetVCCPosZ()+0.5*geo->GetVCCThick();
  //printf("Cylinder %f %f %f %f %f\n",cyLen,cyThick,cyRIn,cyROut,cyPosZ);
  G4Tubs* solidCyl = new G4Tubs("VCCylExt",0.,cyROut,0.5*cyLen,0.*deg,360.*deg);

  //G4LogicalVolume* logicalCyl = new G4LogicalVolume(solidCyl,G4Material::GetMaterial("Vacuum"),"Cyl",0,0,0);
  //new G4PVPlacement(0,G4ThreeVector(0.,0.,cyPosZ),logicalCyl,"Cyl",fMotherVolume,false,0);

  // Flange towards ECal thin window
  G4double flaLen = geo->GetVCCFThick()+1.*m;
  //G4double flaThick = geo->GetVCCFThick();
  G4double flaRIn = geo->GetVCCFRIn();
  //G4double flaROut = geo->GetVCCFROut();
  G4double flaPosZ = geo->GetVCCFPosZ();
  //G4Tubs* solidFla = new G4Tubs("VCFlaGlob",0.,flaROut,0.5*flaThick,0.*deg,360.*deg);
  G4Tubs* solidFla = new G4Tubs("VCFlaGlob",0.,flaRIn,0.5*flaLen,0.*deg,360.*deg);
  printf("Flange hole %f %f %f\n",flaLen,flaRIn,flaPosZ);

  // Create hole at beam entrance (will need flange to connect to target structure)
  G4double holeThick = geo->GetVCInHoleThick()+1.*mm;
  G4double holeRadius = geo->GetVCInHoleRadius();
  G4double holePosZ = geo->GetVCInHolePosZ();
  G4Tubs* solidInHole = new G4Tubs("VCInHole",0.,holeRadius,0.5*holeThick,0.*deg,360.*deg);

  // Attach cylinder and flange to vacuum chamber
  G4UnionSolid* solid0 = new G4UnionSolid("VCInt0",solidMain,solidCyl,0,G4ThreeVector(0.,0.,cyPosZ));
  G4UnionSolid* solid1 = new G4UnionSolid("VCInt1",solid0,solidFla,0,G4ThreeVector(0.,0.,flaPosZ));
  G4UnionSolid* solidFinal = new G4UnionSolid("ChamberInternal",solid1,solidInHole,0,G4ThreeVector(0.,0.,holePosZ));
  //G4UnionSolid* solidFinal = new G4UnionSolid("ChamberGlobal",solidMain,solidCyl,0,G4ThreeVector(0.,0.,cyPosZ));

  return solidFinal;

}
