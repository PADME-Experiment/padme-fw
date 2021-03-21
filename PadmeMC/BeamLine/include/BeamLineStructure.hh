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
#include "G4RotationMatrix.hh"

#include "BeamLineGeometry.hh"
#include "BeamFlagSD.hh"  

class G4LogicalVolume;
class G4UnionSolid;
//class G4RotationMatrix;
class QuadSetup;
class BeamLineMessenger;

class BeamLineStructure
{

public:

  ~BeamLineStructure();
  BeamLineStructure(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void CreateGeometry();

  //G4LogicalVolume* GetBeamLineLogicalVolume() { return fGlobalLogicalVolume; }

  //void EnableBeamLine()  { fBeamLineExists = 1; }
  //void DisableBeamLine() { fBeamLineExists = 0; }

  void SetBeamLineVisible()   { fBeamLineIsVisible = 1; }
  void SetBeamLineInvisible() { fBeamLineIsVisible = 0; }

private:

  void CreateBeThinWindow();
  void CreateMylarThinWindow(); //Raggi 06/2020

  void CreateBeamLine();
  void CreateBeamLine2020();  //Raggi 06/2020

  void CreateDHSTB002Magnet();
  //  void CreateQuadMagnets();
  G4LogicalVolume* CreateQuadMagnets(G4double , G4double , G4double ,G4ThreeVector , G4RotationMatrix*);

  G4LogicalVolume* fMotherVolume;
  //G4LogicalVolume* fGlobalLogicalVolume;
  G4LogicalVolume* fBeWindowVolume;
  G4LogicalVolume* fMylarWindowVolume;
  G4LogicalVolume* fBeamFlag1Volume;
  G4LogicalVolume* fBeamFlag2Volume;
  QuadSetup * fMagneticFieldManager;
  //G4int fBeamLineExists;
  G4int fBeamLineIsVisible; 
  BeamLineMessenger* fBeamLineMessenger;

  //
  G4String BeamFlag2SDName;
  BeamLineGeometry* geo;
  BeamFlagSD* beamFlagSD;

};

#endif
