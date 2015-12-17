// HEPVetoMessenger.hh
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#ifndef HEPVetoMessenger_h
#define HEPVetoMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class HEPVetoDetector;
class HEPVetoGeometry;
class G4UIdirectory;
class G4UIcommand;

class HEPVetoMessenger: public G4UImessenger
{
public:
  HEPVetoMessenger(HEPVetoDetector* );
  ~HEPVetoMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:

  HEPVetoDetector* fHEPVetoDetector;
  HEPVetoGeometry* fHEPVetoGeometry;

  G4UIdirectory* fHEPVetoDetectorDir;

  G4UIcommand* fSetHEPVetoNFingersCmd;
  G4UIcommand* fSetFingerSizeCmd;
  G4UIcommand* fSetFingerLengthCmd;

  //G4UIcommand* fSetHEPVetoPosXCmd;
  G4UIcommand* fSetHEPVetoPosYCmd;
  G4UIcommand* fSetHEPVetoPosZCmd;

  G4UIcommand* fSetHEPVetoRotXCmd;
  //G4UIcommand* fSetHEPVetoRotYCmd;
  //G4UIcommand* fSetHEPVetoRotZCmd;

};
#endif
