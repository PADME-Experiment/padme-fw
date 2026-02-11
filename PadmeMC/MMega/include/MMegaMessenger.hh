// MMegaMessenger.hh
// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
//
// --------------------------------------------------------------

#ifndef MMegaMessenger_h
#define MMegaMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class MMegaDetector;
class MMegaGeometry;
class G4UIdirectory;
class G4UIcommand;

class MMegaMessenger: public G4UImessenger
{
public:
  MMegaMessenger(MMegaDetector* );
  ~MMegaMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:

  MMegaDetector* fMMegaDetector;
  MMegaGeometry* fMMegaGeometry;

  G4UIdirectory* fMMegaDetectorDir;

  // D. these i don't know how to use
  //D. G4UIcommand* fSetMMegaInnerRadiusCmd;
  //D. G4UIcommand* fSetMMegaOuterRadiusCmd;
  G4UIcommand* fSetMMegaZLengthCmd;

  G4UIcommand* fSetMMegaFrontFaceZCmd;

};
#endif
