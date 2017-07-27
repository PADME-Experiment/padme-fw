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
{
  fChamberExists = 1;    // If =0 the physical structure of the chamber is not created
  fChamberIsVisible = 1; // If =0 all chamber structures are invisible (debug only)
}

ChamberStructure::~ChamberStructure()
{}

void ChamberStructure::CreateGeometry()
{

  // The main parts of the Vacuum Chamber are imported in DetectorConstruction.cc from GDML files
  // Here we only define flanges, portholes and other components of the chamber

  // Create the thin window membrane in front of ECal with its flange
  CreateECalAlThinWindow();

  // Create crossed pipes in the target area
  CreateTargetPipes();

}

void ChamberStructure::CreateECalAlThinWindow()
{

  /////////////////////////////////////////
  // Thin Al window flange in front of ECal //
  /////////////////////////////////////////

  ChamberGeometry* geo = ChamberGeometry::GetInstance();

  // Get properties of thin window
  G4double ewR = geo->GetEWAlRadius(); // Radius of window
  G4double ewT = geo->GetEWAlThick(); // Thickness of window
  G4double ewC = geo->GetEWAlConvexity(); // Convexity at window center

  // Get properties of flange
  G4double ewFRIn = geo->GetEWAlFlangeRIn();
  G4double ewFROut = geo->GetEWAlFlangeROut();
  G4double ewFThick = geo->GetEWAlFlangeThick();

  // Get Z coordinate of front and back face of flange
  G4double efFFrontZ = geo->GetEWAlFrontFacePosZ();
  //G4double efFBackZ = geo->GetEWAlBackFacePosZ();

  G4VisAttributes alVisAttr = G4VisAttributes(G4Colour::Blue());
  if ( ! fChamberIsVisible ) alVisAttr = G4VisAttributes::Invisible;

  // Compute thin window sphere's radius and z position of center
  G4double ewr1 = (ewR*ewR+ewC*ewC)/(2.*ewC);
  //G4double ewz1 = efFBackZ+(ewr1-ewC);
  G4double ewth1 = asin(ewR/ewr1);

  // Create flange around thin window
  G4Tubs* solidEWFlange = new G4Tubs("EWFlange",ewFRIn,ewFROut,0.5*ewFThick,0.*deg,360.*deg);

  // Create the thin window spherical cap
  G4Sphere* solidEWSphere = new G4Sphere("EWSphere",ewr1,ewr1+ewT,0.*deg,360.*deg,180.*deg-ewth1,ewth1);

  // Cretae the final structure by joining the two volumes
  //G4ThreeVector ewspherePos = G4ThreeVector(0.,0.,-sqrt(ewr1*ewr1-ewR*ewR)-0.5*ewT);
  //G4UnionSolid* solidEWindow = new G4UnionSolid("EAlWindow",solidEWSphere,solidEWFlange,0,flangePos);
  G4ThreeVector spherePos = G4ThreeVector(0.,0.,0.5*ewFThick+ewr1-ewC);
  G4UnionSolid* solidEWindow = new G4UnionSolid("ChamberAlWindow",solidEWFlange,solidEWSphere,0,spherePos);
  G4LogicalVolume* logicalEWindow = new G4LogicalVolume(solidEWindow,G4Material::GetMaterial("G4_Al"), "ChamberAlWindow",0,0,0);
  logicalEWindow->SetVisAttributes(alVisAttr);
  //new G4PVPlacement(0,G4ThreeVector(0.,0.,ewz1),logicalEWindow,"ChamberAlWindow",fMotherVolume,false,0,true);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,efFFrontZ+0.5*ewFThick),logicalEWindow,"ChamberAlWindow",fMotherVolume,false,0,true);

}

void ChamberStructure::CreateTargetPipes()
{

  ChamberGeometry* geo = ChamberGeometry::GetInstance();

  G4VisAttributes steelVisAttr = G4VisAttributes(G4Colour::Grey());
  if ( ! fChamberIsVisible ) steelVisAttr = G4VisAttributes::Invisible;

  // Crossed pipes at target position
  G4double cpzRIn = geo->GetCPZRIn();
  G4double cpzROut = geo->GetCPZROut();
  G4double cpzLen = geo->GetCPZLength();
  //G4Tubs* solidCPZe = new G4Tubs("CPZe",0.,cpzROut-1.*um,0.5*cpzLen-1.*um,0.*deg,360.*deg);
  //G4Tubs* solidCPZi = new G4Tubs("CPZi",0.,cpzRIn,0.5*cpzLen+1.*mm,0.*deg,360.*deg);
  G4Tubs* solidCPZe = new G4Tubs("CPZe",0.,cpzROut,0.5*cpzLen,0.*deg,360.*deg);
  G4Tubs* solidCPZi = new G4Tubs("CPZi",0.,cpzRIn,0.5*cpzLen+1.*mm,0.*deg,360.*deg);

  G4double cpxRIn = geo->GetCPXRIn();
  G4double cpxROut = geo->GetCPXROut();
  G4double cpxLen = geo->GetCPXLength();
  //G4Tubs* solidCPXe = new G4Tubs("CPXe",0.,cpxROut-1.*um,0.5*cpxLen-1.*um,0.*deg,360.*deg);
  //G4Tubs* solidCPXi = new G4Tubs("CPXi",0.,cpxRIn,0.5*cpxLen+1.*mm,0.*deg,360.*deg);
  G4Tubs* solidCPXe = new G4Tubs("CPXe",0.,cpxROut,0.5*cpxLen,0.*deg,360.*deg);
  G4Tubs* solidCPXi = new G4Tubs("CPXi",0.,cpxRIn,0.5*cpxLen+1.*mm,0.*deg,360.*deg);

  G4RotationMatrix* rotCPX = new G4RotationMatrix;
  rotCPX->rotateY(90.*deg);

  G4UnionSolid* solidCP1 = new G4UnionSolid("VCCP1",solidCPZe,solidCPXe,rotCPX,G4ThreeVector(0.,0.,0.));
  G4SubtractionSolid* solidCP2 = new G4SubtractionSolid("VCCP2",solidCP1,solidCPZi,0,G4ThreeVector(0.,0.,0.));
  G4SubtractionSolid* solidCP3 = new G4SubtractionSolid("VCCP3",solidCP2,solidCPXi,rotCPX,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicalCP = new G4LogicalVolume(solidCP3,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"VCCP",0,0,0);
  logicalCP->SetVisAttributes(steelVisAttr);
  //new G4PVPlacement(0,G4ThreeVector(0.,0.,geo->GetCPZPosZ()),logicalCP,"CrossPipeSteel",fGlobalLogicalVolume,false,0);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,geo->GetCPZPosZ()),logicalCP,"CrossPipeSteel",fMotherVolume,false,0);

}
/*
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

G4double ChamberStructure::GetChamberInnerX()
{
  return ChamberGeometry::GetInstance()->GetVCInnerX();
}

G4double ChamberStructure::GetChamberInnerY()
{
  return ChamberGeometry::GetInstance()->GetVCInnerY();
}

G4double ChamberStructure::GetChamberInnerZ()
{
  return ChamberGeometry::GetInstance()->GetVCInnerZ();
}
*/
