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
    CreateBeThinWindow();

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

  // Get position of entrance point to the magnet pipe section
  G4double mpEntPosX = geo->GetMagPipeEnterPosX();
  G4double mpEntPosY = geo->GetMagPipeEnterPosY();
  G4double mpEntPosZ = geo->GetMagPipeEnterPosZ();

  // Get rotation angle at magnet entrance
  G4double magAngle = geo->GetDHSTB002AngularSpan();

  // Create junction pipe between magnet pipe and Be flange
  G4double beJunLen = geo->GetBeJunctionLength();
  G4double beJunRIn = geo->GetBeJunctionRIn();
  G4double beJunROut = geo->GetBeJunctionROut();
  G4double beJunFlgFntR = geo->GetBeJunctionFlangeFrontRadius();
  G4double beJunFlgFntT = geo->GetBeJunctionFlangeFrontThick();
  G4double beJunFlgBckR = geo->GetBeJunctionFlangeBackRadius();
  G4double beJunFlgBckT = geo->GetBeJunctionFlangeBackThick();

  // Create solid pipe
  G4Tubs* solidBeJunFull1 = new G4Tubs("solidBeJunFull1",0.,beJunROut,0.5*beJunLen,0.*deg,360.*deg);

  // Add flange on front (upstream) side
  G4Tubs* solidBeJunFlgFnt = new G4Tubs("solidBeJunFlgFnt",0.,beJunFlgFntR,0.5*beJunFlgFntT,0.*deg,360.*deg);
  G4UnionSolid* solidBeJunFull2 = new G4UnionSolid("solidBeJunFull2",solidBeJunFull1,solidBeJunFlgFnt,0,G4ThreeVector(0.,0.,-0.5*beJunLen+0.5*beJunFlgFntT));

  // Add flange on back (downstream) side
  G4Tubs* solidBeJunFlgBck = new G4Tubs("solidBeJunFlgBck",0.,beJunFlgBckR,0.5*beJunFlgBckT,0.*deg,360.*deg);
  G4UnionSolid* solidBeJunFull = new G4UnionSolid("solidBeJunFull",solidBeJunFull2,solidBeJunFlgBck,0,G4ThreeVector(0.,0.,0.5*beJunLen-0.5*beJunFlgBckT));

  // Carve hole inside junction (add usual small tolerance)
  G4Tubs* solidBeJunHole = new G4Tubs("solidBeJunHole",0.,beJunRIn,0.5*beJunLen+10.*um,0.*deg,360.*deg);
  G4SubtractionSolid* solidBeJunction = new G4SubtractionSolid("solidBeJunction",solidBeJunFull,solidBeJunHole,0,G4ThreeVector(0.,0.,0.));

  // Create junction logical volume
  G4LogicalVolume* logicalBeJunction = new G4LogicalVolume(solidBeJunction,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalBeJunction",0,0,0);
  logicalBeJunction->SetVisAttributes(steelVisAttr);

  // Position junction close to the magnet entrance
  G4double beJunMgPosX = mpEntPosX+0.5*beJunLen*sin(magAngle);
  G4double beJunMgPosY = mpEntPosY;
  G4double beJunMgPosZ = mpEntPosZ-0.5*beJunLen*cos(magAngle);
  G4ThreeVector beJunMgPos = G4ThreeVector(beJunMgPosX,beJunMgPosY,beJunMgPosZ);
  G4RotationMatrix* beJunMgRot = new G4RotationMatrix;
  beJunMgRot->rotateY(magAngle);
  new G4PVPlacement(beJunMgRot,beJunMgPos,logicalBeJunction,"BeamLineMagnetJunction",fMotherVolume,false,0,true);

  // Create long pipe between magnet pipe and Be flange
  G4double bePipeLen = geo->GetBePipeLength();
  G4double bePipeRIn = geo->GetBePipeRIn();
  G4double bePipeROut = geo->GetBePipeROut();
  G4double bePipeFlgFntR = geo->GetBePipeFlangeFrontRadius();
  G4double bePipeFlgFntT = geo->GetBePipeFlangeFrontThick();
  G4double bePipeFlgBckR = geo->GetBePipeFlangeBackRadius();
  G4double bePipeFlgBckT = geo->GetBePipeFlangeBackThick();

  // Create solid pipe
  G4Tubs* solidBePipeFull1 = new G4Tubs("solidBePipeFull1",0.,bePipeROut,0.5*bePipeLen,0.*deg,360.*deg);

  // Add flange on front (upstream) side
  //G4Tubs* solidBePipeFlgFnt = new G4Tubs("solidBePipeFlgFnt",0.,bePipeFlgFntR,0.5*bePipeFlgFntT,0.*deg,360.*deg);
  //G4UnionSolid* solidBePipeFull2 = new G4UnionSolid("solidBePipeFull2",solidBePipeFull1,solidBePipeFlgFnt,0,G4ThreeVector(0.,0.,-0.5*bePipeLen+0.5*bePipeFlgFntT));

  // Add flange on back (downstream) side
  //G4Tubs* solidBePipeFlgBck = new G4Tubs("solidBePipeFlgBck",0.,bePipeFlgBckR,0.5*bePipeFlgBckT,0.*deg,360.*deg);
  //G4UnionSolid* solidBePipeFull = new G4UnionSolid("solidBePipeFull",solidBePipeFull2,solidBePipeFlgBck,0,G4ThreeVector(0.,0.,0.5*bePipeLen-0.5*bePipeFlgBckT));

  // Carve hole inside pipe (add usual small tolerance)
  G4Tubs* solidBePipeHole = new G4Tubs("solidBePipeHole",0.,bePipeRIn,0.5*bePipeLen+10.*um,0.*deg,360.*deg);
  //G4SubtractionSolid* solidBePipe = new G4SubtractionSolid("solidBePipe",solidBePipeFull,solidBePipeHole,0,G4ThreeVector(0.,0.,0.));
  G4SubtractionSolid* solidBePipe = new G4SubtractionSolid("solidBePipe",solidBePipeFull1,solidBePipeHole,0,G4ThreeVector(0.,0.,0.));

  // Create junction logical volume
  G4LogicalVolume* logicalBePipe = new G4LogicalVolume(solidBePipe,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalBePipe",0,0,0);
  logicalBePipe->SetVisAttributes(steelVisAttr);

  // Position long pipe after the magnet-side junction
  G4double bePipePosX = mpEntPosX+(beJunLen+0.5*bePipeLen)*sin(magAngle);
  G4double bePipePosY = mpEntPosY;
  G4double bePipePosZ = mpEntPosZ-(beJunLen+0.5*bePipeLen)*cos(magAngle);
  G4ThreeVector bePipePos = G4ThreeVector(bePipePosX,bePipePosY,bePipePosZ);
  G4RotationMatrix* bePipeRot = new G4RotationMatrix;
  bePipeRot->rotateY(magAngle);
  new G4PVPlacement(bePipeRot,bePipePos,logicalBePipe,"BeamLineBePipe",fMotherVolume,false,0,true);

  // For some reason, boolean solid visualization fails to show solid created with fullpipe+flangeF+flangeB-hole
  // Create here the flanges as independent volumes

  G4Tubs* solidBePipeFlgFnt = new G4Tubs("solidBePipeFlgFnt",bePipeROut,bePipeFlgFntR,0.5*bePipeFlgFntT,0.*deg,360.*deg);
  G4LogicalVolume* logicalBePipeFlgFnt = new G4LogicalVolume(solidBePipeFlgFnt,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalBePipeFlangeFront",0,0,0);
  logicalBePipeFlgFnt->SetVisAttributes(steelVisAttr);
  G4double bePipeFFPosX = mpEntPosX+(beJunLen+bePipeLen-0.5*bePipeFlgFntT)*sin(magAngle);
  G4double bePipeFFPosY = mpEntPosY;
  G4double bePipeFFPosZ = mpEntPosZ-(beJunLen+bePipeLen-0.5*bePipeFlgFntT)*cos(magAngle);
  G4ThreeVector bePipeFFPos = G4ThreeVector(bePipeFFPosX,bePipeFFPosY,bePipeFFPosZ);
  new G4PVPlacement(bePipeRot,bePipeFFPos,logicalBePipeFlgFnt,"BeamLineBePipeFlangeFront",fMotherVolume,false,0,true);

  G4Tubs* solidBePipeFlgBck = new G4Tubs("solidBePipeFlgBck",bePipeROut,bePipeFlgBckR,0.5*bePipeFlgBckT,0.*deg,360.*deg);
  G4LogicalVolume* logicalBePipeFlgBck = new G4LogicalVolume(solidBePipeFlgBck,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalBePipeFlangeBack",0,0,0);
  logicalBePipeFlgBck->SetVisAttributes(steelVisAttr);
  G4double bePipeFBPosX = mpEntPosX+(beJunLen+0.5*bePipeFlgBckT)*sin(magAngle);
  G4double bePipeFBPosY = mpEntPosY;
  G4double bePipeFBPosZ = mpEntPosZ-(beJunLen+0.5*bePipeFlgBckT)*cos(magAngle);
  G4ThreeVector bePipeFBPos = G4ThreeVector(bePipeFBPosX,bePipeFBPosY,bePipeFBPosZ);
  new G4PVPlacement(bePipeRot,bePipeFBPos,logicalBePipeFlgBck,"BeamLineBePipeFlangeBack",fMotherVolume,false,0,true);

  // Position junction close to Be window
  // Use same logical volume created for Magnet-side junction
  G4double beJunBePosX = mpEntPosX+(beJunLen+bePipeLen+0.5*beJunLen)*sin(magAngle);
  G4double beJunBePosY = mpEntPosY;
  G4double beJunBePosZ = mpEntPosZ-(beJunLen+bePipeLen+0.5*beJunLen)*cos(magAngle);
  G4ThreeVector beJunBePos = G4ThreeVector(beJunBePosX,beJunBePosY,beJunBePosZ);
  G4RotationMatrix* beJunBeRot = new G4RotationMatrix;
  beJunBeRot->rotateY(magAngle);
  new G4PVPlacement(beJunBeRot,beJunBePos,logicalBeJunction,"BeamLineBeWinJunction",fMotherVolume,false,0,true);

  // Create Be thin window and its support flange

  G4double beWinR = geo->GetBeWindowRadius();
  G4double beWinT = geo->GetBeWindowThick();
  G4Tubs* solidBeWin = new G4Tubs("solidBeWin",0.,beWinR,0.5*beWinT,0.*deg,360.*deg);
  G4LogicalVolume* logicalBeWin = new G4LogicalVolume(solidBeWin,G4Material::GetMaterial("G4_Be"),"logicalBeWin",0,0,0);
  logicalBeWin->SetVisAttributes(BeVisAttr);

  G4double beWinFlgR = geo->GetBeWindowFlangeRadius();
  G4double beWinFlgT = geo->GetBeWindowFlangeThick();
  G4Tubs* solidBeWinFlgFull = new G4Tubs("solidBeWinFlgFull",0.,beWinFlgR,0.5*beWinFlgT,0.*deg,360.*deg);
  G4Tubs* solidBeWinFlgHole = new G4Tubs("solidBeWinFlgHole",0.,beWinR+1.*um,0.5*beWinFlgT+10*um,0.*deg,360.*deg);
  G4SubtractionSolid* solidBeWinFlg = new G4SubtractionSolid("solidBeWinFlg",solidBeWinFlgFull,solidBeWinFlgHole,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicalBeWinFlg = new G4LogicalVolume(solidBeWinFlg,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalBeWinFlg",0,0,0);
  logicalBeWinFlg->SetVisAttributes(steelVisAttr);

  G4double beWinFlgPosX = mpEntPosX+(beJunLen+bePipeLen+beJunLen+0.5*beWinFlgT)*sin(magAngle);
  G4double beWinFlgPosY = mpEntPosY;
  G4double beWinFlgPosZ = mpEntPosZ-(beJunLen+bePipeLen+beJunLen+0.5*beWinFlgT)*cos(magAngle);
  G4ThreeVector beWinFlgPos = G4ThreeVector(beWinFlgPosX,beWinFlgPosY,beWinFlgPosZ);
  G4RotationMatrix* beWinFlgRot = new G4RotationMatrix;
  beWinFlgRot->rotateY(magAngle);
  // Position flange
  new G4PVPlacement(beWinFlgRot,beWinFlgPos,logicalBeWinFlg,"BeamLineBeWindowFlange",fMotherVolume,false,0,true);

  // Position Be window if enabled
  if ( geo->BeWindowIsEnabled() ) {
    // N.B. upstream face of Be thin window is at center of support flange
    G4double beWinPosX = mpEntPosX+(beJunLen+bePipeLen+beJunLen+0.5*beWinFlgT-0.5*beWinT)*sin(magAngle);
    G4double beWinPosY = mpEntPosY;
    G4double beWinPosZ = mpEntPosZ-(beJunLen+bePipeLen+beJunLen+0.5*beWinFlgT-0.5*beWinT)*cos(magAngle);
    G4ThreeVector beWinPos = G4ThreeVector(beWinPosX,beWinPosY,beWinPosZ);
    G4RotationMatrix* beWinRot = new G4RotationMatrix;
    beWinRot->rotateY(magAngle);
    new G4PVPlacement(beWinRot,beWinPos,logicalBeWin,"BeamLineBeWindow",fMotherVolume,false,0,true);
  }

  printf("BeamLine - Be window entry face center is at (%.2f,%.2f,%.2f) mm\n",beWinFlgPosX,beWinFlgPosY,beWinFlgPosZ);

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

}
