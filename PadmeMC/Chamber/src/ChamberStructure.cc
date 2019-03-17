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

  if (fChamberExists) {

    // Create the thin window membrane in front of ECal with its flange
    //CreateECalAlThinWindow();
    CreateECalCarbonThinWindow();

    // Create crossed pipes in the target area
    CreateTargetPipes();

    // Create junction pipe between cross and vacuum chamber
    CreateJunctionPipe();

    // Create porthole caps for both section of the chamber
    CreatePortholeCaps();

    // Create cap for large porthole iin front of TimePix
    // Includes round cap with thin Mylar window
    CreateTPixPortholeCap();

  }

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

void ChamberStructure::CreateECalCarbonThinWindow()
{

  ///////////////////////////////////////////////////////
  // Thin Carbon window and Al flange in front of ECal //
  ///////////////////////////////////////////////////////

  ChamberGeometry* geo = ChamberGeometry::GetInstance();

  // Get properties of thin window
  G4double ewR = geo->GetEWCarbonRadius(); // Radius of window
  G4double ewT = geo->GetEWCarbonThick(); // Thickness of window
  G4double ewC = geo->GetEWCarbonConvexity(); // Convexity at window center

  // Get properties of flange
  G4double ewFRIn = geo->GetEWAlFlangeRIn();
  G4double ewFROut = geo->GetEWAlFlangeROut();
  G4double ewFThick = geo->GetEWAlFlangeThick();

  // Get Z coordinate of front face of flange
  G4double efFFrontZ = geo->GetEWAlFrontFacePosZ();

  G4VisAttributes alVisAttr = G4VisAttributes(G4Colour::Blue());
  //if ( ! fChamberIsVisible ) alVisAttr = G4VisAttributes::Invisible;

  G4VisAttributes cVisAttr = G4VisAttributes(G4Colour::Yellow());
  //if ( ! fChamberIsVisible ) cVisAttr = G4VisAttributes::Invisible;

  // Compute thin window sphere's radius and z position of center
  G4double ewr1 = (ewR*ewR+ewC*ewC)/(2.*ewC);
  //G4double ewz1 = efFBackZ+(ewr1-ewC);
  G4double ewth1 = asin(ewR/ewr1);

  // Create flange around thin window
  G4Tubs* solidEWFlange = new G4Tubs("EWFlange",ewFRIn,ewFROut,0.5*ewFThick,0.*deg,360.*deg);
  G4LogicalVolume* logicalEWFlange = new G4LogicalVolume(solidEWFlange,G4Material::GetMaterial("G4_Al"), "ChamberECalWindowFlange",0,0,0);
  logicalEWFlange->SetVisAttributes(alVisAttr);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,efFFrontZ+0.5*ewFThick),logicalEWFlange,"ChamberECalWindowFlange",fMotherVolume,false,0,true);

  // Create the thin window spherical cap and subtract flange to smooth its edge
  G4Sphere* solidEWSphere = new G4Sphere("EWSphere",ewr1,ewr1+ewT,0.*deg,360.*deg,180.*deg-ewth1,ewth1);
  G4ThreeVector spherePos = G4ThreeVector(0.,0.,0.5*ewFThick+ewr1-ewC);
  G4SubtractionSolid* solidEWindow = new G4SubtractionSolid("ChamberECalWindow",solidEWSphere,solidEWFlange,0,G4ThreeVector(0.,0.,-0.5*ewFThick-ewr1+ewC));
  G4LogicalVolume* logicalEWindow = new G4LogicalVolume(solidEWindow,G4Material::GetMaterial("G4_C"), "ChamberECalWindow",0,0,0);
  logicalEWindow->SetVisAttributes(cVisAttr);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,efFFrontZ+ewFThick+ewr1-ewC),logicalEWindow,"ChamberECalWindow",fMotherVolume,false,0,true);

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
  G4double cpzPosZ = geo->GetCPZPosZ();

  G4Tubs* solidCPZe = new G4Tubs("CPZe",0.,cpzROut,0.5*cpzLen,0.*deg,360.*deg);
  G4Tubs* solidCPZi = new G4Tubs("CPZi",0.,cpzRIn,0.5*cpzLen+1.*mm,0.*deg,360.*deg);

  G4double cpxRIn = geo->GetCPXRIn();
  G4double cpxROut = geo->GetCPXROut();
  G4double cpxLen = geo->GetCPXLength();

  G4Tubs* solidCPXe = new G4Tubs("CPXe",0.,cpxROut,0.5*cpxLen,0.*deg,360.*deg);
  G4Tubs* solidCPXi = new G4Tubs("CPXi",0.,cpxRIn,0.5*cpxLen+1.*mm,0.*deg,360.*deg);

  G4RotationMatrix* rotCPX = new G4RotationMatrix;
  rotCPX->rotateY(90.*deg);

  G4UnionSolid* solidCP1 = new G4UnionSolid("VCCP1",solidCPZe,solidCPXe,rotCPX,G4ThreeVector(0.,0.,0.));
  G4SubtractionSolid* solidCP2 = new G4SubtractionSolid("VCCP2",solidCP1,solidCPZi,0,G4ThreeVector(0.,0.,0.));
  G4SubtractionSolid* solidCP3 = new G4SubtractionSolid("VCCP3",solidCP2,solidCPXi,rotCPX,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicalCP = new G4LogicalVolume(solidCP3,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"VCCP",0,0,0);
  logicalCP->SetVisAttributes(steelVisAttr);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,cpzPosZ),logicalCP,"CrossPipeSteel",fMotherVolume,false,0,true);

  // Create flanges for crossed pipe

  G4double flangezRIn = geo->GetCPZFlangeRIn();
  G4double flangezROut = geo->GetCPZFlangeROut();
  G4double flangezThick = geo->GetCPZFlangeThick();
  G4Tubs* solidFlangeZ = new G4Tubs("JunFlangeZ",flangezRIn,flangezROut,0.5*flangezThick,0.*deg,360.*deg);
  G4LogicalVolume* logicalFlangeZ = new G4LogicalVolume(solidFlangeZ,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"JunFlangeZ",0,0,0);
  logicalFlangeZ->SetVisAttributes(steelVisAttr);
  G4double flangez0PosZ = cpzPosZ-0.5*cpzLen+0.5*flangezThick;
  new G4PVPlacement(0,G4ThreeVector(0.,0.,flangez0PosZ),logicalFlangeZ,"CPZFlange",fMotherVolume,false,0,true);
  G4double flangez1PosZ = cpzPosZ+0.5*cpzLen-0.5*flangezThick;
  new G4PVPlacement(0,G4ThreeVector(0.,0.,flangez1PosZ),logicalFlangeZ,"CPZFlange",fMotherVolume,false,1,true);

  G4double flangexRIn = geo->GetCPXFlangeRIn();
  G4double flangexROut = geo->GetCPXFlangeROut();
  G4double flangexThick = geo->GetCPXFlangeThick();
  G4Tubs* solidFlangeX = new G4Tubs("JunFlangeX",flangexRIn,flangexROut,0.5*flangexThick,0.*deg,360.*deg);
  G4LogicalVolume* logicalFlangeX = new G4LogicalVolume(solidFlangeX,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"JunFlangeX",0,0,0);
  logicalFlangeX->SetVisAttributes(steelVisAttr);
  G4double flangex0PosX = -0.5*cpxLen+0.5*flangexThick;
  G4double flangex0PosZ = cpzPosZ;
  new G4PVPlacement(rotCPX,G4ThreeVector(flangex0PosX,0.,flangex0PosZ),logicalFlangeX,"CPXFlange",fMotherVolume,false,0,true);
  G4double flangex1PosX = +0.5*cpxLen-0.5*flangexThick;
  G4double flangex1PosZ = cpzPosZ;
  new G4PVPlacement(rotCPX,G4ThreeVector(flangex1PosX,0.,flangex1PosZ),logicalFlangeX,"CPXFlange",fMotherVolume,false,1,true);

}

