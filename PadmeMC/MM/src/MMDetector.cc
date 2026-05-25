
// MMDetector.cc
// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
// --------------------------------------------------------------

#include "MMDetector.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Region.hh"
#include "G4VisAttributes.hh"
#include "G4DigiManager.hh"
#include "G4ProductionCuts.hh"
#include "G4SubtractionSolid.hh"
#include "G4NistManager.hh"
#include "G4UnitsTable.hh"

#include "MMGeometry.hh"
#include "MMSD.hh"
#include "MMDigitizer.hh"
// #include "MMROGeometry.hh"



MMDetector::MMDetector(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{

  // Connect to MMMessenger to enable datacard configuration
  fMMMessenger = new MMMessenger(this);

}

MMDetector::~MMDetector()
{
  delete fMMMessenger;
}

// IMPLEMENTS the MM geometry

void MMDetector::CreateGeometry()
{

  MMGeometry* geo = MMGeometry::GetInstance();
  
  // CREATE MM CONTAINER
  printf("MM be placed at %f %f %f\n",geo->GetMMPosX(),geo->GetMMPosY(),geo->GetMMPosZ());
  G4ThreeVector MMPos = G4ThreeVector(geo->GetMMPosX(),geo->GetMMPosY(),geo->GetMMPosZ()); 
  G4double MMSizeX = geo->GetMMSizeX();
  G4double MMSizeY = geo->GetMMSizeY();
  G4double MMSizeZ = geo->GetMMSizeZ();

  printf("MM size is %f %f %f\n",MMSizeX,MMSizeY,MMSizeZ);
  G4Box* solidMMFull = new G4Box("solidMMFull",0.5*MMSizeX,0.5*MMSizeY,0.5*MMSizeZ);
  fMMVolume = new G4LogicalVolume(solidMMFull,G4Material::GetMaterial("Vacuum"),"MMLogic",0,0,0);
  G4Region* MMregion = new G4Region("MM-Region");
  MMregion->AddRootLogicalVolume(fMMVolume);

  // G4ProductionCuts* cuts = new G4ProductionCuts();
  // cuts->SetProductionCut(5*um, G4ProductionCuts::GetIndex("e-"));
  // cuts->SetProductionCut(5*um, G4ProductionCuts::GetIndex("gamma"));
  // MMregion->SetProductionCuts(cuts);

  fMMVolume->SetVisAttributes(G4VisAttributes(G4Colour::Green()));
  fMMVolume->SetVisAttributes(G4VisAttributes::GetInvisible);
  G4RotationMatrix* rotationMatrix = new G4RotationMatrix(); 
  rotationMatrix->rotateY(0.*deg); // added to mimic test beam conditions
  new G4PVPlacement(rotationMatrix,MMPos,fMMVolume,"MM",fMotherVolume,false,0,false);

  //GAS VOLUME
  G4double MMDriftSizeX = geo->GetMMDriftSizeX(); 
  G4double MMDriftSizeY = geo->GetMMDriftSizeY(); 
  G4double MMDriftSizeZ = geo->GetMMDriftSizeZ() + 2*(geo->GetMMAmpGapSizeZ());

  printf("MM Gas Volume size is %f %f %f\n",MMDriftSizeX,MMDriftSizeY,MMDriftSizeZ);
  G4Box* solidMMDrift = new G4Box("solidMMDrift", 0.5*MMDriftSizeX, 0.5*MMDriftSizeY, 0.5*MMDriftSizeZ);
  printf("GasDensity: %f\n", G4Material::GetMaterial("ArCF4Iso")->GetDensity() / (g/cm3));
  fDriftVolume = new G4LogicalVolume(solidMMDrift, G4Material::GetMaterial("ArCF4Iso"), "MMDrift",0,0,0);
  fDriftVolume->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
  new G4PVPlacement(0,G4ThreeVector(0,0,0),fDriftVolume,"MMDrift",fMMVolume,false,0,false);

  // //ADDING AN FR4 tube
  // G4double tubeThickness =5*mm;

  // G4double rOuter = 10*cm;   
  // G4double rInner = rOuter - tubeThickness;

  // G4Tubs* solidFR4Tube = new G4Tubs("solidFR4Tube",rInner,rOuter,0.5 * MMDriftSizeZ,0.*deg,360.*deg);
  // G4LogicalVolume* logicFR4Tube = new G4LogicalVolume(solidFR4Tube, G4Material::GetMaterial("CarbonFiber"), "CarbonFiberTube");
  // logicFR4Tube->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));
  // new G4PVPlacement( 0,G4ThreeVector(0,0,0),logicFR4Tube,"FR4Tube",fMMVolume,false,0,false);

  // G4Tubs* solidAirTube = new G4Tubs("solidAirTube",0.,rInner,0.5 * MMDriftSizeZ,0.*deg,360.*deg);
  // G4LogicalVolume* logicAirTube = new G4LogicalVolume(solidAirTube, G4Material::GetMaterial("ArCF4Iso"), "AirTube");
  // logicAirTube->SetVisAttributes(G4VisAttributes(G4Colour::White()));
  // new G4PVPlacement( 0,G4ThreeVector(0,0,0),logicAirTube,"AirTube",fMMVolume,false,0,false);


  //READOUT PLANES
  G4double MMPanelSizeX = geo->GetMMPanelSizeX();
  G4double MMPanelSizeY = geo->GetMMPanelSizeY();
  G4double MMPanelSizeZ = geo->GetMMPanelSizeZ();

  G4double MMFaradayPanelSizeX = geo->GetMMFaradayPanelSizeX();
  G4double MMFaradayPanelSizeY = geo->GetMMFaradayPanelSizeY();

  //define layers of planes
  G4double MMCopperSizeZ = geo->GetMMCopperSizeZ();
  G4Box* solidCopperVolume = new G4Box("solidCopperVolume", 0.5*MMPanelSizeX, 0.5*MMPanelSizeY, 0.5*MMCopperSizeZ);
  G4LogicalVolume* fMMCopperVolume = new G4LogicalVolume(solidCopperVolume, G4Material::GetMaterial("G4_Cu"), "CopperShieldVolume",0,0,0);
  fMMCopperVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));

  //Effective densities
  G4double CopperDensity = G4Material::GetMaterial("G4_Cu")->GetDensity() / (g/cm3);
  G4double XEffDensity = 0, YEffDensity = 0;

  G4String ReadoutType = geo->GetReadoutType();

  if(ReadoutType == "strips"){
    printf("MM Readout Type : strips\n");
    XEffDensity = CopperDensity * (geo->GetMMStripWidth()/geo->GetMMStripPitch()); //strip case
    YEffDensity = CopperDensity * (geo->GetMMStripWidth()/geo->GetMMStripPitch()); //strip case
  }

  if(ReadoutType == "pads"){
    printf("MM Readout Type : pads\n");
    XEffDensity = CopperDensity * (geo->GetXPadArea()/(geo->GetPadDistance()*geo->GetPadDistance())); //pads case
    YEffDensity = CopperDensity * (geo->GetYPadArea()/(geo->GetPadDistance()*geo->GetPadDistance())); //pads case
  }
  

  
  G4Material* XEffMaterial = new G4Material("XEffMaterial", XEffDensity, 1);
  XEffMaterial->AddMaterial(G4Material::GetMaterial("G4_Cu"), 1.0);
  printf("CopperDensity: %f, XEffDensity: %f \n", CopperDensity, XEffDensity);
   
  G4LogicalVolume* fXReadoutVolume = new G4LogicalVolume(solidCopperVolume, XEffMaterial, "XReadoutVolume",0,0,0);
  fXReadoutVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  
  G4Material* YEffMaterial = new G4Material("YEffMaterial", YEffDensity, 1);
  YEffMaterial->AddMaterial(G4Material::GetMaterial("G4_Cu"), 1.0);
  printf("CopperDensity: %f, YEffDensity: %f \n", CopperDensity, YEffDensity);
  
  G4LogicalVolume* fYReadoutVolume = new G4LogicalVolume(solidCopperVolume, YEffMaterial, "YReadoutVolume",0,0,0);
  fYReadoutVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));

  G4double MMFR4SizeZ = geo->GetMMFR4SizeZ();
  G4Box* solidFR4Volume = new G4Box("solidFR4Volume", 0.5*MMPanelSizeX, 0.5*MMPanelSizeY, 0.5*MMFR4SizeZ);
  G4LogicalVolume* fMMFR4Volume = new G4LogicalVolume(solidFR4Volume, G4Material::GetMaterial("PCB"), "FR4Volume", 0,0,0);
  fMMFR4Volume->SetVisAttributes(G4VisAttributes(G4Colour::Green()));
  //Lateral panels, FR4 and Brass
  G4double MMLateralFR4 = geo->GetMMLateralFR4();
  G4Box* solidLateralFR4 = new G4Box("solidLateralFR4", 0.5*MMLateralFR4, 0.5*MMPanelSizeY, 0.5*geo->GetMMDriftSizeZ());
  G4LogicalVolume* fMMLateralFR4Volume =new G4LogicalVolume(solidLateralFR4,G4Material::GetMaterial("PCB"), "solidLateralFR4Volume",0,0,0);

  G4double MMLateralBrass = geo->GetMMLateralBrass();
  G4Box* solidLateralBrass = new G4Box("solidLateralBrass",0.5*MMLateralBrass,0.5*MMPanelSizeY, 0.5*geo->GetMMDriftSizeZ());
  G4LogicalVolume* fMMLateralBrassVolume =new G4LogicalVolume(solidLateralBrass,G4Material::GetMaterial("G4_BRASS"), "solidLateralBrassVolume",0,0,0);

  fMMLateralFR4Volume->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));
  fMMLateralBrassVolume->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));

  G4double MMFaradayFR4SizeZ = geo->GetMMFaradayFR4SizeZ();
  G4Box* solidFaradayFR4Volume = new G4Box("solidFaradayFR4Volume", 0.5*MMFaradayPanelSizeX, 0.5*MMFaradayPanelSizeY, 0.5*MMFaradayFR4SizeZ);
  G4LogicalVolume* fMMFaradayFR4Volume = new G4LogicalVolume(solidFaradayFR4Volume, G4Material::GetMaterial("PCB"), "FaradayFR4Volume", 0,0,0);
  fMMFaradayFR4Volume->SetVisAttributes(G4VisAttributes(G4Colour::Green()));

  G4double MMFaradayCopperSizeZ = geo->GetMMFaradayCopperSizeZ();
  G4Box* solidFaradayCopperVolume = new G4Box("solidFaradayCopperVolume", 0.5*MMFaradayPanelSizeX, 0.5*MMFaradayPanelSizeY, 0.5*MMFaradayCopperSizeZ);
  G4LogicalVolume* fMMFaradayCopperVolume = new G4LogicalVolume(solidFaradayCopperVolume, G4Material::GetMaterial("PCB"), "FaradayCopperVolume", 0,0,0);
  fMMFaradayCopperVolume->SetVisAttributes(G4VisAttributes(G4Colour::Green()));


  G4double MMKaptonSizeZ = geo->GetMMKaptonSizeZ();
  G4Box* solidKaptonVolume = new G4Box("solidKaptonVolume", 0.5*MMPanelSizeX, 0.5*MMPanelSizeY, 0.5*MMKaptonSizeZ);
  G4LogicalVolume* fMMKaptonVolume = new G4LogicalVolume(solidKaptonVolume, G4Material::GetMaterial("Kapton"), "KaptonVolume",0,0,0);
  fMMKaptonVolume->SetVisAttributes(G4VisAttributes(G4Colour::Brown()));

  G4double MMFR4ReadoutSizeZ = geo->GetMMFR4ReadoutSizeZ();
  G4Box* solidFR4ReadoutVolume = new G4Box("solidFR4ReadoutVolume", 0.5*MMPanelSizeX, 0.5*MMPanelSizeY, 0.5*MMFR4ReadoutSizeZ);
  G4LogicalVolume* fMMFR4ReadoutVolume = new G4LogicalVolume(solidFR4ReadoutVolume, G4Material::GetMaterial("PCB"), "FR4ReadoutVolume",0,0,0);
  fMMFR4ReadoutVolume->SetVisAttributes(G4VisAttributes(G4Colour::Green()));

  G4double MMNomexSizeZ = geo->GetMMNomexSizeZ();
  G4Box* solidNomexVolume = new G4Box("solidNomexVolume", 0.5*MMPanelSizeX, 0.5*MMPanelSizeY, 0.5*MMNomexSizeZ);
  G4LogicalVolume* fMMNomexVolume = new G4LogicalVolume(solidNomexVolume, G4Material::GetMaterial("NomexFoilMM"), "NomexVolume", 0,0,0);
  fMMNomexVolume->SetVisAttributes(G4VisAttributes(G4Colour::Magenta()));

  G4double MMCarbonSizeZ = geo->GetMMCarbonSizeZ();
  G4Box* solidCarbonVolume = new G4Box("solidCarbonVolume", 0.5*MMPanelSizeX, 0.5*MMPanelSizeY, 0.5*MMCarbonSizeZ);
  G4LogicalVolume* fMMCarbonVolume = new G4LogicalVolume(solidCarbonVolume, G4Material::GetMaterial("G4_C"), "CarbonVolume", 0,0,0);
  fMMCarbonVolume->SetVisAttributes(G4VisAttributes(G4Colour::Cyan()));
  //For test with Mylar panel
  G4double MMMylarClosing = 100*um;
  G4Box* solidMylarClosing = new G4Box("solidMylarClosing", 0.5*MMPanelSizeX, 0.5*MMPanelSizeY, 0.5*MMMylarClosing);
  G4LogicalVolume* fMMMylarClosing = new G4LogicalVolume(solidMylarClosing, G4Material::GetMaterial("G4_MYLAR"), "MylarClosingVolume", 0,0,0);
  fMMMylarClosing->SetVisAttributes(G4VisAttributes(G4Colour::Red()));

  
  // Placement of layers inside front panel
  //maybe put this into a function
  G4double MMLayerGap = geo->GetMMLayerGap();

  G4ThreeVector CarbonPos = G4ThreeVector(0,0,-0.5*(MMDriftSizeZ+MMCarbonSizeZ+MMLayerGap));
  G4ThreeVector MylarPos = G4ThreeVector(0,0,-0.5*(MMDriftSizeZ+MMMylarClosing+MMLayerGap));
  //TEST WITH MYLAR
  //new G4PVPlacement(0,CarbonPos, fMMMylarClosing, "MylarClosingVolume", fMMVolume,false,0,false);
  new G4PVPlacement(0,CarbonPos, fMMCarbonVolume, "CarbonResistiveLayer", fMMVolume,false,0,false);
  G4ThreeVector KaptonPos2 = CarbonPos - G4ThreeVector(0,0,0.5*(MMCarbonSizeZ+MMKaptonSizeZ+MMLayerGap));
  new G4PVPlacement(0,KaptonPos2, fMMKaptonVolume, "KaptonLayer2", fMMVolume, false,0,false);
  G4ThreeVector CopperPos2 = KaptonPos2 - G4ThreeVector(0,0,0.5*(MMKaptonSizeZ+MMCopperSizeZ+MMLayerGap));
  new G4PVPlacement(0,CopperPos2, fYReadoutVolume, "YReadoutLayer", fMMVolume, false,0,false);
  //to be changed to FR4Readout
  // G4ThreeVector KaptonPos1 = CopperPos2 - G4ThreeVector(0,0,0.5*(MMCopperSizeZ+MMKaptonSizeZ+MMLayerGap));
  // new G4PVPlacement(0,KaptonPos1, fMMKaptonVolume, "KaptonLayer1", fMMVolume, false,0,false);
  // G4ThreeVector CopperPos1 = KaptonPos1 - G4ThreeVector(0,0,0.5*(MMKaptonSizeZ+MMCopperSizeZ+MMLayerGap));
  G4ThreeVector FR4ReadoutPos1 = CopperPos2 - G4ThreeVector(0,0,0.5*(MMCopperSizeZ+MMFR4ReadoutSizeZ+MMLayerGap));
  new G4PVPlacement(0,FR4ReadoutPos1, fMMFR4ReadoutVolume, "FR4ReadoutLayerFront", fMMVolume, false,0,false);
  G4ThreeVector CopperPos1 = FR4ReadoutPos1 - G4ThreeVector(0,0,0.5*(MMFR4ReadoutSizeZ+MMCopperSizeZ+MMLayerGap));

  new G4PVPlacement(0,CopperPos1, fXReadoutVolume, "XReadoutLayer", fMMVolume, false,0,false);
  G4ThreeVector FR4Pos2 = CopperPos1 - G4ThreeVector(0,0,0.5*(MMCopperSizeZ+MMFR4SizeZ+MMLayerGap));
  new G4PVPlacement(0,FR4Pos2, fMMFR4Volume, "FR4Layer2", fMMVolume, false,0,false);
  G4ThreeVector NomexPos = FR4Pos2 - G4ThreeVector(0,0,0.5*(MMFR4SizeZ+MMNomexSizeZ+MMLayerGap));
  new G4PVPlacement(0,NomexPos, fMMNomexVolume, "NomexLayer", fMMVolume, false,0,false);
  
  G4ThreeVector FR4Pos1 = NomexPos - G4ThreeVector(0,0,0.5*(MMNomexSizeZ+MMFR4SizeZ+MMLayerGap));
  new G4PVPlacement(0,FR4Pos1, fMMFR4Volume, "FR4Layer1", fMMVolume, false,0,false);
  
  //Faraday cage positioning
  G4ThreeVector FR4FaradayPos1 = FR4Pos1 - G4ThreeVector(0,0,0.5*(MMFR4SizeZ + MMFaradayFR4SizeZ+MMLayerGap));
  new G4PVPlacement(0, FR4FaradayPos1, fMMFaradayFR4Volume, "FaradayFR4", fMMVolume,false,0,false);
  G4ThreeVector CopperFaradayPos1 = FR4FaradayPos1 - G4ThreeVector(0,0,0.5*(MMFaradayFR4SizeZ + MMFaradayCopperSizeZ+MMLayerGap));
  new G4PVPlacement(0, CopperFaradayPos1, fMMFaradayCopperVolume, "FaradayCopper", fMMVolume,false,0,false);
  
  

  // Placement of layers inside rear panel
  CarbonPos = G4ThreeVector(0,0,+0.5*(MMDriftSizeZ+MMCarbonSizeZ+MMLayerGap));
  //new G4PVPlacement(0,CarbonPos, fMMMylarClosing, "MylarClosing", fMMVolume,false,1,false);
  new G4PVPlacement(0,CarbonPos, fMMCarbonVolume, "CarbonResistiveLayer", fMMVolume,false,1,false);
  KaptonPos2 = CarbonPos + G4ThreeVector(0,0,0.5*(MMCarbonSizeZ+MMKaptonSizeZ+MMLayerGap));
  new G4PVPlacement(0,KaptonPos2, fMMKaptonVolume, "KaptonLayer2", fMMVolume, false,1,false);
  CopperPos2 = KaptonPos2 + G4ThreeVector(0,0,0.5*(MMKaptonSizeZ+MMCopperSizeZ+MMLayerGap));
  new G4PVPlacement(0,CopperPos2, fYReadoutVolume, "YReadoutLayer", fMMVolume, false,1,false);
  //to be changed to FR4Readout
  // KaptonPos1 = CopperPos2 + G4ThreeVector(0,0,0.5*(MMCopperSizeZ+MMKaptonSizeZ+MMLayerGap));
  // new G4PVPlacement(0,KaptonPos1, fMMKaptonVolume, "KaptonLayer1", fMMVolume, false,1,false);
  // CopperPos1 = KaptonPos1 + G4ThreeVector(0,0,0.5*(MMKaptonSizeZ+MMCopperSizeZ+MMLayerGap));
  FR4ReadoutPos1 = CopperPos2 + G4ThreeVector(0,0,0.5*(MMCopperSizeZ+MMFR4ReadoutSizeZ+MMLayerGap));
  new G4PVPlacement(0,FR4ReadoutPos1, fMMFR4ReadoutVolume, "FR4ReadoutLayerRear", fMMVolume, false,0,false);
  CopperPos1 = FR4ReadoutPos1 + G4ThreeVector(0,0,0.5*(MMFR4ReadoutSizeZ+MMCopperSizeZ+MMLayerGap));

  new G4PVPlacement(0,CopperPos1, fXReadoutVolume, "XReadoutLayer", fMMVolume, false,1,false);
  FR4Pos2 = CopperPos1 + G4ThreeVector(0,0,0.5*(MMCopperSizeZ+MMFR4SizeZ+MMLayerGap));
  new G4PVPlacement(0,FR4Pos2, fMMFR4Volume, "FR4Layer2", fMMVolume, false,1,false);
  NomexPos = FR4Pos2 + G4ThreeVector(0,0,0.5*(MMFR4SizeZ+MMNomexSizeZ+MMLayerGap));
  new G4PVPlacement(0,NomexPos, fMMNomexVolume, "NomexLayer", fMMVolume, false,1,false);
  FR4Pos1 = NomexPos + G4ThreeVector(0,0,0.5*(MMNomexSizeZ+MMFR4SizeZ+MMLayerGap));
  new G4PVPlacement(0,FR4Pos1, fMMFR4Volume, "FR4Layer1", fMMVolume, false,1,false);
  // ShieldPos = FR4Pos1 + G4ThreeVector(0,0,0.5*(MMFR4SizeZ + MMCopperSizeZ+MMLayerGap));
  // new G4PVPlacement(0, ShieldPos, fMMCopperVolume, "CopperShield", fMMVolume,false,1,false);
  
  FR4FaradayPos1 = FR4Pos1 + G4ThreeVector(0,0,0.5*(MMFR4SizeZ + MMFaradayFR4SizeZ+MMLayerGap));
  new G4PVPlacement(0, FR4FaradayPos1, fMMFaradayFR4Volume, "FaradayFR4", fMMVolume,false,0,false);
  CopperFaradayPos1 = FR4FaradayPos1 + G4ThreeVector(0,0,0.5*(MMFaradayFR4SizeZ + MMFaradayCopperSizeZ+MMLayerGap));
  new G4PVPlacement(0, CopperFaradayPos1, fMMFaradayCopperVolume, "FaradayCopper", fMMVolume,false,0,false);
  
  
  //Definition of the Meshes 
  G4double AmpMeshSizeZ = geo->GetMMAmpMeshSizeZ();
  G4double CathodeMeshSizeZ = geo->GetMMCathodeMeshSizeZ();
  
  G4Box* solidAmpMeshVolume = new G4Box("solidAmpMeshVolume", 0.5*MMPanelSizeX, 0.5*MMPanelSizeY, 0.5*AmpMeshSizeZ);
  fAmpMeshVolume = new G4LogicalVolume(solidAmpMeshVolume, G4Material::GetMaterial("G4_STAINLESS-STEEL"), "MeshVolume",0,0,0);
  fAmpMeshVolume->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));
  G4Box* solidCathodeMeshVolume = new G4Box("solidCathodeMeshVolume", 0.5*MMPanelSizeX, 0.5*MMPanelSizeY, 0.5*CathodeMeshSizeZ);
  fCathodeMeshVolume = new G4LogicalVolume(solidCathodeMeshVolume, G4Material::GetMaterial("G4_STAINLESS-STEEL"), "MeshVolume",0,0,0);
  fCathodeMeshVolume->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));


  new G4PVPlacement(0, G4ThreeVector(0,0,-0.5*geo->GetMMDriftSizeZ()), fAmpMeshVolume, "FrontMesh", fDriftVolume,false,0,false);
  new G4PVPlacement(0, G4ThreeVector(0,0, 0.5*geo->GetMMDriftSizeZ()), fAmpMeshVolume, "RearMesh", fDriftVolume,false,0,false);
  new G4PVPlacement(0, G4ThreeVector(0,0,0), fCathodeMeshVolume, "CenterMesh", fDriftVolume,false,0,false);


  //Placement of the lateral panels
  G4double xOffsetFR4 = 0.5*MMDriftSizeX + 0.5*MMLateralFR4;
  G4double xOffsetBrass = 0.5*MMDriftSizeX + MMLateralFR4 +10*um+0.5*MMLateralBrass;
  //left side
  new G4PVPlacement(0, G4ThreeVector(-xOffsetFR4,0,0), fMMLateralFR4Volume,"LeftSideLateralFR4",fMMVolume, false,0, false); 
  new G4PVPlacement(0, G4ThreeVector(-xOffsetBrass,0,0), fMMLateralBrassVolume,"LeftSideLateralBrass",fMMVolume, false,0, false); 
  //right side
  new G4PVPlacement(0, G4ThreeVector(xOffsetFR4,0,0), fMMLateralFR4Volume,"RightSideLateralFR4",fMMVolume, false,0, false); 
  new G4PVPlacement(0, G4ThreeVector(xOffsetBrass,0,0), fMMLateralBrassVolume,"RightSideLateralBrass",fMMVolume, false,0, false); 
  
  // Create digitizer for MM
  G4DigiManager* theDM = G4DigiManager::GetDMpointer();
  G4String MMDName = geo->GetMMDigitizerName();
  printf("Registering MM Digitizer %s\n",MMDName.data());
  MMDigitizer* MMD = new MMDigitizer(MMDName);
  theDM->AddNewModule(MMD);
  if(MMD != nullptr){printf("Creating MMDigitizer\n");}
  

  // Make MMDriftVolume a sensitive detector
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String MMSDName = geo->GetMMSensitiveDetectorName();
  printf("Registering MM SD %s\n",MMSDName.data());
  MMSD* MMsd = new MMSD(MMSDName);
  sdMan->AddNewDetector(MMsd);
  fDriftVolume->SetSensitiveDetector(MMsd); //setting only gas volume as sensitive
  printf("Setting drift volume as SD \n");

  // Computation of Radiation Length
  G4double inverse_sum = MMDriftSizeZ/(fDriftVolume->GetMaterial()->GetRadlen()) +
                        2*MMCopperSizeZ/(fMMCopperVolume->GetMaterial()->GetRadlen()) + 
                        2*MMCarbonSizeZ/(fMMCarbonVolume->GetMaterial()->GetRadlen()) +
                        4*MMFR4SizeZ/(fMMFR4Volume->GetMaterial()->GetRadlen()) + 
                        2*MMNomexSizeZ/(fMMNomexVolume->GetMaterial()->GetRadlen())  + 
                        4*MMKaptonSizeZ/(fMMKaptonVolume->GetMaterial()->GetRadlen()) + 
                        2*MMCopperSizeZ/(fXReadoutVolume->GetMaterial()->GetRadlen()) + 
                        2*MMCopperSizeZ/(fYReadoutVolume->GetMaterial()->GetRadlen()) +   
                        2*AmpMeshSizeZ/(fAmpMeshVolume->GetMaterial()->GetRadlen())  + 
                        CathodeMeshSizeZ/(fCathodeMeshVolume->GetMaterial()->GetRadlen());

  G4double radlen_tot = ((MMSizeZ-5*mm)/inverse_sum);
  G4cout << "Total Radiation Length is : " << G4BestUnit(radlen_tot, "Length")<<G4endl;
  //Total Radiation Length is : 664.797 cm


}

