#include "SACSD.hh"

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

SACSD::SACSD(G4String name)
:G4VSensitiveDetector(name)
{
  G4String HCname;
  collectionName.insert(HCname="SACCollection"); //crea il collection name
}

SACSD::~SACSD(){}

void SACSD::Initialize(G4HCofThisEvent* HCE)
{
  fSACCollection = new SACHitsCollection(SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if(HCID<0) HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  HCE->AddHitsCollection(HCID,fSACCollection); 
}

G4bool SACSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{

  G4double edep = aStep->GetTotalEnergyDeposit();
  if (edep == 0.) return false;
  G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
  G4StepPoint* postStepPoint = aStep->GetPostStepPoint();
  G4TouchableHandle touchHPre = aStep->GetPreStepPoint()->GetTouchableHandle();

  SACHit* newHit = new SACHit();
  
  newHit->SetEdep(preStepPoint->GetTotalEnergy());
  //newHit->SetEdep(aStep->GetTrack()->GetTotalEnergy());
  newHit->SetTrackID(aStep->GetTrack()->GetTrackID());

  //newHit->SetChannelId(touchHPre->GetCopyNumber());
  newHit->SetChannelId(touchHPre->GetCopyNumber(1)); // Copy id is that of the cell, not of the crystal
  newHit->SetEnergy(edep);

  // G4cout << " SACSD:  Pre energy of the track: " << preStepPoint->GetTotalEnergy() 
  // 	 << " Post energy of the track: " << postStepPoint->GetTotalEnergy() 
  // 	 << " Total energy  " << aStep->GetTrack()->GetTotalEnergy() 
  // 	 << "   Energy deposited: " <<  aStep->GetTotalEnergyDeposit() 
  // 	 << G4endl;

  newHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime());

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
  newHit->SetPType(ClassifyTrack(aStep->GetTrack()));
  fSACCollection->insert(newHit);

  return true;

}

G4int SACSD::ClassifyTrack(G4Track* track){

  G4ParticleDefinition* particleType = track->GetDefinition();
  if (particleType == G4Gamma::GammaDefinition()) {
    return 1;
  } else if (particleType == G4Positron::PositronDefinition()) {
    return 2;
  } else if (particleType == G4Electron::ElectronDefinition()) {
    return 3;
  } else if (particleType == G4Neutron::NeutronDefinition()) {
    return 4;
  } else if (particleType == G4PionPlus::PionPlusDefinition() ||
	     particleType == G4PionMinus::PionMinusDefinition()) {
    return 5;
  } else if (particleType == G4PionZero::PionZeroDefinition()) {
    return 6;
  } else if (particleType == G4MuonPlus::MuonPlusDefinition() ||
	     particleType == G4MuonMinus::MuonMinusDefinition()) {
    return 7;
  } else return -1;

}

void SACSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel>0) { 
    G4int NbHits = fSACCollection->entries();
    G4cout << "\n-- SAC Hits Collection: " << NbHits << " hits --" << G4endl;
    for (G4int i=0;i<NbHits;i++) (*fSACCollection)[i]->Print();
  }
}
