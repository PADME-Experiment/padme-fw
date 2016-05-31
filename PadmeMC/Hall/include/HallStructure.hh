// HallStructure.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-05-31
// --------------------------------------------------------------

#ifndef HallStructure_H
#define HallStructure_H 1

#include "globals.hh"

class G4LogicalVolume;

class HallStructure
{

public:

  ~HallStructure();
  HallStructure(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void CreateGeometry();

private:

  G4LogicalVolume* fMotherVolume;

};

#endif
