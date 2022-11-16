//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************

#include "StackingAction.hh"

#include "G4Track.hh"
#include "G4NeutrinoE.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Neutron.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4PionZero.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StackingAction::StackingAction()
{

  NGenN=0;

  fTurboMode = false;

  fTurboModeVerbose = 0;

  //fCutG4 = 0.1*cm;

  fEpKillEnergy = 10.*CLHEP::MeV;
  fEmKillEnergy = 10.*CLHEP::MeV;
  fGKillEnergy  =  1.*CLHEP::MeV;
  fNKillEnergy  = 10.*CLHEP::MeV;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StackingAction::~StackingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
double i=0;
double j=0;
double k=0;
double l=0;
double Nene=0;

extern double NNeutrons;
extern double Npionc;
extern double Npi0;
extern double Nmuons;

G4ClassificationOfNewTrack
StackingAction::ClassifyNewTrack(const G4Track* track)
{

  // If we are not in TurboMode, do nothing
  if (! fTurboMode) return fUrgent;

  // Keep primary particles
  if(track->GetParentID() == 0) return fUrgent;

  //Selects and Kill e+/e-/gamma if below threshold
  G4ParticleDefinition* particleType = track->GetDefinition();
  if ( particleType == G4Gamma::GammaDefinition() ) {
    if ( track->GetTotalEnergy()<fGKillEnergy ) {
      l++;
      if (fTurboModeVerbose >= 10)
	G4cout<<particleType->GetParticleName()<<" "<<particleType->GetPDGCharge()<<" "<<track->GetTotalEnergy()<<" "<<l<<G4endl;
      return fKill;
    }
  } else if ( particleType == G4Positron::PositronDefinition() ) {
    if ( track->GetKineticEnergy()<fEpKillEnergy ) {
      k++;
      if (fTurboModeVerbose >= 10)
	G4cout<<particleType->GetParticleName()<<" "<<particleType->GetPDGCharge()<<" "<<track->GetKineticEnergy()<<" "<<k<<G4endl;
      return fKill;
    }
  } else if ( particleType == G4Electron::ElectronDefinition() ) {
    if ( track->GetKineticEnergy()<fEmKillEnergy ) {
      j++;
      if (fTurboModeVerbose >= 10)
	G4cout<<particleType->GetParticleName()<<" "<<particleType->GetPDGCharge()<<" "<<track->GetKineticEnergy()<<" "<<j<<G4endl;
      return fKill;
    }
  } else if (particleType == G4Neutron::NeutronDefinition()) {
    NNeutrons++;
    Nene=track->GetKineticEnergy();
    if (fTurboModeVerbose >= 10)
      G4cout<<particleType->GetParticleName()<<" "<<particleType->GetPDGCharge()<<" "<<Nene<<" "<<NNeutrons<<G4endl;
    if (Nene < fNKillEnergy) return fKill;
  } else if (particleType == G4PionPlus::PionPlusDefinition() || particleType == G4PionMinus::PionMinusDefinition()) {
    Npionc++;
    if (fTurboModeVerbose >= 10)
      G4cout<<particleType->GetParticleName()<<" "<<particleType->GetPDGCharge()<<" "<<track->GetKineticEnergy()<<" "<<Npionc<<G4endl;
  } else if (particleType == G4PionZero::PionZeroDefinition()) {
    Npi0++;
    if (fTurboModeVerbose >= 10)
      G4cout<<particleType->GetParticleName()<<" "<<particleType->GetPDGCharge()<<" "<<track->GetKineticEnergy()<<" "<<Npi0<<G4endl;
  } else if(particleType == G4MuonPlus::MuonPlusDefinition() || particleType == G4MuonMinus::MuonMinusDefinition()) {
    Nmuons++;
    if (fTurboModeVerbose >= 10)
      G4cout<<particleType->GetParticleName()<<" "<<particleType->GetPDGCharge()<<" "<<track->GetKineticEnergy()<<" "<<Nmuons<<G4endl;
  } else {
    if (fTurboModeVerbose >= 10)
      G4cout<<particleType->GetParticleName()<<" "<<particleType->GetPDGCharge()<<" "<<track->GetKineticEnergy()<<G4endl;
  }
  return fUrgent;

}

void StackingAction::EnableTurboMode() {
  fTurboMode = true;
  char buffer[50];
  G4cout << "**************************" << G4endl;
  G4cout << "**  TURBO MODE ENABLED  **" << G4endl;
  sprintf(buffer,"** gamma thr: %5.1f MeV **",fGKillEnergy/CLHEP::MeV);
  G4cout << buffer << G4endl;
  sprintf(buffer,"** e+    thr: %5.1f MeV **",fEpKillEnergy/CLHEP::MeV);
  G4cout << buffer << G4endl;
  sprintf(buffer,"** e-    thr: %5.1f MeV **",fEmKillEnergy/CLHEP::MeV);
  G4cout << buffer << G4endl;
  sprintf(buffer,"** netrn thr: %5.1f MeV **",fNKillEnergy/CLHEP::MeV);
  G4cout << buffer << G4endl;
  G4cout << "**************************" << G4endl;
}

void StackingAction::DisableTurboMode() {
  fTurboMode = false;
  G4cout << "*************************" << G4endl;
  G4cout << "** TURBO MODE DISABLED **" << G4endl;
  G4cout << "*************************" << G4endl;
}

void StackingAction::TurboModeEventSummary() {
  if ( fTurboMode && fTurboModeVerbose ) {
    G4cout << "Here we will show a summary of TurboMode activities for a single event" << G4endl;
  }
}

void StackingAction::TurboModeRunSummary() {
  if ( fTurboMode && fTurboModeVerbose ) {
    G4cout << "Here we will show a summary of TurboMode activities for a full run" << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
