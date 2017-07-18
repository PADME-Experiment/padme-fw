// TargetMessenger.hh
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#ifndef TargetMessenger_h
#define TargetMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class TargetDetector;
class TargetGeometry;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;

class TargetMessenger: public G4UImessenger
{
public:
  TargetMessenger(TargetDetector* );
  ~TargetMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:

  TargetDetector* fTargetDetector;
  TargetGeometry* fTargetGeometry;

  G4UIdirectory* fTargetDetectorDir;

  G4UIcmdWithADoubleAndUnit* fSetTargetFrontFaceZCmd;
  G4UIcmdWithADoubleAndUnit* fSetTargetSizeCmd;
  G4UIcmdWithADoubleAndUnit* fSetTargetThicknessCmd;

  G4UIcmdWithoutParameter* fEnableFastDigitizationCmd;
  G4UIcmdWithoutParameter* fDisableFastDigitizationCmd;

  G4UIcmdWithoutParameter* fEnableSaveWaveformToDigiCmd;
  G4UIcmdWithoutParameter* fDisableSaveWaveformToDigiCmd;

};
#endif
