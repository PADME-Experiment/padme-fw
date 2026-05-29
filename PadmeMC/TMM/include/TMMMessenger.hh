// TMMMessenger.hh
// --------------------------------------------------------------
// History:
//
// Created by Elisa Di Meco (elisa.dimeco@lnf.infn.it) 2026-05-27
//
// --------------------------------------------------------------

#ifndef TMMMessenger_h
#define TMMMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class TMMDetector;
class TMMGeometry;
class G4UIdirectory;
class G4UIcommand;

class TMMMessenger: public G4UImessenger
{
public:
  TMMMessenger(TMMDetector* );
  ~TMMMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:

  TMMDetector* fTMMDetector;
  TMMGeometry* fTMMGeometry;

  G4UIdirectory* fTMMDetectorDir;

  // D. these i don't know how to use
  //D. G4UIcommand* fSetTMMInnerRadiusCmd;
  //D. G4UIcommand* fSetTMMOuterRadiusCmd;
  G4UIcommand* fSetTMMZLengthCmd;

  G4UIcommand* fSetTMMFrontFaceZCmd;

};
#endif
