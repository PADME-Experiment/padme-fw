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
//
// $Id$
//
/// \file B3StackingAction.hh
/// \brief Definition of the B3StackingAction class

#ifndef StackingAction_h
#define StackingAction_h 1

#include "G4UserStackingAction.hh"
#include "globals.hh"

/// Stacking action class : manage the newly generated particles
///
/// One wishes do not track secondary neutrino.Therefore one kills it 
/// immediately, before created particles will  put in a stack.

class StackingAction : public G4UserStackingAction
{
public:

  StackingAction();
  virtual ~StackingAction();
     
  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);
  double GetNGenN()   {return NGenN;};
  //void   SetNGenN(float value);

  void EnableTurboMode();
  void DisableTurboMode();

  void SetTurboModeVerboseLevel(G4int l) { fTurboModeVerbose = l; }
  G4int GetTurboModeVerboseLevel() { return fTurboModeVerbose; }

  //G4double GetCutG4() { return fCutG4; }
  //void SetCutG4(G4double v) { fCutG4 = v; }

  void SetEpKillEnergy(G4double v) { fEpKillEnergy = v; }
  void SetEmKillEnergy(G4double v) { fEmKillEnergy = v; }
  void SetGKillEnergy(G4double v)  { fGKillEnergy = v; }
  void SetNKillEnergy(G4double v)  { fNKillEnergy = v; }
  G4double GetEpKillEnergyutG4() { return fEpKillEnergy; }
  G4double GetEmKillEnergyutG4() { return fEmKillEnergy; }
  G4double GetGKillEnergyutG4()  { return fGKillEnergy; }
  G4double GetNKillEnergyutG4()  { return fNKillEnergy; }

  void TurboModeRunSummary();
  void TurboModeEventSummary();

private:

  G4double NGenN;
  G4double NGenPI0;
  G4double NGenPIC;

  G4bool fTurboMode;

  G4int fTurboModeVerbose;

  //G4double fCutG4;
  G4double fEpKillEnergy; // Cut on kinetic energy of positrons
  G4double fEmKillEnergy; // Cut on kinetic energy of electrons
  G4double fGKillEnergy; // Cut on energy of gammas
  G4double fNKillEnergy; // Cut on kinetic energy of neutrons

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

