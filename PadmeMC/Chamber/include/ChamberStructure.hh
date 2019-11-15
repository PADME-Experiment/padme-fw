// ChamberStructure.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-06-10
// --------------------------------------------------------------

#ifndef ChamberStructure_H
#define ChamberStructure_H 1

#include "globals.hh"
#include "G4ThreeVector.hh"

class G4LogicalVolume;
class G4UnionSolid;

class ChamberStructure
{

public:

  ~ChamberStructure();
  ChamberStructure(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void SetCrossMotherVolume(G4LogicalVolume* v) { fCrossMotherVolume = v; }
  void CreateGeometry();

  //G4double GetChamberMostExternalX();
  //G4double GetChamberMostAdvancedZ();
  //G4double GetChamberBackFaceAngle();
  //G4double GetChamberBackFaceThickness();
  //G4ThreeVector GetChamberBackFaceCorner();

  //G4double GetChamberInnerX();
  //G4double GetChamberInnerY();
  //G4double GetChamberInnerZ();

  G4LogicalVolume* GetChamberLogicalVolume() { return fGlobalLogicalVolume; }

  void EnableChamber()  { fChamberExists = 1; }
  void DisableChamber() { fChamberExists = 0; }

  void SetChamberVisible()   { fChamberIsVisible = 1; }
  void SetChamberInvisible() { fChamberIsVisible = 0; }

  // Define displacement of Cross region along Z due to position of magnetic volume
  void SetCrossDisplacePosZ(G4double z) { fCrossDisplacePosZ = z; }

private:

  G4UnionSolid* CreateVCFacetGlobalSolid();
  G4UnionSolid* CreateVCFacetExternalSolid();
  G4UnionSolid* CreateVCFacetInternalSolid();

  void CreateECalAlThinWindow();
  void CreateECalCarbonThinWindow();
  void CreateTargetPipes();
  void CreateJunctionPipe();
  void CreateBTFJunction(); //M. Raggi 20/03/2019
  void CreatePortholeCaps();
  void CreateTPixPortholeCap();

  G4LogicalVolume* fMotherVolume;
  G4LogicalVolume* fCrossMotherVolume;
  G4LogicalVolume* fGlobalLogicalVolume;

  G4double fCrossDisplacePosZ; // Displacement of Cross region along Z due to positioning inside magnetic volume

  G4int fChamberExists;
  G4int fChamberIsVisible;

};

#endif
