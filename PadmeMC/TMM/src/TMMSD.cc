// TMMSD.cc
// --------------------------------------------------------------
// History:
//
// Created by  Elisa Di Meco (elisa.dimeco@lnf.infn.it) 2026-05-27
//
// --------------------------------------------------------------

#include "TMMSD.hh"

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

TMMSD::TMMSD(G4String name)
:G4VSensitiveDetector(name)
{
  G4String HCname;
  collectionName.insert(HCname="TMMCollection"); //crea il collection name
}

TMMSD::~TMMSD(){}

void TMMSD::Initialize(G4HCofThisEvent* HCE)
{
  TMMCollection = new TMMHitsCollection(SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if (HCID<0) HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  HCE->AddHitsCollection(HCID,TMMCollection); 
}

G4bool TMMSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  
  G4double edep = aStep->GetTotalEnergyDeposit();
  // G4cout << "entering TMMSD with:" << edep << G4endl;
  if (edep == 0.) return false;

  G4Track* track = aStep->GetTrack();
  G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
  G4StepPoint* postStepPoint = aStep->GetPostStepPoint();
  const G4VProcess* currentProcess = postStepPoint->GetProcessDefinedStep();
  if (currentProcess == 0) return false;
  //if ( (currentProcess == 0) || (currentProcess->GetProcessName() != "Transportation") || (track->GetVolume()->GetName() != "TMMDrift") ) return false;
  if ( (currentProcess == 0) || (track->GetVolume()->GetName() != "TMMDrift") ) return false;
  G4TouchableHandle touchHPre = aStep->GetPreStepPoint()->GetTouchableHandle();

  TMMHit* newHit = new TMMHit();

  newHit->SetPType(ClassifyTrack(aStep->GetTrack()));
  newHit->SetTrackID(track->GetTrackID());
  newHit->SetParentID(track->GetParentID());
  newHit->SetVertexPosition(track->GetVertexPosition());
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

  TMMCollection ->insert(newHit);     

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4int TMMSD::ClassifyTrack(G4Track* track){
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

void TMMSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel>0) { 
     G4int NbHits = TMMCollection->entries();
     G4cout << "\n-------->Hits Collection: in this event they are " << NbHits 
            << " hits in the TMM : " << G4endl;
     for (G4int i=0;i<NbHits;i++) (*TMMCollection)[i]->Print();
    } 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