void ChamberStructure::CreateJunctionPipe()
{

  ChamberGeometry* geo = ChamberGeometry::GetInstance();

  G4VisAttributes steelVisAttr = G4VisAttributes(G4Colour::Grey());
  if ( ! fChamberIsVisible ) steelVisAttr = G4VisAttributes::Invisible;

  // Junction pipe

  G4double junRIn  = geo->GetJunRIn();
  G4double junROut = geo->GetJunROut();
  G4double junLen  = geo->GetJunLength();
  G4double junPosZ = geo->GetJunPosZ();

  G4Tubs* solidJun = new G4Tubs("JunPipe",junRIn,junROut,0.5*junLen,0.*deg,360.*deg);
  G4LogicalVolume* logicalJun = new G4LogicalVolume(solidJun,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"JunPipe",0,0,0);
  logicalJun->SetVisAttributes(steelVisAttr);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,junPosZ),logicalJun,"JunctionPipe",fMotherVolume,false,0,true);

  printf("Junction pipe RIn %.1fmm Rout %.1fmm Zlen %.3fmm Zpos %.3fmm\n",junRIn/mm,junROut/mm,junLen/mm,junPosZ/mm);

  // Add flanges
  G4double flangeRIn = geo->GetJunFlangeRIn();
  G4double flangeROut = geo->GetJunFlangeROut();
  G4double flangeThick = geo->GetJunFlangeThick();
  G4Tubs* solidFlange = new G4Tubs("JunFlange",flangeRIn,flangeROut,0.5*flangeThick,0.*deg,360.*deg);
  G4LogicalVolume* logicalFlange = new G4LogicalVolume(solidFlange,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"JunFlange",0,0,0);
  logicalFlange->SetVisAttributes(steelVisAttr);
  G4double flange0PosZ = junPosZ-0.5*junLen+0.5*flangeThick;
  new G4PVPlacement(0,G4ThreeVector(0.,0.,flange0PosZ),logicalFlange,"JunctionFlange",fMotherVolume,false,0,true);
  G4double flange1PosZ = junPosZ+0.5*junLen-0.5*flangeThick;
  new G4PVPlacement(0,G4ThreeVector(0.,0.,flange1PosZ),logicalFlange,"JunctionFlange",fMotherVolume,false,1,true);

}

