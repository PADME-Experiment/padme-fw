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

  fSetMagnetFrontFaceZCmd = new G4UIcommand("/Detector/Magnet/FrontFaceZ",this);
  fSetMagnetFrontFaceZCmd->SetGuidance("Set position along Z of Magnet (iron yoke) front face in cm.");
  G4UIparameter* tffPosZParameter = new G4UIparameter("PosZ",'d',false);
  tffPosZParameter->SetParameterRange("PosZ >= -100. && PosZ <= 100.");
  fSetMagnetFrontFaceZCmd->SetParameter(tffPosZParameter);
  fSetMagnetFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

MagnetMessenger::~MagnetMessenger()
{

  delete fMagnetDetectorDir;

  delete fSetMagnetFrontFaceZCmd;

}

void MagnetMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetMagnetFrontFaceZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    printf("Setting MagnetFrontFacePosZ to %f cm\n",z);
    fMagnetGeometry->SetMagnetFrontFacePosZ(z*cm);
  }

}
