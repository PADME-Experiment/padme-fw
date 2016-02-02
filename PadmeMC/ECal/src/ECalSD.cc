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
//
// $Id: ECalSD.cc,v 1.2 2014/06/23 13:44:16 veni Exp $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "ECalSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Neutron.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4PionZero.hh"
#include "G4VProcess.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ECalSD::ECalSD(G4String name)
:G4VSensitiveDetector(name)
{
  G4String HCname;
  collectionName.insert(HCname="ECalCollection"); //crea il collection name
}

//....Ooooo0ooooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ECalSD::~ECalSD(){ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ECalSD::Initialize(G4HCofThisEvent* HCE)
{
  ECalCollection = new ECalHitsCollection
                          (SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if(HCID<0)
  { HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); }
  HCE->AddHitsCollection(HCID,ECalCollection); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool ECalSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{
  G4double edep  = aStep->GetTotalEnergyDeposit();
  G4ThreeVector posix = aStep->GetPostStepPoint()->GetPosition();
  G4int TrackType=-1;

  if(TrackType==-1){
    G4Track* track = aStep->GetTrack();
    G4int TrackType = ClassifyTrack(track);
  }
  //  printf("Hey I'm processing a Hit %f\n",edep);
  if(edep==0.) return false;
  ECalHit* newHit = new ECalHit();
  //  newHit->SetECalID  (aStep->GetTrack()->GetTrackID());
  newHit -> SetCryNb(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
  newHit -> SetEdep(edep);
  newHit -> SetPos (aStep->GetPostStepPoint()->GetPosition());
  newHit -> SetTime (aStep->GetTrack()->GetGlobalTime());
  newHit -> SetPType(TrackType);
  ECalCollection ->insert( newHit );
  // Totedep=ToTEDep+edep;
  //  printf("EDep %f %f %f %f %f\n",edep,posix[0],posix[1],posix[2],aStep->GetTrack()->GetGlobalTime());
  //  newHit->Print();
  //  newHit->Draw();

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4int ECalSD::ClassifyTrack(G4Track* track){
	G4ParticleDefinition * particleType = track->GetDefinition();
	if(particleType == G4Gamma::GammaDefinition())  {
	//	G4cout<<"G "<<particleType<<" "<<particleType->GetPDGCharge()<<" "<<track->GetTotalEnergy()<<G4endl;
		return 1;
	}else if(particleType == G4Positron::PositronDefinition() )  {
//				G4cout<<"e+"<<particleType<<" "<<particleType->GetPDGCharge()<<" "<<track->GetTotalEnergy()<<G4endl;
		return 2;
	}else if(particleType == G4Electron::ElectronDefinition() )  {
			//	G4cout<<"e-"<<particleType<<" "<<particleType->GetPDGCharge()<<" "<<track->GetTotalEnergy()<<G4endl;
		return 3;
	}else if(particleType == G4Neutron::NeutronDefinition())  {
	  //		NNeutrons_aft++;
		//Nene=track->GetKineticEnergy();
		//G4cout<<"Neutron"<<NNeutrons<<" "<<particleType->GetPDGCharge()<<" "<<Nene<<" "<<i<<G4endl;
		return 4;
	}else if(particleType == G4PionPlus::PionPlusDefinition() || particleType == G4PionMinus::PionMinusDefinition())  {
	  //		Npionc_aft++;
		//	Nene=track->GetKineticEnergy();
		G4cout<<"pionc "<<particleType->GetPDGCharge()<<" "<<track->GetKineticEnergy()<<G4endl;
		return 5;
	}else if(particleType == G4PionZero::PionZeroDefinition())  {
	  //		Npi0_aft++;
		//	Nene=track->GetKineticEnergy();
	  //		G4cout<<"pi0 "<<particleType<<" "<<particleType->GetPDGCharge()<<" "<<track->GetKineticEnergy()<<G4endl;
		return 6;
	}else if(particleType == G4MuonPlus::MuonPlusDefinition() || particleType == G4MuonMinus::MuonMinusDefinition())  {
	  //		Nmuons_aft++;
		//	Nene=track->GetKineticEnergy();
	  //		G4cout<<"mu "<<particleType<<" "<<particleType->GetPDGCharge()<<" "<<track->GetKineticEnergy()<<G4endl;
		return 7;
	}else return -1;
}

void ECalSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel>0) { 
     G4int NbHits = ECalCollection->entries();
     G4cout << "\n-------->Hits Collection: in this event there are " << NbHits 
            << " hits in the ECal : " << G4endl;
     for (G4int i=0;i<NbHits;i++) (*ECalCollection)[i]->Print();
    } 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

