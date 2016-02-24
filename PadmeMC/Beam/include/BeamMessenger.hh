// BeamMessenger.hh
// --------------------------------------------------------------
// History:
//
// 2016-02-11 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#ifndef BeamMessenger_h
#define BeamMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class G4UIdirectory;

class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3Vector;
class G4UIcmdWithAString;

class BeamGenerator;
class BeamParameters;

class BeamMessenger: public G4UImessenger
{
public:
  BeamMessenger(BeamGenerator*);
  ~BeamMessenger();

  void SetNewValue(G4UIcommand*, G4String);

  G4String GetCurrentValue(G4UIcommand*);

private:

  BeamGenerator*  fBeamGenerator;
  BeamParameters* fBeamParameters;

  G4UIdirectory* fBeamGeneratorDir;

  G4UIcmdWithAnInteger* fSetNPositronsPerBunchCmd;
  G4UIcmdWithABool* fEnableNPositronsPerBunchSpreadCmd;

  G4UIcmdWithABool* fEnableBunchStructureCmd;
  G4UIcmdWithADoubleAndUnit* fSetBunchTimeLengthCmd;
  G4UIcmdWithADoubleAndUnit* fSetMicroBunchTimeLengthCmd;
  G4UIcmdWithADoubleAndUnit* fSetMicroBunchTimeDelayCmd;

  G4UIcmdWithADoubleAndUnit* fSetBeamCenterPosXCmd;
  G4UIcmdWithADoubleAndUnit* fSetBeamCenterPosYCmd;
  G4UIcmdWithABool* fEnableBeamCenterPosSpreadCmd;
  G4UIcmdWithADoubleAndUnit* fSetBeamCenterPosXSpreadCmd;
  G4UIcmdWithADoubleAndUnit* fSetBeamCenterPosYSpreadCmd;

  G4UIcmdWithADoubleAndUnit* fSetBeamMomentumCmd;
  G4UIcmdWithABool* fEnableBeamMomentumSpreadCmd;
  G4UIcmdWithADoubleAndUnit* fSetBeamMomentumSpreadCmd;

  G4UIcmdWith3Vector* fSetBeamDirectionCmd;
  G4UIcmdWithABool* fEnableBeamEmittanceCmd;
  G4UIcmdWithADouble* fSetBeamEmittanceXCmd;
  G4UIcmdWithADouble* fSetBeamEmittanceYCmd;
  
  G4UIcmdWithAnInteger* fSetNUbosonDecaysPerBunchCmd;
  G4UIcmdWithADoubleAndUnit* fSetUbosonMassCmd;
    
  G4UIcmdWithAnInteger* fSetNThreePhotonDecaysPerBunchCmd;
  G4UIcmdWithAString* fSetThreePhotonDecaysFilenameCmd;

  G4UIcmdWithABool* fEnableCalibRunCmd;
  G4UIcmdWithADoubleAndUnit* fSetCalibRunEnergyCmd;
  G4UIcmdWithADoubleAndUnit* fSetCalibRunCenterXCmd;
  G4UIcmdWithADoubleAndUnit* fSetCalibRunCenterYCmd;
  G4UIcmdWithADoubleAndUnit* fSetCalibRunRadiusCmd;

};
#endif
