// ECalDetector.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#include "ECalDetector.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Box.hh"
#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"

#include "ECalMessenger.hh"
#include "ECalGeometry.hh"
#include "ECalSD.hh"

ECalDetector::ECalDetector(G4LogicalVolume* motherVolume)
{

  fMotherVolume = motherVolume;

  // Connect to ECalDetectorMessenger to enable datacard configuration
  fECalMessenger = new ECalMessenger(this);

}

ECalDetector::~ECalDetector()
{
  delete fECalMessenger;
}

void ECalDetector::CreateGeometry()
{

  ECalGeometry* geo = ECalGeometry::GetInstance();

  // Create main ECal box
  printf("ECal will be placed at %f %f %f\n",geo->GetECalPosX(),geo->GetECalPosY(),geo->GetECalPosZ());
  G4ThreeVector ecalPos = G4ThreeVector(geo->GetECalPosX(),geo->GetECalPosY(),geo->GetECalPosZ()); 
  G4double ecalSizeX = geo->GetECalSizeX();
  G4double ecalSizeY = geo->GetECalSizeY();
  G4double ecalSizeZ = geo->GetECalSizeZ();
  printf("ECal size is %f %f %f\n",ecalSizeX,ecalSizeY,ecalSizeZ);
  G4Box* solidEcal = new G4Box("ECal",0.5*ecalSizeX,0.5*ecalSizeY,0.5*ecalSizeZ);
  fECalVolume = new G4LogicalVolume(solidEcal,G4Material::GetMaterial("Vacuum"),"ECal",0,0,0);
  fECalVolume->SetVisAttributes(G4VisAttributes::Invisible);
  new G4PVPlacement(0,ecalPos,fECalVolume,"ECal",fMotherVolume,false,0,false);

  // Show size of gap between crystals
  printf("Gap between crystals is %f\n",geo->GetCrystalGap());

  // Show size of gap between crystals
  printf("Coating around crystals is %f\n",geo->GetCrystalCoating());

  // Create standard BGO crystal
  G4double crySizeX = geo->GetCrystalSizeX();
  G4double crySizeY = geo->GetCrystalSizeY();
  G4double crySizeZ = geo->GetCrystalSizeZ();
  printf("Crystal size is %f %f %f\n",crySizeX,crySizeY,crySizeZ);
  G4Box* solidCry  = new G4Box("ECalCry",0.5*crySizeX,0.5*crySizeY,0.5*crySizeZ);
  fCrystalVolume  = new G4LogicalVolume(solidCry,G4Material::GetMaterial("G4_BGO"),"ECalCry",0, 0, 0);
  //fCrystalVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Magenta()));
  fCrystalVolume->SetVisAttributes(G4VisAttributes(G4Colour::Cyan()));

  // Make crystal a sensitive detector
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String ecalSDName = geo->GetECalSensitiveDetectorName();
  printf("Registering ECal SD %s\n",ecalSDName.data());
  ECalSD* ecalSD = new ECalSD(ecalSDName);
  sdMan->AddNewDetector(ecalSD);
  fCrystalVolume->SetSensitiveDetector(ecalSD);

  // Create ECal cell (BGO crystal+coating)
  G4double cellSizeX = geo->GetCellSizeX();
  G4double cellSizeY = geo->GetCellSizeY();
  G4double cellSizeZ = geo->GetCellSizeZ();
  printf("ECal cell size is %f %f %f\n",cellSizeX,cellSizeY,cellSizeZ);
  G4Box* solidCell  = new G4Box("ECalCell",0.5*cellSizeX,0.5*cellSizeY,0.5*cellSizeZ);
  fCellVolume  = new G4LogicalVolume(solidCell,G4Material::GetMaterial("EJ510Paint"),"ECalCell",0, 0, 0);
  fCellVolume->SetVisAttributes(G4VisAttributes(G4Colour::Cyan()));

  // Position BGO crystal inside cell
  new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),fCrystalVolume,"ECalCry",fCellVolume,false,0,false);

  // Verify if Tedlar slip thickness is compatible with gap between crystals
  if (geo->GetCrystalGap() <= geo->GetTedlarThickness()) {
    printf("ECalDetector::CreateGeometry - ERROR - Gap between crystals of %f is smaller than Tedlar slip thickness of %f\n",geo->GetCrystalGap(),geo->GetTedlarThickness());
  } else {
    printf("Tedlar slips thickness is %f\n",geo->GetTedlarThickness());
  }

  // Create horizontal Tedlar slip
  G4double tedlarHSizeX = geo->GetTedlarHSizeX();
  G4double tedlarHSizeY = geo->GetTedlarHSizeY();
  G4double tedlarHSizeZ = geo->GetTedlarHSizeZ();
  printf("ECal Tedlar horizontal slip size is %f %f %f\n",tedlarHSizeX,tedlarHSizeY,tedlarHSizeZ);
  G4Box* solidTedlarH = new G4Box("ECalTedlarH",0.5*tedlarHSizeX,0.5*tedlarHSizeY,0.5*tedlarHSizeZ);
  G4LogicalVolume* logicTedlarH = new G4LogicalVolume(solidTedlarH,G4Material::GetMaterial("G4_POLYVINYLIDENE_FLUORIDE"),"ECalTedlarH",0, 0, 0);
  logicTedlarH->SetVisAttributes(G4VisAttributes(G4Colour::Red()));

  // Create vertical Tedlar slip
  G4double tedlarVSizeX = geo->GetTedlarVSizeX();
  G4double tedlarVSizeY = geo->GetTedlarVSizeY();
  G4double tedlarVSizeZ = geo->GetTedlarVSizeZ();
  printf("ECal Tedlar vertical slip size is %f %f %f\n",tedlarVSizeX,tedlarVSizeY,tedlarVSizeZ);
  G4Box* solidTedlarV = new G4Box("ECalTedlarV",0.5*tedlarVSizeX,0.5*tedlarVSizeY,0.5*tedlarVSizeZ);
  G4LogicalVolume* logicTedlarV = new G4LogicalVolume(solidTedlarV,G4Material::GetMaterial("G4_POLYVINYLIDENE_FLUORIDE"),"ECalTedlarV",0, 0, 0);
  logicTedlarV->SetVisAttributes(G4VisAttributes(G4Colour::Red()));

  // Get number of rows and columns of crystals and position all crystals
  G4int nTotCry = 0;
  G4int nRow = geo->GetECalNRows();
  G4int nCol = geo->GetECalNCols();
  for (G4int row=0;row<nRow;row++){
     for (G4int col=0;col<nCol;col++){
       if (geo->ExistsCrystalAt(row,col)) {
	 nTotCry++;
	 G4int idxCell = row*ECALGEOMETRY_N_COLS_MAX+col;
	 G4ThreeVector positionCry = G4ThreeVector(geo->GetCrystalPosX(row,col),geo->GetCrystalPosY(row,col),geo->GetCrystalPosZ(row,col));
	 //new G4PVPlacement(0,positionCry,fCrystalVolume,"ECalCry",fECalVolume,false,idxCry,false);
	 new G4PVPlacement(0,positionCry,fCellVolume,"ECalCell",fECalVolume,false,idxCell,false);
       }
     }
  }
  G4cout << "ECalDetector - Positioned " << nTotCry << " crystals." << G4endl;

  // Position all tedlar slips
  G4int nTotTedlarH = 0;
  G4int nTotTedlarV = 0;
  for (G4int row=0;row<nRow;row++){
     for (G4int col=0;col<nCol;col++){
       G4int idxCell = row*ECALGEOMETRY_N_COLS_MAX+col;
       if (geo->ExistsTedlarHAt(row,col)) {
	 nTotTedlarH++;
	 G4ThreeVector positionTedlarH = G4ThreeVector(geo->GetTedlarHPosX(row,col),geo->GetTedlarHPosY(row,col),geo->GetTedlarHPosZ(row,col));
	 new G4PVPlacement(0,positionTedlarH,logicTedlarH,"ECalTedlarH",fECalVolume,false,idxCell,false);
       }
       if (geo->ExistsTedlarVAt(row,col)) {
	 nTotTedlarV++;
	 G4ThreeVector positionTedlarV = G4ThreeVector(geo->GetTedlarVPosX(row,col),geo->GetTedlarVPosY(row,col),geo->GetTedlarVPosZ(row,col));
	 new G4PVPlacement(0,positionTedlarV,logicTedlarV,"ECalTedlarV",fECalVolume,false,idxCell,false);
       }
     }
  }
  G4cout << "ECalDetector - Positioned " << nTotTedlarH << " horizontal and " << nTotTedlarV << " vertical Tedlar slips." << G4endl;

  // Create plastic panel in front of ECal
  if ( geo->ECalPanelIsEnabled() ) {
    printf("ECal panel will be placed at %f %f %f\n",geo->GetECalPanelPosX(),geo->GetECalPanelPosY(),geo->GetECalPanelPosZ());
    G4ThreeVector ecalPanelPos = G4ThreeVector(geo->GetECalPanelPosX(),geo->GetECalPanelPosY(),geo->GetECalPanelPosZ()); 
    G4double ecalPanelSizeX = geo->GetECalPanelSizeX();
    G4double ecalPanelSizeY = geo->GetECalPanelSizeY();
    G4double ecalPanelSizeZ = geo->GetECalPanelSizeZ();
    printf("ECal panel size is %f %f %f\n",ecalPanelSizeX,ecalPanelSizeY,ecalPanelSizeZ);
    G4Box* solidEcalPanel = new G4Box("ECalPanel",0.5*ecalPanelSizeX,0.5*ecalPanelSizeY,0.5*ecalPanelSizeZ);
    fECalPanelVolume = new G4LogicalVolume(solidEcalPanel,G4Material::GetMaterial("G4_PLEXIGLASS"),"ECalPanel",0,0,0);
    fECalPanelVolume->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));
    new G4PVPlacement(0,ecalPanelPos,fECalPanelVolume,"ECalPanel",fMotherVolume,false,0,false);
  } else {
    printf("ECal panel is disabled\n");
  }

}
