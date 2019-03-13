// BeamLineStructure.cc
// --------------------------------------------------------------
// History:
//
// Created by Mauro Raggi (mauro.raggi@roma1.infn.it) 2016-06-10
// --------------------------------------------------------------

#include "BeamLineStructure.hh"

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

#include "BeamLineGeometry.hh"

using namespace CLHEP;

BeamLineStructure::BeamLineStructure(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{
  fBeamLineExists = 1;    // If =0 the physical structure of the chamber is not created
  fBeamLineIsVisible = 1; // If =0 all chamber structures are invisible (debug only)
}

BeamLineStructure::~BeamLineStructure()
{}

void BeamLineStructure::CreateGeometry()
{

  // The main parts of the Vacuum BeamLine are imported in DetectorConstruction.cc from GDML files
  // Here we only define flanges, portholes and other components of the chamber

  if (fBeamLineExists) {
    
    // Create the thin window membrane in front of ECal with its flange
    CreateBeThinWindow();
    
    // Create DHSTB002 inner pipe
    CreateMagnetPipe();
    
    // Create junction pipe between Be window and DHSTB002
    CreateJunctionPipe();
    
    // Create DHSTB002 magnet
    CreateDHSTB002Magnet();
    
  }

}

//void BeamLineStructure::CreateECalAlThinWindow()
//{
//
//  /////////////////////////////////////////
//  // Thin Al window flange in front of ECal //
//  /////////////////////////////////////////
//
//  BeamLineGeometry* geo = BeamLineGeometry::GetInstance();
//
//  // Get properties of thin window
//  G4double ewR = geo->GetEWAlRadius(); // Radius of window
//  G4double ewT = geo->GetEWAlThick(); // Thickness of window
//  G4double ewC = geo->GetEWAlConvexity(); // Convexity at window center
//
//  // Get properties of flange
//  G4double ewFRIn = geo->GetEWAlFlangeRIn();
//  G4double ewFROut = geo->GetEWAlFlangeROut();
//  G4double ewFThick = geo->GetEWAlFlangeThick();
//
//  // Get Z coordinate of front and back face of flange
//  G4double efFFrontZ = geo->GetEWAlFrontFacePosZ();
//  //G4double efFBackZ = geo->GetEWAlBackFacePosZ();
//
//  G4VisAttributes alVisAttr = G4VisAttributes(G4Colour::Blue());
//  if ( ! fBeamLineIsVisible ) alVisAttr = G4VisAttributes::Invisible;
//
//  // Compute thin window sphere's radius and z position of center
//  G4double ewr1 = (ewR*ewR+ewC*ewC)/(2.*ewC);
//  //G4double ewz1 = efFBackZ+(ewr1-ewC);
//  G4double ewth1 = asin(ewR/ewr1);
//
////  // Create flange around thin window
//  G4Tubs* solidEWFlange = new G4Tubs("EWFlange",ewFRIn,ewFROut,0.5*ewFThick,0.*deg,360.*deg);

//  // Create the thin window spherical cap
//  G4Sphere* solidEWSphere = new G4Sphere("EWSphere",ewr1,ewr1+ewT,0.*deg,360.*deg,180.*deg-ewth1,ewth1);
//
//  // Cretae the final structure by joining the two volumes
//  //G4ThreeVector ewspherePos = G4ThreeVector(0.,0.,-sqrt(ewr1*ewr1-ewR*ewR)-0.5*ewT);
//  //G4UnionSolid* solidEWindow = new G4UnionSolid("EAlWindow",solidEWSphere,solidEWFlange,0,flangePos);
//  G4ThreeVector spherePos = G4ThreeVector(0.,0.,0.5*ewFThick+ewr1-ewC);
//  G4UnionSolid* solidEWindow = new G4UnionSolid("BeamLineAlWindow",solidEWFlange,solidEWSphere,0,spherePos);
//  G4LogicalVolume* logicalEWindow = new G4LogicalVolume(solidEWindow,G4Material::GetMaterial("G4_Al"), "BeamLineAlWindow",0,0,0);
//  logicalEWindow->SetVisAttributes(alVisAttr);
//  //new G4PVPlacement(0,G4ThreeVector(0.,0.,ewz1),logicalEWindow,"BeamLineAlWindow",fMotherVolume,false,0,true);
//  new G4PVPlacement(0,G4ThreeVector(0.,0.,efFFrontZ+0.5*ewFThick),logicalEWindow,"BeamLineAlWindow",fMotherVolume,false,0,true);
//
//}

void BeamLineStructure::CreateBeThinWindow()
{

  ///////////////////////////////////////////////////////
  // Thin Be 250um window monted before DHSTB002       //
  ///////////////////////////////////////////////////////

  BeamLineGeometry* geo = BeamLineGeometry::GetInstance();

  // Get properties of thin window
  G4double BeWThick = geo->GetBeWThick() ; //Thickness of window
  G4double BeWDiam  = geo->GetBeWDiameter() ; //Thickness of window

  G4VisAttributes alVisAttr = G4VisAttributes(G4Colour::Blue());
  //if ( ! fBeamLineIsVisible ) alVisAttr = G4VisAttributes::Invisible;

  G4VisAttributes cVisAttr = G4VisAttributes(G4Colour::Yellow());
  //if ( ! fBeamLineIsVisible ) cVisAttr = G4VisAttributes::Invisible;

  // Create flange around thin window
  //  G4Tubs* solidEWFlange = new G4Tubs("EWFlange",ewFRIn,ewFROut,0.5*ewFThick,0.*deg,360.*deg);
  //  G4Box* solidBeWindow = new G4Box("BeWind",BeWSizeX,BeWSizeY,BeWThick);
  G4Tubs* solidBeWindow = new G4Tubs("BeWind",0.,BeWDiam*0.5,BeWThick,0.*deg,360.*deg);
  G4LogicalVolume* logicalBeWindow = new G4LogicalVolume(solidBeWindow,G4Material::GetMaterial("G4_Be"), "logicalBeWindow",0,0,0);
  // logicalBeWindow->SetVisAttributes(BeVisAttr);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,-3000.*mm),logicalBeWindow,"BeamLineBeWindow",fMotherVolume,false,0,true);
  // Create flange around thin window
  G4double FlThick    = geo->GetBeFlThick();
  G4double FlDiameter = geo->GetBeFlDiameter();

  G4Tubs* solidBeFlange = new G4Tubs("solidBeFlange",0.,FlDiameter*0.5,FlThick,0.*deg,360.*deg);
  G4LogicalVolume* logicalBeFlange = new G4LogicalVolume(solidBeFlange,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalBeFlange",0,0,0);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,-3001.*mm),logicalBeFlange,"BeamLineBeFlange",fMotherVolume,false,0,true);

//  // Create the thin window spherical cap and subtract flange to smooth its edge
//  G4Sphere* solidEWSphere = new G4Sphere("EWSphere",ewr1,ewr1+ewT,0.*deg,360.*deg,180.*deg-ewth1,ewth1);
//  G4ThreeVector spherePos = G4ThreeVector(0.,0.,0.5*ewFThick+ewr1-ewC);
//  G4SubtractionSolid* solidEWindow = new G4SubtractionSolid("BeamLineECalWindow",solidEWSphere,solidEWFlange,0,G4ThreeVector(0.,0.,-0.5*ewFThick-ewr1+ewC));
//  G4LogicalVolume* logicalEWindow = new G4LogicalVolume(solidEWindow,G4Material::GetMaterial("G4_C"), "BeamLineECalWindow",0,0,0);
//  logicalEWindow->SetVisAttributes(cVisAttr);
//  new G4PVPlacement(0,G4ThreeVector(0.,0.,efFFrontZ+ewFThick+ewr1-ewC),logicalEWindow,"BeamLineECalWindow",fMotherVolume,false,0,true);

}

void BeamLineStructure::CreateMagnetPipe()
{

  BeamLineGeometry* geo = BeamLineGeometry::GetInstance();

// G4VisAttributes steelVisAttr = G4VisAttributes(G4Colour::Grey());
// if ( ! fBeamLineIsVisible ) steelVisAttr = G4VisAttributes::Invisible;
// 
// // Crossed pipes at target position
// G4double cpzRIn = geo->GetCPZRIn();
// G4double cpzROut = geo->GetCPZROut();
// G4double cpzLen = geo->GetCPZLength();
// G4double cpzPosZ = geo->GetCPZPosZ();
// 
// G4Tubs* solidCPZe = new G4Tubs("CPZe",0.,cpzROut,0.5*cpzLen,0.*deg,360.*deg);
// G4Tubs* solidCPZi = new G4Tubs("CPZi",0.,cpzRIn,0.5*cpzLen+1.*mm,0.*deg,360.*deg);
// 
// G4double cpxRIn = geo->GetCPXRIn();
// G4double cpxROut = geo->GetCPXROut();
// G4double cpxLen = geo->GetCPXLength();
// 
// G4Tubs* solidCPXe = new G4Tubs("CPXe",0.,cpxROut,0.5*cpxLen,0.*deg,360.*deg);
// G4Tubs* solidCPXi = new G4Tubs("CPXi",0.,cpxRIn,0.5*cpxLen+1.*mm,0.*deg,360.*deg);
// 
// G4RotationMatrix* rotCPX = new G4RotationMatrix;
// rotCPX->rotateY(90.*deg);
// 
// G4UnionSolid* solidCP1 = new G4UnionSolid("VCCP1",solidCPZe,solidCPXe,rotCPX,G4ThreeVector(0.,0.,0.));
// G4SubtractionSolid* solidCP2 = new G4SubtractionSolid("VCCP2",solidCP1,solidCPZi,0,G4ThreeVector(0.,0.,0.));
// G4SubtractionSolid* solidCP3 = new G4SubtractionSolid("VCCP3",solidCP2,solidCPXi,rotCPX,G4ThreeVector(0.,0.,0.));
// G4LogicalVolume* logicalCP = new G4LogicalVolume(solidCP3,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"VCCP",0,0,0);
// logicalCP->SetVisAttributes(steelVisAttr);
// new G4PVPlacement(0,G4ThreeVector(0.,0.,cpzPosZ),logicalCP,"CrossPipeSteel",fMotherVolume,false,0,true);


  // Create flanges for crossed pipe

//  G4double flangezRIn = geo->GetCPZFlangeRIn();
//  G4double flangezROut = geo->GetCPZFlangeROut();
//  G4double flangezThick = geo->GetCPZFlangeThick();
//  G4Tubs* solidFlangeZ = new G4Tubs("JunFlangeZ",flangezRIn,flangezROut,0.5*flangezThick,0.*deg,360.*deg);
//  G4LogicalVolume* logicalFlangeZ = new G4LogicalVolume(solidFlangeZ,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"JunFlangeZ",0,0,0);
//  logicalFlangeZ->SetVisAttributes(steelVisAttr);
//  G4double flangez0PosZ = cpzPosZ-0.5*cpzLen+0.5*flangezThick;
//  new G4PVPlacement(0,G4ThreeVector(0.,0.,flangez0PosZ),logicalFlangeZ,"CPZFlange",fMotherVolume,false,0,true);
//  G4double flangez1PosZ = cpzPosZ+0.5*cpzLen-0.5*flangezThick;
//  new G4PVPlacement(0,G4ThreeVector(0.,0.,flangez1PosZ),logicalFlangeZ,"CPZFlange",fMotherVolume,false,1,true);
//
//  G4double flangexRIn = geo->GetCPXFlangeRIn();
//  G4double flangexROut = geo->GetCPXFlangeROut();
//  G4double flangexThick = geo->GetCPXFlangeThick();
//  G4Tubs* solidFlangeX = new G4Tubs("JunFlangeX",flangexRIn,flangexROut,0.5*flangexThick,0.*deg,360.*deg);
//  G4LogicalVolume* logicalFlangeX = new G4LogicalVolume(solidFlangeX,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"JunFlangeX",0,0,0);
//  logicalFlangeX->SetVisAttributes(steelVisAttr);
//  G4double flangex0PosX = -0.5*cpxLen+0.5*flangexThick;
//  G4double flangex0PosZ = cpzPosZ;
//  new G4PVPlacement(rotCPX,G4ThreeVector(flangex0PosX,0.,flangex0PosZ),logicalFlangeX,"CPXFlange",fMotherVolume,false,0,true);
//  G4double flangex1PosX = +0.5*cpxLen-0.5*flangexThick;
//  G4double flangex1PosZ = cpzPosZ;
//  new G4PVPlacement(rotCPX,G4ThreeVector(flangex1PosX,0.,flangex1PosZ),logicalFlangeX,"CPXFlange",fMotherVolume,false,1,true);

}

void BeamLineStructure::CreateJunctionPipe()
{

//BeamLineGeometry* geo = BeamLineGeometry::GetInstance();
//
//G4VisAttributes steelVisAttr = G4VisAttributes(G4Colour::Grey());
//if ( ! fBeamLineIsVisible ) steelVisAttr = G4VisAttributes::Invisible;
//
//// Junction pipe
//
//G4double junRIn  = geo->GetJunRIn();
//G4double junROut = geo->GetJunROut();
//G4double junLen  = geo->GetJunLength();
//G4double junPosZ = geo->GetJunPosZ();
//
//G4Tubs* solidJun = new G4Tubs("JunPipe",junRIn,junROut,0.5*junLen,0.*deg,360.*deg);
//G4LogicalVolume* logicalJun = new G4LogicalVolume(solidJun,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"JunPipe",0,0,0);
//logicalJun->SetVisAttributes(steelVisAttr);
//new G4PVPlacement(0,G4ThreeVector(0.,0.,junPosZ),logicalJun,"JunctionPipe",fMotherVolume,false,0,true);
//
//printf("Junction pipe RIn %.1fmm Rout %.1fmm Zlen %.3fmm Zpos %.3fmm\n",junRIn/mm,junROut/mm,junLen/mm,junPosZ/mm);
//
//// Add flanges
//G4double flangeRIn = geo->GetJunFlangeRIn();
//G4double flangeROut = geo->GetJunFlangeROut();
//G4double flangeThick = geo->GetJunFlangeThick();
//G4Tubs* solidFlange = new G4Tubs("JunFlange",flangeRIn,flangeROut,0.5*flangeThick,0.*deg,360.*deg);
//G4LogicalVolume* logicalFlange = new G4LogicalVolume(solidFlange,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"JunFlange",0,0,0);
//logicalFlange->SetVisAttributes(steelVisAttr);
//G4double flange0PosZ = junPosZ-0.5*junLen+0.5*flangeThick;
//new G4PVPlacement(0,G4ThreeVector(0.,0.,flange0PosZ),logicalFlange,"JunctionFlange",fMotherVolume,false,0,true);
//G4double flange1PosZ = junPosZ+0.5*junLen-0.5*flangeThick;
//new G4PVPlacement(0,G4ThreeVector(0.,0.,flange1PosZ),logicalFlange,"JunctionFlange",fMotherVolume,false,1,true);

}

void BeamLineStructure::CreateDHSTB002Magnet()
{

  BeamLineGeometry* geo = BeamLineGeometry::GetInstance();

  G4VisAttributes steelVisAttr = G4VisAttributes(G4Colour(0.4,0.4,0.4)); // Dark gray
  if ( ! fBeamLineIsVisible ) steelVisAttr = G4VisAttributes::Invisible;


  G4double DHSTB002Thick    = geo->GetDHSTB002Thick();
  G4double DHSTB002Radius   = geo->GetDHSTB002Radius();
  G4double DHSTB002InnHole  = geo->GetDHSTB002InnHole();
  G4double GapCenter        = (DHSTB002Radius-DHSTB002InnHole)/2+DHSTB002InnHole; 

  G4RotationMatrix* rotDHSTB = new G4RotationMatrix;
  rotDHSTB->rotateX(90.*deg);
  rotDHSTB->rotateZ(180.*deg);
  //  rotDHSTB->rotateY(-45.*deg);
  //  rotDHSTB->rotateXYZ(90.*deg,0.,180.*deg);

  G4Tubs* solidDHSTB002 = new G4Tubs("solidDHSTB002",DHSTB002InnHole,DHSTB002Radius,DHSTB002Thick*0.5,315.*deg,45.*deg);
  G4LogicalVolume* logicalDHSTB002 = new G4LogicalVolume(solidDHSTB002,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalDHSTB002",0,0,0);
  new G4PVPlacement(rotDHSTB,G4ThreeVector(GapCenter*mm,0.,-3001.*mm),logicalDHSTB002,"BeamLineDHSTB002",fMotherVolume,false,0,true);

  G4double Gap1Thick    = geo->GetDHSTB002Gap1Thick();
  G4double Gap1Radius   = geo->GetDHSTB002Gap1Radius();
  G4double Gap1InnHole  = geo->GetDHSTB002Gap1InnHole();
  //  G4double Gap1Center        = (DHSTB002Radius-DHSTB002InnHole)/2+DHSTB002InnHole; 
  G4Tubs* solidGap1 = new G4Tubs("solidGap1",Gap1InnHole,Gap1Radius,Gap1Thick*0.5,315.*deg,45.*deg);
  G4LogicalVolume* logicalGap1 = new G4LogicalVolume(solidGap1,G4Material::GetMaterial("G4_Vacuum"),"logicalGap1",0,0,0);
  new G4PVPlacement(rotDHSTB,G4ThreeVector(GapCenter*mm,0.,-3001.*mm),logicalGap1,"BeamLineDHSTB002Gap1",fMotherVolume,false,0,true);

  G4double Gap2Thick    = geo->GetDHSTB002Gap2Thick();
  G4double Gap2Radius   = geo->GetDHSTB002Gap2Radius();
  G4double Gap2InnHole  = geo->GetDHSTB002Gap2InnHole();
  //  G4double Gap2Center        = (DHSTB002Radius-DHSTB002InnHole)/2+DHSTB002InnHole; 
  G4Tubs* solidGap2 = new G4Tubs("solidGap2",Gap2InnHole,Gap2Radius,Gap2Thick*0.5,315.*deg,45.*deg);
  G4LogicalVolume* logicalGap2 = new G4LogicalVolume(solidGap2,G4Material::GetMaterial("G4_Vacuum"),"logicalGap2",0,0,0);
  new G4PVPlacement(rotDHSTB,G4ThreeVector(GapCenter*mm,0.,-3001.*mm),logicalGap2,"BeamLineDHSTB002Gap2",fMotherVolume,false,0,true);



  G4double TGapThick    = geo->GetDHSTB002TGapThick();
  G4double TGapRadius   = geo->GetDHSTB002TGapRadius();
  G4double TGapInnHole  = geo->GetDHSTB002TGapInnHole();

  G4Tubs* solidTGap = new G4Tubs("solidTGap",TGapInnHole,TGapRadius,TGapThick*0.5,315.*deg,45.*deg);
  G4LogicalVolume* logicalTGap = new G4LogicalVolume(solidTGap,G4Material::GetMaterial("G4_Vacuum"),"logicalTGap",0,0,0);
  new G4PVPlacement(rotDHSTB,G4ThreeVector(GapCenter*mm,0.,-3001.*mm),logicalTGap,"BeamLineDHSTB002TGap",fMotherVolume,false,0,true);


 // G4int nPH = geo->GetPHCapNumber();
 // for (G4int iPH=0; iPH<nPH; iPH++) {
 //   char phcN[7];
 //   sprintf(phcN,"PHCap%d",iPH);
 //   G4double phcR = geo->GetPHCapRadius(iPH);
 //   G4double phcT = geo->GetPHCapThick(iPH);
 //   G4Tubs* solidPHC = new G4Tubs(phcN,0.,phcR,0.5*phcT,0.*deg,360.*deg);
 //   G4LogicalVolume* logicalPHC = new G4LogicalVolume(solidPHC,G4Material::GetMaterial("G4_STAINLESS-STEEL"),phcN,0,0,0);
 //   logicalPHC->SetVisAttributes(steelVisAttr);
 //
 //   G4ThreeVector posPHC = geo->GetPHCapPos(iPH);
 //   G4RotationMatrix* rotPHC = new G4RotationMatrix;
 //   rotPHC->rotateY(90.*deg);
 //   new G4PVPlacement(rotPHC,posPHC,logicalPHC,phcN,fMotherVolume,false,0,true);
 //
 //   printf("Porthole cap %d (%s) radius %f thick %f position %f %f %f\n",iPH,phcN,phcR,phcT,posPHC.x(),posPHC.y(),posPHC.z());
 //
 // }

}

