// TungstenDetector.hh
// --------------------------------------------------------------
// History:
//
// 2017-10-03 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#ifndef TungstenDetector_H
#define TungstenDetector_H 1

#include "globals.hh"
#include "G4LogicalVolume.hh"

#include "TungstenGeometry.hh"

class TungstenMessenger;

class TungstenDetector
{

public:

  ~TungstenDetector();
  TungstenDetector(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void CreateGeometry();

  G4double GetTungstenFrontFaceZ() { return TungstenGeometry::GetInstance()->GetTungstenFrontFacePosZ(); }
  G4double GetTungstenLength() { return TungstenGeometry::GetInstance()->GetTungstenSizeZ(); }

  // Define displacement of Tungsten along Z due to position of magnetic volume
  void SetTungstenDisplacePosZ(G4double z) { fTungstenDisplacePosZ = z; }

private:

  G4LogicalVolume* fMotherVolume;

  TungstenMessenger* fTungstenMessenger;

  G4double fTungstenDisplacePosZ; // Displacement of Tungsten along Z due to positioning inside magnetic volume

};

#endif
