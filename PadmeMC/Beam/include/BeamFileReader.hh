// BeamFileReader.hh
// --------------------------------------------------------------
// Reads primary beam particles from an external ROOT TTree
// (branches NFlag,p,PX,PY,PZ,theta,phi,x,y,z; x,y,z in mm, p,PX,PY,PZ in MeV),
// keeps only the entries
// with the selected NFlag and serves them one by one, in the file frame
// (beam along +Z, offsets wrt the beam axis). The roto-translation to the
// PADME frame (/beam/direction, /beam/position_x,y,z) is applied in
// BeamGenerator::GeneratePrimaryPositron.
// When the end of the sample is reached it restarts from the beginning.
//
// All settings are taken from BeamParameters (set via BeamMessenger,
// /beam/input_file* commands) when the file is loaded, i.e. at the
// first generated particle.
// --------------------------------------------------------------

#ifndef BeamFileReader_h
#define BeamFileReader_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "TH1D.h"
#include <vector>

struct BeamFileParticle
{
  G4ThreeVector pos;  // (x,y,0) offset wrt beam axis (G4 units)
  G4ThreeVector mom;  // momentum vector, beam along +Z (G4 units)
};

class BeamFileReader
{
public:

  static BeamFileReader* GetInstance();
  ~BeamFileReader();

  // Fills 'part' with the next particle (file frame, G4 units).
  // Loads the file on first call, wraps around at end of sample.
  void GetNextParticle(BeamFileParticle& part);
  G4long GetNRecycles() const { return fNRecycles; }
  TH1D* GetPOTProfileHistogram(){
    if (!fPOTLoaded) LoadPOTFile();
    return fPOTProfile;
  }
private:

  BeamFileReader();
  void LoadFile();
  void LoadPOTFile();
  static BeamFileReader* fInstance;

  G4bool fLoaded;
  G4bool fPOTLoaded;
  // File-frame values, already converted to G4 units
  std::vector<G4ThreeVector> fPos;
  std::vector<G4ThreeVector> fMom;
  size_t fIndex;
  G4long fNRecycles;
  TH1D* fPOTProfile;
};
#endif
