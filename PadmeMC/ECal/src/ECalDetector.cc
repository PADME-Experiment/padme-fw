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

}
