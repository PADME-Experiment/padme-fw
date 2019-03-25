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

//#include "G4MagneticField.hh"

using namespace CLHEP;

BeamLineStructure::BeamLineStructure(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{
  fBeamLineExists = 1;    // If =0 the physical structure of the chamber is not created
  fBeamLineIsVisible = 1; // If =0 all chamber structures are invisible (debug only)
  fBz=1*tesla;
}

BeamLineStructure::~BeamLineStructure()
{}

void BeamLineStructure::CreateGeometry()
{

  // The main parts of the Vacuum BeamLine are imported in DetectorConstruction.cc from GDML files
  // Here we only define flanges, portholes and other components of the chamber
  if (fBeamLineExists) {

    // Create DHSTB002 magnet
    CreateDHSTB002Magnet();

    // Create DHSTB002 inner pipe
    CreateMagnetPipe();
    
    // Create the thin window membrane in front of ECal with its flange
    CreateBeThinWindow();
        
    // Create region in between DHSTB002 and PADME target cross
    CreateJunctionRegion(); 
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

  double BeWPosX=junUpDN60PosX+UpDN60junLen*cos(45*deg)/2+FlThick*cos(45*deg)/2;
  double BeWPosZ=junUpDN60PosZ-UpDN60junLen*sin(45*deg)/2-FlThick*sin(45*deg)/2;
  
  G4Tubs* solidBeFlange = new G4Tubs("solidBeFlange",BeWDiam*0.5,FlDiameter*0.5,FlThick,0.*deg,360.*deg);
  G4LogicalVolume* logicalBeFlange = new G4LogicalVolume(solidBeFlange,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalBeFlange",0,0,0);
  logicalBeFlange->SetVisAttributes(steelVisAttr);
  new G4PVPlacement(rotBe,G4ThreeVector(BeWPosX*mm,0.,BeWPosZ*mm),logicalBeFlange,"BeamLineBeFlange",fMotherVolume,false,0,true);
  rotBe->rotateY(45.*deg);

  // place the BE window 
  //  new G4PVPlacement(rotBe,G4ThreeVector(BeWPosX*mm,0.,BeWPosZ*mm),logicalBeWindow,"BeamLineBeWindow",fMotherVolume,false,0,true);
  std::cout<<"******************************************************************************** Flange X "<<BeWPosX<<std::endl;
  std::cout<<"******************************************************************************** Flange Z "<<BeWPosZ<<std::endl;
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
  class G4MagneticField;
  BeamLineGeometry* geo = BeamLineGeometry::GetInstance();

  //  G4VisAttributes steelVisAttr   = G4VisAttributes(G4Colour(0.4,0.4,0.4)); // Dark gray
  G4VisAttributes steelVisAttr   = G4VisAttributes(G4Color::Grey()); // Dark gray
  G4VisAttributes DHSTB002VisAtt = G4VisAttributes(G4Colour::Red());
  if ( ! fBeamLineIsVisible ) steelVisAttr   = G4VisAttributes::Invisible;
  if ( ! fBeamLineIsVisible ) DHSTB002VisAtt = G4VisAttributes::Invisible;

  G4double DHSTB002Thick    = geo->GetDHSTB002Thick();
  G4double DHSTB002Radius   = geo->GetDHSTB002Radius();
  G4double DHSTB002InnHole  = geo->GetDHSTB002InnHole();
  G4double GapCenter        = geo->GetDHSTB002CenterRadius();
  
  G4double DHSTB002PosZ     = geo->GetDHSTB002PosZ();

  G4RotationMatrix* rotDHSTB = new G4RotationMatrix;
  rotDHSTB->rotateX(90.*deg);
  rotDHSTB->rotateZ(180.*deg);

  G4Tubs* solidDHSTB002Iron = new G4Tubs("solidDHSTB002Iron",DHSTB002InnHole,DHSTB002Radius,DHSTB002Thick*0.5,315.*deg,45.*deg);

  //GAPs angulare extension greater than the iron to avoid surface problems
  G4double GapStartAngle = 314;
  G4double GapDeltaAngle = 46;

  G4double Gap1Thick    = geo->GetDHSTB002Gap1Thick();
  G4double Gap1Radius   = geo->GetDHSTB002Gap1Radius();
  G4double Gap1InnHole  = geo->GetDHSTB002Gap1InnHole();

  G4double Gap2Thick     = geo->GetDHSTB002Gap2Thick();
  G4double Gap2Radius    = geo->GetDHSTB002Gap2Radius();
  G4double Gap2InnHole   = geo->GetDHSTB002Gap2InnHole();

  G4double TGapThick    = geo->GetDHSTB002TGapThick();
  G4double TGapRadius   = geo->GetDHSTB002TGapRadius();
  G4double TGapInnHole  = geo->GetDHSTB002TGapInnHole();

  G4Tubs* solidGap2 = new G4Tubs("solidGap2",Gap2InnHole,Gap2Radius,Gap2Thick*0.5,GapStartAngle*deg,GapDeltaAngle*deg);
  G4Tubs* solidGap1 = new G4Tubs("solidGap1",Gap1InnHole,Gap1Radius,Gap1Thick*0.5,GapStartAngle*deg,GapDeltaAngle*deg);
  G4Tubs* solidTGap = new G4Tubs("solidTGap",TGapInnHole,TGapRadius,TGapThick*0.5,GapStartAngle*deg,GapDeltaAngle*deg);
  
  // the gamma line gap
  G4double GLinGapSX = geo->GetDHSTB002GLineGapSizeX();
  G4double GLinGapSY = geo->GetDHSTB002GLineGapSizeY();
  G4double GLinGapSZ = geo->GetDHSTB002GLineGapSizeZ();
  G4Box*  solidGLineGap  = new G4Box("solidGLineGap",GLinGapSX/2*mm,GLinGapSY/2*mm,GLinGapSZ/2*mm);

  G4double GLinGapPX = 1723.*cos(315+22.5*deg)*mm;
  G4double GLinGapPY = 1723.*sin(315+22.5*deg)*mm;;

  G4UnionSolid* solidG12  = new G4UnionSolid("solidG12",solidGap1,solidGap2,0,G4ThreeVector(0.,0.,0.));
  //  G4UnionSolid* solidGAllGamma = new G4UnionSolid("solidGAllGamma",solidGLineGap,solidG12,0,G4ThreeVector(0.,0.,0.));
  //  G4UnionSolid* solidGAll = new G4UnionSolid("solidGAll",solidTGap,solidGAllGamma,0,G4ThreeVector(GLinGapPX,GLinGapPY,0.));
  G4UnionSolid* solidGAll = new G4UnionSolid("solidGAll",solidTGap,solidG12,0,G4ThreeVector(0.,0.,0.));
  // adding gamma line hole
  
  G4SubtractionSolid* solidDHSTB002 = new G4SubtractionSolid("solidDHSTB002",solidDHSTB002Iron,solidGAll,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicalDHSTB002  = new G4LogicalVolume(solidDHSTB002,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalDHSTB002",0,0,0);
  logicalDHSTB002->SetVisAttributes(DHSTB002VisAtt);
  new G4PVPlacement(rotDHSTB,G4ThreeVector(GapCenter*mm,0.,DHSTB002PosZ*mm),logicalDHSTB002,"BeamLineDHSTB002",fMotherVolume,false,0,true);
}

void BeamLineStructure::CreateMagnetPipe()
{
  BeamLineGeometry* geo = BeamLineGeometry::GetInstance();
  G4VisAttributes steelVisAttr   = G4VisAttributes(G4Color::Grey()); // Dark gray
  if ( ! fBeamLineIsVisible ) steelVisAttr   = G4VisAttributes::Invisible;

  G4double DHSTB002PipeThick    = geo->GetDHSTB002PipeThick();
  G4double DHSTB002PipeRadius   = geo->GetDHSTB002PipeRadius();
  G4double DHSTB002PipeInnHole  = geo->GetDHSTB002PipeInnHole();
  G4double GapCenter            = (DHSTB002PipeRadius-DHSTB002PipeInnHole)/2+DHSTB002PipeInnHole; 

  G4double DHSTB002PosZ            = geo->GetDHSTB002PosZ();

  G4double DHSTB002PipeGapThick    = geo->GetDHSTB002PipeGapThick();
  G4double DHSTB002PipeGapRadius   = geo->GetDHSTB002PipeGapRadius();
  G4double DHSTB002PipeGapInnHole  = geo->GetDHSTB002PipeGapInnHole();

  G4RotationMatrix* rotDHSTB = new G4RotationMatrix;
  rotDHSTB->rotateX(90.*deg);
  rotDHSTB->rotateZ(180.*deg);

  G4Tubs* solidDHSTB002PipeIron = new G4Tubs("solidDHSTB002PipeIron",DHSTB002PipeInnHole,DHSTB002PipeRadius,DHSTB002PipeThick*0.5,315.*deg,45.*deg);
  G4Tubs* solidDHSTB002PipeGap  = new G4Tubs("solidDHSTB002PipeGap",DHSTB002PipeGapInnHole,DHSTB002PipeGapRadius,DHSTB002PipeGapThick*0.5,314.*deg,46.*deg);
  G4SubtractionSolid* solidDHSTB002Pipe = new G4SubtractionSolid("solidDHSTB002Pipe",solidDHSTB002PipeIron,solidDHSTB002PipeGap,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicalDHSTB002Pipe    = new G4LogicalVolume(solidDHSTB002Pipe,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalDHSTB002Pipe",0,0,0);
  G4LogicalVolume* logicalDHSTB002PipeGap = new G4LogicalVolume(solidDHSTB002PipeGap,G4Material::GetMaterial("Vacuum"),"logicalDHSTB002PipeGap",0,0,0);
  logicalDHSTB002Pipe   ->SetVisAttributes(steelVisAttr);
  logicalDHSTB002PipeGap->SetVisAttributes(steelVisAttr);
  // G4LogicalVolume* logicalDHSTB002Pipe = new G4LogicalVolume(solidDHSTB002PipeIron,G4Material::GetMaterial("G4_AIR"),"logicalDHSTB002Pipe",0,0,0);
  new G4PVPlacement(rotDHSTB,G4ThreeVector(GapCenter*mm,0.,DHSTB002PosZ*mm),logicalDHSTB002Pipe,"BeamLineDHSTB002Pipe",fMotherVolume,false,0,true);
  new G4PVPlacement(rotDHSTB,G4ThreeVector(GapCenter*mm,0.,DHSTB002PosZ*mm),logicalDHSTB002PipeGap,"BeamLineDHSTB002PipeGap",fMotherVolume,false,0,true);

  // Let's add a constant magnetic field
  G4ThreeVector fieldV(0.,-1.02*tesla,0.);
  G4MagneticField* magField = new G4UniformMagField(fieldV);
  G4FieldManager* localFieldManager = new G4FieldManager(magField);

  //Magnetized volume inside magnet gap better to put the pipe gap as child volume
  G4double BGapThick    = geo->GetDHSTB002TGapThick()   -0.01*mm;
  G4double BGapRadius   = geo->GetDHSTB002TGapRadius()  -0.01*mm;
  G4double BGapInnHole  = geo->GetDHSTB002TGapInnHole() +0.01*mm;
  G4Tubs* solidDHSTB002MagField  = new G4Tubs("solidDHSTB002MagField",BGapInnHole,BGapRadius,BGapThick*0.5,314*deg,46*deg);
  G4LogicalVolume* logicalDHSTB002MagField = new G4LogicalVolume(solidDHSTB002MagField,G4Material::GetMaterial("Vacuum"),"logicalDHSTB002MagField",0,0,0);
  logicalDHSTB002MagField->SetVisAttributes(steelVisAttr);
  new G4PVPlacement(rotDHSTB,G4ThreeVector(GapCenter*mm,0.,DHSTB002PosZ*mm),logicalDHSTB002MagField,"DHSTB002MagField",fMotherVolume,false,0,true);
  logicalDHSTB002PipeGap->SetFieldManager(localFieldManager,true);
  logicalDHSTB002MagField->SetFieldManager(localFieldManager,true);

  // Strait sections
  G4double  PipeSizeX =  DHSTB002PipeRadius-DHSTB002PipeInnHole;
  G4double  PipeSizeY =  DHSTB002PipeThick;
  G4double  PipeSizeZ =  210*mm;  //from drawings

  G4Box*    solidStraitPipeIron = new G4Box("solidStraitPipeIron",PipeSizeX/2.,PipeSizeY/2.,PipeSizeZ/2.);
  G4Box*    solidStraitPipeGap  = new G4Box("solidStraitPipeGap",(PipeSizeX-6)/2*mm,(PipeSizeY-8)/2*mm,(PipeSizeZ+2)*mm);
  G4SubtractionSolid* solidStraitPipe = new G4SubtractionSolid("solidStraitPipe",solidStraitPipeIron,solidStraitPipeGap,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicalStraitPipe  = new G4LogicalVolume(solidStraitPipe,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalStraitPipe",0,0,0);
  logicalStraitPipe->SetVisAttributes(steelVisAttr);
  new G4PVPlacement(0,G4ThreeVector(0,0.,(DHSTB002PosZ+PipeSizeZ/2)*mm),logicalStraitPipe,"BeamLineStraitPipe",fMotherVolume,false,0,true);

  //Create a DN63 flange using Leibold specs 
  G4double FlThick    = geo->GetBeFlThick();
  G4double FlDiameter = geo->GetBeFlDiameter();

  G4Tubs* solidFlangeIron = new G4Tubs("solidFlangeIron",0.,FlDiameter*0.5,FlThick,0.*deg,360.*deg);
  G4Box*  solidFlangeGap  = new G4Box("solidFlangeGap",((PipeSizeX-6)/2)*mm,((PipeSizeY-8)/2)*mm,(FlThick+2.)*mm);
  G4SubtractionSolid* solidFlange = new G4SubtractionSolid("solidFlange",solidFlangeIron,solidFlangeGap,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicalFlange = new G4LogicalVolume(solidFlange,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalFlange",0,0,0);
  logicalFlange->SetVisAttributes(steelVisAttr);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,(DHSTB002PosZ+PipeSizeZ+FlThick/2)*mm),logicalFlange,"BeamLineFlange",fMotherVolume,false,0,true);

  G4RotationMatrix* rotPipe= new G4RotationMatrix;
  rotPipe->rotateY(45.*deg);

  G4double xpipe = (GapCenter-GapCenter*cos(45*deg)+PipeSizeZ/2*cos(45*deg))*mm;
  G4double zpipe = (DHSTB002PosZ-GapCenter*sin(45*deg)-PipeSizeZ/2*sin(45*deg))*mm;
//  std::cout<<"******************************************************************************** Pipe "<<xpipe<<" X "<<504.+105.*cos(45*deg)<<std::endl;
//  std::cout<<"******************************************************************************** Pipe "<<zpipe<<" Z "<<-3001-1218-105.*sin(45*deg)<<std::endl;
  new G4PVPlacement(rotPipe,G4ThreeVector(xpipe,0.,zpipe),logicalStraitPipe,"BeamLineStraitPipe",fMotherVolume,false,0,true);
  new G4PVPlacement(rotPipe,G4ThreeVector((xpipe+PipeSizeZ/2*cos(45*deg)+FlThick/2*cos(45*deg))*mm,0.,(zpipe-PipeSizeZ/2*sin(45*deg)-FlThick/2*sin(45*deg))*mm),logicalFlange,"BeamLineFlange",fMotherVolume,false,0,true);
  std::cout<<"******************************************************************************** Flange X "<<xpipe+PipeSizeZ/2*cos(45*deg)+FlThick/2*cos(45*deg)<<std::endl;
  std::cout<<"******************************************************************************** Flange Z "<<zpipe-PipeSizeZ/2*sin(45*deg)-FlThick/2*sin(45*deg)<<std::endl;
  UpStreamFlangePosX = xpipe+PipeSizeZ/2*cos(45*deg)+FlThick/2*cos(45*deg);
  UpStreamFlangePosZ = zpipe-PipeSizeZ/2*sin(45*deg)-FlThick/2*sin(45*deg);
  
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
