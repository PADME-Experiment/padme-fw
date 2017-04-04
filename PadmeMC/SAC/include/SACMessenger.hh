// SACMessenger.hh
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#ifndef SACMessenger_h
#define SACMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class SACDetector;
class SACGeometry;

class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;

class SACMessenger: public G4UImessenger
{
public:
  SACMessenger(SACDetector* );
  ~SACMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:

  SACDetector* fSACDetector;
  SACGeometry* fSACGeometry;

  G4UIdirectory* fSACDetectorDir;

  G4UIcmdWithAnInteger* fSetSACNRowsCmd;
  G4UIcmdWithAnInteger* fSetSACNColsCmd;

  G4UIcmdWithADoubleAndUnit* fSetCrystalSizeCmd;
  G4UIcmdWithADoubleAndUnit* fSetCrystalLengthCmd;

  G4UIcmdWithADoubleAndUnit* fSetCrystalGapCmd;
  G4UIcmdWithADoubleAndUnit* fSetCrystalCoatingCmd;

  G4UIcmdWithADoubleAndUnit* fSetSACFrontFaceZCmd;

};
#endif
