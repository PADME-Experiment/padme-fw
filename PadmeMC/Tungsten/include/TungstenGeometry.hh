// TungstenGeometry.hh
// --------------------------------------------------------------
// History:
//
// 2107-10-03 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
// --------------------------------------------------------------

#ifndef TungstenGeometry_H
#define TungstenGeometry_H 1

#include "globals.hh"
#include <vector>

class G4LogicalVolume;

class TungstenGeometry
{

public:

  ~TungstenGeometry();
  static TungstenGeometry* GetInstance();
  std::vector<G4String> GetHashTable();

private:

  static TungstenGeometry* fInstance;

protected:

  TungstenGeometry();

public:

  void SetDetectorSetup(G4int s) { fDetectorSetup = s; }
  G4int GetDetectorSetup() { return fDetectorSetup; }

  // Position of center of Tungsten cylinder
  G4double GetTungstenPosX() { return 0.; }
  G4double GetTungstenPosY() { return 0.; }
  G4double GetTungstenPosZ() { return fTungstenFrontFacePosZ+fTungstenSizeZ*0.5; }

  // Size of Tungsten cylinder
  G4double GetTungstenRadius() { return fTungstenRadius; }
  G4double GetTungstenSizeZ() { return fTungstenSizeZ; }
  void SetTungstenSizeZ(G4double s) { fTungstenSizeZ = s; }

  // Set position along Z of Tungsten front face
  G4double GetTungstenFrontFacePosZ() { return fTungstenFrontFacePosZ; }
  void SetTungstenFrontFacePosZ(G4double z) { fTungstenFrontFacePosZ = z; }

private:

  G4int fDetectorSetup;

  G4double fTungstenRadius;
  G4double fTungstenSizeZ;

  G4double fTungstenFrontFacePosZ; // Position along Z axis of Tungsten front face

};

#endif
