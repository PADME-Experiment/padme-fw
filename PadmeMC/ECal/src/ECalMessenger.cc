// ECalMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "ECalMessenger.hh"

#include "ECalDetector.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "ECalGeometryParameters.hh"

ECalMessenger::ECalMessenger(ECalDetector* det)
:fECalDetector(det)
{

  fECalGeometry = ECalGeometryParameters::GetInstance();

  fECalDetectorDir = new G4UIdirectory("/Detector/ECal/");
  fECalDetectorDir->SetGuidance("UI commands to control ECal detector geometry");

  fSetECalGeometryCmd = new G4UIcommand("/Detector/ECal/Geometry",this);
  fSetECalGeometryCmd->SetGuidance("Set number of ECal rows and columns.");
  G4UIparameter* egNRowsParameter = new G4UIparameter("NRows",'i',false);
  egNRowsParameter->SetParameterRange("NRows >= 1 && NRows <= 100");
  fSetECalGeometryCmd->SetParameter(egNRowsParameter);
  G4UIparameter* egNColsParameter = new G4UIparameter("NCols",'i',false);
  egNColsParameter->SetParameterRange("NCols >= 1 && NCols <= 100");
  fSetECalGeometryCmd->SetParameter(egNColsParameter);
  fSetECalGeometryCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetCrystalSizeCmd = new G4UIcommand("/Detector/ECal/CrystalSize",this);
  fSetCrystalSizeCmd->SetGuidance("Set size of crystal front face in cm.");
  G4UIparameter* csSizeParameter = new G4UIparameter("Size",'f',false);
  csSizeParameter->SetParameterRange("Size > 0. && Size <= 10.");
  fSetCrystalSizeCmd->SetParameter(csSizeParameter);
  fSetCrystalSizeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetECalFrontFaceZCmd = new G4UIcommand("/Detector/ECal/FrontFaceZ",this);
  fSetECalFrontFaceZCmd->SetGuidance("Set position along Z of ECal front face in cm.");
  G4UIparameter* effPosZParameter = new G4UIparameter("PosZ",'f',false);
  effPosZParameter->SetParameterRange("PosZ >= 100. && PosZ <= 1000.");
  fSetECalFrontFaceZCmd->SetParameter(effPosZParameter);
  fSetECalFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

ECalMessenger::~ECalMessenger()
{

  delete fECalDetectorDir;

  delete fSetECalGeometryCmd;
  delete fSetCrystalSizeCmd;
  delete fSetECalFrontFaceZCmd;

}

void ECalMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetECalGeometryCmd ) {
    G4int r,c; std::istringstream is(par); is >> r >> c;
    fECalGeometry->SetECalNRows(r);
    fECalGeometry->SetECalNCols(c);
  }

  if ( cmd == fSetCrystalSizeCmd ) {
    G4double s; std::istringstream is(par); is >> s;
    fECalGeometry->SetCrystalNominalSizeX(s*cm);
    fECalGeometry->SetCrystalNominalSizeY(s*cm);
  }

  if ( cmd == fSetECalFrontFaceZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fECalGeometry->SetECalFrontFacePosZ(z*cm);
  }

}
