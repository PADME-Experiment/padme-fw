// BeamLineStructure.hh
// --------------------------------------------------------------
// History:
//
// Created by M. Raggi (mauro.raggi@roma1.infn.it) 2019-03-07
// --------------------------------------------------------------

#ifndef BeamLineStructure_H
#define BeamLineStructure_H 1

#include "globals.hh"
#include "G4ThreeVector.hh"


class G4LogicalVolume;
class G4UnionSolid;

class BeamLineStructure
{

public:

  ~BeamLineStructure();
  BeamLineStructure(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v)   { fMotherVolume = v; }
  void CreateGeometry();

  G4LogicalVolume* GetBeamLineLogicalVolume() { return fGlobalLogicalVolume; }

  void EnableBeamLine()  { fBeamLineExists = 1; }
  void DisableBeamLine() { fBeamLineExists = 0; }

  void SetBeamLineVisible()   { fBeamLineIsVisible = 1; }
  void SetBeamLineInvisible() { fBeamLineIsVisible = 0; }
  double GetFieldValue(){ return fBz; };

private:

  G4UnionSolid* CreateVCFacetGlobalSolid();
  G4UnionSolid* CreateVCFacetExternalSolid();
  G4UnionSolid* CreateVCFacetInternalSolid();

  void CreateBeThinWindow();
  void CreateMagnetPipe();
  void CreateJunctionRegion();
  void CreateDHSTB002Magnet();

  G4LogicalVolume* fMotherVolume;
  G4LogicalVolume* fGlobalLogicalVolume;
  //  G4MagneticField* fmagField;
  double fBz;
  G4int fBeamLineExists;
  G4int fBeamLineIsVisible;

  G4double UpStreamFlangePosX;
  G4double UpStreamFlangePosZ;

};

#endif
