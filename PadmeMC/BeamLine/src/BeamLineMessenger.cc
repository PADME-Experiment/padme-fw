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

  //New Mylar window command line 
  fEnableMylarWindowCmd = new G4UIcmdWithABool("/Detector/BeamLine/EnableMylarWindow",this);
  fEnableMylarWindowCmd->SetGuidance("Enable (true) or disable (false) positioning of Mylar window.");
  fEnableMylarWindowCmd->SetParameterName("MW",true);
  fEnableMylarWindowCmd->SetDefaultValue(true);
  fEnableMylarWindowCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  //data card for beam setup M. Raggi 06/2020
  fBeamLineSetupCmd = new G4UIcmdWithADouble("/Detector/BeamLine/Setup",this);
  fBeamLineSetupCmd->SetGuidance("Set Beam line setup 0=2019 Be  1=2020 Mylar");
  fBeamLineSetupCmd->SetParameterName("BLS",false);
  fBeamLineSetupCmd->SetRange("BLS  >=0. && BLS < 15.");
  fBeamLineSetupCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fEnableBeamFlagCmd = new G4UIcmdWithABool("/Detector/BeamLine/BeamFlag",this);
  fEnableBeamFlagCmd->SetGuidance("Enable (true) or disable (false) positioning of beam monitors flags.");
  fEnableBeamFlagCmd->SetParameterName("BF",true);
  fEnableBeamFlagCmd->SetDefaultValue(true);
  fEnableBeamFlagCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetDHSTB002MagneticFieldYCmd = new G4UIcmdWithADoubleAndUnit("/Detector/BeamLine/DHST002_FieldY",this);
  fSetDHSTB002MagneticFieldYCmd->SetGuidance("Set vertical component of magnetic field inside DHST002.");
  fSetDHSTB002MagneticFieldYCmd->SetParameterName("DFY",false);
  fSetDHSTB002MagneticFieldYCmd->SetDefaultUnit("tesla");
  fSetDHSTB002MagneticFieldYCmd->SetRange("DFY > -10. && DFY < 10.");
  fSetDHSTB002MagneticFieldYCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  //Windows Thicknerss
  fSetWindowThicknessCmd = new G4UIcmdWithADoubleAndUnit("/Detector/BeamLine/WindowThickness",this);
  fSetWindowThicknessCmd->SetGuidance("Thickness of Mylar or BeW");
  fSetWindowThicknessCmd->SetParameterName("DFY",false);
  fSetWindowThicknessCmd->SetDefaultUnit("mm");
  fSetWindowThicknessCmd->SetRange("DFY >=0. && DFY < 1.");
  fSetWindowThicknessCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  //SLTB3 aperture
  fSetSLTB3Cmd = new G4UIcmdWithADoubleAndUnit("/Detector/BeamLine/SLTB3Aperture",this);
  fSetSLTB3Cmd->SetGuidance("");
  fSetSLTB3Cmd->SetParameterName("DFY",false);
  fSetSLTB3Cmd->SetDefaultUnit("mm");
  fSetSLTB3Cmd->SetRange("DFY >=0. && DFY < 10.");
  fSetSLTB3Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  //SLTB4 aperture
  fSetSLTB4Cmd = new G4UIcmdWithADoubleAndUnit("/Detector/BeamLine/SLTB4Aperture",this);
  fSetSLTB4Cmd->SetGuidance("");
  fSetSLTB4Cmd->SetParameterName("DFY",false);
  fSetSLTB4Cmd->SetDefaultUnit("mm");
  fSetSLTB4Cmd->SetRange("DFY >=0. && DFY < 10.");
  fSetSLTB4Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fEnableQuadrupolesCmd = new G4UIcmdWithABool("/Detector/BeamLine/EnableQuadrupoles",this);
  fEnableQuadrupolesCmd->SetGuidance("Enable (true) or disable (false) positioning of Quadrupoles Q1 and Q2.");
  fEnableQuadrupolesCmd->SetParameterName("QPS",false);
  fEnableQuadrupolesCmd->SetDefaultValue(false);
  fEnableQuadrupolesCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  //data card for Q1 quadrupole magnet gradient.
  fSetQ1_FieldGradCmd = new G4UIcmdWithADouble("/Detector/BeamLine/Q1_FieldGrad",this);
  fSetQ1_FieldGradCmd->SetGuidance("Set gradient of the Q1 quadrupole in T/m");
  fSetQ1_FieldGradCmd->SetParameterName("Q1GF",false);
  fSetQ1_FieldGradCmd->SetRange("Q1GF > -15. && Q1GF < 15.");
  fSetQ1_FieldGradCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  //data card for Q2 quadrupole magnet gradient.
  fSetQ2_FieldGradCmd = new G4UIcmdWithADouble("/Detector/BeamLine/Q2_FieldGrad",this);
  fSetQ2_FieldGradCmd->SetGuidance("Set gradient of the Q2 quadrupole in T/m");
  fSetQ2_FieldGradCmd->SetParameterName("Q2GF",false);
  fSetQ2_FieldGradCmd->SetRange("Q2GF > -15. && Q2GF < 15.");
  fSetQ2_FieldGradCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  //data card for Q3 quadrupole magnet gradient.
  fSetQ3_FieldGradCmd = new G4UIcmdWithADouble("/Detector/BeamLine/Q3_FieldGrad",this);
  fSetQ3_FieldGradCmd->SetGuidance("Set gradient of the Q3 quadrupole in T/m");
  fSetQ3_FieldGradCmd->SetParameterName("Q3GF",false);
  fSetQ3_FieldGradCmd->SetRange("Q3GF > -15. && Q3GF < 15.");
  fSetQ3_FieldGradCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  //data card for Q4 quadrupole magnet gradient.
  fSetQ4_FieldGradCmd = new G4UIcmdWithADouble("/Detector/BeamLine/Q4_FieldGrad",this);
  fSetQ4_FieldGradCmd->SetGuidance("Set gradient of the Q4 quadrupole in T/m");
  fSetQ4_FieldGradCmd->SetParameterName("Q4GF",false);
  fSetQ4_FieldGradCmd->SetRange("Q4GF > -15. && Q4GF < 15.");
  fSetQ4_FieldGradCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

