// LeadGlassSD.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2023-09-21
// --------------------------------------------------------------

#include "LeadGlassSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4VProcess.hh"

LeadGlassSD::LeadGlassSD(G4String name)
:G4VSensitiveDetector(name)
{
  G4String HCname;
  collectionName.insert(HCname="LeadGlassCollection"); //crea il collection name
}

LeadGlassSD::~LeadGlassSD(){}

void LeadGlassSD::Initialize(G4HCofThisEvent* HCE)
{
  LeadGlassCollection = new LeadGlassHitsCollection(SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if (HCID<0) HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  HCE->AddHitsCollection(HCID,LeadGlassCollection); 
}

G4bool LeadGlassSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{
  G4double edep = aStep->GetTotalEnergyDeposit();
  if (edep == 0.) return false;

  G4Track* track = aStep->GetTrack();
  if ( (track->GetVolume()->GetName() != "PbGlBlock") && (track->GetVolume()->GetName() != "LightGuide") ) return false;
  G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
  const G4VProcess* currentProcess = preStepPoint->GetProcessDefinedStep();
  if ( (currentProcess == 0) || (currentProcess->GetProcessName() != "Transportation") ) return false;

  G4TouchableHandle touchHPre = aStep->GetPreStepPoint()->GetTouchableHandle();

  LeadGlassHit* newHit = new LeadGlassHit();

  newHit->SetEnergy(aStep->GetTotalEnergyDeposit());
  newHit->SetTime(track->GetGlobalTime());

  G4ThreeVector worldPosPre = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector localPosPre = touchHPre->GetHistory()->GetTopTransform().TransformPoint(worldPosPre);

  newHit->SetPosition(worldPosPre);
  newHit->SetLocalPosition(localPosPre);

  LeadGlassCollection->insert(newHit);     

  //track->SetTrackStatus(fStopAndKill);

  return true;
}

void LeadGlassSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel>0) {
    G4int NbHits = LeadGlassCollection->entries();
    G4cout << "\n-------->Hits Collection: in this event they are " << NbHits 
	   << " hits in the LeadGlass : " << G4endl;
    for (G4int i=0;i<NbHits;i++) (*LeadGlassCollection)[i]->Print();
  } 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

