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
class G4UIcmdWithAnInteger;
class G4UIcmdWithABool;

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

  G4UIcommand* fSetECalNRowsCmd;
  G4UIcommand* fSetECalNColsCmd;

  G4UIcmdWithAnInteger* fSetCrystalMapIdCmd;

  G4UIcommand* fSetCrystalSizeCmd;
  G4UIcommand* fSetCrystalLengthCmd;

  G4UIcommand* fSetCrystalGapCmd;

  G4UIcommand* fSetCrystalCoatingCmd;

  G4UIcommand* fSetTedlarThickCmd;

  G4UIcommand* fSetECalFrontFaceZCmd;

  G4UIcmdWithABool* fEnablePanelCmd;
  G4UIcommand* fSetECalPanelThickCmd;
  G4UIcommand* fSetECalPanelGapCmd;

};
#endif
