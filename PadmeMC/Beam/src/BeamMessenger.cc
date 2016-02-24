// BeamMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2016-02-11 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "BeamMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithAString.hh"

#include "BeamGenerator.hh"
#include "BeamParameters.hh"

BeamMessenger::BeamMessenger(BeamGenerator* bgen)
 :fBeamGenerator(bgen)
{

  fBeamParameters = BeamParameters::GetInstance();

  fBeamGeneratorDir = new G4UIdirectory("/beam/");
  fBeamGeneratorDir->SetGuidance("UI commands to control Beam generation");

  fSetNPositronsPerBunchCmd = new G4UIcmdWithAnInteger("/beam/n_e+_per_bunch",this);
  fSetNPositronsPerBunchCmd->SetGuidance("Set number of positrons in each bunch.");
  fSetNPositronsPerBunchCmd->SetParameterName("NP",false);
  fSetNPositronsPerBunchCmd->SetRange("NP > 0 && NP <= 10000");
  fSetNPositronsPerBunchCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fEnableNPositronsPerBunchSpreadCmd = new G4UIcmdWithABool("/beam/n_e+_poisson_on",this);
  fEnableNPositronsPerBunchSpreadCmd->SetGuidance("Enable (true) or disable (false) poissonian spread for the number of positrons in each bunch.");
  fEnableNPositronsPerBunchSpreadCmd->SetParameterName("ES",true);
  fEnableNPositronsPerBunchSpreadCmd->SetDefaultValue(true);
  fEnableNPositronsPerBunchSpreadCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fEnableBunchStructureCmd = new G4UIcmdWithABool("/beam/bunch_structure_on",this);
  fEnableBunchStructureCmd->SetGuidance("Enable (true) or disable (false) application of bunch time structure to generation time of the primary positrons.");
  fEnableBunchStructureCmd->SetParameterName("EBS",true);
  fEnableBunchStructureCmd->SetDefaultValue(true);
  fEnableBunchStructureCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetBunchTimeLengthCmd = new G4UIcmdWithADoubleAndUnit("/beam/bunch_time_length",this);
  fSetBunchTimeLengthCmd->SetGuidance("Set time length of a particle bunch.");
  fSetBunchTimeLengthCmd->SetParameterName("BTL",false);
  fSetBunchTimeLengthCmd->SetDefaultUnit("ns");
  fSetBunchTimeLengthCmd->SetRange("BTL > 0. && BTL <= 500.");
  fSetBunchTimeLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetMicroBunchTimeLengthCmd = new G4UIcmdWithADoubleAndUnit("/beam/ubunch_time_length",this);
  fSetMicroBunchTimeLengthCmd->SetGuidance("Set time length of the micro bunch.");
  fSetMicroBunchTimeLengthCmd->SetParameterName("MTL",false);
  fSetMicroBunchTimeLengthCmd->SetDefaultUnit("ns");
  fSetMicroBunchTimeLengthCmd->SetRange("MTL > 0. && MTL <= 1.");
  fSetMicroBunchTimeLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetMicroBunchTimeDelayCmd = new G4UIcmdWithADoubleAndUnit("/beam/ubunch_time_delay",this);
  fSetMicroBunchTimeDelayCmd->SetGuidance("Set time delay between and of a micro bunch and start of next micro bunch.");
  fSetMicroBunchTimeDelayCmd->SetParameterName("MTD",false);
  fSetMicroBunchTimeDelayCmd->SetDefaultUnit("ns");
  fSetMicroBunchTimeDelayCmd->SetRange("MTD > 0. && MTD <= 1.");
  fSetMicroBunchTimeDelayCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetBeamCenterPosXCmd = new G4UIcmdWithADoubleAndUnit("/beam/position_x",this);
  fSetBeamCenterPosXCmd->SetGuidance("Set center of beam X coordinate at Target entrance.");
  fSetBeamCenterPosXCmd->SetParameterName("X",false);
  fSetBeamCenterPosXCmd->SetDefaultUnit("mm");
  fSetBeamCenterPosXCmd->SetRange("X >= -20. && X <= 20.");
  fSetBeamCenterPosXCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetBeamCenterPosYCmd = new G4UIcmdWithADoubleAndUnit("/beam/position_y",this);
  fSetBeamCenterPosYCmd->SetGuidance("Set center of beam Y coordinate at Target entrance.");
  fSetBeamCenterPosYCmd->SetParameterName("Y",false);
  fSetBeamCenterPosYCmd->SetDefaultUnit("mm");
  fSetBeamCenterPosYCmd->SetRange("Y >= -20. && Y <= 20.");
  fSetBeamCenterPosYCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fEnableBeamCenterPosSpreadCmd = new G4UIcmdWithABool("/beam/position_spread_on",this);
  fEnableBeamCenterPosSpreadCmd->SetGuidance("Enable (true) or disable (false) gaussian spread of beam center X/Y coordinates.");
  fEnableBeamCenterPosSpreadCmd->SetParameterName("ECS",true);
  fEnableBeamCenterPosSpreadCmd->SetDefaultValue(true);
  fEnableBeamCenterPosSpreadCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetBeamCenterPosXSpreadCmd = new G4UIcmdWithADoubleAndUnit("/beam/position_x_spread",this);
  fSetBeamCenterPosXSpreadCmd->SetGuidance("Set sigma of gaussian spread for center of beam X coordinate at Target entrance.");
  fSetBeamCenterPosXSpreadCmd->SetParameterName("XS",false);
  fSetBeamCenterPosXSpreadCmd->SetDefaultUnit("mm");
  fSetBeamCenterPosXSpreadCmd->SetRange("XS >= 0. && XS <= 5.");
  fSetBeamCenterPosXSpreadCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetBeamCenterPosYSpreadCmd = new G4UIcmdWithADoubleAndUnit("/beam/position_y_spread",this);
  fSetBeamCenterPosYSpreadCmd->SetGuidance("Set sigma of gaussian spread for center of beam Y coordinate at Target entrance.");
  fSetBeamCenterPosYSpreadCmd->SetParameterName("YS",false);
  fSetBeamCenterPosYSpreadCmd->SetDefaultUnit("mm");
  fSetBeamCenterPosYSpreadCmd->SetRange("YS >= 0. && YS <= 5.");
  fSetBeamCenterPosYSpreadCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetBeamMomentumCmd = new G4UIcmdWithADoubleAndUnit("/beam/momentum",this);
  fSetBeamMomentumCmd->SetGuidance("Set momentum of beam.");
  fSetBeamMomentumCmd->SetParameterName("P",false);
  fSetBeamMomentumCmd->SetDefaultUnit("MeV");
  fSetBeamMomentumCmd->SetRange("P > 0. && P <= 1000.");
  fSetBeamMomentumCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fEnableBeamMomentumSpreadCmd = new G4UIcmdWithABool("/beam/momentum_spread_on",this);
  fEnableBeamMomentumSpreadCmd->SetGuidance("Enable (true) or disable (false) gaussian spread of beam momentum.");
  fEnableBeamMomentumSpreadCmd->SetParameterName("EPS",true);
  fEnableBeamMomentumSpreadCmd->SetDefaultValue(true);
  fEnableBeamMomentumSpreadCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetBeamMomentumSpreadCmd = new G4UIcmdWithADoubleAndUnit("/beam/momentum_spread",this);
  fSetBeamMomentumSpreadCmd->SetGuidance("Set sigma of gaussian spread on momentum of beam.");
  fSetBeamMomentumSpreadCmd->SetParameterName("PS",false);
  fSetBeamMomentumSpreadCmd->SetDefaultUnit("MeV");
  fSetBeamMomentumSpreadCmd->SetRange("PS >= 0. && PS <= 100.");
  fSetBeamMomentumSpreadCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetBeamDirectionCmd = new G4UIcmdWith3Vector("/beam/direction",this);
  fSetBeamDirectionCmd->SetGuidance("Set 3vector direction of beam at Target entrance.");
  fSetBeamDirectionCmd->SetParameterName("VX","VY","VZ",false);
  fSetBeamDirectionCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fEnableBeamEmittanceCmd = new G4UIcmdWithABool("/beam/emittance_on",this);
  fEnableBeamEmittanceCmd->SetGuidance("Enable (true) or disable (false) gaussian spread of beam direction.");
  fEnableBeamEmittanceCmd->SetParameterName("EEM",true);
  fEnableBeamEmittanceCmd->SetDefaultValue(true);
  fEnableBeamEmittanceCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetBeamEmittanceXCmd = new G4UIcmdWithADouble("/beam/emittance_x",this);
  fSetBeamEmittanceXCmd->SetGuidance("Set sigma of gaussian spread on beam X direction.");
  fSetBeamEmittanceXCmd->SetParameterName("EX",false);
  fSetBeamEmittanceXCmd->SetRange("EX >= 0. && EX <= 1.");
  fSetBeamEmittanceXCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetBeamEmittanceYCmd = new G4UIcmdWithADouble("/beam/emittance_y",this);
  fSetBeamEmittanceYCmd->SetGuidance("Set sigma of gaussian spread on beam Y direction.");
  fSetBeamEmittanceYCmd->SetParameterName("EY",false);
  fSetBeamEmittanceYCmd->SetRange("EY >= 0. && EY <= 1.");
  fSetBeamEmittanceYCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetNUbosonDecaysPerBunchCmd = new G4UIcmdWithAnInteger("/beam/n_uboson_per_bunch",this);
  fSetNUbosonDecaysPerBunchCmd->SetGuidance("Set number of Uboson decays in each bunch.");
  fSetNUbosonDecaysPerBunchCmd->SetParameterName("NU",false);
  fSetNUbosonDecaysPerBunchCmd->SetRange("NU == 0 || NU == 1");
  fSetNUbosonDecaysPerBunchCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetUbosonMassCmd = new G4UIcmdWithADoubleAndUnit("/beam/uboson_mass",this);
  fSetUbosonMassCmd->SetGuidance("Set mass of Uboson (in energy units, e.g. MeV).");
  fSetUbosonMassCmd->SetParameterName("UM",false);
  fSetUbosonMassCmd->SetDefaultUnit("MeV");
  fSetUbosonMassCmd->SetRange("UM > 0. && UM <= 100.");
  fSetUbosonMassCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetNThreePhotonDecaysPerBunchCmd = new G4UIcmdWithAnInteger("/beam/n_3g_per_bunch",this);
  fSetNThreePhotonDecaysPerBunchCmd->SetGuidance("Set number of three photon decays in each bunch.");
  fSetNThreePhotonDecaysPerBunchCmd->SetParameterName("NTP",false);
  fSetNThreePhotonDecaysPerBunchCmd->SetRange("NTP == 0 || NTP == 1");
  fSetNThreePhotonDecaysPerBunchCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetThreePhotonDecaysFilenameCmd = new G4UIcmdWithAString("/beam/3g_file",this);
  fSetThreePhotonDecaysFilenameCmd->SetParameterName("TPF",false);
  fSetThreePhotonDecaysFilenameCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fEnableCalibRunCmd = new G4UIcmdWithABool("/beam/calibration",this);
  fEnableCalibRunCmd->SetGuidance("Enable (true) or disable (false) calibration beam, i.e. photon of given energy pointing to ECal.");
  fEnableCalibRunCmd->SetParameterName("CR",false);
  fEnableCalibRunCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetCalibRunEnergyCmd = new G4UIcmdWithADoubleAndUnit("/beam/calib_energy",this);
  fSetCalibRunEnergyCmd->SetGuidance("Set energy of calibration gamma.");
  fSetCalibRunEnergyCmd->SetParameterName("GE",false);
  fSetCalibRunEnergyCmd->SetDefaultUnit("MeV");
  fSetCalibRunEnergyCmd->SetRange("GE > 0. && GE <= 1000.");
  fSetCalibRunEnergyCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetCalibRunCenterXCmd = new G4UIcmdWithADoubleAndUnit("/beam/calib_x",this);
  fSetCalibRunCenterXCmd->SetGuidance("Set x coordinate for the impact point of the calibration gamma on ECal front face.");
  fSetCalibRunCenterXCmd->SetParameterName("X",false);
  fSetCalibRunCenterXCmd->SetDefaultUnit("cm");
  fSetCalibRunCenterXCmd->SetRange("X >= -30. && X <= 30.");
  fSetCalibRunCenterXCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetCalibRunCenterYCmd = new G4UIcmdWithADoubleAndUnit("/beam/calib_y",this);
  fSetCalibRunCenterYCmd->SetGuidance("Set y coordinate for the impact point of the calibration gamma on ECal front face.");
  fSetCalibRunCenterYCmd->SetParameterName("Y",false);
  fSetCalibRunCenterYCmd->SetDefaultUnit("cm");
  fSetCalibRunCenterYCmd->SetRange("Y >= -30. && Y <= 30.");
  fSetCalibRunCenterYCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetCalibRunRadiusCmd = new G4UIcmdWithADoubleAndUnit("/beam/calib_radius",this);
  fSetCalibRunRadiusCmd->SetGuidance("Set radius of circle for the impact point of the calibration gamma on ECal front face.");
  fSetCalibRunRadiusCmd->SetParameterName("R",false);
  fSetCalibRunRadiusCmd->SetDefaultUnit("cm");
  fSetCalibRunRadiusCmd->SetRange("R >= 0. && R <= 30.");
  fSetCalibRunRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

BeamMessenger::~BeamMessenger()
{

  delete fBeamGeneratorDir;

  delete fSetNPositronsPerBunchCmd;
  delete fEnableNPositronsPerBunchSpreadCmd;

  delete fEnableBunchStructureCmd;
  delete fSetBunchTimeLengthCmd;
  delete fSetMicroBunchTimeLengthCmd;
  delete fSetMicroBunchTimeDelayCmd;

  delete fSetBeamCenterPosXCmd;
  delete fSetBeamCenterPosYCmd;
  delete fEnableBeamCenterPosSpreadCmd;
  delete fSetBeamCenterPosXSpreadCmd;
  delete fSetBeamCenterPosYSpreadCmd;

  delete fSetBeamMomentumCmd;
  delete fEnableBeamMomentumSpreadCmd;
  delete fSetBeamMomentumSpreadCmd;

  delete fSetBeamDirectionCmd;
  delete fEnableBeamEmittanceCmd;
  delete fSetBeamEmittanceXCmd;
  delete fSetBeamEmittanceYCmd;

  delete fSetNUbosonDecaysPerBunchCmd;
  delete fSetUbosonMassCmd;

  delete fSetNThreePhotonDecaysPerBunchCmd;
  delete fSetThreePhotonDecaysFilenameCmd;

  delete fEnableCalibRunCmd;
  delete fSetCalibRunEnergyCmd;
  delete fSetCalibRunCenterXCmd;
  delete fSetCalibRunCenterYCmd;
  delete fSetCalibRunRadiusCmd;

}

void BeamMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetNPositronsPerBunchCmd )
    fBeamParameters->SetNPositronsPerBunch(fSetNPositronsPerBunchCmd->GetNewIntValue(par));

  else if ( cmd == fEnableNPositronsPerBunchSpreadCmd ) {
    if (fEnableNPositronsPerBunchSpreadCmd->GetNewBoolValue(par)) {
      fBeamParameters->NPositronsPerBunchEnableSpread();
    } else {
      fBeamParameters->NPositronsPerBunchDisableSpread();
    }
  }

  else if ( cmd == fEnableBunchStructureCmd ) {
    if (fEnableBunchStructureCmd->GetNewBoolValue(par)) {
      fBeamParameters->BeamEnableBunchStructure();
    } else {
      fBeamParameters->BeamDisableBunchStructure();
    }
  }

  else if ( cmd == fSetBunchTimeLengthCmd )
    fBeamParameters->SetBunchTimeLength(fSetBunchTimeLengthCmd->GetNewDoubleValue(par));

  else if ( cmd == fSetMicroBunchTimeLengthCmd )
    fBeamParameters->SetMicroBunchTimeLength(fSetMicroBunchTimeLengthCmd->GetNewDoubleValue(par));

  else if ( cmd == fSetMicroBunchTimeDelayCmd )
    fBeamParameters->SetMicroBunchTimeDelay(fSetMicroBunchTimeDelayCmd->GetNewDoubleValue(par));

  else if ( cmd == fSetBeamCenterPosXCmd )
    fBeamParameters->SetBeamCenterPosX(fSetBeamCenterPosXCmd->GetNewDoubleValue(par));

  else if ( cmd == fSetBeamCenterPosYCmd )
    fBeamParameters->SetBeamCenterPosY(fSetBeamCenterPosYCmd->GetNewDoubleValue(par));

  else if ( cmd == fEnableBeamCenterPosSpreadCmd ) {
    if (fEnableBeamCenterPosSpreadCmd->GetNewBoolValue(par)) {
      fBeamParameters->BeamCenterPosEnableSpread();
    } else {
      fBeamParameters->BeamCenterPosDisableSpread();
    }
  }

  else if ( cmd == fSetBeamCenterPosXSpreadCmd )
    fBeamParameters->SetBeamCenterPosXSpread(fSetBeamCenterPosXSpreadCmd->GetNewDoubleValue(par));

  else if ( cmd == fSetBeamCenterPosYSpreadCmd )
    fBeamParameters->SetBeamCenterPosYSpread(fSetBeamCenterPosYSpreadCmd->GetNewDoubleValue(par));

  else if ( cmd == fSetBeamMomentumCmd )
    fBeamParameters->SetBeamMomentum(fSetBeamMomentumCmd->GetNewDoubleValue(par));

  else if ( cmd == fEnableBeamMomentumSpreadCmd ) {
    if (fEnableBeamMomentumSpreadCmd->GetNewBoolValue(par)) {
      fBeamParameters->BeamMomentumEnableSpread();
    } else {
      fBeamParameters->BeamMomentumDisableSpread();
    }
  }

  else if ( cmd == fSetBeamMomentumSpreadCmd )
    fBeamParameters->SetBeamMomentumSpread(fSetBeamMomentumSpreadCmd->GetNewDoubleValue(par));

  else if ( cmd == fSetBeamDirectionCmd )
    fBeamParameters->SetBeamDirection(fSetBeamDirectionCmd->GetNew3VectorValue(par));

  else if ( cmd == fEnableBeamEmittanceCmd ) {
    if (fEnableBeamEmittanceCmd->GetNewBoolValue(par)) {
      fBeamParameters->BeamEnableEmittance();
    } else {
      fBeamParameters->BeamDisableEmittance();
    }
  }

  else if ( cmd == fSetBeamEmittanceXCmd )
    fBeamParameters->SetBeamEmittanceX(fSetBeamEmittanceXCmd->GetNewDoubleValue(par));

  else if ( cmd == fSetBeamEmittanceYCmd )
    fBeamParameters->SetBeamEmittanceY(fSetBeamEmittanceYCmd->GetNewDoubleValue(par));

  else if ( cmd == fSetNUbosonDecaysPerBunchCmd )
    fBeamParameters->SetNUbosonDecaysPerBunch(fSetNUbosonDecaysPerBunchCmd->GetNewIntValue(par));

  else if ( cmd == fSetUbosonMassCmd )
    fBeamParameters->SetUbosonMass(fSetUbosonMassCmd->GetNewDoubleValue(par));

  else if ( cmd == fSetNThreePhotonDecaysPerBunchCmd )
    fBeamParameters->SetNThreePhotonDecaysPerBunch(fSetNThreePhotonDecaysPerBunchCmd->GetNewIntValue(par));

  else if ( cmd == fSetThreePhotonDecaysFilenameCmd )
    fBeamParameters->SetThreePhotonDecaysFilename(par);

  else if ( cmd == fEnableCalibRunCmd ) {
    if (fEnableCalibRunCmd->GetNewBoolValue(par)) {
      fBeamGenerator->CalibrationRunEnable();
    } else {
      fBeamGenerator->CalibrationRunDisable();
    }
  }

  else if ( cmd == fSetCalibRunEnergyCmd )
    fBeamGenerator->SetCalibRunEnergy(fSetCalibRunEnergyCmd->GetNewDoubleValue(par));

  else if ( cmd == fSetCalibRunCenterXCmd )
    fBeamGenerator->SetCalibRunCenterX(fSetCalibRunCenterXCmd->GetNewDoubleValue(par));

  else if ( cmd == fSetCalibRunCenterYCmd )
    fBeamGenerator->SetCalibRunCenterY(fSetCalibRunCenterYCmd->GetNewDoubleValue(par));

  else if ( cmd == fSetCalibRunRadiusCmd )
    fBeamGenerator->SetCalibRunRadius(fSetCalibRunRadiusCmd->GetNewDoubleValue(par));

}

