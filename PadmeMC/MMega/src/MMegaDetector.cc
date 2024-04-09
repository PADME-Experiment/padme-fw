
// MMegaDetector.cc
// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
// --------------------------------------------------------------

#include "MMegaDetector.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4VisAttributes.hh"
#include "G4DigiManager.hh"
#include "G4SubtractionSolid.hh"
#include "G4NistManager.hh"

#include "MMegaGeometry.hh"
#include "MMegaSD.hh"
#include "MMegaDigitizer.hh"



MMegaDetector::MMegaDetector(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{

  // Connect to MMegaMessenger to enable datacard configuration
  fMMegaMessenger = new MMegaMessenger(this);

}

MMegaDetector::~MMegaDetector()
{
  delete fMMegaMessenger;
}

// IMPLEMENTS the MMega geometry

void MMegaDetector::CreateGeometry()
{

  MMegaGeometry* geo = MMegaGeometry::GetInstance();
  
  // CREATE MMEGA CONTAINER
  printf("MMega be placed at %f %f %f\n",geo->GetMMegaPosX(),geo->GetMMegaPosY(),geo->GetMMegaPosZ());
  G4ThreeVector MMegaPos = G4ThreeVector(geo->GetMMegaPosX(),geo->GetMMegaPosY(),geo->GetMMegaPosZ()); 
  G4double MMegaSizeX = geo->GetMMegaSizeX();
  G4double MMegaSizeY = geo->GetMMegaSizeY();
  G4double MMegaSizeZ = geo->GetMMegaSizeZ();

  printf("MMega size is %f %f %f\n",MMegaSizeX,MMegaSizeY,MMegaSizeZ);
  G4Box* solidMMegaFull = new G4Box("solidMMegaFull",0.5*MMegaSizeX,0.5*MMegaSizeY,0.5*MMegaSizeZ);
  fMMegaVolume = new G4LogicalVolume(solidMMegaFull,G4Material::GetMaterial("Vacuum"),"MMegaLogic",0,0,0);
  fMMegaVolume->SetVisAttributes(G4VisAttributes(G4Colour::Green()));
  fMMegaVolume->SetVisAttributes(G4VisAttributes::Invisible);
  new G4PVPlacement(0,MMegaPos,fMMegaVolume,"MMega",fMotherVolume,false,0,false);

  //GAS VOLUME
  G4double MMegaDriftSizeX = geo->GetMMegaDriftSizeX(); 
  G4double MMegaDriftSizeY = geo->GetMMegaDriftSizeY(); 
  G4double MMegaDriftSizeZ = geo->GetMMegaDriftSizeZ() + 2*(geo->GetMMegaAmpGapSizeZ());

  printf("MMega Gas Volume size is %f %f %f\n",MMegaDriftSizeX,MMegaDriftSizeY,MMegaDriftSizeZ);
  G4Box* solidMMegaDrift = new G4Box("solidMMegaDrift", 0.5*MMegaDriftSizeX, 0.5*MMegaDriftSizeY, 0.5*MMegaDriftSizeZ);
  printf("GasDensity: %f\n", G4Material::GetMaterial("ArCF4Iso")->GetDensity() / (g/cm3));
  fDriftVolume = new G4LogicalVolume(solidMMegaDrift, G4Material::GetMaterial("ArCF4Iso"), "MMegaDrift",0,0,0);
  fDriftVolume->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
  new G4PVPlacement(0,G4ThreeVector(0,0,0),fDriftVolume,"MMegaDrift",fMMegaVolume,false,0,false);

  //READOUT PLANES
  G4double MMegaPanelSizeX = geo->GetMMegaPanelSizeX();
  G4double MMegaPanelSizeY = geo->GetMMegaPanelSizeY();
  G4double MMegaPanelSizeZ = geo->GetMMegaPanelSizeZ();

  //define layers of planes
  G4double MMegaCopperSizeZ = geo->GetMMegaCopperSizeZ();
  G4Box* solidCopperVolume = new G4Box("solidCopperVolume", 0.5*MMegaPanelSizeX, 0.5*MMegaPanelSizeY, 0.5*MMegaCopperSizeZ);
  G4LogicalVolume* fMMegaCopperVolume = new G4LogicalVolume(solidCopperVolume, G4Material::GetMaterial("G4_Cu"), "CopperShieldVolume",0,0,0);
  fMMegaCopperVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));

  //Effective densities
  G4double CopperDensity = G4Material::GetMaterial("G4_Cu")->GetDensity() / (g/cm3);
  G4double XEffDensity, YEffDensity;

  if(fReadoutType == "strips"){
    printf("MMega Readout Type : strips\n");
    XEffDensity = CopperDensity * (geo->GetMMegaStripWidth()/geo->GetMMegaStripPitch()); //strip case
    YEffDensity = CopperDensity * (geo->GetMMegaStripWidth()/geo->GetMMegaStripPitch()); //strip case
  }

  if(fReadoutType == "pads"){
    printf("MMega Readout Type : pads\n");
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

  G4double MMegaFreonSizeZ = geo->GetMMegaFreonSizeZ();
  G4Box* solidFreonVolume = new G4Box("solidFreonVolume", 0.5*MMegaPanelSizeX, 0.5*MMegaPanelSizeY, 0.5*MMegaFreonSizeZ);
  G4LogicalVolume* fMMegaFreonVolume = new G4LogicalVolume(solidFreonVolume, G4Material::GetMaterial("PCB"), "FreonVolume", 0,0,0);
  fMMegaFreonVolume->SetVisAttributes(G4VisAttributes(G4Colour::Green()));

  G4double MMegaKaptonSizeZ = geo->GetMMegaKaptonSizeZ();
  G4Box* solidKaptonVolume = new G4Box("solidKaptonVolume", 0.5*MMegaPanelSizeX, 0.5*MMegaPanelSizeY, 0.5*MMegaKaptonSizeZ);
  G4LogicalVolume* fMMegaKaptonVolume = new G4LogicalVolume(solidKaptonVolume, G4Material::GetMaterial("Kapton"), "KaptonVolume",0,0,0);
  fMMegaKaptonVolume->SetVisAttributes(G4VisAttributes(G4Colour::Brown()));

  G4double MMegaNomexSizeZ = geo->GetMMegaNomexSizeZ();
  G4Box* solidNomexVolume = new G4Box("solidNomexVolume", 0.5*MMegaPanelSizeX, 0.5*MMegaPanelSizeY, 0.5*MMegaNomexSizeZ);
  G4LogicalVolume* fMMegaNomexVolume = new G4LogicalVolume(solidNomexVolume, G4Material::GetMaterial("Nomex"), "NomexVolume", 0,0,0);
  fMMegaNomexVolume->SetVisAttributes(G4VisAttributes(G4Colour::Magenta()));

  G4double MMegaCarbonSizeZ = geo->GetMMegaCarbonSizeZ();
  G4Box* solidCarbonVolume = new G4Box("solidCarbonVolume", 0.5*MMegaPanelSizeX, 0.5*MMegaPanelSizeY, 0.5*MMegaCarbonSizeZ);
  G4LogicalVolume* fMMegaCarbonVolume = new G4LogicalVolume(solidCarbonVolume, G4Material::GetMaterial("G4_C"), "CarbonVolume", 0,0,0);
  fMMegaCarbonVolume->SetVisAttributes(G4VisAttributes(G4Colour::Cyan()));

  
  // Placement of layers inside front panel
  //maybe put this into a function
  G4double MMegaLayerGap = geo->GetMMegaLayerGap();

  G4ThreeVector CarbonPos = G4ThreeVector(0,0,-0.5*(MMegaDriftSizeZ+MMegaCarbonSizeZ+MMegaLayerGap));
  new G4PVPlacement(0,CarbonPos, fMMegaCarbonVolume, "CarbonResistiveLayer", fMMegaVolume,false,0,false);
  G4ThreeVector KaptonPos2 = CarbonPos - G4ThreeVector(0,0,0.5*(MMegaCarbonSizeZ+MMegaKaptonSizeZ+MMegaLayerGap));
  new G4PVPlacement(0,KaptonPos2, fMMegaKaptonVolume, "KaptonLayer2", fMMegaVolume, false,0,false);
  G4ThreeVector CopperPos2 = KaptonPos2 - G4ThreeVector(0,0,0.5*(MMegaKaptonSizeZ+MMegaCopperSizeZ+MMegaLayerGap));
  new G4PVPlacement(0,CopperPos2, fXReadoutVolume, "XReadoutLayer", fMMegaVolume, false,0,false);
  G4ThreeVector KaptonPos1 = CopperPos2 - G4ThreeVector(0,0,0.5*(MMegaCopperSizeZ+MMegaKaptonSizeZ+MMegaLayerGap));
  new G4PVPlacement(0,KaptonPos1, fMMegaKaptonVolume, "KaptonLayer1", fMMegaVolume, false,0,false);
  G4ThreeVector CopperPos1 = KaptonPos1 - G4ThreeVector(0,0,0.5*(MMegaKaptonSizeZ+MMegaCopperSizeZ+MMegaLayerGap));
  new G4PVPlacement(0,CopperPos1, fYReadoutVolume, "YReadoutLayer", fMMegaVolume, false,0,false);
  G4ThreeVector FreonPos2 = CopperPos1 - G4ThreeVector(0,0,0.5*(MMegaCopperSizeZ+MMegaFreonSizeZ+MMegaLayerGap));
  new G4PVPlacement(0,FreonPos2, fMMegaFreonVolume, "FreonLayer2", fMMegaVolume, false,0,false);
  G4ThreeVector NomexPos = FreonPos2 - G4ThreeVector(0,0,0.5*(MMegaFreonSizeZ+MMegaNomexSizeZ+MMegaLayerGap));
  new G4PVPlacement(0,NomexPos, fMMegaNomexVolume, "NomexLayer", fMMegaVolume, false,0,false);
  G4ThreeVector FreonPos1 = NomexPos - G4ThreeVector(0,0,0.5*(MMegaNomexSizeZ+MMegaFreonSizeZ+MMegaLayerGap));
  new G4PVPlacement(0,FreonPos1, fMMegaFreonVolume, "FreonLayer1", fMMegaVolume, false,0,false);
  G4ThreeVector ShieldPos = FreonPos1 - G4ThreeVector(0,0,0.5*(MMegaFreonSizeZ + MMegaCopperSizeZ+MMegaLayerGap));
  new G4PVPlacement(0, ShieldPos, fMMegaCopperVolume, "CopperShield", fMMegaVolume,false,0,false);
  
  

  // Placement of layers inside rear panel
  CarbonPos = G4ThreeVector(0,0,+0.5*(MMegaDriftSizeZ+MMegaCarbonSizeZ+MMegaLayerGap));
  new G4PVPlacement(0,CarbonPos, fMMegaCarbonVolume, "CarbonResistiveLayer", fMMegaVolume,false,1,false);
  KaptonPos2 = CarbonPos + G4ThreeVector(0,0,0.5*(MMegaCarbonSizeZ+MMegaKaptonSizeZ+MMegaLayerGap));
  new G4PVPlacement(0,KaptonPos2, fMMegaKaptonVolume, "KaptonLayer2", fMMegaVolume, false,1,false);
  CopperPos2 = KaptonPos2 + G4ThreeVector(0,0,0.5*(MMegaKaptonSizeZ+MMegaCopperSizeZ+MMegaLayerGap));
  new G4PVPlacement(0,CopperPos2, fXReadoutVolume, "XReadoutLayer", fMMegaVolume, false,1,false);
  KaptonPos1 = CopperPos2 + G4ThreeVector(0,0,0.5*(MMegaCopperSizeZ+MMegaKaptonSizeZ+MMegaLayerGap));
  new G4PVPlacement(0,KaptonPos1, fMMegaKaptonVolume, "KaptonLayer1", fMMegaVolume, false,1,false);
  CopperPos1 = KaptonPos1 + G4ThreeVector(0,0,0.5*(MMegaKaptonSizeZ+MMegaCopperSizeZ+MMegaLayerGap));
  new G4PVPlacement(0,CopperPos1, fYReadoutVolume, "YReadoutLayer", fMMegaVolume, false,1,false);
  FreonPos2 = CopperPos1 + G4ThreeVector(0,0,0.5*(MMegaCopperSizeZ+MMegaFreonSizeZ+MMegaLayerGap));
  new G4PVPlacement(0,FreonPos2, fMMegaFreonVolume, "FreonLayer2", fMMegaVolume, false,1,false);
  NomexPos = FreonPos2 + G4ThreeVector(0,0,0.5*(MMegaFreonSizeZ+MMegaNomexSizeZ+MMegaLayerGap));
  new G4PVPlacement(0,NomexPos, fMMegaNomexVolume, "NomexLayer", fMMegaVolume, false,1,false);
  FreonPos1 = NomexPos + G4ThreeVector(0,0,0.5*(MMegaNomexSizeZ+MMegaFreonSizeZ+MMegaLayerGap));
  new G4PVPlacement(0,FreonPos1, fMMegaFreonVolume, "FreonLayer1", fMMegaVolume, false,1,false);
  ShieldPos = FreonPos1 + G4ThreeVector(0,0,0.5*(MMegaFreonSizeZ + MMegaCopperSizeZ+MMegaLayerGap));
  new G4PVPlacement(0, ShieldPos, fMMegaCopperVolume, "CopperShield", fMMegaVolume,false,1,false);
  

  
  
  //Definition of the Meshes 
  G4double AmpMeshSizeZ = geo->GetMMegaAmpMeshSizeZ();
  G4double CathodeMeshSizeZ = geo->GetMMegaCathodeMeshSizeZ();
  
  G4Box* solidAmpMeshVolume = new G4Box("solidAmpMeshVolume", 0.5*MMegaPanelSizeX, 0.5*MMegaPanelSizeY, 0.5*AmpMeshSizeZ);
  fAmpMeshVolume = new G4LogicalVolume(solidAmpMeshVolume, G4Material::GetMaterial("G4_STAINLESS-STEEL"), "MeshVolume",0,0,0);
  fAmpMeshVolume->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));
  G4Box* solidCathodeMeshVolume = new G4Box("solidCathodeMeshVolume", 0.5*MMegaPanelSizeX, 0.5*MMegaPanelSizeY, 0.5*CathodeMeshSizeZ);
  fCathodeMeshVolume = new G4LogicalVolume(solidCathodeMeshVolume, G4Material::GetMaterial("G4_STAINLESS-STEEL"), "MeshVolume",0,0,0);
  fCathodeMeshVolume->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));


  new G4PVPlacement(0, G4ThreeVector(0,0,-0.5*geo->GetMMegaDriftSizeZ()), fAmpMeshVolume, "FrontMesh", fDriftVolume,false,0,false);
  new G4PVPlacement(0, G4ThreeVector(0,0, 0.5*geo->GetMMegaDriftSizeZ()), fAmpMeshVolume, "RearMesh", fDriftVolume,false,0,false);
  new G4PVPlacement(0, G4ThreeVector(0,0,0), fCathodeMeshVolume, "CenterMesh", fDriftVolume,false,0,false);

  // // Create digitizer for MMega
  // G4DigiManager* theDM = G4DigiManager::GetDMpointer();
  // G4String MMegaDName = geo->GetMMegaDigitizerName();
  // printf("Registering MMega Digitizer %s\n",MMegaDName.data());
  // MMegaDigitizer* MMegaD = new MMegaDigitizer(MMegaDName);
  // theDM->AddNewModule(MMegaD);
  // if(MMegaD != nullptr){printf("Creating MMegaDigitizer\n");}
  

  // // Make whole MMega a sensitive detector

  // G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  // G4String mMegaSDName = geo->GetMMegaSensitiveDetectorName();
  // printf("Registering MMega SD %s\n",mMegaSDName.data());
  // MMegaSD* mMegaSD = new MMegaSD(mMegaSDName);
  // sdMan->AddNewDetector(mMegaSD);
  // fDriftVolume->SetSensitiveDetector(mMegaSD); //setting only gas volume as sensitive
  // printf("Setting drift volume as SD \n");
}

