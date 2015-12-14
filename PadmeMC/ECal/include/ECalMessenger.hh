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
class ECalGeometryParameters;
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
  ECalGeometryParameters* fECalGeometry;

  G4UIdirectory* fECalDetectorDir;

  G4UIcommand* fSetECalGeometryCmd;
  G4UIcommand* fSetECalFrontFaceZCmd;
  G4UIcommand* fSetCrystalSizeCmd;


  //G4UIcmdWithoutParameter* fEnableAllStationsCmd;

  //G4UIcommand* fEnableLayerCmd;

  //G4UIcmdWithAString* fSelectBlockSimulationCmd;

};
#endif
