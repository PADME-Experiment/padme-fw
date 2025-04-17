// ETagMessenger.hh
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#ifndef ETagMessenger_h
#define ETagMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class ETagDetector;
class ETagGeometry;
class G4UIdirectory;
//class G4UIcommand;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;

class ETagMessenger: public G4UImessenger
{
public:
  ETagMessenger(ETagDetector* );
  ~ETagMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:

  ETagDetector* fETagDetector;
  ETagGeometry* fETagGeometry;

  G4UIdirectory* fETagDetectorDir;

  //G4UIcommand* fSetETagFrontFaceZCmd;
  G4UIcmdWithADoubleAndUnit* fSetETagFrontFaceZCmd;

  G4UIcmdWithAnInteger* fSetVerboseLevelCmd;

};
#endif