void ChamberStructure::CreatePortholeCaps()
{

  ChamberGeometry* geo = ChamberGeometry::GetInstance();

  G4VisAttributes steelVisAttr = G4VisAttributes(G4Colour(0.4,0.4,0.4)); // Dark gray
  if ( ! fChamberIsVisible ) steelVisAttr = G4VisAttributes::Invisible;

  G4int nPH = geo->GetPHCapNumber();
  for (G4int iPH=0; iPH<nPH; iPH++) {
    char phcN[7];
    sprintf(phcN,"PHCap%d",iPH);
    G4double phcR = geo->GetPHCapRadius(iPH);
    G4double phcT = geo->GetPHCapThick(iPH);
    G4Tubs* solidPHC = new G4Tubs(phcN,0.,phcR,0.5*phcT,0.*deg,360.*deg);
    G4LogicalVolume* logicalPHC = new G4LogicalVolume(solidPHC,G4Material::GetMaterial("G4_STAINLESS-STEEL"),phcN,0,0,0);
    logicalPHC->SetVisAttributes(steelVisAttr);

    G4ThreeVector posPHC = geo->GetPHCapPos(iPH);
    G4RotationMatrix* rotPHC = new G4RotationMatrix;
    rotPHC->rotateY(90.*deg);
    new G4PVPlacement(rotPHC,posPHC,logicalPHC,phcN,fMotherVolume,false,0,true);

    printf("Porthole cap %d (%s) radius %f thick %f position %f %f %f\n",iPH,phcN,phcR,phcT,posPHC.x(),posPHC.y(),posPHC.z());

  }

}
void ChamberStructure::CreateTPixPortholeCap()
{

  ChamberGeometry* geo = ChamberGeometry::GetInstance();

  G4VisAttributes steelVisAttr = G4VisAttributes(G4Colour(0.4,0.4,0.4)); // Dark gray
  if ( ! fChamberIsVisible ) steelVisAttr = G4VisAttributes::Invisible;

  // Add 1.5mm to avoid overlap between extrnal vacuum chamber and large rectangular porthole cap
  G4double phExtraThick = 1.5*mm;

  // Create rectangular cap
  G4double phcW = geo->GetTPPHCapWidth();
  G4double phcH = geo->GetTPPHCapHeight();
  G4double phcT = geo->GetTPPHCapThick();
  G4Box* solidPHC1 = new G4Box("TPPHCAP1",0.5*phcW,0.5*phcH,0.5*phcT);
  printf("Rectangular flange size %6.1f x %6.1f mm2 thick %4.1f mm\n",phcW/mm,phcH/mm,phcT/mm);

  // Carve beam exit hole in cap
  G4double phhR = geo->GetTPPHHoleRadius();
  G4double phhD = geo->GetTPPHHoleDist();
  G4Tubs* solidPHH = new G4Tubs("TPPHHole",0.,phhR,0.5*phcT+1.*mm,0.*deg,360.*deg);
  G4ThreeVector posPHH = G4ThreeVector(-0.5*phcW+phhD,0.,0.);
  G4SubtractionSolid* solidPHC2 = new G4SubtractionSolid("TPPHCAP2",solidPHC1,solidPHH,0,posPHH);
  printf("Beam exit (TPix) porthole cap radius %5.1f mm, center at %5.1f mm from flange border\n",phhR/mm,phhD/mm);

  // Carve service hole in cap
  G4double phhsR = geo->GetTPPHHoleSRadius();
  G4double phhsD = geo->GetTPPHHoleSDist();
  G4Tubs* solidPHHS = new G4Tubs("TPPHHoleS",0.,phhsR,0.5*phcT+1.*mm,0.*deg,360.*deg);
  G4ThreeVector posPHHS = G4ThreeVector(-0.5*phcW+phhsD,0.,0.);
  G4SubtractionSolid* solidPHC = new G4SubtractionSolid("TPPHCap",solidPHC2,solidPHHS,0,posPHHS);
  printf("Service porthole cap radius %5.1f mm, center at %5.1f mm from flange border\n",phhsR/mm,phhsD/mm);

  G4LogicalVolume* logicalPHC = new G4LogicalVolume(solidPHC,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"TPPHCap",0,0,0);
  logicalPHC->SetVisAttributes(steelVisAttr);

  G4ThreeVector corner = geo->GetVCBackFaceCorner();
  G4double angle = geo->GetVCBackFaceAngle();

  G4double phcPosX = corner.x()+0.5*phcW*cos(angle)+(phExtraThick+0.5*phcT)*sin(angle);
  G4double phcPosY = 0.;
  G4double phcPosZ = corner.z()-0.5*phcW*sin(angle)+(phExtraThick+0.5*phcT)*cos(angle);
  G4ThreeVector posPHC = G4ThreeVector(phcPosX,phcPosY,phcPosZ);

  G4RotationMatrix* rotPHC = new G4RotationMatrix;
  rotPHC->rotateY(-angle);

  new G4PVPlacement(rotPHC,posPHC,logicalPHC,"TPPHCap",fMotherVolume,false,0,true);

  // Rectangular flange which will host the thin window
  G4double flangeW = geo->GetTPPHFlangeWidth();
  G4double flangeH = geo->GetTPPHFlangeHeight();
  G4double flangeT = geo->GetTPPHFlangeThick();
  printf("Timepix flange size %5.1f x %5.1f mm2, thick %3.1f mm\n",flangeW/mm,flangeH/mm,flangeT/mm);

  // Thin window
  G4double windR = geo->GetTPPHWindRadius();
  G4double windW = geo->GetTPPHWindWidth();
  G4double windT = geo->GetTPPHWindThick();
  printf("Timepix thin window width %5.1f mm, radius %5.1f mm, thick %3.1f mm\n",windW/mm,windR/mm,windT/mm);

  // Create thin window by carving hole on back side of rectangular flange leaving a thin layer of aluminum
  G4Box* solidPHF1 = new G4Box("TPPHFlg1",0.5*flangeW,0.5*flangeH,0.5*flangeT);

  G4Box* solidWindB = new G4Box("WindB",0.5*windW,windR,0.5*flangeT);
  G4ThreeVector posWindB  = G4ThreeVector(0.,0.,windT);

  G4Tubs* solidWindT = new G4Tubs("WindT",0.,windR,0.5*flangeT,0.*deg,360.*deg);
  G4ThreeVector posWindT1 = G4ThreeVector(-0.5*windW-0.1*mm,0.,windT);
  G4ThreeVector posWindT2 = G4ThreeVector(+0.5*windW+0.1*mm,0.,windT);

  G4SubtractionSolid* solidPHF2 = new G4SubtractionSolid("TPPHFlg2",  solidPHF1,solidWindB,0,posWindB);
  G4SubtractionSolid* solidPHF3 = new G4SubtractionSolid("TPPHFlg3",  solidPHF2,solidWindT,0,posWindT1);
  G4SubtractionSolid* solidPHF  = new G4SubtractionSolid("TPPHFlange",solidPHF3,solidWindT,0,posWindT2);

  G4LogicalVolume* logicalFlange = new G4LogicalVolume(solidPHF,G4Material::GetMaterial("G4_Al"),"TPPHFlange",0,0,0);
  logicalFlange->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));

  //// Carve hole for Mylar window in circular cap
  //G4double windR = geo->GetTPPHWindRadius();
  //G4double windW = geo->GetTPPHWindWidth();
  //G4Tubs* solidWindT = new G4Tubs("WindT",0.,windR,0.5*circT+1.*mm,0.*deg,360.*deg);
  //G4Box* solidWindB = new G4Box("WindB",0.5*windW,windR,0.5*circT+1.*mm);
  //G4ThreeVector posWindT1 = G4ThreeVector(-0.5*windW,0.,0.);
  //G4ThreeVector posWindT2 = G4ThreeVector(+0.5*windW,0.,0.);
  //G4ThreeVector posWindB  = G4ThreeVector(0.,0.,0.);
  //G4SubtractionSolid* solidCirc1 = new G4SubtractionSolid("TPPHCirc1",solidCirc,solidWindT,0,posWindT1);
  //G4SubtractionSolid* solidCirc2 = new G4SubtractionSolid("TPPHCirc2",solidCirc1,solidWindT,0,posWindT2);
  //G4SubtractionSolid* solidCirc3 = new G4SubtractionSolid("TPPHCirc3",solidCirc2,solidWindB,0,posWindB);

  //G4LogicalVolume* logicalCirc = new G4LogicalVolume(solidCirc3,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"TPPHCirc",0,0,0);
  //logicalCirc->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));

  // Position flange in front of circular porthole
  //G4double flangePosX = corner.x()+phhD*cos(angle)+(1.5*mm+phcT+0.5*flangeT)*sin(angle);
  //G4double flangePosY = 0.;
  //G4double flangePosZ = corner.z()-phhD*sin(angle)+(1.5*mm+phcT+0.5*circT)*cos(angle);
  G4double flangePosX = corner.x()+phhD*cos(angle)+(phExtraThick+phcT+0.5*flangeT)*sin(angle);
  G4double flangePosY = 0.;
  G4double flangePosZ = corner.z()-phhD*sin(angle)+(phExtraThick+phcT+0.5*flangeT)*cos(angle);
  G4ThreeVector posFlange = G4ThreeVector(flangePosX,flangePosY,flangePosZ);

  G4RotationMatrix* rotFlange = new G4RotationMatrix;
  rotFlange->rotateY(-angle);

  new G4PVPlacement(rotFlange,posFlange,logicalFlange,"TPPHFlange",fMotherVolume,false,0,true);

  // Create and position steel flange in front of service hole
  G4double sflangeR = geo->GetTPPHSFlangeRadius();
  G4double sflangeT = geo->GetTPPHSFlangeThick();
  G4Tubs* solidSFlange = new G4Tubs("SFlange",0.,sflangeR,0.5*sflangeT,0.*deg,360.*deg);
  G4LogicalVolume* logicalSFlange = new G4LogicalVolume(solidSFlange,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"TPPHSFlange",0,0,0);
  logicalSFlange->SetVisAttributes(steelVisAttr);
  G4RotationMatrix* rotSFlange = new G4RotationMatrix;
  rotSFlange->rotateY(-angle);
  G4double sflangePosX = corner.x()+phhsD*cos(angle)+(phExtraThick+phcT+0.5*sflangeT)*sin(angle);
  G4double sflangePosY = 0.;
  G4double sflangePosZ = corner.z()-phhsD*sin(angle)+(phExtraThick+phcT+0.5*sflangeT)*cos(angle);
  G4ThreeVector posSFlange = G4ThreeVector(sflangePosX,sflangePosY,sflangePosZ);
  new G4PVPlacement(rotSFlange,posSFlange,logicalSFlange,"TPPHFlangeS",fMotherVolume,false,0,true);

  //// Create Mylar window
  //G4double windT = geo->GetTPPHWindThick();
  //G4Box* solidMylarB = new G4Box("MylarB",0.5*windW,windR,0.5*windT);
  //G4Tubs* solidMylarT1 = new G4Tubs("MylarT1",0.,windR,0.5*windT, 90.*deg,180.*deg);
  //G4Tubs* solidMylarT2 = new G4Tubs("MylarT2",0.,windR,0.5*windT,-90.*deg,180.*deg);
  ////G4ThreeVector posMylarT1 = G4ThreeVector(-0.5*windW-1.*mm,0.,0.);
  ////G4ThreeVector posMylarT2 = G4ThreeVector(+0.5*windW+1.*mm,0.,0.);
  //G4ThreeVector posMylarT1 = G4ThreeVector(-0.5*windW,0.,0.);
  //G4ThreeVector posMylarT2 = G4ThreeVector(+0.5*windW,0.,0.);
  //G4UnionSolid* solidMylar1 = new G4UnionSolid("Mylar1",solidMylarB,solidMylarT1,0,posMylarT1);
  //G4UnionSolid* solidMylar2 = new G4UnionSolid("Mylar2",solidMylar1,solidMylarT2,0,posMylarT2);
  //
  //G4LogicalVolume* logicalMylar = new G4LogicalVolume(solidMylar2,G4Material::GetMaterial("G4_MYLAR"),"TPPHMylar",0,0,0);
  //logicalMylar->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));
  //
  //G4double mylarPosX = corner.x()+phhD*cos(angle)+(1.5*mm+phcT+circT+0.5*windT)*sin(angle);
  //G4double mylarPosY = 0.;
  //G4double mylarPosZ = corner.z()-phhD*sin(angle)+(1.5*mm+phcT+circT+0.5*windT)*cos(angle);
  //G4ThreeVector posMylar = G4ThreeVector(mylarPosX,mylarPosY,mylarPosZ);
  //
  //G4RotationMatrix* rotMylar = new G4RotationMatrix;
  //rotMylar->rotateY(-angle);
  //
  //new G4PVPlacement(rotMylar,posMylar,logicalMylar,"TPPHMylar",fMotherVolume,false,0,true);

  //// Create stop flange for Mylar window
  //G4double stopR = geo->GetTPPHStopRadius();
  //G4double stopW = geo->GetTPPHStopWidth();
  //G4double stopT = geo->GetTPPHStopThick();
  //
  //G4ThreeVector posStopT1 = G4ThreeVector(-0.5*stopW,0.,0.);
  //G4ThreeVector posStopT2 = G4ThreeVector(+0.5*stopW,0.,0.);
  //
  //G4Box* solidStopBL = new G4Box("StopBL",0.5*stopW,stopR,0.5*stopT);
  //G4Tubs* solidStopTL1 = new G4Tubs("StopTL1",0.,stopR,0.5*stopT, 90.*deg,180.*deg);
  //G4Tubs* solidStopTL2 = new G4Tubs("StopTL2",0.,stopR,0.5*stopT,-90.*deg,180.*deg);
  //G4UnionSolid* solidStopL1 = new G4UnionSolid("StopL1",solidStopBL,solidStopTL1,0,posStopT1);
  //G4UnionSolid* solidStopL2 = new G4UnionSolid("StopL2",solidStopL1,solidStopTL2,0,posStopT2);
  //
  //G4Box* solidStopBS = new G4Box("StopBS",0.5*stopW,windR,0.5*stopT+1.*mm);
  //G4Tubs* solidStopTS1 = new G4Tubs("StopTS1",0.,windR,0.5*stopT+1.*mm, 89.*deg,182.*deg);
  //G4Tubs* solidStopTS2 = new G4Tubs("StopTS2",0.,windR,0.5*stopT+1.*mm,-91.*deg,182.*deg);
  //G4UnionSolid* solidStopS1 = new G4UnionSolid("StopS1",solidStopBS,solidStopTS1,0,posStopT1);
  //G4UnionSolid* solidStopS2 = new G4UnionSolid("StopS2",solidStopS1,solidStopTS2,0,posStopT2);
  //
  //G4SubtractionSolid* solidStop = new G4SubtractionSolid("TPPHStopFlange",solidStopL2,solidStopS2,0,G4ThreeVector(0.,0.,0.));
  //
  //G4LogicalVolume* logicalStop = new G4LogicalVolume(solidStop,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"TPPHStopFlange",0,0,0);
  //logicalStop->SetVisAttributes(G4VisAttributes(G4Colour::Green()));
  //
  //G4double stopPosX = corner.x()+phhD*cos(angle)+(1.5*mm+phcT+circT+windT+0.5*stopT)*sin(angle);
  //G4double stopPosY = 0.;
  //G4double stopPosZ = corner.z()-phhD*sin(angle)+(1.5*mm+phcT+circT+windT+0.5*stopT)*cos(angle);
  //G4ThreeVector posStop = G4ThreeVector(stopPosX,stopPosY,stopPosZ);
  //
  //G4RotationMatrix* rotStop = new G4RotationMatrix;
  //rotStop->rotateY(-angle);
  //
  //new G4PVPlacement(rotStop,posStop,logicalStop,"TPPHStopFlange",fMotherVolume,false,0,true);

}
