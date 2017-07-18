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
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"

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

  fSetCrystalMapIdCmd = new G4UIcmdWithAnInteger("/Detector/ECal/CrystalMapId",this);
  fSetCrystalMapIdCmd->SetGuidance("Set id of crystal map to use for ECal.");
  fSetCrystalMapIdCmd->SetParameterName("MI",false);
  fSetCrystalMapIdCmd->SetRange("MI >= 0 && MI <= 1");
  fSetCrystalMapIdCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

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

  fSetCrystalCoatingCmd = new G4UIcommand("/Detector/ECal/CrystalCoating",this);
  fSetCrystalCoatingCmd->SetGuidance("Set thickness of paint coating around crystal in mm.");
  G4UIparameter* ccThickParameter = new G4UIparameter("Thick",'d',false);
  ccThickParameter->SetParameterRange("Thick >= 0. && Thick <= 1.");
  fSetCrystalCoatingCmd->SetParameter(ccThickParameter);
  fSetCrystalCoatingCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTedlarThickCmd = new G4UIcommand("/Detector/ECal/TedlarThickness",this);
  fSetTedlarThickCmd->SetGuidance("Set thickness of tedlar slips between crystals in mm.");
  G4UIparameter* ttThickParameter = new G4UIparameter("Thick",'d',false);
  ttThickParameter->SetParameterRange("Thick > 0. && Thick <= 10.");
  fSetTedlarThickCmd->SetParameter(ttThickParameter);
  fSetTedlarThickCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetECalFrontFaceZCmd = new G4UIcommand("/Detector/ECal/FrontFaceZ",this);
  fSetECalFrontFaceZCmd->SetGuidance("Set position along Z of ECal front face in cm.");
  G4UIparameter* effPosZParameter = new G4UIparameter("PosZ",'d',false);
  effPosZParameter->SetParameterRange("PosZ > 100. && PosZ <= 1000.");
  fSetECalFrontFaceZCmd->SetParameter(effPosZParameter);
  fSetECalFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fEnablePanelCmd = new G4UIcmdWithABool("/Detector/ECal/EnablePanel",this);
  fEnablePanelCmd->SetGuidance("Enable (true) or disable (false) panel in front of ECal.");
  fEnablePanelCmd->SetParameterName("EP",true);
  fEnablePanelCmd->SetDefaultValue(true);
  fEnablePanelCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetECalPanelThickCmd = new G4UIcommand("/Detector/ECal/PanelThickness",this);
  fSetECalPanelThickCmd->SetGuidance("Set thickness of plastic panel in front of ECal in cm.");
  G4UIparameter* eptThickParameter = new G4UIparameter("Thick",'d',false);
  eptThickParameter->SetParameterRange("Thick > 0. && Thick <= 10.");
  fSetECalPanelThickCmd->SetParameter(eptThickParameter);
  fSetECalPanelThickCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetECalPanelGapCmd = new G4UIcommand("/Detector/ECal/PanelGap",this);
  fSetECalPanelGapCmd->SetGuidance("Set air gap between ECal and plastic panel in mm.");
  G4UIparameter* eptGapParameter = new G4UIparameter("Gap",'d',false);
  eptGapParameter->SetParameterRange("Gap >= 0. && Gap <= 10.");
  fSetECalPanelGapCmd->SetParameter(eptGapParameter);
  fSetECalPanelGapCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

ECalMessenger::~ECalMessenger()
{

  delete fECalDetectorDir;

  delete fSetECalNRowsCmd;
  delete fSetECalNColsCmd;

  delete fSetCrystalMapIdCmd;

  delete fSetCrystalSizeCmd;
  delete fSetCrystalLengthCmd;

  delete fSetCrystalGapCmd;

  delete fSetCrystalCoatingCmd;

  delete fSetTedlarThickCmd;

  delete fSetECalFrontFaceZCmd;

  delete fEnablePanelCmd;
  delete fSetECalPanelThickCmd;
  delete fSetECalPanelGapCmd;

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
 
 if ( cmd == fSetCrystalMapIdCmd )
    fECalGeometry->SetCrystalMapId(fSetCrystalMapIdCmd->GetNewIntValue(par));

  if ( cmd == fSetCrystalSizeCmd ) {
    G4double s; std::istringstream is(par); is >> s;
    fECalGeometry->SetCrystalSizeX(s*cm);
    fECalGeometry->SetCrystalSizeY(s*cm);
  }

  if ( cmd == fSetCrystalLengthCmd ) {
    G4double l; std::istringstream is(par); is >> l;
    fECalGeometry->SetCrystalSizeZ(l*cm);
  }

  if ( cmd == fSetCrystalGapCmd ) {
    G4double g; std::istringstream is(par); is >> g;
    fECalGeometry->SetCrystalGap(g*mm);
  }

  if ( cmd == fSetCrystalCoatingCmd ) {
    G4double c; std::istringstream is(par); is >> c;
    fECalGeometry->SetCrystalCoating(c*mm);
  }

  if ( cmd == fSetTedlarThickCmd ) {
    G4double t; std::istringstream is(par); is >> t;
    fECalGeometry->SetTedlarThickness(t*mm);
  }

  if ( cmd == fSetECalFrontFaceZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fECalGeometry->SetECalFrontFacePosZ(z*cm);
  }

  if ( cmd == fEnablePanelCmd ) {
    if (fEnablePanelCmd->GetNewBoolValue(par)) {
      fECalGeometry->EnableECalPanel();
    } else {
      fECalGeometry->DisableECalPanel();
    }
  }

  if ( cmd == fSetECalPanelThickCmd ) {
    G4double t; std::istringstream is(par); is >> t;
    fECalGeometry->SetECalPanelThickness(t*cm);
  }

  if ( cmd == fSetECalPanelGapCmd ) {
    G4double g; std::istringstream is(par); is >> g;
    fECalGeometry->SetECalPanelGap(g*mm);
  }

}
