// MagnetMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "MagnetMessenger.hh"

#include "G4UIdirectory.hh"

#include "MagnetStructure.hh"
//#include "MagnetGeometry.hh"

MagnetMessenger::MagnetMessenger(MagnetStructure* mag)
:fMagnetStructure(mag)
{

  //fMagnetGeometry = MagnetGeometry::GetInstance();

  //fMagnetDetectorDir = new G4UIdirectory("/Detector/Magnet/");
  //fMagnetDetectorDir->SetGuidance("UI commands to control Dipole Magnet geometry");

}

MagnetMessenger::~MagnetMessenger()
{

  //delete fMagnetDetectorDir;

}

void MagnetMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{;}
