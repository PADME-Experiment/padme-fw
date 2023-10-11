// LeadGlassMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2023-09-22 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "LeadGlassMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIparameter.hh"

#include "LeadGlassDetector.hh"
#include "LeadGlassGeometry.hh"

LeadGlassMessenger::LeadGlassMessenger(LeadGlassDetector* det)
:fLeadGlassDetector(det)
{

  fLeadGlassGeometry = LeadGlassGeometry::GetInstance();

  fLeadGlassDetectorDir = new G4UIdirectory("/Detector/LeadGlass/");
  fLeadGlassDetectorDir->SetGuidance("UI commands to control LeadGlass detector geometry");

  fSetLeadGlassFrontFaceZCmd = new G4UIcmdWithADoubleAndUnit("/Detector/LeadGlass/FrontFaceZ",this);
  fSetLeadGlassFrontFaceZCmd->SetGuidance("Set position along Z of LeadGlass front face.");
  fSetLeadGlassFrontFaceZCmd->SetParameterName("LGFFZ",false);
  fSetLeadGlassFrontFaceZCmd->SetDefaultUnit("cm");
  fSetLeadGlassFrontFaceZCmd->SetRange("LGFFZ >= 300. && LGFFZ <= 400.");
  fSetLeadGlassFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fFullSimulationCmd = new G4UIcmdWithABool("/Detector/LeadGlass/FullSimulation",this);
  fFullSimulationCmd->SetGuidance("Enable (true) or disable (false) full simulation of LeadGlass showers.");
  fFullSimulationCmd->SetParameterName("FS",true);
  fFullSimulationCmd->SetDefaultValue(true);
  fFullSimulationCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

LeadGlassMessenger::~LeadGlassMessenger()
{

  delete fSetLeadGlassFrontFaceZCmd;
  delete fFullSimulationCmd;
  delete fLeadGlassDetectorDir;

}

void LeadGlassMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetLeadGlassFrontFaceZCmd )

    fLeadGlassGeometry->SetLeadGlassFrontFacePosZ(fSetLeadGlassFrontFaceZCmd->GetNewDoubleValue(par));

  else if ( cmd == fFullSimulationCmd ) {

    if ( fFullSimulationCmd->GetNewBoolValue(par)) {
      fLeadGlassGeometry->EnableFullSimulation();
    } else {
      fLeadGlassGeometry->DisableFullSimulation();
    }

  }

}
