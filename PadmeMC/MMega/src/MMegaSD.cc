// MMegaSD.cc
// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
//
// --------------------------------------------------------------

#include "MMegaSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4UnitsTable.hh"
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

extern double NNeutrons_aft;
extern double Npionc_aft;
extern double Npi0_aft;
extern double Nmuons_aft;

MMegaSD::MMegaSD(G4String name)
:G4VSensitiveDetector(name)
{
  G4String HCname;
  collectionName.insert(HCname="MMegaCollection"); //crea il collection name
}

MMegaSD::~MMegaSD(){}

void MMegaSD::Initialize(G4HCofThisEvent* HCE)
{
  MMegaCollection = new MMegaHitsCollection(SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if (HCID<0) HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  HCE->AddHitsCollection(HCID,MMegaCollection); 
}

G4bool MMegaSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  
  G4double edep = aStep->GetTotalEnergyDeposit();
  // G4cout << "entering MMegaSD with:" << edep << G4endl;
  if (edep == 0.) return false;

  G4Track* track = aStep->GetTrack();
  G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
  G4StepPoint* postStepPoint = aStep->GetPostStepPoint();
  const G4VProcess* currentProcess = postStepPoint->GetProcessDefinedStep();
  if (currentProcess == 0) return false;
  //if ( (currentProcess == 0) || (currentProcess->GetProcessName() != "Transportation") || (track->GetVolume()->GetName() != "MMegaDrift") ) return false;
  if ( (currentProcess == 0) || (track->GetVolume()->GetName() != "MMegaDrift") ) return false;
  G4TouchableHandle touchHPre = aStep->GetPreStepPoint()->GetTouchableHandle();

  MMegaHit* newHit = new MMegaHit();

  newHit->SetPType(ClassifyTrack(aStep->GetTrack()));
  newHit->SetTrackID(track->GetTrackID());
  newHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime());
  newHit->SetEnergy(aStep->GetTotalEnergyDeposit() - aStep->GetNonIonizingEnergyDeposit());
  newHit->SetETrack(track->GetTotalEnergy());

  G4ThreeVector worldPosPre = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector worldPosPost = aStep->GetPostStepPoint()->GetPosition();
  G4ThreeVector localPosPre = touchHPre->GetHistory()->GetTopTransform().TransformPoint(worldPosPre);
  G4ThreeVector localPosPost = touchHPre->GetHistory()->GetTopTransform().TransformPoint(worldPosPost);

  newHit->SetPosition(worldPosPre);
  newHit->SetLocalPositionStart(localPosPre);
  newHit->SetLocalPositionEnd(localPosPost);

  // newHit->Print(); //prints in G4 output the hit informations
  // newHit->Draw();

  MMegaCollection ->insert(newHit);     

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4int MMegaSD::ClassifyTrack(G4Track* track){
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
		NNeutrons_aft++;
		//Nene=track->GetKineticEnergy();
		//G4cout<<"Neutron"<<NNeutrons<<" "<<particleType->GetPDGCharge()<<" "<<Nene<<" "<<i<<G4endl;
		return 4;
	}else if(particleType == G4PionPlus::PionPlusDefinition() || particleType == G4PionMinus::PionMinusDefinition())  {
		Npionc_aft++;
		//	Nene=track->GetKineticEnergy();
		G4cout<<"pionc "<<particleType->GetPDGCharge()<<" "<<track->GetKineticEnergy()<<G4endl;
		return 5;
	}else if(particleType == G4PionZero::PionZeroDefinition())  {
		Npi0_aft++;
		//	Nene=track->GetKineticEnergy();
		G4cout<<"pi0 "<<particleType<<" "<<particleType->GetPDGCharge()<<" "<<track->GetKineticEnergy()<<G4endl;
		return 6;
	}else if(particleType == G4MuonPlus::MuonPlusDefinition() || particleType == G4MuonMinus::MuonMinusDefinition())  {
		Nmuons_aft++;
		//	Nene=track->GetKineticEnergy();
		G4cout<<"mu "<<particleType<<" "<<particleType->GetPDGCharge()<<" "<<track->GetKineticEnergy()<<G4endl;
		return 7;
	}else return -1;
}

void MMegaSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel>0) { 
     G4int NbHits = MMegaCollection->entries();
     G4cout << "\n-------->Hits Collection: in this event they are " << NbHits 
            << " hits in the MMega : " << G4endl;
     for (G4int i=0;i<NbHits;i++) (*MMegaCollection)[i]->Print();
    } 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

