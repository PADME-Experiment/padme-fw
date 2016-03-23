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
#include "Constants.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StackingAction::StackingAction()
{
	NGenN=0;
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
	//keep primary particle
	if(track->GetParentID() == 0) return fUrgent;
	if(IsTurboOn==1){
		G4ParticleDefinition * particleType = track->GetDefinition();
		//Selects and Kill gamma below thr cut
		if(particleType == G4Gamma::GammaDefinition() && track->GetTotalEnergy()<gKillEnergy)  {
			l++;
			//G4cout<<"G "<<particleType<<" "<<particleType->GetPDGCharge()<<" "<<track->GetTotalEnergy()<<" "<<l<<G4endl;
			return fKill;
		}else if(particleType == G4Positron::PositronDefinition() && track->GetTotalEnergy()<epKillEnergy)  {
//			G4cout<<"e+"<<particleType<<" "<<particleType->GetPDGCharge()<<" "<<track->GetTotalEnergy()<<G4endl;
			k++;
			return fKill;
		}else if(particleType == G4Electron::ElectronDefinition() && track->GetTotalEnergy()<emKillEnergy)  {
			//	G4cout<<"e-"<<particleType<<" "<<particleType->GetPDGCharge()<<" "<<track->GetTotalEnergy()<<G4endl;
			j++;
			return fKill;
		}else if(particleType == G4Neutron::NeutronDefinition() )  {
			NNeutrons++;
			Nene=track->GetKineticEnergy();
			//G4cout<<"Neutron"<<NNeutrons<<" "<<particleType->GetPDGCharge()<<" "<<Nene<<" "<<i<<G4endl;
			return fUrgent;
		}else if(particleType == G4PionPlus::PionPlusDefinition() || particleType == G4PionMinus::PionMinusDefinition())  {
			Npionc++;
		//	Nene=track->GetKineticEnergy();
			G4cout<<"pionc "<<Npionc<<" "<<particleType->GetPDGCharge()<<" "<<track->GetKineticEnergy()<<" "<<i<<G4endl;
			return fUrgent;
		}else if(particleType == G4PionZero::PionZeroDefinition())  {
			Npi0++;
		//	Nene=track->GetKineticEnergy();
			G4cout<<"pi0 "<<particleType<<" "<<particleType->GetPDGCharge()<<" "<<track->GetKineticEnergy()<<" "<<i<<G4endl;
			return fUrgent;
		}else if(particleType == G4MuonPlus::MuonPlusDefinition() || particleType == G4MuonMinus::MuonMinusDefinition())  {
			Nmuons++;
		//	Nene=track->GetKineticEnergy();
			G4cout<<"mu "<<particleType<<" "<<particleType->GetPDGCharge()<<" "<<track->GetKineticEnergy()<<" "<<i<<G4endl;
			return fUrgent;
		}else return fUrgent;
	}
	else return fUrgent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
