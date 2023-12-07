// TPixDetector.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#include "TPixDetector.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"

#include "TPixGeometry.hh"
#include "TPixSD.hh"
#include "TPixDigitizer.hh"

TPixDetector::TPixDetector(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{

  // Connect to TPixMessenger to enable datacard configuration
  fTPixMessenger = new TPixMessenger(this);

}

TPixDetector::~TPixDetector()
{
  delete fTPixMessenger;
}

void TPixDetector::CreateGeometry()
{

  TPixGeometry* geo = TPixGeometry::GetInstance();

  G4Colour colourCu = G4Colour::Red();
  G4Colour colourBox = G4Colour::Grey();
  G4Colour colourTimePix = G4Colour::Blue();
  G4Colour colourPCB = G4Colour::Green();

  // Create main TPix box
  G4double boxSizeX = geo->GetBoxSizeX();
  G4double boxSizeY = geo->GetBoxSizeY();
  G4double boxSizeZ = geo->GetBoxSizeZ();
  printf("TPix Box size is %.2f x %.2f x %.2f mm3\n",boxSizeX/mm,boxSizeY/mm,boxSizeZ/mm);
  G4Box* solidBox = new G4Box("TPixBoxSolid",0.5*boxSizeX,0.5*boxSizeY,0.5*boxSizeZ);
  fTPixBoxVolume = new G4LogicalVolume(solidBox,G4Material::GetMaterial("Air"),"TPixBoxLogic",0,0,0);
  //fTPixVolume->SetVisAttributes(G4VisAttributes::Invisible);
  //fTPixBoxVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  fTPixBoxVolume->SetVisAttributes(G4VisAttributes(colourBox));

  G4double boxPosX = geo->GetBoxPosX();
  G4double boxPosY = geo->GetBoxPosY();
  G4double boxPosZ = geo->GetBoxPosZ();
  G4ThreeVector posBox = G4ThreeVector(boxPosX,boxPosY,boxPosZ);
  G4double boxRotY = geo->GetTPixRotY();
  G4RotationMatrix* rotBox = new G4RotationMatrix;
  rotBox->rotateY(boxRotY);
  new G4PVPlacement(rotBox,posBox,fTPixBoxVolume,"TPixBox",fMotherVolume,false,0,true);
  printf("TPix Box placed at (%.1f,%.1f,%.1f) mm with a rotation of %.3f rad\n",
	 boxPosX/mm,boxPosY/mm,boxPosZ/mm,boxRotY/rad);

  // Create TPix box for chips
  G4double tpixSizeX = geo->GetTPixSizeX();
  G4double tpixSizeY = geo->GetTPixSizeY();
  G4double tpixSizeZ = geo->GetTPixSizeZ();
  printf("TPix size is %.2f x %.2f x %.2f mm3\n",tpixSizeX/mm,tpixSizeY/mm,tpixSizeZ/mm);
  G4Box* solidTPix = new G4Box("TPixSolid",0.5*tpixSizeX,0.5*tpixSizeY,0.5*tpixSizeZ);
  fTPixVolume = new G4LogicalVolume(solidTPix,G4Material::GetMaterial("Vacuum"),"TPixLogic",0,0,0);
  fTPixVolume->SetVisAttributes(G4VisAttributes::Invisible);
  //fTPixVolume->SetVisAttributes(G4VisAttributes(G4Colour::White()));

  /*
  // Position TPix in MotherVolume
  G4double tpixPosX = geo->GetTPixPosX();
  G4double tpixPosY = geo->GetTPixPosY();
  G4double tpixPosZ = geo->GetTPixPosZ();
  G4ThreeVector posTPix = G4ThreeVector(tpixPosX,tpixPosY,tpixPosZ);
  G4double tpixRotY = geo->GetTPixRotY();
  G4RotationMatrix* rotTPix = new G4RotationMatrix;
  rotTPix->rotateY(tpixRotY);
  new G4PVPlacement(rotTPix,posTPix,fTPixVolume,"TPix",fMotherVolume,false,0,true);
  printf("TPix placed at (%.1f,%.1f,%.1f) mm with a rotation of %.3f rad\n",
	 tpixPosX/mm,tpixPosY/mm,tpixPosZ/mm,tpixRotY/rad);
  */

  // Position TPix in main box
  G4double tpixDispX = geo->GetTPixDispX();
  G4double tpixDispY = geo->GetTPixDispY();
  G4double tpixDispZ = geo->GetTPixDispZ();
  G4ThreeVector dispTPix = G4ThreeVector(tpixDispX,tpixDispY,tpixDispZ);
  new G4PVPlacement(0,dispTPix,fTPixVolume,"TPix",fTPixBoxVolume,false,0,true);
  printf("TPix placed inside main box at (%.1f,%.1f,%.1f) mm\n",
	 tpixDispX/mm,tpixDispY/mm,tpixDispZ/mm);

  // Create standard TimePix chip
  G4double tpixChipX = geo->GetChipSizeX();
  G4double tpixChipY = geo->GetChipSizeY();
  G4double tpixChipZ = geo->GetChipSizeZ();
  printf("TPix Chip size is %.2f x %.2f x %.2f mm3\n",tpixChipX/mm,tpixChipY/mm,tpixChipZ/mm);
  G4Box* solidChip  = new G4Box("TPixChipSolid",0.5*tpixChipX,0.5*tpixChipY,0.5*tpixChipZ);
  fTPixChipVolume  = new G4LogicalVolume(solidChip,G4Material::GetMaterial("G4_Si"),"TPixChipLogic",0,0,0);
  fTPixChipVolume->SetVisAttributes(G4VisAttributes(colourTimePix));

  // Get number of fingers and position them
  for (G4int row=0;row<geo->GetTPixNRows();row++){
    for (G4int col=0;col<geo->GetTPixNCols();col++){
      G4int idx = 10*row+col;
      G4double x = geo->GetChipPosX(row,col);
      G4double y = geo->GetChipPosY(row,col);
      G4double z = geo->GetChipPosZ(row,col);
      printf("TPix chip %02d will be placed at (%.1f,%.1f,%.1f) mm\n",idx,x/mm,y/mm,z/mm);
      G4ThreeVector posChip = G4ThreeVector(x,y,z);
      new G4PVPlacement(0,posChip,fTPixChipVolume,"TPixChip",fTPixVolume,false,idx,true);
    }
  }
  
  // Create Cu structure
  
  // Cu slab behind TimePix
  G4double slabSizeX = tpixSizeX;
  G4double slabSizeY = tpixSizeY;
  G4double slabSizeZ = geo->GetCuSlabThick();
  printf("TPix Cu slab size is %.2f x %.2f x %.2f mm3\n",slabSizeX/mm,slabSizeY/mm,slabSizeZ/mm);
  G4Box* solidSlab = new G4Box("TPixCuSlabSolid",0.5*slabSizeX,0.5*slabSizeY,0.5*slabSizeZ);
  G4LogicalVolume* slabVolume = new G4LogicalVolume(solidSlab,G4Material::GetMaterial("G4_Cu"),"TPixCuSlabLogic",0,0,0);
  slabVolume->SetVisAttributes(G4VisAttributes(colourCu));

  // Position Cu slab behind TimePix
  G4double slabDispX = tpixDispX;
  G4double slabDispY = tpixDispY;
  G4double slabDispZ = tpixDispZ+0.5*tpixSizeZ+0.5*slabSizeZ+1.*um;
  G4ThreeVector dispSlab = G4ThreeVector(slabDispX,slabDispY,slabDispZ);
  new G4PVPlacement(0,dispSlab,slabVolume,"TPixCuSlab",fTPixBoxVolume,false,0,true);
  printf("TPix Cu slab placed inside main box at (%.1f,%.1f,%.1f) mm\n",
	 slabDispX/mm,slabDispY/mm,slabDispZ/mm);

  // Cu bars behind readout electronics
  G4double barSizeX = tpixSizeX;
  G4double barSizeY = geo->GetCuBarHeight();
  G4double barSizeZ = geo->GetCuBarThick();
  printf("TPix Cu bar size is %.2f x %.2f x %.2f mm3\n",barSizeX/mm,barSizeY/mm,barSizeZ/mm);
  G4Box* solidBar = new G4Box("TPixCuBarSolid",0.5*barSizeX,0.5*barSizeY,0.5*barSizeZ);
  G4LogicalVolume* barVolume = new G4LogicalVolume(solidBar,G4Material::GetMaterial("G4_Cu"),"TPixCuBarLogic",0,0,0);
  barVolume->SetVisAttributes(G4VisAttributes(colourCu));

  // Position Cu bars above and below TimePix
  G4double barTopDispX = tpixDispX;
  G4double barTopDispY = tpixDispY+0.5*tpixSizeY+0.5*barSizeY;
  G4double barTopDispZ = tpixDispZ+0.5*tpixSizeZ+0.5*barSizeZ;
  G4ThreeVector dispBarTop = G4ThreeVector(barTopDispX,barTopDispY,barTopDispZ);
  new G4PVPlacement(0,dispBarTop,barVolume,"TPixCuBarTop",fTPixBoxVolume,false,0,true);
  printf("TPix Cu Bar Top placed inside main box at (%.1f,%.1f,%.1f) mm\n",
	 barTopDispX/mm,barTopDispY/mm,barTopDispZ/mm);

  G4double barBotDispX = tpixDispX;
  G4double barBotDispY = tpixDispY-0.5*tpixSizeY-0.5*barSizeY;
  G4double barBotDispZ = tpixDispZ+0.5*tpixSizeZ+0.5*barSizeZ;
  G4ThreeVector dispBarBot = G4ThreeVector(barBotDispX,barBotDispY,barBotDispZ);
  new G4PVPlacement(0,dispBarBot,barVolume,"TPixCuBarBottom",fTPixBoxVolume,false,0,true);
  printf("TPix Cu Bar Bottom placed inside main box at (%.1f,%.1f,%.1f) mm\n",
	 barBotDispX/mm,barBotDispY/mm,barBotDispZ/mm);

  // Cu support frame
  G4double suppSizeX = geo->GetCuSuppWidth();
  G4double suppSizeY = geo->GetCuSuppHeight();
  G4double suppSizeZ = geo->GetCuSuppThick();
  printf("TPix Cu support is %.2f x %.2f x %.2f mm3\n",suppSizeX/mm,suppSizeY/mm,suppSizeZ/mm);
  G4Box* solidSupp = new G4Box("TPixCuSuppSolid",0.5*suppSizeX,0.5*suppSizeY,0.5*suppSizeZ);
  G4double holeSizeX = geo->GetCuHoleWidth();
  G4double holeSizeY = tpixSizeY;
  G4double holeSizeZ = suppSizeZ+10.*um;
  printf("TPix Cu hole is %.2f x %.2f x %.2f mm3\n",holeSizeX/mm,holeSizeY/mm,holeSizeZ/mm);
  G4Box* solidHole = new G4Box("TPixCuHoleSolid",0.5*holeSizeX,0.5*holeSizeY,0.5*holeSizeZ);
  G4SubtractionSolid* solidFrame = new G4SubtractionSolid("TPixCuFrameSolid",solidSupp,solidHole,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* frameVolume = new G4LogicalVolume(solidFrame,G4Material::GetMaterial("G4_Cu"),"TPixCuFrameLogic",0,0,0);
  frameVolume->SetVisAttributes(G4VisAttributes(colourCu));

  // Position Cu frame around TimePix behind bars
  G4double frameDispX = tpixDispX;
  G4double frameDispY = tpixDispY;
  G4double frameDispZ = tpixDispZ+0.5*tpixSizeZ+barSizeZ+0.5*suppSizeZ;
  G4ThreeVector dispFrame = G4ThreeVector(frameDispX,frameDispY,frameDispZ);
  new G4PVPlacement(0,dispFrame,frameVolume,"TPixCuFrame",fTPixBoxVolume,false,0,true);
  printf("TPix Cu frame placed inside main box at (%.1f,%.1f,%.1f) mm\n",
	 frameDispX/mm,frameDispY/mm,frameDispZ/mm);

  // PCB bars on top and bottom of TimePix
  G4double pcbSizeX = tpixSizeX;
  G4double pcbSizeY = geo->GetPCBBarHeight();
  G4double pcbSizeZ = geo->GetPCBBarThick();
  printf("TPix PCB size is %.2f x %.2f x %.2f mm3\n",pcbSizeX/mm,pcbSizeY/mm,pcbSizeZ/mm);
  G4Box* solidPCB = new G4Box("TPixPCBBarSolid",0.5*pcbSizeX,0.5*pcbSizeY,0.5*pcbSizeZ);
  G4LogicalVolume* pcbVolume = new G4LogicalVolume(solidPCB,G4Material::GetMaterial("PCB"),"TPixPCBBarLogic",0,0,0);
  pcbVolume->SetVisAttributes(G4VisAttributes(colourPCB));

  // Position PCB bars above and below TimePix
  G4double pcbTopDispX = tpixDispX;
  G4double pcbTopDispY = tpixDispY+0.5*tpixSizeY+0.5*pcbSizeY;
  G4double pcbTopDispZ = tpixDispZ-0.5*tpixSizeZ-geo->GetPCBBarDispZ()-0.5*pcbSizeZ;
  G4ThreeVector dispPCBTop = G4ThreeVector(pcbTopDispX,pcbTopDispY,pcbTopDispZ);
  new G4PVPlacement(0,dispPCBTop,pcbVolume,"TPixPCBBarTop",fTPixBoxVolume,false,0,true);
  printf("TPix PCB Bar Top placed inside main box at (%.1f,%.1f,%.1f) mm\n",
	 pcbTopDispX/mm,pcbTopDispY/mm,pcbTopDispZ/mm);

  G4double pcbBotDispX = tpixDispX;
  G4double pcbBotDispY = tpixDispY-0.5*tpixSizeY-0.5*pcbSizeY;
  G4double pcbBotDispZ = tpixDispZ-0.5*tpixSizeZ-geo->GetPCBBarDispZ()-0.5*pcbSizeZ;
  G4ThreeVector dispPCBBot = G4ThreeVector(pcbBotDispX,pcbBotDispY,pcbBotDispZ);
  new G4PVPlacement(0,dispPCBBot,pcbVolume,"TPixPCBBarBottom",fTPixBoxVolume,false,0,true);
  printf("TPix PCB Bar Bottom placed inside main box at (%.1f,%.1f,%.1f) mm\n",
	 pcbBotDispX/mm,pcbBotDispY/mm,pcbBotDispZ/mm);

  // Create digitizer for TPix
  G4DigiManager* theDM = G4DigiManager::GetDMpointer();
  G4String tpixDName = geo->GetTPixDigitizerName();
  printf("Registering TPix Digitizer %s\n",tpixDName.data());
  TPixDigitizer* tpixD = new TPixDigitizer(tpixDName);
  theDM->AddNewModule(tpixD);

  // Make chip a sensitive detector
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String tpixSDName = geo->GetTPixSensitiveDetectorName();
  printf("Registering TPix Sensitive Detector %s\n",tpixSDName.data());
  TPixSD* tpixSD = new TPixSD(tpixSDName);
  sdMan->AddNewDetector(tpixSD);
  fTPixChipVolume->SetSensitiveDetector(tpixSD);

}
/*
void TPixDetector::SetTPixChamberWallAngle(G4double a)
{
  TPixGeometry::GetInstance()->SetTPixChamberWallAngle(a);
}

void TPixDetector::SetTPixChamberWallCorner(G4ThreeVector c)
{
  TPixGeometry::GetInstance()->SetTPixChamberWallCorner(c);
}
*/
