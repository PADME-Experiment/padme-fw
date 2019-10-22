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

  fBeamLineDir = new G4UIdirectory("/Detector/BeamLine/");
  fBeamLineDir->SetGuidance("UI commands to control BeamLine setup");

  fEnableBeWindowCmd = new G4UIcmdWithABool("/Detector/BeamLine/EnableBeWindow",this);
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

  fEnableQuadrupolesCmd = new G4UIcmdWithABool("/Detector/BeamLine/EnableQuadrupoles",this);
  fEnableQuadrupolesCmd->SetGuidance("Enable (true) or disable (false) positioning of Quadrupoles Q1 and Q2.");
  fEnableQuadrupolesCmd->SetParameterName("QPS",false);
  fEnableQuadrupolesCmd->SetDefaultValue(false);
  fEnableQuadrupolesCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  //data card for Q1 quadrupole magnet gradient.
  fSetQ1_FieldGradCmd = new G4UIcmdWithADouble("/Detector/BeamLine/Q1_FieldGrad",this);
  fSetQ1_FieldGradCmd->SetGuidance("Set gradient of the Q1 quadrupole in T/m");
  fSetQ1_FieldGradCmd->SetParameterName("Q1GF",false);
  fSetQ1_FieldGradCmd->SetRange("Q1GF > 0. && Q1GF < 15.");
  fSetQ1_FieldGradCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  //data card for Q2 quadrupole magnet gradient.
  fSetQ2_FieldGradCmd = new G4UIcmdWithADouble("/Detector/BeamLine/Q2_FieldGrad",this);
  fSetQ2_FieldGradCmd->SetGuidance("Set gradient of the Q1 quadrupole in T/m");
  fSetQ2_FieldGradCmd->SetParameterName("Q2GF",false);
  fSetQ2_FieldGradCmd->SetRange("Q2GF > 0. && Q2GF < 15.");
  fSetQ2_FieldGradCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

BeamLineMessenger::~BeamLineMessenger()
{

  delete fEnableQuadrupolesCmd;
  delete fSetQ1_FieldGradCmd; 
  delete fSetQ2_FieldGradCmd; 
  delete fEnableBeWindowCmd;
  delete fSetDHSTB002MagneticFieldYCmd;
  delete fBeamLineDir;

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

  else if ( cmd == fEnableQuadrupolesCmd ) {
    if (fEnableQuadrupolesCmd->GetNewBoolValue(par)) {
      fBeamLineGeometry->EnableQuadrupoles();
    } else {
      fBeamLineGeometry->DisableQuadrupoles();
    }
  }

  // Set Q1 gradient
  else if ( cmd == fSetQ1_FieldGradCmd )
    fBeamLineGeometry->SetQ1MagneticFieldGrad(fSetQ1_FieldGradCmd->GetNewDoubleValue(par));

  // Set Q1 gradient
  else if ( cmd == fSetQ2_FieldGradCmd )
    fBeamLineGeometry->SetQ2MagneticFieldGrad(fSetQ2_FieldGradCmd->GetNewDoubleValue(par));

}

G4String BeamLineMessenger::GetCurrentValue(G4UIcommand* cmd)
{

  G4String cv;

  if ( cmd == fEnableBeWindowCmd )
    cv = fEnableBeWindowCmd->ConvertToString(fBeamLineGeometry->BeWindowIsEnabled());

  else if ( cmd == fSetDHSTB002MagneticFieldYCmd )
    cv = fSetDHSTB002MagneticFieldYCmd->ConvertToString(fBeamLineGeometry->GetDHSTB002MagneticFieldY(),"tesla");

  else if ( cmd == fEnableQuadrupolesCmd )
    cv = fEnableQuadrupolesCmd->ConvertToString(fBeamLineGeometry->QuadrupolesAreEnabled());

  else if ( cmd == fSetQ1_FieldGradCmd )
   cv = fSetQ1_FieldGradCmd->ConvertToString(fBeamLineGeometry->GetQ1MagneticFieldGrad(),"tesla/m");

  else if ( cmd == fSetQ2_FieldGradCmd )
   cv = fSetQ2_FieldGradCmd->ConvertToString(fBeamLineGeometry->GetQ2MagneticFieldGrad(),"tesla/m");

  return cv;

}
