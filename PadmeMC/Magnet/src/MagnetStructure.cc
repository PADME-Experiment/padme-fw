// MagnetStructure.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-01-11
// --------------------------------------------------------------

#include "MagnetStructure.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
//#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"
//#include "G4BlineTracer.hh"

//#include "MagneticFieldSetup.hh"
#include "MagnetGeometry.hh"
//#include "MagnetSD.hh"

MagnetStructure::MagnetStructure(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{

  // Connect to MagnetMessenger to enable datacard configuration
  fMagnetMessenger = new MagnetMessenger(this);

  // Magnetic field is ON by default
  //fMagneticField = 1;

  // Magnetic volume is invisible by default
  //fMagneticVolumeIsVisible = 0;

}

MagnetStructure::~MagnetStructure()
{
  delete fMagnetMessenger;
}

void MagnetStructure::CreateGeometry()
{

  MagnetGeometry* geo = MagnetGeometry::GetInstance();

  // Define colors to use for the various parts of the magnet
  G4VisAttributes yokeVisAttr = G4VisAttributes(G4Colour::Green());
  G4VisAttributes coilVisAttr = G4VisAttributes(G4Colour::Red());
  G4VisAttributes slabVisAttr = G4VisAttributes(G4Colour::Green());
  G4VisAttributes neopreneVisAttr = G4VisAttributes(G4Colour::White());

  // Small gap between magnet yoke's sections
  G4double magGap = geo->GetMagnetGap();

  // Create and position upper part of magnet yoke
  G4double yokeUpSizeX = geo->GetYokeUpSizeX();
  G4double yokeUpSizeY = geo->GetYokeUpSizeY();
  G4double yokeUpSizeZ = geo->GetYokeUpSizeZ();
  G4Box* yokeUpSolid = new G4Box("YokeUp",0.5*(yokeUpSizeX-magGap),0.5*(yokeUpSizeY-magGap),0.5*(yokeUpSizeZ-magGap));
  G4LogicalVolume* yokeUpVolume = new G4LogicalVolume(yokeUpSolid,G4Material::GetMaterial("G4_Fe"),"YokeUp",0,0,0);
  //yokeUpVolume->SetVisAttributes(G4VisAttributes(G4Colour::Green()));
  yokeUpVolume->SetVisAttributes(yokeVisAttr);
  G4ThreeVector yokeUpPos = G4ThreeVector(geo->GetYokeUpPosX(),geo->GetYokeUpPosY(),geo->GetYokeUpPosZ());
  new G4PVPlacement(0,yokeUpPos,yokeUpVolume,"YokeUp",fMotherVolume,false,0,false);

  // Create and position lower part of magnet yoke
  G4double yokeDownSizeX = geo->GetYokeDownSizeX();
  G4double yokeDownSizeY = geo->GetYokeDownSizeY();
  G4double yokeDownSizeZ = geo->GetYokeDownSizeZ();
  G4Box* yokeDownSolid = new G4Box("YokeDown",0.5*(yokeDownSizeX-magGap),0.5*(yokeDownSizeY-magGap),0.5*(yokeDownSizeZ-magGap));
  G4LogicalVolume* yokeDownVolume = new G4LogicalVolume(yokeDownSolid,G4Material::GetMaterial("G4_Fe"),"YokeDown",0,0,0);
  //yokeDownVolume->SetVisAttributes(G4VisAttributes(G4Colour::Green()));
  yokeDownVolume->SetVisAttributes(yokeVisAttr);
  G4ThreeVector yokeDownPos = G4ThreeVector(geo->GetYokeDownPosX(),geo->GetYokeDownPosY(),geo->GetYokeDownPosZ());
  new G4PVPlacement(0,yokeDownPos,yokeDownVolume,"YokeDown",fMotherVolume,false,0,false);

  // Create and position right part of magnet yoke
  G4double yokeRightSizeX = geo->GetYokeRightSizeX();
  G4double yokeRightSizeY = geo->GetYokeRightSizeY();
  G4double yokeRightSizeZ = geo->GetYokeRightSizeZ();
  G4Box* yokeRightSolid = new G4Box("YokeRight",0.5*(yokeRightSizeX-magGap),0.5*(yokeRightSizeY-magGap),0.5*(yokeRightSizeZ-magGap));
  G4LogicalVolume* yokeRightVolume = new G4LogicalVolume(yokeRightSolid,G4Material::GetMaterial("G4_Fe"),"YokeRight",0,0,0);
  //yokeRightVolume->SetVisAttributes(G4VisAttributes(G4Colour::Green()));
  yokeRightVolume->SetVisAttributes(yokeVisAttr);
  G4ThreeVector yokeRightPos = G4ThreeVector(geo->GetYokeRightPosX(),geo->GetYokeRightPosY(),geo->GetYokeRightPosZ());
  new G4PVPlacement(0,yokeRightPos,yokeRightVolume,"YokeRight",fMotherVolume,false,0,false);

  // Create and position left part of magnet yoke
  G4double yokeLeftSizeX = geo->GetYokeLeftSizeX();
  G4double yokeLeftSizeY = geo->GetYokeLeftSizeY();
  G4double yokeLeftSizeZ = geo->GetYokeLeftSizeZ();
  G4Box* yokeLeftSolid = new G4Box("YokeLeft",0.5*(yokeLeftSizeX-magGap),0.5*(yokeLeftSizeY-magGap),0.5*(yokeLeftSizeZ-magGap));
  G4LogicalVolume* yokeLeftVolume = new G4LogicalVolume(yokeLeftSolid,G4Material::GetMaterial("G4_Fe"),"YokeLeft",0,0,0);
  //yokeLeftVolume->SetVisAttributes(G4VisAttributes(G4Colour::Green()));
  yokeLeftVolume->SetVisAttributes(yokeVisAttr);
  G4ThreeVector yokeLeftPos = G4ThreeVector(geo->GetYokeLeftPosX(),geo->GetYokeLeftPosY(),geo->GetYokeLeftPosZ());
  new G4PVPlacement(0,yokeLeftPos,yokeLeftVolume,"YokeLeft",fMotherVolume,false,0,false);

  // Create and position upper bar part of magnet yoke
  G4double yokeUpBarSizeX = geo->GetYokeUpBarSizeX();
  G4double yokeUpBarSizeY = geo->GetYokeUpBarSizeY();
  G4double yokeUpBarSizeZ = geo->GetYokeUpBarSizeZ();
  G4Box* yokeUpBarSolid = new G4Box("YokeUpBar",0.5*(yokeUpBarSizeX-magGap),0.5*(yokeUpBarSizeY-magGap),0.5*(yokeUpBarSizeZ-magGap));
  G4LogicalVolume* yokeUpBarVolume = new G4LogicalVolume(yokeUpBarSolid,G4Material::GetMaterial("G4_Fe"),"YokeUpBar",0,0,0);
  //yokeUpBarVolume->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));
  yokeUpBarVolume->SetVisAttributes(yokeVisAttr);
  G4ThreeVector yokeUpBarPos = G4ThreeVector(geo->GetYokeUpBarPosX(),geo->GetYokeUpBarPosY(),geo->GetYokeUpBarPosZ());
  new G4PVPlacement(0,yokeUpBarPos,yokeUpBarVolume,"YokeUpBar",fMotherVolume,false,0,false);

  // Create and position lower bar part of magnet yoke
  G4double yokeDownBarSizeX = geo->GetYokeDownBarSizeX();
  G4double yokeDownBarSizeY = geo->GetYokeDownBarSizeY();
  G4double yokeDownBarSizeZ = geo->GetYokeDownBarSizeZ();
  G4Box* yokeDownBarSolid = new G4Box("YokeDownBar",0.5*(yokeDownBarSizeX-magGap),0.5*(yokeDownBarSizeY-magGap),0.5*(yokeDownBarSizeZ-magGap));
  G4LogicalVolume* yokeDownBarVolume = new G4LogicalVolume(yokeDownBarSolid,G4Material::GetMaterial("G4_Fe"),"YokeDownBar",0,0,0);
  //yokeDownBarVolume->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));
  yokeDownBarVolume->SetVisAttributes(yokeVisAttr);
  G4ThreeVector yokeDownBarPos = G4ThreeVector(geo->GetYokeDownBarPosX(),geo->GetYokeDownBarPosY(),geo->GetYokeDownBarPosZ());
  new G4PVPlacement(0,yokeDownBarPos,yokeDownBarVolume,"YokeDownBar",fMotherVolume,false,0,false);

  // Create coils

  G4Box* coilUFSolid = new G4Box("CoilUF",0.5*(geo->GetCoilUFSizeX()-magGap),0.5*(geo->GetCoilUFSizeY()-magGap),0.5*(geo->GetCoilUFSizeZ()-magGap));
  G4LogicalVolume* coilUFVolume = new G4LogicalVolume(coilUFSolid,G4Material::GetMaterial("G4_Cu"),"CoilUF",0,0,0);
  //coilUFVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  coilUFVolume->SetVisAttributes(coilVisAttr);
  G4ThreeVector coilUFPos = G4ThreeVector(geo->GetCoilUFPosX(),geo->GetCoilUFPosY(),geo->GetCoilUFPosZ());
  new G4PVPlacement(0,coilUFPos,coilUFVolume,"CoilUF",fMotherVolume,false,0,false);

  G4Box* coilUBSolid = new G4Box("CoilUB",0.5*(geo->GetCoilUBSizeX()-magGap),0.5*(geo->GetCoilUBSizeY()-magGap),0.5*(geo->GetCoilUBSizeZ()-magGap));
  G4LogicalVolume* coilUBVolume = new G4LogicalVolume(coilUBSolid,G4Material::GetMaterial("G4_Cu"),"CoilUB",0,0,0);
  //coilUBVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  coilUBVolume->SetVisAttributes(coilVisAttr);
  G4ThreeVector coilUBPos = G4ThreeVector(geo->GetCoilUBPosX(),geo->GetCoilUBPosY(),geo->GetCoilUBPosZ());
  new G4PVPlacement(0,coilUBPos,coilUBVolume,"CoilUB",fMotherVolume,false,0,false);

  G4Box* coilULSolid = new G4Box("CoilUL",0.5*(geo->GetCoilULSizeX()-magGap),0.5*(geo->GetCoilULSizeY()-magGap),0.5*(geo->GetCoilULSizeZ()-magGap));
  G4LogicalVolume* coilULVolume = new G4LogicalVolume(coilULSolid,G4Material::GetMaterial("G4_Cu"),"CoilUL",0,0,0);
  //coilULVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  coilULVolume->SetVisAttributes(coilVisAttr);
  G4ThreeVector coilULPos = G4ThreeVector(geo->GetCoilULPosX(),geo->GetCoilULPosY(),geo->GetCoilULPosZ());
  new G4PVPlacement(0,coilULPos,coilULVolume,"CoilUL",fMotherVolume,false,0,false);

  G4Box* coilURSolid = new G4Box("CoilUR",0.5*(geo->GetCoilURSizeX()-magGap),0.5*(geo->GetCoilURSizeY()-magGap),0.5*(geo->GetCoilURSizeZ()-magGap));
  G4LogicalVolume* coilURVolume = new G4LogicalVolume(coilURSolid,G4Material::GetMaterial("G4_Cu"),"CoilUR",0,0,0);
  //coilURVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  coilURVolume->SetVisAttributes(coilVisAttr);
  G4ThreeVector coilURPos = G4ThreeVector(geo->GetCoilURPosX(),geo->GetCoilURPosY(),geo->GetCoilURPosZ());
  new G4PVPlacement(0,coilURPos,coilURVolume,"CoilUR",fMotherVolume,false,0,false);

  G4Box* coilDFSolid = new G4Box("CoilDF",0.5*(geo->GetCoilDFSizeX()-magGap),0.5*(geo->GetCoilDFSizeY()-magGap),0.5*(geo->GetCoilDFSizeZ()-magGap));
  G4LogicalVolume* coilDFVolume = new G4LogicalVolume(coilDFSolid,G4Material::GetMaterial("G4_Cu"),"CoilDF",0,0,0);
  //coilDFVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  coilDFVolume->SetVisAttributes(coilVisAttr);
  G4ThreeVector coilDFPos = G4ThreeVector(geo->GetCoilDFPosX(),geo->GetCoilDFPosY(),geo->GetCoilDFPosZ());
  new G4PVPlacement(0,coilDFPos,coilDFVolume,"CoilDF",fMotherVolume,false,0,false);

  G4Box* coilDBSolid = new G4Box("CoilDB",0.5*(geo->GetCoilDBSizeX()-magGap),0.5*(geo->GetCoilDBSizeY()-magGap),0.5*(geo->GetCoilDBSizeZ()-magGap));
  G4LogicalVolume* coilDBVolume = new G4LogicalVolume(coilDBSolid,G4Material::GetMaterial("G4_Cu"),"CoilDB",0,0,0);
  //coilDBVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  coilDBVolume->SetVisAttributes(coilVisAttr);
  G4ThreeVector coilDBPos = G4ThreeVector(geo->GetCoilDBPosX(),geo->GetCoilDBPosY(),geo->GetCoilDBPosZ());
  new G4PVPlacement(0,coilDBPos,coilDBVolume,"CoilDB",fMotherVolume,false,0,false);

  G4Box* coilDLSolid = new G4Box("CoilDL",0.5*(geo->GetCoilDLSizeX()-magGap),0.5*(geo->GetCoilDLSizeY()-magGap),0.5*(geo->GetCoilDLSizeZ()-magGap));
  G4LogicalVolume* coilDLVolume = new G4LogicalVolume(coilDLSolid,G4Material::GetMaterial("G4_Cu"),"CoilDL",0,0,0);
  //coilDLVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  coilDLVolume->SetVisAttributes(coilVisAttr);
  G4ThreeVector coilDLPos = G4ThreeVector(geo->GetCoilDLPosX(),geo->GetCoilDLPosY(),geo->GetCoilDLPosZ());
  new G4PVPlacement(0,coilDLPos,coilDLVolume,"CoilDL",fMotherVolume,false,0,false);

  G4Box* coilDRSolid = new G4Box("CoilDR",0.5*(geo->GetCoilDRSizeX()-magGap),0.5*(geo->GetCoilDRSizeY()-magGap),0.5*(geo->GetCoilDRSizeZ()-magGap));
  G4LogicalVolume* coilDRVolume = new G4LogicalVolume(coilDRSolid,G4Material::GetMaterial("G4_Cu"),"CoilDR",0,0,0);
  //coilDRVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  coilDRVolume->SetVisAttributes(coilVisAttr);
  G4ThreeVector coilDRPos = G4ThreeVector(geo->GetCoilDRPosX(),geo->GetCoilDRPosY(),geo->GetCoilDRPosZ());
  new G4PVPlacement(0,coilDRPos,coilDRVolume,"CoilDR",fMotherVolume,false,0,false);

  G4double coilInnR = geo->GetCoilInnerRadius();
  G4double coilOutR = geo->GetCoilOuterRadius();
  G4double coilSizeY = geo->GetCoilSizeY();
  G4RotationMatrix* coilRot = new G4RotationMatrix();
  coilRot->rotateX(90.*deg);

  G4Tubs* coilUFLSolid = new G4Tubs("CoilUFL",coilInnR,coilOutR,0.5*coilSizeY,geo->GetCoilUFLAngle(),90.*deg);
  G4LogicalVolume* coilUFLVolume = new G4LogicalVolume(coilUFLSolid,G4Material::GetMaterial("G4_Cu"),"CoilUFL",0,0,0);
  //coilUFLVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  coilUFLVolume->SetVisAttributes(coilVisAttr);
  G4ThreeVector coilUFLPos = G4ThreeVector(geo->GetCoilUFLPosX(),geo->GetCoilUFLPosY(),geo->GetCoilUFLPosZ());
  new G4PVPlacement(coilRot,coilUFLPos,coilUFLVolume,"CoilUFL",fMotherVolume,false,0,false);

  G4Tubs* coilUFRSolid = new G4Tubs("CoilUFR",coilInnR,coilOutR,0.5*coilSizeY,geo->GetCoilUFRAngle(),90.*deg);
  G4LogicalVolume* coilUFRVolume = new G4LogicalVolume(coilUFRSolid,G4Material::GetMaterial("G4_Cu"),"CoilUFR",0,0,0);
  //coilUFRVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  coilUFRVolume->SetVisAttributes(coilVisAttr);
  G4ThreeVector coilUFRPos = G4ThreeVector(geo->GetCoilUFRPosX(),geo->GetCoilUFRPosY(),geo->GetCoilUFRPosZ());
  new G4PVPlacement(coilRot,coilUFRPos,coilUFRVolume,"CoilUFR",fMotherVolume,false,0,false);

  G4Tubs* coilUBLSolid = new G4Tubs("CoilUBL",coilInnR,coilOutR,0.5*coilSizeY,geo->GetCoilUBLAngle(),90.*deg);
  G4LogicalVolume* coilUBLVolume = new G4LogicalVolume(coilUBLSolid,G4Material::GetMaterial("G4_Cu"),"CoilUBL",0,0,0);
  //coilUBLVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  coilUBLVolume->SetVisAttributes(coilVisAttr);
  G4ThreeVector coilUBLPos = G4ThreeVector(geo->GetCoilUBLPosX(),geo->GetCoilUBLPosY(),geo->GetCoilUBLPosZ());
  new G4PVPlacement(coilRot,coilUBLPos,coilUBLVolume,"CoilUBL",fMotherVolume,false,0,false);

  G4Tubs* coilUBRSolid = new G4Tubs("CoilUBR",coilInnR,coilOutR,0.5*coilSizeY,geo->GetCoilUBRAngle(),90.*deg);
  G4LogicalVolume* coilUBRVolume = new G4LogicalVolume(coilUBRSolid,G4Material::GetMaterial("G4_Cu"),"CoilUBR",0,0,0);
  //coilUBRVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  coilUBRVolume->SetVisAttributes(coilVisAttr);
  G4ThreeVector coilUBRPos = G4ThreeVector(geo->GetCoilUBRPosX(),geo->GetCoilUBRPosY(),geo->GetCoilUBRPosZ());
  new G4PVPlacement(coilRot,coilUBRPos,coilUBRVolume,"CoilUBR",fMotherVolume,false,0,false);

  G4Tubs* coilDFLSolid = new G4Tubs("CoilDFL",coilInnR,coilOutR,0.5*coilSizeY,geo->GetCoilDFLAngle(),90.*deg);
  G4LogicalVolume* coilDFLVolume = new G4LogicalVolume(coilDFLSolid,G4Material::GetMaterial("G4_Cu"),"CoilDFL",0,0,0);
  //coilDFLVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  coilDFLVolume->SetVisAttributes(coilVisAttr);
  G4ThreeVector coilDFLPos = G4ThreeVector(geo->GetCoilDFLPosX(),geo->GetCoilDFLPosY(),geo->GetCoilDFLPosZ());
  new G4PVPlacement(coilRot,coilDFLPos,coilDFLVolume,"CoilDFL",fMotherVolume,false,0,false);

  G4Tubs* coilDFRSolid = new G4Tubs("CoilDFR",coilInnR,coilOutR,0.5*coilSizeY,geo->GetCoilDFRAngle(),90.*deg);
  G4LogicalVolume* coilDFRVolume = new G4LogicalVolume(coilDFRSolid,G4Material::GetMaterial("G4_Cu"),"CoilDFR",0,0,0);
  //coilDFRVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  coilDFRVolume->SetVisAttributes(coilVisAttr);
  G4ThreeVector coilDFRPos = G4ThreeVector(geo->GetCoilDFRPosX(),geo->GetCoilDFRPosY(),geo->GetCoilDFRPosZ());
  new G4PVPlacement(coilRot,coilDFRPos,coilDFRVolume,"CoilDFR",fMotherVolume,false,0,false);

  G4Tubs* coilDBLSolid = new G4Tubs("CoilDBL",coilInnR,coilOutR,0.5*coilSizeY,geo->GetCoilDBLAngle(),90.*deg);
  G4LogicalVolume* coilDBLVolume = new G4LogicalVolume(coilDBLSolid,G4Material::GetMaterial("G4_Cu"),"CoilDBL",0,0,0);
  //coilDBLVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  coilDBLVolume->SetVisAttributes(coilVisAttr);
  G4ThreeVector coilDBLPos = G4ThreeVector(geo->GetCoilDBLPosX(),geo->GetCoilDBLPosY(),geo->GetCoilDBLPosZ());
  new G4PVPlacement(coilRot,coilDBLPos,coilDBLVolume,"CoilDBL",fMotherVolume,false,0,false);

  G4Tubs* coilDBRSolid = new G4Tubs("CoilDBR",coilInnR,coilOutR,0.5*coilSizeY,geo->GetCoilDBRAngle(),90.*deg);
  G4LogicalVolume* coilDBRVolume = new G4LogicalVolume(coilDBRSolid,G4Material::GetMaterial("G4_Cu"),"CoilDBR",0,0,0);
  //coilDBRVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  coilDBRVolume->SetVisAttributes(coilVisAttr);
  G4ThreeVector coilDBRPos = G4ThreeVector(geo->GetCoilDBRPosX(),geo->GetCoilDBRPosY(),geo->GetCoilDBRPosZ());
  new G4PVPlacement(coilRot,coilDBRPos,coilDBRVolume,"CoilDBR",fMotherVolume,false,0,false);

  // Create Slabs

  G4double slabSizeX = geo->GetSlabULSizeX();
  G4double slabSizeY = geo->GetSlabULSizeY();
  G4double slabSizeZ = geo->GetSlabULSizeZ();
  G4Box* slabSolid = new G4Box("Slab",0.5*(slabSizeX-magGap),0.5*(slabSizeY-magGap),0.5*(slabSizeZ-magGap));
  G4LogicalVolume* slabVolume = new G4LogicalVolume(slabSolid,G4Material::GetMaterial("G4_Fe"),"Slab",0,0,0);
  //slabVolume->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));
  slabVolume->SetVisAttributes(slabVisAttr);

  G4ThreeVector slabULPos = G4ThreeVector(geo->GetSlabULPosX(),geo->GetSlabULPosY(),geo->GetSlabULPosZ());
  new G4PVPlacement(0,slabULPos,slabVolume,"SlabUL",fMotherVolume,false,0,false);

  G4ThreeVector slabURPos = G4ThreeVector(geo->GetSlabURPosX(),geo->GetSlabURPosY(),geo->GetSlabURPosZ());
  new G4PVPlacement(0,slabURPos,slabVolume,"SlabUR",fMotherVolume,false,0,false);

  G4ThreeVector slabDLPos = G4ThreeVector(geo->GetSlabDLPosX(),geo->GetSlabDLPosY(),geo->GetSlabDLPosZ());
  new G4PVPlacement(0,slabDLPos,slabVolume,"SlabDL",fMotherVolume,false,0,false);

  G4ThreeVector slabDRPos = G4ThreeVector(geo->GetSlabDRPosX(),geo->GetSlabDRPosY(),geo->GetSlabDRPosZ());
  new G4PVPlacement(0,slabDRPos,slabVolume,"SlabDR",fMotherVolume,false,0,false);

  /*
  // Create rails

  G4double railSizeX = geo->GetRailULSizeX();
  G4double railSizeY = geo->GetRailULSizeY();
  G4double railSizeZ = geo->GetRailULSizeZ();
  G4Box* railSolid = new G4Box("Rail",0.5*(railSizeX-magGap),0.5*(railSizeY-magGap),0.5*(railSizeZ-magGap));
  G4LogicalVolume* railVolume = new G4LogicalVolume(railSolid,G4Material::GetMaterial("G4_Fe"),"Rail",0,0,0);
  railVolume->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));

  G4ThreeVector railULPos = G4ThreeVector(geo->GetRailULPosX(),geo->GetRailULPosY(),geo->GetRailULPosZ());
  new G4PVPlacement(0,railULPos,railVolume,"RailUL",fMotherVolume,false,0,false);

  G4ThreeVector railURPos = G4ThreeVector(geo->GetRailURPosX(),geo->GetRailURPosY(),geo->GetRailURPosZ());
  new G4PVPlacement(0,railURPos,railVolume,"RailUR",fMotherVolume,false,0,false);

  G4ThreeVector railDLPos = G4ThreeVector(geo->GetRailDLPosX(),geo->GetRailDLPosY(),geo->GetRailDLPosZ());
  new G4PVPlacement(0,railDLPos,railVolume,"RailDL",fMotherVolume,false,0,false);

  G4ThreeVector railDRPos = G4ThreeVector(geo->GetRailDRPosX(),geo->GetRailDRPosY(),geo->GetRailDRPosZ());
  new G4PVPlacement(0,railDRPos,railVolume,"RailDR",fMotherVolume,false,0,false);
  */

  // Neoprene sheets

  G4double neopreneSizeX = geo->GetNeopreneULTSizeX();
  G4double neopreneSizeY = geo->GetNeopreneULTSizeY();
  G4double neopreneSizeZ = geo->GetNeopreneULTSizeZ();
  G4Box* neopreneSolid = new G4Box("Neoprene",0.5*(neopreneSizeX-magGap),0.5*(neopreneSizeY-magGap),0.5*(neopreneSizeZ-magGap));
  G4LogicalVolume* neopreneVolume = new G4LogicalVolume(neopreneSolid,G4Material::GetMaterial("G4_NEOPRENE"),"Neoprene",0,0,0);
  //neopreneVolume->SetVisAttributes(G4VisAttributes(G4Colour::White()));
  neopreneVolume->SetVisAttributes(neopreneVisAttr);

  G4ThreeVector neopreneULTPos = G4ThreeVector(geo->GetNeopreneULTPosX(),geo->GetNeopreneULTPosY(),geo->GetNeopreneULTPosZ());
  new G4PVPlacement(0,neopreneULTPos,neopreneVolume,"NeopreneULT",fMotherVolume,false,0,false);

  G4ThreeVector neopreneULBPos = G4ThreeVector(geo->GetNeopreneULBPosX(),geo->GetNeopreneULBPosY(),geo->GetNeopreneULBPosZ());
  new G4PVPlacement(0,neopreneULBPos,neopreneVolume,"NeopreneULB",fMotherVolume,false,0,false);

  G4ThreeVector neopreneURTPos = G4ThreeVector(geo->GetNeopreneURTPosX(),geo->GetNeopreneURTPosY(),geo->GetNeopreneURTPosZ());
  new G4PVPlacement(0,neopreneURTPos,neopreneVolume,"NeopreneURT",fMotherVolume,false,0,false);

  G4ThreeVector neopreneURBPos = G4ThreeVector(geo->GetNeopreneURBPosX(),geo->GetNeopreneURBPosY(),geo->GetNeopreneURBPosZ());
  new G4PVPlacement(0,neopreneURBPos,neopreneVolume,"NeopreneURB",fMotherVolume,false,0,false);

  G4ThreeVector neopreneDLTPos = G4ThreeVector(geo->GetNeopreneDLTPosX(),geo->GetNeopreneDLTPosY(),geo->GetNeopreneDLTPosZ());
  new G4PVPlacement(0,neopreneDLTPos,neopreneVolume,"NeopreneDLT",fMotherVolume,false,0,false);

  G4ThreeVector neopreneDLBPos = G4ThreeVector(geo->GetNeopreneDLBPosX(),geo->GetNeopreneDLBPosY(),geo->GetNeopreneDLBPosZ());
  new G4PVPlacement(0,neopreneDLBPos,neopreneVolume,"NeopreneDLB",fMotherVolume,false,0,false);

  G4ThreeVector neopreneDRTPos = G4ThreeVector(geo->GetNeopreneDRTPosX(),geo->GetNeopreneDRTPosY(),geo->GetNeopreneDRTPosZ());
  new G4PVPlacement(0,neopreneDRTPos,neopreneVolume,"NeopreneDRT",fMotherVolume,false,0,false);

  G4ThreeVector neopreneDRBPos = G4ThreeVector(geo->GetNeopreneDRBPosX(),geo->GetNeopreneDRBPosY(),geo->GetNeopreneDRBPosZ());
  new G4PVPlacement(0,neopreneDRBPos,neopreneVolume,"NeopreneDRB",fMotherVolume,false,0,false);

  // Make upper and lower sections of magnet yoke a sensitive detector
  //G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  //G4String magnetSDName = geo->GetMagnetSensitiveDetectorName();
  //printf("Registering Magnet SD %s\n",magnetSDName.data());
  //MagnetSD* magnetSD = new MagnetSD(magnetSDName);
  //sdMan->AddNewDetector(magnetSD);
  //yokeUpBarVolume->SetSensitiveDetector(magnetSD);
  //yokeDownBarVolume->SetSensitiveDetector(magnetSD);
  //railVolume->SetSensitiveDetector(magnetSD);

}
