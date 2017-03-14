#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;
class G4UIcmdWithADoubleAndUnit;

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

  G4UIcmdWithAString* fEnableSubDetCmd;
  G4UIcmdWithAString* fDisableSubDetCmd;

  G4UIcmdWithAString* fEnableStructCmd;
  G4UIcmdWithAString* fDisableStructCmd;

  G4UIcmdWithoutParameter* fEnableMagFieldCmd;
  G4UIcmdWithoutParameter* fDisableMagFieldCmd;

  G4UIcmdWithoutParameter* fMagVolVisibleCmd;
  G4UIcmdWithoutParameter* fMagVolInvisibleCmd;

  G4UIcmdWithoutParameter* fChamberVisibleCmd;
  G4UIcmdWithoutParameter* fChamberInvisibleCmd;

  G4UIcmdWithADoubleAndUnit* fSetMagFieldValueCmd;

  G4UIcmdWithoutParameter* fWorldIsAirCmd;
  G4UIcmdWithoutParameter* fWorldIsVacuumCmd;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
