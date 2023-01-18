#ifndef DatacardMessenger_h
#define DatacardMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class DatacardManager;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithABool;
class G4UIcmdWithADoubleAndUnit;

class DatacardMessenger: public G4UImessenger
{

public:
  DatacardMessenger(DatacardManager*);
  ~DatacardMessenger();
  void SetNewValue(G4UIcommand*, G4String);

private:

  DatacardManager* fDatacardManager;

  G4UIdirectory* fOutputDir;
  G4UIdirectory* fAnalysisDir;
  G4UIdirectory* fSettingsDir;

  G4UIcmdWithAString* fOutNameCmd;
  G4UIcmdWithAString* fHistoNameCmd;

  G4UIcmdWithAString* fEnableDetectorIOCmd;
  G4UIcmdWithAString* fDisableDetectorIOCmd;

  G4UIcmdWithABool* fDisableSACAnalysisCmd;
  G4UIcmdWithABool* fDisableECalAnalysisCmd;

  //M. Raggi 06/23/2018
  G4UIcmdWithABool* fEnableSaveSACCmd; 
  G4UIcmdWithABool* fEnableSaveVetoCmd;
  G4UIcmdWithABool* fEnableSaveEcalCmd;

  G4UIcmdWithAnInteger* fPrintOutFrequencyCmd;
  G4UIcmdWithABool* fAutomaticRandomSeedCmd;

  G4UIdirectory* fTurboModeDir;
  G4UIcmdWithABool* fTurboModeCmd;
  G4UIcmdWithAnInteger* fTurboModeVerboseCmd;
  G4UIcmdWithADoubleAndUnit* fTurboPositronKillEnergyCmd;
  G4UIcmdWithADoubleAndUnit* fTurboElectronKillEnergyCmd;
  G4UIcmdWithADoubleAndUnit* fTurboGammaKillEnergyCmd;
  G4UIcmdWithADoubleAndUnit* fTurboNeutronKillEnergyCmd;

  G4UIcmdWithABool* fEnableMCTruthCmd;

};
#endif // DatacardMessenger_h
