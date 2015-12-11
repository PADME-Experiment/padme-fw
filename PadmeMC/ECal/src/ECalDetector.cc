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

#include "G4Material.hh"

//#include "ECalDetectorMessenger.hh"
#include "ECalGeometryParameters.hh"
//#include "ECalMaterialParameters.hh"

ECalDetector::ECalDetector(G4LogicalVolume* MotherVolume)
{
  fMotherVolume = MotherVolume;

  // Connect to ECalDetectorMessenger to enable datacard configuration
  //fECalMessenger = new LAVDetectorMessenger(this);

  // Initialize materials used by ECal
  //ECalMaterialParameters::GetInstance();

}

ECalDetector::~ECalDetector()
{
  //delete fECalMessenger;
}

void ECalDetector::CreateGeometry()
{

  ECalGeometryParameters* geo = ECalGeometryParameters::GetInstance();

  // Create main ECal box
  G4ThreeVector positionEcal = G4ThreeVector(geo->GetECalPosX(),geo->GetECalPosY(),geo->GetECalPosZ); 
  G4double ECalX      = geo->GetECalSizeX();
  G4double ECalY      = geo->GetECalSizeY();
  G4double ECalLength = geo->GetECalSizeZ();

  solidEcal = new G4Box("ECalSolid",ECalX*0.5,ECalY*0.5,ECalLength*0.5);
  logicEcal = new G4LogicalVolume(solidEcal,G4Material::GetMaterial("G4_Galactic"),"ECalLogic",0, 0, 0);
  physiEcal  = new G4PVPlacement(0,             // no rotation
				 positionEcal,  // at (x,y,z)
				 logicEcal,     // its logical volume
				 "ECal",        // its name
				 fMotherVolume, // its mother  volume
				 false,         // no boolean operations
				 0,             // copy number 
				 false);        // check for overlaps 

  // Create standard BGO crystal
  G4double ECryX      = geo->GetCrystalSizeX();
  G4double ECryY      = geo->GetCrystalSizeY();
  G4double ECryLength = geo->GetCrystalSizeZ();
  solidCry  = new G4Box("Ecry",ECryX*0.5,ECryY*0.5,ECryLength*0.5);
  logicCry  = new G4LogicalVolume(solidCry,G4Material::GetMaterial("G4_BGO"),"ECry",0, 0, 0);

  // Get number of rows and columns of crystals and position all crystals
  G4Int nRow = geo->GetECalNRows();
  G4Int nCol = geo->GetECalNColumns();
  for (G4int row=0;row<nRow;row++){
     for (G4int col=0;col<nCol;col++){
       if (geo->ExistsCrystalAt(row,col)) {
	 G4Int idxCry = col*nRow+row;
	 G4ThreeVector positionCry = G4ThreeVector(geo->GetCrystalPosX(row,col),geo->GetCrystalPosY(row,col),geo->GetCrystalPosZ(row,col));
	 physiCry  = new G4PVPlacement(0,           // no rotation
				       positionCry, // at (x,y,z)
				       logicCry,    // its logical volume                                  
				       "ECry",      // its name
				       logicEcal,   // its mother  volume
				       false,       // no boolean operations
				       idxCry,      // copy number 
				       false);      // check for overlaps
       }
     }
  }

}
