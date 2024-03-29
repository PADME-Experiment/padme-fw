#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class DetectorConstruction;
class G4UIcmdWithABool;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAnInteger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorMessenger: public G4UImessenger
{
public:

  DetectorMessenger(DetectorConstruction*);
  ~DetectorMessenger();
    
  void SetNewValue(G4UIcommand*,G4String);
    
private:

  DetectorConstruction* fDetector;
    
  G4UIdirectory* fDetectorDir;

  G4UIcmdWithAnInteger* fDetectorSetupCmd;

  G4UIcmdWithAString* fEnableSubDetCmd;
  G4UIcmdWithAString* fDisableSubDetCmd;

  G4UIcmdWithAString* fEnableStructCmd;
  G4UIcmdWithAString* fDisableStructCmd;

  G4UIcmdWithoutParameter* fEnableMagFieldCmd;
  G4UIcmdWithoutParameter* fDisableMagFieldCmd;

  G4UIcmdWithAString* fCrossMagVolCmd;

  G4UIcmdWithoutParameter* fMagVolVisibleCmd;
  G4UIcmdWithoutParameter* fMagVolInvisibleCmd;

  G4UIcmdWithoutParameter* fMagnetVisibleCmd;
  G4UIcmdWithoutParameter* fMagnetInvisibleCmd;

  G4UIcmdWithoutParameter* fChamberVisibleCmd;
  G4UIcmdWithoutParameter* fChamberInvisibleCmd;

  G4UIcmdWithoutParameter* fBeamLineVisibleCmd;     //M. Raggi 07/03/2019
  G4UIcmdWithoutParameter* fBeamLineInvisibleCmd;   //M. Raggi 07/03/2019

  G4UIcmdWithADoubleAndUnit* fSetMagFieldValueCmd;

  G4UIcmdWithoutParameter* fWorldIsAirCmd;
  G4UIcmdWithoutParameter* fWorldIsVacuumCmd;

  G4UIcmdWithAnInteger* fSetVerboseLevelCmd;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
