// TungstenMessenger.hh
// --------------------------------------------------------------
// History:
//
// 2017-10-03 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#ifndef TungstenMessenger_h
#define TungstenMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class TungstenDetector;
class TungstenGeometry;

class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;

class TungstenMessenger: public G4UImessenger
{
public:

  TungstenMessenger(TungstenDetector* );
  ~TungstenMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:

  TungstenDetector* fTungstenDetector;
  TungstenGeometry* fTungstenGeometry;

  G4UIdirectory* fTungstenDetectorDir;

  G4UIcmdWithADoubleAndUnit* fSetTungstenFrontFaceZCmd;
  G4UIcmdWithADoubleAndUnit* fSetTungstenLengthCmd;

};
#endif
