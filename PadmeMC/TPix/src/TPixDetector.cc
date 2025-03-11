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
#include "G4Tubs.hh"
#include "G4ExtrudedSolid.hh"
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
  //G4Colour colourBox = G4Colour::Grey();
  G4Colour colourTimePix = G4Colour::Blue();
  G4Colour colourWater = G4Colour::Blue();
  G4Colour colourPCB = G4Colour::Green();

  // Create main TPix box
  G4double boxSizeX = geo->GetBoxSizeX();
  G4double boxSizeY = geo->GetBoxSizeY();
  G4double boxSizeZ = geo->GetBoxSizeZ();
  printf("TPix Box size is %.2f x %.2f x %.2f mm3\n",boxSizeX/mm,boxSizeY/mm,boxSizeZ/mm);
  G4Box* solidBox = new G4Box("TPixBoxSolid",0.5*boxSizeX,0.5*boxSizeY,0.5*boxSizeZ);
  fTPixBoxVolume = new G4LogicalVolume(solidBox,G4Material::GetMaterial("Air"),"TPixBoxLogic",0,0,0);
  fTPixBoxVolume->SetVisAttributes(G4VisAttributes::Invisible);
  //fTPixBoxVolume->SetVisAttributes(G4VisAttributes(colourBox));

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
  fTPixVolume = new G4LogicalVolume(solidTPix,G4Material::GetMaterial("Air"),"TPixLogic",0,0,0);
  fTPixVolume->SetVisAttributes(G4VisAttributes::Invisible);
  //fTPixVolume->SetVisAttributes(G4VisAttributes(colourBox));

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

  // Get number of chips and position them
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
  G4LogicalVolume* slabVolume = new G4LogicalVolume(solidSlab,G4Material::GetMaterial("Air"),"TPixCuSlabLogic",0,0,0);
  //slabVolume->SetVisAttributes(G4VisAttributes(colourBox));
  slabVolume->SetVisAttributes(G4VisAttributes::Invisible);

  // Position Cu fingers inside slab
  G4double fingerWidth = geo->GetCuSlabFingerWidth();
  G4double fingerSizeX = fingerWidth;
  G4double fingerSizeY = tpixChipY;
  G4double fingerSizeZ = slabSizeZ;
  G4Box* solidFinger = new G4Box("TPixCuFingerSolid",0.5*fingerSizeX,0.5*fingerSizeY,0.5*fingerSizeZ);
  G4LogicalVolume* fingerVolume = new G4LogicalVolume(solidFinger,G4Material::GetMaterial("G4_Cu"),"TPixCuFingerLogic",0,0,0);
  fingerVolume->SetVisAttributes(G4VisAttributes(colourCu));
  for (G4int row=0;row<geo->GetTPixNRows();row++){
    for (G4int col=0;col<geo->GetTPixNCols();col++){
      G4double fingerLPosX = geo->GetChipPosX(row,col)-0.5*tpixChipX+0.5*fingerSizeX;
      G4double fingerRPosX = geo->GetChipPosX(row,col)+0.5*tpixChipX-0.5*fingerSizeX;
      G4double fingerPosY = geo->GetChipPosY(row,col);
      G4double fingerPosZ = 0.;
      G4int fingerLIdx = 10*row+col;
      G4int fingerRIdx = 100+10*row+col;
      new G4PVPlacement(0,G4ThreeVector(fingerLPosX,fingerPosY,fingerPosZ),fingerVolume,"TPixCuFinger",slabVolume,false,fingerLIdx,true);
      new G4PVPlacement(0,G4ThreeVector(fingerRPosX,fingerPosY,fingerPosZ),fingerVolume,"TPixCuFinger",slabVolume,false,fingerRIdx,true);
    }
  }

  // Position Cu slab fingers behind TimePix
  G4double slabDispX = tpixDispX;
  G4double slabDispY = tpixDispY;
  G4double slabDispZ = tpixDispZ+0.5*tpixSizeZ+0.5*slabSizeZ+1.*um;
  G4ThreeVector dispSlab = G4ThreeVector(slabDispX,slabDispY,slabDispZ);
  new G4PVPlacement(0,dispSlab,slabVolume,"TPixCuSlab",fTPixBoxVolume,false,0,true);
  printf("TPix Cu slab placed inside main box at (%.1f,%.1f,%.1f) mm\n",
	 slabDispX/mm,slabDispY/mm,slabDispZ/mm);

  // Cu support shape above and below TPix chips
  std::vector<G4TwoVector> shPoly = geo->GetCuShapePolygon();
  G4double shWidth = geo->GetCuShapeWidth();
  G4double shHeight = geo->GetCuShapeHeight();
  G4double shDepth = geo->GetCuShapeDepth();
  G4ExtrudedSolid* solidShape = new G4ExtrudedSolid("TPixCuShapeSolid",shPoly,0.5*shWidth,G4TwoVector(0.,0.),1.,G4TwoVector(0.,0.),1.);
  G4LogicalVolume* shVolume = new G4LogicalVolume(solidShape,G4Material::GetMaterial("G4_Cu"),"TPixCuShapeLogic",0,0,0);
  shVolume->SetVisAttributes(G4VisAttributes(colourCu));

  // Cu blocks behind readout electronics
  G4double blockSizeX = geo->GetCuBlockWidth();
  G4double blockSizeY = geo->GetCuBlockHeight();
  G4double blockSizeZ = geo->GetCuBlockDepth();
  printf("TPix Cu block size is %.2f x %.2f x %.2f mm3\n",blockSizeX/mm,blockSizeY/mm,blockSizeZ/mm);
  G4Box* solidBlock = new G4Box("TPixCuBlockSolid",0.5*blockSizeX,0.5*blockSizeY,0.5*blockSizeZ);
  G4LogicalVolume* blockVolume = new G4LogicalVolume(solidBlock,G4Material::GetMaterial("G4_Cu"),"TPixCuBlockLogic",0,0,0);
  blockVolume->SetVisAttributes(G4VisAttributes(colourCu));

  G4RotationMatrix* rotUpTPixSh = new G4RotationMatrix;
  rotUpTPixSh->rotateX(90.*deg);
  rotUpTPixSh->rotateY(-90.*deg);
  G4RotationMatrix* rotDownTPixSh = new G4RotationMatrix;
  rotDownTPixSh->rotateX(90.*deg);
  rotDownTPixSh->rotateY(90.*deg);
  for (G4int row=0;row<geo->GetTPixNRows();row++){
    for (G4int col=0;col<geo->GetTPixNCols();col++){
      G4int idx = 10*row+col;
      if (row==0) {
	G4RotationMatrix* rot = rotUpTPixSh;
	G4ThreeVector disp = G4ThreeVector(tpixDispX+geo->GetChipPosX(row,col),tpixDispY+0.5*tpixSizeY+shHeight,slabDispZ-0.5*slabSizeZ+shDepth);
	new G4PVPlacement(rot,disp,shVolume,"TPixCuShape",fTPixBoxVolume,false,idx,true);
	G4ThreeVector dispB = G4ThreeVector(disp.x()+0.5*shWidth,disp.y()-shHeight+0.5*blockSizeY,disp.z()+0.5*blockSizeZ);
	new G4PVPlacement(0,dispB,blockVolume,"TPixCuBlock",fTPixBoxVolume,false,idx,true);
      } else {
	G4RotationMatrix* rot = rotDownTPixSh;
	G4ThreeVector disp = G4ThreeVector(tpixDispX+geo->GetChipPosX(row,col),tpixDispY-0.5*tpixSizeY-shHeight,slabDispZ-0.5*slabSizeZ+shDepth);
	new G4PVPlacement(rot,disp,shVolume,"TPixCuShape",fTPixBoxVolume,false,idx,true);
	G4ThreeVector dispB = G4ThreeVector(disp.x()-0.5*shWidth,disp.y()+shHeight-0.5*blockSizeY,disp.z()+0.5*blockSizeZ);
	new G4PVPlacement(0,dispB,blockVolume,"TPixCuBlock",fTPixBoxVolume,false,idx,true);
      }
    }
  }

  // Cu support frame
  G4double suppSizeX = geo->GetCuSuppWidth();
  G4double suppSizeY = geo->GetCuSuppHeight();
  G4double suppSizeZ = geo->GetCuSuppThick();
  printf("TPix Cu support is %.2f x %.2f x %.2f mm3\n",suppSizeX/mm,suppSizeY/mm,suppSizeZ/mm);
  G4Box* solidSupp = new G4Box("TPixCuSuppSolid",0.5*suppSizeX,0.5*suppSizeY,0.5*suppSizeZ);
  G4double holeSizeX = geo->GetCuHoleWidth();
  G4double holeSizeY = geo->GetCuHoleHeight();
  G4double holeSizeZ = suppSizeZ+10.*um;
  printf("TPix Cu hole is %.2f x %.2f x %.2f mm3\n",holeSizeX/mm,holeSizeY/mm,holeSizeZ/mm);
  G4Box* solidHole = new G4Box("TPixCuHoleSolid",0.5*holeSizeX,0.5*holeSizeY,0.5*holeSizeZ);
  G4double holeDispX = 0.5*suppSizeX-geo->GetCuHoleDispX()-0.5*holeSizeX;
  G4double holeDispY = -0.5*suppSizeY+geo->GetCuHoleDispY()+0.5*holeSizeY;
  G4ThreeVector holeDisp = G4ThreeVector(holeDispX,holeDispY,0.);
  G4SubtractionSolid* solidFrame = new G4SubtractionSolid("TPixCuFrameSolid",solidSupp,solidHole,0,holeDisp);
  G4LogicalVolume* frameVolume = new G4LogicalVolume(solidFrame,G4Material::GetMaterial("G4_Cu"),"TPixCuFrameLogic",0,0,0);
  frameVolume->SetVisAttributes(G4VisAttributes(colourCu));

  // Add internal pipes with cooling water
  G4double pipeSizeR = geo->GetWaterPipeRadius();
  G4double pipeSizeX = geo->GetWaterPipeLength();
  G4Tubs* solidPipe = new G4Tubs("TPixWaterPipe",0.,pipeSizeR,0.5*pipeSizeX,0.*deg,360.*deg);
  G4LogicalVolume* pipeVolume = new G4LogicalVolume(solidPipe,G4Material::GetMaterial("Water"),"TPixWaterPipeLogic",0,0,0);
  pipeVolume->SetVisAttributes(G4VisAttributes(colourWater));
  // Pipes are directed along X
  G4RotationMatrix* rotPipe = new G4RotationMatrix;
  rotPipe->rotateY(90.*deg);
  G4ThreeVector dispPipeUp = G4ThreeVector(0.,0.5*suppSizeY-geo->GetWaterPipeUpDisp(),0.);
  G4ThreeVector dispPipeDown = G4ThreeVector(0.,-0.5*suppSizeY+geo->GetWaterPipeDownDisp(),0.);
  new G4PVPlacement(rotPipe,dispPipeUp,pipeVolume,"TPixWaterPipe",frameVolume,false,0,true);
  new G4PVPlacement(rotPipe,dispPipeDown,pipeVolume,"TPixWaterPipe",frameVolume,false,0,true);

  // Position Cu frame around TimePix behind bars (mind the X displacement)
  G4double frameDispX = tpixDispX+0.5*tpixSizeX+geo->GetCuSuppToChipDispX()-0.5*suppSizeX;
  G4double frameDispY = tpixDispY;
  G4double frameDispZ = tpixDispZ+0.5*tpixSizeZ+shDepth+blockSizeZ+0.5*suppSizeZ+10.*um; // Add small displacement to avoid overlaps with blocks
  G4ThreeVector dispFrame = G4ThreeVector(frameDispX,frameDispY,frameDispZ);
  new G4PVPlacement(0,dispFrame,frameVolume,"TPixCuFrame",fTPixBoxVolume,false,0,true);
  printf("TPix Cu frame placed inside main box at (%.1f,%.1f,%.1f) mm\n",
  	 frameDispX/mm,frameDispY/mm,frameDispZ/mm);

  // Add small Cu bar on top side of support frame
  G4double stickSizeX = suppSizeX;
  G4double stickSizeY = 3.*mm;
  G4double stickSizeZ = 2.*mm;
  G4Box* solidStick = new G4Box("TPixCuStickSolid",0.5*stickSizeX,0.5*stickSizeY,0.5*stickSizeZ);
  G4LogicalVolume* stickVolume = new G4LogicalVolume(solidStick,G4Material::GetMaterial("G4_Cu"),"TPixCuStickLogic",0,0,0);
  stickVolume->SetVisAttributes(G4VisAttributes(colourCu));
  G4double stickPosX = frameDispX;
  G4double stickPosY = frameDispY+0.5*suppSizeY-7.2*mm-0.5*stickSizeY;
  G4double stickPosZ = frameDispZ-0.5*suppSizeZ-0.5*stickSizeZ;
  new G4PVPlacement(0,G4ThreeVector(stickPosX,stickPosY,stickPosZ),stickVolume,"TPixCuStick",fTPixBoxVolume,false,0,true);

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
