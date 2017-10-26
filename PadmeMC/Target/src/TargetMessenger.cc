// TargetMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "TargetMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "TargetDetector.hh"
#include "TargetGeometry.hh"

TargetMessenger::TargetMessenger(TargetDetector* det)
:fTargetDetector(det)
{

  fTargetGeometry = TargetGeometry::GetInstance();

  fTargetDetectorDir = new G4UIdirectory("/Detector/Target/");
  fTargetDetectorDir->SetGuidance("UI commands to control Target detector geometry");

  fSetTargetSizeCmd = new G4UIcmdWithADoubleAndUnit("/Detector/Target/Size",this);
  fSetTargetSizeCmd->SetGuidance("Set size (side of squared plaque) of Target detector.");
  fSetTargetSizeCmd->SetParameterName("Size",false);
  fSetTargetSizeCmd->SetDefaultUnit("cm");
  fSetTargetSizeCmd->SetRange("Size >= 0.1 && Size <= 5.");
  fSetTargetSizeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
 
  fSetTargetThicknessCmd = new G4UIcmdWithADoubleAndUnit("/Detector/Target/Thickness",this);
  fSetTargetThicknessCmd->SetGuidance("Set thickness of Target detector.");
  fSetTargetThicknessCmd->SetParameterName("Thick",false);
  fSetTargetThicknessCmd->SetDefaultUnit("um");
  fSetTargetThicknessCmd->SetRange("Thick >= 10. && Thick <= 500.");
  fSetTargetThicknessCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTargetFrontFaceZCmd = new G4UIcmdWithADoubleAndUnit("/Detector/Target/FrontFaceZ",this);
  fSetTargetFrontFaceZCmd->SetGuidance("Set position along Z of Target front face (reference: center of magnet).");
  fSetTargetFrontFaceZCmd->SetParameterName("PosZ",false);
  fSetTargetFrontFaceZCmd->SetDefaultUnit("cm");
  fSetTargetFrontFaceZCmd->SetRange("PosZ >= -100. && PosZ <= -56."); // Upper limit is close to back face of vacuum chamber
  fSetTargetFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  /// digitization parameters

  fSetTargetPitchCmd = new G4UIcmdWithADoubleAndUnit("/Detector/Target/Pitch",this);
  fSetTargetPitchCmd->SetGuidance("Set Pitch of Target detector.");
  fSetTargetPitchCmd->SetParameterName("Pitch",false);
  fSetTargetPitchCmd->SetDefaultUnit("mm");
  fSetTargetPitchCmd->SetRange("Pitch >= 0.5 && Pitch <= 2.");
  fSetTargetPitchCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
 
  fSetTargetStripSizeCmd = new G4UIcmdWithADoubleAndUnit("/Detector/Target/StripSize",this);
  fSetTargetStripSizeCmd->SetGuidance("Set Strip size of Target detector.");
  fSetTargetStripSizeCmd->SetParameterName("StripSize",false);
  fSetTargetStripSizeCmd->SetDefaultUnit("um");
  fSetTargetStripSizeCmd->SetRange("Size >= 425. && Size <= 1700.");
  fSetTargetStripSizeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
 
  fEnableFastDigitizationCmd = new G4UIcmdWithoutParameter("/Detector/Target/EnableFastDigitization",this);
  fEnableFastDigitizationCmd->SetGuidance("Enable fast digitization for Target.");
  fEnableFastDigitizationCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fDisableFastDigitizationCmd = new G4UIcmdWithoutParameter("/Detector/Target/DisableFastDigitization",this);
  fDisableFastDigitizationCmd->SetGuidance("Disable fast digitization for Target.");
  fDisableFastDigitizationCmd->SetGuidance("WARNING: full digitization is time consuming: 170ms/evt");
  fDisableFastDigitizationCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fEnableSaveWaveformToDigiCmd = new G4UIcmdWithoutParameter("/Detector/Target/EnableSaveWaveformToDigi",this);
  fEnableSaveWaveformToDigiCmd->SetGuidance("Enable saving of Target digitized waveforms to persistent digis.");
  fEnableSaveWaveformToDigiCmd->SetGuidance("WARNING: saving waveforms adds 50(30?)KB of data to each event!");
  fEnableSaveWaveformToDigiCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fDisableSaveWaveformToDigiCmd = new G4UIcmdWithoutParameter("/Detector/Target/DisableSaveWaveformToDigi",this);
  fDisableSaveWaveformToDigiCmd->SetGuidance("Disable saving of Target digitized waveforms to persistent digis.");
  fDisableSaveWaveformToDigiCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fEnableReduceWaveformCmd = new G4UIcmdWithoutParameter("/Detector/Target/EnableReduceWaveform",this);
  fEnableReduceWaveformCmd->SetGuidance("Enable reducing of Target digitized waveforms to integration window in persistent digis.");
  fEnableReduceWaveformCmd->SetGuidance("WARNING: reducing waveforms saves XXKB for each event!");
  fEnableReduceWaveformCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fDisableReduceWaveformCmd = new G4UIcmdWithoutParameter("/Detector/Target/DisableSaveWaveformToDigi",this);
  fDisableReduceWaveformCmd->SetGuidance("Disable reducing of Target digitized waveforms in persistent digis.");
  fDisableReduceWaveformCmd->SetGuidance("WARNING: not reducing waveforms adds XXKB for each event!");
  fDisableReduceWaveformCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTargetDigiNChannelsCmd = new G4UIcommand("/Detector/Target/NChannels",this);
  fSetTargetDigiNChannelsCmd->SetGuidance("Set Number of Target channels");
  G4UIparameter* tdNChannelsParameter = new G4UIparameter("NChannels",'i',false);
  tdNChannelsParameter->SetParameterRange("NChannels >= 16 && NChannels <= 64");
  fSetTargetDigiNChannelsCmd->SetParameter(tdNChannelsParameter);
  fSetTargetDigiNChannelsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTargetDigiNTrackDivCmd = new G4UIcommand("/Detector/Target/NTrackDiv",this);
  fSetTargetDigiNTrackDivCmd->SetGuidance("Set number of steps for Target signal derivation");
  G4UIparameter* tdNTrackDivParameter = new G4UIparameter("NTrackDiv",'i',false);
  tdNTrackDivParameter->SetParameterRange("NTrackDiv >= 1 && NTrackDiv <= 100");
  fSetTargetDigiNTrackDivCmd->SetParameter(tdNTrackDivParameter);
  fSetTargetDigiNTrackDivCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTargetDigiNoiseRMSCmd = new G4UIcmdWithADouble("/Detector/Target/NoiseRMS",this);
  fSetTargetDigiNoiseRMSCmd->SetGuidance("Set RMS Noise of Target channels  (1800-5000 e-)");
  fSetTargetDigiNoiseRMSCmd->SetParameterName("NoiseRMS",false);
  fSetTargetDigiNoiseRMSCmd->SetRange("NoiseRMS >= 0.8 && NoiseRMS <= 2.5");
  fSetTargetDigiNoiseRMSCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTargetDigiBaselineCmd = new G4UIcmdWithADouble("/Detector/Target/Baseline",this);
  fSetTargetDigiBaselineCmd->SetGuidance("Set uA baseline for digitizing Target channels.");
  fSetTargetDigiBaselineCmd->SetParameterName("Baseline",false);
  fSetTargetDigiBaselineCmd->SetRange("Baseline >= 0. && Baseline <= 10.");
  fSetTargetDigiBaselineCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTargetDigiNBinsCmd = new G4UIcommand("/Detector/Target/NBins",this);
  fSetTargetDigiNBinsCmd->SetGuidance("Set number of bins for Target digitized signal");
  G4UIparameter* tdNBinsParameter = new G4UIparameter("NBins",'i',false);
  tdNBinsParameter->SetParameterRange("NBins >= 100 && NBins <= 1024");
  fSetTargetDigiNBinsCmd->SetParameter(tdNBinsParameter);
  fSetTargetDigiNBinsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTargetDigiTrigOffsetCmd = new G4UIcmdWithADouble("/Detector/Target/TrigOffset",this);
  fSetTargetDigiTrigOffsetCmd->SetGuidance("Set offset for signal integration.");
  fSetTargetDigiTrigOffsetCmd->SetParameterName("TrigOffset",false);
  fSetTargetDigiTrigOffsetCmd->SetRange("TrigOffset >= 0. && TrigOffset <= 900.");
  fSetTargetDigiTrigOffsetCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTargetDigiWindowCmd = new G4UIcmdWithADouble("/Detector/Target/Window",this);
  fSetTargetDigiWindowCmd->SetGuidance("Set window for integrating Target channels signals.");
  fSetTargetDigiWindowCmd->SetParameterName("Window",false);
  fSetTargetDigiWindowCmd->SetRange("Window >= 0. && Window <= 100.");
  fSetTargetDigiWindowCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTargetDigiThresholdCmd = new G4UIcmdWithADouble("/Detector/Target/Threshold",this);
  fSetTargetDigiThresholdCmd->SetGuidance("Set threshold for digitizing Target channels.");
  fSetTargetDigiThresholdCmd->SetParameterName("Threshold",false);
  fSetTargetDigiThresholdCmd->SetRange("Threshold >= 0. && Threshold <= 5e-9");
  fSetTargetDigiThresholdCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTargetDigiNoiseChargeRMSCmd = new G4UIcmdWithADouble("/Detector/Target/NoiseChargeRMS",this);
  fSetTargetDigiNoiseChargeRMSCmd->SetGuidance("Set RMS Charge of noise channels");
  fSetTargetDigiNoiseChargeRMSCmd->SetParameterName("NoiseChargeRMS",false);
  fSetTargetDigiNoiseChargeRMSCmd->SetRange("NoiseChargeRMS >= 1.e-9  && NoiseChargeRMS <= 1.e-8");
  fSetTargetDigiNoiseChargeRMSCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTargetDigiCCDCmd = new G4UIcmdWithADouble("/Detector/Target/CCD",this);
  fSetTargetDigiCCDCmd->SetGuidance("Set CCD factor");
  fSetTargetDigiCCDCmd->SetParameterName("CCD",false);
  fSetTargetDigiCCDCmd->SetRange("CCD >= 0  && CCD <= 1");
  fSetTargetDigiCCDCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTargetDigiMeV2QCmd = new G4UIcmdWithADouble("/Detector/Target/MeV2Q",this);
  fSetTargetDigiMeV2QCmd->SetGuidance("Set dEdX to charge conversion factor");
  fSetTargetDigiMeV2QCmd->SetParameterName("MeV2Q",false);
  fSetTargetDigiMeV2QCmd->SetRange("MeV2Q >= 1.e-9  && MeV2Q <= 2.5e-9");
  fSetTargetDigiMeV2QCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
}

