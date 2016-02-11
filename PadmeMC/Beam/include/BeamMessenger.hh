// BeamMessenger.hh
// --------------------------------------------------------------
// History:
//
// 2016-02-11 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#ifndef BeamMessenger_h
#define BeamMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class G4UIdirectory;
class G4UIcommand;

class BeamGenerator;
class BeamParameters;

class BeamMessenger: public G4UImessenger
{
public:
  BeamMessenger(BeamGenerator*);
  ~BeamMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:

  BeamGenerator*  fBeamGenerator;
  BeamParameters* fBeamParameters;

  G4UIdirectory* fBeamGeneratorDir;

  //G4UIcommand* fSetTargetRadiusCmd;
  //G4UIcommand* fSetTargetLengthCmd;
  //
  //G4UIcommand* fSetTargetFrontFaceZCmd;
  //
  //G4UIcommand* fSetBeamFrontFaceZCmd;

};
#endif
