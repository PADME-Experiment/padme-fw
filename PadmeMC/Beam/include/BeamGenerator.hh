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
#include "TRandom3.h"

#include <fstream>

class G4Event;

class BeamMessenger;
class HistoManager;
class DetectorConstruction;
class MCTruthManager;

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

// MR 26/11/2026 input file of a file-based generator (2gamma, 3gamma, BhaBha, Babayaga).
// It stays open between events: before, the file was reopened and re-read from the
// beginning at every event, i.e. a cost growing with the event number (O(N^2) per run).
struct BeamGeneratorInputFile
{
  std::ifstream stream;
  G4String      name;
  G4int         nRead = 0; // lines (Babayaga: event headers) already read from the stream
};

class BeamGenerator
{
public:

  BeamGenerator(DetectorConstruction*);
  ~BeamGenerator();
  
public:

  void GenerateBeam(G4Event*);

  G4double GetUbosonMass() { return BeamParameters::GetInstance()->GetUbosonMass(); }

private:

  void GeneratePrimaryPositron();
  void GenerateTargetPositron();
  void CreateFinalStateUboson();
  void CreateFinalStateThreeGamma(G4double);
  void CreateFinalStateTwoGamma(G4double);
  void CreateFinalStateBhaBha(G4double);
  void CreateFinalStateBabayaga(G4double);
  void CreateFinalStateBabayagaGG(G4double);
  void GenerateCalibrationParticle();

  G4double GetGammaAngle(G4ThreeVector ,G4ThreeVector );

  G4double CS(G4double,G4double,G4double,G4double);
  G4double CS2(G4double,G4double,G4double,G4double);

  std::ifstream& OpenInputFile(BeamGeneratorInputFile&, const G4String&, const char*);
  BeamGeneratorInputFile fThreeGammaFile;
  BeamGeneratorInputFile fTwoGammaFile;
  BeamGeneratorInputFile fBhaBhaFile;
  BeamGeneratorInputFile fBabayagaFile;
  BeamGeneratorInputFile fBabayagaGGFile;

  G4Event* fEvent;
  
  DetectorConstruction* fDetector;

  MCTruthManager* fMCTruthMgr;

  BeamMessenger* fBeamMessenger;
  HistoManager* fHistoManager;

  BeamPrimaryPositron fPositron;
  TRandom3* fRndm3;
};
#endif
