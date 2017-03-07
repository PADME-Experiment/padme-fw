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
  G4double GetChamberMostExternalX();
  G4double GetChamberMostAdvancedZ();
  //G4LogicalVolume* GetChamberInternalLogicalVolume() { return fInternalLogicalVolume; }
  G4LogicalVolume* GetChamberLogicalVolume() { return fGlobalLogicalVolume; }

private:

  G4UnionSolid* CreateVCGlobalSolid();
  G4UnionSolid* CreateVCExternalSolid();
  G4UnionSolid* CreateVCInternalSolid();

  G4UnionSolid* CreateVCFacetGlobalSolid();
  G4UnionSolid* CreateVCFacetExternalSolid();
  G4UnionSolid* CreateVCFacetInternalSolid();

  G4LogicalVolume* fMotherVolume;
  //G4LogicalVolume* fInternalLogicalVolume;
  //G4LogicalVolume* fExternalLogicalVolume;
  G4LogicalVolume* fGlobalLogicalVolume;
  G4LogicalVolume* fSteelShellLogicalVolume;

};

#endif
