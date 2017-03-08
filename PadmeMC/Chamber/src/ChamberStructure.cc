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

    /////////////////////////
   // Main vacuum chamber //
  /////////////////////////

  printf("Creating global VC volume\n");
  G4UnionSolid* solidGlobVC = CreateVCFacetGlobalSolid();
  fGlobalLogicalVolume = new G4LogicalVolume(solidGlobVC,G4Material::GetMaterial("Vacuum"),"ChamberGlobal",0,0,0);
  //fGlobalLogicalVolume->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));
  fGlobalLogicalVolume->SetVisAttributes(G4VisAttributes::Invisible);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),fGlobalLogicalVolume,"VacuumChamber",fMotherVolume,false,0);

  printf("Creating external VC volume\n");
  G4UnionSolid* solidExtVC = CreateVCFacetExternalSolid();
  //fExternalLogicalVolume = new G4LogicalVolume(solidExtVC,G4Material::GetMaterial("G4_STAINLESS-STEEL"),
  //					       "VCExternal",0,0,0);
  //fExternalLogicalVolume->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));
  //new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),fExternalLogicalVolume,"ChamberExternal",fGlobalLogicalVolume,false,0);

  printf("Creating internal VC volume\n");
  G4UnionSolid* solidIntVC = CreateVCFacetInternalSolid();
  //G4LogicalVolume* internalLogicalVolume = new G4LogicalVolume(solidIntVC,G4Material::GetMaterial("Vacuum"), "VCInternal",0,0,0);
  //internalLogicalVolume->SetVisAttributes(G4VisAttributes::Invisible);
  //internalLogicalVolume->SetVisAttributes(G4VisAttributes(G4Colour::White()));
  //new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),internalLogicalVolume,"ChamberInternal",fMotherVolume,false,0);

  G4SubtractionSolid* solidSteelShell = new G4SubtractionSolid("ChamberSteelShell",solidExtVC,solidIntVC,0,G4ThreeVector(0.,0.,0.));
  fSteelShellLogicalVolume = new G4LogicalVolume(solidSteelShell,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"ChamberShell",0,0,0);
  //fSteelShellLogicalVolume->SetVisAttributes(G4VisAttributes::Invisible);
  fSteelShellLogicalVolume->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
  new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),fSteelShellLogicalVolume,"ChamberShell",fGlobalLogicalVolume,false,0);

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
  G4double ewz1 = geo->GetEWBackFacePosZ()+(ewr1-ewC);
  G4double ewth1 = asin(ewR/ewr1);
  //printf("ewr1 %f ewz1 %f ewth1 %11.9f pi-ewth1 %11.9f\n",ewr1,ewz1,ewth1,180.*deg-ewth1);
  G4Sphere* solidEWSphere1 = new G4Sphere("EWSphere1",ewr1,ewr1+ewd1,0.*deg,360.*deg,180.*deg-ewth1,ewth1);
  G4Tubs* solidEWRing1 = new G4Tubs("EWRing1",ewR-10.*um,ewR+10.*mm,0.7*ewd1,0.*deg,360.*deg);
  G4ThreeVector ring1Pos = G4ThreeVector(0.,0.,-sqrt(ewr1*ewr1-ewR*ewR)-0.5*ewd1);
  G4SubtractionSolid* solidEWLayer1 = new G4SubtractionSolid("EWMylarExt",solidEWSphere1,solidEWRing1,0,ring1Pos);
  G4LogicalVolume* logicalEWLayer1 = new G4LogicalVolume(solidEWLayer1,G4Material::GetMaterial("G4_MYLAR"), "EWMylarExt",0,0,0);
  logicalEWLayer1->SetVisAttributes(G4VisAttributes(G4Colour::Green()));
  new G4PVPlacement(0,G4ThreeVector(0.,0.,ewz1),logicalEWLayer1,"EWMylarExt",fGlobalLogicalVolume,false,0);

  // Central kevlar membrane
  G4double ewr2 = ewr1+ewd1;
  G4double ewz2 = ewz1;
  G4double ewth2 = asin(ewR/ewr2);
  //printf("ewr2 %f ewz2 %f ewth2 %11.9f\n",ewr2,ewz2,ewth2);
  G4Sphere* solidEWSphere2 = new G4Sphere("EWSphere2",ewr2,ewr2+ewd2,0.*deg,360.*deg,180.*deg-ewth2,ewth2);
  G4Tubs* solidEWRing2 = new G4Tubs("EWRing2",ewR-10.*um,ewR+10.*mm,0.7*ewd2,0.*deg,360.*deg);
  G4ThreeVector ring2Pos = G4ThreeVector(0.,0.,-sqrt(ewr2*ewr2-ewR*ewR)-0.5*ewd2);
  G4SubtractionSolid* solidEWLayer2 = new G4SubtractionSolid("EWKevlar",solidEWSphere2,solidEWRing2,0,ring2Pos);
  G4LogicalVolume* logicalEWLayer2 = new G4LogicalVolume(solidEWLayer2,G4Material::GetMaterial("Kevlar"), "EWKevlar",0,0,0);
  logicalEWLayer2->SetVisAttributes(G4VisAttributes(G4Colour::Green()));
  new G4PVPlacement(0,G4ThreeVector(0.,0.,ewz2),logicalEWLayer2,"EWKevlar",fGlobalLogicalVolume,false,0);

  // Internal mylar membrane
  G4double ewr3 = ewr2+ewd2;
  G4double ewz3 = ewz1;
  G4double ewth3 = asin(ewR/ewr3);
  //printf("ewr3 %f ewz3 %f ewth3 %11.9f\n",ewr3,ewz3,ewth3);
  G4Sphere* solidEWSphere3 = new G4Sphere("EWSphere3",ewr3,ewr3+ewd3,0.*deg,360.*deg,180.*deg-ewth3,ewth3);
  G4Tubs* solidEWRing3 = new G4Tubs("EWRing3",ewR-10.*um,ewR+10.*mm,0.7*ewd3,0.*deg,360.*deg);
  G4ThreeVector ring3Pos = G4ThreeVector(0.,0.,-sqrt(ewr3*ewr3-ewR*ewR)-0.5*ewd3);
  G4SubtractionSolid* solidEWLayer3 = new G4SubtractionSolid("EWMylarInt",solidEWSphere3,solidEWRing3,0,ring3Pos);
  G4LogicalVolume* logicalEWLayer3 = new G4LogicalVolume(solidEWLayer3,G4Material::GetMaterial("G4_MYLAR"), "EWMylarInt",0,0,0);
  logicalEWLayer3->SetVisAttributes(G4VisAttributes(G4Colour::Green()));
  new G4PVPlacement(0,G4ThreeVector(0.,0.,ewz3),logicalEWLayer3,"EWMylarInt",fGlobalLogicalVolume,false,0);

  // First steel ring (outer)
  G4Tubs* solidEWIronRing1 = new G4Tubs("EWIronRing1",geo->GetEWF1RIn(),geo->GetEWF1ROut(),0.5*geo->GetEWF1Thick(),0.*deg,360.*deg);
  G4LogicalVolume* logicalEWIronRing1 = new G4LogicalVolume(solidEWIronRing1,G4Material::GetMaterial("G4_STAINLESS-STEEL"), "EWIronRing1",0,0,0);
  logicalEWIronRing1->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
  new G4PVPlacement(0,G4ThreeVector(0.,0.,geo->GetEWF1PosZ()),logicalEWIronRing1,"EWIronRing1",fGlobalLogicalVolume,false,0);

  // Second steel ring
  G4Tubs* solidEWIronRing2 = new G4Tubs("EWIronRing2",geo->GetEWF2RIn(),geo->GetEWF2ROut(),0.5*geo->GetEWF2Thick(),0.*deg,360.*deg);
  G4LogicalVolume* logicalEWIronRing2 = new G4LogicalVolume(solidEWIronRing2,G4Material::GetMaterial("G4_STAINLESS-STEEL"), "EWIronRing2",0,0,0);
  logicalEWIronRing2->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
  new G4PVPlacement(0,G4ThreeVector(0.,0.,geo->GetEWF2PosZ()),logicalEWIronRing2,"EWIronRing2",fGlobalLogicalVolume,false,0);

  // Third steel ring
  G4Tubs* solidEWIronRing3 = new G4Tubs("EWIronRing3",geo->GetEWF3RIn(),geo->GetEWF3ROut(),0.5*geo->GetEWF3Thick(),0.*deg,360.*deg);
  G4LogicalVolume* logicalEWIronRing3 = new G4LogicalVolume(solidEWIronRing3,G4Material::GetMaterial("G4_STAINLESS-STEEL"), "EWIronRing3",0,0,0);
  logicalEWIronRing3->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
  new G4PVPlacement(0,G4ThreeVector(0.,0.,geo->GetEWF3PosZ()),logicalEWIronRing3,"EWIronRing3",fGlobalLogicalVolume,false,0);

}

