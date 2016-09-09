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

#include "ECalGeometry.hh"

ECalMessenger::ECalMessenger(ECalDetector* det)
:fECalDetector(det)
{

  fECalGeometry = ECalGeometry::GetInstance();

  fECalDetectorDir = new G4UIdirectory("/Detector/ECal/");
  fECalDetectorDir->SetGuidance("UI commands to control ECal detector geometry");

  fSetECalNRowsCmd = new G4UIcommand("/Detector/ECal/NRows",this);
  fSetECalNRowsCmd->SetGuidance("Set number of crystal rows in ECal.");
  G4UIparameter* egNRowsParameter = new G4UIparameter("NRows",'i',false);
  egNRowsParameter->SetParameterRange("NRows >= 1 && NRows <= 100");
  fSetECalNRowsCmd->SetParameter(egNRowsParameter);
  fSetECalNRowsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetECalNColsCmd = new G4UIcommand("/Detector/ECal/NCols",this);
  fSetECalNColsCmd->SetGuidance("Set number of crystal columns in ECal.");
  G4UIparameter* egNColsParameter = new G4UIparameter("NCols",'i',false);
  egNColsParameter->SetParameterRange("NCols >= 1 && NCols <= 100");
  fSetECalNColsCmd->SetParameter(egNColsParameter);
  fSetECalNColsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetCrystalSizeCmd = new G4UIcommand("/Detector/ECal/CrystalSize",this);
  fSetCrystalSizeCmd->SetGuidance("Set size (side of square) of crystal front face in cm.");
  G4UIparameter* csSizeParameter = new G4UIparameter("Size",'d',false);
  csSizeParameter->SetParameterRange("Size > 0. && Size <= 10.");
  fSetCrystalSizeCmd->SetParameter(csSizeParameter);
  fSetCrystalSizeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetCrystalLengthCmd = new G4UIcommand("/Detector/ECal/CrystalLength",this);
  fSetCrystalLengthCmd->SetGuidance("Set length of crystal in cm.");
  G4UIparameter* csLengthParameter = new G4UIparameter("Length",'d',false);
  csLengthParameter->SetParameterRange("Length > 0. && Length <= 35.");
  fSetCrystalLengthCmd->SetParameter(csLengthParameter);
  fSetCrystalLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetCrystalGapCmd = new G4UIcommand("/Detector/ECal/CrystalGap",this);
  fSetCrystalGapCmd->SetGuidance("Set size of gap between crystals in mm.");
  G4UIparameter* cgSizeParameter = new G4UIparameter("Gap",'d',false);
  cgSizeParameter->SetParameterRange("Gap > 0. && Gap <= 10.");
  fSetCrystalGapCmd->SetParameter(cgSizeParameter);
  fSetCrystalGapCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  //fSetECalInnerRadiusCmd = new G4UIcommand("/Detector/ECal/InnerRadius",this);
  //fSetECalInnerRadiusCmd->SetGuidance("Set radius of inner hole of ECal detector in cm.");
  //G4UIparameter* eirRadParameter = new G4UIparameter("Rad",'d',false);
  //eirRadParameter->SetParameterRange("Rad >= 0. && Rad < 20.");
  //fSetECalInnerRadiusCmd->SetParameter(eirRadParameter);
  //fSetECalInnerRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  //fSetECalOuterRadiusCmd = new G4UIcommand("/Detector/ECal/OuterRadius",this);
  //fSetECalOuterRadiusCmd->SetGuidance("Set external radius of ECal detector in cm.");
  //G4UIparameter* eorRadParameter = new G4UIparameter("Rad",'d',false);
  //eorRadParameter->SetParameterRange("Rad >= 20. && Rad < 200.");
  //fSetECalOuterRadiusCmd->SetParameter(eorRadParameter);
  //fSetECalOuterRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetECalFrontFaceZCmd = new G4UIcommand("/Detector/ECal/FrontFaceZ",this);
  fSetECalFrontFaceZCmd->SetGuidance("Set position along Z of ECal front face in cm.");
  G4UIparameter* effPosZParameter = new G4UIparameter("PosZ",'d',false);
  effPosZParameter->SetParameterRange("PosZ > 100. && PosZ <= 1000.");
  fSetECalFrontFaceZCmd->SetParameter(effPosZParameter);
  fSetECalFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

ECalMessenger::~ECalMessenger()
{

  delete fECalDetectorDir;

  delete fSetECalNRowsCmd;
  delete fSetECalNColsCmd;

  delete fSetCrystalSizeCmd;
  delete fSetCrystalLengthCmd;

  delete fSetCrystalGapCmd;

  //delete fSetECalInnerRadiusCmd;
  //delete fSetECalOuterRadiusCmd;

  delete fSetECalFrontFaceZCmd;

}

void ECalMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetECalNRowsCmd ) {
    G4int r; std::istringstream is(par); is >> r;
    fECalGeometry->SetECalNRows(r);
  }

  if ( cmd == fSetECalNColsCmd ) {
    G4int c; std::istringstream is(par); is >> c;
    fECalGeometry->SetECalNCols(c);
  }

  if ( cmd == fSetCrystalSizeCmd ) {
    G4double s; std::istringstream is(par); is >> s;
    fECalGeometry->SetCrystalSizeX(s*cm);
    fECalGeometry->SetCrystalSizeY(s*cm);
  }

  if ( cmd == fSetCrystalLengthCmd ) {
    G4double s; std::istringstream is(par); is >> s;
    fECalGeometry->SetCrystalSizeZ(s*cm);
  }

  if ( cmd == fSetCrystalGapCmd ) {
    G4double s; std::istringstream is(par); is >> s;
    fECalGeometry->SetCrystalGap(s*mm);
  }

  //if ( cmd == fSetECalInnerRadiusCmd ) {
  //  G4double r; std::istringstream is(par); is >> r;
  //  fECalGeometry->SetECalInnerRadius(r*cm);
  //}

  //if ( cmd == fSetECalOuterRadiusCmd ) {
  //  G4double r; std::istringstream is(par); is >> r;
  //  fECalGeometry->SetECalOuterRadius(r*cm);
  //}

  if ( cmd == fSetECalFrontFaceZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fECalGeometry->SetECalFrontFacePosZ(z*cm);
  }

}
