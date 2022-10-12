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
#include "G4SubtractionSolid.hh"
#include "ETagGeometry.hh"
#include "ETagSD.hh"

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
  G4VisAttributes ETagBarVisAtt = G4VisAttributes(G4Colour::Green());
  ETagGeometry* geo = ETagGeometry::GetInstance();
  // Create ETag container box
  printf("ETag be placed at %f %f %f\n",geo->GetETagPosX(),geo->GetETagPosY(),geo->GetETagPosZ());
  G4ThreeVector ETagPos = G4ThreeVector(geo->GetETagPosX(),geo->GetETagPosY(),geo->GetETagPosZ()); 

  //ETag main volume
  G4double ETagSizeX = geo->GetETagSizeX();
  G4double ETagSizeY = geo->GetETagSizeY();
  G4double ETagSizeZ = geo->GetETagSizeZ();

  //ETag Hole volume
  G4double ETagHoleSizeX  = geo->GetETagHoleSizeX();
  G4double ETagHoleSizeY  = geo->GetETagHoleSizeY();
  G4double ETagHoleSizeZ  = geo->GetETagHoleSizeZ();

  printf("ETag size is %f %f %f\n",ETagSizeX,ETagSizeX,ETagSizeZ);
  G4Box* solidETagFull = new G4Box("solidETagFull",0.5*ETagSizeX,0.5*ETagSizeY,0.5*ETagSizeZ);
  G4Box* solidETagHole = new G4Box("solidETagHole",0.5*ETagHoleSizeX,0.5*ETagHoleSizeY,0.5*ETagHoleSizeZ);
  G4SubtractionSolid* solidETag = new G4SubtractionSolid("solidETag",solidETagFull,solidETagHole,0,G4ThreeVector(0.,0.,0.));

  fETagVolume = new G4LogicalVolume(solidETag,G4Material::GetMaterial("Vacuum"),"ETagLogic",0,0,0);
  fETagVolume->SetVisAttributes(G4VisAttributes(G4Colour::Magenta()));
  fETagVolume->SetVisAttributes(G4VisAttributes::Invisible);
  new G4PVPlacement(0,ETagPos,fETagVolume,"ETag",fMotherVolume,false,0,false);

  // Delta ray absorber
  //Delta Abs size
  G4double DeltaAbsSizeX = ETagSizeX; 
  G4double DeltaAbsSizeY = ETagSizeY;
  G4double DeltaAbsSizeZ = 6*mm;

  //Delta Abs size
  G4double DeltaAbsHoleSizeX = ETagHoleSizeX-3*mm; 
  G4double DeltaAbsHoleSizeY = ETagHoleSizeY-3*mm;
  G4double DeltaAbsHoleSizeZ = DeltaAbsSizeZ+0.2*mm;

  printf("ETag be placed at %f %f %f\n",geo->GetETagPosX(),geo->GetETagPosY(),geo->GetETagPosZ());
  G4ThreeVector DeltaAbsPos = G4ThreeVector(geo->GetETagPosX(),geo->GetETagPosY(),geo->GetETagPosZ()-3*ETagSizeZ); 

  G4Box* solidDeltaAbsFull = new G4Box("solidDeltaAbsFull",0.5*DeltaAbsSizeX,0.5*DeltaAbsSizeY,0.5*DeltaAbsSizeZ);
  G4Box* solidDeltaAbsHole = new G4Box("solidDeltaAbsHole",0.5*DeltaAbsHoleSizeX,0.5*DeltaAbsHoleSizeY,0.5*DeltaAbsHoleSizeZ);
  G4SubtractionSolid* solidDeltaAbs = new G4SubtractionSolid("solidDeltaAbs",solidDeltaAbsFull,solidDeltaAbsHole,0,G4ThreeVector(0.,0.,0.));

  fDeltaAbsVolume = new G4LogicalVolume(solidDeltaAbs,G4Material::GetMaterial("G4_PLEXIGLASS"),"DeltaAbs",0,0,0);
  fDeltaAbsVolume->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
  new G4PVPlacement(0,DeltaAbsPos,fDeltaAbsVolume,"DeltaAbs",fMotherVolume,false,0,false);

