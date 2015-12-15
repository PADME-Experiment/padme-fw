#ifndef DatacardMessenger_h
#define DatacardMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class DatacardManager;
class G4UIdirectory;
class G4UIcmdWithAString;
//class G4UIcmdWithADouble;
//class G4UIcmdWithAnInteger;
//class G4UIcmdWith3Vector;

class DatacardMessenger: public G4UImessenger
{
  public:
    DatacardMessenger(DatacardManager*);
   ~DatacardMessenger();
    void SetNewValue(G4UIcommand*, G4String);

  private:
    DatacardManager*      fDatacardManager;
    G4UIcmdWithAString*   fOutNameCmd;
};
#endif // DatacardMessenger_h
