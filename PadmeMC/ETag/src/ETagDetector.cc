// ETagDetector.cc
// --------------------------------------------------------------
// History:
//
// Created by Mauro Raggi (mauro.raggi@roma1.infn.it) 2022-06-01
// --------------------------------------------------------------

#include "ETagDetector.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"
#include "G4DigiManager.hh"
#include "G4SubtractionSolid.hh"

#include "ETagGeometry.hh"
#include "ETagSD.hh"
#include "ETagDigitizer.hh"

ETagDetector::ETagDetector(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{

  // Connect to ETagMessenger to enable datacard configuration
  fETagMessenger = new ETagMessenger(this);

}

ETagDetector::~ETagDetector()
{
  delete fETagMessenger;
}

// IMPLEMENTS the ETag geometry M. Raggi 06/06/2022

void ETagDetector::CreateGeometry()
{

  G4VisAttributes colourETag = G4VisAttributes::Invisible;
  //G4VisAttributes colourETag = G4VisAttributes(G4Colour::Magenta());
  G4VisAttributes colourBar = G4VisAttributes(G4Colour::Green());
  G4VisAttributes colourPaint = G4VisAttributes(G4Colour::White());
  G4VisAttributes colourFrame = G4VisAttributes(G4Colour::Gray());
  G4VisAttributes colourTedlar = G4VisAttributes(G4Colour::Blue());

  ETagGeometry* geo = ETagGeometry::GetInstance();

  // Create ETag container box

  //ETag main volume
  G4double ETagSizeX = geo->GetETagSizeX();
  G4double ETagSizeY = geo->GetETagSizeY();
  G4double ETagSizeZ = geo->GetETagSizeZ();
  printf("ETag main box size is %.2fmm %.2fmm %.2fmm\n",ETagSizeX/mm,ETagSizeY/mm,ETagSizeZ/mm);
  G4Box* solidETag = new G4Box("solidETag",0.5*ETagSizeX+1.*um,0.5*ETagSizeY+1.*um,0.5*ETagSizeZ+1.*um);
  fETagVolume = new G4LogicalVolume(solidETag,G4Material::GetMaterial("Air"),"ETagLogic",0,0,0);
  fETagVolume->SetVisAttributes(colourETag);

  // Position main ETag box
  G4double ETagPosX = geo->GetETagPosX();
  G4double ETagPosY = geo->GetETagPosY();
  G4double ETagPosZ = geo->GetETagPosZ();
  printf("ETag main box placed at %.2fmm %.2fmm %.2fmm\n",ETagPosX/mm,ETagPosY/mm,ETagPosZ/mm);
  G4ThreeVector ETagPos = G4ThreeVector(ETagPosX,ETagPosY,ETagPosZ); 
  new G4PVPlacement(0,ETagPos,fETagVolume,"ETag",fMotherVolume,false,0,true);

  //ETag frame volume
  G4double ETagFrameSizeX  = geo->GetETagFrameSizeX();
  G4double ETagFrameSizeY  = geo->GetETagFrameSizeY();
  G4double ETagFrameSizeZ  = geo->GetETagFrameSizeZ();
  G4double ETagFrameThick  = geo->GetETagFrameThick();
  printf("ETag metal frame size is %.2fmm %.2fmm %.2fmm and has thickness %.2fmm\n",ETagFrameSizeX/mm,ETagFrameSizeY/mm,ETagFrameSizeZ/mm,ETagFrameThick/mm);
  G4Box* solidETagFrameFull = new G4Box("solidETagFrameFull",0.5*ETagFrameSizeX,0.5*ETagFrameSizeY,0.5*ETagFrameSizeZ);
  G4Box* solidETagFrameHole = new G4Box("solidETagFrameHole",0.5*ETagFrameSizeX-ETagFrameThick,0.5*ETagFrameSizeY-ETagFrameThick,0.5*ETagFrameSizeZ+1.*mm);
  G4SubtractionSolid* solidETagFrame = new G4SubtractionSolid("solidETagFrame",solidETagFrameFull,solidETagFrameHole,0,G4ThreeVector(0.,0.,0.));
  G4LogicalVolume* logicETagFrame = new G4LogicalVolume(solidETagFrame,G4Material::GetMaterial("G4_Al"),"ETagFrameLogic",0,0,0);
  logicETagFrame->SetVisAttributes(colourFrame);
  G4ThreeVector ETagFramePos = G4ThreeVector(0.,0.,0.); 
  new G4PVPlacement(0,ETagFramePos,logicETagFrame,"ETagFrame",fETagVolume,false,0,true);

  // Tedlar cover
  G4double ETagTedlarSizeX  = geo->GetETagTedlarSizeX();
  G4double ETagTedlarSizeY  = geo->GetETagTedlarSizeY();
  G4double ETagTedlarSizeZ  = geo->GetETagTedlarSizeZ();
  printf("ETag Tedlar cover size is %.2fmm %.2fmm %.2fmm\n",ETagTedlarSizeX/mm,ETagTedlarSizeX/mm,ETagTedlarSizeZ/mm);
  G4Box* solidETagTedlar = new G4Box("solidETagTedlar",0.5*ETagTedlarSizeX,0.5*ETagTedlarSizeY,0.5*ETagTedlarSizeZ);
  G4LogicalVolume* logicETagTedlar = new G4LogicalVolume(solidETagTedlar,G4Material::GetMaterial("G4_POLYVINYLIDENE_FLUORIDE"),"ETagTedlarLogic",0,0,0);
  logicETagTedlar->SetVisAttributes(colourTedlar);
  G4ThreeVector ETagTedlarPos = G4ThreeVector(0.,0.,-0.5*ETagFrameSizeZ+0.5*ETagTedlarSizeZ); 
  new G4PVPlacement(0,ETagTedlarPos,logicETagTedlar,"ETagTedlar",fETagVolume,false,0,true);

  // ETagBars

  // Thickness of paint around bars
  G4double ETagBarPaintThick = geo->GetETagBarPaintThick();

  // Long bar
  G4double ETagBarSizeX  = geo->GetETagBarSizeX();
  G4double ETagBarSizeY  = geo->GetETagBarSizeY();
  G4double ETagBarSizeZ  = geo->GetETagBarSizeZ();
  printf("ETag long bar size is %.2fmm %.2fmm %.2fmm\n",ETagBarSizeX/mm,ETagBarSizeX/mm,ETagBarSizeZ/mm);
  G4Box* solidETagBar = new G4Box("solidETagBar",0.5*ETagBarSizeX,0.5*ETagBarSizeY,0.5*ETagBarSizeZ);
  fETagBarVolume = new G4LogicalVolume(solidETagBar,G4Material::GetMaterial("G4_POLYSTYRENE"),"ETagBar",0,0,0);
  fETagBarVolume->SetVisAttributes(colourBar);

  // Add paint coating
  G4Box* solidETagBarPaint = new G4Box("solidETagBarPaint",0.5*ETagBarSizeX+ETagBarPaintThick,0.5*ETagBarSizeY+ETagBarPaintThick,0.5*ETagBarSizeZ+ETagBarPaintThick);
  G4LogicalVolume* logicETagBarPaint = new G4LogicalVolume(solidETagBarPaint,G4Material::GetMaterial("EJ510Paint"),"ETagCell",0,0,0);
  logicETagBarPaint->SetVisAttributes(colourPaint);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),fETagBarVolume,"ETagBar",logicETagBarPaint,false,0,true);

  // Short bar
  G4double ETagShortBarSizeX  = geo->GetETagShortBarSizeX();
  G4double ETagShortBarSizeY  = geo->GetETagShortBarSizeY();
  G4double ETagShortBarSizeZ  = geo->GetETagShortBarSizeZ();
  printf("ETag short bar size is %.2fmm %.2fmm %.2fmm\n",ETagShortBarSizeX/mm,ETagShortBarSizeX/mm,ETagShortBarSizeZ/mm);
  G4Box* solidETagShortBar = new G4Box("solidETagShortBar",0.5*ETagShortBarSizeX,0.5*ETagShortBarSizeY,0.5*ETagShortBarSizeZ);
  fETagShortBarVolume = new G4LogicalVolume(solidETagShortBar,G4Material::GetMaterial("G4_POLYSTYRENE"),"ETagShortBar",0,0,0);
  fETagShortBarVolume->SetVisAttributes(colourBar);
 
  // Add paint coating
  G4Box* solidETagShortBarPaint = new G4Box("solidETagShortBarPaint",0.5*ETagShortBarSizeX+ETagBarPaintThick,0.5*ETagShortBarSizeY+ETagBarPaintThick,0.5*ETagShortBarSizeZ+ETagBarPaintThick);
  G4LogicalVolume* logicETagShortBarPaint = new G4LogicalVolume(solidETagShortBarPaint,G4Material::GetMaterial("EJ510Paint"),"ETagShortCell",0,0,0);
  logicETagShortBarPaint->SetVisAttributes(colourPaint);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),fETagShortBarVolume,"ETagShortBar",logicETagShortBarPaint,false,0,true);

  G4double ETagBarPosX = 0.;
  G4double ETagBarPosY = 0.;
  G4double ETagBarPosZ = 0.5*ETagFrameSizeZ-geo->GetETagBarBackDisp()-0.5*ETagBarSizeZ-ETagBarPaintThick;
  G4ThreeVector ETagBarPos;
  G4int ETagBarID;

  // Vertical gap between two bars
  G4double ETagBarGapY = geo->GetETagBarGapY();

  // Create 15 rows (0-15). Rows 0-5 and 9-14 are a single long bars, Rows 6-8 are two short bars
  for(G4int nb=0; nb<geo->GetETagNBars(); nb++){

    // Start with row 0 at lower position
    
    ETagBarID = nb*10;

    // Bar 7 is at center
    ETagBarPosY = (nb-7)*(ETagBarSizeY+2.*ETagBarPaintThick+ETagBarGapY);

    if (nb>=6 && nb<9) {

      // Short bars
      ETagBarPosX = -0.5*ETagBarSizeX+0.5*ETagShortBarSizeX;
      printf("ETag Bar row %2d left  (wall) (ID=%3d) placed at %7.2fmm %7.2fmm %7.2fmm\n",nb,ETagBarID,ETagBarPosX,ETagBarPosY,ETagBarPosZ);
      ETagBarPos = G4ThreeVector(ETagBarPosX,ETagBarPosY,ETagBarPosZ); 
      new G4PVPlacement(0,ETagBarPos,logicETagShortBarPaint,"ETagPaint",fETagVolume,false,ETagBarID,true);

      ETagBarID++;
      ETagBarPosX = 0.5*ETagBarSizeX-0.5*ETagShortBarSizeX;
      printf("ETag Bar row %2d right (door) (ID=%3d) placed at %7.2fmm %7.2fmm %7.2fmm\n",nb,ETagBarID,ETagBarPosX,ETagBarPosY,ETagBarPosZ);
      ETagBarPos = G4ThreeVector(ETagBarPosX,ETagBarPosY,ETagBarPosZ); 
      new G4PVPlacement(0,ETagBarPos,logicETagShortBarPaint,"ETagPaint",fETagVolume,false,ETagBarID,true);

    } else {

      // Long bars
      ETagBarPosX = 0.;
      printf("ETag Bar row %2d              (ID=%3d) placed at %7.2fmm %7.2fmm %7.2fmm\n",nb,ETagBarID,ETagBarPosX,ETagBarPosY,ETagBarPosZ);
      ETagBarPos = G4ThreeVector(ETagBarPosX,ETagBarPosY,ETagBarPosZ); 
      new G4PVPlacement(0,ETagBarPos,logicETagBarPaint,"ETagPaint",fETagVolume,false,ETagBarID,true);

    }

  }

  // Create digitizer for ETag
  G4DigiManager* theDM = G4DigiManager::GetDMpointer();
  G4String ETagDName = geo->GetETagDigitizerName();
  printf("Registering ETag Digitizer %s\n",ETagDName.data());
  ETagDigitizer* ETagD = new ETagDigitizer(ETagDName);
  theDM->AddNewModule(ETagD);

  // Make all ETag bars a sensitive detector
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String etagSDName = geo->GetETagSensitiveDetectorName();
  printf("Registering ETag SD %s\n",etagSDName.data());
  ETagSD* etagSD = new ETagSD(etagSDName);
  sdMan->AddNewDetector(etagSD);
  fETagBarVolume->SetSensitiveDetector(etagSD);
  fETagShortBarVolume->SetSensitiveDetector(etagSD);

}