BeamLineMessenger::~BeamLineMessenger()
{

  delete fEnableQuadrupolesCmd;
  delete fSetQ1_FieldGradCmd; 
  delete fSetQ2_FieldGradCmd; 
  delete fSetQ3_FieldGradCmd; 
  delete fSetQ4_FieldGradCmd; 

  delete fEnableBeWindowCmd;
  delete fEnableMylarWindowCmd;
  delete fEnableBeamFlagCmd;
  delete fSetDHSTB002MagneticFieldYCmd;
  delete fSetWindowThicknessCmd;
  delete fBeamLineDir;
  delete fBeamLineSetupCmd;

}

void BeamLineMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{
  
  if ( cmd == fEnableMylarWindowCmd ) {
    if (fEnableMylarWindowCmd->GetNewBoolValue(par)) {
      fBeamLineGeometry->EnableMylarWindow();
    } else {
      fBeamLineGeometry->DisableMylarWindow();
    }
  }

  if ( cmd == fEnableBeWindowCmd ) {
    if (fEnableBeWindowCmd->GetNewBoolValue(par)) {
      fBeamLineGeometry->EnableBeWindow();
    } else {
      fBeamLineGeometry->DisableBeWindow();
    }
  }

  else if ( cmd == fEnableBeamFlagCmd ) {
    if (fEnableBeamFlagCmd->GetNewBoolValue(par)) {
      fBeamLineGeometry->EnableBeamFlag();
    } else {
      fBeamLineGeometry->DisableBeamFlag();
    }
  }
  
  else if ( cmd == fSetDHSTB002MagneticFieldYCmd )
    fBeamLineGeometry->SetDHSTB002MagneticFieldY(fSetDHSTB002MagneticFieldYCmd->GetNewDoubleValue(par));
  //window Thickness MR 29/03/2021
  else if ( cmd == fSetWindowThicknessCmd )
    fBeamLineGeometry->SetWindowThickness(fSetWindowThicknessCmd->GetNewDoubleValue(par));

  else if ( cmd == fBeamLineSetupCmd )
    fBeamLineGeometry->SetBeamLineSetup(fBeamLineSetupCmd->GetNewDoubleValue(par));

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

  // Set Q2 gradient
  else if ( cmd == fSetQ2_FieldGradCmd )
    fBeamLineGeometry->SetQ2MagneticFieldGrad(fSetQ2_FieldGradCmd->GetNewDoubleValue(par));

  // Set Q3 gradient
  else if ( cmd == fSetQ3_FieldGradCmd )
    fBeamLineGeometry->SetQ3MagneticFieldGrad(fSetQ3_FieldGradCmd->GetNewDoubleValue(par));

  // Set Q4 gradient
  else if ( cmd == fSetQ4_FieldGradCmd )
    fBeamLineGeometry->SetQ4MagneticFieldGrad(fSetQ4_FieldGradCmd->GetNewDoubleValue(par));

  // Set SLTB3 gradient
  else if ( cmd == fSetSLTB3Cmd )
    fBeamLineGeometry->SetSLTB3Aperture(fSetSLTB3Cmd->GetNewDoubleValue(par));

  // Set SLTB4 gradient
  else if ( cmd == fSetSLTB4Cmd )
    fBeamLineGeometry->SetSLTB4Aperture(fSetSLTB4Cmd->GetNewDoubleValue(par));

}

