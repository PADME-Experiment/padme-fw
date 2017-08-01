// BeamGenerator.hh
// --------------------------------------------------------------
// History:
//
// 2016-02-11 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#ifndef BeamGenerator_h
#define BeamGenerator_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

#include "BeamParameters.hh"

class G4Event;

class BeamMessenger;
class HistoManager;
class DetectorConstruction;

struct BeamPrimaryPositron
{
  G4double      t;   // Vertex time
  G4ThreeVector pos; // Vertex position
  G4double      E;   // Positron total energy
  G4double      m;   // Positron mass (from PDG)
  G4double      P;   // Positron momentum magnitude
  G4ThreeVector p;   // Positron momentum vector
  G4ThreeVector dir; // Positron direction (unit of momentum vector)
};

class BeamGenerator
{
public:

  BeamGenerator(DetectorConstruction*);
  ~BeamGenerator();
  
public:

  void GenerateBeam(G4Event*);

  G4double GetUbosonMass() { return BeamParameters::GetInstance()->GetUbosonMass(); }

  void CalibrationRunEnable()  { fCalibrationRun = true; }
  void CalibrationRunDisable() { fCalibrationRun = false; }
  G4bool CalibrationRun() { return fCalibrationRun; }

  G4double GetCalibRunEnergy() { return fCalibRunEnergy; }
  void SetCalibRunEnergy(G4double e) { fCalibRunEnergy  = e; }

  G4double GetCalibRunCenterX() { return fCalibRunCenterX; }
  void SetCalibRunCenterX(G4double x) { fCalibRunCenterX = x; }

  G4double GetCalibRunCenterY() { return fCalibRunCenterY; }
  void SetCalibRunCenterY(G4double y) { fCalibRunCenterY = y; }

  G4double GetCalibRunRadius()  { return fCalibRunRadius; }
  void SetCalibRunRadius(G4double r)  { fCalibRunRadius  = r; }

private:

  void GeneratePrimaryPositron();
  void CreateFinalStateUboson();
  void CreateFinalStateThreeGamma();
  void GenerateCalibrationGamma();

  G4double GetGammaAngle(G4ThreeVector ,G4ThreeVector );

  G4double CS(G4double,G4double,G4double,G4double);
  G4double CS2(G4double,G4double,G4double,G4double);

  G4Event* fEvent;
  
  DetectorConstruction* fDetector;

  BeamMessenger* fBeamMessenger;
  HistoManager* fHistoManager;

  BeamPrimaryPositron fPositron;

  // In calibration mode we shoot gamma of fixed energy from
  // the center of the Target to a circle on the ECal surface (flat distribution)
  G4bool   fCalibrationRun;
  G4double fCalibRunEnergy;  // Energy of the photon
  G4double fCalibRunCenterX; // X of center of cirlce 
  G4double fCalibRunCenterY; // Y of center of circle
  G4double fCalibRunRadius;  // Radius of circle

};
#endif
