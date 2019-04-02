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
  void CreateBeamLine();
  void CreateDHSTB002Magnet();

  G4LogicalVolume* fMotherVolume;
  //G4LogicalVolume* fGlobalLogicalVolume;
  G4LogicalVolume* fBeWindowVolume;

  //G4int fBeamLineExists;
  G4int fBeamLineIsVisible;

  BeamLineMessenger* fBeamLineMessenger;

};

#endif
