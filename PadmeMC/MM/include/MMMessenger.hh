// MMMessenger.hh
// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
//
// --------------------------------------------------------------

#ifndef MMMessenger_h
#define MMMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class MMDetector;
class MMGeometry;
class G4UIdirectory;
class G4UIcommand;

class MMMessenger: public G4UImessenger
{
public:
  MMMessenger(MMDetector* );
  ~MMMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:

  MMDetector* fMMDetector;
  MMGeometry* fMMGeometry;

  G4UIdirectory* fMMDetectorDir;

  // D. these i don't know how to use
  //D. G4UIcommand* fSetMMInnerRadiusCmd;
  //D. G4UIcommand* fSetMMOuterRadiusCmd;
  G4UIcommand* fSetMMZLengthCmd;

  G4UIcommand* fSetMMFrontFaceZCmd;

};
#endif