G4String BeamLineMessenger::GetCurrentValue(G4UIcommand* cmd)
{

  G4String cv;

  if ( cmd == fEnableBeWindowCmd )
    cv = fEnableBeWindowCmd->ConvertToString(fBeamLineGeometry->BeWindowIsEnabled());

  else if ( cmd == fEnableMylarWindowCmd )
    cv = fEnableMylarWindowCmd->ConvertToString(fBeamLineGeometry->MylarWindowIsEnabled());

  else if ( cmd == fEnableBeamFlagCmd )
    cv = fEnableBeamFlagCmd->ConvertToString(fBeamLineGeometry->BeamFlagIsEnabled());

  else if ( cmd == fSetDHSTB002MagneticFieldYCmd )
    cv = fSetDHSTB002MagneticFieldYCmd->ConvertToString(fBeamLineGeometry->GetDHSTB002MagneticFieldY(),"tesla");

  else if ( cmd == fBeamLineSetupCmd )
    cv = fBeamLineSetupCmd->ConvertToString(fBeamLineGeometry->GetBeamLineSetup());

  else if ( cmd == fEnableQuadrupolesCmd )
    cv = fEnableQuadrupolesCmd->ConvertToString(fBeamLineGeometry->QuadrupolesAreEnabled());

  else if ( cmd == fSetQ1_FieldGradCmd )
   cv = fSetQ1_FieldGradCmd->ConvertToString(fBeamLineGeometry->GetQ1MagneticFieldGrad(),"tesla/m");

  else if ( cmd == fSetQ2_FieldGradCmd )
   cv = fSetQ2_FieldGradCmd->ConvertToString(fBeamLineGeometry->GetQ2MagneticFieldGrad(),"tesla/m");

  else if ( cmd == fSetQ3_FieldGradCmd )
   cv = fSetQ3_FieldGradCmd->ConvertToString(fBeamLineGeometry->GetQ3MagneticFieldGrad(),"tesla/m");

  else if ( cmd == fSetQ2_FieldGradCmd )
   cv = fSetQ4_FieldGradCmd->ConvertToString(fBeamLineGeometry->GetQ4MagneticFieldGrad(),"tesla/m");

  return cv;

}
