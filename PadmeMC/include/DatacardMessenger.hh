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
    DatacardManager*      fDatacardManager;
    G4UIcmdWithAString*   fOutNameCmd;
    G4UIcmdWithAString*   fHistoNameCmd;
};
#endif // DatacardMessenger_h
