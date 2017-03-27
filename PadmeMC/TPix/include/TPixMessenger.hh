// TPixMessenger.hh
// --------------------------------------------------------------
// History:
//
// 2016-02-05 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#ifndef TPixMessenger_h
#define TPixMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class TPixDetector;
class TPixGeometry;

class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;

class TPixMessenger: public G4UImessenger
{
public:

  TPixMessenger(TPixDetector* );
  ~TPixMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:

  TPixDetector* fTPixDetector;
  TPixGeometry* fTPixGeometry;

  G4UIdirectory* fTPixDetectorDir;

  G4UIcmdWithAnInteger* fSetNColumnsCmd;
  G4UIcmdWithADoubleAndUnit* fSetDistToCornerCmd;

};
#endif
