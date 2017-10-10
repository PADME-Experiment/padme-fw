// HEPVetoSD.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#include "HEPVetoSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

HEPVetoSD::HEPVetoSD(G4String name)
:G4VSensitiveDetector(name)
{
  G4String HCname;
  collectionName.insert(HCname="HEPVetoCollection"); //crea il collection name
}

HEPVetoSD::~HEPVetoSD(){}

void HEPVetoSD::Initialize(G4HCofThisEvent* HCE)
{
  fHEPVetoCollection = new HEPVetoHitsCollection(SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if(HCID<0) HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  HCE->AddHitsCollection(HCID,fHEPVetoCollection); 
}

G4bool HEPVetoSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{
  G4double edep = aStep->GetTotalEnergyDeposit();
  if (edep == 0.) return false;

  G4TouchableHandle touchHPre = aStep->GetPreStepPoint()->GetTouchableHandle();

  HEPVetoHit* newHit = new HEPVetoHit();

  newHit->SetChannelId(touchHPre->GetCopyNumber());
  newHit->SetEnergy(edep);
  newHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime());

  G4ThreeVector worldPosPre = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector localPosPre = touchHPre->GetHistory()->GetTopTransform().TransformPoint(worldPosPre);
  //G4cout << "PreStepPoint in " << touchHPre->GetVolume()->GetName()
  //	 << " global " << G4BestUnit(worldPosPre,"Length")
  //	 << " local " << G4BestUnit(localPosPre,"Length") << G4endl;

  //G4TouchableHandle touchHPost = aStep->GetPostStepPoint()->GetTouchableHandle();
  //G4ThreeVector worldPosPost = aStep->GetPostStepPoint()->GetPosition();
  //G4ThreeVector localPosPost = touchHPost->GetHistory()->GetTopTransform().TransformPoint(worldPosPost);
  //G4cout << "PostStepPoint in " << touchHPost->GetVolume()->GetName()
  //	 << " global " << G4BestUnit(worldPosPost,"Length")
  //	 << " local " << G4BestUnit(localPosPost,"Length") << G4endl;

  newHit->SetPosition(worldPosPre);
  newHit->SetLocalPosition(localPosPre);

  newHit -> SetTrackEnergy( aStep->GetPreStepPoint()->GetTotalEnergy());
  newHit->SetTrackId(aStep->GetTrack()->GetTrackID());

  fHEPVetoCollection ->insert( newHit );

  return true;
}

void HEPVetoSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel>0) { 
    G4int NbHits = fHEPVetoCollection->entries();
    G4cout << "\n-- HEPVeto Hits Collection: " << NbHits << " hits --" << G4endl;
    for (G4int i=0;i<NbHits;i++) (*fHEPVetoCollection)[i]->Print();
  } 
}
