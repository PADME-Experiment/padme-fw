// ECalMessenger.hh
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#ifndef ECalMessenger_h
#define ECalMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class ECalDetector;
class ECalGeometry;
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;

class ECalMessenger: public G4UImessenger
{
public:
  ECalMessenger(ECalDetector* );
  ~ECalMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:

  ECalDetector* fECalDetector;
  ECalGeometry* fECalGeometry;

  G4UIdirectory* fECalDetectorDir;

  G4UIcommand* fSetECalGeometryCmd;
  G4UIcommand* fSetECalFrontFaceZCmd;
  G4UIcommand* fSetECalInnerRadiusCmd;
  G4UIcommand* fSetECalOuterRadiusCmd;
  G4UIcommand* fSetCrystalSizeCmd;

};
#endif
