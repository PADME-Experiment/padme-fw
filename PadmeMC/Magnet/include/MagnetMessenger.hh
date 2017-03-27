// MagnetMessenger.hh
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#ifndef MagnetMessenger_h
#define MagnetMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class MagnetStructure;
class MagnetGeometry;

class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithoutParameter;

class MagnetMessenger: public G4UImessenger
{
public:

  MagnetMessenger(MagnetStructure* );
  ~MagnetMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:

  MagnetStructure* fMagnetStructure;
  MagnetGeometry* fMagnetGeometry;

  G4UIdirectory* fMagnetDetectorDir;

  //G4UIcmdWithoutParameter* fEnableMagneticFieldCmd;
  //G4UIcmdWithoutParameter* fDisableMagneticFieldCmd;

  //G4UIcmdWithoutParameter* fSetMagneticVolumeVisibleCmd;
  //G4UIcmdWithoutParameter* fSetMagneticVolumeInvisibleCmd;

  //G4UIcommand* fSetVacuumChamberWallThicknessCmd;

  //G4UIcommand* fSetMagneticFieldConstantValueCmd;

};
#endif
