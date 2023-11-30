// BeamLineStructure.cc
// ---------------------------o-----------------------------------
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
#include "BeamLineMessenger.hh"

#include "G4SDManager.hh" // 29/04/2019 M. Raggi
#include "BeWSD.hh"       // 29/04/2019 M. Raggi
#include "MylarWSD.hh"    // 18/06/2020 M. Raggi
#include "BeamFlagSD.hh"  // 29/08/2019 M. Raggi

#include "QuadrupoleMagField.hh"  // M. Raggi 8/04/2019
#include "QuadSetup.hh"           // M. Raggi 10/04/2019

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
  delete fBeamLineMessenger;
}

void BeamLineStructure::CreateGeometry()
{
  geo = BeamLineGeometry::GetInstance();
  G4double fLineSetup = geo->GetBeamLineSetup();
  
  if (geo->BeamFlagIsEnabled() ) {
    BeamFlag2SDName = geo->GetBeamFlag2SensitiveDetectorName();
    beamFlagSD = new BeamFlagSD(BeamFlag2SDName);
    //    G4SDManager::GetSDMpointer()->AddNewDetector(beamFlagSD);
  }
  
  if(fLineSetup==0.){
    G4cout<<"######### Create 2019 Beam Line ###############"<<fLineSetup<<G4endl;
    CreateBeThinWindow();   // Create BeWindow
    CreateDHSTB002Magnet(); // Create DHSTB002 magnet
    CreateBeamLine();       // Create Beam line 2019
  }
  if(fLineSetup==1.){
    G4cout<<"######### Create 2020 Beam Line ###############"<<fLineSetup<<G4endl;
    CreateMylarThinWindow(); // Create mylar Window
    CreateDHSTB002Magnet();
    CreateDHSTB001Magnet();
    CreateBeamLine2020();    // Beam line 2020
  }
  
  if(fLineSetup==2.){
    G4cout<<"######### Create 2022 Beam Line ###############"<<fLineSetup<<G4endl;
    CreateMylarThinWindow(); // Create mylar Window
    CreateDHSTB002Magnet();
    CreateDHSTB001Magnet();
    CreateWallAndPipe();
    PositionAllQuads();
    CreateAllSLTB();
    CreatePulsedMagnet();
    CreateBeamLine2022();    // Beam line 2022
  }
}