TargetMessenger::~TargetMessenger()
{

  delete fTargetDetectorDir;

  delete fSetTargetSizeCmd;
  delete fSetTargetThicknessCmd;
  delete fSetTargetFrontFaceZCmd;

  delete fEnableFastDigitizationCmd;
  delete fDisableFastDigitizationCmd;
  delete fEnableSaveWaveformToDigiCmd;
  delete fDisableSaveWaveformToDigiCmd;
  delete fEnableReduceWaveformCmd;
  delete fDisableReduceWaveformCmd;


  delete fSetTargetPitchCmd;
  delete fSetTargetStripSizeCmd;

  delete fSetTargetDigiNChannelsCmd;
  delete fSetTargetDigiNTrackDivCmd;

  delete fSetTargetDigiNoiseRMSCmd;
  delete fSetTargetDigiThresholdCmd;
  delete fSetTargetDigiBaselineCmd;
  delete fSetTargetDigiNBinsCmd;
  delete fSetTargetDigiWindowCmd;
  delete fSetTargetDigiTrigOffsetCmd;

  delete fSetTargetDigiNoiseChargeRMSCmd;
  delete fSetTargetDigiCCDCmd;
  delete fSetTargetDigiMeV2QCmd;

}

void TargetMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetTargetSizeCmd ) {
    fTargetGeometry->SetTargetSizeX(fSetTargetSizeCmd->GetNewDoubleValue(par));
    fTargetGeometry->SetTargetSizeY(fSetTargetSizeCmd->GetNewDoubleValue(par));
  }

  if ( cmd == fSetTargetThicknessCmd )
    fTargetGeometry->SetTargetSizeZ(fSetTargetThicknessCmd->GetNewDoubleValue(par));

  if ( cmd == fSetTargetFrontFaceZCmd )
    fTargetGeometry->SetTargetFrontFacePosZ(fSetTargetFrontFaceZCmd->GetNewDoubleValue(par));

  if ( cmd == fSetTargetPitchCmd )
    fTargetGeometry->SetTargetPitch(fSetTargetPitchCmd->GetNewDoubleValue(par));

  if ( cmd == fSetTargetStripSizeCmd )
    fTargetGeometry->SetTargetStripSize(fSetTargetStripSizeCmd->GetNewDoubleValue(par));

  if ( cmd == fEnableFastDigitizationCmd )  fTargetGeometry->EnableFastDigitization();
  if ( cmd == fDisableFastDigitizationCmd ) fTargetGeometry->DisableFastDigitization();

  if ( cmd == fEnableSaveWaveformToDigiCmd )  fTargetGeometry->EnableSaveWaveformToDigi();
  if ( cmd == fDisableSaveWaveformToDigiCmd ) fTargetGeometry->DisableSaveWaveformToDigi();

  if ( cmd == fEnableReduceWaveformCmd )  fTargetGeometry->EnableReduceWaveform();
  if ( cmd == fDisableReduceWaveformCmd ) fTargetGeometry->DisableReduceWaveform();

  //  if ( cmd == fSetTargetDigiNChannelsCmd )
  //  fTargetGeometry->SetTargetDigiNChannels(fSetTargetDigiNChannelsCmd->GetNewValue(par));
  //  if ( cmd == fSetTargetDigiNTrackDivCmd )
  //  fTargetGeometry->SetTargetDigiNTrackDiv(fSetTargetDigiNTrackDivCmd->GetNewValue(par));

  if ( cmd == fSetTargetDigiNChannelsCmd ) {
    G4int c; std::istringstream is(par); is >> c;
    fTargetGeometry->SetTargetDigiNChannels(c);
  }
  if ( cmd == fSetTargetDigiNTrackDivCmd ) {
    G4int t; std::istringstream is(par); is >> t;
    fTargetGeometry->SetTargetDigiNTrackDiv(t);
  }

  if ( cmd == fSetTargetDigiNoiseRMSCmd )
    fTargetGeometry->SetTargetDigiNoiseRMS(fSetTargetDigiNoiseRMSCmd->GetNewDoubleValue(par));

  if ( cmd == fSetTargetDigiThresholdCmd )
    fTargetGeometry->SetTargetDigiThreshold(fSetTargetDigiThresholdCmd->GetNewDoubleValue(par));

  if ( cmd == fSetTargetDigiNBinsCmd ) {
    G4int b; std::istringstream is(par); is >> b;
    fTargetGeometry->SetTargetDigiNBins(b);
  }
  if ( cmd == fSetTargetDigiBaselineCmd )
    fTargetGeometry->SetTargetDigiBaseline(fSetTargetDigiBaselineCmd->GetNewDoubleValue(par));

  if ( cmd == fSetTargetDigiTrigOffsetCmd )
    fTargetGeometry->SetTargetDigiTrigOffset(fSetTargetDigiTrigOffsetCmd->GetNewDoubleValue(par));

  if ( cmd == fSetTargetDigiWindowCmd )
    fTargetGeometry->SetTargetDigiWindow(fSetTargetDigiWindowCmd->GetNewDoubleValue(par));

  if ( cmd == fSetTargetDigiNoiseChargeRMSCmd )
    fTargetGeometry->SetTargetDigiNoiseChargeRMS(fSetTargetDigiNoiseChargeRMSCmd->GetNewDoubleValue(par));

  if ( cmd == fSetTargetDigiCCDCmd )
    fTargetGeometry->SetTargetDigiCCD(fSetTargetDigiCCDCmd->GetNewDoubleValue(par));

  if ( cmd == fSetTargetDigiMeV2QCmd )
    fTargetGeometry->SetTargetDigiMeV2Q(fSetTargetDigiMeV2QCmd->GetNewDoubleValue(par));


}
