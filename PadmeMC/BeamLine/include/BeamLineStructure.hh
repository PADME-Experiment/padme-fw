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

// Magnetic field
//#include "G4FieldManager.hh"
//#include "G4TransportationManager.hh"
//#include "G4Mag_UsualEqRhs.hh"
//#include "G4MagIntegratorStepper.hh"
//#include "G4ChordFinder.hh"

//#include "globals.hh"
#include "G4MagneticField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"

class G4LogicalVolume;
class G4UnionSolid;

class BeamLineStructure
{

public:

  ~BeamLineStructure();
  BeamLineStructure(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v)   { fMotherVolume = v; }
  void CreateGeometry();

  //G4double GetBeamLineMostExternalX();
  //G4double GetBeamLineMostAdvancedZ();
  //G4double GetBeamLineBackFaceAngle();
  //G4double GetBeamLineBackFaceThickness();
  //G4ThreeVector GetBeamLineBackFaceCorner();

  //G4double GetBeamLineInnerX();
  //G4double GetBeamLineInnerY();
  //G4double GetBeamLineInnerZ();

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
  //  void CreateTPixPortholeCap();

  G4LogicalVolume* fMotherVolume;
  G4LogicalVolume* fGlobalLogicalVolume;
  G4MagneticField* fmagField;
  double fBz;
  G4int fBeamLineExists;
  G4int fBeamLineIsVisible;
};

#endif