///////////////////////////////////////////////////////
// Creates a quadrupole  K>0 Defocus on X   Focus On Y
//                       K<0 Focus   on X Defocus On Y
///////////////////////////////////////////////////////
G4LogicalVolume* BeamLineStructure::CreateQuadMagnets(G4double Grad, G4double Length, G4double Radius, G4ThreeVector Pos, G4RotationMatrix* Rot)
{
  //create the Qaudrupole support structure around the pipe
  G4VisAttributes QuadVisAttr = G4VisAttributes(G4Colour::Green());
  G4Box* solidSupportFull = new G4Box("solidSupportFull",2.0*Radius,2.0*Radius,0.5*Length);
  G4Box* solidSupportHole = new G4Box("solidSupportHole",1.4*Radius,1.4*Radius,0.5*Length+100*um);
  
  G4SubtractionSolid* solidSupport = new G4SubtractionSolid("solidSupport",solidSupportFull,solidSupportHole,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicalSupport = new G4LogicalVolume(solidSupport,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicSupport",0,0,0);

  logicalSupport->SetVisAttributes(QuadVisAttr);
  //  logicalSupport->SetName("logicQuadSupport");
  new G4PVPlacement(Rot,Pos,logicalSupport,"QuadSuport",fMotherVolume,false,0,true);

  G4Tubs* solidQuadMagField = new G4Tubs("solidQuadMagField",0.,Radius,0.5*Length,0.*deg,360.*deg);  
  printf("Creating Qx quadrupole with gradient %f %f %f %f\n",Grad*m/tesla,Pos.x(),Pos.y(),Pos.z());

  QuadSetup* QFieldManager = new QuadSetup(Grad,Pos,Rot);
  G4LogicalVolume* logicQMagField = new G4LogicalVolume(solidQuadMagField,G4Material::GetMaterial("Vacuum"),"logicQ1MagField",0,0,0);
  logicQMagField->SetFieldManager(QFieldManager->GetLocalFieldManager(),true);
  return logicQMagField;
}

void BeamLineStructure::CreateMylarThinWindow()
{
  ///////////////////////////////////////////////////////
  // Thin Mylar 140um window mounted before DHSTB002
  // and pipes in the upstream region
  ///////////////////////////////////////////////////////
  
  G4VisAttributes steelVisAttr = G4VisAttributes(G4Colour::Grey());
  G4VisAttributes MylarVisAttr = G4VisAttributes(G4Colour::Red());
  if(!fBeamLineIsVisible){
    MylarVisAttr = G4VisAttributes::Invisible;
    steelVisAttr = G4VisAttributes::Invisible;
  }  
  // Create Mylar thin window and its support flange

  G4double mylarWinR = geo->GetMylarWindowRadius();
  //  G4double mylarWinT = geo->GetMylarWindowThick();  //from the geometry
  G4double mylarWinT = geo->GetWindowThickness();  //from the new data card M. Raggi 29/03/2021
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
  // logicalMylarWinFlange->SetVisAttributes(steelVisAttr);
  logicalMylarWinFlange->SetVisAttributes(MylarVisAttr);

  // Position Mylar window and its flange inside top volume
  // Shift Mylar window so that its entry face is at center of support flange
  new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicalMylarWinFlange,"BeamLineMylarWinFlange",fMylarWindowVolume,false,0,true);

  if( geo->MylarWindowIsEnabled() ) {
    new G4PVPlacement(0,G4ThreeVector(0.,0.,0.5*mylarWinT),logicalMylarWin,"BeamLineMylarWindow",fMylarWindowVolume,false,0,true);    
    // The Mylar window is a sensitive detector
    G4String MylarWSDName = geo->GetMylarWSensitiveDetectorName();
    printf("Registering MylarW SD %s with Thickness %f\n",MylarWSDName.data(),mylarWinT);
    MylarWSD* mylarWSD = new MylarWSD(MylarWSDName);
    fMylarWindowVolume->SetSensitiveDetector(mylarWSD);
    G4SDManager::GetSDMpointer()->AddNewDetector(mylarWSD);
  }
}

// written by M. Raggi 02.2021
void BeamLineStructure::CreateBeamLine2020()
{
  G4VisAttributes steelVisAttr   = G4VisAttributes(G4Color::Grey()); // Dark gray
  G4VisAttributes FlagVisAttr   = G4VisAttributes(G4Color::Yellow()); // Beam Flags
  G4VisAttributes DentroilMuroVisAttr   = G4VisAttributes(G4Color::Red()); // Beam Flags

  if ( ! fBeamLineIsVisible ) steelVisAttr = G4VisAttributes::Invisible;

  //Gettining intial positions
  G4double mpEntPosX = geo->GetMagPipeEnterPosX();
  G4double mpEntPosY = geo->GetMagPipeEnterPosY();
  G4double mpEntPosZ = geo->GetMagPipeEnterPosZ();
  G4double beJunLen  = geo->GetBeJunctionLength();
  G4double bePipeLen = geo->GetBePipeLength();
  G4double magnetAngle = geo->GetDHSTB002AngularSpan();

// Create long pipe between DHSTB002 magnet and BTF Walls
//  G4double WallPipeLen  = geo->GetBePipeLength();
  G4double WallPipeLen  = geo->GetDHSTB002WallDistance();
  G4double WallPipeRIn  = geo->Get2020PipeInnerRadius();
  G4double WallPipeROut = geo->Get2020PipeOuterRadius();

  // Add flange on front (upstream) side
  //G4Tubs* solidBePipeFlgFnt = new G4Tubs("solidBePipeFlgFnt",0.,bePipeFlgFntR,0.5*bePipeFlgFntT,0.*deg,360.*deg);
  //G4UnionSolid* solidBePipeFull2 = new G4UnionSolid("solidBePipeFull2",solidBePipeFull1,solidBePipeFlgFnt,0,G4ThreeVector(0.,0.,-0.5*bePipeLen+0.5*bePipeFlgFntT));

  // Add flange on back (downstream) side
  //G4Tubs* solidBePipeFlgBck = new G4Tubs("solidBePipeFlgBck",0.,bePipeFlgBckR,0.5*bePipeFlgBckT,0.*deg,360.*deg);
  //G4UnionSolid* solidBePipeFull = new G4UnionSolid("solidBePipeFull",solidBePipeFull2,solidBePipeFlgBck,0,G4ThreeVector(0.,0.,0.5*bePipeLen-0.5*bePipeFlgBckT));

  // Create solid pipe From DHSTB002 entrance and BTF Wall
  G4Tubs* solidWallPipeFull1 = new G4Tubs("solidWallPipeFull1",0.,WallPipeROut,0.5*WallPipeLen,0.*deg,360.*deg);

  // Carve hole inside pipe (add usual small tolerance)
  G4Tubs* solidWallPipeHole = new G4Tubs("solidWallPipeHole",0.,WallPipeRIn,0.5*WallPipeLen+10.*um,0.*deg,360.*deg);
  G4SubtractionSolid* solidWallPipe = new G4SubtractionSolid("solidWallPipe",solidWallPipeFull1,solidWallPipeHole,0,G4ThreeVector(0.,0.,0.));

  // Create Wall Pipe logical volume
  G4LogicalVolume* logicalWallPipe = new G4LogicalVolume(solidWallPipe,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalWallPipe",0,0,0);
  logicalWallPipe->SetVisAttributes(steelVisAttr);

  // Position long pipe in between DHSTB002 and BTF wall
  G4double WallPipePosX = mpEntPosX+(0.5*WallPipeLen)*sin(magnetAngle);
  G4double WallPipePosY = mpEntPosY;
  G4double WallPipePosZ = mpEntPosZ-(0.5*WallPipeLen)*cos(magnetAngle);
  G4ThreeVector WallPipePos = G4ThreeVector(WallPipePosX,WallPipePosY,WallPipePosZ);
  G4RotationMatrix* WallPipeRot = new G4RotationMatrix;
  WallPipeRot->rotateY(magnetAngle);
  new G4PVPlacement(WallPipeRot,WallPipePos,logicalWallPipe,"BeamLineWallPipe",fMotherVolume,false,0,true);

  // Create solid pipe From DHSTB002 entrance and BTF Wall
  G4double InWallPipeLen  = geo->GetWallPipeLen();
  G4Tubs* solidInWallPipeFull1 = new G4Tubs("solidInWallPipeFull1",0.,WallPipeROut,0.5*InWallPipeLen,0.*deg,360.*deg);

  // Carve hole inside pipe (add usual small tolerance)
  G4Tubs* solidInWallPipeHole = new G4Tubs("solidInWallPipeHole",0.,WallPipeRIn,0.5*InWallPipeLen+10.*um,0.*deg,360.*deg);
  G4SubtractionSolid* solidInWallPipe = new G4SubtractionSolid("solidInWallPipe",solidInWallPipeFull1,solidInWallPipeHole,0,G4ThreeVector(0.,0.,0.));

  // Create Wall Pipe logical volume
  G4LogicalVolume* logicalInWallPipe = new G4LogicalVolume(solidInWallPipe,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalInWallPipe",0,0,0);
  logicalInWallPipe->SetVisAttributes(DentroilMuroVisAttr);

  // Position long pipe in between DHSTB002 and BTF wall
  G4double InWallPipePosX = mpEntPosX+(WallPipeLen+0.5*InWallPipeLen)*sin(magnetAngle);
  G4double InWallPipePosY = mpEntPosY;
  G4double InWallPipePosZ = mpEntPosZ-(WallPipeLen+0.5*InWallPipeLen)*cos(magnetAngle);
  G4ThreeVector InWallPipePos = G4ThreeVector(InWallPipePosX,InWallPipePosY,InWallPipePosZ);
  new G4PVPlacement(WallPipeRot,InWallPipePos,logicalInWallPipe,"BeamLineInWallPipe",fMotherVolume,false,0,true);

  // Create solid pipe From WALL to Mylar Window Into Linac
  G4double InLinacPipeLen  = geo->GetWallMylarWinDistance();
  G4Tubs* solidInLinacPipeFull1 = new G4Tubs("solidInLinacPipeFull1",0.,WallPipeROut,0.5*InLinacPipeLen,0.*deg,360.*deg);

  // Carve hole inside pipe (add usual small tolerance)
  G4Tubs* solidInLinacPipeHole = new G4Tubs("solidInLinacPipeHole",0.,WallPipeRIn,0.5*InLinacPipeLen+10.*um,0.*deg,360.*deg);
  G4SubtractionSolid* solidInLinacPipe = new G4SubtractionSolid("solidInLinacPipe",solidInLinacPipeFull1,solidInLinacPipeHole,0,G4ThreeVector(0.,0.,0.));

  // Create Wall Pipe logical volume
  G4LogicalVolume* logicalInLinacPipe = new G4LogicalVolume(solidInLinacPipe,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalInLinacPipe",0,0,0);
  //logicalInLinacPipe->SetVisAttributes(steelVisAttr);

  // Position long pipe in between Wall and Mylar window
  G4double InLinacPipePosX = mpEntPosX+(WallPipeLen+InWallPipeLen+0.5*InLinacPipeLen)*sin(magnetAngle);
  G4double InLinacPipePosY = mpEntPosY;
  G4double InLinacPipePosZ = mpEntPosZ-(WallPipeLen+InWallPipeLen+0.5*InLinacPipeLen)*cos(magnetAngle);
  G4ThreeVector InLinacPipePos = G4ThreeVector(InLinacPipePosX,InLinacPipePosY,InLinacPipePosZ);
  new G4PVPlacement(WallPipeRot,InLinacPipePos,logicalInLinacPipe,"BeamLineInLinacPipe",fMotherVolume,false,0,true);

  G4double LinacWallThick  = geo->GetWallThickness();
  G4double WallHoleRadius  = geo->GetWallHoleRadius();

  G4VSolid* solidLinacWallFull = new G4Box("solidLinacWallFull",LinacWallThick*0.5,2*m,2*m);
  G4Tubs* solidLinacWallHole = new G4Tubs("solidLinacWallHole",0.,WallPipeROut*2.5,0.5*LinacWallThick*1.5+100.*um,0.*deg,360.*deg);

  G4SubtractionSolid* solidLinacWall = new G4SubtractionSolid("solidLinacWall",solidLinacWallFull,solidLinacWallHole,WallPipeRot,G4ThreeVector(0,0.,mpEntPosZ-(+LinacWallThick*0.5+293.*cm)-(InWallPipePosZ)));
  G4LogicalVolume* logicLinacWall = new G4LogicalVolume(solidLinacWall,G4Material::GetMaterial("G4_CONCRETE"),"logicLinacWall", 0, 0, 0);
  //  G4ThreeVector LinacWallPos = G4ThreeVector(InWallPipePosX,InWallPipePosY,InWallPipePosZ);
  G4ThreeVector LinacWallPos = G4ThreeVector(293*cm + mpEntPosX+LinacWallThick*0.5,0.,InWallPipePosZ);
  //  new G4PVPlacement(,LinacWallPos,logicLinacWall,"LinacWall",fMotherVolume,false,0,true);
  new G4PVPlacement(0,LinacWallPos,logicLinacWall,"LinacWall",fMotherVolume,false,0,true);

  // Position Mylar window with relative support flange
  G4double MylarWinFlgT    = geo->GetMylarWindowFlangeThick();
  G4double MylarWinFlgPosX = mpEntPosX+(WallPipeLen+InWallPipeLen+InLinacPipeLen+0.5*MylarWinFlgT)*sin(magnetAngle);
  G4double MylarWinFlgPosY = mpEntPosY;
  G4double MylarWinFlgPosZ = mpEntPosZ-(WallPipeLen+InWallPipeLen+InLinacPipeLen+0.5*MylarWinFlgT)*cos(magnetAngle);
  G4ThreeVector MylarWinFlgPos = G4ThreeVector(MylarWinFlgPosX,MylarWinFlgPosY,MylarWinFlgPosZ);
  G4RotationMatrix* MylarWinFlgRot = new G4RotationMatrix;
  MylarWinFlgRot->rotateY(magnetAngle);
  printf("BeamLine - Mylar window exit face center CRASH  is at %f %f\n",MylarWinFlgPosX,MylarWinFlgPosZ);
  printf("BeamLine - DHSTB002 exit %f %f\n",mpEntPosX,mpEntPosZ);
  new G4PVPlacement(MylarWinFlgRot,MylarWinFlgPos,fMylarWindowVolume,"BeamLineMylarWinVolume",fMotherVolume,false,0,true);

  ////////////////////////////////////////////////////////////////////////
  // Collimators after mylar sltb4(LR) sltb3(UPDW)  Mylar 
  ///////////////////////////////////////////////////////////////////////
  // November 2020 tune L. Foggetta
  //TB4R   26.16 TB4L   22.30          TB3 Up 26.06  TB3 Down 23.32	
  //TB4R_M 28.44 TB4L_M 24.01 	       TB3U_M 26.024 TB3D_M   26.39    	
  //TB4Ape  2.28    +    1.71 = 4mm    TB3Ape  0     +         3.07 = 3.07 mm

  // P. Valente to be checked with Luca
  //TB2R        TB2L   22.30           
  //TB2R_M 26.6 TB2L_M 28.29 	       
  //TB2Ape  1.71 = 1.71mm    

  std::cout<<"Set collimators aperture SLTB3 "<<geo->GetSLTB3Aperture()<<std::endl;
  std::cout<<"Set collimators aperture SLTB4 "<<geo->GetSLTB4Aperture()<<std::endl;
  std::cout<<"Set collimators aperture SLTB5 "<<geo->GetSLTB5Aperture()<<std::endl;
  G4double SLTBThickness = 35.0*mm;
  // Support of the collimatros just to visualize them
  G4VisAttributes CollVisAttr = G4VisAttributes(G4Colour::Blue());
  G4double Radius=60*mm;
  G4Box* solidCollSupportFull = new G4Box("solidCollSupportFull",2.0*Radius,2.0*Radius,SLTBThickness*0.5);
  G4Box* solidCollSupportHole = new G4Box("solidCollSupportHole",1.4*Radius,1.4*Radius,SLTBThickness*0.5+100*um);
  
  G4SubtractionSolid* solidCollSupport = new G4SubtractionSolid("solidCollSupport",solidCollSupportFull,solidCollSupportHole,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicalCollSupport = new G4LogicalVolume(solidCollSupport,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicCollSupport",0,0,0);

  logicalCollSupport->SetVisAttributes(CollVisAttr);
  //  logicalSupport->SetName("logicCollSupport");
  
  G4double SLTB3ToMylar = 309.24*mm;
  G4double SLTB3Aperture = geo->GetSLTB3Aperture();
  G4double SLTB3PosX = MylarWinFlgPosX-(SLTB3ToMylar+SLTBThickness*0.5)*sin(magnetAngle);
  G4double SLTB3PosY = MylarWinFlgPosY;							 
  G4double SLTB3PosZ = MylarWinFlgPosZ+(SLTB3ToMylar+SLTBThickness*0.5)*cos(magnetAngle);

  G4double SLTB4ToMylar = 519.24*mm;
  G4double SLTB4Aperture = geo->GetSLTB4Aperture();

  G4double SLTB4PosX = MylarWinFlgPosX-(SLTB4ToMylar+SLTBThickness*0.5)*sin(magnetAngle);
  G4double SLTB4PosY = MylarWinFlgPosY;							 
  G4double SLTB4PosZ = MylarWinFlgPosZ+(SLTB4ToMylar+SLTBThickness*0.5)*cos(magnetAngle);

  G4ThreeVector SLTB4Pos = G4ThreeVector(SLTB4PosX,SLTB4PosY,SLTB4PosZ);    
  G4ThreeVector SLTB3Pos = G4ThreeVector(SLTB3PosX,SLTB3PosY,SLTB3PosZ);    
  
  G4Tubs* solidSLTB3Full = new G4Tubs("solidSLTB3Full",0.,WallPipeRIn-0.1*mm,SLTBThickness,0.*deg,360.*deg);
  //  G4VSolid* solidSLTB3Hole = new G4Box("solidSLTB3Hole",SLTB3Aperture*0.5,WallPipeRIn-0.05*mm,SLTBThickness+2*mm);
  G4VSolid* solidSLTB3Hole =new G4Box("solidSLTB3Hole",WallPipeRIn-0.1*mm,SLTB3Aperture*0.5,SLTBThickness+2*mm);
  G4SubtractionSolid* solidSLTB3 = new G4SubtractionSolid("solidSLTB3",solidSLTB3Full,solidSLTB3Hole,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume*  logicalSLTB3 = new G4LogicalVolume(solidSLTB3,G4Material::GetMaterial("G4_W"),"logicalSLTB3",0,0,0);
  logicalSLTB3->SetVisAttributes(steelVisAttr);

  G4Tubs* solidSLTB4Full = new G4Tubs("solidSLTB4Full",0.,WallPipeRIn-0.1*mm,SLTBThickness,0.*deg,360.*deg);
  //  G4VSolid* solidSLTB4Hole = new G4Box("solidSLTB4Hole",WallPipeRIn-0.05*mm,SLTB4Aperture*0.5,SLTBThickness+2*mm);
  G4VSolid* solidSLTB4Hole = new G4Box("solidSLTB4Hole",SLTB4Aperture*0.5,WallPipeRIn-0.05*mm,SLTBThickness+2*mm);
  G4SubtractionSolid* solidSLTB4 = new G4SubtractionSolid("solidSLTB4",solidSLTB4Full,solidSLTB4Hole,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicalSLTB4 = new G4LogicalVolume(solidSLTB4,G4Material::GetMaterial("G4_W"),"logicalSLTB4",0,0,0);
  logicalSLTB4->SetVisAttributes(steelVisAttr);

  // positioning supports 
  new G4PVPlacement(MylarWinFlgRot,SLTB3Pos,logicalCollSupport,"CollSuport",fMotherVolume,false,0,true);
  new G4PVPlacement(MylarWinFlgRot,SLTB4Pos,logicalCollSupport,"CollSuport",fMotherVolume,false,0,true);
    
  // positioning collimators
  new G4PVPlacement(MylarWinFlgRot,SLTB3Pos,logicalSLTB3,"BeamSLTB3",fMotherVolume,false,0,true);
  new G4PVPlacement(MylarWinFlgRot,SLTB4Pos,logicalSLTB4,"BeamSLTB4",fMotherVolume,false,0,true);

  //***********************************************
  // Setup quadrupoles pairs along the beam line
  //***********************************************
  if(!geo->QuadrupolesAreEnabled()){
    geo -> SetQ1MagneticFieldGrad(0.);
    geo -> SetQ2MagneticFieldGrad(0.);
    geo -> SetQ3MagneticFieldGrad(0.);
    geo -> SetQ4MagneticFieldGrad(0.);
  }

  if ( geo->QuadrupolesAreEnabled() ) {    
    G4VisAttributes QuadVisAttr  = G4VisAttributes(G4Colour::Green());
    
    G4double Q4Radius       = geo -> Get2020PipeInnerRadius()-0.1*mm; // get the 2019 values
    G4double Q4Leng         = geo -> GetQuadMagSizeZ();             // get the 2019 values
    G4RotationMatrix* Q4Rot = new G4RotationMatrix;
    Q4Rot->rotateY(magnetAngle);

    // Position junction close to the magnet entrance
    G4double beJunMgPosX = mpEntPosX+0.5*beJunLen*sin(magnetAngle);
    G4double beJunMgPosY = mpEntPosY;
    G4double beJunMgPosZ = mpEntPosZ-0.5*beJunLen*cos(magnetAngle);

    G4double Q1BGradient    = geo -> GetQ1MagneticFieldGrad();
    G4double Q2BGradient    = geo -> GetQ2MagneticFieldGrad();
    G4double Q3BGradient    = geo -> GetQ3MagneticFieldGrad();
    G4double Q4BGradient    = geo -> GetQ4MagneticFieldGrad();

    // Setting up Q4 quadrupole near DHSTB002    
    G4double Q4DistFromDHSTB002=geo->GetQ4DistFromDHSTB002();
    G4double Q4PosX = beJunMgPosX+Q4DistFromDHSTB002*sin(magnetAngle); 
    G4double Q4PosY = beJunMgPosY; 
    G4double Q4PosZ = beJunMgPosZ-Q4DistFromDHSTB002*cos(magnetAngle); 
    G4ThreeVector Q4Pos = G4ThreeVector(Q4PosX,Q4PosY,Q4PosZ);

    G4LogicalVolume* logicQ4Quad = CreateQuadMagnets(-Q4BGradient,Q4Leng,Q4Radius,Q4Pos,Q4Rot);
    logicQ4Quad->SetName("logicQ4Quad");
    logicQ4Quad->SetVisAttributes(QuadVisAttr);
    new G4PVPlacement(Q4Rot,Q4Pos,logicQ4Quad,"Q4Quad",fMotherVolume,false,0,true);
    printf("Placing Q4 quadrupole with gradient %f \n",-Q4BGradient*m/tesla);

    // Q3 quadrupole first after BTF wall
    G4double Q3Q4Dist = geo->GetQ3Q4Dist();
    G4double Q3PosX   = Q4PosX+Q3Q4Dist*sin(magnetAngle); 
    G4double Q3PosY   = Q4PosY; 
    G4double Q3PosZ   = Q4PosZ-Q3Q4Dist*cos(magnetAngle);  
    G4ThreeVector Q3Pos = G4ThreeVector(Q3PosX,Q3PosY,Q3PosZ);

    G4RotationMatrix* Q3Rot = new G4RotationMatrix;
    Q3Rot->rotateY(magnetAngle);

    // Negative gradient focus on X
    G4LogicalVolume* logicQ3Quad = CreateQuadMagnets(Q3BGradient,Q4Leng,Q4Radius,Q3Pos,Q3Rot);
    logicQ3Quad->SetVisAttributes(QuadVisAttr);
    logicQ3Quad->SetName("logicQ3Quad");
    new G4PVPlacement(Q3Rot,Q3Pos,logicQ3Quad,"Q3Quad",fMotherVolume,false,0,true);
    printf("Placing Q3 quadrupole with gradient %f \n",Q3BGradient*m/tesla);
    //  }
//////////////////////////////////////////////////////////////////////////
//// Flag 5 just before the target M. Raggi 12/03/2021
/////////////////////////////////////////////////////////////////////////
////  In between the Q4 and Q3 quads           DHSTB002---Q4----FL5-----Q3
    G4VisAttributes FlagVisAttr   = G4VisAttributes(G4Color::Yellow()); // Beam Flags
    G4double      Flag5FrontPosX = beJunMgPosX+379*sin(magnetAngle)*mm+Q3Q4Dist/2*sin(magnetAngle);  
    G4double      Flag5FrontPosY = beJunMgPosY;  
    G4double      Flag5FrontPosZ = beJunMgPosZ-379*cos(magnetAngle)*mm-Q3Q4Dist/2*cos(magnetAngle);  

    G4ThreeVector Flag5FrontPos = G4ThreeVector(Flag5FrontPosX,Flag5FrontPosY,Flag5FrontPosZ);
    G4Tubs* solidBeamFlag5 = new G4Tubs("solidBeamFlag5",0.,Q4Radius-0.2*mm,0.1*mm,0.*deg,360.*deg);

    G4LogicalVolume* logicalBeamFlag5 = new G4LogicalVolume(solidBeamFlag5,G4Material::GetMaterial("Vacuum"),"logicalBeamFlag5",0,0,0);
    logicalBeamFlag5->SetVisAttributes(FlagVisAttr);

    G4RotationMatrix* Flag5Rot = new G4RotationMatrix;  
    Flag5Rot->rotateY(magnetAngle);
    //
    new G4PVPlacement(Flag5Rot,Flag5FrontPos,logicalBeamFlag5,"BeamLineBeamFlag5",fMotherVolume,false,0,true);    
    logicalBeamFlag5->SetSensitiveDetector(beamFlagSD);
    printf("Placing Flag5 at Position %f %f %f \n",Flag5FrontPosX,Flag5FrontPosY,Flag5FrontPosZ);
    
    //    if ( geo->QuadrupolesAreEnabled() ) {
      // Q2 quadrupole second one in LINAC side
    G4double OffsetwrtWallPipe = 440*mm; // Checked on drawings M. Raggi 02/03/2021
    G4double Q2PosX   = InWallPipePosX + (InWallPipeLen*0.5+OffsetwrtWallPipe)*sin(magnetAngle); 
    G4double Q2PosY   = InWallPipePosY; //Q3PosY; 
    G4double Q2PosZ   = InWallPipePosZ - (InWallPipeLen*0.5+OffsetwrtWallPipe)*cos(magnetAngle); 
    
    G4ThreeVector Q2Pos = G4ThreeVector(Q2PosX,Q2PosY,Q2PosZ);
    G4RotationMatrix* Q2Rot = Q4Rot; //da veirficare
    // Positive gradient focus on Y if you don't rotate it for negative particles
    // https://en.wikipedia.org/wiki/Quadrupole_magnet
    // We will use different signs of K to change from X to Y focusing wihout rotating
    // 45 deg rotation is not needed because it's only for the coils but the magnetic field is already ok 
    
    G4LogicalVolume* logicQ2Quad = CreateQuadMagnets(-Q2BGradient,Q4Leng,Q4Radius,Q2Pos,Q2Rot);
    logicQ2Quad->SetVisAttributes(QuadVisAttr);
    logicQ2Quad->SetName("logicQ2Quad");
    new G4PVPlacement(Q2Rot,Q2Pos,logicQ2Quad,"Q2Quad",fMotherVolume,false,0,true);
    printf("Placing Q2 quadrupole with gradient %f \n",-Q2BGradient*m/tesla);
    //    }
    
    G4double Q1Q2Dist = geo->GetQ1Q2Dist();
    ////////////////////////////////////////////////////////////////////////
    // Flag 6 
    ///////////////////////////////////////////////////////////////////////
    ////  In between the Q1 and Q2 quads           MyW---Q1----FL6-----Q2
    
    G4double Flag6FrontPosX   = Q2PosX+Q1Q2Dist/2*sin(magnetAngle); 
    G4double Flag6FrontPosY   = Q2PosY; 
    G4double Flag6FrontPosZ   = Q2PosZ-Q1Q2Dist/2*cos(magnetAngle);  
    G4ThreeVector Flag6FrontPos = G4ThreeVector(Flag6FrontPosX,Flag6FrontPosY,Flag6FrontPosZ);    
    
    G4Tubs* solidBeamFlag6 = new G4Tubs("solidBeamFlag6",0.,Q4Radius-0.1*mm,0.1*mm,0.*deg,360.*deg);
    G4LogicalVolume* logicalBeamFlag6 = new G4LogicalVolume(solidBeamFlag6,G4Material::GetMaterial("Vacuum"),"logicalBeamFlag6",0,0,0);
    logicalBeamFlag6->SetVisAttributes(FlagVisAttr);
    
    printf("Registering Flag6 %d\n",geo->BeamFlagIsEnabled());
    if (geo->BeamFlagIsEnabled() ) {
      new G4PVPlacement(Q2Rot,Flag6FrontPos,logicalBeamFlag6,"BeamLineBeamFlag6",fMotherVolume,false,0,true);    
      logicalBeamFlag6->SetSensitiveDetector(beamFlagSD);
    }
   
    //Q1 quadrupole fist of the 2020 beam line
    G4double Q1PosX   = Q2PosX+Q1Q2Dist*sin(magnetAngle); 
    G4double Q1PosY   = Q2PosY; 
    G4double Q1PosZ   = Q2PosZ-Q1Q2Dist*cos(magnetAngle);  
 
    G4ThreeVector Q1Pos = G4ThreeVector(Q1PosX,Q1PosY,Q1PosZ);
    G4RotationMatrix* Q1Rot = Q2Rot; //da veirficare

    G4RotationMatrix* Q0Rot= new G4RotationMatrix; //da veirficare
    Q0Rot->rotateY(0.);
    
    G4LogicalVolume* logicQ1Quad = CreateQuadMagnets(Q1BGradient,Q4Leng,Q4Radius,Q1Pos,Q1Rot);
    logicQ1Quad->SetVisAttributes(QuadVisAttr);
    logicQ1Quad->SetName("logicQ1Quad");
    new G4PVPlacement(Q1Rot,Q1Pos,logicQ1Quad,"Q1Quad",fMotherVolume,false,0,true);
    printf("Placing Q1 quadrupole with gradient %f \n",Q1BGradient*m/tesla);

    ////////////////////////////////////////////////////////////////////////
    // Flag 7
    ///////////////////////////////////////////////////////////////////////
    ////  In between the Mylar and Q1 quads   MyW--Fl7--Q1----FL6-----Q2
    ///   Moved after the SLTB4 to monitor beam energy spread M. Raggi 18/01/2022
    
//    G4double Flag7FrontPosX   = Q2PosX+(Q1Q2Dist+Q4Leng*1.1)*sin(magnetAngle); 
//    G4double Flag7FrontPosY   = Q2PosY; 
//    G4double Flag7FrontPosZ   = Q2PosZ-(Q1Q2Dist+Q4Leng*1.1)*cos(magnetAngle);  

    G4double Flag7FrontPosX = MylarWinFlgPosX-(SLTB4ToMylar+SLTBThickness*1.5*mm+2*mm)*sin(magnetAngle);
    G4double Flag7FrontPosY = MylarWinFlgPosY;							 
    G4double Flag7FrontPosZ = MylarWinFlgPosZ+(SLTB4ToMylar+SLTBThickness*1.5*mm+2*mm)*cos(magnetAngle);
    
    G4ThreeVector Flag7FrontPos = G4ThreeVector(Flag7FrontPosX,Flag7FrontPosY,Flag7FrontPosZ);    
  
    G4Tubs* solidBeamFlag7 = new G4Tubs("solidBeamFlag7",0.,Q4Radius-0.1*mm,0.1*mm,0.*deg,360.*deg);
    G4LogicalVolume* logicalBeamFlag7 = new G4LogicalVolume(solidBeamFlag7,G4Material::GetMaterial("Vacuum"),"logicalBeamFlag7",0,0,0);
    logicalBeamFlag7->SetVisAttributes(FlagVisAttr);
    
    printf("Registering Flag7 %d at %f %f %f\n",geo->BeamFlagIsEnabled(),Flag7FrontPosX,Flag7FrontPosY,Flag7FrontPosZ);
    new G4PVPlacement(Q2Rot,Flag7FrontPos,logicalBeamFlag7,"BeamLineBeamFlag7",fMotherVolume,false,0,true);    
    logicalBeamFlag7->SetSensitiveDetector(beamFlagSD);
  }

  //create the connection pipe to DHSTB001 Raggi 02/02/2022
  G4double DHSTB001_PipeLen  = 1040*mm-geo->GetMagPipeStraightLength()*mm;
  G4double DHSTB001_PipeRIn  = geo->Get2020PipeInnerRadius();
  G4double DHSTB001_PipeROut = geo->Get2020PipeOuterRadius();
  G4double strFlangeThick = geo->GetMagPipeFlangeThick();

  G4Tubs* solidDHSTB001_PipeFull1 = new G4Tubs("solidDHSTB_001PipeFull1",0.,DHSTB001_PipeROut,0.5*DHSTB001_PipeLen,0.*deg,360.*deg);
  // Carve hole inside pipe (add usual small tolerance)
  G4Tubs* solidDHSTB001_PipeHole = new G4Tubs("solidDHSTB001_PipeHole",0.,DHSTB001_PipeRIn,0.5*DHSTB001_PipeLen+10.*um,0.*deg,360.*deg);
  G4SubtractionSolid* solidDHSTB001_Pipe = new G4SubtractionSolid("solidDHSTB001_Pipe",solidDHSTB001_PipeFull1,solidDHSTB001_PipeHole,0,G4ThreeVector(0.,0.,0.));

  // Create DHSTB001_ Pipe logical volume
  G4LogicalVolume* logicalDHSTB001_Pipe = new G4LogicalVolume(solidDHSTB001_Pipe,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalDHSTB001_Pipe",0,0,0);
  logicalDHSTB001_Pipe->SetVisAttributes(steelVisAttr);

  // Position long pipe in between DHSTB002 and BTF wall
  G4ThreeVector DHSTB001_PipePos = G4ThreeVector(7232.1*mm+(DHSTB001_PipeLen/2+strFlangeThick)*mm*sin(magnetAngle),0.,-9580.76*mm-(DHSTB001_PipeLen/2+strFlangeThick)*mm*cos(magnetAngle));
  //  G4ThreeVector DHSTB001_PipePos = G4ThreeVector(DHSTB001_PipePosX,DHSTB001_PipePosY,DHSTB001_PipePosZ);
  G4RotationMatrix* DHSTB001_PipeRot = new G4RotationMatrix;
  DHSTB001_PipeRot->rotateY(magnetAngle);
  new G4PVPlacement(DHSTB001_PipeRot,DHSTB001_PipePos,logicalDHSTB001_Pipe,"BeamLineDHSTB001_Pipe",fMotherVolume,false,0,true);
  G4ThreeVector DHSTB001_PipeEnd = G4ThreeVector(7232.1*mm+(DHSTB001_PipeLen+strFlangeThick)*mm*sin(magnetAngle),0.,-9580.76*mm-(DHSTB001_PipeLen+strFlangeThick)*mm*cos(magnetAngle));
  printf("End of DHSTB002 linac side pipe 2020 line at  %f %f %f\n",DHSTB001_PipeEnd.x(),DHSTB001_PipeEnd.y(),DHSTB001_PipeEnd.z());
} //END OF 2020 BeamLine


// *********Beam Line 2022******************
// written by M. Raggi 02.2022 
// *****************************************xs 
void BeamLineStructure::CreateBeamLine2022()
{
  G4VisAttributes steelVisAttr   = G4VisAttributes(G4Color::Grey()); // Dark gray
  G4VisAttributes FlagVisAttr   = G4VisAttributes(G4Color::Yellow()); // Beam Flags
  G4VisAttributes DentroilMuroVisAttr   = G4VisAttributes(G4Color::Red()); // Beam Flags
  
  if ( ! fBeamLineIsVisible ) steelVisAttr = G4VisAttributes::Invisible;
  printf("****************************************** \n");
  printf("Creating a beamline in 2022 RunIII setup   \n");
  printf("****************************************** \n");

} //END OF 2022 BeamLine



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
} // end of BeW creator

void BeamLineStructure::CreateDHSTB002Magnet()
{
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
  G4Tubs* solidHole1 = new G4Tubs("solidDHSTB002Hole1",hole1MinR,hole1MaxR,0.5*hole1SizeY,0.*deg-0.01*deg,yokeAngle+0.02*deg);

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
  G4ThreeVector strSidePos = G4ThreeVector(strSidePosX,0.,strSidePosZ);
  new G4PVPlacement(strSideRot,strSidePos,logicalStraightPipe,"DHSTB002FlangeSide",fMotherVolume,false,0,true);

  G4double FlagR = geo->GetBeWindowRadius()-0.2*mm; //Use the same of the BeW
  G4double FlagT = geo->GetBeWindowThick();  //Use the same of the BeW

  ////////////////////////////////////////////////////////////////////////
  // Beam Flag1 to monitor beam in different locations M. Raggi 29/08/2019
  // Entrance of DHSTB002 Magnet from linac side
  ///////////////////////////////////////////////////////////////////////

  G4double      FlagBackPosX = mpEntPosX;
  G4double      FlagBackPosY = mpEntPosY;
  G4double      FlagBackPosZ = mpEntPosZ-2*mm;
  G4ThreeVector FlagBackPos  = G4ThreeVector(FlagBackPosX,FlagBackPosY,FlagBackPosZ);

  G4Tubs* solidBeamFlag1 = new G4Tubs("solidBeamFlag1",0.,geo->GetBeJunctionRIn()-4*mm,0.5*FlagT,0.*deg,360.*deg);
  G4LogicalVolume* logicalBeamFlag1 = new G4LogicalVolume(solidBeamFlag1,G4Material::GetMaterial("Vacuum"),"logicalBeamFlag1",0,0,0);
  logicalBeamFlag1->SetVisAttributes(FlagVisAttr);

  ////////////////////////////////////////////////////////////////////////
  // Beam Flag to monitor beam in different locations M. Raggi 29/08/2019
  // Flag 2 after the DHSTB002 output pipe
  ///////////////////////////////////////////////////////////////////////
  
  G4double     FlagFrontPosX = geo->GetMagPipeStraightFrontPosX()               +2*mm*sin(strFrontRotY);
  G4double     FlagFrontPosZ = geo->GetMagPipeStraightFrontPosZ()+strPipeSizeZ/2+2*mm*cos(strFrontRotY);
  G4ThreeVector FlagFrontPos = G4ThreeVector(FlagFrontPosX,0.,FlagFrontPosZ);
  
  G4Tubs* solidBeamFlag2 = new G4Tubs("solidBeamFlag2",0.,FlagR-4*mm,0.5*FlagT,0.*deg,360.*deg);
  G4LogicalVolume* logicalBeamFlag2 = new G4LogicalVolume(solidBeamFlag2,G4Material::GetMaterial("Vacuum"),"logicalBeamFlag2",0,0,0);
  logicalBeamFlag2->SetVisAttributes(FlagVisAttr);
  
  if (geo->BeamFlagIsEnabled() ) {
    //   printf("Registering Flag2 %b\n",geo->BeamFlagIsEnabled());
    new G4PVPlacement(strFrontRot,FlagFrontPos,logicalBeamFlag2,"BeamLineBeamFlag2",fMotherVolume,false,0,true);    
    BeamFlag2SDName = geo->GetBeamFlag2SensitiveDetectorName();
    printf("Registering Flag2 SD %s\n",BeamFlag2SDName.data());
    logicalBeamFlag2->SetSensitiveDetector(beamFlagSD);
    G4SDManager::GetSDMpointer()->AddNewDetector(beamFlagSD);

    G4RotationMatrix* Flag1Rot = new G4RotationMatrix;  
    Flag1Rot->rotateY(magnetAngle);
    new G4PVPlacement(Flag1Rot,FlagBackPos,logicalBeamFlag1,"BeamLineBeamFlag1",fMotherVolume,false,0,true);    
    logicalBeamFlag1->SetSensitiveDetector(beamFlagSD);
    G4SDManager::GetSDMpointer()->AddNewDetector(beamFlagSD);
  }

  ////////////////////////////////////////////////////////////////////////
  // Beam Flag to monitor beam in different locations M. Raggi 29/08/2019
  // Flag 3 exit of DHSTB002 into large 100mm cross region No Rotation needed
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
    printf("Registering Flag3 %d\n",geo->BeamFlagIsEnabled());
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

//  ////////////////////////////////////////////////////////////////////////
//  // Flag 5 just before the target M. Raggi 23/09/2019
//  ///////////////////////////////////////////////////////////////////////
//
//  G4double      Flag5FrontPosX = 0.;
//  G4double      Flag5FrontPosZ = -490.0*mm-Flag3T*0.5;
//  G4ThreeVector Flag5FrontPos = G4ThreeVector(Flag5FrontPosX,0.,Flag5FrontPosZ);
//// in between the Q4 and Q3 quads            Q3----FL5-----Q4
//
//  G4Tubs* solidBeamFlag5 = new G4Tubs("solidBeamFlag5",0.,Flag3R,3*mm,0.*deg,360.*deg);
//  G4LogicalVolume* logicalBeamFlag5 = new G4LogicalVolume(solidBeamFlag5,G4Material::GetMaterial("Vacuum"),"logicalBeamFlag5",0,0,0);
//  logicalBeamFlag5->SetVisAttributes(FlagVisAttr);
//  if ( geo->BeamFlagIsEnabled() ) {
//    new G4PVPlacement(strFrontRot,Flag5FrontPos,logicalBeamFlag5,"BeamLineBeamFlag5",fMotherVolume,false,0,true);    
//    logicalBeamFlag5->SetSensitiveDetector(beamFlagSD);
//  }
//  ////////////////////////////////////////////////////////////////////////
//  // Flag 6 just before TimePix M. Raggi 30/09/2019
//  ///////////////////////////////////////////////////////////////////////
//
//  G4double      Flag6FrontPosX = 746.1; //from TPix positioning
//  G4double      Flag6FrontPosZ = 2250.6*mm-Flag3T*0.5; //from TPix positioning
//  G4ThreeVector Flag6FrontPos = G4ThreeVector(Flag6FrontPosX,0.,Flag6FrontPosZ);
//
//  G4RotationMatrix* rotTPix = new G4RotationMatrix;
//  rotTPix->rotateY(-0.314159); //from chamber geometry
//
//  G4Box* solidBeamFlag6 =  new G4Box("solidBeamFlag6",15*cm,10*cm,0.5*1*mm);
//  G4LogicalVolume* logicalBeamFlag6 = new G4LogicalVolume(solidBeamFlag6,G4Material::GetMaterial("Vacuum"),"logicalBeamFlag6",0,0,0);
//  logicalBeamFlag6->SetVisAttributes(FlagVisAttr);
//
//  printf("Registering Flag6 %d\n",geo->BeamFlagIsEnabled());
//  if (geo->BeamFlagIsEnabled() ) {
//    new G4PVPlacement(rotTPix,Flag6FrontPos,logicalBeamFlag6,"BeamLineBeamFlag6",fMotherVolume,false,0,true);    
//    logicalBeamFlag6->SetSensitiveDetector(beamFlagSD);
//  }

}// END of DHSTB002 magnet creation

// Create DHSTB001 magnet and SLTB2
void BeamLineStructure::CreateDHSTB001Magnet()
{
  G4VisAttributes steelVisAttr   = G4VisAttributes(G4Color::Grey()); // Dark gray
  G4VisAttributes CopperVisAttr  = G4VisAttributes(G4Color::Magenta()); //Magenta
  G4VisAttributes FlagVisAttr    = G4VisAttributes(G4Color::Yellow()); //Beam Flags
  G4VisAttributes DHSTB001VisAtt = G4VisAttributes(G4Colour::Red());
  G4VisAttributes MagFieldVisAtt = G4VisAttributes(G4Colour::Blue());

  if ( ! fBeamLineIsVisible ) {
    steelVisAttr   = G4VisAttributes::Invisible;
    DHSTB001VisAtt = G4VisAttributes::Invisible;
  }
// Create the straight section of the beam pipe (with its flange)
// and position it at entrance and exits of magnet section of the beam line

  // Angular span of the DHSTB001 identical to DHSTB002 magnet (45 deg)
  G4double magnetAngle = geo->GetDHSTB002AngularSpan();

  G4double strHoleSizeX = geo->GetMagPipeHoleSizeX();
  G4double strHoleSizeY = geo->GetMagPipeHoleSizeY();
  G4double strHoleSizeZ = geo->GetMagPipeStraightLength()+10.*um;
  G4double strFlangeR   = geo->GetMagPipeFlangeRadius();
  G4double strFlangeThick = geo->GetMagPipeFlangeThick();

  G4double strPipeSizeX = geo->GetMagPipeSizeX();
  G4double strPipeSizeY = geo->GetMagPipeSizeY();
  G4double strPipeSizeZ = geo->GetMagPipeStraightLength();
 
  G4Box* solidStrFull = new G4Box("solidStrFull",0.5*strPipeSizeX,0.5*strPipeSizeY,0.5*strPipeSizeZ);
  G4Box* solidStrHole = new G4Box("solidStrHole",0.5*strHoleSizeX,0.5*strHoleSizeY,0.5*strHoleSizeZ);
  G4Tubs* solidStrFlange = new G4Tubs("solidStrFlange",0.,strFlangeR,0.5*strFlangeThick,0.*deg,360.*deg);
  
  G4ThreeVector posFlange = G4ThreeVector(0.,0.,geo->GetMagPipeFlangePosZ());
  G4UnionSolid* solidStrFullFlange = new G4UnionSolid("solidStrFullFlange",solidStrFull,solidStrFlange,0,posFlange);
  G4SubtractionSolid* solidStraightPipe = new G4SubtractionSolid("solidStraightPipe",solidStrFullFlange,solidStrHole,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicalStraightPipe = new G4LogicalVolume(solidStraightPipe,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalStraightPipe",0,0,0);
  logicalStraightPipe->SetVisAttributes(steelVisAttr);

  //  G4ThreeVector strFrontPos = G4ThreeVector(7232.1*mm+strPipeSizeZ*mm*sin(magnetAngle),0.,-9580.76*mm-(strPipeSizeZ)*mm*cos(magnetAngle));
  G4ThreeVector strFrontPos=G4ThreeVector(7833.494317,0,-10182.154317); // position of the DHSTB002 flange PADME Side
  strFrontPos = G4ThreeVector(strFrontPos.x()+strPipeSizeZ/2*mm*sin(magnetAngle),0,strFrontPos.z()-strPipeSizeZ/2*mm*cos(magnetAngle));
  G4RotationMatrix* WallPipeRot = new G4RotationMatrix;
  WallPipeRot->rotateY(magnetAngle);
  new G4PVPlacement(WallPipeRot,strFrontPos,logicalStraightPipe,"DHSTB001FlangeFront",fMotherVolume,false,0,true);

  //*******************************************
  //         Start Creating Magnet
  //*******************************************

  // Create magnet yoke
  G4double yokeMinR = geo->GetDHSTB002MinRadius();
  G4double yokeMaxR = geo->GetDHSTB002MaxRadius();
  G4double yokeSizeY = geo->GetDHSTB002SizeY();
  G4double yokeAngle = geo->GetDHSTB002AngularSpan();
  G4Tubs* solidDHSTB001Iron = new G4Tubs("solidDHSTB001Iron",yokeMinR,yokeMaxR,0.5*yokeSizeY,0.*deg,yokeAngle);

  // Create three parts of H-shaped internal hole
  G4double holeCenterR = geo->GetDHSTB002CenterRadius();
  G4double holeL1 = geo->GetDHSTB002L1();
  G4double holeL2 = geo->GetDHSTB002L2();
  G4double holeL3 = geo->GetDHSTB002L3();
  G4double holeL4 = geo->GetDHSTB002L4();

  G4double hole1MinR = holeCenterR-0.5*holeL1;
  G4double hole1MaxR = holeCenterR-0.5*holeL2;
  G4double hole1SizeY = holeL3;
  G4Tubs* solidHole1 = new G4Tubs("solidDHSTB001Hole1",hole1MinR,hole1MaxR,0.5*hole1SizeY,0.*deg-0.01*deg,yokeAngle+0.02*deg);

  G4double hole2MinR = holeCenterR-0.5*holeL2-100.*um; // Expand a bit to avoid graphic problems
  G4double hole2MaxR = holeCenterR+0.5*holeL2+100.*um;
  G4double hole2SizeY = holeL4;
  G4Tubs* solidHole2 = new G4Tubs("solidDHSTB001Hole2",hole2MinR,hole2MaxR,0.5*hole2SizeY,0.*deg-0.01*deg,yokeAngle+0.02*deg);

  G4double hole3MinR = holeCenterR+0.5*holeL2;
  G4double hole3MaxR = holeCenterR+0.5*holeL1;
  G4double hole3SizeY = holeL3;
  G4Tubs* solidHole3 = new G4Tubs("solidDHSTB001Hole3",hole3MinR,hole3MaxR,0.5*hole3SizeY,0.*deg-0.01*deg,yokeAngle+0.02*deg);
  G4SubtractionSolid* solidDHSTB001_1 = new G4SubtractionSolid("solidDHSTB001_1",solidDHSTB001Iron,solidHole1,0,G4ThreeVector(0.,0.,0.));
  G4SubtractionSolid* solidDHSTB001_2 = new G4SubtractionSolid("solidDHSTB001_2",solidDHSTB001_1,solidHole2,0,G4ThreeVector(0.,0.,0.));
  G4SubtractionSolid* solidDHSTB001_3 = new G4SubtractionSolid("solidDHSTB001_3",solidDHSTB001_2,solidHole3,0,G4ThreeVector(0.,0.,0.));

  // Carve rectangular hole into yoke for straight section of the beam pipe
  G4double magBPSizeX = geo->GetMagPipeMaxRadius()-geo->GetMagPipeMinRadius()+1.*mm;
  G4double magBPSizeY = geo->GetMagPipeLineLength();
  G4double magBPSizeZ = geo->GetMagPipeSizeY()+1.*mm;

  G4Box* solidPipeHole = new G4Box("solidMagPipeHole",0.5*magBPSizeX,0.5*magBPSizeY,0.5*magBPSizeZ);
  G4ThreeVector magBPPos = G4ThreeVector(geo->GetDHSTB002CenterRadius(),0.5*magBPSizeY,0.);
  G4SubtractionSolid* solidDHSTB001 = new G4SubtractionSolid("solidDHSTB001",solidDHSTB001_3,solidPipeHole,0,magBPPos);
  //G4cout << "BeamPipe Straight hole " << magBPSizeX << " " << magBPSizeY << " " << magBPSizeZ << " " << magBPPos << G4endl;

  // Create DHSTB001 magnet logical volume
  G4LogicalVolume* logicalDHSTB001 = new G4LogicalVolume(solidDHSTB001,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalDHSTB001",0,0,0);
  logicalDHSTB001->SetVisAttributes(DHSTB001VisAtt);

  // Now rotate DHSTB001 to the XZ plane and position it so that its exit is aligned with the PADME Z axis
  G4RotationMatrix* rotDHSTB01 = new G4RotationMatrix;
  rotDHSTB01->rotateX(90.*deg); 
  rotDHSTB01->rotateZ(225.*deg+geo->GetDHSTB002AngularSpan());

  G4ThreeVector EndFrontFlange = G4ThreeVector(strFrontPos.x()*mm+strPipeSizeZ/2*mm*sin(magnetAngle),0.,strFrontPos.z()*mm-strPipeSizeZ/2*mm*cos(magnetAngle));
  G4ThreeVector posDHSTB01 = G4ThreeVector(EndFrontFlange.x()+holeCenterR*mm*sin(magnetAngle),0.,EndFrontFlange.z()+holeCenterR*mm*cos(magnetAngle));
  new G4PVPlacement(rotDHSTB01,posDHSTB01,logicalDHSTB001,"DHSTB001",fMotherVolume,false,0,true);

  // Create the magnetic volume
  G4double magvolSizeY = geo->GetMagVolSizeY();
  G4double magvolMinR = geo->GetMagVolMinRadius();
  G4double magvolMaxR = geo->GetMagVolMaxRadius();
  G4Tubs* solidMagVol = new G4Tubs("solidMagVol",magvolMinR,magvolMaxR,0.5*magvolSizeY,0.*deg,magnetAngle);
  G4LogicalVolume* logicalMagVol = new G4LogicalVolume(solidMagVol,G4Material::GetMaterial("Vacuum"),"logicalMagVol",0,0,0);
  logicalMagVol->SetVisAttributes(G4VisAttributes::Invisible);
  //  logicalMagVol->SetVisAttributes(MagFieldVisAtt);

  // Create another volume externally adjacent to magnetic volume: used for cuts
  G4double extvolSizeY = geo->GetMagVolSizeY();
  G4double extvolMinR = geo->GetMagVolMaxRadius();
  G4double extvolMaxR = geo->GetMagVolMaxRadius()+1.*m; // Arbitrary as long as it is large enough
  G4Tubs* solidExtVol = new G4Tubs("solidExtVol",extvolMinR,extvolMaxR,0.5*extvolSizeY,0.*deg,magnetAngle);

  // Add a constant magnetic field to the magnetic volume
  G4ThreeVector fieldV(0.,geo->GetDHSTB001MagneticFieldY(),0.);
  G4MagneticField* magField = new G4UniformMagField(fieldV);
  G4FieldManager* localFieldManager = new G4FieldManager(magField);
  logicalMagVol->SetFieldManager(localFieldManager,true);

  // Position the magnetic volume at center of H-shaped hole of DHSTB002
  G4RotationMatrix* rotMagVol = new G4RotationMatrix;
  G4ThreeVector posMagVol = posDHSTB01;//check it it's just a try MR
  new G4PVPlacement(rotDHSTB01,posMagVol,logicalMagVol,"DHSTB001MagneticVolume",fMotherVolume,false,0,true);
  //
  // *****************END OF MAGNET CREATION

  G4double magBPBSizeY = geo->GetMagPipeSizeY();
  G4double magBPBMinR = geo->GetMagPipeMinRadius();
  G4double magBPBMaxR = geo->GetMagPipeMaxRadius();
  G4Tubs* solidBeamPipeBendFull = new G4Tubs("solidBeamPipeBendFull",magBPBMinR,magBPBMaxR,0.5*magBPBSizeY,0.*deg,magnetAngle);
  
// Linear pipe - Full
  G4double magBPLSizeX = geo->GetMagPipeMaxRadius()-geo->GetMagPipeMinRadius();
  G4double magBPLSizeY = geo->GetMagPipeLineLength();
  G4double magBPLSizeZ = geo->GetMagPipeSizeY(); 
// Complete beam pipe - Full
  G4ThreeVector magBPLPos = G4ThreeVector(geo->GetDHSTB002CenterRadius(),0.5*magBPLSizeY,0.);
  
// Bended pipe - Hole
  G4double magBPBHSizeY = geo->GetMagPipeHoleSizeY();
  G4double magBPBHMinR = geo->GetMagPipeHoleMinRadius();
  G4double magBPBHMaxR = geo->GetMagPipeHoleMaxRadius();
  G4Tubs* solidBeamPipeBendHole  = new G4Tubs("solidBeamPipeBendHole",magBPBHMinR,magBPBHMaxR,0.5*magBPBHSizeY,0.*deg,magnetAngle);
  
// Complete beam pipe (remove hole from full pipe)
  G4SubtractionSolid* solidBeamPipe = new G4SubtractionSolid("solidBeamPipe",solidBeamPipeBendFull,solidBeamPipeBendHole,0,G4ThreeVector(0.,0.,0.));  
  G4LogicalVolume* logicalBeamPipe = new G4LogicalVolume(solidBeamPipe,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalBeamPipe",0,0,0);
  logicalBeamPipe->SetVisAttributes(steelVisAttr);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicalBeamPipe,"DHSTB001BeamPipe",logicalMagVol,false,0,true);

  //Positioning of the straigth back section and it's flange
  G4double strBackPosX = EndFrontFlange.x()+holeCenterR*sin(yokeAngle)+0.5*strPipeSizeZ*mm;
  G4double strBackPosZ = EndFrontFlange.z()-holeCenterR*(1-cos(yokeAngle));

  G4ThreeVector strBackPos = G4ThreeVector(strBackPosX,0.,strBackPosZ);
  G4RotationMatrix* strBackRot = new G4RotationMatrix;
  strBackRot->rotateY(-90*deg);
  new G4PVPlacement(strBackRot,strBackPos,logicalStraightPipe,"DHSTB001FlangeBack",fMotherVolume,false,0,true);
  printf("DHSTB001FlangeBack exit: X=  %f Z= %f\n",strBackPosX+0.5*strPipeSizeZ,strBackPosZ);

  //***********************************************************
  // START POSITIONING SLTB2           MR 01/03/2022
  //***********************************************************
  //  G4double SLTB2ToMylar = 519.24*mm;
  G4double SLTBThickness = 35.0*mm;
  G4double SLTB2Aperture = geo->GetSLTB2Aperture();  
  G4double WallPipeRIn  = geo->Get2020PipeInnerRadius();
  G4double SLTB2PosX = strBackPosX+0.5*mm*strPipeSizeZ+SLTBThickness*0.5*mm+strFlangeThick*mm+100*mm; 
  G4double SLTB2PosY = 0; //MylarWinFlgPosY;							 
  G4double SLTB2PosZ = strBackPosZ;//-40*mm*(1-cos(yokeAngle))+SLTBThickness*0.5*(1-cos(yokeAngle));
  G4ThreeVector SLTB2Pos = G4ThreeVector(SLTB2PosX,SLTB2PosY,SLTB2PosZ);    
  
  G4Tubs* solidSLTB2Full = new G4Tubs("solidSLTB2Full",0.,WallPipeRIn-0.1*mm,SLTBThickness,0.*deg,360.*deg);
  G4VSolid* solidSLTB2Hole = new G4Box("solidSLTB2Hole",SLTB2Aperture*0.5,WallPipeRIn-0.05*mm,SLTBThickness+2*mm);
  G4SubtractionSolid* solidSLTB2 = new G4SubtractionSolid("solidSLTB2",solidSLTB2Full,solidSLTB2Hole,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicalSLTB2 = new G4LogicalVolume(solidSLTB2,G4Material::GetMaterial("G4_W"),"logicalSLTB2",0,0,0);
  logicalSLTB2->SetVisAttributes(steelVisAttr);
  
  // positioning supports 
  //  new G4PVPlacement(MylarWinFlgRot,SLTB2Pos,logicalCollSupport,"CollSuport",fMotherVolume,false,0,true); //rotation matrix    
  // positioning collimators
  new G4PVPlacement(strBackRot,SLTB2Pos,logicalSLTB2,"BeamSLTB2",fMotherVolume,false,0,true);


  //***********************************************************
  // START POSITIONING BTF TARGET  MR 31/08/2022
  //***********************************************************
  G4double BTFTargetThickness = geo->GetBTFTargetThickness();  
  G4double BTFTargetDistance = geo->GetBTFTargetDistance();  
  
  G4double BTFTargetPosX = strBackPosX+0.5*mm*strPipeSizeZ+SLTBThickness*0.5*mm+strFlangeThick*mm+100*mm+BTFTargetDistance; 
  G4double BTFTargetPosY = 0; 							 
  G4double BTFTargetPosZ = strBackPosZ;//-40*mm*(1-cos(yokeAngle))+SLTBThickness*0.5*(1-cos(yokeAngle));
  G4ThreeVector BTFTargetPos = G4ThreeVector(BTFTargetPosX,BTFTargetPosY,BTFTargetPosZ);    
  
  G4Tubs* solidBTFTarget = new G4Tubs("solidBTFTarget",0.,WallPipeRIn-0.1*mm,BTFTargetThickness,0.*deg,360.*deg);
  G4LogicalVolume* logicalBTFTarget = new G4LogicalVolume(solidBTFTarget,G4Material::GetMaterial("G4_Cu"),"logicalBTFTarget",0,0,0);
  logicalBTFTarget->SetVisAttributes(CopperVisAttr);
  
  // positioning BTF target
  if(geo->BTFTargetIsEnabled()){
    new G4PVPlacement(strBackRot,BTFTargetPos,logicalBTFTarget,"BeamBTFTarget",fMotherVolume,false,0,true);
    
    
    ////////////////////////////////////////////////////////////////////////
    // Flag 8 just after the BTF target
    ///////////////////////////////////////////////////////////////////////
    
    G4double Flag8PosX = strBackPosX+0.5*mm*strPipeSizeZ+SLTBThickness*0.5*mm+strFlangeThick*mm+BTFTargetDistance; ;
    G4double Flag8PosY = 0;							 
    G4double Flag8PosZ = strBackPosZ;
    
    G4ThreeVector Flag8Pos = G4ThreeVector(Flag8PosX,Flag8PosY,Flag8PosZ);    
    
    G4Tubs* solidBeamFlag8 = new G4Tubs("solidBeamFlag8",0.,WallPipeRIn-0.1*mm,0.1*mm,0.*deg,360.*deg);
    G4LogicalVolume* logicalBeamFlag8 = new G4LogicalVolume(solidBeamFlag8,G4Material::GetMaterial("Vacuum"),"logicalBeamFlag8",0,0,0);
    logicalBeamFlag8->SetVisAttributes(FlagVisAttr);
    
    printf("Registering Flag8 %d\n",geo->BeamFlagIsEnabled());
    new G4PVPlacement(strBackRot,Flag8Pos,logicalBeamFlag8,"BeamLineBeamFlag8",fMotherVolume,false,0,true);    
    logicalBeamFlag8->SetSensitiveDetector(beamFlagSD);  
  }
}// End of Create DHSTB001 


// Create the 2019 beam line configuration revised M. Raggi 03/2021
void BeamLineStructure::CreateBeamLine()
{
  //  BeamLineGeometry* geo = BeamLineGeometry::GetInstance();
  // Create all pipes along the beam line from the Be window to the Target junction

  //  BeamFlagSD* beamFlagSD;
  //  G4String BeamFlag2SDName = geo->GetBeamFlag2SensitiveDetectorName();
  //  beamFlagSD = new BeamFlagSD(BeamFlag2SDName);

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

  if ( geo->QuadrupolesAreEnabled() ) {    
    /////////////////////////////////////////////////////////////
    // Creates the pair of quadrupoles located in BTF before the 
    // entrance of DHSTB002  
    /////////////////////////////////////////////////////////////
    G4VisAttributes QuadVisAttr  = G4VisAttributes(G4Colour::Green());    
    //    G4ThreeVector BOrigin = G4ThreeVector(0.,0.,0.);

    G4double Q4BGradient    = geo->GetQ4MagneticFieldGrad();
    G4RotationMatrix* Q4Rot = new G4RotationMatrix;
    Q4Rot->rotateY(magnetAngle);

    printf("******************QUADRUPOLES*********************** \n");
    printf("Placing Q1 quadrupole with gradient %f \n",Q4BGradient*m/tesla);
    printf("Placing Q2 quadrupole with gradient %f \n",Q4BGradient*m/tesla);
    printf("Placing Q3 quadrupole with gradient %f \n",Q4BGradient*m/tesla);

    G4double Q4Radius       = geo -> Get2020PipeInnerRadius()-0.1*mm; // get the 2019 values
    G4double Q4Leng         = geo -> GetQuadMagSizeZ();             //
 
   // Generating quadrupole Q4
    G4double Q4PosX = beJunMgPosX+379*sin(magnetAngle)*mm; 
    G4double Q4PosY = beJunMgPosY; 
    G4double Q4PosZ = beJunMgPosZ-379*cos(magnetAngle)*mm; ; 
    G4ThreeVector Q4Pos = G4ThreeVector(Q4PosX,Q4PosY,Q4PosZ);

    G4LogicalVolume* logicQ4Quad = CreateQuadMagnets(Q4BGradient,Q4Leng,Q4Radius,Q4Pos,Q4Rot);
    logicQ4Quad->SetVisAttributes(QuadVisAttr);
    logicQ4Quad->SetName("logicQ4Quad");
    new G4PVPlacement(Q4Rot,Q4Pos,logicQ4Quad,"Q4Quad",fMotherVolume,false,0,true);
    printf("Placing Q4 quadrupole with gradient %f \n",Q4BGradient*m/tesla);

    G4double Q3Q4Dist = geo->GetQ3Q4Dist();
////////////////////////////////////////////////////////////////////////
// Flag 5 just before the target M. Raggi 12/03/2021
///////////////////////////////////////////////////////////////////////
//  In between the Q4 and Q3 quads            Q3----FL5-----Q4
//
//    G4VisAttributes FlagVisAttr   = G4VisAttributes(G4Color::Yellow()); // Beam Flags
//
//    G4double      Flag5FrontPosX = beJunMgPosX+379*sin(magnetAngle)*mm+Q3Q4Dist/2*sin(magnetAngle);  
//    G4double      Flag5FrontPosY = beJunMgPosY;  
//    G4double      Flag5FrontPosZ = beJunMgPosZ-379*cos(magnetAngle)*mm-Q3Q4Dist/2*cos(magnetAngle);  
//    
//    G4ThreeVector Flag5FrontPos = G4ThreeVector(Flag5FrontPosX,Flag5FrontPosY,Flag5FrontPosZ);
//    G4Tubs* solidBeamFlag5 = new G4Tubs("solidBeamFlag5",0.,Q4Radius,0.1*mm,0.*deg,360.*deg);
//
//    G4LogicalVolume* logicalBeamFlag5 = new G4LogicalVolume(solidBeamFlag5,G4Material::GetMaterial("Vacuum"),"logicalBeamFlag5",0,0,0);
//    logicalBeamFlag5->SetVisAttributes(FlagVisAttr);
//
//    G4RotationMatrix* Flag5Rot = new G4RotationMatrix;  
//    Flag5Rot->rotateY(magnetAngle);
//
//    new G4PVPlacement(Flag5Rot,Flag5FrontPos,logicalBeamFlag5,"BeamLineBeamFlag5",fMotherVolume,false,0,true);    
    //    logicalBeamFlag5->SetSensitiveDetector(beamFlagSD);
    //    G4SDManager::GetSDMpointer()->AddNewDetector(beamFlagSD);
//    
//    printf("Placing Flag5 at Position %f %f %f \n",Flag5FrontPosX,Flag5FrontPosY,Flag5FrontPosZ);

 
    // Generating quadrupole Q3 
    G4double Q3BGradient    = geo->GetQ3MagneticFieldGrad();
    G4RotationMatrix* Q3Rot = new G4RotationMatrix;
    Q3Rot->rotateY(magnetAngle);
    //   G4double Q3Q4Dist = geo->GetQ3Q4Dist();

    G4double Q3PosX   = beJunMgPosX+379*sin(magnetAngle)*mm+Q3Q4Dist*sin(magnetAngle); 
    G4double Q3PosY   = beJunMgPosY; 
    G4double Q3PosZ   = beJunMgPosZ-379*cos(magnetAngle)*mm-Q3Q4Dist*cos(magnetAngle);  
    G4ThreeVector Q3Pos = G4ThreeVector(Q3PosX,Q3PosY,Q3PosZ);

    G4LogicalVolume* logicQ3Quad = CreateQuadMagnets(-Q3BGradient,Q4Leng,Q4Radius,Q3Pos,Q3Rot);
    logicQ3Quad->SetVisAttributes(QuadVisAttr);
    logicQ3Quad->SetName("logicQ3Quad");
    new G4PVPlacement(Q3Rot,Q3Pos,logicQ3Quad,"Q3Quad",fMotherVolume,false,0,true);
    printf("Placing Q3 quadrupole with gradient %f \n",Q3BGradient*m/tesla);
  }

}//end of 2019 beam line

void BeamLineStructure::CreateWallAndPipe()
{
  G4VisAttributes steelVisAttr   = G4VisAttributes(G4Color::Grey()); // Dark gray
  G4VisAttributes FlagVisAttr   = G4VisAttributes(G4Color::Yellow()); // Beam Flags
  G4VisAttributes DentroilMuroVisAttr   = G4VisAttributes(G4Color::Red()); // Beam Flags

  if ( ! fBeamLineIsVisible ) steelVisAttr = G4VisAttributes::Invisible;

  //Gettining intial positions
  G4double mpEntPosX = geo->GetMagPipeEnterPosX();
  G4double mpEntPosY = geo->GetMagPipeEnterPosY();
  G4double mpEntPosZ = geo->GetMagPipeEnterPosZ();
  G4double beJunLen  = geo->GetBeJunctionLength();
  G4double magnetAngle = geo->GetDHSTB002AngularSpan();

// define the distance in between DHSTB002 magnet and BTF Walls do not touch it
  G4double WallPipeLen  = geo->GetDHSTB002WallDistance();
  G4double WallPipeRIn  = geo->Get2020PipeInnerRadius();
  G4double WallPipeROut = geo->Get2020PipeOuterRadius();

  G4double DHSTB002toWallPipeLen  = 2466*mm;

  // Add flange on back (downstream) side of connection pipe
  G4double FlangeR = geo->GetPulsedPipeFlangeRadius();
  G4double FlThick = geo->GetPulsedPipeFlangeThick();
  G4Tubs* solidPipeFlgBck = new G4Tubs("solidPipeFlgBck",0.,FlangeR,0.5*FlThick,0.*deg,360.*deg); //oggi

  // Create solid pipe From DHSTB002 in the direction of the BTF Wall
  G4Tubs* solidDHSTB002toWallPipeFull1 = new G4Tubs("solidDHSTB002toWallPipeFull1",0.,WallPipeROut,0.5*DHSTB002toWallPipeLen,0.*deg,360.*deg);
  G4UnionSolid* solidDHSTB002toWallPipeFullFl = new G4UnionSolid("solidDHSTB002toWallPipeFullFl",solidDHSTB002toWallPipeFull1,solidPipeFlgBck,0,G4ThreeVector(0.,0.,-0.5*DHSTB002toWallPipeLen+0.5*FlThick)); //oggi

  // Carve hole inside pipe (add usual small tolerance)
  G4Tubs* solidWallPipeHole = new G4Tubs("solidWallPipeHole",0.,WallPipeRIn,0.5*WallPipeLen+10.*mm,0.*deg,360.*deg);
  G4SubtractionSolid* solidDHSTB002toWallPipe = new G4SubtractionSolid("solidDHSTB002toWallPipe",solidDHSTB002toWallPipeFullFl,solidWallPipeHole,0,G4ThreeVector(0.,0.,0.));

  // Create Wall Pipe logical volume
  G4LogicalVolume* logicalDHSTB002toWallPipe = new G4LogicalVolume(solidDHSTB002toWallPipe,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalDHSTB002toWallPipe",0,0,0);
  logicalDHSTB002toWallPipe->SetVisAttributes(steelVisAttr);

  // Position long pipe in between DHSTB002 and BTF wall
  G4double DHSTB002toWallPipePosX = mpEntPosX+(0.5*DHSTB002toWallPipeLen)*sin(magnetAngle);
  G4double DHSTB002toWallPipePosY = mpEntPosY;
  G4double DHSTB002toWallPipePosZ = mpEntPosZ-(0.5*DHSTB002toWallPipeLen)*cos(magnetAngle);
  G4ThreeVector DHSTB002toWallPipePos = G4ThreeVector(DHSTB002toWallPipePosX,DHSTB002toWallPipePosY,DHSTB002toWallPipePosZ);
  G4RotationMatrix* WallPipeRot = new G4RotationMatrix;
  WallPipeRot->rotateY(magnetAngle);
  new G4PVPlacement(WallPipeRot,DHSTB002toWallPipePos,logicalDHSTB002toWallPipe,"BeamLineDHSTB002toWallPipe",fMotherVolume,false,0,true);

  // Create solid pipe From DHSTB002 entrance and BTF Wall
  G4double InWallPipeLen  = geo->GetWallPipeLen();
  G4Tubs* solidInWallPipeFull1 = new G4Tubs("solidInWallPipeFull1",0.,WallPipeROut,0.5*InWallPipeLen,0.*deg,360.*deg);

  // Carve hole inside pipe (add usual small tolerance)
  G4Tubs* solidInWallPipeHole = new G4Tubs("solidInWallPipeHole",0.,WallPipeRIn,0.5*InWallPipeLen+10.*um,0.*deg,360.*deg);
  G4SubtractionSolid* solidInWallPipe = new G4SubtractionSolid("solidInWallPipe",solidInWallPipeFull1,solidInWallPipeHole,0,G4ThreeVector(0.,0.,0.));

  // Create Wall Pipe logical volume
  G4LogicalVolume* logicalInWallPipe = new G4LogicalVolume(solidInWallPipe,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalInWallPipe",0,0,0);
  logicalInWallPipe->SetVisAttributes(DentroilMuroVisAttr);

  // Position long pipe in between DHSTB002 and BTF wall
  G4double InWallPipePosX = mpEntPosX+(WallPipeLen+0.5*InWallPipeLen)*sin(magnetAngle);
  G4double InWallPipePosY = mpEntPosY;
  G4double InWallPipePosZ = mpEntPosZ-(WallPipeLen+0.5*InWallPipeLen)*cos(magnetAngle);
  G4ThreeVector InWallPipePos = G4ThreeVector(InWallPipePosX,InWallPipePosY,InWallPipePosZ);
  new G4PVPlacement(WallPipeRot,InWallPipePos,logicalInWallPipe,"BeamLineInWallPipe",fMotherVolume,false,0,true);

  // Create solid pipe From WALL to Mylar Window Into Linac
  G4double InLinacPipeLen  = geo->GetWallMylarWinDistance();
  G4Tubs* solidInLinacPipeFull1 = new G4Tubs("solidInLinacPipeFull1",0.,WallPipeROut,0.5*InLinacPipeLen,0.*deg,360.*deg);

  // Carve hole inside pipe (add usual small tolerance)
  G4Tubs* solidInLinacPipeHole = new G4Tubs("solidInLinacPipeHole",0.,WallPipeRIn,0.5*InLinacPipeLen+10.*um,0.*deg,360.*deg);
  G4SubtractionSolid* solidInLinacPipe = new G4SubtractionSolid("solidInLinacPipe",solidInLinacPipeFull1,solidInLinacPipeHole,0,G4ThreeVector(0.,0.,0.));

  // Create Wall Pipe logical volume
  G4LogicalVolume* logicalInLinacPipe = new G4LogicalVolume(solidInLinacPipe,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalInLinacPipe",0,0,0);
  //logicalInLinacPipe->SetVisAttributes(steelVisAttr);

  // Position long pipe in between Wall and Mylar window
  G4double InLinacPipePosX = mpEntPosX+(WallPipeLen+InWallPipeLen+0.5*InLinacPipeLen)*sin(magnetAngle);
  G4double InLinacPipePosY = mpEntPosY;
  G4double InLinacPipePosZ = mpEntPosZ-(WallPipeLen+InWallPipeLen+0.5*InLinacPipeLen)*cos(magnetAngle);
  G4ThreeVector InLinacPipePos = G4ThreeVector(InLinacPipePosX,InLinacPipePosY,InLinacPipePosZ);
  new G4PVPlacement(WallPipeRot,InLinacPipePos,logicalInLinacPipe,"BeamLineInLinacPipe",fMotherVolume,false,0,true);

  G4double LinacWallThick  = geo->GetWallThickness();
  G4double WallHoleRadius  = geo->GetWallHoleRadius();

  G4VSolid* solidLinacWallFull = new G4Box("solidLinacWallFull",LinacWallThick*0.5,2*m,2*m);
  G4Tubs* solidLinacWallHole = new G4Tubs("solidLinacWallHole",0.,WallPipeROut*2.5,0.5*LinacWallThick*1.5+100.*um,0.*deg,360.*deg);


  G4SubtractionSolid* solidLinacWall = new G4SubtractionSolid("solidLinacWall",solidLinacWallFull,solidLinacWallHole,WallPipeRot,G4ThreeVector(0,0.,mpEntPosZ-(+LinacWallThick*0.5+293.*cm)-(InWallPipePosZ)));
  G4LogicalVolume* logicLinacWall = new G4LogicalVolume(solidLinacWall,G4Material::GetMaterial("G4_CONCRETE"),"logicLinacWall", 0, 0, 0);
  //  G4ThreeVector LinacWallPos = G4ThreeVector(InWallPipePosX,InWallPipePosY,InWallPipePosZ);
  G4ThreeVector LinacWallPos = G4ThreeVector(293*cm + mpEntPosX+LinacWallThick*0.5,0.,InWallPipePosZ);
  //  new G4PVPlacement(,LinacWallPos,logicLinacWall,"LinacWall",fMotherVolume,false,0,true);
  new G4PVPlacement(0,LinacWallPos,logicLinacWall,"LinacWall",fMotherVolume,false,0,true);

  // Position Mylar window with relative support flange
  G4double MylarWinFlgT    = geo->GetMylarWindowFlangeThick();
  G4double MylarWinFlgPosX = mpEntPosX+(WallPipeLen+InWallPipeLen+InLinacPipeLen+0.5*MylarWinFlgT)*sin(magnetAngle);
  G4double MylarWinFlgPosY = mpEntPosY;
  G4double MylarWinFlgPosZ = mpEntPosZ-(WallPipeLen+InWallPipeLen+InLinacPipeLen+0.5*MylarWinFlgT)*cos(magnetAngle);
  G4ThreeVector MylarWinFlgPos = G4ThreeVector(MylarWinFlgPosX,MylarWinFlgPosY,MylarWinFlgPosZ);
  G4RotationMatrix* MylarWinFlgRot = new G4RotationMatrix;
  MylarWinFlgRot->rotateY(magnetAngle);
  printf("BeamLine - Mylar window exit face center CRASH  is at %f %f\n",MylarWinFlgPosX,MylarWinFlgPosZ);
  printf("BeamLine - DHSTB002 exit %f %f\n",mpEntPosX,mpEntPosZ);
  new G4PVPlacement(MylarWinFlgRot,MylarWinFlgPos,fMylarWindowVolume,"BeamLineMylarWinVolume",fMotherVolume,false,0,true);

  //create the connection pipe to DHSTB001 Raggi 02/02/2022
  G4double DHSTB001_PipeLen  = 1040*mm-geo->GetMagPipeStraightLength()*mm;
  G4double DHSTB001_PipeRIn  = geo->Get2020PipeInnerRadius();
  G4double DHSTB001_PipeROut = geo->Get2020PipeOuterRadius();
  G4double strFlangeThick = geo->GetMagPipeFlangeThick();

  G4Tubs* solidDHSTB001_PipeFull1 = new G4Tubs("solidDHSTB_001PipeFull1",0.,DHSTB001_PipeROut,0.5*DHSTB001_PipeLen,0.*deg,360.*deg);
  // Carve hole inside pipe (add usual small tolerance)
  G4Tubs* solidDHSTB001_PipeHole = new G4Tubs("solidDHSTB001_PipeHole",0.,DHSTB001_PipeRIn,0.5*DHSTB001_PipeLen+10.*um,0.*deg,360.*deg);
  G4SubtractionSolid* solidDHSTB001_Pipe = new G4SubtractionSolid("solidDHSTB001_Pipe",solidDHSTB001_PipeFull1,solidDHSTB001_PipeHole,0,G4ThreeVector(0.,0.,0.));

  // Create DHSTB001_ Pipe logical volume
  G4LogicalVolume* logicalDHSTB001_Pipe = new G4LogicalVolume(solidDHSTB001_Pipe,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalDHSTB001_Pipe",0,0,0);
  logicalDHSTB001_Pipe->SetVisAttributes(steelVisAttr);

  // Position long pipe in between DHSTB002 and BTF wall
  G4ThreeVector DHSTB001_PipePos = G4ThreeVector(7232.1*mm+(DHSTB001_PipeLen/2+strFlangeThick)*mm*sin(magnetAngle),0.,-9580.76*mm-(DHSTB001_PipeLen/2+strFlangeThick)*mm*cos(magnetAngle));
  //  G4ThreeVector DHSTB001_PipePos = G4ThreeVector(DHSTB001_PipePosX,DHSTB001_PipePosY,DHSTB001_PipePosZ);
  G4RotationMatrix* DHSTB001_PipeRot = new G4RotationMatrix;
  DHSTB001_PipeRot->rotateY(magnetAngle);
  new G4PVPlacement(DHSTB001_PipeRot,DHSTB001_PipePos,logicalDHSTB001_Pipe,"BeamLineDHSTB001_Pipe",fMotherVolume,false,0,true);
  G4ThreeVector DHSTB001_PipeEnd = G4ThreeVector(7232.1*mm+(DHSTB001_PipeLen+strFlangeThick)*mm*sin(magnetAngle),0.,-9580.76*mm-(DHSTB001_PipeLen+strFlangeThick)*mm*cos(magnetAngle));
  printf("End of 2020 line at  %f %f %f\n",DHSTB001_PipeEnd.x(),DHSTB001_PipeEnd.y(),DHSTB001_PipeEnd.z());
  
}

void BeamLineStructure::PositionAllQuads()
{
  //***********************************************
  // Setup quadrupoles pairs along the beam line
  //***********************************************
  if(!geo->QuadrupolesAreEnabled()){
    geo -> SetQ1MagneticFieldGrad(0.);
    geo -> SetQ2MagneticFieldGrad(0.);
    geo -> SetQ3MagneticFieldGrad(0.);
    geo -> SetQ4MagneticFieldGrad(0.);
  }


  if ( geo->QuadrupolesAreEnabled() ) {    
    G4VisAttributes QuadVisAttr  = G4VisAttributes(G4Colour::Green());

    G4double mpEntPosX = geo->GetMagPipeEnterPosX();
    G4double mpEntPosY = geo->GetMagPipeEnterPosY();
    G4double mpEntPosZ = geo->GetMagPipeEnterPosZ();
    G4double beJunLen  = geo->GetBeJunctionLength();
    G4double bePipeLen = geo->GetBePipeLength();
    G4double WallPipeLen  = geo->GetDHSTB002WallDistance();
    G4double InWallPipeLen  = geo->GetWallPipeLen();
    
    G4double Q4Radius       = geo -> Get2020PipeInnerRadius()-0.1*mm; // get the 2019 values
    G4double Q4Leng         = geo -> GetQuadMagSizeZ();             // get the 2019 values
    G4RotationMatrix* Q4Rot = new G4RotationMatrix;
    G4double magnetAngle = geo->GetDHSTB002AngularSpan();
    Q4Rot->rotateY(magnetAngle);

    //Gettining intial positions
    G4double InWallPipePosX = mpEntPosX+(WallPipeLen+0.5*InWallPipeLen)*sin(magnetAngle);
    G4double InWallPipePosY = mpEntPosY;
    G4double InWallPipePosZ = mpEntPosZ-(WallPipeLen+0.5*InWallPipeLen)*cos(magnetAngle);
    G4ThreeVector InWallPipePos = G4ThreeVector(InWallPipePosX,InWallPipePosY,InWallPipePosZ);
    
    // Position junction close to the magnet entrance
    G4double beJunMgPosX = mpEntPosX+0.5*beJunLen*sin(magnetAngle);
    G4double beJunMgPosY = mpEntPosY;
    G4double beJunMgPosZ = mpEntPosZ-0.5*beJunLen*cos(magnetAngle);

    G4double Q1BGradient    = geo -> GetQ1MagneticFieldGrad();
    G4double Q2BGradient    = geo -> GetQ2MagneticFieldGrad();
    G4double Q3BGradient    = geo -> GetQ3MagneticFieldGrad();
    G4double Q4BGradient    = geo -> GetQ4MagneticFieldGrad();

    // Setting up Q4 quadrupole near DHSTB002    
    G4double Q4DistFromDHSTB002=geo->GetQ4DistFromDHSTB002();
    G4double Q4PosX = beJunMgPosX+Q4DistFromDHSTB002*sin(magnetAngle); 
    G4double Q4PosY = beJunMgPosY; 
    G4double Q4PosZ = beJunMgPosZ-Q4DistFromDHSTB002*cos(magnetAngle); 
    G4ThreeVector Q4Pos = G4ThreeVector(Q4PosX,Q4PosY,Q4PosZ);

    G4LogicalVolume* logicQ4Quad = CreateQuadMagnets(-Q4BGradient,Q4Leng,Q4Radius,Q4Pos,Q4Rot);
    logicQ4Quad->SetName("logicQ4Quad");
    logicQ4Quad->SetVisAttributes(QuadVisAttr);
    new G4PVPlacement(Q4Rot,Q4Pos,logicQ4Quad,"Q4Quad",fMotherVolume,false,0,true);
    printf("Placing Q4 quadrupole with gradient %f \n",-Q4BGradient*m/tesla);

    // Q3 quadrupole first after BTF wall
    G4double Q3Q4Dist = geo->GetQ3Q4Dist();
    G4double Q3PosX   = Q4PosX+Q3Q4Dist*sin(magnetAngle); 
    G4double Q3PosY   = Q4PosY; 
    G4double Q3PosZ   = Q4PosZ-Q3Q4Dist*cos(magnetAngle);  
    G4ThreeVector Q3Pos = G4ThreeVector(Q3PosX,Q3PosY,Q3PosZ);

    G4RotationMatrix* Q3Rot = new G4RotationMatrix;
    Q3Rot->rotateY(magnetAngle);

    // Negative gradient focus on X
    G4LogicalVolume* logicQ3Quad = CreateQuadMagnets(Q3BGradient,Q4Leng,Q4Radius,Q3Pos,Q3Rot);
    logicQ3Quad->SetVisAttributes(QuadVisAttr);
    logicQ3Quad->SetName("logicQ3Quad");
    new G4PVPlacement(Q3Rot,Q3Pos,logicQ3Quad,"Q3Quad",fMotherVolume,false,0,true);
    printf("Placing Q3 quadrupole with gradient %f \n",Q3BGradient*m/tesla);
    //  }


//////////////////////////////////////////////////////////////////////////
//// Flag 5 just before the target M. Raggi 12/03/2021
/////////////////////////////////////////////////////////////////////////
////  In between the Q4 and Q3 quads           DHSTB002---Q4----FL5-----Q3
    G4VisAttributes FlagVisAttr   = G4VisAttributes(G4Color::Yellow()); // Beam Flags
    G4double      Flag5FrontPosX = beJunMgPosX+379*sin(magnetAngle)*mm+Q3Q4Dist/2*sin(magnetAngle);  
    G4double      Flag5FrontPosY = beJunMgPosY;  
    G4double      Flag5FrontPosZ = beJunMgPosZ-379*cos(magnetAngle)*mm-Q3Q4Dist/2*cos(magnetAngle);  

    G4ThreeVector Flag5FrontPos = G4ThreeVector(Flag5FrontPosX,Flag5FrontPosY,Flag5FrontPosZ);
    G4Tubs* solidBeamFlag5 = new G4Tubs("solidBeamFlag5",0.,Q4Radius-0.2*mm,0.1*mm,0.*deg,360.*deg);

    G4LogicalVolume* logicalBeamFlag5 = new G4LogicalVolume(solidBeamFlag5,G4Material::GetMaterial("Vacuum"),"logicalBeamFlag5",0,0,0);
    logicalBeamFlag5->SetVisAttributes(FlagVisAttr);

    G4RotationMatrix* Flag5Rot = new G4RotationMatrix;  
    Flag5Rot->rotateY(magnetAngle);
    //
    new G4PVPlacement(Flag5Rot,Flag5FrontPos,logicalBeamFlag5,"BeamLineBeamFlag5",fMotherVolume,false,0,true);    
    logicalBeamFlag5->SetSensitiveDetector(beamFlagSD);
    printf("Placing Flag5 at Position %f %f %f \n",Flag5FrontPosX,Flag5FrontPosY,Flag5FrontPosZ);
    
    // Q2 quadrupole second one in LINAC side
    G4double OffsetwrtWallPipe = 440*mm; // Checked on drawings M. Raggi 02/03/2021
    G4double Q2PosX   = InWallPipePosX + (InWallPipeLen*0.5+OffsetwrtWallPipe)*sin(magnetAngle); 
    G4double Q2PosY   = InWallPipePosY; //Q3PosY; 
    G4double Q2PosZ   = InWallPipePosZ - (InWallPipeLen*0.5+OffsetwrtWallPipe)*cos(magnetAngle); 
    
    G4ThreeVector Q2Pos = G4ThreeVector(Q2PosX,Q2PosY,Q2PosZ);
    G4RotationMatrix* Q2Rot = Q4Rot; //da veirficare
    // Positive gradient focus on Y if you don't rotate it for negative particles
    // https://en.wikipedia.org/wiki/Quadrupole_magnet
    // We will use different signs of K to change from X to Y focusing wihout rotating
    // 45 deg rotation is not needed because it's only for the coils but the magnetic field is already ok 
    
    G4LogicalVolume* logicQ2Quad = CreateQuadMagnets(-Q2BGradient,Q4Leng,Q4Radius,Q2Pos,Q2Rot);
    logicQ2Quad->SetVisAttributes(QuadVisAttr);
    logicQ2Quad->SetName("logicQ2Quad");
    new G4PVPlacement(Q2Rot,Q2Pos,logicQ2Quad,"Q2Quad",fMotherVolume,false,0,true);
    printf("Placing Q2 quadrupole with gradient %f \n",-Q2BGradient*m/tesla);
    //    }
    
    G4double Q1Q2Dist = geo->GetQ1Q2Dist();

    ////////////////////////////////////////////////////////////////////////
    // Flag 6 
    ///////////////////////////////////////////////////////////////////////
    ////  In between the Q1 and Q2 quads           MyW---Q1----FL6-----Q2
    
    G4double Flag6FrontPosX   = Q2PosX+Q1Q2Dist/2*sin(magnetAngle); 
    G4double Flag6FrontPosY   = Q2PosY; 
    G4double Flag6FrontPosZ   = Q2PosZ-Q1Q2Dist/2*cos(magnetAngle);  
    G4ThreeVector Flag6FrontPos = G4ThreeVector(Flag6FrontPosX,Flag6FrontPosY,Flag6FrontPosZ);    
    
    G4Tubs* solidBeamFlag6 = new G4Tubs("solidBeamFlag6",0.,Q4Radius-0.1*mm,0.1*mm,0.*deg,360.*deg);
    G4LogicalVolume* logicalBeamFlag6 = new G4LogicalVolume(solidBeamFlag6,G4Material::GetMaterial("Vacuum"),"logicalBeamFlag6",0,0,0);
    logicalBeamFlag6->SetVisAttributes(FlagVisAttr);
    
    printf("Registering Flag6 %d\n",geo->BeamFlagIsEnabled());
    if (geo->BeamFlagIsEnabled() ) {
      new G4PVPlacement(Q2Rot,Flag6FrontPos,logicalBeamFlag6,"BeamLineBeamFlag6",fMotherVolume,false,0,true);    
      logicalBeamFlag6->SetSensitiveDetector(beamFlagSD);
    }
   
    //Q1 quadrupole fist of the 2020 beam line
    G4double Q1PosX   = Q2PosX+Q1Q2Dist*sin(magnetAngle); 
    G4double Q1PosY   = Q2PosY; 
    G4double Q1PosZ   = Q2PosZ-Q1Q2Dist*cos(magnetAngle);  
 
    G4ThreeVector Q1Pos = G4ThreeVector(Q1PosX,Q1PosY,Q1PosZ);
    G4RotationMatrix* Q1Rot = Q2Rot; //da veirficare

    G4RotationMatrix* Q0Rot= new G4RotationMatrix; //da veirficare
    Q0Rot->rotateY(0.);
    
    G4LogicalVolume* logicQ1Quad = CreateQuadMagnets(Q1BGradient,Q4Leng,Q4Radius,Q1Pos,Q1Rot);
    logicQ1Quad->SetVisAttributes(QuadVisAttr);
    logicQ1Quad->SetName("logicQ1Quad");
    new G4PVPlacement(Q1Rot,Q1Pos,logicQ1Quad,"Q1Quad",fMotherVolume,false,0,true);
    printf("Placing Q1 quadrupole with gradient %f \n",Q1BGradient*m/tesla);

    ////////////////////////////////////////////////////////////////////////
    // Flag 7
    ///////////////////////////////////////////////////////////////////////
    ////  In between the Mylar and Q1 quads   MyW--Fl7--Q1----FL6-----Q2
    ///   Moved after the SLTB4 to monitor beam energy spread M. Raggi 18/01/2022

    G4double Flag7FrontPosX = Q1PosX+500*mm*sin(magnetAngle);
    G4double Flag7FrontPosY = 0;							 
    G4double Flag7FrontPosZ = Q1PosZ-500*mm*cos(magnetAngle);
    G4ThreeVector Flag7FrontPos = G4ThreeVector(Flag7FrontPosX,Flag7FrontPosY,Flag7FrontPosZ);    
  
    //    G4Tubs* solidBeamFlag7 = new G4Tubs("solidBeamFlag7",0.,Q4Radius-0.1*mm,100.1*mm,0.*deg,360.*deg);
    G4Tubs* solidBeamFlag7 = new G4Tubs("solidBeamFlag7",0.,Q4Radius-0.15*mm,0.1*mm,0.*deg,360.*deg);
    G4LogicalVolume* logicalBeamFlag7 = new G4LogicalVolume(solidBeamFlag7,G4Material::GetMaterial("Vacuum"),"logicalBeamFlag7",0,0,0);
    logicalBeamFlag7->SetVisAttributes(FlagVisAttr);
    
    printf("Registering Flag7 %d at %f %f %f\n",geo->BeamFlagIsEnabled(),Flag7FrontPosX,Flag7FrontPosY,Flag7FrontPosZ);
    new G4PVPlacement(Q1Rot,Flag7FrontPos,logicalBeamFlag7,"BeamLineBeamFlag7",fMotherVolume,false,0,true);    
    logicalBeamFlag7->SetSensitiveDetector(beamFlagSD);
  }
}

void BeamLineStructure::CreateAllSLTB()
{
  printf("Setting up collimators SLTB2 SLTB3 SLTB4 \n");

  ////////////////////////////////////////////////////////////////////////
  // Collimators after mylar sltb4(LR) sltb3(UPDW)  Mylar 
  ///////////////////////////////////////////////////////////////////////
  // November 2020 tune L. Foggetta
  //TB4R   26.16 TB4L   22.30          TB3 Up 26.06  TB3 Down 23.32	
  //TB4R_M 28.44 TB4L_M 24.01 	       TB3U_M 26.024 TB3D_M   26.39    	
  //TB4Ape  2.28    +    1.71 = 4mm    TB4Ape  0     +         3.07 = 3.07 mm

  // From Luca tune 22 run stable.
  //TB2R   25.6 TB2L   22.30           
  //TB2R_M 26.6 TB2L_M 28.29 	       
  //TB2Ape  1.71 = 1.71mm    

  std::cout<<"Set collimators aperture SLTB3 "<<geo->GetSLTB3Aperture()<<std::endl;
  std::cout<<"Set collimators aperture SLTB4 "<<geo->GetSLTB4Aperture()<<std::endl;
  G4double SLTBThickness = 35.0*mm;
  G4double magnetAngle = geo->GetDHSTB002AngularSpan();
  G4double WallPipeRIn  = geo->Get2020PipeInnerRadius();
  G4double InLinacPipeLen  = geo->GetWallMylarWinDistance();
  G4double mpEntPosX = geo->GetMagPipeEnterPosX();
  G4double mpEntPosY = geo->GetMagPipeEnterPosY();
  G4double mpEntPosZ = geo->GetMagPipeEnterPosZ();
  G4double WallPipeLen  = geo->GetDHSTB002WallDistance();
  G4double InWallPipeLen  = geo->GetWallPipeLen();
  G4double MylarWinFlgT    = geo->GetMylarWindowFlangeThick();

  G4double MylarWinFlgPosX = mpEntPosX+(WallPipeLen+InWallPipeLen+InLinacPipeLen+0.5*MylarWinFlgT)*sin(magnetAngle);
  G4double MylarWinFlgPosY = mpEntPosY;
  G4double MylarWinFlgPosZ = mpEntPosZ-(WallPipeLen+InWallPipeLen+InLinacPipeLen+0.5*MylarWinFlgT)*cos(magnetAngle);
  G4ThreeVector MylarWinFlgPos = G4ThreeVector(MylarWinFlgPosX,MylarWinFlgPosY,MylarWinFlgPosZ);
  G4RotationMatrix* MylarWinFlgRot = new G4RotationMatrix;
  MylarWinFlgRot->rotateY(magnetAngle);


  G4VisAttributes steelVisAttr   = G4VisAttributes(G4Color::Grey()); // Dark gray
  // Support of the collimatros just to visualize them
  G4VisAttributes CollVisAttr = G4VisAttributes(G4Colour::Blue());
  G4double Radius=60*mm;
  G4Box* solidCollSupportFull = new G4Box("solidCollSupportFull",2.0*Radius,2.0*Radius,SLTBThickness*0.5);
  G4Box* solidCollSupportHole = new G4Box("solidCollSupportHole",1.4*Radius,1.4*Radius,SLTBThickness*0.5+100*um);
  
  G4SubtractionSolid* solidCollSupport = new G4SubtractionSolid("solidCollSupport",solidCollSupportFull,solidCollSupportHole,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicalCollSupport = new G4LogicalVolume(solidCollSupport,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicCollSupport",0,0,0);

  logicalCollSupport->SetVisAttributes(CollVisAttr);
  //  logicalSupport->SetName("logicCollSupport");
  
  G4double SLTB3ToMylar = 309.24*mm;
  G4double SLTB3Aperture = geo->GetSLTB3Aperture();
  G4double SLTB3PosX = MylarWinFlgPosX-(SLTB3ToMylar+SLTBThickness*0.5)*sin(magnetAngle);
  G4double SLTB3PosY = MylarWinFlgPosY;							 
  G4double SLTB3PosZ = MylarWinFlgPosZ+(SLTB3ToMylar+SLTBThickness*0.5)*cos(magnetAngle);

  G4double SLTB4ToMylar = 519.24*mm;
  G4double SLTB4Aperture = geo->GetSLTB4Aperture();

  G4double SLTB4PosX = MylarWinFlgPosX-(SLTB4ToMylar+SLTBThickness*0.5)*sin(magnetAngle);
  G4double SLTB4PosY = MylarWinFlgPosY;							 
  G4double SLTB4PosZ = MylarWinFlgPosZ+(SLTB4ToMylar+SLTBThickness*0.5)*cos(magnetAngle);

  G4ThreeVector SLTB4Pos = G4ThreeVector(SLTB4PosX,SLTB4PosY,SLTB4PosZ);    
  G4ThreeVector SLTB3Pos = G4ThreeVector(SLTB3PosX,SLTB3PosY,SLTB3PosZ);    
  
  G4Tubs* solidSLTB3Full = new G4Tubs("solidSLTB3Full",0.,WallPipeRIn-0.1*mm,SLTBThickness,0.*deg,360.*deg);
  //  G4VSolid* solidSLTB3Hole = new G4Box("solidSLTB3Hole",SLTB3Aperture*0.5,WallPipeRIn-0.05*mm,SLTBThickness+2*mm);
  G4Box* solidSLTB3Hole =new G4Box("solidSLTB3Hole",WallPipeRIn-0.1*mm,SLTB3Aperture*0.5,SLTBThickness+2*mm);
  G4SubtractionSolid* solidSLTB3 = new G4SubtractionSolid("solidSLTB3",solidSLTB3Full,solidSLTB3Hole,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume*  logicalSLTB3 = new G4LogicalVolume(solidSLTB3,G4Material::GetMaterial("G4_W"),"logicalSLTB3",0,0,0);
  logicalSLTB3->SetVisAttributes(steelVisAttr);

  G4Tubs* solidSLTB4Full = new G4Tubs("solidSLTB4Full",0.,WallPipeRIn-0.1*mm,SLTBThickness,0.*deg,360.*deg);
  G4Box* solidSLTB4Hole = new G4Box("solidSLTB4Hole",SLTB4Aperture*0.5,WallPipeRIn-0.05*mm,SLTBThickness+2*mm);
  G4SubtractionSolid* solidSLTB4 = new G4SubtractionSolid("solidSLTB4",solidSLTB4Full,solidSLTB4Hole,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicalSLTB4 = new G4LogicalVolume(solidSLTB4,G4Material::GetMaterial("G4_W"),"logicalSLTB4",0,0,0);
  logicalSLTB4->SetVisAttributes(steelVisAttr);

  // positioning supports 
//  new G4PVPlacement(MylarWinFlgRot,SLTB3Pos,logicalCollSupport,"CollSuport",fMotherVolume,false,0,true);
//  new G4PVPlacement(MylarWinFlgRot,SLTB4Pos,logicalCollSupport,"CollSuport",fMotherVolume,false,0,true);
    
  // positioning collimators
//  new G4PVPlacement(MylarWinFlgRot,SLTB3Pos,logicalSLTB3,"BeamSLTB3",fMotherVolume,false,0,true);
//  new G4PVPlacement(MylarWinFlgRot,SLTB4Pos,logicalSLTB4,"BeamSLTB4",fMotherVolume,false,0,true);

  //*************************************************************
  // SLTB5 just before the DHSTB002 magnet only LR
  //***************************************************************

  G4double SLTB4oSLTB5Distance= 8677*mm;
  G4double SLTB5Aperture = geo->GetSLTB5Aperture();
  G4double SLTB5PosX = MylarWinFlgPosX-(SLTB4ToMylar+SLTBThickness*0.5)*sin(magnetAngle)-SLTB4oSLTB5Distance*sin(magnetAngle);
  G4double SLTB5PosY = MylarWinFlgPosY;							 
  G4double SLTB5PosZ = MylarWinFlgPosZ+(SLTB4ToMylar+SLTBThickness*0.5)*cos(magnetAngle)+SLTB4oSLTB5Distance*cos(magnetAngle);
  G4ThreeVector SLTB5Pos = G4ThreeVector(SLTB5PosX,SLTB5PosY,SLTB5PosZ);    

  G4Tubs* solidSLTB5Full = new G4Tubs("solidSLTB5Full",0.,WallPipeRIn-0.1*mm,SLTBThickness,0.*deg,360.*deg);
  G4Box*  solidSLTB5Hole = new G4Box("solidSLTB5Hole",SLTB5Aperture*0.5,WallPipeRIn-0.1*mm,SLTBThickness+2*mm);
  G4SubtractionSolid* solidSLTB5 = new G4SubtractionSolid("solidSLTB5",solidSLTB5Full,solidSLTB5Hole,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume*  logicalSLTB5 = new G4LogicalVolume(solidSLTB5,G4Material::GetMaterial("G4_W"),"logicalSLTB5",0,0,0);
  logicalSLTB5->SetVisAttributes(steelVisAttr);

  // positioning supports 
  //  new G4PVPlacement(MylarWinFlgRot,SLTB5Pos,logicalCollSupport,"CollSuport",fMotherVolume,false,0,true);
  // positioning collimators
  new G4PVPlacement(MylarWinFlgRot,SLTB5Pos,logicalSLTB5,"BeamSLTB5",fMotherVolume,false,0,true);

}

//
// Create the pulsed splitting magnets which diverts the beam towards BTF2 
//
void BeamLineStructure::CreatePulsedMagnet()
{
  printf("Setting up pulsed magnet for 2022 simulations \n");
  G4VisAttributes PulsedVisAttr = G4VisAttributes(G4Colour::Brown());
  G4VisAttributes steelVisAttr   = G4VisAttributes(G4Color::Grey()); // Dark gray
  if ( ! fBeamLineIsVisible ) {
    steelVisAttr   = G4VisAttributes::Invisible;
    PulsedVisAttr = G4VisAttributes::Invisible;
  }

  G4double magnetAngle = geo->GetDHSTB002AngularSpan();  
  G4ThreeVector DHSTB002FlangeExitPos=G4ThreeVector(7833.494317,0,-10182.154317); //exit of DHSTB001 flange
  G4double DistanceFromDHSTB002Exit=  geo->GetDHSTB002ToPulsedMagnet();

  G4double PulsedPipeSizeX = geo->GetPulsedPipeSizeX();		      
  G4double PulsedPipeSizeY = geo->GetPulsedPipeSizeY();		      
  G4double PulsedPipeSizeZ = geo->GetPulsedPipeStraightLength();	      
  
  G4double PulsedHoleSizeX = geo->GetPulsedPipeHoleSizeX();	      
  G4double PulsedHoleSizeY = geo->GetPulsedPipeHoleSizeY();	      
  G4double PulsedHoleSizeZ = geo->GetPulsedPipeStraightLength()+100.*um;

  // create the strait flat pipe and attach the two flanges
  G4double PulsedFlangeR     = geo->GetPulsedPipeFlangeRadius();
  G4double PulsedFlangeThick = geo->GetPulsedPipeFlangeThick();
  G4ThreeVector posFlangeF = G4ThreeVector(0.,0.,PulsedPipeSizeZ*0.5-PulsedFlangeThick*0.5);
  G4ThreeVector posFlangeB = G4ThreeVector(0.,0.,-PulsedPipeSizeZ*0.5+PulsedFlangeThick*0.5);
  
  G4Box* solidPulsedFull = new G4Box("solidPulsedFull",0.5*PulsedPipeSizeX,0.5*PulsedPipeSizeY,0.5*PulsedPipeSizeZ);
  G4Box* solidPulsedHole = new G4Box("solidPulsedHole",0.5*PulsedHoleSizeX,0.5*PulsedHoleSizeY,0.5*PulsedHoleSizeZ);
  G4Tubs* solidPulsedFlange = new G4Tubs("solidPulsedFlange",0.,PulsedFlangeR,0.5*PulsedFlangeThick,0.*deg,360.*deg);
  
  G4UnionSolid* solidPulsedFull1Flange = new G4UnionSolid("solidPulsedFull1Flange",solidPulsedFull,solidPulsedFlange,0,posFlangeB);
  G4UnionSolid* solidPulsedFullFlange = new G4UnionSolid("solidPulsedFullFlange",solidPulsedFull1Flange,solidPulsedFlange,0,posFlangeF);
  
  G4SubtractionSolid* solidStraightPulsedPipe = new G4SubtractionSolid("solidStraightPulsedPipe",solidPulsedFullFlange,solidPulsedHole,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicalStraightPulsedPipe = new G4LogicalVolume(solidStraightPulsedPipe,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalStraightPulsedPipe",0,0,0);
  logicalStraightPulsedPipe->SetVisAttributes(steelVisAttr);
  
  // compute the position starting from the exit of DHSTB002 and moving back
  G4double PulsedMagnetPipePosX =  DHSTB002FlangeExitPos.x()*mm-PulsedPipeSizeZ/2*mm*sin(magnetAngle)-DistanceFromDHSTB002Exit*mm*sin(magnetAngle);
  G4double PulsedMagnetPipePosZ =  DHSTB002FlangeExitPos.z()*mm+PulsedPipeSizeZ/2*mm*cos(magnetAngle)+DistanceFromDHSTB002Exit*mm*cos(magnetAngle);
  G4ThreeVector PulsedMagnetPipePos =G4ThreeVector(PulsedMagnetPipePosX,0,PulsedMagnetPipePosZ);
  G4RotationMatrix* WallPipeRot = new G4RotationMatrix;
  WallPipeRot->rotateY(magnetAngle);
  new G4PVPlacement(WallPipeRot,PulsedMagnetPipePos,logicalStraightPulsedPipe,"PulsedFlangeFront",fMotherVolume,false,0,true); 
  
  //Build the Iron structure
  G4double PulsedIronSizeX = 276*mm; 
  G4double PulsedIronSizeY = 360*mm;
  G4double PulsedIronSizeZ = 900*mm;

  G4double PulsedIronHoleSizeX = 57*mm;
  G4double PulsedIronHoleSizeY = 180*mm;
  G4double PulsedIronHoleSizeZ = 901*mm;
  		
  G4double PulsedIronGapSizeX = 110*mm;
  G4double PulsedIronGapSizeY = 25*mm;
  G4double PulsedIronGapSizeZ = 901*mm;
  G4ThreeVector posGap = G4ThreeVector(PulsedIronHoleSizeX/2+PulsedIronGapSizeX/2,0.,0.);
  
  G4Box* solidPulsedIronFull = new G4Box("solidPulsedIronFull",0.5*PulsedIronSizeX,0.5*PulsedIronSizeY,0.5*PulsedIronSizeZ);
  G4Box* solidPulsedIronHole = new G4Box("solidPulsedIronHole",0.5*PulsedIronHoleSizeX,0.5*PulsedIronHoleSizeY,0.5*PulsedIronHoleSizeZ);
  G4Box* solidPulsedIronGap  = new G4Box("solidPulsedIronGap",0.5*PulsedIronGapSizeX,0.5*PulsedIronGapSizeY,0.5*PulsedIronGapSizeZ);
  
  G4UnionSolid* solidPulsedIronFullHole  = new G4UnionSolid("solidPulsedIronFullHole",solidPulsedIronHole,solidPulsedIronGap,0,posGap);
  G4SubtractionSolid*  solidPulsedIron = new G4SubtractionSolid("solidPulsedIron",solidPulsedIronFull,solidPulsedIronFullHole,0,G4ThreeVector(0.,0.,0.));
  
  G4LogicalVolume* logicalPulsedIron = new G4LogicalVolume(solidPulsedIron,G4Material::GetMaterial("G4_STAINLESS-STEEL"),"logicalPulsedIron",0,0,0);
  logicalPulsedIron->SetVisAttributes(PulsedVisAttr);
  
  G4double PulsedMagnetIronPosX =  DHSTB002FlangeExitPos.x()*mm-PulsedPipeSizeZ/2*mm*sin(magnetAngle)-DistanceFromDHSTB002Exit*mm*sin(magnetAngle)-(PulsedIronHoleSizeX/2+PulsedIronGapSizeX/2)*mm*sin(magnetAngle);
  G4double PulsedMagnetIronPosZ =  DHSTB002FlangeExitPos.z()*mm+PulsedPipeSizeZ/2*mm*cos(magnetAngle)+DistanceFromDHSTB002Exit*mm*cos(magnetAngle)-(PulsedIronHoleSizeX/2+PulsedIronGapSizeX/2)*mm*cos(magnetAngle);
  G4ThreeVector PulsedMagnetIronPos =G4ThreeVector(PulsedMagnetIronPosX,0,PulsedMagnetIronPosZ);
  new G4PVPlacement(WallPipeRot,PulsedMagnetIronPos,logicalPulsedIron,"PulsedIron",fMotherVolume,false,0,true); 
  
}
