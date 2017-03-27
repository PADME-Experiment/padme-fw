#ifndef DatacardMessenger_h
#define DatacardMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class DatacardManager;
class G4UIdirectory;
class G4UIcmdWithAString;

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

};
#endif // DatacardMessenger_h
