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
#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"

#include "MagnetGeometry.hh"
#include "MagnetSD.hh"

MagnetStructure::MagnetStructure(G4LogicalVolume* motherVolume)
  :fMotherVolume(motherVolume)
{

  // Connect to MagnetMessenger to enable datacard configuration
  fMagnetMessenger = new MagnetMessenger(this);

}

MagnetStructure::~MagnetStructure()
{
  delete fMagnetMessenger;
}

void MagnetStructure::CreateGeometry()
{

  MagnetGeometry* geo = MagnetGeometry::GetInstance();

  // Small gap between magnet yoke's sections
  G4double magGap = geo->GetMagnetGap();

  // Create and position upper part of magnet yoke
  G4double magUpSizeX = geo->GetMagnetUpSizeX();
  G4double magUpSizeY = geo->GetMagnetUpSizeY();
  G4double magUpSizeZ = geo->GetMagnetUpSizeZ();
  G4Box* magUpSolid = new G4Box("MagnetUp",0.5*(magUpSizeX-magGap),0.5*(magUpSizeY-magGap),0.5*(magUpSizeZ-magGap));
  G4LogicalVolume* magUpVolume = new G4LogicalVolume(magUpSolid,G4Material::GetMaterial("G4_Fe"),"MagnetUp",0,0,0);
  magUpVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Green()));
  G4ThreeVector magUpPos = G4ThreeVector(geo->GetMagnetUpPosX(),geo->GetMagnetUpPosY(),geo->GetMagnetUpPosZ());
  new G4PVPlacement(0,magUpPos,magUpVolume,"MagnetUp",fMotherVolume,false,0,false);

  // Create and position lower part of magnet yoke
  G4double magDownSizeX = geo->GetMagnetDownSizeX();
  G4double magDownSizeY = geo->GetMagnetDownSizeY();
  G4double magDownSizeZ = geo->GetMagnetDownSizeZ();
  G4Box* magDownSolid = new G4Box("MagnetDown",0.5*(magDownSizeX-magGap),0.5*(magDownSizeY-magGap),0.5*(magDownSizeZ-magGap));
  G4LogicalVolume* magDownVolume = new G4LogicalVolume(magDownSolid,G4Material::GetMaterial("G4_Fe"),"MagnetDown",0,0,0);
  magDownVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Green()));
  G4ThreeVector magDownPos = G4ThreeVector(geo->GetMagnetDownPosX(),geo->GetMagnetDownPosY(),geo->GetMagnetDownPosZ());
  new G4PVPlacement(0,magDownPos,magDownVolume,"MagnetDown",fMotherVolume,false,0,false);

  // Create and position right part of magnet yoke
  G4double magRightSizeX = geo->GetMagnetRightSizeX();
  G4double magRightSizeY = geo->GetMagnetRightSizeY();
  G4double magRightSizeZ = geo->GetMagnetRightSizeZ();
  G4Box* magRightSolid = new G4Box("MagnetRight",0.5*(magRightSizeX-magGap),0.5*(magRightSizeY-magGap),0.5*(magRightSizeZ-magGap));
  G4LogicalVolume* magRightVolume = new G4LogicalVolume(magRightSolid,G4Material::GetMaterial("G4_Fe"),"MagnetRight",0,0,0);
  magRightVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Green()));
  G4ThreeVector magRightPos = G4ThreeVector(geo->GetMagnetRightPosX(),geo->GetMagnetRightPosY(),geo->GetMagnetRightPosZ());
  new G4PVPlacement(0,magRightPos,magRightVolume,"MagnetRight",fMotherVolume,false,0,false);

  // Create and position left part of magnet yoke
  G4double magLeftSizeX = geo->GetMagnetLeftSizeX();
  G4double magLeftSizeY = geo->GetMagnetLeftSizeY();
  G4double magLeftSizeZ = geo->GetMagnetLeftSizeZ();
  G4Box* magLeftSolid = new G4Box("MagnetLeft",0.5*(magLeftSizeX-magGap),0.5*(magLeftSizeY-magGap),0.5*(magLeftSizeZ-magGap));
  G4LogicalVolume* magLeftVolume = new G4LogicalVolume(magLeftSolid,G4Material::GetMaterial("G4_Fe"),"MagnetLeft",0,0,0);
  magLeftVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Green()));
  G4ThreeVector magLeftPos = G4ThreeVector(geo->GetMagnetLeftPosX(),geo->GetMagnetLeftPosY(),geo->GetMagnetLeftPosZ());
  new G4PVPlacement(0,magLeftPos,magLeftVolume,"MagnetLeft",fMotherVolume,false,0,false);

  // Create and position upper bar part of magnet yoke
  //G4cout << "Bar size " << geo->GetMagnetUpBarSizeX() << " " << geo->GetMagnetUpBarSizeY() << " " << geo->GetMagnetUpBarSizeZ() << G4endl;
  //G4cout << "Bar position " << geo->GetMagnetUpBarPosX() << " " << geo->GetMagnetUpBarPosY() << " " << geo->GetMagnetUpBarPosZ() << G4endl;
  G4double magUpBarSizeX = geo->GetMagnetUpBarSizeX();
  G4double magUpBarSizeY = geo->GetMagnetUpBarSizeY();
  G4double magUpBarSizeZ = geo->GetMagnetUpBarSizeZ();
  G4Box* magUpBarSolid = new G4Box("MagnetUpBar",0.5*(magUpBarSizeX-magGap),0.5*(magUpBarSizeY-magGap),0.5*(magUpBarSizeZ-magGap));
  G4LogicalVolume* magUpBarVolume = new G4LogicalVolume(magUpBarSolid,G4Material::GetMaterial("G4_Fe"),"MagnetUpBar",0,0,0);
  magUpBarVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Grey()));
  G4ThreeVector magUpBarPos = G4ThreeVector(geo->GetMagnetUpBarPosX(),geo->GetMagnetUpBarPosY(),geo->GetMagnetUpBarPosZ());
  new G4PVPlacement(0,magUpBarPos,magUpBarVolume,"MagnetUpBar",fMotherVolume,false,0,false);

  // Create and position lower bar part of magnet yoke
  G4double magDownBarSizeX = geo->GetMagnetDownBarSizeX();
  G4double magDownBarSizeY = geo->GetMagnetDownBarSizeY();
  G4double magDownBarSizeZ = geo->GetMagnetDownBarSizeZ();
  G4Box* magDownBarSolid = new G4Box("MagnetDownBar",0.5*(magDownBarSizeX-magGap),0.5*(magDownBarSizeY-magGap),0.5*(magDownBarSizeZ-magGap));
  G4LogicalVolume* magDownBarVolume = new G4LogicalVolume(magDownBarSolid,G4Material::GetMaterial("G4_Fe"),"MagnetDownBar",0,0,0);
  magDownBarVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Grey()));
  G4ThreeVector magDownBarPos = G4ThreeVector(geo->GetMagnetDownBarPosX(),geo->GetMagnetDownBarPosY(),geo->GetMagnetDownBarPosZ());
  new G4PVPlacement(0,magDownBarPos,magDownBarVolume,"MagnetDownBar",fMotherVolume,false,0,false);

  // Create coils

  G4Box* coilUFSolid = new G4Box("CoilUF",0.5*(geo->GetCoilUFSizeX()-magGap),0.5*(geo->GetCoilUFSizeY()-magGap),0.5*(geo->GetCoilUFSizeZ()-magGap));
  G4LogicalVolume* coilUFVolume = new G4LogicalVolume(coilUFSolid,G4Material::GetMaterial("G4_Cu"),"CoilUF",0,0,0);
  coilUFVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Red()));
  G4ThreeVector coilUFPos = G4ThreeVector(geo->GetCoilUFPosX(),geo->GetCoilUFPosY(),geo->GetCoilUFPosZ());
  new G4PVPlacement(0,coilUFPos,coilUFVolume,"CoilUF",fMotherVolume,false,0,false);

  G4Box* coilUBSolid = new G4Box("CoilUB",0.5*(geo->GetCoilUBSizeX()-magGap),0.5*(geo->GetCoilUBSizeY()-magGap),0.5*(geo->GetCoilUBSizeZ()-magGap));
  G4LogicalVolume* coilUBVolume = new G4LogicalVolume(coilUBSolid,G4Material::GetMaterial("G4_Cu"),"CoilUB",0,0,0);
  coilUBVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Red()));
  G4ThreeVector coilUBPos = G4ThreeVector(geo->GetCoilUBPosX(),geo->GetCoilUBPosY(),geo->GetCoilUBPosZ());
  new G4PVPlacement(0,coilUBPos,coilUBVolume,"CoilUB",fMotherVolume,false,0,false);

  G4Box* coilULSolid = new G4Box("CoilUL",0.5*(geo->GetCoilULSizeX()-magGap),0.5*(geo->GetCoilULSizeY()-magGap),0.5*(geo->GetCoilULSizeZ()-magGap));
  G4LogicalVolume* coilULVolume = new G4LogicalVolume(coilULSolid,G4Material::GetMaterial("G4_Cu"),"CoilUL",0,0,0);
  coilULVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Red()));
  G4ThreeVector coilULPos = G4ThreeVector(geo->GetCoilULPosX(),geo->GetCoilULPosY(),geo->GetCoilULPosZ());
  new G4PVPlacement(0,coilULPos,coilULVolume,"CoilUL",fMotherVolume,false,0,false);

  G4Box* coilURSolid = new G4Box("CoilUR",0.5*(geo->GetCoilURSizeX()-magGap),0.5*(geo->GetCoilURSizeY()-magGap),0.5*(geo->GetCoilURSizeZ()-magGap));
  G4LogicalVolume* coilURVolume = new G4LogicalVolume(coilURSolid,G4Material::GetMaterial("G4_Cu"),"CoilUR",0,0,0);
  coilURVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Red()));
  G4ThreeVector coilURPos = G4ThreeVector(geo->GetCoilURPosX(),geo->GetCoilURPosY(),geo->GetCoilURPosZ());
  new G4PVPlacement(0,coilURPos,coilURVolume,"CoilUR",fMotherVolume,false,0,false);

  G4Box* coilDFSolid = new G4Box("CoilDF",0.5*(geo->GetCoilDFSizeX()-magGap),0.5*(geo->GetCoilDFSizeY()-magGap),0.5*(geo->GetCoilDFSizeZ()-magGap));
  G4LogicalVolume* coilDFVolume = new G4LogicalVolume(coilDFSolid,G4Material::GetMaterial("G4_Cu"),"CoilDF",0,0,0);
  coilDFVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Red()));
  G4ThreeVector coilDFPos = G4ThreeVector(geo->GetCoilDFPosX(),geo->GetCoilDFPosY(),geo->GetCoilDFPosZ());
  new G4PVPlacement(0,coilDFPos,coilDFVolume,"CoilDF",fMotherVolume,false,0,false);

  G4Box* coilDBSolid = new G4Box("CoilDB",0.5*(geo->GetCoilDBSizeX()-magGap),0.5*(geo->GetCoilDBSizeY()-magGap),0.5*(geo->GetCoilDBSizeZ()-magGap));
  G4LogicalVolume* coilDBVolume = new G4LogicalVolume(coilDBSolid,G4Material::GetMaterial("G4_Cu"),"CoilDB",0,0,0);
  coilDBVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Red()));
  G4ThreeVector coilDBPos = G4ThreeVector(geo->GetCoilDBPosX(),geo->GetCoilDBPosY(),geo->GetCoilDBPosZ());
  new G4PVPlacement(0,coilDBPos,coilDBVolume,"CoilDB",fMotherVolume,false,0,false);

  G4Box* coilDLSolid = new G4Box("CoilDL",0.5*(geo->GetCoilDLSizeX()-magGap),0.5*(geo->GetCoilDLSizeY()-magGap),0.5*(geo->GetCoilDLSizeZ()-magGap));
  G4LogicalVolume* coilDLVolume = new G4LogicalVolume(coilDLSolid,G4Material::GetMaterial("G4_Cu"),"CoilDL",0,0,0);
  coilDLVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Red()));
  G4ThreeVector coilDLPos = G4ThreeVector(geo->GetCoilDLPosX(),geo->GetCoilDLPosY(),geo->GetCoilDLPosZ());
  new G4PVPlacement(0,coilDLPos,coilDLVolume,"CoilDL",fMotherVolume,false,0,false);

  G4Box* coilDRSolid = new G4Box("CoilDR",0.5*(geo->GetCoilDRSizeX()-magGap),0.5*(geo->GetCoilDRSizeY()-magGap),0.5*(geo->GetCoilDRSizeZ()-magGap));
  G4LogicalVolume* coilDRVolume = new G4LogicalVolume(coilDRSolid,G4Material::GetMaterial("G4_Cu"),"CoilDR",0,0,0);
  coilDRVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Red()));
  G4ThreeVector coilDRPos = G4ThreeVector(geo->GetCoilDRPosX(),geo->GetCoilDRPosY(),geo->GetCoilDRPosZ());
  new G4PVPlacement(0,coilDRPos,coilDRVolume,"CoilDR",fMotherVolume,false,0,false);

  G4double coilInnR = geo->GetCoilInnerRadius();
  G4double coilOutR = geo->GetCoilOuterRadius();
  G4double coilSizeX = geo->GetCoilSizeX();
  G4RotationMatrix* coilRot = new G4RotationMatrix();
  coilRot->rotateY(90.*deg);

  G4Tubs* coilUFLSolid = new G4Tubs("CoilUFL",coilInnR,coilOutR,0.5*coilSizeX,geo->GetCoilUFLAngle(),90.*deg);
  G4LogicalVolume* coilUFLVolume = new G4LogicalVolume(coilUFLSolid,G4Material::GetMaterial("G4_Cu"),"CoilUFL",0,0,0);
  coilUFLVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Red()));
  G4ThreeVector coilUFLPos = G4ThreeVector(geo->GetCoilUFLPosX(),geo->GetCoilUFLPosY(),geo->GetCoilUFLPosZ());
  new G4PVPlacement(coilRot,coilUFLPos,coilUFLVolume,"CoilUFL",fMotherVolume,false,0,false);

  G4Tubs* coilUFRSolid = new G4Tubs("CoilUFR",coilInnR,coilOutR,0.5*coilSizeX,geo->GetCoilUFRAngle(),90.*deg);
  G4LogicalVolume* coilUFRVolume = new G4LogicalVolume(coilUFRSolid,G4Material::GetMaterial("G4_Cu"),"CoilUFR",0,0,0);
  coilUFRVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Red()));
  G4ThreeVector coilUFRPos = G4ThreeVector(geo->GetCoilUFRPosX(),geo->GetCoilUFRPosY(),geo->GetCoilUFRPosZ());
  new G4PVPlacement(coilRot,coilUFRPos,coilUFRVolume,"CoilUFR",fMotherVolume,false,0,false);

  G4Tubs* coilUBLSolid = new G4Tubs("CoilUBL",coilInnR,coilOutR,0.5*coilSizeX,geo->GetCoilUBLAngle(),90.*deg);
  G4LogicalVolume* coilUBLVolume = new G4LogicalVolume(coilUBLSolid,G4Material::GetMaterial("G4_Cu"),"CoilUBL",0,0,0);
  coilUBLVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Red()));
  G4ThreeVector coilUBLPos = G4ThreeVector(geo->GetCoilUBLPosX(),geo->GetCoilUBLPosY(),geo->GetCoilUBLPosZ());
  new G4PVPlacement(coilRot,coilUBLPos,coilUBLVolume,"CoilUBL",fMotherVolume,false,0,false);

  G4Tubs* coilUBRSolid = new G4Tubs("CoilUBR",coilInnR,coilOutR,0.5*coilSizeX,geo->GetCoilUBRAngle(),90.*deg);
  G4LogicalVolume* coilUBRVolume = new G4LogicalVolume(coilUBRSolid,G4Material::GetMaterial("G4_Cu"),"CoilUBR",0,0,0);
  coilUBRVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Red()));
  G4ThreeVector coilUBRPos = G4ThreeVector(geo->GetCoilUBRPosX(),geo->GetCoilUBRPosY(),geo->GetCoilUBRPosZ());
  new G4PVPlacement(coilRot,coilUBRPos,coilUBRVolume,"CoilUBR",fMotherVolume,false,0,false);

  G4Tubs* coilDFLSolid = new G4Tubs("CoilDFL",coilInnR,coilOutR,0.5*coilSizeX,geo->GetCoilDFLAngle(),90.*deg);
  G4LogicalVolume* coilDFLVolume = new G4LogicalVolume(coilDFLSolid,G4Material::GetMaterial("G4_Cu"),"CoilDFL",0,0,0);
  coilDFLVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Red()));
  G4ThreeVector coilDFLPos = G4ThreeVector(geo->GetCoilDFLPosX(),geo->GetCoilDFLPosY(),geo->GetCoilDFLPosZ());
  new G4PVPlacement(coilRot,coilDFLPos,coilDFLVolume,"CoilDFL",fMotherVolume,false,0,false);

  G4Tubs* coilDFRSolid = new G4Tubs("CoilDFR",coilInnR,coilOutR,0.5*coilSizeX,geo->GetCoilDFRAngle(),90.*deg);
  G4LogicalVolume* coilDFRVolume = new G4LogicalVolume(coilDFRSolid,G4Material::GetMaterial("G4_Cu"),"CoilDFR",0,0,0);
  coilDFRVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Red()));
  G4ThreeVector coilDFRPos = G4ThreeVector(geo->GetCoilDFRPosX(),geo->GetCoilDFRPosY(),geo->GetCoilDFRPosZ());
  new G4PVPlacement(coilRot,coilDFRPos,coilDFRVolume,"CoilDFR",fMotherVolume,false,0,false);

  G4Tubs* coilDBLSolid = new G4Tubs("CoilDBL",coilInnR,coilOutR,0.5*coilSizeX,geo->GetCoilDBLAngle(),90.*deg);
  G4LogicalVolume* coilDBLVolume = new G4LogicalVolume(coilDBLSolid,G4Material::GetMaterial("G4_Cu"),"CoilDBL",0,0,0);
  coilDBLVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Red()));
  G4ThreeVector coilDBLPos = G4ThreeVector(geo->GetCoilDBLPosX(),geo->GetCoilDBLPosY(),geo->GetCoilDBLPosZ());
  new G4PVPlacement(coilRot,coilDBLPos,coilDBLVolume,"CoilDBL",fMotherVolume,false,0,false);

  G4Tubs* coilDBRSolid = new G4Tubs("CoilDBR",coilInnR,coilOutR,0.5*coilSizeX,geo->GetCoilDBRAngle(),90.*deg);
  G4LogicalVolume* coilDBRVolume = new G4LogicalVolume(coilDBRSolid,G4Material::GetMaterial("G4_Cu"),"CoilDBR",0,0,0);
  coilDBRVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Red()));
  G4ThreeVector coilDBRPos = G4ThreeVector(geo->GetCoilDBRPosX(),geo->GetCoilDBRPosY(),geo->GetCoilDBRPosZ());
  new G4PVPlacement(coilRot,coilDBRPos,coilDBRVolume,"CoilDBR",fMotherVolume,false,0,false);

  // Create Slabs

  G4double slabSizeX = geo->GetSlabULSizeX();
  G4double slabSizeY = geo->GetSlabULSizeY();
  G4double slabSizeZ = geo->GetSlabULSizeZ();
  G4Box* slabSolid = new G4Box("Slab",0.5*(slabSizeX-magGap),0.5*(slabSizeY-magGap),0.5*(slabSizeZ-magGap));
  G4LogicalVolume* slabVolume = new G4LogicalVolume(slabSolid,G4Material::GetMaterial("G4_Fe"),"Slab",0,0,0);
  slabVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Grey()));

  G4ThreeVector slabULPos = G4ThreeVector(geo->GetSlabULPosX(),geo->GetSlabULPosY(),geo->GetSlabULPosZ());
  new G4PVPlacement(0,slabULPos,slabVolume,"SlabUL",fMotherVolume,false,0,false);

  G4ThreeVector slabURPos = G4ThreeVector(geo->GetSlabURPosX(),geo->GetSlabURPosY(),geo->GetSlabURPosZ());
  new G4PVPlacement(0,slabURPos,slabVolume,"SlabUR",fMotherVolume,false,0,false);

  G4ThreeVector slabDLPos = G4ThreeVector(geo->GetSlabDLPosX(),geo->GetSlabDLPosY(),geo->GetSlabDLPosZ());
  new G4PVPlacement(0,slabDLPos,slabVolume,"SlabDL",fMotherVolume,false,0,false);

  G4ThreeVector slabDRPos = G4ThreeVector(geo->GetSlabDRPosX(),geo->GetSlabDRPosY(),geo->GetSlabDRPosZ());
  new G4PVPlacement(0,slabDRPos,slabVolume,"SlabDR",fMotherVolume,false,0,false);

  // Create rails

  G4double railSizeX = geo->GetRailULSizeX();
  G4double railSizeY = geo->GetRailULSizeY();
  G4double railSizeZ = geo->GetRailULSizeZ();
  G4Box* railSolid = new G4Box("Rail",0.5*(railSizeX-magGap),0.5*(railSizeY-magGap),0.5*(railSizeZ-magGap));
  G4LogicalVolume* railVolume = new G4LogicalVolume(railSolid,G4Material::GetMaterial("G4_Fe"),"Rail",0,0,0);
  railVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::Grey()));

  G4ThreeVector railULPos = G4ThreeVector(geo->GetRailULPosX(),geo->GetRailULPosY(),geo->GetRailULPosZ());
  new G4PVPlacement(0,railULPos,railVolume,"RailUL",fMotherVolume,false,0,false);

  G4ThreeVector railURPos = G4ThreeVector(geo->GetRailURPosX(),geo->GetRailURPosY(),geo->GetRailURPosZ());
  new G4PVPlacement(0,railURPos,railVolume,"RailUR",fMotherVolume,false,0,false);

  G4ThreeVector railDLPos = G4ThreeVector(geo->GetRailDLPosX(),geo->GetRailDLPosY(),geo->GetRailDLPosZ());
  new G4PVPlacement(0,railDLPos,railVolume,"RailDL",fMotherVolume,false,0,false);

  G4ThreeVector railDRPos = G4ThreeVector(geo->GetRailDRPosX(),geo->GetRailDRPosY(),geo->GetRailDRPosZ());
  new G4PVPlacement(0,railDRPos,railVolume,"RailDR",fMotherVolume,false,0,false);

  // Neoprene sheets

  G4double neopreneSizeX = geo->GetNeopreneULTSizeX();
  G4double neopreneSizeY = geo->GetNeopreneULTSizeY();
  G4double neopreneSizeZ = geo->GetNeopreneULTSizeZ();
  G4Box* neopreneSolid = new G4Box("Neoprene",0.5*(neopreneSizeX-magGap),0.5*(neopreneSizeY-magGap),0.5*(neopreneSizeZ-magGap));
  G4LogicalVolume* neopreneVolume = new G4LogicalVolume(neopreneSolid,G4Material::GetMaterial("G4_NEOPRENE"),"Neoprene",0,0,0);
  neopreneVolume->SetVisAttributes(G4VisAttributes::G4VisAttributes(G4Colour::White()));

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
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String magnetSDName = geo->GetMagnetSensitiveDetectorName();
  printf("Registering Magnet SD %s\n",magnetSDName.data());
  MagnetSD* magnetSD = new MagnetSD(magnetSDName);
  sdMan->AddNewDetector(magnetSD);
  magUpBarVolume->SetSensitiveDetector(magnetSD);
  magDownBarVolume->SetSensitiveDetector(magnetSD);
  railVolume->SetSensitiveDetector(magnetSD);

}
