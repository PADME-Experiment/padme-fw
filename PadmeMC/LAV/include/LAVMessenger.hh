// LAVMessenger.hh
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#ifndef LAVMessenger_h
#define LAVMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class LAVDetector;
class LAVGeometry;
class G4UIdirectory;
class G4UIcommand;

class LAVMessenger: public G4UImessenger
{
public:
  LAVMessenger(LAVDetector* );
  ~LAVMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:

  LAVDetector* fLAVDetector;
  LAVGeometry* fLAVGeometry;

  G4UIdirectory* fLAVDetectorDir;

  G4UIcommand* fSetLAVInnerRadiusCmd;
  G4UIcommand* fSetLAVOuterRadiusCmd;
  G4UIcommand* fSetLAVZLengthCmd;

  G4UIcommand* fSetLAVFrontFaceZCmd;

};
#endif
