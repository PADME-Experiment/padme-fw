// BeamLineMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2019-03-26 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "BeamLineMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithAString.hh"

#include "BeamLineGeometry.hh"

BeamLineMessenger::BeamLineMessenger(BeamLineStructure* blstruc)
 :fBeamLineStructure(blstruc)
{

  fBeamLineGeometry = BeamLineGeometry::GetInstance();

  fBeamLineDir = new G4UIdirectory("/Detector/BeamLine");
  fBeamLineDir->SetGuidance("UI commands to control BeamLine setup");

  fEnableBeWindowCmd = new G4UIcmdWithABool("/Detector/BeamLine/Be_window",this);
  fEnableBeWindowCmd->SetGuidance("Enable (true) or disable (false) positioning of Berillium window.");
  fEnableBeWindowCmd->SetParameterName("BW",true);
  fEnableBeWindowCmd->SetDefaultValue(true);
  fEnableBeWindowCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetDHSTB002MagneticFieldYCmd = new G4UIcmdWithADoubleAndUnit("/Detector/BeamLine/DHST002_FieldY",this);
  fSetDHSTB002MagneticFieldYCmd->SetGuidance("Set vertical component of magnetic field inside DHST002.");
  fSetDHSTB002MagneticFieldYCmd->SetParameterName("DFY",false);
  fSetDHSTB002MagneticFieldYCmd->SetDefaultUnit("tesla");
  fSetDHSTB002MagneticFieldYCmd->SetRange("DFY > -10. && DFY < 10.");
  fSetDHSTB002MagneticFieldYCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  // Q1 magnetic field gradient
  fSetDHSTB002MagneticFieldYCmd = new G4UIcmdWithADoubleAndUnit("/Detector/BeamLine/Q1_FieldGrad",this);
  fSetDHSTB002MagneticFieldYCmd->SetGuidance("Set gradient of the Q1 quadrupole");
  fSetDHSTB002MagneticFieldYCmd->SetParameterName("Q1GF",false);
  fSetDHSTB002MagneticFieldYCmd->SetDefaultUnit("tesla/m");
  fSetDHSTB002MagneticFieldYCmd->SetRange("Q1GF > 0. && DFY < 10.");
  fSetDHSTB002MagneticFieldYCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

BeamLineMessenger::~BeamLineMessenger()
{

  delete fBeamLineDir;

  delete fEnableBeWindowCmd;
  delete fSetDHSTB002MagneticFieldYCmd;

}

void BeamLineMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fEnableBeWindowCmd ) {
    if (fEnableBeWindowCmd->GetNewBoolValue(par)) {
      fBeamLineGeometry->EnableBeWindow();
    } else {
      fBeamLineGeometry->DisableBeWindow();
    }
  }

  else if ( cmd == fSetDHSTB002MagneticFieldYCmd )
    fBeamLineGeometry->SetDHSTB002MagneticFieldY(fSetDHSTB002MagneticFieldYCmd->GetNewDoubleValue(par));

}

G4String BeamLineMessenger::GetCurrentValue(G4UIcommand* cmd)
{

  G4String cv;

  if ( cmd == fEnableBeWindowCmd )
    cv = fEnableBeWindowCmd->ConvertToString(fBeamLineGeometry->BeWindowIsEnabled());

  else if ( cmd == fSetDHSTB002MagneticFieldYCmd )
    cv = fSetDHSTB002MagneticFieldYCmd->ConvertToString(fBeamLineGeometry->GetDHSTB002MagneticFieldY(),"tesla");

  return cv;

}
