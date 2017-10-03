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
  new G4PVPlacement(0,G4ThreeVector(0.,0.,geo->GetCPZPosZ()),logicalCP,"CrossPipeSteel",fMotherVolume,false,0,true);

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

  // Create rectangular cap
  G4double phcW = geo->GetTPPHCapWidth();
  G4double phcH = geo->GetTPPHCapHeight();
  G4double phcT = geo->GetTPPHCapThick();
  G4Box* solidPHB = new G4Box("TPPHBox",0.5*phcW,0.5*phcH,0.5*phcT);
  printf("Rectangular flange size %f x %f mm2 thick %f mm\n",phcW/mm,phcH/mm,phcT/mm);

  // Carve hole in cap
  G4double phhR = geo->GetTPPHHoleRadius();
  G4double phhD = geo->GetTPPHHoleDist();
  G4Tubs* solidPHH = new G4Tubs("TPPHHole",0.,phhR,0.5*phcT+1.*mm,0.*deg,360.*deg);
  G4ThreeVector posPHH = G4ThreeVector(-0.5*phcW+phhD,0.,0.);
  G4SubtractionSolid* solidPHC = new G4SubtractionSolid("TPPHCap",solidPHB,solidPHH,0,posPHH);
  printf("Center of beam exit (TPix) porthole cap at %f mm from flange border\n",phhD/mm);

  G4LogicalVolume* logicalPHC = new G4LogicalVolume(solidPHC,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"TPPHCap",0,0,0);
  logicalPHC->SetVisAttributes(steelVisAttr);

  G4ThreeVector corner = geo->GetVCBackFaceCorner();
  G4double angle = geo->GetVCBackFaceAngle();

  G4double phcPosX = corner.x()+0.5*phcW*cos(angle)+(0.5*phcT+1.5*mm)*sin(angle);
  G4double phcPosY = 0.;
  G4double phcPosZ = corner.z()-0.5*phcW*sin(angle)+(0.5*phcT+1.5*mm)*cos(angle);
  G4ThreeVector posPHC = G4ThreeVector(phcPosX,phcPosY,phcPosZ);

  G4RotationMatrix* rotPHC = new G4RotationMatrix;
  rotPHC->rotateY(-angle);

  new G4PVPlacement(rotPHC,posPHC,logicalPHC,"TPPHCap",fMotherVolume,false,0,true);

  // Create circular cap
  G4double circR = geo->GetTPPHCircRadius();
  G4double circT = geo->GetTPPHCircThick();
  G4Tubs* solidCirc = new G4Tubs("TPPHCirc",0.,circR,0.5*circT,0.*deg,360.*deg);

  // Carve hole for Mylar window in circular cap
  G4double windR = geo->GetTPPHWindRadius();
  G4double windW = geo->GetTPPHWindWidth();
  G4Tubs* solidWindT = new G4Tubs("WindT",0.,windR,0.5*circT+1.*mm,0.*deg,360.*deg);
  G4Box* solidWindB = new G4Box("WindB",0.5*windW,windR,0.5*circT+1.*mm);
  G4ThreeVector posWindT1 = G4ThreeVector(-0.5*windW,0.,0.);
  G4ThreeVector posWindT2 = G4ThreeVector(+0.5*windW,0.,0.);
  G4ThreeVector posWindB  = G4ThreeVector(0.,0.,0.);
  G4SubtractionSolid* solidCirc1 = new G4SubtractionSolid("TPPHCirc1",solidCirc,solidWindT,0,posWindT1);
  G4SubtractionSolid* solidCirc2 = new G4SubtractionSolid("TPPHCirc2",solidCirc1,solidWindT,0,posWindT2);
  G4SubtractionSolid* solidCirc3 = new G4SubtractionSolid("TPPHCirc3",solidCirc2,solidWindB,0,posWindB);

  G4LogicalVolume* logicalCirc = new G4LogicalVolume(solidCirc3,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"TPPHCirc",0,0,0);
  logicalCirc->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));

  G4double circPosX = corner.x()+phhD*cos(angle)+(1.5*mm+phcT+0.5*circT)*sin(angle);
  G4double circPosY = 0.;
  G4double circPosZ = corner.z()-phhD*sin(angle)+(1.5*mm+phcT+0.5*circT)*cos(angle);
  G4ThreeVector posCirc = G4ThreeVector(circPosX,circPosY,circPosZ);

  G4RotationMatrix* rotCirc = new G4RotationMatrix;
  rotCirc->rotateY(-angle);

  new G4PVPlacement(rotCirc,posCirc,logicalCirc,"TPPHCirc",fMotherVolume,false,0,true);

  // Create Mylar window
  G4double windT = geo->GetTPPHWindThick();
  G4Box* solidMylarB = new G4Box("MylarB",0.5*windW,windR,0.5*windT);
  G4Tubs* solidMylarT1 = new G4Tubs("MylarT1",0.,windR,0.5*windT, 90.*deg,180.*deg);
  G4Tubs* solidMylarT2 = new G4Tubs("MylarT2",0.,windR,0.5*windT,-90.*deg,180.*deg);
  //G4ThreeVector posMylarT1 = G4ThreeVector(-0.5*windW-1.*mm,0.,0.);
  //G4ThreeVector posMylarT2 = G4ThreeVector(+0.5*windW+1.*mm,0.,0.);
  G4ThreeVector posMylarT1 = G4ThreeVector(-0.5*windW,0.,0.);
  G4ThreeVector posMylarT2 = G4ThreeVector(+0.5*windW,0.,0.);
  G4UnionSolid* solidMylar1 = new G4UnionSolid("Mylar1",solidMylarB,solidMylarT1,0,posMylarT1);
  G4UnionSolid* solidMylar2 = new G4UnionSolid("Mylar2",solidMylar1,solidMylarT2,0,posMylarT2);

  G4LogicalVolume* logicalMylar = new G4LogicalVolume(solidMylar2,G4Material::GetMaterial("G4_MYLAR"),"TPPHMylar",0,0,0);
  logicalMylar->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));

  G4double mylarPosX = corner.x()+phhD*cos(angle)+(1.5*mm+phcT+circT+0.5*windT)*sin(angle);
  G4double mylarPosY = 0.;
  G4double mylarPosZ = corner.z()-phhD*sin(angle)+(1.5*mm+phcT+circT+0.5*windT)*cos(angle);
  G4ThreeVector posMylar = G4ThreeVector(mylarPosX,mylarPosY,mylarPosZ);

  G4RotationMatrix* rotMylar = new G4RotationMatrix;
  rotMylar->rotateY(-angle);

  new G4PVPlacement(rotMylar,posMylar,logicalMylar,"TPPHMylar",fMotherVolume,false,0,true);

  // Create stop flange for Mylar window
  G4double stopR = geo->GetTPPHStopRadius();
  G4double stopW = geo->GetTPPHStopWidth();
  G4double stopT = geo->GetTPPHStopThick();

  G4ThreeVector posStopT1 = G4ThreeVector(-0.5*stopW,0.,0.);
  G4ThreeVector posStopT2 = G4ThreeVector(+0.5*stopW,0.,0.);

  G4Box* solidStopBL = new G4Box("StopBL",0.5*stopW,stopR,0.5*stopT);
  G4Tubs* solidStopTL1 = new G4Tubs("StopTL1",0.,stopR,0.5*stopT, 90.*deg,180.*deg);
  G4Tubs* solidStopTL2 = new G4Tubs("StopTL2",0.,stopR,0.5*stopT,-90.*deg,180.*deg);
  G4UnionSolid* solidStopL1 = new G4UnionSolid("StopL1",solidStopBL,solidStopTL1,0,posStopT1);
  G4UnionSolid* solidStopL2 = new G4UnionSolid("StopL2",solidStopL1,solidStopTL2,0,posStopT2);

  G4Box* solidStopBS = new G4Box("StopBS",0.5*stopW,windR,0.5*stopT+1.*mm);
  G4Tubs* solidStopTS1 = new G4Tubs("StopTS1",0.,windR,0.5*stopT+1.*mm, 89.*deg,182.*deg);
  G4Tubs* solidStopTS2 = new G4Tubs("StopTS2",0.,windR,0.5*stopT+1.*mm,-91.*deg,182.*deg);
  G4UnionSolid* solidStopS1 = new G4UnionSolid("StopS1",solidStopBS,solidStopTS1,0,posStopT1);
  G4UnionSolid* solidStopS2 = new G4UnionSolid("StopS2",solidStopS1,solidStopTS2,0,posStopT2);

  G4SubtractionSolid* solidStop = new G4SubtractionSolid("TPPHStopFlange",solidStopL2,solidStopS2,0,G4ThreeVector(0.,0.,0.));

  G4LogicalVolume* logicalStop = new G4LogicalVolume(solidStop,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"TPPHStopFlange",0,0,0);
  logicalStop->SetVisAttributes(G4VisAttributes(G4Colour::Green()));

  G4double stopPosX = corner.x()+phhD*cos(angle)+(1.5*mm+phcT+circT+windT+0.5*stopT)*sin(angle);
  G4double stopPosY = 0.;
  G4double stopPosZ = corner.z()-phhD*sin(angle)+(1.5*mm+phcT+circT+windT+0.5*stopT)*cos(angle);
  G4ThreeVector posStop = G4ThreeVector(stopPosX,stopPosY,stopPosZ);

  G4RotationMatrix* rotStop = new G4RotationMatrix;
  rotStop->rotateY(-angle);

  new G4PVPlacement(rotStop,posStop,logicalStop,"TPPHStopFlange",fMotherVolume,false,0,true);

}
