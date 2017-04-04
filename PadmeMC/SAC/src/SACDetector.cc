// SACDetector.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#include "SACDetector.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Box.hh"
#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"

#include "SACMessenger.hh"
#include "SACGeometry.hh"
#include "SACSD.hh"

SACDetector::SACDetector(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{

  // Connect to SACMessenger to enable datacard configuration
  fSACMessenger = new SACMessenger(this);

}

SACDetector::~SACDetector()
{
  delete fSACMessenger;
}

void SACDetector::CreateGeometry()
{

  SACGeometry* geo = SACGeometry::GetInstance();

  // Create main SAC box
  printf("SAC will be placed at %f %f %f\n",geo->GetSACPosX(),geo->GetSACPosY(),geo->GetSACPosZ());
  G4ThreeVector sacPos = G4ThreeVector(geo->GetSACPosX(),geo->GetSACPosY(),geo->GetSACPosZ()); 
  G4double sacSizeX = geo->GetSACSizeX();
  G4double sacSizeY = geo->GetSACSizeY();
  G4double sacSizeZ = geo->GetSACSizeZ();
  printf("SAC size is %f %f %f\n",sacSizeX,sacSizeY,sacSizeZ);
  G4Box* solidSAC = new G4Box("SAC",0.5*sacSizeX,0.5*sacSizeY,0.5*sacSizeZ);
  fSACVolume = new G4LogicalVolume(solidSAC,G4Material::GetMaterial("Vacuum"),"SAC",0,0,0);
  fSACVolume->SetVisAttributes(G4VisAttributes::Invisible);
  new G4PVPlacement(0,sacPos,fSACVolume,"SAC",fMotherVolume,false,0,false);

  // Show size of gap between crystals
  printf("Gap between SAC crystals is %f\n",geo->GetCrystalGap());

  // Show thickness of coating around crystals
  printf("Coating around SAC crystals is %f\n",geo->GetCrystalCoating());

  // Create standard SF57 crystal
  G4double crySizeX = geo->GetCrystalSizeX();
  G4double crySizeY = geo->GetCrystalSizeY();
  G4double crySizeZ = geo->GetCrystalSizeZ();
  printf("SAC Crystal size is %f %f %f\n",crySizeX,crySizeY,crySizeZ);
  G4Box* solidCry  = new G4Box("SACCry",0.5*crySizeX,0.5*crySizeY,0.5*crySizeZ);
  //  fCrystalVolume  = new G4LogicalVolume(solidCry,G4Material::GetMaterial("G4_BARIUM_FLUORIDE"),"SACCry",0,0,0);
  fCrystalVolume  = new G4LogicalVolume(solidCry,G4Material::GetMaterial("PbGl_SF57"),"SACCry",0,0,0);
  fCrystalVolume->SetVisAttributes(G4VisAttributes(G4Colour::Magenta()));

  // Make crystal a sensitive detector
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String sacSDName = geo->GetSACSensitiveDetectorName();
  printf("Registering SAC SD %s\n",sacSDName.data());
  SACSD* sacSD = new SACSD(sacSDName);
  sdMan->AddNewDetector(sacSD);
  fCrystalVolume->SetSensitiveDetector(sacSD);

  // Create ECal cell (BGO crystal+coating)
  G4double cellSizeX = geo->GetCellSizeX();
  G4double cellSizeY = geo->GetCellSizeY();
  G4double cellSizeZ = geo->GetCellSizeZ();
  printf("SAC cell size is %f %f %f\n",cellSizeX,cellSizeY,cellSizeZ);
  G4Box* solidCell  = new G4Box("SACCell",0.5*cellSizeX,0.5*cellSizeY,0.5*cellSizeZ);
  fCellVolume  = new G4LogicalVolume(solidCell,G4Material::GetMaterial("EJ510Paint"),"SACCell",0, 0, 0);
  fCellVolume->SetVisAttributes(G4VisAttributes(G4Colour::Magenta()));

  // Position SAc crystal inside cell
  new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),fCrystalVolume,"SACCry",fCellVolume,false,0,false);

  // Get number of rows and columns of crystals and position all crystals
  G4int nRow = geo->GetSACNRows();
  G4int nCol = geo->GetSACNCols();
  for (G4int row=0;row<nRow;row++){
    for (G4int col=0;col<nCol;col++){
      if (geo->ExistsCrystalAt(row,col)) {
	G4ThreeVector positionCry = G4ThreeVector(geo->GetCrystalPosX(row,col),geo->GetCrystalPosY(row,col),geo->GetCrystalPosZ(row,col));
	G4int idxCell = row*SACGEOMETRY_N_COLS_MAX+col;
	new G4PVPlacement(0,positionCry,fCellVolume,"SACCell",fSACVolume,false,idxCell,false);
      }
    }
  }

}