G4String BeamMessenger::GetCurrentValue(G4UIcommand* cmd)
{

  G4String cv;

  if ( cmd == fSetNPositronsPerBunchCmd )
    cv = fSetNPositronsPerBunchCmd->ConvertToString(fBeamParameters->GetNPositronsPerBunch());

  else if ( cmd == fEnableNPositronsPerBunchSpreadCmd )
    cv = fEnableNPositronsPerBunchSpreadCmd->ConvertToString(fBeamParameters->NPositronsPerBunchApplySpread());

  else if ( cmd == fEnableBunchStructureCmd )
    cv = fEnableBunchStructureCmd->ConvertToString(fBeamParameters->BeamApplyBunchStructure());

  else if ( cmd == fSetBunchTimeLengthCmd )
    cv = fSetBunchTimeLengthCmd->ConvertToString(fBeamParameters->GetBunchTimeLength(),"ns");

  else if ( cmd == fSetMicroBunchTimeLengthCmd )
    cv = fSetMicroBunchTimeLengthCmd->ConvertToString(fBeamParameters->GetMicroBunchTimeLength(),"ns");

  else if ( cmd == fSetMicroBunchTimeDelayCmd )
    cv = fSetMicroBunchTimeDelayCmd->ConvertToString(fBeamParameters->GetMicroBunchTimeDelay(),"ns");

  else if ( cmd == fSetBeamCenterPosXCmd )
    cv = fSetBeamCenterPosXCmd->ConvertToString(fBeamParameters->GetBeamCenterPosX(),"mm");

  else if ( cmd == fSetBeamCenterPosYCmd )
    cv = fSetBeamCenterPosYCmd->ConvertToString(fBeamParameters->GetBeamCenterPosY(),"mm");

  else if ( cmd == fEnableBeamCenterPosSpreadCmd )
    cv = fEnableBeamCenterPosSpreadCmd->ConvertToString(fBeamParameters->BeamCenterPosApplySpread());

  else if ( cmd == fSetBeamCenterPosXSpreadCmd )
    cv = fSetBeamCenterPosXSpreadCmd->ConvertToString(fBeamParameters->GetBeamCenterPosXSpread(),"mm");

  else if ( cmd == fSetBeamCenterPosYSpreadCmd )
    cv = fSetBeamCenterPosYSpreadCmd->ConvertToString(fBeamParameters->GetBeamCenterPosYSpread(),"mm");

  else if ( cmd == fSetBeamMomentumCmd )
    cv = fSetBeamMomentumCmd->ConvertToString(fBeamParameters->GetBeamMomentum(),"MeV");

  else if ( cmd == fEnableBeamMomentumSpreadCmd )
    cv = fEnableBeamMomentumSpreadCmd->ConvertToString(fBeamParameters->BeamMomentumApplySpread());

  else if ( cmd == fSetBeamMomentumSpreadCmd )
    cv = fSetBeamMomentumSpreadCmd->ConvertToString(fBeamParameters->GetBeamMomentumSpread(),"MeV");

  else if ( cmd == fSetBeamDirectionCmd )
    cv = fSetBeamDirectionCmd->ConvertToString(fBeamParameters->GetBeamDirection());

  else if ( cmd == fEnableBeamEmittanceCmd )
    cv = fEnableBeamEmittanceCmd->ConvertToString(fBeamParameters->BeamApplyEmittance());

  else if ( cmd == fSetBeamEmittanceXCmd )
    cv = fSetBeamEmittanceXCmd->ConvertToString(fBeamParameters->GetBeamEmittanceX());

  else if ( cmd == fSetBeamEmittanceYCmd )
    cv = fSetBeamEmittanceYCmd->ConvertToString(fBeamParameters->GetBeamEmittanceY());

  else if ( cmd == fSetNUbosonDecaysPerBunchCmd )
    cv = fSetNUbosonDecaysPerBunchCmd->ConvertToString(fBeamParameters->GetNUbosonDecaysPerBunch());

  else if ( cmd == fSetUbosonMassCmd )
    cv = fSetUbosonMassCmd->ConvertToString(fBeamParameters->GetUbosonMass(),"MeV");

  else if ( cmd == fSetNThreePhotonDecaysPerBunchCmd )
    cv = fSetNThreePhotonDecaysPerBunchCmd->ConvertToString(fBeamParameters->GetNThreePhotonDecaysPerBunch());

  else if ( cmd == fSetThreePhotonDecaysFilenameCmd )
    cv = fBeamParameters->GetThreePhotonDecaysFilename();

  else if ( cmd == fEnableCalibRunCmd )
    cv = fEnableCalibRunCmd->ConvertToString(fBeamGenerator->CalibrationRun());

  else if ( cmd == fSetCalibRunEnergyCmd )
    cv = fSetCalibRunEnergyCmd->ConvertToString(fBeamGenerator->GetCalibRunEnergy());

  else if ( cmd == fSetCalibRunCenterXCmd )
    cv = fSetCalibRunCenterXCmd->ConvertToString(fBeamGenerator->GetCalibRunCenterX());

  else if ( cmd == fSetCalibRunCenterYCmd )
    cv = fSetCalibRunCenterYCmd->ConvertToString(fBeamGenerator->GetCalibRunCenterY());

  else if ( cmd == fSetCalibRunRadiusCmd )
    cv = fSetCalibRunRadiusCmd->ConvertToString(fBeamGenerator->GetCalibRunRadius());

  return cv;

}
