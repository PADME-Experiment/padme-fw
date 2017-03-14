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

  void SetMotherVolume(G4LogicalVolume* v)   { fMotherVolume = v; }
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

private:

  G4UnionSolid* CreateVCFacetGlobalSolid();
  G4UnionSolid* CreateVCFacetExternalSolid();
  G4UnionSolid* CreateVCFacetInternalSolid();

  void CreateECalThinWindow();
  void CreateTargetPipes();

  G4LogicalVolume* fMotherVolume;
  G4LogicalVolume* fGlobalLogicalVolume;

  G4int fChamberExists;
  G4int fChamberIsVisible;

};

#endif
