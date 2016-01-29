// TDumpMessenger.hh
// --------------------------------------------------------------
// History:
//
// 2016-01-29 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#ifndef TDumpMessenger_h
#define TDumpMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class TDumpDetector;
class TDumpGeometry;
class G4UIdirectory;
class G4UIcommand;

class TDumpMessenger: public G4UImessenger
{
public:
  TDumpMessenger(TDumpDetector* );
  ~TDumpMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:

  TDumpDetector* fTDumpDetector;
  TDumpGeometry* fTDumpGeometry;

  G4UIdirectory* fTDumpDetectorDir;

  G4UIcommand* fSetTargetRadiusCmd;
  G4UIcommand* fSetTargetLengthCmd;

  G4UIcommand* fSetTargetFrontFaceZCmd;

  G4UIcommand* fSetTDumpFrontFaceZCmd;

};
#endif
