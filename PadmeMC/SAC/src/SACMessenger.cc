// SACMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "SACMessenger.hh"

#include "SACDetector.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "SACGeometry.hh"

SACMessenger::SACMessenger(SACDetector* det)
:fSACDetector(det)
{

  fSACGeometry = SACGeometry::GetInstance();

  fSACDetectorDir = new G4UIdirectory("/Detector/SAC/");
  fSACDetectorDir->SetGuidance("UI commands to control SAC detector geometry");

  /*
  fSetSACGeometryCmd = new G4UIcommand("/Detector/SAC/Geometry",this);
  fSetSACGeometryCmd->SetGuidance("Set number of SAC rows and columns.");
  G4UIparameter* egNRowsParameter = new G4UIparameter("NRows",'i',false);
  egNRowsParameter->SetParameterRange("NRows >= 1 && NRows <= 10");
  fSetSACGeometryCmd->SetParameter(egNRowsParameter);
  G4UIparameter* egNColsParameter = new G4UIparameter("NCols",'i',false);
  egNColsParameter->SetParameterRange("NCols >= 1 && NCols <= 10");
  fSetSACGeometryCmd->SetParameter(egNColsParameter);
  fSetSACGeometryCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  */

  fSetCrystalSizeCmd = new G4UIcommand("/Detector/SAC/CrystalSize",this);
  fSetCrystalSizeCmd->SetGuidance("Set size (side of square) of crystal front face in cm.");
  G4UIparameter* csSizeParameter = new G4UIparameter("Size",'d',false);
  csSizeParameter->SetParameterRange("Size > 0. && Size <= 10.");
  fSetCrystalSizeCmd->SetParameter(csSizeParameter);
  fSetCrystalSizeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetSACFrontFaceZCmd = new G4UIcommand("/Detector/SAC/FrontFaceZ",this);
  fSetSACFrontFaceZCmd->SetGuidance("Set position along Z of SAC front face in cm.");
  G4UIparameter* effPosZParameter = new G4UIparameter("PosZ",'d',false);
  effPosZParameter->SetParameterRange("PosZ >= 100. && PosZ <= 1000.");
  fSetSACFrontFaceZCmd->SetParameter(effPosZParameter);
  fSetSACFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

SACMessenger::~SACMessenger()
{

  delete fSACDetectorDir;

  //delete fSetSACGeometryCmd;
  delete fSetCrystalSizeCmd;
  delete fSetSACFrontFaceZCmd;

}

void SACMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  /*
  if ( cmd == fSetSACGeometryCmd ) {
    G4int r,c; std::istringstream is(par); is >> r >> c;
    fSACGeometry->SetSACNRows(r);
    fSACGeometry->SetSACNCols(c);
  }
  */

  if ( cmd == fSetCrystalSizeCmd ) {
    G4double s; std::istringstream is(par); is >> s;
    fSACGeometry->SetCrystalNominalSizeX(s*cm);
    fSACGeometry->SetCrystalNominalSizeY(s*cm);
  }

  if ( cmd == fSetSACFrontFaceZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fSACGeometry->SetSACFrontFacePosZ(z*cm);
  }

}
