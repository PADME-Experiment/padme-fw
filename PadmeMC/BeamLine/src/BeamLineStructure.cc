// BeamLineStructure.cc
// --------------------------------------------------------------
// History:
//
// Created by Mauro Raggi (mauro.raggi@roma1.infn.it) 2019-03-10
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

#include "G4MagneticField.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"

#include "G4UIcommand.hh"

#include "BeamLineGeometry.hh"
#include "BeamLineMessenger.hh"

//#include "G4MagneticField.hh"

using namespace CLHEP;

BeamLineStructure::BeamLineStructure(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{

  fBeamLineMessenger = new BeamLineMessenger(this);

  fBeamLineExists = 1;    // If =0 the physical structure of the chamber is not created
  fBeamLineIsVisible = 1; // If =0 all chamber structures are invisible (debug only)

}

BeamLineStructure::~BeamLineStructure()
{}

void BeamLineStructure::CreateGeometry()
{

  if (fBeamLineExists) {

    // Create DHSTB002 magnet
    CreateDHSTB002Magnet();

    // Create DHSTB002 inner pipe
    CreateMagnetPipe();
    
    // Create the thin window membrane in front of ECal with its flange
    //CreateBeThinWindow();
        
    // Create region in between DHSTB002 and PADME target cross
    // Now demanded to Chamber classes
    //CreateJunctionRegion();

  }

}


void BeamLineStructure::CreateBeThinWindow()
{
  ///////////////////////////////////////////////////////
  // Thin Be 250um window mounted before DHSTB002
  // and pipes in the upstream region
  ///////////////////////////////////////////////////////
  BeamLineGeometry* geo = BeamLineGeometry::GetInstance();

  G4VisAttributes steelVisAttr = G4VisAttributes(G4Colour::Grey());
  if ( ! fBeamLineIsVisible ) steelVisAttr = G4VisAttributes::Invisible;

  G4VisAttributes BeVisAttr = G4VisAttributes(G4Colour::Blue());
  if ( ! fBeamLineIsVisible ) BeVisAttr = G4VisAttributes::Invisible;

  // Get properties of thin window
  G4double BeWThick = geo->GetBeWThick() ; //Thickness of window
  G4double BeWDiam  = geo->GetBeWDiameter() ; //Thickness of window

  // Create flange around thin window
  //  G4double FlThick    = geo->GetBeFlThick();
  G4double FlThick    = 36*mm;
  G4double FlDiameter = geo->GetBeFlDiameter();
  G4RotationMatrix* rotBe= new G4RotationMatrix;

  G4double DN60junRIn  =  53*mm;    // From Drawings
  G4double DN60junROut =  60*mm;    // From Drawings

  G4double UpDN60junLen  = 254*mm;    // From Drawings next to the Be Window
  G4double DwDN60junLen  = 288*mm;    // From Drawings next to the magnet

  G4double junRIn  = 42*mm;    //Diameter  From Drawings
  G4double junROut = 48*mm;    //Diameter  From Drawings
  G4double junLen  = 1664*mm;  //From Drawings

  G4double junDwDN60PosX = UpStreamFlangePosX + DwDN60junLen/2*cos(45*deg)+0.5*FlThick*cos(45*deg);
  G4double junDwDN60PosZ = UpStreamFlangePosZ-DwDN60junLen/2*sin(45*deg)-0.5*FlThick*sin(45*deg);

  G4Tubs* solidDwDN60Jun = new G4Tubs("DwDN60JunPipe",DN60junRIn*0.5,DN60junROut*0.5,DwDN60junLen*0.5,0.*deg,360.*deg);
  G4LogicalVolume* logicalDwDN60Jun = new G4LogicalVolume(solidDwDN60Jun,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"JunPipe",0,0,0);
  logicalDwDN60Jun->SetVisAttributes(steelVisAttr);
  new G4PVPlacement(rotBe,G4ThreeVector(junDwDN60PosX,0.,junDwDN60PosZ),logicalDwDN60Jun,"DwDN60JunctionPipe",fMotherVolume,false,0,true);
  
  G4double junPosX =  UpStreamFlangePosX +DwDN60junLen*cos(45*deg)+junLen/2*cos(45*deg)+0.5*FlThick*cos(45*deg);
  G4double junPosZ =  UpStreamFlangePosZ-DwDN60junLen*sin(45*deg)-junLen/2*sin(45*deg)-0.5*FlThick*sin(45*deg);
  
  G4Tubs* solidJun = new G4Tubs("JunPipe",junRIn*0.5,junROut*0.5,junLen*0.5,0.*deg,360.*deg);
  G4LogicalVolume* logicalJun = new G4LogicalVolume(solidJun,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"JunPipe",0,0,0);
  logicalJun->SetVisAttributes(steelVisAttr);
  new G4PVPlacement(rotBe,G4ThreeVector(junPosX,0.,junPosZ),logicalJun,"JunctionPipe",fMotherVolume,false,0,true);

  G4double junUpDN60PosX =  UpStreamFlangePosX + UpDN60junLen*cos(45*deg)/2 + junLen*cos(45*deg) + DwDN60junLen*cos(45*deg)+0.5*FlThick*cos(45*deg);
  G4double junUpDN60PosZ =  UpStreamFlangePosZ - UpDN60junLen*sin(45*deg)/2 - junLen*sin(45*deg) - DwDN60junLen*sin(45*deg)-0.5*FlThick*sin(45*deg);

  G4Tubs* solidUpDN60Jun = new G4Tubs("UpDN60JunPipe",DN60junRIn*0.5,DN60junROut*0.5,0.5*UpDN60junLen,0.*deg,360.*deg);
  G4LogicalVolume* logicalUpDN60Jun = new G4LogicalVolume(solidUpDN60Jun,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"UpDN60JunPipe",0,0,0);
  logicalJun->SetVisAttributes(steelVisAttr);
  new G4PVPlacement(rotBe,G4ThreeVector(junUpDN60PosX,0.,junUpDN60PosZ),logicalUpDN60Jun,"UpDN60JunctionPipe",fMotherVolume,false,0,true);

  // Create Be window around thin window
  G4Tubs* solidBeWindow = new G4Tubs("BeWind",0.,BeWDiam*0.5,BeWThick*0.5,0.*deg,360.*deg);
  G4LogicalVolume* logicalBeWindow = new G4LogicalVolume(solidBeWindow,G4Material::GetMaterial("G4_Be"), "logicalBeWindow",0,0,0);
  logicalBeWindow->SetVisAttributes(BeVisAttr);

  double BeWPosX=junUpDN60PosX+UpDN60junLen*cos(45*deg)/2+FlThick*cos(45*deg)/2-5.*mm;
  double BeWPosZ=junUpDN60PosZ-UpDN60junLen*sin(45*deg)/2-FlThick*sin(45*deg)/2+5.*mm;
  
  G4Tubs* solidBeFlange = new G4Tubs("solidBeFlange",BeWDiam*0.5,FlDiameter*0.5,FlThick,0.*deg,360.*deg);
  G4LogicalVolume* logicalBeFlange = new G4LogicalVolume(solidBeFlange,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalBeFlange",0,0,0);
  logicalBeFlange->SetVisAttributes(steelVisAttr);
  new G4PVPlacement(rotBe,G4ThreeVector(BeWPosX*mm,0.,BeWPosZ*mm),logicalBeFlange,"BeamLineBeFlange",fMotherVolume,false,0,true);
  rotBe->rotateY(45.*deg);

  // place the BE window 
  if (geo->BeWindowIsEnabled()) {
    printf("Berillium Window positioned at X = %.2f mm Z = %.2f mm\n",BeWPosX/mm,BeWPosZ/mm);
    new G4PVPlacement(rotBe,G4ThreeVector(BeWPosX*mm,0.,BeWPosZ*mm),logicalBeWindow,"BeamLineBeWindow",fMotherVolume,false,0,true);
  }

}

//*******************************************
//  Describes PADME to BTF junction region.
//*******************************************
void BeamLineStructure::CreateJunctionRegion()
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

  G4VisAttributes steelVisAttr   = G4VisAttributes(G4Color::Grey()); // Dark gray
  G4VisAttributes DHSTB002VisAtt = G4VisAttributes(G4Colour::Red());
  if ( ! fBeamLineIsVisible ) {
    steelVisAttr   = G4VisAttributes::Invisible;
    DHSTB002VisAtt = G4VisAttributes::Invisible;
  }

  // Create magnet yoke
  G4double yokeMinR = geo->GetDHSTB002MinRadius();
  G4double yokeMaxR = geo->GetDHSTB002MaxRadius();
  G4double yokeSizeY = geo->GetDHSTB002SizeY();
  G4double yokeAngle = geo->GetDHSTB002AngularSpan();

  G4Tubs* solidDHSTB002Iron = new G4Tubs("solidDHSTB002Iron",yokeMinR,yokeMaxR,0.5*yokeSizeY,0.*deg,yokeAngle);

  // Create three parts of H-shaped internal hole
  G4double holeCenterR = geo->GetDHSTB002CenterRadius();
  G4double holeL1 = geo->GetDHSTB002L1();
  G4double holeL2 = geo->GetDHSTB002L2();
  G4double holeL3 = geo->GetDHSTB002L3();
  G4double holeL4 = geo->GetDHSTB002L4();

  G4double hole1MinR = holeCenterR-0.5*holeL1;
  G4double hole1MaxR = holeCenterR-0.5*holeL2;
  G4double hole1SizeY = holeL3;
  G4Tubs* solidHole1 = new G4Tubs("solidDHSTB002Hole1",hole1MinR,hole1MaxR,0.5*hole1SizeY,
				  0.*deg-0.01*deg,yokeAngle+0.02*deg);

  G4double hole2MinR = holeCenterR-0.5*holeL2-100.*um; // Expand a bit to avoid graphic problems
  G4double hole2MaxR = holeCenterR+0.5*holeL2+100.*um;
  G4double hole2SizeY = holeL4;
  G4Tubs* solidHole2 = new G4Tubs("solidDHSTB002Hole2",hole2MinR,hole2MaxR,0.5*hole2SizeY,
				  0.*deg-0.01*deg,yokeAngle+0.02*deg);

  G4double hole3MinR = holeCenterR+0.5*holeL2;
  G4double hole3MaxR = holeCenterR+0.5*holeL1;
  G4double hole3SizeY = holeL3;
  G4Tubs* solidHole3 = new G4Tubs("solidDHSTB002Hole3",hole3MinR,hole3MaxR,0.5*hole3SizeY,
				  0.*deg-0.01*deg,yokeAngle+0.02*deg);

  G4SubtractionSolid* solidDHSTB002_1 = new G4SubtractionSolid("solidDHSTB002_1",solidDHSTB002Iron,solidHole1,0,G4ThreeVector(0.,0.,0.));
  G4SubtractionSolid* solidDHSTB002_2 = new G4SubtractionSolid("solidDHSTB002_2",solidDHSTB002_1,solidHole2,0,G4ThreeVector(0.,0.,0.));
  G4SubtractionSolid* solidDHSTB002 = new G4SubtractionSolid("solidDHSTB002",solidDHSTB002_2,solidHole3,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicalDHSTB002 = new G4LogicalVolume(solidDHSTB002,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalDHSTB002",0,0,0);
  logicalDHSTB002->SetVisAttributes(DHSTB002VisAtt);

  // Now rotate DHSTB002 to the XZ plane and position it so that its exit is aligned with the PADME Z axis
  G4RotationMatrix* rotDHSTB = new G4RotationMatrix;
  rotDHSTB->rotateX(-90.*deg);
  rotDHSTB->rotateZ(180.*deg);
  G4ThreeVector posDHSTB = G4ThreeVector(holeCenterR,0.,geo->GetDHSTB002ExitPosZ());
  new G4PVPlacement(rotDHSTB,posDHSTB,logicalDHSTB002,"DHSTB002",fMotherVolume,false,0,true);

}

void BeamLineStructure::CreateMagnetPipe()
{

  BeamLineGeometry* geo = BeamLineGeometry::GetInstance();
  G4VisAttributes steelVisAttr   = G4VisAttributes(G4Color::Grey()); // Dark gray
  if ( ! fBeamLineIsVisible ) steelVisAttr   = G4VisAttributes::Invisible;

  // Angular span of the DHSTB002 magnet (45 deg)
  G4double magnetAngle = geo->GetDHSTB002AngularSpan();

  // Create the magnetic volume
  G4double magvolSizeY = geo->GetMagVolSizeY();
  G4double magvolMinR = geo->GetMagVolMinRadius();
  G4double magvolMaxR = geo->GetMagVolMaxRadius();
  G4Tubs* solidMagVol = new G4Tubs("solidMagVol",magvolMinR,magvolMaxR,0.5*magvolSizeY,0.*deg,magnetAngle);
  G4LogicalVolume* logicalMagVol = new G4LogicalVolume(solidMagVol,G4Material::GetMaterial("Vacuum"),"logicalMagVol",0,0,0);
  //logicalMagVol->SetVisAttributes(G4VisAttributes::Invisible);

  // Add a constant magnetic field to the magnetic volume
  G4ThreeVector fieldV(0.,geo->GetDHSTB002MagneticFieldY(),0.);
  G4MagneticField* magField = new G4UniformMagField(fieldV);
  G4FieldManager* localFieldManager = new G4FieldManager(magField);
  logicalMagVol->SetFieldManager(localFieldManager,true);

  // Position the magnetic volume at center of H-shaped hole of DHSTB002
  G4RotationMatrix* rotMagVol = new G4RotationMatrix;
  rotMagVol->rotateX(-90.*deg);
  rotMagVol->rotateZ(180.*deg);
  G4ThreeVector posMagVol = G4ThreeVector(geo->GetDHSTB002CenterRadius(),0.,geo->GetDHSTB002ExitPosZ());
  new G4PVPlacement(rotMagVol,posMagVol,logicalMagVol,"DHSTB002MagneticVolume",fMotherVolume,false,0,true);

  // Create bended section of the rectangular beam pipe and position it inside the magnetic volume

  // Steel pipe
  G4double magBPSizeY = geo->GetMagPipeSizeY();
  G4double magBPMinR = geo->GetMagPipeMinRadius();
  G4double magBPMaxR = geo->GetMagPipeMaxRadius();
  G4Tubs* solidBeamPipeFull = new G4Tubs("solidBeamPipeFull",magBPMinR,magBPMaxR,0.5*magBPSizeY,0.*deg,magnetAngle);

  // Hole inside steel pipe
  G4double magBPHSizeY = geo->GetMagPipeHoleSizeY();
  G4double magBPHMinR = geo->GetMagPipeHoleMinRadius();
  G4double magBPHMaxR = geo->GetMagPipeHoleMaxRadius();
  G4Tubs* solidBeamPipeHole  = new G4Tubs("solidBeamPipeHole",magBPHMinR,magBPHMaxR,0.5*magBPHSizeY,0.*deg-0.001*deg,magnetAngle+0.002*deg);

  // Final pipe (steel with hole inside)
  G4SubtractionSolid* solidBeamPipe = new G4SubtractionSolid("solidBeamPipe",solidBeamPipeFull,solidBeamPipeHole,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicalBeamPipe = new G4LogicalVolume(solidBeamPipe,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalBeamPipe",0,0,0);
  logicalBeamPipe->SetVisAttributes(steelVisAttr);

  // Beam pipe is placed inside the magnetic volume
  new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicalBeamPipe,"DHSTB002MagneticPipe",logicalMagVol,false,0,true);

  // Create the straight section of the beam pipe an dposition it at entrance and exit of magnet

  G4double strPipeSizeX = geo->GetMagPipeSizeX();
  G4double strPipeSizeY = geo->GetMagPipeSizeY();
  G4double strPipeSizeZ = geo->GetMagPipeStraightLength();
  G4double strHoleSizeX = geo->GetMagPipeHoleSizeX();
  G4double strHoleSizeY = geo->GetMagPipeHoleSizeY();
  G4double strHoleSizeZ = geo->GetMagPipeStraightLength()+10.*um;
  G4double strFlangeR = geo->GetMagPipeFlangeRadius();
  G4double strFlangeThick = geo->GetMagPipeFlangeThick();

  G4Box* solidStrFull = new G4Box("solidStrFull",0.5*strPipeSizeX,0.5*strPipeSizeY,0.5*strPipeSizeZ);
  G4Box* solidStrHole = new G4Box("solidStrHole",0.5*strHoleSizeX,0.5*strHoleSizeY,0.5*strHoleSizeZ);
  G4Tubs* solidStrFlange = new G4Tubs("solidStrFlange",0.,strFlangeR,0.5*strFlangeThick,0.*deg,360.*deg);

  G4ThreeVector posFlange = G4ThreeVector(0.,0.,geo->GetMagPipeFlangePosZ());
  G4UnionSolid* solidStrFullFlange = new G4UnionSolid("solidStrFullFlange",solidStrFull,solidStrFlange,0,posFlange);
  G4SubtractionSolid* solidStraightPipe = new G4SubtractionSolid("solidStraightPipe",solidStrFullFlange,solidStrHole,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicalStraightPipe = new G4LogicalVolume(solidStraightPipe,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalStraightPipe",0,0,0);
  logicalStraightPipe->SetVisAttributes(steelVisAttr);

  // Position front straight section
  G4double strFrontPosX = geo->GetMagPipeStraightFrontPosX();
  G4double strFrontPosZ = geo->GetMagPipeStraightFrontPosZ();
  G4double strFrontRotY = geo->GetMagPipeStraightFrontRotY();
  G4RotationMatrix* strFrontRot = new G4RotationMatrix;
  strFrontRot->rotateY(strFrontRotY);
  G4ThreeVector strFrontPos = G4ThreeVector(strFrontPosX,0.,strFrontPosZ);
  new G4PVPlacement(strFrontRot,strFrontPos,logicalStraightPipe,"DHSTB002StraightPipeFront",fMotherVolume,false,0,true);

  // Position back straight section
  G4double strBackPosX = geo->GetMagPipeStraightBackPosX();
  G4double strBackPosZ = geo->GetMagPipeStraightBackPosZ();
  G4double strBackRotY = geo->GetMagPipeStraightBackRotY();
  G4RotationMatrix* strBackRot = new G4RotationMatrix;
  strBackRot->rotateY(strBackRotY);
  G4ThreeVector strBackPos = G4ThreeVector(strBackPosX,0.,strBackPosZ);
  new G4PVPlacement(strBackRot,strBackPos,logicalStraightPipe,"DHSTB002StraightPipeBack",fMotherVolume,false,0,true);
  
//// Gamma line pipe
//G4double GLinSX = geo->GetDHSTB002GLinePipeSizeX();
//G4double GLinSY = geo->GetDHSTB002GLinePipeSizeY();
//G4double GLinSZ = geo->GetDHSTB002GLinePipeSizeZ();
//G4Box*  solidGLinePipeIron  = new G4Box("solidGLinePipeIron",GLinSX/2*mm,GLinSY/2*mm,GLinSZ/2*mm);
//
//G4double GLinPipeGapSX = geo->GetDHSTB002GLinePipeGapSizeX();
//G4double GLinPipeGapSY = geo->GetDHSTB002GLinePipeGapSizeY();
//G4double GLinPipeGapSZ = geo->GetDHSTB002GLinePipeGapSizeZ();
//G4Box*  solidGLinePipeGap  = new G4Box("solidGLinePipeGap",GLinPipeGapSX/2*mm,GLinPipeGapSY/2*mm,GLinPipeGapSZ/2*mm);
//
//G4double GLinePosX=0.;
//G4double GLinePosZ=-5000.;
//
//G4SubtractionSolid* solidGLine = new G4SubtractionSolid("solidGLine",solidGLinePipeIron,solidGLinePipeGap,0,G4ThreeVector(0.,0.,0.));
//G4LogicalVolume* logicalGLine = new G4LogicalVolume(solidGLine,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalGLine",0,0,0);
//logicalGLine->SetVisAttributes(steelVisAttr);
//new G4PVPlacement(rotPipe,G4ThreeVector(0,0.,GLinePosZ),logicalGLine,"GLine",fMotherVolume,false,0,true);
//
//// end of G line flange placement
//new G4PVPlacement(rotPipe,G4ThreeVector(GLinePosX-(GLinSZ/2)*cos(45.*deg)*mm,0.,GLinePosZ+(GLinSZ/2)*sin(45.*deg)*mm),logicalFlange,"BeamLineFlange",fMotherVolume,false,0,true);

}

//double BeamLine::GetFieldValue(const double Point[3],double *Bfield) const
//double BeamLineStructure::GetFieldValue()
//{
//  fBz = 1.0*tesla;
////  Double rmax_sq = sqr(50.*cm);
////  DOuble zmax = 100.*cm;
////
////  Bfield[0] = 0.;
////  Bfield[1] = 0.;
////  if(std::abs(Point[2])<zmax && (sqr(Point[0])+sqr(Point[1]))<rmax_sq)
////  { Bfield[2] = Bz; }
////  else
////  { Bfield[2] = 0.; }
//  return fBz;
//}