G4double ChamberStructure::GetChamberMostExternalX()
{
  return ChamberGeometry::GetInstance()->GetVCMostExternalX();
}

G4double ChamberStructure::GetChamberMostAdvancedZ()
{
  return ChamberGeometry::GetInstance()->GetVCMostAdvancedZ();
}

G4ThreeVector ChamberStructure::GetChamberBackFaceCorner()
{
  return ChamberGeometry::GetInstance()->GetVCBackFaceCorner();
}

G4double ChamberStructure::GetChamberBackFaceAngle()
{
  return ChamberGeometry::GetInstance()->GetVCBackFaceAngle();
}

G4double ChamberStructure::GetChamberBackFaceThickness()
{
  return ChamberGeometry::GetInstance()->GetVCBackFaceThickness();
}

G4UnionSolid* ChamberStructure::CreateVCFacetGlobalSolid()
{

  // Create the main volume of the vacuum chamber

  ChamberGeometry* geo = ChamberGeometry::GetInstance();

  // Create main VC volume as a tessellated solid
  G4TessellatedSolid* solidMain = new G4TessellatedSolid("VCGlobMain");

  G4int nFacets = geo->GetVCNFacets();
  for(G4int f=0; f<nFacets; f++) {
    G4ThreeVector vtx[4];
    G4int quadFacet = 1;
    for(G4int v=0; v<4; v++) {
      G4int ivtx = geo->GetVCFacetVtx(f,v);
      if (ivtx != -1) {
	G4int is = ivtx/10; // Section id (0-11)
	G4int iv = ivtx%10; // Vertex id (0-3)
	vtx[v] = geo->GetVCExtVtx(is,iv);
      } else {
	quadFacet = 0;
      }
    }
    if (quadFacet) {
      G4QuadrangularFacet* facet = new G4QuadrangularFacet(vtx[0],vtx[1],vtx[2],vtx[3],ABSOLUTE);
      solidMain->AddFacet((G4VFacet*)facet);
    } else {
      G4TriangularFacet* facet = new G4TriangularFacet(vtx[0],vtx[1],vtx[2],ABSOLUTE);
      solidMain->AddFacet((G4VFacet*)facet);
    }
  }
  solidMain->SetSolidClosed(true);

  // In depth debug of tesselated solid
  //G4cout << "Dump of solid:\n" << solidMain << G4endl;
  //G4Polyhedron* polyhedron = solidMain->GetPolyhedron();
  //G4cout << "\nLocal polyhedron coordinates:\n" << *polyhedron << G4endl;

  //G4LogicalVolume* logicalMain = new G4LogicalVolume(solidMain,G4Material::GetMaterial("Vacuum"),"VCGlobMain",0,0,0);
  //new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicalMain,"VCGlobMain",fMotherVolume,false,0);

  // Big cylinder
  G4double cyLen = geo->GetVCCLength();
  G4double cyROut = geo->GetVCCROut();
  G4double cyPosZ = geo->GetVCCPosZ();
  //printf("Cylinder %f %f %f %f %f\n",cyLen,cyThick,cyRIn,cyROut,cyPosZ);
  G4Tubs* solidCyl = new G4Tubs("VCCylGlob",0.,cyROut,0.5*cyLen,0.*deg,360.*deg);

  // Flange towards ECal thin window (includes thin window and its flange)
  G4double flaLen = geo->GetVCMostAdvancedZ()-(cyPosZ+0.5*cyLen);
  G4double flaROut = geo->GetVCCFROut();
  G4double flaPosZ = cyPosZ+0.5*cyLen+0.5*flaLen;
  G4Tubs* solidFla = new G4Tubs("VCFlaGlob",0.,flaROut,0.5*flaLen,0.*deg,360.*deg);

  // Crossed pipes at target position
  G4double cpzR = geo->GetCPZROut();
  G4double cpzLen = geo->GetCPZLength();
  G4double cpzPosZ = geo->GetCPZPosZ();
  G4Tubs* solidCPZ = new G4Tubs("CPZGlob",0.,cpzR,0.5*cpzLen,0.*deg,360.*deg);
  G4ThreeVector posCPZ = G4ThreeVector(0.,0.,cpzPosZ);

  G4double cpxR = geo->GetCPXROut();
  G4double cpxLen = geo->GetCPXLength();
  G4double cpxPosZ = geo->GetCPXPosZ();
  G4Tubs* solidCPX = new G4Tubs("CPXGlob",0.,cpxR,0.5*cpxLen,0.*deg,360.*deg);
  G4ThreeVector posCPX = G4ThreeVector(0.,0.,cpxPosZ);
  G4RotationMatrix* rotCPX = new G4RotationMatrix;
  rotCPX->rotateY(90.*deg);

  // Attach cylinder, flange, and pipes to vacuum chamber
  G4UnionSolid* solid0 = new G4UnionSolid("VCGlobal0",solidMain,solidCyl,0,G4ThreeVector(0.,0.,cyPosZ));
  G4UnionSolid* solid1 = new G4UnionSolid("VCGlobal1",solid0,solidFla,0,G4ThreeVector(0.,0.,flaPosZ));
  G4UnionSolid* solid2 = new G4UnionSolid("VCGlobal2",solid1,solidCPZ,0,posCPZ);
  G4UnionSolid* solid3 = new G4UnionSolid("ChamberGlobal",solid2,solidCPX,rotCPX,posCPX);

  return solid3;

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

  // Crossed pipes at target position
  G4double cpzR = geo->GetCPZROut();
  G4double cpzLen = geo->GetCPZLength();
  G4double cpzPosZ = geo->GetCPZPosZ();
  G4Tubs* solidCPZ = new G4Tubs("CPZExt",0.,cpzR-1.*um,0.5*cpzLen-1.*um,0.*deg,360.*deg);
  G4ThreeVector posCPZ = G4ThreeVector(0.,0.,cpzPosZ);

  G4double cpxR = geo->GetCPXROut();
  G4double cpxLen = geo->GetCPXLength();
  G4double cpxPosZ = geo->GetCPXPosZ();
  G4Tubs* solidCPX = new G4Tubs("CPXExt",0.,cpxR-1.*um,0.5*cpxLen-1.*um,0.*deg,360.*deg);
  G4ThreeVector posCPX = G4ThreeVector(0.,0.,cpxPosZ);
  G4RotationMatrix* rotCPX = new G4RotationMatrix;
  rotCPX->rotateY(90.*deg);

  // Attach cylinder and flange to vacuum chamber
  G4UnionSolid* solid0 = new G4UnionSolid("VCExt0",solidMain,solidCyl,0,G4ThreeVector(0.,0.,cyPosZ));
  G4UnionSolid* solid1 = new G4UnionSolid("VCExt1",solid0,solidFla,0,G4ThreeVector(0.,0.,flaPosZ));
  G4UnionSolid* solid2 = new G4UnionSolid("VCExt2",solid1,solidCPZ,0,posCPZ);
  G4UnionSolid* solid3 = new G4UnionSolid("ChamberExternal",solid2,solidCPX,rotCPX,posCPX);
  //G4UnionSolid* solidFinal = new G4UnionSolid("ChamberExternal",solidMain,solidCyl,0,G4ThreeVector(0.,0.,cyPosZ));

  return solid3;

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

  //G4cout << "Dump of solid:\n" << solidMain << G4endl;
  //G4Polyhedron* polyhedron = solidMain->GetPolyhedron();
  //G4cout << "\nLocal polyhedron coordinates:\n" << *polyhedron << G4endl;

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
  G4double flaLen = geo->GetVCCFThick()+1.*cm;
  //G4double flaThick = geo->GetVCCFThick();
  G4double flaRIn = geo->GetVCCFRIn();
  //G4double flaROut = geo->GetVCCFROut();
  G4double flaPosZ = geo->GetVCCFPosZ();
  //G4Tubs* solidFla = new G4Tubs("VCFlaGlob",0.,flaROut,0.5*flaThick,0.*deg,360.*deg);
  G4Tubs* solidFla = new G4Tubs("VCFlaGlob",0.,flaRIn,0.5*flaLen,0.*deg,360.*deg);
  printf("Flange hole %f %f %f\n",flaLen,flaRIn,flaPosZ);

  // Create hole at beam entrance (will need flange to connect to target structure)
  // This is now done with a prolugation of the along-Z section of the crossed pipe
  //G4double holeThick = geo->GetVCInHoleThick()+1.*mm;
  //G4double holeRadius = geo->GetVCInHoleRadius();
  //G4double holePosZ = geo->GetVCInHolePosZ();
  //G4Tubs* solidInHole = new G4Tubs("VCInHole",0.,holeRadius,0.5*holeThick,0.*deg,360.*deg);

  // Create hole at beam exit (will need flange to connect to thin TPix window)
  G4double hoT = geo->GetVCOutHoleThick();
  G4double hoR = geo->GetVCOutHoleRadius();
  G4double hoL = geo->GetVCOutHoleLength();
  G4double hoD = geo->GetVCOutHoleDistToEdge();
  G4double hoA = geo->GetVCBackFaceAngle();
  G4Box* solidHO0 = new G4Box("VCHOut0",0.5*hoL,hoR,0.5*hoT+0.7*cm);
  G4Tubs* solidHO1 = new G4Tubs("VCHOut1",0.,hoR,0.5*hoT+0.5*cm,0.*deg,360.*deg);
  G4UnionSolid* solidHO2 = new G4UnionSolid("VCHOut2",solidHO0,solidHO1,0,G4ThreeVector(-0.5*hoL,0.,0.));
  G4UnionSolid* solidOutHole = new G4UnionSolid("VCHOut3",solidHO2,solidHO1,0,G4ThreeVector(0.5*hoL,0.,0.));
  G4RotationMatrix* rotHOut = new G4RotationMatrix;
  rotHOut->rotateY(hoA);
  G4ThreeVector hoEdge = geo->GetVCExtVtx(11,1);
  G4double hoCX = hoEdge.x()-hoD*cos(hoA)+0.5*hoT*sin(hoA);
  G4double hoCZ = hoEdge.z()-hoD*sin(hoA)-0.5*hoT*cos(hoA);
  G4ThreeVector posHOut = G4ThreeVector(hoCX,0.,hoCZ);
  //printf("Exit hole %f %f %f %f %f %f %f\n",hoT,hoR,hoL,hoD,hoA,hoCX,hoCZ);

  // Crossed pipes at target position
  G4double cpzR = geo->GetCPZRIn();
  G4double cpzLen = geo->GetCPZLength();
  G4double cpzPosZ = geo->GetCPZPosZ();
  G4Tubs* solidCPZ = new G4Tubs("CPZGlob",0.,cpzR,0.5*cpzLen+2.*cm,0.*deg,360.*deg);
  G4ThreeVector posCPZ = G4ThreeVector(0.,0.,cpzPosZ);

  G4double cpxR = geo->GetCPXRIn();
  G4double cpxLen = geo->GetCPXLength();
  G4double cpxPosZ = geo->GetCPXPosZ();
  G4Tubs* solidCPX = new G4Tubs("CPXGlob",0.,cpxR,0.5*cpxLen+1.*cm,0.*deg,360.*deg);
  G4ThreeVector posCPX = G4ThreeVector(0.,0.,cpxPosZ);
  G4RotationMatrix* rotCPX = new G4RotationMatrix;
  rotCPX->rotateY(90.*deg);

  // Attach cylinder, flange, hole, and pipes to vacuum chamber
  G4UnionSolid* solid0 = new G4UnionSolid("VCInt0",solidMain,solidCyl,0,G4ThreeVector(0.,0.,cyPosZ));
  G4UnionSolid* solid1 = new G4UnionSolid("VCInt1",solid0,solidFla,0,G4ThreeVector(0.,0.,flaPosZ));
  //G4UnionSolid* solid2 = new G4UnionSolid("VCInt2",solid1,solidInHole,0,G4ThreeVector(0.,0.,holePosZ));
  G4UnionSolid* solid3 = new G4UnionSolid("VCInt3",solid1,solidOutHole,rotHOut,posHOut);
  G4UnionSolid* solid4 = new G4UnionSolid("VCInt4",solid3,solidCPZ,0,posCPZ);
  G4UnionSolid* solid5 = new G4UnionSolid("ChamberInternal",solid4,solidCPX,rotCPX,posCPX);

  return solid4; // Use this for visualization
  //return solid5; // Use this for simulation

}
