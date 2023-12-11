// ETagMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "ETagMessenger.hh"

#include "G4UIdirectory.hh"
//#include "G4UIcommand.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
//#include "G4UIparameter.hh"

#include "ETagDetector.hh"
#include "ETagGeometry.hh"

ETagMessenger::ETagMessenger(ETagDetector* det)
:fETagDetector(det)
{

  fETagGeometry = ETagGeometry::GetInstance();

  fETagDetectorDir = new G4UIdirectory("/Detector/ETag/");
  fETagDetectorDir->SetGuidance("UI commands to control ETag detector geometry");

  //fSetETagFrontFaceZCmd = new G4UIcommand("/Detector/ETag/FrontFaceZ",this);
  //fSetETagFrontFaceZCmd->SetGuidance("Set position along Z of ETag front face in cm.");
  //G4UIparameter* effPosZParameter = new G4UIparameter("PosZ",'d',false);
  //effPosZParameter->SetParameterRange("PosZ >= -100. && PosZ <= -50.");
  //fSetETagFrontFaceZCmd->SetParameter(effPosZParameter);
  //fSetETagFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetETagFrontFaceZCmd = new G4UIcmdWithADoubleAndUnit("/Detector/ETag/FrontFaceZ",this);
  fSetETagFrontFaceZCmd->SetGuidance("Set position along Z of ETag front face (Tedlar foil).");
  fSetETagFrontFaceZCmd->SetParameterName("ETFFZ",false);
  fSetETagFrontFaceZCmd->SetDefaultUnit("cm");
  fSetETagFrontFaceZCmd->SetRange("ETFFZ >= 200. && ETFFZ <= 300.");
  fSetETagFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetVerboseLevelCmd = new G4UIcmdWithAnInteger("/Detector/ETag/VerboseLevel",this);
  fSetVerboseLevelCmd->SetGuidance("Set verbose level for ETag code.");
  fSetVerboseLevelCmd->SetParameterName("VL",false);
  fSetVerboseLevelCmd->SetRange("VL >= 0");
  fSetVerboseLevelCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

ETagMessenger::~ETagMessenger()
{


  delete fSetETagFrontFaceZCmd;
  delete fSetVerboseLevelCmd;
  delete fETagDetectorDir;

}

void ETagMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetETagFrontFaceZCmd ) {

    fETagGeometry->SetETagFrontFacePosZ(fSetETagFrontFaceZCmd->GetNewDoubleValue(par));
    //G4double z; std::istringstream is(par); is >> z;
    //fETagGeometry->SetETagFrontFacePosZ(z*cm);
  }

  if ( cmd == fSetVerboseLevelCmd ) fETagGeometry->SetVerboseLevel(fSetVerboseLevelCmd->GetNewIntValue(par));

}
