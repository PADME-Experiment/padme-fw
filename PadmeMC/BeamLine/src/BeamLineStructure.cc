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

#include "G4SDManager.hh" // 29/04/2019 M. Raggi
#include "BeWSD.hh"       // 29/04/2019 M. Raggi
#include "MylarWSD.hh"    // 18/06/2020 M. Raggi
#include "BeamFlagSD.hh"       // 29/08/2019 M. Raggi

#include "QuadrupoleMagField.hh"  // M. Raggi 8/04/2019
#include "QuadSetup.hh" // M. Raggi 10/04/2019

//#include "G4MagneticField.hh"

using namespace CLHEP;

BeamLineStructure::BeamLineStructure(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{

  fBeamLineMessenger = new BeamLineMessenger(this);
  fBeamLineIsVisible = 1; // If =0 all chamber structures are invisible (debug only)
 }

BeamLineStructure::~BeamLineStructure()
{
}

void BeamLineStructure::CreateGeometry()
{
  BeamLineGeometry* geo = BeamLineGeometry::GetInstance();
  // Create the thin window membrane in front of ECal with its flange
  G4double fLineSetup = geo->GetBeamLineSetup();

  if(fLineSetup==0.){
    G4cout<<"######### Create 2019 Beam Line ###############"<<fLineSetup<<G4endl;
    CreateBeThinWindow();   // Create BeWindow
    CreateDHSTB002Magnet(); // Create DHSTB002 magnet
    //CreateQuadMagnets();  // Create Quadrupole magnets
    CreateBeamLine();       // Create Beam line 2019
  }
  if(fLineSetup==1.){
    G4cout<<"######### Create 2020 Beam Line ###############"<<fLineSetup<<G4endl;
    CreateMylarThinWindow();
    CreateDHSTB002Magnet();
    //CreateQuadMagnets();
    CreateBeamLine2020(); //beam line 2020
  }
}

//void BeamLineStructure::CreateQuadMagnets()
//{
//}

void BeamLineStructure::CreateMylarThinWindow()
{
  ///////////////////////////////////////////////////////
  // Thin Mylar 140um window mounted before DHSTB002
  // and pipes in the upstream region
  ///////////////////////////////////////////////////////
  
  BeamLineGeometry* geo = BeamLineGeometry::GetInstance();
  
  G4VisAttributes steelVisAttr = G4VisAttributes(G4Colour::Grey());
  G4VisAttributes MylarVisAttr = G4VisAttributes(G4Colour::Red());
  if(!fBeamLineIsVisible){
    MylarVisAttr = G4VisAttributes::Invisible;
    steelVisAttr = G4VisAttributes::Invisible;
  }
  
  // Create Mylar thin window and its support flange

  G4double mylarWinR = geo->GetMylarWindowRadius();
  G4double mylarWinT = geo->GetMylarWindowThick();
  G4double mylarWinFlgR = geo->GetMylarWindowFlangeRadius();
  G4double mylarWinFlgT = geo->GetMylarWindowFlangeThick();

  // Top volume containing Mylar window and its flange
  G4Tubs* solidMylarWinVolume = new G4Tubs("solidMylarWinVolume",0.,mylarWinFlgR,0.5*mylarWinFlgT,0.*deg,360.*deg);
  fMylarWindowVolume = new G4LogicalVolume(solidMylarWinVolume,G4Material::GetMaterial("Vacuum"),"logicalMylarWinVolume",0,0,0);
  fMylarWindowVolume->SetVisAttributes(G4VisAttributes::Invisible);

  // Mylar thin window
  G4Tubs* solidMylarWin = new G4Tubs("solidMylarWin",0.,mylarWinR,0.5*mylarWinT,0.*deg,360.*deg);
  G4LogicalVolume* logicalMylarWin = new G4LogicalVolume(solidMylarWin,G4Material::GetMaterial("G4_MYLAR"),"logicalMylarWin",0,0,0);
  logicalMylarWin->SetVisAttributes(MylarVisAttr);

  // Support flange for Mylar thin window
  G4Tubs* solidMylarWinFlange = new G4Tubs("solidMylarWinFlange",mylarWinR,mylarWinFlgR,0.5*mylarWinFlgT,0.*deg,360.*deg);
  G4LogicalVolume* logicalMylarWinFlange = new G4LogicalVolume(solidMylarWinFlange,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalMylarWinFlange",0,0,0);
  logicalMylarWinFlange->SetVisAttributes(steelVisAttr);

  // Position Mylar window and its flange inside top volume
  // Shift Mylar window so that its entry face is at center of support flange
  new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicalMylarWinFlange,"BeamLineMylarWinFlange",fMylarWindowVolume,false,0,true);

  if( geo->MylarWindowIsEnabled() ) {
    new G4PVPlacement(0,G4ThreeVector(0.,0.,0.5*mylarWinT),logicalMylarWin,"BeamLineMylarWindow",fMylarWindowVolume,false,0,true);
    
    // The Mylar window is a sensitive detector
    G4String MylarWSDName = geo->GetMylarWSensitiveDetectorName();
    printf("Registering MylarW SD %s\n",MylarWSDName.data());
    MylarWSD* mylarWSD = new MylarWSD(MylarWSDName);
    fMylarWindowVolume->SetSensitiveDetector(mylarWSD);
    G4SDManager::GetSDMpointer()->AddNewDetector(mylarWSD);
  }
}

void BeamLineStructure::CreateBeamLine2020()
{
  
}

void BeamLineStructure::CreateBeThinWindow()
{
  ///////////////////////////////////////////////////////
  // Thin Be 250um window mounted before DHSTB002
  // and pipes in the upstream region
  ///////////////////////////////////////////////////////

  BeamLineGeometry* geo = BeamLineGeometry::GetInstance();

  G4VisAttributes steelVisAttr = G4VisAttributes(G4Colour::Grey());
  G4VisAttributes BeVisAttr = G4VisAttributes(G4Colour::Blue());
  if ( ! fBeamLineIsVisible ) {
    BeVisAttr = G4VisAttributes::Invisible;
    steelVisAttr = G4VisAttributes::Invisible;
  }

  // Create Be thin window and its support flange

  G4double beWinR = geo->GetBeWindowRadius();
  G4double beWinT = geo->GetBeWindowThick();
  G4double beWinFlgR = geo->GetBeWindowFlangeRadius();
  G4double beWinFlgT = geo->GetBeWindowFlangeThick();

  // Top volume containing Be window and its flange
  G4Tubs* solidBeWinVolume = new G4Tubs("solidBeWinVolume",0.,beWinFlgR,0.5*beWinFlgT,0.*deg,360.*deg);
  fBeWindowVolume = new G4LogicalVolume(solidBeWinVolume,G4Material::GetMaterial("Vacuum"),"logicalBeWinVolume",0,0,0);
  fBeWindowVolume->SetVisAttributes(G4VisAttributes::Invisible);

  // Be thin window
  G4Tubs* solidBeWin = new G4Tubs("solidBeWin",0.,beWinR,0.5*beWinT,0.*deg,360.*deg);
  G4LogicalVolume* logicalBeWin = new G4LogicalVolume(solidBeWin,G4Material::GetMaterial("G4_Be"),"logicalBeWin",0,0,0);
  
//  G4Tubs* solidBeWin = new G4Tubs("solidBeWin",0.,beWinR,0.5*40*um,0.*deg,360.*deg);
//  G4LogicalVolume* logicalBeWin = new G4LogicalVolume(solidBeWin,G4Material::GetMaterial("G4_MYLAR"),"logicalBeWin",0,0,0);
  logicalBeWin->SetVisAttributes(BeVisAttr);

  // Support flange for Be thin window
  G4Tubs* solidBeWinFlange = new G4Tubs("solidBeWinFlange",beWinR,beWinFlgR,0.5*beWinFlgT,0.*deg,360.*deg);
  G4LogicalVolume* logicalBeWinFlange = new G4LogicalVolume(solidBeWinFlange,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalBeWinFlange",0,0,0);
  logicalBeWinFlange->SetVisAttributes(steelVisAttr);

  // Position Be window and its flange inside top volume
  // Shift Be window so that its entry face is at center of support flange
  new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicalBeWinFlange,"BeamLineBeWinFlange",fBeWindowVolume,false,0,true);


  if ( geo->BeWindowIsEnabled() ) {
    new G4PVPlacement(0,G4ThreeVector(0.,0.,0.5*beWinT),logicalBeWin,"BeamLineBeWindow",fBeWindowVolume,false,0,true);    
    // The Be window is a sensitive detector
    G4String BeWSDName = geo->GetBeWSensitiveDetectorName();
    printf("Registering BeW SD %s\n",BeWSDName.data());
    BeWSD* beWSD = new BeWSD(BeWSDName);
    fBeWindowVolume->SetSensitiveDetector(beWSD);
    G4SDManager::GetSDMpointer()->AddNewDetector(beWSD);
  }

}

void BeamLineStructure::CreateDHSTB002Magnet()
{
  BeamFlagSD* beamFlagSD;
  BeamLineGeometry* geo = BeamLineGeometry::GetInstance();

  G4VisAttributes steelVisAttr   = G4VisAttributes(G4Color::Grey()); // Dark gray
  G4VisAttributes FlagVisAttr   = G4VisAttributes(G4Color::Yellow()); // Beam Flags
  G4VisAttributes DHSTB002VisAtt = G4VisAttributes(G4Colour::Red());
  if ( ! fBeamLineIsVisible ) {
    steelVisAttr   = G4VisAttributes::Invisible;
    DHSTB002VisAtt = G4VisAttributes::Invisible;
  }

  // Get position of entrance point to the magnet pipe section
  G4double mpEntPosX = geo->GetMagPipeEnterPosX();
  G4double mpEntPosY = geo->GetMagPipeEnterPosY();
  G4double mpEntPosZ = geo->GetMagPipeEnterPosZ();

  G4double strPipeSizeX = geo->GetMagPipeSizeX();
  G4double strPipeSizeY = geo->GetMagPipeSizeY();
  G4double strPipeSizeZ = geo->GetMagPipeStraightLength();


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
  G4SubtractionSolid* solidDHSTB002_3 = new G4SubtractionSolid("solidDHSTB002_3",solidDHSTB002_2,solidHole3,0,G4ThreeVector(0.,0.,0.));

  // Carve rectangular hole into yoke for straight section of the beam pipe
  G4double magBPSizeX = geo->GetMagPipeMaxRadius()-geo->GetMagPipeMinRadius()+1.*mm;
  //G4double magBPSizeY = geo->GetDHSTB002CenterRadius()*sin(geo->GetDHSTB002AngularSpan());
  G4double magBPSizeY = geo->GetMagPipeLineLength();
  G4double magBPSizeZ = geo->GetMagPipeSizeY()+1.*mm;
  G4Box* solidPipeHole = new G4Box("solidMagPipeHole",0.5*magBPSizeX,0.5*magBPSizeY,0.5*magBPSizeZ);
  G4ThreeVector magBPPos = G4ThreeVector(geo->GetDHSTB002CenterRadius(),0.5*magBPSizeY,0.);
  G4SubtractionSolid* solidDHSTB002 = new G4SubtractionSolid("solidDHSTB002",solidDHSTB002_3,solidPipeHole,0,magBPPos);
  //G4cout << "BeamPipe Straight hole " << magBPSizeX << " " << magBPSizeY << " " << magBPSizeZ << " " << magBPPos << G4endl;

  // Create DHSTB002 magnet logical volume
  G4LogicalVolume* logicalDHSTB002 = new G4LogicalVolume(solidDHSTB002,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalDHSTB002",0,0,0);
  logicalDHSTB002->SetVisAttributes(DHSTB002VisAtt);

  // Now rotate DHSTB002 to the XZ plane and position it so that its exit is aligned with the PADME Z axis
  G4RotationMatrix* rotDHSTB = new G4RotationMatrix;
  //rotDHSTB->rotateX(-90.*deg);
  //rotDHSTB->rotateZ(180.*deg);
  rotDHSTB->rotateX(90.*deg);
  rotDHSTB->rotateZ(180.*deg+geo->GetDHSTB002AngularSpan());
  G4ThreeVector posDHSTB = G4ThreeVector(holeCenterR,0.,geo->GetDHSTB002ExitPosZ());
  new G4PVPlacement(rotDHSTB,posDHSTB,logicalDHSTB002,"DHSTB002",fMotherVolume,false,0,true);

  // Angular span of the DHSTB002 magnet (45 deg)
  G4double magnetAngle = geo->GetDHSTB002AngularSpan();

  // Create the magnetic volume
  G4double magvolSizeY = geo->GetMagVolSizeY();
  G4double magvolMinR = geo->GetMagVolMinRadius();
  G4double magvolMaxR = geo->GetMagVolMaxRadius();
  G4Tubs* solidMagVol = new G4Tubs("solidMagVol",magvolMinR,magvolMaxR,0.5*magvolSizeY,0.*deg,magnetAngle);
  G4LogicalVolume* logicalMagVol = new G4LogicalVolume(solidMagVol,G4Material::GetMaterial("Vacuum"),"logicalMagVol",0,0,0);
  //  logicalMagVol->SetVisAttributes(G4VisAttributes::Invisible);

  // Create another volume externally adjacent to magnetic volume: used for cuts
  G4double extvolSizeY = geo->GetMagVolSizeY();
  G4double extvolMinR = geo->GetMagVolMaxRadius();
  G4double extvolMaxR = geo->GetMagVolMaxRadius()+1.*m; // Arbitrary as long as it is large enough
  G4Tubs* solidExtVol = new G4Tubs("solidExtVol",extvolMinR,extvolMaxR,0.5*extvolSizeY,0.*deg,magnetAngle);

  // Add a constant magnetic field to the magnetic volume
  G4ThreeVector fieldV(0.,geo->GetDHSTB002MagneticFieldY(),0.);
  G4MagneticField* magField = new G4UniformMagField(fieldV);
  G4FieldManager* localFieldManager = new G4FieldManager(magField);
  logicalMagVol->SetFieldManager(localFieldManager,true);

  // Position the magnetic volume at center of H-shaped hole of DHSTB002
  G4RotationMatrix* rotMagVol = new G4RotationMatrix;
  //rotMagVol->rotateX(-90.*deg);
  //rotMagVol->rotateZ(180.*deg);
  rotMagVol->rotateX(90.*deg);
  rotMagVol->rotateZ(180.*deg+magnetAngle);
  G4ThreeVector posMagVol = G4ThreeVector(geo->GetDHSTB002CenterRadius(),0.,geo->GetDHSTB002ExitPosZ());
  new G4PVPlacement(rotMagVol,posMagVol,logicalMagVol,"DHSTB002MagneticVolume",fMotherVolume,false,0,true);

  G4double magBPBSizeY = geo->GetMagPipeSizeY();
  G4double magBPBMinR = geo->GetMagPipeMinRadius();
  G4double magBPBMaxR = geo->GetMagPipeMaxRadius();
  G4Tubs* solidBeamPipeBendFull = new G4Tubs("solidBeamPipeBendFull",magBPBMinR,magBPBMaxR,0.5*magBPBSizeY,0.*deg,magnetAngle);
  
// Linear pipe - Full
  G4double magBPLSizeX = geo->GetMagPipeMaxRadius()-geo->GetMagPipeMinRadius();
  G4double magBPLSizeY = geo->GetMagPipeLineLength();
  G4double magBPLSizeZ = geo->GetMagPipeSizeY();
  G4Box* solidBeamPipeLineFull = new G4Box("solidBeamPipeLineFull",0.5*magBPLSizeX,0.5*magBPLSizeY,0.5*magBPLSizeZ);
 
// Complete beam pipe - Full
  G4ThreeVector magBPLPos = G4ThreeVector(geo->GetDHSTB002CenterRadius(),0.5*magBPLSizeY,0.);
  G4UnionSolid* solidBeamPipeFull = new G4UnionSolid("solidBeamPipeFull",solidBeamPipeBendFull,solidBeamPipeLineFull,0,magBPLPos);
  
// Bended pipe - Hole
  G4double magBPBHSizeY = geo->GetMagPipeHoleSizeY();
  G4double magBPBHMinR = geo->GetMagPipeHoleMinRadius();
  G4double magBPBHMaxR = geo->GetMagPipeHoleMaxRadius();
  G4Tubs* solidBeamPipeBendHole  = new G4Tubs("solidBeamPipeBendHole",magBPBHMinR,magBPBHMaxR,0.5*magBPBHSizeY,0.*deg,magnetAngle);
  
// Linear pipe - Hole
  G4double magBPLHSizeX = geo->GetMagPipeHoleMaxRadius()-geo->GetMagPipeHoleMinRadius();
  G4double magBPLHSizeY = geo->GetMagPipeLineLength();
  G4double magBPLHSizeZ = geo->GetMagPipeHoleSizeY();
  G4Box* solidBeamPipeLineHole = new G4Box("solidBeamPipeLineHole",0.5*magBPLHSizeX,0.5*magBPLHSizeY,0.5*magBPLHSizeZ);
  
// Complete beam pipe - Hole
  G4UnionSolid* solidBeamPipeHole = new G4UnionSolid("solidBeamPipeHole",solidBeamPipeBendHole,solidBeamPipeLineHole,0,magBPLPos);
  
// Complete beam pipe (remove hole from full pipe)
  G4SubtractionSolid* solidBeamPipe = new G4SubtractionSolid("solidBeamPipe",solidBeamPipeFull,solidBeamPipeHole,0,G4ThreeVector(0.,0.,0.));
  
// Now split the complete beam pipe in a section inside the magnetic volume...
  G4SubtractionSolid* solidBeamPipeIn = new G4SubtractionSolid("solidBeamPipeIn",solidBeamPipe,solidExtVol,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicalBeamPipeIn = new G4LogicalVolume(solidBeamPipeIn,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalBeamPipeIn",0,0,0);
  logicalBeamPipeIn->SetVisAttributes(steelVisAttr);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicalBeamPipeIn,"DHSTB002BeamPipeIn",logicalMagVol,false,0,true);
  
// ...and a section outside it
  G4SubtractionSolid* solidBeamPipeOut = new G4SubtractionSolid("solidBeamPipeOut",solidBeamPipe,solidMagVol,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicalBeamPipeOut = new G4LogicalVolume(solidBeamPipeOut,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalBeamPipeOut",0,0,0);
  logicalBeamPipeOut->SetVisAttributes(steelVisAttr);
  new G4PVPlacement(rotMagVol,posMagVol,logicalBeamPipeOut,"DHSTB002BeamPipeOut",fMotherVolume,false,0,true);
  
// Create the straight section of the beam pipe (with its flange)
// and position it at entrance and exits of magnet section of the beam line
  
//  G4double strPipeSizeX = geo->GetMagPipeSizeX();
//  G4double strPipeSizeY = geo->GetMagPipeSizeY();
//  G4double strPipeSizeZ = geo->GetMagPipeStraightLength();
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
  new G4PVPlacement(strFrontRot,strFrontPos,logicalStraightPipe,"DHSTB002FlangeFront",fMotherVolume,false,0,true);

  G4double beWinFlgT = geo->GetBeWindowFlangeThick(); //non mi piace molto
  G4double beWin2FlgPosX = mpEntPosX-(strPipeSizeZ+magBPLSizeY+strPipeSizeZ+0.5*beWinFlgT)*sin(magnetAngle);
  G4double beWin2FlgPosY = mpEntPosY;
  G4double beWin2FlgPosZ = mpEntPosZ+(strPipeSizeZ+magBPLSizeY+strPipeSizeZ+0.5*beWinFlgT)*cos(magnetAngle);
  G4ThreeVector beWin2FlgPos = G4ThreeVector(beWin2FlgPosX,beWin2FlgPosY,beWin2FlgPosZ);

  G4double strSideRotY = geo->GetMagPipeStraightSideRotY();
  G4RotationMatrix* strSideRot = new G4RotationMatrix;
  strSideRot->rotateY(strSideRotY);

  ////////////////////////////////////////////////////////////////////////
  // Beam Flag to monitor beam in different locations M. Raggi 29/08/2019
  // Flag 2 after the DHSTB002 output pipe
  ///////////////////////////////////////////////////////////////////////

  G4double FlagR = geo->GetBeWindowRadius()-15*um; //Use the same of the BeW
  G4double FlagT = geo->GetBeWindowThick();  //Use the same of the BeW
  
  G4double     FlagFrontPosX = geo->GetMagPipeStraightFrontPosX();
  G4double     FlagFrontPosZ = geo->GetMagPipeStraightFrontPosZ()+strPipeSizeZ/2;
  G4ThreeVector FlagFrontPos = G4ThreeVector(FlagFrontPosX,0.,FlagFrontPosZ);
  
  G4Tubs* solidBeamFlag2 = new G4Tubs("solidBeamFlag2",0.,FlagR,0.5*FlagT,0.*deg,360.*deg);
  G4LogicalVolume* logicalBeamFlag2 = new G4LogicalVolume(solidBeamFlag2,G4Material::GetMaterial("Vacuum"),"logicalBeamFlag2",0,0,0);
  logicalBeamFlag2->SetVisAttributes(FlagVisAttr);
  
  if ( geo->BeamFlagIsEnabled() ) {
    //   printf("Registering Flag2 %b\n",geo->BeamFlagIsEnabled());
    new G4PVPlacement(strFrontRot,FlagFrontPos,logicalBeamFlag2,"BeamLineBeamFlag2",fMotherVolume,false,0,true);    
    G4String BeamFlag2SDName = geo->GetBeamFlag2SensitiveDetectorName();
    printf("Registering Flag2 SD %s\n",BeamFlag2SDName.data());
    beamFlagSD = new BeamFlagSD(BeamFlag2SDName);
    logicalBeamFlag2->SetSensitiveDetector(beamFlagSD);
    G4SDManager::GetSDMpointer()->AddNewDetector(beamFlagSD);
  }
  
  ////////////////////////////////////////////////////////////////////////
  // Beam Flag to monitor beam in different locations M. Raggi 29/08/2019
  // Flag 3 exit of DHSTB002
  ///////////////////////////////////////////////////////////////////////
  
  G4double Flag3R = 44.5*mm; // 99mm diameter
  G4double Flag3T = 50*um;  //Use the same of the BeW
  
  G4double      Flag3FrontPosX = 0.;
  G4double      Flag3FrontPosZ = -1039*mm;
  G4ThreeVector Flag3FrontPos = G4ThreeVector(Flag3FrontPosX,0.,Flag3FrontPosZ);
  
  G4Tubs* solidBeamFlag3 = new G4Tubs("solidBeamFlag3",0.,Flag3R,0.5*Flag3T,0.*deg,360.*deg);
  G4LogicalVolume* logicalBeamFlag3 = new G4LogicalVolume(solidBeamFlag3,G4Material::GetMaterial("Vacuum"),"logicalBeamFlag3",0,0,0);
  logicalBeamFlag3->SetVisAttributes(FlagVisAttr);
  if ( geo->BeamFlagIsEnabled() ) {
    //   printf("Registering Flag3 %b\n",geo->BeamFlagIsEnabled());
    new G4PVPlacement(strFrontRot,Flag3FrontPos,logicalBeamFlag3,"BeamLineBeamFlag3",fMotherVolume,false,0,true);    
    logicalBeamFlag3->SetSensitiveDetector(beamFlagSD);
  }
  ////////////////////////////////////////////////////////////////////////
  // Beam Flag to monitor beam in different locations M. Raggi 06/09/2019
  // Flag 4 on the strait DHSTB002 exit section
  ///////////////////////////////////////////////////////////////////////
  
  G4double      Flag4FrontPosX = mpEntPosX-(strPipeSizeZ+magBPLSizeY+strPipeSizeZ+0.5*beWinFlgT +5*cm)*sin(magnetAngle);; 
  G4double      Flag4FrontPosZ = mpEntPosZ+(strPipeSizeZ+magBPLSizeY+strPipeSizeZ+0.5*beWinFlgT +5*cm)*cos(magnetAngle);
  G4ThreeVector Flag4FrontPos  = G4ThreeVector(Flag4FrontPosX,beWin2FlgPosY,Flag4FrontPosZ);
  
  G4Tubs* solidBeamFlag4 = new G4Tubs("solidBeamFlag4",0.,30.*mm,0.1*mm,0.*deg,360.*deg);
  G4LogicalVolume* logicalBeamFlag4 = new G4LogicalVolume(solidBeamFlag4,G4Material::GetMaterial("Vacuum"),"logicalBeamFlag4",0,0,0);
  logicalBeamFlag4->SetVisAttributes(FlagVisAttr);
  
  if ( geo->BeamFlagIsEnabled() ) {
    new G4PVPlacement(strSideRot,Flag4FrontPos,logicalBeamFlag4,"BeamLineBeamFlag4",fMotherVolume,false,0,true);    
    logicalBeamFlag4->SetSensitiveDetector(beamFlagSD);
  }

  // Position back straight section
  G4double strBackPosX = geo->GetMagPipeStraightBackPosX();
  G4double strBackPosZ = geo->GetMagPipeStraightBackPosZ();
  G4double strBackRotY = geo->GetMagPipeStraightBackRotY();
  G4RotationMatrix* strBackRot = new G4RotationMatrix;
  strBackRot->rotateY(strBackRotY);
  G4ThreeVector strBackPos = G4ThreeVector(strBackPosX,0.,strBackPosZ);
  new G4PVPlacement(strBackRot,strBackPos,logicalStraightPipe,"DHSTB002FlangeBack",fMotherVolume,false,0,true);

  // Position side straight section
  G4double strSidePosX = geo->GetMagPipeStraightSidePosX();
  G4double strSidePosZ = geo->GetMagPipeStraightSidePosZ();
//  G4double strSideRotY = geo->GetMagPipeStraightSideRotY();
//  G4RotationMatrix* strSideRot = new G4RotationMatrix;
//  strSideRot->rotateY(strSideRotY);
  G4ThreeVector strSidePos = G4ThreeVector(strSidePosX,0.,strSidePosZ);
  new G4PVPlacement(strSideRot,strSidePos,logicalStraightPipe,"DHSTB002FlangeSide",fMotherVolume,false,0,true);

  // end of test
}

void BeamLineStructure::CreateBeamLine()
{

  // Create all pipes along the beam line from the Be window to the Target junction

  BeamFlagSD* beamFlagSD;
  BeamLineGeometry* geo = BeamLineGeometry::GetInstance();
  G4VisAttributes steelVisAttr   = G4VisAttributes(G4Color::Grey()); // Dark gray
  G4VisAttributes FlagVisAttr   = G4VisAttributes(G4Color::Yellow()); // Beam Flags
  if ( ! fBeamLineIsVisible ) steelVisAttr = G4VisAttributes::Invisible;


  G4double Flag3R = 44.5*mm; // 99mm diameter
  G4double Flag3T = 50*um;  //Use the same of the BeW
  G4double FlagT = geo->GetBeWindowThick();  //Use the same of the BeW

//  // Angular span of the DHSTB002 magnet (45 deg)
  G4double magnetAngle = geo->GetDHSTB002AngularSpan();
//
//  // Create the magnetic volume
//  G4double magvolSizeY = geo->GetMagVolSizeY();
//  G4double magvolMinR = geo->GetMagVolMinRadius();
//  G4double magvolMaxR = geo->GetMagVolMaxRadius();
//  G4Tubs* solidMagVol = new G4Tubs("solidMagVol",magvolMinR,magvolMaxR,0.5*magvolSizeY,0.*deg,magnetAngle);
//  G4LogicalVolume* logicalMagVol = new G4LogicalVolume(solidMagVol,G4Material::GetMaterial("Vacuum"),"logicalMagVol",0,0,0);
//  logicalMagVol->SetVisAttributes(G4VisAttributes::Invisible);
//
//  // Create another volume externally adjacent to magnetic volume: used for cuts
//  G4double extvolSizeY = geo->GetMagVolSizeY();
//  G4double extvolMinR = geo->GetMagVolMaxRadius();
//  G4double extvolMaxR = geo->GetMagVolMaxRadius()+1.*m; // Arbitrary as long as it is large enough
//  G4Tubs* solidExtVol = new G4Tubs("solidExtVol",extvolMinR,extvolMaxR,0.5*extvolSizeY,0.*deg,magnetAngle);
//
//  // Add a constant magnetic field to the magnetic volume
//  G4ThreeVector fieldV(0.,geo->GetDHSTB002MagneticFieldY(),0.);
//  G4MagneticField* magField = new G4UniformMagField(fieldV);
//  G4FieldManager* localFieldManager = new G4FieldManager(magField);
//  logicalMagVol->SetFieldManager(localFieldManager,true);
//
//  // Position the magnetic volume at center of H-shaped hole of DHSTB002
//  G4RotationMatrix* rotMagVol = new G4RotationMatrix;
//  //rotMagVol->rotateX(-90.*deg);
//  //rotMagVol->rotateZ(180.*deg);
//  rotMagVol->rotateX(90.*deg);
//  rotMagVol->rotateZ(180.*deg+magnetAngle);
//  G4ThreeVector posMagVol = G4ThreeVector(geo->GetDHSTB002CenterRadius(),0.,geo->GetDHSTB002ExitPosZ());
//  new G4PVPlacement(rotMagVol,posMagVol,logicalMagVol,"DHSTB002MagneticVolume",fMotherVolume,false,0,true);

  // Create bended section of the rectangular beam pipe and position it inside the magnetic volume

  // Bended pipe - Full
  G4double magBPBSizeY = geo->GetMagPipeSizeY();
///  G4double magBPBMinR = geo->GetMagPipeMinRadius();
///  G4double magBPBMaxR = geo->GetMagPipeMaxRadius();
///  G4Tubs* solidBeamPipeBendFull = new G4Tubs("solidBeamPipeBendFull",magBPBMinR,magBPBMaxR,0.5*magBPBSizeY,0.*deg,magnetAngle);
///
///  // Linear pipe - Full
    G4double magBPLSizeX = geo->GetMagPipeMaxRadius()-geo->GetMagPipeMinRadius();
    G4double magBPLSizeY = geo->GetMagPipeLineLength();
    G4double magBPLSizeZ = geo->GetMagPipeSizeY();
///  G4Box* solidBeamPipeLineFull = new G4Box("solidBeamPipeLineFull",0.5*magBPLSizeX,0.5*magBPLSizeY,0.5*magBPLSizeZ);
///
///  // Complete beam pipe - Full
///  G4ThreeVector magBPLPos = G4ThreeVector(geo->GetDHSTB002CenterRadius(),0.5*magBPLSizeY,0.);
///  G4UnionSolid* solidBeamPipeFull = new G4UnionSolid("solidBeamPipeFull",solidBeamPipeBendFull,solidBeamPipeLineFull,0,magBPLPos);
///
///  // Bended pipe - Hole
///  G4double magBPBHSizeY = geo->GetMagPipeHoleSizeY();
///  G4double magBPBHMinR = geo->GetMagPipeHoleMinRadius();
///  G4double magBPBHMaxR = geo->GetMagPipeHoleMaxRadius();
///  G4Tubs* solidBeamPipeBendHole  = new G4Tubs("solidBeamPipeBendHole",magBPBHMinR,magBPBHMaxR,0.5*magBPBHSizeY,0.*deg,magnetAngle);
///
///  // Linear pipe - Hole
///  G4double magBPLHSizeX = geo->GetMagPipeHoleMaxRadius()-geo->GetMagPipeHoleMinRadius();
///  G4double magBPLHSizeY = geo->GetMagPipeLineLength();
///  G4double magBPLHSizeZ = geo->GetMagPipeHoleSizeY();
///  G4Box* solidBeamPipeLineHole = new G4Box("solidBeamPipeLineHole",0.5*magBPLHSizeX,0.5*magBPLHSizeY,0.5*magBPLHSizeZ);
///
///  // Complete beam pipe - Hole
///  G4UnionSolid* solidBeamPipeHole = new G4UnionSolid("solidBeamPipeHole",solidBeamPipeBendHole,solidBeamPipeLineHole,0,magBPLPos);
///
///  // Complete beam pipe (remove hole from full pipe)
///  G4SubtractionSolid* solidBeamPipe = new G4SubtractionSolid("solidBeamPipe",solidBeamPipeFull,solidBeamPipeHole,0,G4ThreeVector(0.,0.,0.));
///
///  // Now split the complete beam pipe in a section inside the magnetic volume...
///  G4SubtractionSolid* solidBeamPipeIn = new G4SubtractionSolid("solidBeamPipeIn",solidBeamPipe,solidExtVol,0,G4ThreeVector(0.,0.,0.));
///  G4LogicalVolume* logicalBeamPipeIn = new G4LogicalVolume(solidBeamPipeIn,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalBeamPipeIn",0,0,0);
///  logicalBeamPipeIn->SetVisAttributes(steelVisAttr);
///  new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicalBeamPipeIn,"DHSTB002BeamPipeIn",logicalMagVol,false,0,true);
///
///  // ...and a section outside it
///  G4SubtractionSolid* solidBeamPipeOut = new G4SubtractionSolid("solidBeamPipeOut",solidBeamPipe,solidMagVol,0,G4ThreeVector(0.,0.,0.));
///  G4LogicalVolume* logicalBeamPipeOut = new G4LogicalVolume(solidBeamPipeOut,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalBeamPipeOut",0,0,0);
///  logicalBeamPipeOut->SetVisAttributes(steelVisAttr);
///  new G4PVPlacement(rotMagVol,posMagVol,logicalBeamPipeOut,"DHSTB002BeamPipeOut",fMotherVolume,false,0,true);
///
///  // Create the straight section of the beam pipe (with its flange)
///  // and position it at entrance and exits of magnet section of the beam line
///
///  G4double strPipeSizeX = geo->GetMagPipeSizeX();
///  G4double strPipeSizeY = geo->GetMagPipeSizeY();
  G4double strPipeSizeZ = geo->GetMagPipeStraightLength();
///  G4double strHoleSizeX = geo->GetMagPipeHoleSizeX();
///  G4double strHoleSizeY = geo->GetMagPipeHoleSizeY();
///  G4double strHoleSizeZ = geo->GetMagPipeStraightLength()+10.*um;
///  G4double strFlangeR = geo->GetMagPipeFlangeRadius();
///  G4double strFlangeThick = geo->GetMagPipeFlangeThick();
///
///  G4Box* solidStrFull = new G4Box("solidStrFull",0.5*strPipeSizeX,0.5*strPipeSizeY,0.5*strPipeSizeZ);
///  G4Box* solidStrHole = new G4Box("solidStrHole",0.5*strHoleSizeX,0.5*strHoleSizeY,0.5*strHoleSizeZ);
///  G4Tubs* solidStrFlange = new G4Tubs("solidStrFlange",0.,strFlangeR,0.5*strFlangeThick,0.*deg,360.*deg);
///
///  G4ThreeVector posFlange = G4ThreeVector(0.,0.,geo->GetMagPipeFlangePosZ());
///  G4UnionSolid* solidStrFullFlange = new G4UnionSolid("solidStrFullFlange",solidStrFull,solidStrFlange,0,posFlange);
///  G4SubtractionSolid* solidStraightPipe = new G4SubtractionSolid("solidStraightPipe",solidStrFullFlange,solidStrHole,0,G4ThreeVector(0.,0.,0.));
///  G4LogicalVolume* logicalStraightPipe = new G4LogicalVolume(solidStraightPipe,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalStraightPipe",0,0,0);
///  logicalStraightPipe->SetVisAttributes(steelVisAttr);
///
///  // Position front straight section
  G4double strFrontPosX = geo->GetMagPipeStraightFrontPosX();
  G4double strFrontPosZ = geo->GetMagPipeStraightFrontPosZ();
  G4double strFrontRotY = geo->GetMagPipeStraightFrontRotY();
  G4RotationMatrix* strFrontRot = new G4RotationMatrix;
  strFrontRot->rotateY(strFrontRotY);
  G4ThreeVector strFrontPos = G4ThreeVector(strFrontPosX,0.,strFrontPosZ);
///  new G4PVPlacement(strFrontRot,strFrontPos,logicalStraightPipe,"DHSTB002FlangeFront",fMotherVolume,false,0,true);

//  ////////////////////////////////////////////////////////////////////////
//  // Beam Flag to monitor beam in different locations M. Raggi 29/08/2019
//  // Flag 2 after the DHSTB002 output pipe
//  ///////////////////////////////////////////////////////////////////////
//
//  G4double FlagR = geo->GetBeWindowRadius()-15*um; //Use the same of the BeW
//  G4double FlagT = geo->GetBeWindowThick();  //Use the same of the BeW
//
//  G4double     FlagFrontPosX = geo->GetMagPipeStraightFrontPosX();
//  G4double     FlagFrontPosZ = geo->GetMagPipeStraightFrontPosZ()+strPipeSizeZ/2;
//  G4ThreeVector FlagFrontPos = G4ThreeVector(FlagFrontPosX,0.,FlagFrontPosZ);
//
//  G4Tubs* solidBeamFlag2 = new G4Tubs("solidBeamFlag2",0.,FlagR,0.5*FlagT,0.*deg,360.*deg);
//  G4LogicalVolume* logicalBeamFlag2 = new G4LogicalVolume(solidBeamFlag2,G4Material::GetMaterial("Vacuum"),"logicalBeamFlag2",0,0,0);
//  logicalBeamFlag2->SetVisAttributes(FlagVisAttr);
//  
//  if ( geo->BeamFlagIsEnabled() ) {
//    //   printf("Registering Flag2 %b\n",geo->BeamFlagIsEnabled());
//    new G4PVPlacement(strFrontRot,FlagFrontPos,logicalBeamFlag2,"BeamLineBeamFlag2",fMotherVolume,false,0,true);    
//    G4String BeamFlag2SDName = geo->GetBeamFlag2SensitiveDetectorName();
//    printf("Registering Flag2 SD %s\n",BeamFlag2SDName.data());
//    beamFlagSD = new BeamFlagSD(BeamFlag2SDName);
//    logicalBeamFlag2->SetSensitiveDetector(beamFlagSD);
//    G4SDManager::GetSDMpointer()->AddNewDetector(beamFlagSD);
//  }
//  // end of test
//


  ////////////////////////////////////////////////////////////////////////
  // Flag 5 just before the target M. Raggi 23/09/2019
  ///////////////////////////////////////////////////////////////////////

  G4double      Flag5FrontPosX = 0.;
  G4double      Flag5FrontPosZ = -490.0*mm-Flag3T*0.5;
  G4ThreeVector Flag5FrontPos = G4ThreeVector(Flag5FrontPosX,0.,Flag5FrontPosZ);

  G4Tubs* solidBeamFlag5 = new G4Tubs("solidBeamFlag5",0.,Flag3R,0.5*Flag3T,0.*deg,360.*deg);
  G4LogicalVolume* logicalBeamFlag5 = new G4LogicalVolume(solidBeamFlag5,G4Material::GetMaterial("Vacuum"),"logicalBeamFlag5",0,0,0);
  logicalBeamFlag5->SetVisAttributes(FlagVisAttr);

  ////////////////////////////////////////////////////////////////////////
  // Flag 6 just before TimePix M. Raggi 30/09/2019
  ///////////////////////////////////////////////////////////////////////

  G4double      Flag6FrontPosX = 746.1; //from TPix positioning
  G4double      Flag6FrontPosZ = 2250.6*mm-Flag3T*0.5; //from TPix positioning
  G4ThreeVector Flag6FrontPos = G4ThreeVector(Flag6FrontPosX,0.,Flag6FrontPosZ);

  G4RotationMatrix* rotTPix = new G4RotationMatrix;
  rotTPix->rotateY(-0.314159); //from chamber geometry

  G4Box* solidBeamFlag6 =  new G4Box("solidBeamFlag6",15*cm,10*cm,0.5*1*mm);
  G4LogicalVolume* logicalBeamFlag6 = new G4LogicalVolume(solidBeamFlag6,G4Material::GetMaterial("Vacuum"),"logicalBeamFlag6",0,0,0);
  logicalBeamFlag6->SetVisAttributes(FlagVisAttr);

  //  printf("Registering Flag3 %b\n",geo->BeamFlagIsEnabled());
  if ( geo->BeamFlagIsEnabled() ) {
//    //   printf("Registering Flag3 %b\n",geo->BeamFlagIsEnabled());
//    new G4PVPlacement(strFrontRot,Flag3FrontPos,logicalBeamFlag3,"BeamLineBeamFlag3",fMotherVolume,false,0,true);    
//    logicalBeamFlag3->SetSensitiveDetector(beamFlagSD);

    new G4PVPlacement(strFrontRot,Flag5FrontPos,logicalBeamFlag5,"BeamLineBeamFlag5",fMotherVolume,false,0,true);    
    logicalBeamFlag5->SetSensitiveDetector(beamFlagSD);

    new G4PVPlacement(rotTPix,Flag6FrontPos,logicalBeamFlag6,"BeamLineBeamFlag6",fMotherVolume,false,0,true);    
    logicalBeamFlag6->SetSensitiveDetector(beamFlagSD);
  }
  // end of test

//  // Position back straight section
  G4double strBackPosX = geo->GetMagPipeStraightBackPosX();
  G4double strBackPosZ = geo->GetMagPipeStraightBackPosZ();
  G4double strBackRotY = geo->GetMagPipeStraightBackRotY();
  G4RotationMatrix* strBackRot = new G4RotationMatrix;
  strBackRot->rotateY(strBackRotY);
//  G4ThreeVector strBackPos = G4ThreeVector(strBackPosX,0.,strBackPosZ);
//  new G4PVPlacement(strBackRot,strBackPos,logicalStraightPipe,"DHSTB002FlangeBack",fMotherVolume,false,0,true);
//
//  // Position side straight section
//  G4double strSidePosX = geo->GetMagPipeStraightSidePosX();
//  G4double strSidePosZ = geo->GetMagPipeStraightSidePosZ();
//  G4double strSideRotY = geo->GetMagPipeStraightSideRotY();
//  G4RotationMatrix* strSideRot = new G4RotationMatrix;
//  strSideRot->rotateY(strSideRotY);
//  G4ThreeVector strSidePos = G4ThreeVector(strSidePosX,0.,strSidePosZ);
//  new G4PVPlacement(strSideRot,strSidePos,logicalStraightPipe,"DHSTB002FlangeSide",fMotherVolume,false,0,true);

  // Get position of entrance point to the magnet pipe section
  G4double mpEntPosX = geo->GetMagPipeEnterPosX();
  G4double mpEntPosY = geo->GetMagPipeEnterPosY();
  G4double mpEntPosZ = geo->GetMagPipeEnterPosZ();

  // Create volume for junction sections between magnet pipe and Be flange
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

  // Carve hole inside junction
  G4Tubs* solidBeJunHole = new G4Tubs("solidBeJunHole",0.,beJunRIn,0.5*beJunLen+10.*um,0.*deg,360.*deg);
  G4SubtractionSolid* solidBeJunction = new G4SubtractionSolid("solidBeJunction",solidBeJunFull,solidBeJunHole,0,G4ThreeVector(0.,0.,0.));

  // Create junction logical volume
  G4LogicalVolume* logicalBeJunction = new G4LogicalVolume(solidBeJunction,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalBeJunction",0,0,0);
  logicalBeJunction->SetVisAttributes(steelVisAttr);

  // Position junction close to the magnet entrance
  G4double beJunMgPosX = mpEntPosX+0.5*beJunLen*sin(magnetAngle);
  G4double beJunMgPosY = mpEntPosY;
  G4double beJunMgPosZ = mpEntPosZ-0.5*beJunLen*cos(magnetAngle);
  G4ThreeVector beJunMgPos = G4ThreeVector(beJunMgPosX,beJunMgPosY,beJunMgPosZ);
  G4RotationMatrix* beJunMgRot = new G4RotationMatrix;
  beJunMgRot->rotateY(magnetAngle);
  new G4PVPlacement(beJunMgRot,beJunMgPos,logicalBeJunction,"BeamLineMagnetJunction",fMotherVolume,false,0,true);

  ////////////////////////////////////////////////////////////////////////
  // Beam Flag to monitor beam in different locations M. Raggi 29/08/2019
  ///////////////////////////////////////////////////////////////////////

  G4double      FlagBackPosX = mpEntPosX;
  G4double      FlagBackPosY = mpEntPosY;
  G4double      FlagBackPosZ = mpEntPosZ;
  G4ThreeVector FlagBackPos  = G4ThreeVector(FlagBackPosX,FlagBackPosY,FlagBackPosZ);

  G4Tubs* solidBeamFlag1 = new G4Tubs("solidBeamFlag1",0.,geo->GetBeJunctionRIn()-150*um,0.5*FlagT,0.*deg,360.*deg);
  G4LogicalVolume* logicalBeamFlag1 = new G4LogicalVolume(solidBeamFlag1,G4Material::GetMaterial("Vacuum"),"logicalBeamFlag1",0,0,0);
  logicalBeamFlag1->SetVisAttributes(FlagVisAttr);

  if ( geo->BeamFlagIsEnabled() ) {
    new G4PVPlacement(strBackRot,FlagBackPos,logicalBeamFlag1,"BeamLineBeamFlag1",fMotherVolume,false,0,true);    
    //G4String BeamFlag2SDName = geo->GetBeamFlag2SensitiveDetectorName();
    //  printf("Registering BeW SD %s\n",BeamFlag2SDName.data());
    //  BeamFlagSD* bfdsSD = new BeamFlagSD(BeamFlag2SDName);
    logicalBeamFlag1->SetSensitiveDetector(beamFlagSD);
    //  G4SDManager::GetSDMpointer()->AddNewDetector(bfdsSD);
  }
  // end of test

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
  G4double bePipePosX = mpEntPosX+(beJunLen+0.5*bePipeLen)*sin(magnetAngle);
  G4double bePipePosY = mpEntPosY;
  G4double bePipePosZ = mpEntPosZ-(beJunLen+0.5*bePipeLen)*cos(magnetAngle);
  G4ThreeVector bePipePos = G4ThreeVector(bePipePosX,bePipePosY,bePipePosZ);
  G4RotationMatrix* bePipeRot = new G4RotationMatrix;
  bePipeRot->rotateY(magnetAngle);
  new G4PVPlacement(bePipeRot,bePipePos,logicalBePipe,"BeamLineBePipe",fMotherVolume,false,0,true);

  // For some reason, boolean solid visualization fails to show solid created with fullpipe+flangeF+flangeB-hole
  // Create here the flanges as independent volumes

  G4Tubs* solidBePipeFlgFnt = new G4Tubs("solidBePipeFlgFnt",bePipeROut,bePipeFlgFntR,0.5*bePipeFlgFntT,0.*deg,360.*deg);
  G4LogicalVolume* logicalBePipeFlgFnt = new G4LogicalVolume(solidBePipeFlgFnt,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalBePipeFlangeFront",0,0,0);
  logicalBePipeFlgFnt->SetVisAttributes(steelVisAttr);
  G4double bePipeFFPosX = mpEntPosX+(beJunLen+bePipeLen-0.5*bePipeFlgFntT)*sin(magnetAngle);
  G4double bePipeFFPosY = mpEntPosY;
  G4double bePipeFFPosZ = mpEntPosZ-(beJunLen+bePipeLen-0.5*bePipeFlgFntT)*cos(magnetAngle);
  G4ThreeVector bePipeFFPos = G4ThreeVector(bePipeFFPosX,bePipeFFPosY,bePipeFFPosZ);
  new G4PVPlacement(bePipeRot,bePipeFFPos,logicalBePipeFlgFnt,"BeamLineBePipeFlangeFront",fMotherVolume,false,0,true);

  G4Tubs* solidBePipeFlgBck = new G4Tubs("solidBePipeFlgBck",bePipeROut,bePipeFlgBckR,0.5*bePipeFlgBckT,0.*deg,360.*deg);
  G4LogicalVolume* logicalBePipeFlgBck = new G4LogicalVolume(solidBePipeFlgBck,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalBePipeFlangeBack",0,0,0);
  logicalBePipeFlgBck->SetVisAttributes(steelVisAttr);
  G4double bePipeFBPosX = mpEntPosX+(beJunLen+0.5*bePipeFlgBckT)*sin(magnetAngle);
  G4double bePipeFBPosY = mpEntPosY;
  G4double bePipeFBPosZ = mpEntPosZ-(beJunLen+0.5*bePipeFlgBckT)*cos(magnetAngle);
  G4ThreeVector bePipeFBPos = G4ThreeVector(bePipeFBPosX,bePipeFBPosY,bePipeFBPosZ);
  new G4PVPlacement(bePipeRot,bePipeFBPos,logicalBePipeFlgBck,"BeamLineBePipeFlangeBack",fMotherVolume,false,0,true);

  // Position junction close to Be window
  // Use same logical volume created for Magnet-side junction
  G4double beJunBePosX = mpEntPosX+(beJunLen+bePipeLen+0.5*beJunLen)*sin(magnetAngle);
  G4double beJunBePosY = mpEntPosY;
  G4double beJunBePosZ = mpEntPosZ-(beJunLen+bePipeLen+0.5*beJunLen)*cos(magnetAngle);
  G4ThreeVector beJunBePos = G4ThreeVector(beJunBePosX,beJunBePosY,beJunBePosZ);
  G4RotationMatrix* beJunBeRot = new G4RotationMatrix;
  beJunBeRot->rotateY(magnetAngle);
  new G4PVPlacement(beJunBeRot,beJunBePos,logicalBeJunction,"BeamLineBeWinJunction",fMotherVolume,false,0,true);

  // Position Be windows with relative support flange
  G4double beWinFlgT = geo->GetBeWindowFlangeThick();
  G4double beWinFlgPosX = mpEntPosX+(beJunLen+bePipeLen+beJunLen+0.5*beWinFlgT)*sin(magnetAngle);
  G4double beWinFlgPosY = mpEntPosY;
  G4double beWinFlgPosZ = mpEntPosZ-(beJunLen+bePipeLen+beJunLen+0.5*beWinFlgT)*cos(magnetAngle);
  G4ThreeVector beWinFlgPos = G4ThreeVector(beWinFlgPosX,beWinFlgPosY,beWinFlgPosZ);
  G4RotationMatrix* beWinFlgRot = new G4RotationMatrix;
  beWinFlgRot->rotateY(magnetAngle);
  printf("BeamLine - Be window exit face center CRASH  is at \n");
  new G4PVPlacement(beWinFlgRot,beWinFlgPos,fBeWindowVolume,"BeamLineBeWinVolume",fMotherVolume,false,0,true);

  printf("BeamLine - Be window entry face center is at (%.2f,%.2f,%.2f) mm\n",beWinFlgPosX,beWinFlgPosY,beWinFlgPosZ);

  G4double beWin2FlgPosX = mpEntPosX-(strPipeSizeZ+magBPLSizeY+strPipeSizeZ+0.5*beWinFlgT)*sin(magnetAngle);
  G4double beWin2FlgPosY = mpEntPosY;
  G4double beWin2FlgPosZ = mpEntPosZ+(strPipeSizeZ+magBPLSizeY+strPipeSizeZ+0.5*beWinFlgT)*cos(magnetAngle);
  G4ThreeVector beWin2FlgPos = G4ThreeVector(beWin2FlgPosX,beWin2FlgPosY,beWin2FlgPosZ);
  new G4PVPlacement(beWinFlgRot,beWin2FlgPos,fBeWindowVolume,"BeamLineExitBeWinVolume",fMotherVolume,false,0,true);

  //  printf("BeamLine - Be window exit face center is at (%.2f,%.2f,%.2f) mm\n",beWin2FlgPosX,beWin2FlgPosY,beWin2FlgPosZ);

//  ////////////////////////////////////////////////////////////////////////
//  // Beam Flag to monitor beam in different locations M. Raggi 06/09/2019
//  // Flag 4 on the strait DHSTB002 exit section
//  ///////////////////////////////////////////////////////////////////////
//
//  G4double      Flag4FrontPosX = mpEntPosX-(strPipeSizeZ+magBPLSizeY+strPipeSizeZ+0.5*beWinFlgT +5*cm)*sin(magnetAngle);; 
//  G4double      Flag4FrontPosZ = mpEntPosZ+(strPipeSizeZ+magBPLSizeY+strPipeSizeZ+0.5*beWinFlgT +5*cm)*cos(magnetAngle);
//  G4ThreeVector Flag4FrontPos  = G4ThreeVector(Flag4FrontPosX,beWin2FlgPosY,Flag4FrontPosZ);
//
//  G4Tubs* solidBeamFlag4 = new G4Tubs("solidBeamFlag4",0.,30.*mm,0.1*mm,0.*deg,360.*deg);
//  G4LogicalVolume* logicalBeamFlag4 = new G4LogicalVolume(solidBeamFlag4,G4Material::GetMaterial("Vacuum"),"logicalBeamFlag4",0,0,0);
//  logicalBeamFlag4->SetVisAttributes(FlagVisAttr);
//  
//  if ( geo->BeamFlagIsEnabled() ) {
//    new G4PVPlacement(strSideRot,Flag4FrontPos,logicalBeamFlag4,"BeamLineBeamFlag4",fMotherVolume,false,0,true);    
//    logicalBeamFlag4->SetSensitiveDetector(beamFlagSD);
//  }

  if ( geo->QuadrupolesAreEnabled() ) {
    
    ///////////////////////////////////////////////////////
    // Creates the pair of quadrupoles located before the 
    // entrance of DHSTB002
    ///////////////////////////////////////////////////////
    
    G4double Q1BGradient    = geo->GetQ1MagneticFieldGrad();
    G4double Q2BGradient    = geo->GetQ2MagneticFieldGrad();

    G4ThreeVector BOrigin = G4ThreeVector(0.,0.,0.);
    G4RotationMatrix* Q1Rot = new G4RotationMatrix;
    Q1Rot->rotateY(magnetAngle);
    Q1Rot->rotateZ(45*deg);

    G4RotationMatrix* Q2Rot = new G4RotationMatrix;
    Q2Rot->rotateY(magnetAngle);
    Q2Rot->rotateZ(-45*deg);

    // Generating quadrupole box
    G4double QuadMagSizeX = geo->GetQuadMagSizeX();
    G4double QuadMagSizeY = geo->GetQuadMagSizeY();
    G4double QuadMagSizeZ = geo->GetQuadMagSizeZ();
    G4Box*   solidQuadMagField = new G4Box("solidQuadMagField",0.5*QuadMagSizeX,0.5*QuadMagSizeY,0.5*QuadMagSizeZ);

    // Generating quadrupole Q1
    G4double Q1PosX = beJunMgPosX+379*sin(magnetAngle)*mm; 
    G4double Q1PosY = beJunMgPosY; 
    G4double Q1PosZ = beJunMgPosZ-379*cos(magnetAngle)*mm; ; 
    G4ThreeVector Q1Pos = G4ThreeVector(Q1PosX,Q1PosY,Q1PosZ);

    printf("Creating Q1 quadrupole with gradient %f ",Q1BGradient*m/tesla);
    QuadSetup* Q1FieldManager = new QuadSetup(Q1BGradient,Q1Pos,Q1Rot);
    G4LogicalVolume* logicQ1MagField = new G4LogicalVolume(solidQuadMagField,G4Material::GetMaterial("Vacuum"),"logicQ1MagField",0,0,0);
    logicQ1MagField->SetFieldManager(Q1FieldManager->GetLocalFieldManager(),true);
    new G4PVPlacement(Q1Rot,Q1Pos,logicQ1MagField,"Q1",fMotherVolume,false,0,true);

    // Generating quadrupole Q2
    G4double Q1Q2Dist = geo->GetQ1Q2Dist();

    G4double Q2PosX   = beJunMgPosX+379*sin(magnetAngle)*mm+Q1Q2Dist*sin(magnetAngle); 
    G4double Q2PosY   = beJunMgPosY; 
    G4double Q2PosZ   = beJunMgPosZ-379*cos(magnetAngle)*mm-Q1Q2Dist*cos(magnetAngle);  
    G4ThreeVector Q2Pos = G4ThreeVector(Q2PosX,Q2PosY,Q2PosZ);

    printf("Creating Q2 quadrupole with gradient %f ",Q2BGradient*m/tesla);
    QuadSetup* Q2FieldManager = new QuadSetup(Q2BGradient,Q2Pos,Q2Rot);
    G4LogicalVolume* logicQ2MagField = new G4LogicalVolume(solidQuadMagField,G4Material::GetMaterial("Vacuum"),"logicQ2MagField",0,0,0);
    logicQ2MagField->SetFieldManager(Q2FieldManager->GetLocalFieldManager(),true);
    new G4PVPlacement(Q2Rot,Q2Pos,logicQ2MagField,"Q2",fMotherVolume,false,0,true);

  }

}
