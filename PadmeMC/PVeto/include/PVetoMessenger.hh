// PVetoMessenger.hh
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#ifndef PVetoMessenger_h
#define PVetoMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class PVetoDetector;
class PVetoGeometry;
class G4UIdirectory;
class G4UIcommand;

class PVetoMessenger: public G4UImessenger
{
public:
  PVetoMessenger(PVetoDetector* );
  ~PVetoMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:

  PVetoDetector* fPVetoDetector;
  PVetoGeometry* fPVetoGeometry;

  G4UIdirectory* fPVetoDetectorDir;

  G4UIcommand* fSetPVetoNFingersCmd;
  G4UIcommand* fSetFingerSizeCmd;
  G4UIcommand* fSetFingerLengthCmd;

  G4UIcommand* fSetPVetoInnerFaceXCmd;
  G4UIcommand* fSetPVetoFrontFaceZCmd;

};
#endif
