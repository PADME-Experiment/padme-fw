// MagnetMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "MagnetMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "MagnetStructure.hh"
#include "MagnetGeometry.hh"

MagnetMessenger::MagnetMessenger(MagnetStructure* mag)
:fMagnetStructure(mag)
{

  fMagnetGeometry = MagnetGeometry::GetInstance();

  fMagnetDetectorDir = new G4UIdirectory("/Detector/Magnet/");
  fMagnetDetectorDir->SetGuidance("UI commands to control Magnet geometry");

  fEnableMagneticFieldCmd = new G4UIcmdWithoutParameter("/Detector/Magnet/EnableMagneticField",this);
  fEnableMagneticFieldCmd->SetGuidance("Enable magnetic field.");
  fEnableMagneticFieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fDisableMagneticFieldCmd = new G4UIcmdWithoutParameter("/Detector/Magnet/DisableMagneticField",this);
  fDisableMagneticFieldCmd->SetGuidance("Disable magnetic field.");
  fDisableMagneticFieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetMagneticFieldConstantValueCmd = new G4UIcommand("/Detector/Magnet/MagneticFieldConstantValue",this);
  fSetMagneticFieldConstantValueCmd->SetGuidance("Set constant (max) value of magnetic field in tesla.");
  G4UIparameter* mfCVParameter = new G4UIparameter("CV",'d',false);
  mfCVParameter->SetParameterRange("CV >= 0. && CV <= 1.");
  fSetMagneticFieldConstantValueCmd->SetParameter(mfCVParameter);
  fSetMagneticFieldConstantValueCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

MagnetMessenger::~MagnetMessenger()
{

  delete fMagnetDetectorDir;

  delete fEnableMagneticFieldCmd;
  delete fDisableMagneticFieldCmd;

  delete fSetMagneticFieldConstantValueCmd;

}

void MagnetMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fEnableMagneticFieldCmd ) {
    printf("Enabling Magnetic Field\n");
    fMagnetStructure->EnableMagneticField();
  }

  if ( cmd == fDisableMagneticFieldCmd ) {
    printf("Disabling Magnetic Field\n");
    fMagnetStructure->DisableMagneticField();
  }

  if ( cmd == fSetMagneticFieldConstantValueCmd ) {
    G4double v; std::istringstream is(par); is >> v;
    printf("Setting magnetic field constant value to %f tesla\n",v);
    fMagnetGeometry->SetMagneticFieldConstantValue(v*tesla);
  }

}
