// BeamLineMessenger.hh
// --------------------------------------------------------------
// History:
//
// 2019-03-26 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#ifndef BeamLineMessenger_h
#define BeamLineMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class G4UIdirectory;

class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3Vector;
class G4UIcmdWithAString;

class BeamLineGeometry;
class BeamLineStructure;

class BeamLineMessenger: public G4UImessenger
{
public:

  BeamLineMessenger(BeamLineStructure*);
  ~BeamLineMessenger();

  void SetNewValue(G4UIcommand*, G4String);

  G4String GetCurrentValue(G4UIcommand*);

private:

  BeamLineGeometry*  fBeamLineGeometry;
  BeamLineStructure*  fBeamLineStructure;

  G4UIdirectory* fBeamLineDir;

  G4UIcmdWithABool* fEnableBeWindowCmd;
  G4UIcmdWithABool* fEnableMylarWindowCmd;
  G4UIcmdWithABool* fEnableBeamFlagCmd;
  G4UIcmdWithADoubleAndUnit* fSetDHSTB002MagneticFieldYCmd;
  
  G4UIcmdWithABool* fEnableQuadrupolesCmd;
  G4UIcmdWithADouble* fSetQ1_FieldGradCmd;
  G4UIcmdWithADouble* fSetQ2_FieldGradCmd;
  G4UIcmdWithADouble* fSetQ3_FieldGradCmd;
  G4UIcmdWithADouble* fSetQ4_FieldGradCmd;

  G4UIcmdWithADouble* fBeamLineSetupCmd;

};
#endif