//  //ETagBars
//  
  G4double ETagBarSizeX  = geo->GetETagBarSizeX();
  G4double ETagBarSizeY  = geo->GetETagBarSizeY();
  G4double ETagBarSizeZ  = geo->GetETagBarSizeZ();
  printf("ETag size is %f %f %f\n",ETagSizeX,ETagSizeX,ETagSizeZ);
  G4Box* solidETagBar = new G4Box("solidETagBar",0.5*ETagBarSizeX,0.5*ETagBarSizeY,0.5*ETagBarSizeZ);
  fETagBarVolume = new G4LogicalVolume(solidETagBar,G4Material::GetMaterial("G4_POLYSTYRENE"),"ETagBar",0,0,0);
  fETagBarVolume->SetVisAttributes(ETagBarVisAtt);
  
  G4double ETagShortBarSizeX  = geo->GetETagShortBarSizeX();  //dai anche Y e Z metodi
  G4double ETagShortBarSizeY  = geo->GetETagShortBarSizeY();
  G4double ETagShortBarSizeZ  = geo->GetETagShortBarSizeZ();
  printf("ETag size is %f %f %f\n",ETagShortBarSizeX,ETagShortBarSizeX,ETagShortBarSizeZ);
  G4Box* solidETagShortBar = new G4Box("solidETagShortBar",0.5*ETagShortBarSizeX,0.5*ETagShortBarSizeY,0.5*ETagShortBarSizeZ);
  fETagShortBarVolume = new G4LogicalVolume(solidETagShortBar,G4Material::GetMaterial("G4_POLYSTYRENE"),"ETagShortBar",0,0,0);
  fETagShortBarVolume->SetVisAttributes(ETagBarVisAtt);
 
  G4double ETagBarPosX=0;
  G4double ETagBarPosY=0;
  G4double ETagBarPosZ=0;
  G4int  NBars = geo->GetETagNBars();
  G4int  BarIndex = 0;

  for(G4int nb=0;nb<NBars;nb++){
    //    printf("Bars be placed at %f %f %f\n",ETagBarPosX,ETagBarPosY,geo->GetETagPosZ());
    ETagBarPosX=0;
    //place the bars from top to bottom.
    ETagBarPosY = ETagSizeY/2-2.5*mm-ETagBarSizeY/2-ETagBarSizeY*nb; //compensate for extra dimension of Etag volume +5*mm
    if(nb>5 && nb<9){
      for(G4int lr=0;lr<2;lr++){
	if(lr==0) ETagBarPosX = ETagSizeX/2  - ETagShortBarSizeX/2 -2.5*mm; //compensate for extra dimension of Etag volume +5*mm
	if(lr==1) ETagBarPosX = -ETagSizeX/2 + ETagShortBarSizeX/2 +2.5*mm; //compensate for extra dimension of Etag volume +5*mm
	printf("ETag Bars %d be placed at %f %f %f %d\n",nb,ETagBarPosX,ETagBarPosY,geo->GetETagPosZ(),lr);
	G4ThreeVector ETagBarPos = G4ThreeVector(ETagBarPosX,ETagBarPosY,ETagBarPosZ); 
	new G4PVPlacement(0,ETagBarPos,fETagShortBarVolume,"ETagBar",fETagVolume,false,BarIndex,false);
      }
    }else{
      ETagBarPosX=0;
      printf("ETag Bars be placed at %d %f %f %f \n",nb,ETagBarPosX,ETagBarPosY,geo->GetETagPosZ());
      G4ThreeVector ETagBarPos = G4ThreeVector(ETagBarPosX,ETagBarPosY,ETagBarPosZ); 
      new G4PVPlacement(0,ETagBarPos,fETagBarVolume,"ETagBar",fETagVolume,false,BarIndex,false);
    }
    BarIndex++;
  }
  
//  // Create digitizer for ETag
//  G4DigiManager* theDM = G4DigiManager::GetDMpointer();
//  G4String ETagDName = geo->GetPVetoDigitizerName();
//  printf("Registering ETag Digitizer %s\n",ETagDName.data());
//  ETagDigitizer* ETagD = new PVetoDigitizer(ETagDName);
//  theDM->AddNewModule(ETagD);


  // Make whole ETag a sensitive detector
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String etagSDName = geo->GetETagSensitiveDetectorName();
  printf("Registering ETag SD %s\n",etagSDName.data());
  ETagSD* etagSD = new ETagSD(etagSDName);
  sdMan->AddNewDetector(etagSD);
  fETagBarVolume->SetSensitiveDetector(etagSD);
  fETagShortBarVolume->SetSensitiveDetector(etagSD);
  //  fETagVolume->SetSensitiveDetector(etagSD);
}
