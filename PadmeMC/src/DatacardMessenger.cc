#include "DatacardMessenger.hh"
#include "DatacardManager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
//#include "G4UIcmdWithADouble.hh"
//#include "G4UIcmdWithAnInteger.hh"
//#include "G4UIcmdWith3Vector.hh"

DatacardMessenger::DatacardMessenger(DatacardManager* datacardMng):fDatacardManager(datacardMng)
{
  fOutNameCmd = new G4UIcmdWithAString("/output/fileName",this);
  fOutNameCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

DatacardMessenger::~DatacardMessenger() {
    delete fOutNameCmd;
}

void DatacardMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {

    G4cout << command->GetCommandName() << " " << newValue << G4endl;

    if (command == fOutNameCmd) fDatacardManager->SetOutputFileName(newValue);

}
