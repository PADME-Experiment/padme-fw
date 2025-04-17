// LeadGlassMessenger.hh
// --------------------------------------------------------------
// History:
//
// 2023-09-22 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#ifndef LeadGlassMessenger_h
#define LeadGlassMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class LeadGlassDetector;
class LeadGlassGeometry;
class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithABool;

class LeadGlassMessenger: public G4UImessenger
{
public:
  LeadGlassMessenger(LeadGlassDetector* );
  ~LeadGlassMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:

  LeadGlassDetector* fLeadGlassDetector;
  LeadGlassGeometry* fLeadGlassGeometry;

  G4UIdirectory* fLeadGlassDetectorDir;

  G4UIcmdWithADoubleAndUnit* fSetLeadGlassFrontFaceZCmd;
  G4UIcmdWithABool* fFullSimulationCmd;

  G4UIcmdWithAnInteger* fSetVerboseLevelCmd;

};
#endif
