// EVetoMessenger.hh
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#ifndef EVetoMessenger_h
#define EVetoMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class EVetoDetector;
class EVetoGeometry;
class G4UIdirectory;
class G4UIcommand;

class EVetoMessenger: public G4UImessenger
{
public:
  EVetoMessenger(EVetoDetector* );
  ~EVetoMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:

  EVetoDetector* fEVetoDetector;
  EVetoGeometry* fEVetoGeometry;

  G4UIdirectory* fEVetoDetectorDir;

  G4UIcommand* fSetEVetoNFingersCmd;
  G4UIcommand* fSetFingerSizeCmd;
  G4UIcommand* fSetFingerLengthCmd;

  G4UIcommand* fSetEVetoInnerFaceXCmd;
  G4UIcommand* fSetEVetoFrontFaceZCmd;

};
#endif
