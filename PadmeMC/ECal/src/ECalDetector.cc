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
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
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

  // Define colours for ECal parts
  G4Colour crystalColour(0.,1.,1.,1.); // Crystals are Cyan
  G4Colour coatingColour(0.,0.8,0.8,1.); // Crystal coating is a slightly darker Cyan
  G4Colour tedlarColour(0.,0.,1.,1.); // Tedlar slips are Blue

  G4Colour panelHoneycombColour(1.,1.,0.,1.); // Honeycomb inside panel is Yellow
  G4Colour panelGlueColour(0.8,0.8,0.,1.); // Glue between honeycomb and foils is a slightly darker Yellow
  G4Colour panelFoilColour(0.6,0.6,0.,1.); // Foils around panel are even darker Yellow

  G4Colour supportColour(0.5,0.5,0.5,1.); // ASA used for ECal support structures is Gray

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
  new G4PVPlacement(0,ecalPos,fECalVolume,"ECal",fMotherVolume,false,0,true);

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
  fCrystalVolume->SetVisAttributes(G4VisAttributes(crystalColour));

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
  fCellVolume->SetVisAttributes(G4VisAttributes(coatingColour));

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
  logicTedlarH->SetVisAttributes(G4VisAttributes(tedlarColour));

  // Create vertical Tedlar slip
  G4double tedlarVSizeX = geo->GetTedlarVSizeX();
  G4double tedlarVSizeY = geo->GetTedlarVSizeY();
  G4double tedlarVSizeZ = geo->GetTedlarVSizeZ();
  printf("ECal Tedlar vertical slip size is %f %f %f\n",tedlarVSizeX,tedlarVSizeY,tedlarVSizeZ);
  G4Box* solidTedlarV = new G4Box("ECalTedlarV",0.5*tedlarVSizeX,0.5*tedlarVSizeY,0.5*tedlarVSizeZ);
  G4LogicalVolume* logicTedlarV = new G4LogicalVolume(solidTedlarV,G4Material::GetMaterial("G4_POLYVINYLIDENE_FLUORIDE"),"ECalTedlarV",0, 0, 0);
  logicTedlarV->SetVisAttributes(G4VisAttributes(tedlarColour));

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

  // Create ASA cell for external support structure

  G4double ecalExtSuppCellSizeX = geo->GetExternalSupportCellSizeX();
  G4double ecalExtSuppCellSizeY = geo->GetExternalSupportCellSizeY();
  G4double ecalExtSuppCellSizeZ = geo->GetExternalSupportCellSizeZ();
  G4Box* solidECalExtSuppCell = new G4Box("ECalExtSuppCell",0.5*ecalExtSuppCellSizeX,0.5*ecalExtSuppCellSizeY,0.5*ecalExtSuppCellSizeZ);
  G4LogicalVolume* ecalExtSuppCellVolume = new G4LogicalVolume(solidECalExtSuppCell,G4Material::GetMaterial("ASA"),"ECalExtSuppCell",0,0,0);
  ecalExtSuppCellVolume->SetVisAttributes(G4VisAttributes(supportColour));

  // Position all cells of external support structure
  G4int nTotExtCell = 0;
  G4int nRowC = geo->GetECalNRows();
  G4int nColC = geo->GetECalNCols();
  for (G4int row=0;row<nRowC;row++){
     for (G4int col=0;col<nColC;col++){
       if (geo->ExistsExternalSupportAt(row,col)) {
	 nTotExtCell++;
	 G4int idxCell = row*ECALGEOMETRY_N_COLS_MAX+col;
	 G4ThreeVector positionCell = G4ThreeVector(geo->GetExternalSupportCellPosX(row,col),geo->GetExternalSupportCellPosY(row,col),geo-> GetExternalSupportCellPosZ(row,col));
	 new G4PVPlacement(0,positionCell,ecalExtSuppCellVolume,"ECalExtSuppCell",fECalVolume,false,idxCell,false);
       }
     }
  }
  G4cout << "ECalDetector - Positioned " << nTotExtCell << " ASA cells for external support." << G4endl;

    ///////////////////////////////////////////
   // Create Alustep panel in front of ECal //
  ///////////////////////////////////////////

  if ( geo->ECalPanelIsEnabled() ) {

    // Create main volume for Nomex panel
    G4double ecalPanelSizeX = geo->GetECalPanelSizeX();
    G4double ecalPanelSizeY = geo->GetECalPanelSizeY();
    G4double ecalPanelSizeZ = geo->GetECalPanelSizeZ();
    printf("ECal panel size is %f x %f x %f mm3\n",ecalPanelSizeX/mm,ecalPanelSizeY/mm,ecalPanelSizeZ/mm);
    G4Box* solidECalPanel = new G4Box("ECalPanel",0.5*ecalPanelSizeX,0.5*ecalPanelSizeY,0.5*ecalPanelSizeZ);
    fECalPanelVolume = new G4LogicalVolume(solidECalPanel,G4Material::GetMaterial("G4_AIR"),"ECalPanel",0,0,0);
    fECalPanelVolume->SetVisAttributes(G4VisAttributes::Invisible);

    // Create hole volume
    G4double ecalPanelHoleR = geo->GetECalPanelHoleRadius();
    printf("ECal panel hole radius is %f mm\n",ecalPanelHoleR/mm);
    G4Tubs* solidEcalPanelHole = new G4Tubs("ECalPanelHole",0.,ecalPanelHoleR,0.5*ecalPanelSizeZ,0.*deg,360.*deg);

    // Create internal honeycomb
    G4double ecalPanelHoneycombThick = geo->GetECalPanelHoneycombThickness();
    G4Box* solidEcalPanelHoneycombFull = new G4Box("ECalPanelHoneycombFull",0.5*ecalPanelSizeX,0.5*ecalPanelSizeY,0.5*ecalPanelHoneycombThick);
    G4SubtractionSolid* solidEcalPanelHoneycomb = new G4SubtractionSolid("ECalPanelHoneycomb",solidEcalPanelHoneycombFull,solidEcalPanelHole,0,G4ThreeVector(0.,0.,0.));
    G4LogicalVolume* ecalPanelHoneycombVolume = new G4LogicalVolume(solidEcalPanelHoneycomb,G4Material::GetMaterial("Nomex"),"ECalPanelHoneycomb",0,0,0);
    ecalPanelHoneycombVolume->SetVisAttributes(G4VisAttributes(panelHoneycombColour)); // Yellow

    // Create external foil
    G4double ecalPanelFoilThick = geo->GetECalPanelFoilThickness();
    G4Box* solidEcalPanelFoilFull = new G4Box("ECalPanelFoilFull",0.5*ecalPanelSizeX,0.5*ecalPanelSizeY,0.5*ecalPanelFoilThick);
    G4SubtractionSolid* solidEcalPanelFoil = new G4SubtractionSolid("ECalPanelFoil",solidEcalPanelFoilFull,solidEcalPanelHole,0,G4ThreeVector(0.,0.,0.));
    G4LogicalVolume* ecalPanelFoilVolume = new G4LogicalVolume(solidEcalPanelFoil,G4Material::GetMaterial("NomexFoil"),"ECalPanelFoil",0,0,0);
    ecalPanelFoilVolume->SetVisAttributes(G4VisAttributes(panelFoilColour));

    // Create glue layer
    G4double ecalPanelGlueThick = geo->GetECalPanelGlueThickness();
    G4Box* solidEcalPanelGlueFull = new G4Box("ECalPanelGlueFull",0.5*ecalPanelSizeX,0.5*ecalPanelSizeY,0.5*ecalPanelGlueThick);
    G4SubtractionSolid* solidEcalPanelGlue = new G4SubtractionSolid("ECalPanelGlue",solidEcalPanelGlueFull,solidEcalPanelHole,0,G4ThreeVector(0.,0.,0.));
    G4LogicalVolume* ecalPanelGlueVolume = new G4LogicalVolume(solidEcalPanelGlue,G4Material::GetMaterial("NomexFoil"),"ECalPanelGlue",0,0,0);
    ecalPanelGlueVolume->SetVisAttributes(G4VisAttributes(panelGlueColour)); // Yellow

    // Insert honeycomb inside main panel volume
    G4double ecalPanelHoneycombPosZ = 0.;
    new G4PVPlacement(0,G4ThreeVector(0.,0.,ecalPanelHoneycombPosZ),ecalPanelHoneycombVolume,"ECalPanelHoneycomb",fECalPanelVolume,false,0,false);

    // Insert glue layers inside main panel volume
    G4double ecalPanelGluePosZ = 0.5*ecalPanelHoneycombThick+0.5*ecalPanelGlueThick;
    new G4PVPlacement(0,G4ThreeVector(0.,0.,-ecalPanelGluePosZ),ecalPanelGlueVolume,"ECalPanelGlue",fECalPanelVolume,false,0,false);
    new G4PVPlacement(0,G4ThreeVector(0.,0.,ecalPanelGluePosZ),ecalPanelGlueVolume,"ECalPanelGlue",fECalPanelVolume,false,1,false);

    // Insert foils inside main panel volume
    G4double ecalPanelFoilPosZ = 0.5*ecalPanelHoneycombThick+ecalPanelGlueThick+0.5*ecalPanelFoilThick;
    new G4PVPlacement(0,G4ThreeVector(0.,0.,-ecalPanelFoilPosZ),ecalPanelFoilVolume,"ECalPanelFoil",fECalPanelVolume,false,0,false);
    new G4PVPlacement(0,G4ThreeVector(0.,0.,ecalPanelFoilPosZ),ecalPanelFoilVolume,"ECalPanelFoil",fECalPanelVolume,false,1,false);

    // Place panel in front of ECal
    G4double ecalPanelPosX = geo->GetECalPanelPosX();
    G4double ecalPanelPosY = geo->GetECalPanelPosY();
    G4double ecalPanelPosZ = geo->GetECalPanelPosZ();
    printf("ECal panel will be placed at %f %f %f\n",ecalPanelPosX,ecalPanelPosY,ecalPanelPosZ);
    new G4PVPlacement(0,G4ThreeVector(ecalPanelPosX,ecalPanelPosY,ecalPanelPosZ),fECalPanelVolume,"ECalPanel",fMotherVolume,false,0,true);

  } else {
    printf("ECal panel is disabled\n");
  }

    /////////////////////////////////////////////////
   // Create ASA support structure in central hole //
  //////////////////////////////////////////////////

  if ( geo->ECalSupportIsEnabled() ) {

    // Create main volume for support structure
    G4double ecalSupportSizeX = geo->GetECalSupportSizeX();
    G4double ecalSupportSizeY = geo->GetECalSupportSizeY();
    G4double ecalSupportSizeZ = geo->GetECalSupportSizeZ();
    printf("ECal support size is %f x %f x %f mm3\n",ecalSupportSizeX/mm,ecalSupportSizeY/mm,ecalSupportSizeZ/mm);
    G4Box* solidECalSupport = new G4Box("ECalSupport",0.5*ecalSupportSizeX,0.5*ecalSupportSizeY,0.5*ecalSupportSizeZ);

    G4LogicalVolume* ecalSupportVolume = new G4LogicalVolume(solidECalSupport,G4Material::GetMaterial("G4_AIR"),"ECalSupport",0,0,0);
    //ecalSupportVolume->SetVisAttributes(G4VisAttributes(G4Colour::Gray()));
    ecalSupportVolume->SetVisAttributes(G4VisAttributes::Invisible);

    // Create main box for support structure
    G4Box* solidECalSuppBoxFull = new G4Box("ECalSuppBoxFull",0.5*ecalSupportSizeX,0.5*ecalSupportSizeY,0.5*ecalSupportSizeZ);

    // Inside of ASA support structure is empty
    G4double ecalSuppBoxCavitySizeX = geo->GetECalSupportCavitySizeX();
    G4double ecalSuppBoxCavitySizeY = geo->GetECalSupportCavitySizeY();
    G4double ecalSuppBoxCavitySizeZ = geo->GetECalSupportCavitySizeZ();
    G4Box* solidECalSuppBoxCavity = new G4Box("ECalSuppBoxCavity",0.5*ecalSuppBoxCavitySizeX,0.5*ecalSuppBoxCavitySizeY,0.5*ecalSuppBoxCavitySizeZ);

    G4double ecalSuppBoxCavityPosX = geo->GetECalSupportCavityPosX();
    G4double ecalSuppBoxCavityPosY = geo->GetECalSupportCavityPosY();
    G4double ecalSuppBoxCavityPosZ = geo->GetECalSupportCavityPosZ();
    G4SubtractionSolid* solidECalSuppBoxEmpty1 = new G4SubtractionSolid("ECalSuppBoxEmpty1",solidECalSuppBoxFull,solidECalSuppBoxCavity,0,G4ThreeVector(ecalSuppBoxCavityPosX,ecalSuppBoxCavityPosY,ecalSuppBoxCavityPosZ));

    // Create hole volume
    G4double ecalSuppBoxHoleR = geo->GetECalSupportHoleRadius();
    //printf("ECal support hole radius is %f mm\n",ecalSupportHoleR/mm);
    G4double ecalSuppBoxFrontThick = geo->GetECalSupportFrontThickness();
    G4Tubs* solidEcalSuppBoxHole = new G4Tubs("ECalSuppBoxHole",0.,ecalSuppBoxHoleR,0.5*ecalSuppBoxFrontThick+1.*mm,0.*deg,360.*deg);

    G4double ecalSuppBoxHolePosX = geo->GetECalSupportHolePosX();
    G4double ecalSuppBoxHolePosY = geo->GetECalSupportHolePosY();
    G4double ecalSuppBoxHolePosZ = geo->GetECalSupportHolePosZ();
    G4SubtractionSolid* solidECalSuppBoxEmpty2 = new G4SubtractionSolid("ECalSuppBoxEmpty2",solidECalSuppBoxEmpty1,solidEcalSuppBoxHole,0,G4ThreeVector(ecalSuppBoxHolePosX,ecalSuppBoxHolePosY,ecalSuppBoxHolePosZ));

    G4LogicalVolume* ecalSuppBoxVolume = new G4LogicalVolume(solidECalSuppBoxEmpty2,G4Material::GetMaterial("ASA"),"ECalSuppBox",0,0,0);
    ecalSuppBoxVolume->SetVisAttributes(G4VisAttributes(supportColour));

    // Postion support empty box inside main support logical volume
    new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),ecalSuppBoxVolume,"ECalSuppBox",ecalSupportVolume,false,0,true);

    // Create lattice structures inside support

    G4int ecalSuppLattNCellXY = geo->GetECalSupportLatticeNCellXY();
    G4int ecalSuppLattNCellZ = geo->GetECalSupportLatticeNCellZ();

    G4double ecalSuppLattThick = geo->GetECalSupportLatticeThickness();
    G4double ecalSuppLattHoleWidth = geo->GetECalSupportLatticeHoleWidth();
    G4double ecalSuppLattHoleLength = geo->GetECalSupportLatticeHoleLength();
    G4double ecalSuppLattCellWidth = geo->GetECalSupportLatticeCellWidth();
    G4double ecalSuppLattCellLength = geo->GetECalSupportLatticeCellLength();

    G4Box* solidECalSuppLattXCellFull = new G4Box("ECalSuppLattXCellFull",0.5*ecalSuppLattCellWidth,0.5*ecalSuppLattThick,0.5*ecalSuppLattCellLength);
    G4Box* solidECalSuppLattXHole = new G4Box("ECalSuppLattXHole",0.5*ecalSuppLattHoleWidth,0.5*ecalSuppLattThick+1.*mm,0.5*ecalSuppLattHoleLength);
    G4SubtractionSolid* solidECalSuppLattXCell = new G4SubtractionSolid("ECalSuppLattXCell",solidECalSuppLattXCellFull,solidECalSuppLattXHole,0,G4ThreeVector(0.,0.,0.));
    G4LogicalVolume* ecalSuppLattXCellVolume = new G4LogicalVolume(solidECalSuppLattXCell,G4Material::GetMaterial("ASA"),"ECalSuppLattXCell",0,0,0);
    ecalSuppLattXCellVolume->SetVisAttributes(G4VisAttributes(supportColour));

    G4double ecalSuppLattXSizeX = geo->GetECalSupportLatticeXSizeX();
    G4double ecalSuppLattXSizeY = geo->GetECalSupportLatticeXSizeY();
    G4double ecalSuppLattXSizeZ = geo->GetECalSupportLatticeXSizeZ();
    G4Box* solidECalSuppLattX = new G4Box("ECalSuppLattX",0.5*ecalSuppLattXSizeX,0.5*ecalSuppLattXSizeY,0.5*ecalSuppLattXSizeZ);
    G4LogicalVolume* ecalSuppLattXVolume = new G4LogicalVolume(solidECalSuppLattX,G4Material::GetMaterial("G4_AIR"),"ECalSuppLattX",0,0,0);
    ecalSuppLattXVolume->SetVisAttributes(G4VisAttributes::Invisible);

    G4int xCell = 0;
    for(G4int x=0; x<ecalSuppLattNCellXY; x++) {
      for(G4int z=0; z<ecalSuppLattNCellZ; z++) {
	G4double posX = geo->GetECalSupportLatticeXCellPosX(x,z);
	G4double posY = geo->GetECalSupportLatticeXCellPosY(x,z);
	G4double posZ = geo->GetECalSupportLatticeXCellPosZ(x,z);
	new G4PVPlacement(0,G4ThreeVector(posX,posY,posZ),ecalSuppLattXCellVolume,"ECalSuppLattXCell",ecalSuppLattXVolume,false,xCell,false);
	xCell++;
      }
    }

    G4double ecalSuppLattXpPosX = geo->GetECalSupportLatticeXpPosX();
    G4double ecalSuppLattXpPosY = geo->GetECalSupportLatticeXpPosY();
    G4double ecalSuppLattXpPosZ = geo->GetECalSupportLatticeXpPosZ();
    new G4PVPlacement(0,G4ThreeVector(ecalSuppLattXpPosX,ecalSuppLattXpPosY,ecalSuppLattXpPosZ),ecalSuppLattXVolume,"ECalSuppLattXp",ecalSupportVolume,false,0,true);

    G4double ecalSuppLattXmPosX = geo->GetECalSupportLatticeXmPosX();
    G4double ecalSuppLattXmPosY = geo->GetECalSupportLatticeXmPosY();
    G4double ecalSuppLattXmPosZ = geo->GetECalSupportLatticeXmPosZ();
    new G4PVPlacement(0,G4ThreeVector(ecalSuppLattXmPosX,ecalSuppLattXmPosY,ecalSuppLattXmPosZ),ecalSuppLattXVolume,"ECalSuppLattXm",ecalSupportVolume,false,0,true);

    G4Box* solidECalSuppLattYCellFull = new G4Box("ECalSuppLattYCellFull",0.5*ecalSuppLattThick,0.5*ecalSuppLattCellWidth,0.5*ecalSuppLattCellLength);
    G4Box* solidECalSuppLattYHole = new G4Box("ECalSuppLattYHole",0.5*ecalSuppLattThick+1.*mm,0.5*ecalSuppLattHoleWidth,0.5*ecalSuppLattHoleLength);
    G4SubtractionSolid* solidECalSuppLattYCell = new G4SubtractionSolid("ECalSuppLattYCell",solidECalSuppLattYCellFull,solidECalSuppLattYHole,0,G4ThreeVector(0.,0.,0.));
    G4LogicalVolume* ecalSuppLattYCellVolume = new G4LogicalVolume(solidECalSuppLattYCell,G4Material::GetMaterial("ASA"),"ECalSuppLattYCell",0,0,0);
    ecalSuppLattYCellVolume->SetVisAttributes(G4VisAttributes(supportColour));

    G4double ecalSuppLattYSizeX = geo->GetECalSupportLatticeYSizeX();
    G4double ecalSuppLattYSizeY = geo->GetECalSupportLatticeYSizeY();
    G4double ecalSuppLattYSizeZ = geo->GetECalSupportLatticeYSizeZ();
    G4Box* solidECalSuppLattY = new G4Box("ECalSuppLattY",0.5*ecalSuppLattYSizeX,0.5*ecalSuppLattYSizeY,0.5*ecalSuppLattYSizeZ);
    G4LogicalVolume* ecalSuppLattYVolume = new G4LogicalVolume(solidECalSuppLattY,G4Material::GetMaterial("G4_AIR"),"ECalSuppLattY",0,0,0);
    ecalSuppLattYVolume->SetVisAttributes(G4VisAttributes::Invisible);

    G4int yCell = 0;
    for(G4int y=0; y<ecalSuppLattNCellXY; y++) {
      for(G4int z=0; z<ecalSuppLattNCellZ; z++) {
	G4double posX = geo->GetECalSupportLatticeYCellPosX(y,z);
	G4double posY = geo->GetECalSupportLatticeYCellPosY(y,z);
	G4double posZ = geo->GetECalSupportLatticeYCellPosZ(y,z);
	new G4PVPlacement(0,G4ThreeVector(posX,posY,posZ),ecalSuppLattYCellVolume,"ECalSuppLattYCell",ecalSuppLattYVolume,false,yCell,false);
	yCell++;
      }
    }

    G4double ecalSuppLattYpPosX = geo->GetECalSupportLatticeYpPosX();
    G4double ecalSuppLattYpPosY = geo->GetECalSupportLatticeYpPosY();
    G4double ecalSuppLattYpPosZ = geo->GetECalSupportLatticeYpPosZ();
    new G4PVPlacement(0,G4ThreeVector(ecalSuppLattYpPosX,ecalSuppLattYpPosY,ecalSuppLattYpPosZ),ecalSuppLattYVolume,"ECalSuppLattYp",ecalSupportVolume,false,0,true);

    G4double ecalSuppLattYmPosX = geo->GetECalSupportLatticeYmPosX();
    G4double ecalSuppLattYmPosY = geo->GetECalSupportLatticeYmPosY();
    G4double ecalSuppLattYmPosZ = geo->GetECalSupportLatticeYmPosZ();
    new G4PVPlacement(0,G4ThreeVector(ecalSuppLattYmPosX,ecalSuppLattYmPosY,ecalSuppLattYmPosZ),ecalSuppLattYVolume,"ECalSuppLattYm",ecalSupportVolume,false,0,true);

    // Create missing corners
    G4double ecalSuppCornerSizeX = geo->GetECalSupportCornerSizeX();
    G4double ecalSuppCornerSizeY = geo->GetECalSupportCornerSizeY();
    G4double ecalSuppCornerSizeZ = geo->GetECalSupportCornerSizeZ();
    G4Box* solidECalSuppCorner = new G4Box("ECalSuppCorner",0.5*ecalSuppCornerSizeX,0.5*ecalSuppCornerSizeY,0.5*ecalSuppCornerSizeZ);
    G4LogicalVolume* ecalSuppCornerVolume = new G4LogicalVolume(solidECalSuppCorner,G4Material::GetMaterial("ASA"),"ECalSuppCorner",0,0,0);
    ecalSuppCornerVolume->SetVisAttributes(G4VisAttributes(supportColour));

    G4double ecalSuppCornerURPosX = geo->GetECalSupportCornerURPosX();
    G4double ecalSuppCornerURPosY = geo->GetECalSupportCornerURPosY();
    G4double ecalSuppCornerURPosZ = geo->GetECalSupportCornerURPosZ();
    new G4PVPlacement(0,G4ThreeVector(ecalSuppCornerURPosX,ecalSuppCornerURPosY,ecalSuppCornerURPosZ),ecalSuppCornerVolume,"ECalSuppCornerUR",ecalSupportVolume,false,0,true);
    G4double ecalSuppCornerULPosX = geo->GetECalSupportCornerULPosX();
    G4double ecalSuppCornerULPosY = geo->GetECalSupportCornerULPosY();
    G4double ecalSuppCornerULPosZ = geo->GetECalSupportCornerULPosZ();
    new G4PVPlacement(0,G4ThreeVector(ecalSuppCornerULPosX,ecalSuppCornerULPosY,ecalSuppCornerULPosZ),ecalSuppCornerVolume,"ECalSuppCornerUL",ecalSupportVolume,false,0,true);
    G4double ecalSuppCornerDRPosX = geo->GetECalSupportCornerDRPosX();
    G4double ecalSuppCornerDRPosY = geo->GetECalSupportCornerDRPosY();
    G4double ecalSuppCornerDRPosZ = geo->GetECalSupportCornerDRPosZ();
    new G4PVPlacement(0,G4ThreeVector(ecalSuppCornerDRPosX,ecalSuppCornerDRPosY,ecalSuppCornerDRPosZ),ecalSuppCornerVolume,"ECalSuppCornerDR",ecalSupportVolume,false,0,true);
    G4double ecalSuppCornerDLPosX = geo->GetECalSupportCornerDLPosX();
    G4double ecalSuppCornerDLPosY = geo->GetECalSupportCornerDLPosY();
    G4double ecalSuppCornerDLPosZ = geo->GetECalSupportCornerDLPosZ();
    new G4PVPlacement(0,G4ThreeVector(ecalSuppCornerDLPosX,ecalSuppCornerDLPosY,ecalSuppCornerDLPosZ),ecalSuppCornerVolume,"ECalSuppCornerDL",ecalSupportVolume,false,0,true);

    // Position support structure inside calorimeter
    G4double ecalSupportPosX = geo->GetECalSupportPosX();
    G4double ecalSupportPosY = geo->GetECalSupportPosY();
    G4double ecalSupportPosZ = geo->GetECalSupportPosZ();
    printf("ECal support will be placed at %f %f %f\n",ecalSupportPosX,ecalSupportPosY,ecalSupportPosZ);
    new G4PVPlacement(0,G4ThreeVector(ecalSupportPosX,ecalSupportPosY,ecalSupportPosZ),ecalSupportVolume,"ECalSupport",fECalVolume,false,0,true);

  } else {
    printf("ECal support is disabled\n");
  }

}
