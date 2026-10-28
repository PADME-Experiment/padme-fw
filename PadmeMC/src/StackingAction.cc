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
#include "MCTruthManager.hh"
#include "MyParticleInfo.hh"
#include "G4PrimaryParticle.hh"
#include "G4ParticleTable.hh"
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
  fECalKineticEnergyThresholdEnergyMCTruth = 1*CLHEP::MeV;
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
  G4ThreeVector postrack = track->GetPosition();
  G4double timetrack = track->GetGlobalTime();
  MCTruthManager* mctm = MCTruthManager::GetInstance();

  if (mctm->IsEnabled()){
    G4String processId = "";
    G4ThreeVector parentmomtrack;
    G4double parentenergytrack;
    int ifound = -1;
    int caseid = -1;

    // check if this G4Track is a primary track from bby, bbygg, bhabha, if so store it in a map

    if (track->GetParentID()==0) {
      const G4PrimaryParticle* primary = track->GetDynamicParticle()->GetPrimaryParticle();
      if (primary) {
        // Retrieve your custom information
        auto* info = static_cast<const MyParticleInfo*>(primary->GetUserInformation());
        if (info) {
	  processId = info->GetProcessID();
	  parentmomtrack = info->GetParentMomentum();
	  parentenergytrack = info->GetParentEnergy();
	  caseid = 0;
	}
      }
    } else { // check if the parent of this G4Track had been stored in the map, if so store it
      if (mctm->IsG4TrackInVertex(track->GetParentID(), 0)){
	//	G4cout << "track " << track->GetTrackID() << " has a parent with id = " << track->GetParentID() << " who is in the map  and is in volume " << track->GetVolume()->GetName();
	G4int vtxOfParentID = mctm->GetG4TrackInVertex(track->GetParentID(),0).first;
	G4int posOfParentID = mctm->GetG4TrackInVertex(track->GetParentID(),0).second;
	//	G4cout << " with links " << vtxOfParentID << " and " << posOfParentID << G4endl;
	if (track->GetVolume()->GetName() != "ECalCry" && track->GetVolume()->GetName() != "LeadGlass" && track->GetKineticEnergy() > fECalKineticEnergyThresholdEnergyMCTruth){ // avoid wasting time with daughters generated while showering or with too soft particles
	  mctm->AddG4TrackSecondaryToVertex(track->GetTrackID(),vtxOfParentID,posOfParentID,1);
	}
      }
    }
    
    // check if this vertex already exists in MCTruthManager

    if (caseid != -1) {
      G4int nvtx = mctm->GetNVertices();
      for (int i=0; i<nvtx; i++) {
	MCTruthVertex* vtx = mctm->Vertex(i);
	int nparticlesIn = vtx->GetNParticlesIn();
	if (nparticlesIn != 1) continue;
	G4ThreeVector posvtx = vtx->GetPosition();
	double distance = (posvtx-postrack).mag();
	G4ThreeVector parentmomvtx = vtx->ParticleIn(0)->GetMomentum();
	double momdifference = (parentmomvtx-parentmomtrack).mag();
	if (distance > 1e-6 || momdifference > 1e-6) continue;

	ifound = i;
	break;
      }

      if (ifound != -1) { // if it exists already, just add particlesout to this vertex
	MCTruthVertex* oldvtx = mctm->Vertex(ifound);
	oldvtx->AddParticleOut(track->GetDefinition()->GetPDGEncoding(),track->GetTotalEnergy(),track->GetMomentum());
	// store G4TrackID in the map as connected to the vertex in a given position out of it
	mctm->AddG4TrackSecondaryToVertex(track->GetTrackID(),ifound,oldvtx->GetNParticlesOut()-1,0);
      }
      else { // create a new vertex
	MCTruthVertex* newvtx = mctm->AddVertex(processId,postrack,timetrack);
	newvtx->AddParticleIn(G4ParticleTable::GetParticleTable()->FindParticle("e+")->GetPDGEncoding(),parentenergytrack,parentmomtrack);
	newvtx->AddParticleOut(track->GetDefinition()->GetPDGEncoding(),track->GetTotalEnergy(),track->GetMomentum());
	// store G4TrackID in the map as connected to the new vertex as first secondary 
	mctm->AddG4TrackSecondaryToVertex(track->GetTrackID(),mctm->GetNVertices()-1,0,0);
      }
    }
  }
//tobe uncommented  // check parent
//tobe uncommented  if (track->GetParentID() == 0){
//tobe uncommented  // check in the mctruthmanager vertex and compare position
//tobe uncommented    G4ThreeVector postrack = track->GetPosition();
//tobe uncommented    
//tobe uncommented    if (nvtx) {
//tobe uncommented      MCTruthVertex* vtx = MCTruthManager::GetInstance()->Vertex(nvtx-1);
//tobe uncommented      G4ThreeVector posvtx = vtx->GetPosition();
//tobe uncommented      double distance = (posvtx-postrack).mag();
//tobe uncommented    }
//tobe uncommented  // check pdgid according to process name
//tobe uncommented  // establish position in the outgoing particles of the mctruthvertex
//tobe uncommented  // store the link with G4Track



  
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
