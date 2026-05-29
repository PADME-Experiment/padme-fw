
// TMMDetector.cc
// --------------------------------------------------------------
// History:
//
// Created by Elisa Di Meco (elisa.dimeco@lnf.infn.it) 2026-05-27
// --------------------------------------------------------------

#include "TMMDetector.hh"

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

#include "TMMGeometry.hh"
#include "TMMSD.hh"
#include "TMMDigitizer.hh"
// #include "TMMROGeometry.hh"



TMMDetector::TMMDetector(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{

  // Connect to TMMMessenger to enable datacard configuration
  fTMMMessenger = new TMMMessenger(this);

}

TMMDetector::~TMMDetector()
{
  delete fTMMMessenger;
}

// IMPLEMENTS the TMM geometry

void TMMDetector::CreateGeometry()
{

  TMMGeometry* geo = TMMGeometry::GetInstance();
  
  // CREATE TMM CONTAINER
  printf("TMM be placed at %f %f %f\n",geo->GetTMMPosX(),geo->GetTMMPosY(),geo->GetTMMPosZ());
  G4ThreeVector TMMPos = G4ThreeVector(geo->GetTMMPosX(),geo->GetTMMPosY(),geo->GetTMMPosZ()); 
  G4double TMMSizeX = geo->GetTMMSizeX();
  G4double TMMSizeY = geo->GetTMMSizeY();
  G4double TMMSizeZ = geo->GetTMMSizeZ();

  printf("TMM size is %f %f %f\n",TMMSizeX,TMMSizeY,TMMSizeZ);
  G4Box* solidTMMFull = new G4Box("solidTMMFull",0.5*TMMSizeX,0.5*TMMSizeY,0.5*TMMSizeZ);
  fTMMVolume = new G4LogicalVolume(solidTMMFull,G4Material::GetMaterial("ArCF4Iso"),"TMMLogic",0,0,0);
  G4Region* TMMregion = new G4Region("TMM-Region");
  TMMregion->AddRootLogicalVolume(fTMMVolume);

  // G4ProductionCuts* cuts = new G4ProductionCuts();
  // cuts->SetProductionCut(5*um, G4ProductionCuts::GetIndex("e-"));
  // cuts->SetProductionCut(5*um, G4ProductionCuts::GetIndex("gamma"));
  // TMMregion->SetProductionCuts(cuts);

  fTMMVolume->SetVisAttributes(G4VisAttributes(G4Colour::Green()));
  fTMMVolume->SetVisAttributes(G4VisAttributes::GetInvisible);
  G4RotationMatrix* rotationMatrix = new G4RotationMatrix(); 
  rotationMatrix->rotateY(0.*deg); // added to mimic test beam conditions
  new G4PVPlacement(rotationMatrix,TMMPos,fTMMVolume,"TMM",fMotherVolume,false,0,false);


  //READOUT PLANES
  G4double TMMPanelSizeX = geo->GetTMMPanelSizeX();
  G4double TMMPanelSizeY = geo->GetTMMPanelSizeY();
  G4double TMMPanelSizeZ = geo->GetTMMPanelSizeZ();

  G4double TMMMylarClosingZ = geo->GetTMMMylarClosingZ();

  G4double TMMExtFrameZ = geo->GetTMMExtFrameZ();
  G4double TMMExtFrameX = geo->GetTMMExtFrameX();
  G4double TMMExtFrameY = geo->GetTMMExtFrameY(); 

  G4double TMMAluFrameSizeZ = geo->GetTMMAluFrameSizeZ();
  G4double TMMAluFrameSizeX = geo->GetTMMAluFrameSizeX();
  G4double TMMAluFrameSizeY = geo->GetTMMAluFrameSizeY();
  G4double TMMAluFrameInternalSizeX = geo->GetTMMAluFrameInternalSizeX();
  G4double TMMAluFrameInternalSizeY = geo->GetTMMAluFrameInternalSizeY();
  

  G4double TMMFaradayPanelSizeX = geo->GetTMMFaradayPanelSizeX();
  G4double TMMFaradayPanelSizeY = geo->GetTMMFaradayPanelSizeY();

  //GAS VOLUME
  G4double TMMDriftSizeX = geo->GetTMMDriftSizeX(); 
  G4double TMMDriftSizeY = geo->GetTMMDriftSizeY(); 
  G4double TMMDriftSizeZ = geo->GetTMMDriftSizeZ() + 1*(geo->GetTMMAmpGapSizeZ());

  G4double DriftOffsetZ = 0.5*TMMAluFrameSizeZ-TMMDriftSizeZ-TMMPanelSizeZ+6.21*mm;//6.21 tuned with visualization
  printf("TMM Gas Volume size is %f %f %f\n",TMMDriftSizeX,TMMDriftSizeY,TMMDriftSizeZ);
  G4Box* solidTMMDrift = new G4Box("solidTMMDrift", 0.5*TMMDriftSizeX, 0.5*TMMDriftSizeY, 0.5*TMMDriftSizeZ);
  printf("GasDensity: %f\n", G4Material::GetMaterial("ArCF4Iso")->GetDensity() / (g/cm3));
  fDriftVolume = new G4LogicalVolume(solidTMMDrift, G4Material::GetMaterial("ArCF4Iso"), "TMMDrift",0,0,0);
  fDriftVolume->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
  new G4PVPlacement(0,G4ThreeVector(0,0,DriftOffsetZ),fDriftVolume,"TMMDrift",fTMMVolume,false,0,false);

  // //ADDING AN FR4 tube
  // G4double tubeThickness =5*mm;

  // G4double rOuter = 10*cm;   
  // G4double rInner = rOuter - tubeThickness;

  // G4Tubs* solidFR4Tube = new G4Tubs("solidFR4Tube",rInner,rOuter,0.5 * TMMDriftSizeZ,0.*deg,360.*deg);
  // G4LogicalVolume* logicFR4Tube = new G4LogicalVolume(solidFR4Tube, G4Material::GetMaterial("CarbonFiber"), "CarbonFiberTube");
  // logicFR4Tube->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));
  // new G4PVPlacement( 0,G4ThreeVector(0,0,0),logicFR4Tube,"FR4Tube",fTMMVolume,false,0,false);

  // G4Tubs* solidAirTube = new G4Tubs("solidAirTube",0.,rInner,0.5 * TMMDriftSizeZ,0.*deg,360.*deg);
  // G4LogicalVolume* logicAirTube = new G4LogicalVolume(solidAirTube, G4Material::GetMaterial("ArCF4Iso"), "AirTube");
  // logicAirTube->SetVisAttributes(G4VisAttributes(G4Colour::White()));
  // new G4PVPlacement( 0,G4ThreeVector(0,0,0),logicAirTube,"AirTube",fTMMVolume,false,0,false);




  

  //define layers of planes
  G4double TMMCopperSizeZ = geo->GetTMMCopperSizeZ();
  G4Box* solidCopperVolume = new G4Box("solidCopperVolume", 0.5*TMMPanelSizeX, 0.5*TMMPanelSizeY, 0.5*TMMCopperSizeZ);
  G4LogicalVolume* fTMMCopperVolume = new G4LogicalVolume(solidCopperVolume, G4Material::GetMaterial("G4_Cu"), "CopperShieldVolume",0,0,0);
  fTMMCopperVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));

  //Effective densities
  G4double CopperDensity = G4Material::GetMaterial("G4_Cu")->GetDensity() / (g/cm3);
  G4double XEffDensity = 0, YEffDensity = 0;

  G4String ReadoutType = geo->GetReadoutType();

  if(ReadoutType == "strips"){
    printf("TMM Readout Type : strips\n");
    XEffDensity = CopperDensity * (geo->GetTMMStripWidth()/geo->GetTMMStripPitch()); //strip case
    YEffDensity = CopperDensity * (geo->GetTMMStripWidth()/geo->GetTMMStripPitch()); //strip case
  }

  if(ReadoutType == "pads"){
    printf("TMM Readout Type : pads\n");
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

  G4double TMMFR4SizeZ = geo->GetTMMFR4SizeZ();
  G4Box* solidFR4Volume = new G4Box("solidFR4Volume", 0.5*TMMPanelSizeX, 0.5*TMMPanelSizeY, 0.5*TMMFR4SizeZ);
  G4LogicalVolume* fTMMFR4Volume = new G4LogicalVolume(solidFR4Volume, G4Material::GetMaterial("PCB"), "FR4Volume", 0,0,0);
  fTMMFR4Volume->SetVisAttributes(G4VisAttributes(G4Colour::Green()));
  
  G4double TMMFaradayFR4SizeZ = geo->GetTMMFaradayFR4SizeZ();
  G4Box* solidFaradayFR4Volume = new G4Box("solidFaradayFR4Volume", 0.5*TMMFaradayPanelSizeX, 0.5*TMMFaradayPanelSizeY, 0.5*TMMFaradayFR4SizeZ);
  G4LogicalVolume* fTMMFaradayFR4Volume = new G4LogicalVolume(solidFaradayFR4Volume, G4Material::GetMaterial("PCB"), "FaradayFR4Volume", 0,0,0);
  fTMMFaradayFR4Volume->SetVisAttributes(G4VisAttributes(G4Colour::Green()));

  G4double TMMFaradayCopperSizeZ = geo->GetTMMFaradayCopperSizeZ();
  G4Box* solidFaradayCopperVolume = new G4Box("solidFaradayCopperVolume", 0.5*TMMFaradayPanelSizeX, 0.5*TMMFaradayPanelSizeY, 0.5*TMMFaradayCopperSizeZ);
  G4LogicalVolume* fTMMFaradayCopperVolume = new G4LogicalVolume(solidFaradayCopperVolume, G4Material::GetMaterial("PCB"), "FaradayCopperVolume", 0,0,0);
  fTMMFaradayCopperVolume->SetVisAttributes(G4VisAttributes(G4Colour::Green()));

  G4double TMMKaptonSizeZ = geo->GetTMMKaptonSizeZ();
  G4Box* solidKaptonVolume = new G4Box("solidKaptonVolume", 0.5*TMMPanelSizeX, 0.5*TMMPanelSizeY, 0.5*TMMKaptonSizeZ);
  G4LogicalVolume* fTMMKaptonVolume = new G4LogicalVolume(solidKaptonVolume, G4Material::GetMaterial("Kapton"), "KaptonVolume",0,0,0);
  fTMMKaptonVolume->SetVisAttributes(G4VisAttributes(G4Colour::Brown()));

  G4double TMMFR4ReadoutSizeZ = geo->GetTMMFR4ReadoutSizeZ();
  G4Box* solidFR4ReadoutVolume = new G4Box("solidFR4ReadoutVolume", 0.5*TMMPanelSizeX, 0.5*TMMPanelSizeY, 0.5*TMMFR4ReadoutSizeZ);
  G4LogicalVolume* fTMMFR4ReadoutVolume = new G4LogicalVolume(solidFR4ReadoutVolume, G4Material::GetMaterial("PCB"), "FR4ReadoutVolume",0,0,0);
  fTMMFR4ReadoutVolume->SetVisAttributes(G4VisAttributes(G4Colour::Green()));

  G4double TMMCarbonSizeZ = geo->GetTMMCarbonSizeZ();
  G4Box* solidCarbonVolume = new G4Box("solidCarbonVolume", 0.5*TMMPanelSizeX, 0.5*TMMPanelSizeY, 0.5*TMMCarbonSizeZ);
  G4LogicalVolume* fTMMCarbonVolume = new G4LogicalVolume(solidCarbonVolume, G4Material::GetMaterial("G4_C"), "CarbonVolume", 0,0,0);
  fTMMCarbonVolume->SetVisAttributes(G4VisAttributes(G4Colour::Cyan()));
  
  //TMM exsternal frames 

  G4Box* solidFrameFull = new G4Box("solidFrameFull", 0.5*TMMAluFrameSizeX, 0.5*TMMAluFrameSizeY, 0.5*TMMExtFrameZ);
  G4Box* solidFrameSubtr = new G4Box("solidFrameSubtr", 0.5*TMMExtFrameX, 0.5*TMMExtFrameY, 0.5*TMMExtFrameZ);
  G4SubtractionSolid* solidFrame = new G4SubtractionSolid("solidFrame", solidFrameFull, solidFrameSubtr, 0, G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* fTMMFrameVolume = new G4LogicalVolume(solidFrame, G4Material::GetMaterial("PCB"), "FR4FrameVolume", 0,0,0);
  fTMMFrameVolume->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));
  
  G4Box* solidFrameCuFull = new G4Box("solidFrameCuFull", 0.5*TMMAluFrameSizeX, 0.5*TMMAluFrameSizeY, 0.5*TMMFaradayCopperSizeZ);
  G4Box* solidFrameCuSubtr = new G4Box("solidFrameCuSubtr", 0.5*TMMExtFrameX, 0.5*TMMExtFrameY, 0.5*TMMExtFrameZ);
  G4SubtractionSolid* solidCuFrame = new G4SubtractionSolid("solidFrame", solidFrameCuFull, solidFrameCuSubtr, 0, G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* fTMMFrameCuVolume = new G4LogicalVolume(solidCuFrame, G4Material::GetMaterial("G4_Cu"), "CuFrameVolume", 0,0,0);
  fTMMFrameCuVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));



  G4Box* solidAluFrameFull = new G4Box("solidAluFrameFull", 0.5*TMMAluFrameSizeX, 0.5*TMMAluFrameSizeY, 0.5*TMMAluFrameSizeZ);
  G4Box* solidAluFrameSubtr = new G4Box("solidAluFrameSubtr", 0.5*TMMAluFrameInternalSizeX, 0.5*TMMAluFrameInternalSizeY, 0.5*TMMAluFrameSizeZ);
  G4SubtractionSolid* solidAluFrame = new G4SubtractionSolid("solidAluFrame", solidAluFrameFull, solidAluFrameSubtr, 0, G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* fTMMAluFrameVolume = new G4LogicalVolume(solidAluFrame, G4Material::GetMaterial("G4_Al"), "AluFrameVolume", 0,0,0);
  fTMMAluFrameVolume->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
  
  
  // Mylar panel
  
  G4Box* solidMylarClosing = new G4Box("solidMylarClosing", 0.5*TMMPanelSizeX, 0.5*TMMPanelSizeY, 0.5*TMMMylarClosingZ);
  G4LogicalVolume* fTMMMylarClosing = new G4LogicalVolume(solidMylarClosing, G4Material::GetMaterial("G4_MYLAR"), "MylarClosingVolume", 0,0,0);
  fTMMMylarClosing->SetVisAttributes(G4VisAttributes(G4Colour::Red()));

  G4ThreeVector AluFramePos = G4ThreeVector(0,0,0);
  new G4PVPlacement(0, AluFramePos, fTMMAluFrameVolume, "AluFrame", fTMMVolume, false, 0, false);
 
  // Placement of layers inside front panel
  G4double TMMLayerGap = geo->GetTMMLayerGap();
  G4ThreeVector MylarPos = G4ThreeVector(0,0,-0.5*(TMMAluFrameSizeZ+TMMMylarClosingZ+TMMLayerGap));
  new G4PVPlacement(0,MylarPos, fTMMMylarClosing, "MylarClosingVolume", fTMMVolume,false,0,false);

  G4ThreeVector FramePos = MylarPos-G4ThreeVector(0,0,0.5*(TMMExtFrameZ+TMMMylarClosingZ+TMMLayerGap));
  new G4PVPlacement(0, FramePos, fTMMFrameVolume, "FR4Frame", fTMMVolume, false, 0, false);
  G4ThreeVector CuFramePos = FramePos - G4ThreeVector(0,0,0.5*(TMMFaradayCopperSizeZ+TMMExtFrameZ+TMMLayerGap));
  new G4PVPlacement(0, CuFramePos, fTMMFrameCuVolume, "CuFrame", fTMMVolume, false, 0, false);
  
  // Placement of layers inside rear panel
  TMMLayerGap=TMMLayerGap;
  G4ThreeVector CarbonPos = G4ThreeVector(0,0,+0.5*(TMMDriftSizeZ+TMMLayerGap)+DriftOffsetZ);

  new G4PVPlacement(0,CarbonPos, fTMMCarbonVolume, "CarbonResistiveLayer", fTMMVolume,false,0,false);
  G4ThreeVector KaptonPos = CarbonPos + G4ThreeVector(0,0,0.5*(TMMCarbonSizeZ+TMMKaptonSizeZ+TMMLayerGap));
  new G4PVPlacement(0,KaptonPos, fTMMKaptonVolume, "KaptonLayer", fTMMVolume, false,0,false);
  G4ThreeVector CopperPos1 = KaptonPos + G4ThreeVector(0,0,0.5*(TMMKaptonSizeZ+TMMCopperSizeZ+TMMLayerGap));
  new G4PVPlacement(0,CopperPos1, fYReadoutVolume, "XReadoutLayer", fTMMVolume, false,0,false);
  G4ThreeVector FR4ReadoutPos1 = CopperPos1 + G4ThreeVector(0,0,0.5*(TMMCopperSizeZ+TMMFR4ReadoutSizeZ+TMMLayerGap));
  new G4PVPlacement(0,FR4ReadoutPos1, fTMMFR4ReadoutVolume, "FR4ReadoutLayerFront", fTMMVolume, false,0,false);
  G4ThreeVector CopperPos2 = FR4ReadoutPos1 + G4ThreeVector(0,0,0.5*(TMMFR4ReadoutSizeZ+TMMCopperSizeZ+TMMLayerGap));
  new G4PVPlacement(0,CopperPos2, fYReadoutVolume, "YReadoutLayer", fTMMVolume, false,0,false);
  G4ThreeVector ExternalFR4 = CopperPos2 + G4ThreeVector(0,0,0.5*(TMMFaradayFR4SizeZ +TMMFR4ReadoutSizeZ+TMMLayerGap));
  new G4PVPlacement(0,ExternalFR4, fTMMFaradayFR4Volume, "FR4plane", fTMMVolume, false,0,false);
  G4ThreeVector CopperFR4 = ExternalFR4 + G4ThreeVector(0,0,0.5*(TMMFaradayCopperSizeZ+TMMFaradayFR4SizeZ +TMMLayerGap));
  new G4PVPlacement(0, CopperFR4, fTMMFaradayCopperVolume, "CopperFR4Plane", fTMMVolume,false,0,false);
  
  // //new G4PVPlacement(0, CopperFaradayPos1, fTMMFaradayCopperVolume, "FaradayCopper", fTMMVolume,false,0,false);
  
  

  // //to be changed to FR4Readout
  // // G4ThreeVector KaptonPos1 = CopperPos2 - G4ThreeVector(0,0,0.5*(TMMCopperSizeZ+TMMKaptonSizeZ+TMMLayerGap));
  // // new G4PVPlacement(0,KaptonPos1, fTMMKaptonVolume, "KaptonLayer1", fTMMVolume, false,0,false);
  // // G4ThreeVector CopperPos1 = KaptonPos1 - G4ThreeVector(0,0,0.5*(TMMKaptonSizeZ+TMMCopperSizeZ+TMMLayerGap));
  // G4ThreeVector FR4ReadoutPos1 = CopperPos2 - G4ThreeVector(0,0,0.5*(TMMCopperSizeZ+TMMFR4ReadoutSizeZ+TMMLayerGap));
  // //new G4PVPlacement(0,FR4ReadoutPos1, fTMMFR4ReadoutVolume, "FR4ReadoutLayerFront", fTMMVolume, false,0,false);
  // G4ThreeVector CopperPos1 = FR4ReadoutPos1 - G4ThreeVector(0,0,0.5*(TMMFR4ReadoutSizeZ+TMMCopperSizeZ+TMMLayerGap));

  // //new G4PVPlacement(0,CopperPos1, fXReadoutVolume, "XReadoutLayer", fTMMVolume, false,0,false);
  // G4ThreeVector FR4Pos2 = CopperPos1 - G4ThreeVector(0,0,0.5*(TMMCopperSizeZ+TMMFR4SizeZ+TMMLayerGap));
  // //new G4PVPlacement(0,FR4Pos2, fTMMFR4Volume, "FR4Layer2", fTMMVolume, false,0,false);
  // G4ThreeVector NomexPos = FR4Pos2 - G4ThreeVector(0,0,0.5*(TMMFR4SizeZ+TMMNomexSizeZ+TMMLayerGap));
  // //new G4PVPlacement(0,NomexPos, fTMMNomexVolume, "NomexLayer", fTMMVolume, false,0,false);
  
  // G4ThreeVector FR4Pos1 = NomexPos - G4ThreeVector(0,0,0.5*(TMMNomexSizeZ+TMMFR4SizeZ+TMMLayerGap));
  // //new G4PVPlacement(0,FR4Pos1, fTMMFR4Volume, "FR4Layer1", fTMMVolume, false,0,false);
  
  // //Faraday cage positioning
  // G4ThreeVector FR4FaradayPos1 = FR4Pos1 - G4ThreeVector(0,0,0.5*(TMMFR4SizeZ + TMMFaradayFR4SizeZ+TMMLayerGap));
  // //new G4PVPlacement(0, FR4FaradayPos1, fTMMFaradayFR4Volume, "FaradayFR4", fTMMVolume,false,0,false);
  // G4ThreeVector CopperFaradayPos1 = FR4FaradayPos1 - G4ThreeVector(0,0,0.5*(TMMFaradayFR4SizeZ + TMMFaradayCopperSizeZ+TMMLayerGap));
  // //new G4PVPlacement(0, CopperFaradayPos1, fTMMFaradayCopperVolume, "FaradayCopper", fTMMVolume,false,0,false);
  
  

  // Placement of layers inside rear panel
  // G4ThreeVector CarbonPos = G4ThreeVector(0,0,+0.5*(TMMDriftSizeZ+TMMCarbonSizeZ+TMMLayerGap));
  // //new G4PVPlacement(0,CarbonPos, fTMMMylarClosing, "MylarClosing", fTMMVolume,false,1,false);
  // new G4PVPlacement(0,CarbonPos, fTMMCarbonVolume, "CarbonResistiveLayer", fTMMVolume,false,1,false);
  // G4ThreeVector KaptonPos2 = CarbonPos + G4ThreeVector(0,0,0.5*(TMMCarbonSizeZ+TMMKaptonSizeZ+TMMLayerGap));
  // new G4PVPlacement(0,KaptonPos2, fTMMKaptonVolume, "KaptonLayer2", fTMMVolume, false,1,false);
  // CopperPos2 = KaptonPos2 + G4ThreeVector(0,0,0.5*(TMMKaptonSizeZ+TMMCopperSizeZ+TMMLayerGap));
  // new G4PVPlacement(0,CopperPos2, fYReadoutVolume, "YReadoutLayer", fTMMVolume, false,1,false);
  // //to be changed to FR4Readout
  // // KaptonPos1 = CopperPos2 + G4ThreeVector(0,0,0.5*(TMMCopperSizeZ+TMMKaptonSizeZ+TMMLayerGap));
  // // new G4PVPlacement(0,KaptonPos1, fTMMKaptonVolume, "KaptonLayer1", fTMMVolume, false,1,false);
  // // CopperPos1 = KaptonPos1 + G4ThreeVector(0,0,0.5*(TMMKaptonSizeZ+TMMCopperSizeZ+TMMLayerGap));
  // FR4ReadoutPos1 = CopperPos2 + G4ThreeVector(0,0,0.5*(TMMCopperSizeZ+TMMFR4ReadoutSizeZ+TMMLayerGap));
  // new G4PVPlacement(0,FR4ReadoutPos1, fTMMFR4ReadoutVolume, "FR4ReadoutLayerRear", fTMMVolume, false,0,false);
  // CopperPos1 = FR4ReadoutPos1 + G4ThreeVector(0,0,0.5*(TMMFR4ReadoutSizeZ+TMMCopperSizeZ+TMMLayerGap));

  // new G4PVPlacement(0,CopperPos1, fXReadoutVolume, "XReadoutLayer", fTMMVolume, false,1,false);
  // FR4Pos2 = CopperPos1 + G4ThreeVector(0,0,0.5*(TMMCopperSizeZ+TMMFR4SizeZ+TMMLayerGap));
  // new G4PVPlacement(0,FR4Pos2, fTMMFR4Volume, "FR4Layer2", fTMMVolume, false,1,false);
  // NomexPos = FR4Pos2 + G4ThreeVector(0,0,0.5*(TMMFR4SizeZ+TMMNomexSizeZ+TMMLayerGap));
  // new G4PVPlacement(0,NomexPos, fTMMNomexVolume, "NomexLayer", fTMMVolume, false,1,false);
  // FR4Pos1 = NomexPos + G4ThreeVector(0,0,0.5*(TMMNomexSizeZ+TMMFR4SizeZ+TMMLayerGap));
  // new G4PVPlacement(0,FR4Pos1, fTMMFR4Volume, "FR4Layer1", fTMMVolume, false,1,false);
  // // ShieldPos = FR4Pos1 + G4ThreeVector(0,0,0.5*(TMMFR4SizeZ + TMMCopperSizeZ+TMMLayerGap));
  // // new G4PVPlacement(0, ShieldPos, fTMMCopperVolume, "CopperShield", fTMMVolume,false,1,false);
  
  // FR4FaradayPos1 = FR4Pos1 + G4ThreeVector(0,0,0.5*(TMMFR4SizeZ + TMMFaradayFR4SizeZ+TMMLayerGap));
  // new G4PVPlacement(0, FR4FaradayPos1, fTMMFaradayFR4Volume, "FaradayFR4", fTMMVolume,false,0,false);
  // CopperFaradayPos1 = FR4FaradayPos1 + G4ThreeVector(0,0,0.5*(TMMFaradayFR4SizeZ + TMMFaradayCopperSizeZ+TMMLayerGap));
  // new G4PVPlacement(0, CopperFaradayPos1, fTMMFaradayCopperVolume, "FaradayCopper", fTMMVolume,false,0,false);
  
  
  // //Definition of the Meshes 
  G4double AmpMeshSizeZ = geo->GetTMMAmpMeshSizeZ();
  G4double CathodeMeshSizeZ = geo->GetTMMCathodeMeshSizeZ();
  
  G4Box* solidAmpMeshVolume = new G4Box("solidAmpMeshVolume", 0.5*TMMPanelSizeX, 0.5*TMMPanelSizeY, 0.5*AmpMeshSizeZ);
  fAmpMeshVolume = new G4LogicalVolume(solidAmpMeshVolume, G4Material::GetMaterial("G4_STAINLESS-STEEL"), "MeshVolume",0,0,0);
  fAmpMeshVolume->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));
  G4Box* solidCathodeMeshVolume = new G4Box("solidCathodeMeshVolume", 0.5*TMMPanelSizeX, 0.5*TMMPanelSizeY, 0.5*CathodeMeshSizeZ);
  fCathodeMeshVolume = new G4LogicalVolume(solidCathodeMeshVolume, G4Material::GetMaterial("G4_STAINLESS-STEEL"), "MeshVolume",0,0,0);
  fCathodeMeshVolume->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));


  new G4PVPlacement(0, G4ThreeVector(0,0,-0.5*geo->GetTMMDriftSizeZ()), fCathodeMeshVolume, "FrontMesh", fDriftVolume,false,0,false);
  new G4PVPlacement(0, G4ThreeVector(0,0, 0.5*geo->GetTMMDriftSizeZ()), fAmpMeshVolume, "RearMesh", fDriftVolume,false,0,false);
  //new G4PVPlacement(0, G4ThreeVector(0,0,0), fCathodeMeshVolume, "CenterMesh", fDriftVolume,false,0,false);


  //Placement of the lateral panels
  //G4double xOffsetFR4 = 0.5*TMMDriftSizeX + 0.5*TMMLateralFR4;
  //G4double xOffsetBrass = 0.5*TMMDriftSizeX + TMMLateralFR4 +10*um+0.5*TMMLateralBrass;
  //left side
  // new G4PVPlacement(0, G4ThreeVector(-xOffsetFR4,0,0), fTMMLateralFR4Volume,"LeftSideLateralFR4",fTMMVolume, false,0, false); 
  // new G4PVPlacement(0, G4ThreeVector(-xOffsetBrass,0,0), fTMMLateralBrassVolume,"LeftSideLateralBrass",fTMMVolume, false,0, false); 
  // //right side
  // new G4PVPlacement(0, G4ThreeVector(xOffsetFR4,0,0), fTMMLateralFR4Volume,"RightSideLateralFR4",fTMMVolume, false,0, false); 
  // new G4PVPlacement(0, G4ThreeVector(xOffsetBrass,0,0), fTMMLateralBrassVolume,"RightSideLateralBrass",fTMMVolume, false,0, false); 
  
  // Create digitizer for TMM
  G4DigiManager* theDM = G4DigiManager::GetDMpointer();
  G4String TMMDName = geo->GetTMMDigitizerName();
  printf("Registering TMM Digitizer %s\n",TMMDName.data());
  TMMDigitizer* TMMD = new TMMDigitizer(TMMDName);
  theDM->AddNewModule(TMMD);
  if(TMMD != nullptr){printf("Creating TMMDigitizer\n");}
  

  // Make TMMDriftVolume a sensitive detector
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String TMMSDName = geo->GetTMMSensitiveDetectorName();
  printf("Registering TMM SD %s\n",TMMSDName.data());
  TMMSD* TMMsd = new TMMSD(TMMSDName);
  sdMan->AddNewDetector(TMMsd);
  fDriftVolume->SetSensitiveDetector(TMMsd); //setting only gas volume as sensitive
  printf("Setting drift volume as SD \n");

  // Computation of Radiation Length
  // G4double inverse_sum = TMMDriftSizeZ/(fDriftVolume->GetMaterial()->GetRadlen()) +
  //                       2*TMMCopperSizeZ/(fTMMCopperVolume->GetMaterial()->GetRadlen()) + 
  //                       2*TMMCarbonSizeZ/(fTMMCarbonVolume->GetMaterial()->GetRadlen()) +
  //                       4*TMMFR4SizeZ/(fTMMFR4Volume->GetMaterial()->GetRadlen()) + 
  //                       2*TMMNomexSizeZ/(fTMMNomexVolume->GetMaterial()->GetRadlen())  + 
  //                       4*TMMKaptonSizeZ/(fTMMKaptonVolume->GetMaterial()->GetRadlen()) + 
  //                       2*TMMCopperSizeZ/(fXReadoutVolume->GetMaterial()->GetRadlen()) + 
  //                       2*TMMCopperSizeZ/(fYReadoutVolume->GetMaterial()->GetRadlen()) +   
  //                       2*AmpMeshSizeZ/(fAmpMeshVolume->GetMaterial()->GetRadlen())  + 
  //                       CathodeMeshSizeZ/(fCathodeMeshVolume->GetMaterial()->GetRadlen());

  // G4double radlen_tot = ((TMMSizeZ-5*mm)/inverse_sum);
  // G4cout << "Total Radiation Length is : " << G4BestUnit(radlen_tot, "Length")<<G4endl;
  // //Total Radiation Length is : 664.797 cm


}

