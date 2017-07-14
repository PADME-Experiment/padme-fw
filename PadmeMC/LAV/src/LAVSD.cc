// LAVSD.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#include "LAVSD.hh"

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

extern double NNeutrons_aft;
extern double Npionc_aft;
extern double Npi0_aft;
extern double Nmuons_aft;

LAVSD::LAVSD(G4String name)
:G4VSensitiveDetector(name)
{
  G4String HCname;
  collectionName.insert(HCname="LAVCollection"); //crea il collection name
}

LAVSD::~LAVSD(){}

void LAVSD::Initialize(G4HCofThisEvent* HCE)
{
  LAVCollection = new LAVHitsCollection(SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if (HCID<0) HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  HCE->AddHitsCollection(HCID,LAVCollection); 
}

G4bool LAVSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{
  G4double edep = aStep->GetTotalEnergyDeposit();
  if (edep == 0.) return false;

  G4Track* track = aStep->GetTrack();
  G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
  const G4VProcess* currentProcess = preStepPoint->GetProcessDefinedStep();
  if ( (currentProcess == 0) || (currentProcess->GetProcessName() != "Transportation") || (track->GetVolume()->GetName() != "LAV") ) return false;

  G4TouchableHandle touchHPre = aStep->GetPreStepPoint()->GetTouchableHandle();

  LAVHit* newHit = new LAVHit();

  newHit->SetChannelId(touchHPre->GetCopyNumber()); 
  newHit->SetEnergy(aStep->GetTotalEnergyDeposit() );
  


  // G4cout << " LAVSD:   Energy of the track: " << preStepPoint->GetTotalEnergy()
  //   //track->GetTotalEnergy() 
  // 	 << "    Energy deposited: " <<  aStep->GetTotalEnergyDeposit() 
  // 	 << G4endl;

  newHit->SetTime(track->GetGlobalTime());

  newHit->SetETrack(track->GetTotalEnergy());

  G4ThreeVector worldPosPre = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector localPosPre = touchHPre->GetHistory()->GetTopTransform().TransformPoint(worldPosPre);
  //G4cout << "PreStepPoint in " << touchHPre->GetVolume()->GetName()
  //	 << " global " << G4BestUnit(worldPosPre,"Length")
  //	 << " local " << G4BestUnit(localPosPre,"Length") << G4endl;

  //G4ThreeVector worldPosPost = aStep->GetPostStepPoint()->GetPosition();
  //G4TouchableHandle touchHPost = aStep->GetPostStepPoint()->GetTouchableHandle();
  //G4ThreeVector localPosPost = touchHPost->GetHistory()->GetTopTransform().TransformPoint(worldPosPost);
  //G4cout << "PostStepPoint in " << touchHPost->GetVolume()->GetName()
  //	 << " global " << G4BestUnit(worldPosPost,"Length")
  //	 << " local " << G4BestUnit(localPosPost,"Length") << G4endl;

  newHit->SetPosition(worldPosPre);
  newHit->SetLocalPosition(localPosPre);

  newHit->SetTrackID(track->GetTrackID());
  newHit->SetPType(ClassifyTrack(aStep->GetTrack()));
  
  LAVCollection ->insert(newHit);     

  track->SetTrackStatus(fStopAndKill);

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4int LAVSD::ClassifyTrack(G4Track* track){
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

void LAVSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel>0) { 
     G4int NbHits = LAVCollection->entries();
     G4cout << "\n-------->Hits Collection: in this event they are " << NbHits 
            << " hits in the LAV : " << G4endl;
     for (G4int i=0;i<NbHits;i++) (*LAVCollection)[i]->Print();
    } 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

