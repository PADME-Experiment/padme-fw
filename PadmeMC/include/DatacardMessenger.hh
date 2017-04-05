#ifndef DatacardMessenger_h
#define DatacardMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class DatacardManager;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithABool;

class DatacardMessenger: public G4UImessenger
{

public:
  DatacardMessenger(DatacardManager*);
  ~DatacardMessenger();
  void SetNewValue(G4UIcommand*, G4String);

private:

  DatacardManager* fDatacardManager;

  G4UIdirectory* fOutputDir;

  G4UIcmdWithAString* fOutNameCmd;
  G4UIcmdWithAString* fHistoNameCmd;

  G4UIcmdWithAString* fEnableDetectorIOCmd;
  G4UIcmdWithAString* fDisableDetectorIOCmd;

  G4UIcmdWithABool* fDisableSACAnalysisCmd;
  G4UIcmdWithABool* fDisableECalAnalysisCmd;

};
#endif // DatacardMessenger_h
