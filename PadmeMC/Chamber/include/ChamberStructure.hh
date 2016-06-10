// ChamberStructure.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-06-10
// --------------------------------------------------------------

#ifndef ChamberStructure_H
#define ChamberStructure_H 1

#include "globals.hh"

class G4LogicalVolume;

class ChamberStructure
{

public:

  ~ChamberStructure();
  ChamberStructure(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void CreateGeometry();

private:

  G4LogicalVolume* fMotherVolume;

};

#endif
